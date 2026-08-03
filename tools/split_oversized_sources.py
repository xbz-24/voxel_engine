"""Split oversized C/C++ translation units into textual include fragments."""

from __future__ import annotations

import re
from pathlib import Path

from split_source_fragments import choose_chunks


ROOT = Path(__file__).resolve().parents[1]
SOURCE_ROOTS = ("src", "include", "Tests", "examples")
SOURCE_EXTENSIONS = {".c", ".cc", ".cpp", ".cxx", ".h", ".hpp", ".inl"}
PART_PATTERN = re.compile(r"\.part\d+\.inl$")


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
    part_paths = [
        path.with_name(f"{path.name}.part{index:02}.inl")
        for index in range(1, len(chunks) + 1)
    ]
    byte_order_mark = b"\xef\xbb\xbf" if has_bom else b""
    for part_path, chunk in zip(part_paths, chunks, strict=True):
        part_path.write_bytes(byte_order_mark + "".join(chunk).encode("utf-8"))

    wrapper_lines = []
    if pragma_lines:
        wrapper_lines.extend(("#pragma once", ""))
    wrapper_lines.extend(f'#include "{part_path.name}"' for part_path in part_paths)
    wrapper = newline.join(wrapper_lines) + newline
    path.write_bytes(byte_order_mark + wrapper.encode("utf-8"))


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
