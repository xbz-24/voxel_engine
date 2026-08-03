"""Lexical boundary helpers for source-file fragmentation."""

from __future__ import annotations

import re


MAX_FRAGMENT_LINES = 88


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
            mode, raw_terminator = "raw_string", f"){delimiter}\""
            index += raw_match.end()
            continue
        if line[index] == '"':
            mode, index = "string", index + 1
        elif line[index] == "'":
            mode, index = "character", index + 1
        else:
            index += 1
    return mode, raw_terminator


def choose_chunks(lines: list[str]) -> list[list[str]]:
    boundaries: set[int] = set()
    state = ("code", "")
    conditional_depth = 0
    for line_number, line in enumerate(lines, start=1):
        directive = re.match(r"^\s*#\s*(if|ifdef|ifndef|endif)\b", line)
        if state[0] == "code" and directive:
            if directive.group(1) == "endif":
                conditional_depth -= 1
            else:
                conditional_depth += 1
        state = lexical_state_after(line, state)
        if state[0] == "code" and conditional_depth == 0 and not line.rstrip().endswith("\\"):
            boundaries.add(line_number)

    chunks: list[list[str]] = []
    start = 0
    while len(lines) - start > MAX_FRAGMENT_LINES:
        preferred_end = start + MAX_FRAGMENT_LINES
        end = next((candidate for candidate in range(preferred_end, start, -1) if candidate in boundaries), None)
        if end is None:
            raise RuntimeError(f"No safe split boundary within {MAX_FRAGMENT_LINES} lines")
        chunks.append(lines[start:end])
        start = end
    chunks.append(lines[start:])
    return chunks
