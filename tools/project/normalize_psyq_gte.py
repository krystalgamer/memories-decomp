#!/usr/bin/env python3
"""Expand Psy-Q's inline RTPS, NCDS and NCLIP placeholders for the GNU assembler.

inline_c.h is written for DMPSX, so its GTE command macros emit marker words
rather than COP2 encodings. This rewrites the three markers the HMD triangle
driver uses and rejects any other marker, so an unsupported command cannot be
assembled silently as data.
"""

from __future__ import annotations

import re
import sys

PLACEHOLDERS = {
    ".word 0x0000007f": ".word 0x4A180001",  # RTPS
    ".word 0x00000fff": ".word 0x4AE80413",  # NCDS
    ".word 0x0000117f": ".word 0x4B400006",  # NCLIP
}
# Every inline_c.h command marker is a small word whose low six bits are set.
MARKER = re.compile(r"\.word\s+0x0000([0-9A-Fa-f]{4})\b")


def main() -> int:
    source = sys.stdin.read()
    if not any(placeholder in source for placeholder in PLACEHOLDERS):
        print("missing Psy-Q GTE placeholder", file=sys.stderr)
        return 1
    for placeholder, word in PLACEHOLDERS.items():
        source = source.replace(placeholder, word)
    for match in MARKER.finditer(source):
        if int(match.group(1), 16) & 0x3F == 0x3F:
            print(f"unsupported Psy-Q GTE placeholder: {match.group(0)}", file=sys.stderr)
            return 1
    sys.stdout.write(source)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
