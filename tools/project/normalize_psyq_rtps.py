#!/usr/bin/env python3
"""Expand Psy-Q's inline RTPS placeholder for the GNU assembler."""

from __future__ import annotations

import sys

PLACEHOLDER = ".word 0x0000007f"
OPCODE = ".word 0x4A180001"


def main() -> int:
    source = sys.stdin.read()
    if PLACEHOLDER not in source:
        print(f"missing Psy-Q RTPS placeholder: {PLACEHOLDER}", file=sys.stderr)
        return 1
    sys.stdout.write(source.replace(PLACEHOLDER, OPCODE))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
