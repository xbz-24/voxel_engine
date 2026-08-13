"""Turn former test include fragments into independent test translation units."""

from __future__ import annotations

import difflib
import re
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
OWNERS = (
    "Tests/EngineSmokeTests.cpp",
    "Tests/NetworkProtocolTests.cpp",
    "Tests/PublicApiCompileTests.cpp",
    "Tests/RenderBackendTests.cpp",
    "Tests/WorldCoordinateTests.cpp",
)
INCLUDE = re.compile(r'^#include "(?P<name>[^"]+\.inl)"\s*$', re.MULTILINE)


def git_text(path: str) -> str:
    return subprocess.run(
        ["git", "show", f"HEAD:{path}"], cwd=ROOT, check=True, capture_output=True
    ).stdout.decode("utf-8-sig")


def expanded_head_fragment(owner: Path, name: str) -> str:
    fragment_path = (owner.parent / name).relative_to(ROOT).as_posix()
    text = git_text(fragment_path)
    part_names = INCLUDE.findall(text)
    if part_names:
        return "".join(expanded_head_fragment(owner, part_name) for part_name in part_names)
    return text


def mapped_position(matcher: difflib.SequenceMatcher, old_position: int) -> int:
    previous_old_end = previous_new_end = 0
    for old_start, new_start, size in matcher.get_matching_blocks():
        if old_start <= old_position <= old_start + size:
            return new_start + old_position - old_start
        if old_position < old_start:
            return new_start if old_position - previous_old_end > old_start - old_position else previous_new_end
        previous_old_end = old_start + size
        previous_new_end = new_start + size
    return previous_new_end


def split_owner(relative_owner: str) -> None:
    owner = ROOT / relative_owner
    head_owner = git_text(relative_owner)
    fragment_names = INCLUDE.findall(head_owner)
    fragments = [expanded_head_fragment(owner, name) for name in fragment_names]
    fragment_by_name = dict(zip(fragment_names, fragments, strict=True))
    expanded_head = INCLUDE.sub(lambda match: fragment_by_name[match.group("name")], head_owner)
    current = owner.read_text(encoding="utf-8-sig")
    old_lines = expanded_head.splitlines(keepends=True)
    current_lines = current.splitlines(keepends=True)
    matcher = difflib.SequenceMatcher(None, old_lines, current_lines, autojunk=False)

    old_boundaries: list[int] = []
    search_start = 0
    for fragment in fragments:
        fragment_start = expanded_head.find(fragment, search_start)
        if fragment_start < 0:
            raise RuntimeError(f"Cannot locate baseline fragment in {relative_owner}")
        old_boundaries.append(expanded_head.count("\n", 0, fragment_start))
        search_start = fragment_start + len(fragment)
    old_boundaries.append(len(old_lines))
    boundaries = [mapped_position(matcher, boundary) for boundary in old_boundaries]
    prefix = "".join(current_lines[:boundaries[0]])
    prefix = prefix.replace("#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN\n", "")

    segments = [
        "".join(current_lines[boundaries[index]:boundaries[index + 1]])
        for index in range(len(fragments))
    ]
    if relative_owner.endswith("EngineSmokeTests.cpp"):
        segments[-1] = segments[0] + "\n" + segments[-1]
        fragment_names = fragment_names[1:]
        segments = segments[1:]

    for name, segment in zip(fragment_names, segments, strict=True):
        destination = owner.with_name(Path(name).stem + ".cpp")
        destination.write_text(prefix.rstrip() + "\n\n" + segment.lstrip(), encoding="utf-8")

    owner.write_text("#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN\n#include <doctest/doctest.h>\n", encoding="utf-8")


def main() -> None:
    for owner in OWNERS:
        split_owner(owner)


if __name__ == "__main__":
    main()
