"""Turn former implementation include fragments into real translation units."""

from __future__ import annotations

import difflib
import re
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
INCLUDE = re.compile(r'^#include "(?P<name>[^"]+\.inl)"\s*$', re.MULTILINE)


def git_text(path: str) -> str:
    return subprocess.run(
        ["git", "show", f"HEAD:{path}"], cwd=ROOT, check=True, capture_output=True
    ).stdout.decode("utf-8-sig")


def expanded_fragment(owner: Path, name: str) -> str:
    relative_path = (owner.parent / name).relative_to(ROOT).as_posix()
    text = git_text(relative_path)
    part_names = [part for part in INCLUDE.findall(text) if ".part" in part]
    return "".join(expanded_fragment(owner, part) for part in part_names) if part_names else text


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


def migrate(owner: Path, head_owner: str, fragment_names: list[str]) -> None:
    fragments = [expanded_fragment(owner, name) for name in fragment_names]
    fragment_by_name = dict(zip(fragment_names, fragments, strict=True))
    expanded_head = INCLUDE.sub(
        lambda match: fragment_by_name.get(match.group("name"), match.group(0)), head_owner
    )
    current = owner.read_text(encoding="utf-8-sig")
    old_lines = expanded_head.splitlines(keepends=True)
    current_lines = current.splitlines(keepends=True)
    matcher = difflib.SequenceMatcher(None, old_lines, current_lines, autojunk=False)

    old_boundaries: list[int] = []
    search_start = 0
    for fragment in fragments:
        start = expanded_head.find(fragment, search_start)
        if start < 0:
            raise RuntimeError(f"Cannot locate {owner}: {fragment_names}")
        old_boundaries.append(expanded_head.count("\n", 0, start))
        search_start = start + len(fragment)
    old_boundaries.append(len(old_lines))
    boundaries = [mapped_position(matcher, boundary) for boundary in old_boundaries]
    prefix = "".join(current_lines[:boundaries[0]]).rstrip() + "\n\n"

    for index, name in enumerate(fragment_names):
        segment = "".join(current_lines[boundaries[index]:boundaries[index + 1]]).lstrip()
        destination = owner.with_name(Path(name).stem + ".cpp")
        destination.write_text(prefix + segment, encoding="utf-8")
    owner.unlink()


def main() -> None:
    tracked_sources = subprocess.run(
        ["git", "grep", "-l", ".inl\"", "HEAD", "--", "src/*.cpp", "src/**/*.cpp"],
        cwd=ROOT,
        check=True,
        capture_output=True,
        text=True,
    ).stdout.splitlines()
    for tracked_source in tracked_sources:
        relative_path = tracked_source.removeprefix("HEAD:")
        owner = ROOT / relative_path
        if not owner.is_file():
            continue
        head_owner = git_text(relative_path)
        fragment_names = [
            name for name in INCLUDE.findall(head_owner) if not re.search(r"\.part\d+\.inl$", name)
        ]
        if fragment_names:
            migrate(owner, head_owner, fragment_names)


if __name__ == "__main__":
    main()
