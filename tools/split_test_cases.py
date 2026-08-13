"""Give each test case an independently compiled, descriptively named source file."""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TEST_CASE = re.compile(r'^TEST_CASE\("(?P<title>[^"]+)"\)', re.MULTILINE)


def file_stem(title: str) -> str:
    words = re.findall(r"[A-Za-z0-9]+", title)
    return "".join(word[:1].upper() + word[1:] for word in words)[:72]


def split_file(path: Path) -> None:
    text = path.read_text(encoding="utf-8-sig")
    matches = list(TEST_CASE.finditer(text))
    if len(text.splitlines()) < 100 or len(matches) < 2:
        return
    prefix = text[:matches[0].start()].rstrip() + "\n\n"
    used_names: set[str] = set()
    for index, match in enumerate(matches):
        end = matches[index + 1].start() if index + 1 < len(matches) else len(text)
        stem = file_stem(match.group("title"))
        candidate = f"{path.stem}_{stem}Tests.cpp"
        suffix = 2
        while candidate.lower() in used_names:
            candidate = f"{path.stem}_{stem}{suffix}Tests.cpp"
            suffix += 1
        used_names.add(candidate.lower())
        path.with_name(candidate).write_text(prefix + text[match.start():end].lstrip(), encoding="utf-8")
    path.unlink()


def main() -> None:
    for path in sorted((ROOT / "Tests").glob("*.cpp")):
        split_file(path)


if __name__ == "__main__":
    main()
