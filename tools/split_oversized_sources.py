"""Split oversized C/C++ translation units into textual include fragments."""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE_ROOTS = ("src", "include", "Tests", "examples")
SOURCE_EXTENSIONS = {".c", ".cc", ".cpp", ".cxx", ".h", ".hpp", ".inl"}
MAX_FRAGMENT_LINES = 88
PART_PATTERN = re.compile(r"\.part\d+\.inl$")


def lexical_state_after(line: str, state: tuple[str, str]) -> tuple[str, str]:
    mode, raw_terminator = state
    index = 0
    while index < len(line):
        if mode == "block_comment":
            end = line.find("*/", index)
            if end < 0:
                return mode, raw_terminator
            mode, index = "code", end + 2
            continue
        if mode == "raw_string":
            end = line.find(raw_terminator, index)
            if end < 0:
                return mode, raw_terminator
            mode, raw_terminator, index = "code", "", end + len(raw_terminator)
            continue
        if mode in {"string", "character"}:
            quote = '"' if mode == "string" else "'"
            if line[index] == "\\":
                index += 2
            elif line[index] == quote:
                mode, index = "code", index + 1
            else:
                index += 1
            continue

        if line.startswith("//", index):
            break
        if line.startswith("/*", index):
            mode, index = "block_comment", index + 2
            continue
        raw_match = re.match(r'R"([^ ()\\\t\r\n]{0,16})\(', line[index:])
        if raw_match:
            delimiter = raw_match.group(1)
            mode = "raw_string"
            raw_terminator = f"){delimiter}\""
            index += raw_match.end()
            continue
        if line[index] == '"':
            mode, index = "string", index + 1
        elif line[index] == "'":
            mode, index = "character", index + 1
        else:
            index += 1
    return mode, raw_terminator


def safe_boundaries(lines: list[str]) -> set[int]:
    boundaries: set[int] = set()
    state = ("code", "")
    for line_number, line in enumerate(lines, start=1):
        state = lexical_state_after(line, state)
        if state[0] == "code" and not line.rstrip("\r\n").rstrip().endswith("\\"):
            boundaries.add(line_number)
    return boundaries


def choose_chunks(lines: list[str]) -> list[list[str]]:
    boundaries = safe_boundaries(lines)
    chunks: list[list[str]] = []
    start = 0
    while len(lines) - start > MAX_FRAGMENT_LINES:
        preferred_end = start + MAX_FRAGMENT_LINES
        end = next(
            (candidate for candidate in range(preferred_end, start, -1) if candidate in boundaries),
            None,
        )
        if end is None:
            raise RuntimeError(f"No safe split boundary within {MAX_FRAGMENT_LINES} lines")
        chunks.append(lines[start:end])
        start = end
    chunks.append(lines[start:])
    return chunks


def split_source(path: Path) -> None:
    raw = path.read_bytes()
    has_bom = raw.startswith(b"\xef\xbb\xbf")
    text = raw.decode("utf-8-sig")
    newline = "\r\n" if "\r\n" in text else "\n"
    lines = text.splitlines(keepends=True)
    if len(lines) < 100:
        return

    pragma_lines = [line for line in lines if line.strip() == "#pragma once"]
    if pragma_lines:
        lines = [line for line in lines if line.strip() != "#pragma once"]
    chunks = choose_chunks(lines)

    part_paths = [path.with_name(f"{path.name}.part{index:02}.inl") for index in range(1, len(chunks) + 1)]
    for part_path, chunk in zip(part_paths, chunks, strict=True):
        part_path.write_bytes((b"\xef\xbb\xbf" if has_bom else b"") + "".join(chunk).encode("utf-8"))

    wrapper_lines = []
    if pragma_lines:
        wrapper_lines.extend(("#pragma once", ""))
    wrapper_lines.extend(f'#include "{part_path.name}"' for part_path in part_paths)
    wrapper = newline.join(wrapper_lines) + newline
    path.write_bytes((b"\xef\xbb\xbf" if has_bom else b"") + wrapper.encode("utf-8"))


def main() -> None:
    paths = (
        path
        for root in SOURCE_ROOTS
        for path in (ROOT / root).rglob("*")
        if path.is_file()
        and path.suffix.lower() in SOURCE_EXTENSIONS
        and not PART_PATTERN.search(path.name)
    )
    for path in sorted(paths):
        split_source(path)


if __name__ == "__main__":
    main()
