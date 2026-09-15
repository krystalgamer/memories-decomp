#!/usr/bin/env python3
"""Expand reviewed Psy-Q inline GTE placeholders for the GNU assembler.

inline_c.h is written for DMPSX, so its GTE command macros emit marker words
rather than COP2 encodings. This rewrites the reviewed RTPS, NCDS, NCCS and
NCLIP markers and rejects every other marker, so an unsupported command
cannot be assembled silently as data.
"""

from __future__ import annotations

import re
import sys

PLACEHOLDERS = {
    ".word 0x0000007f": ".word 0x4A180001",  # RTPS
    ".word 0x00000fff": ".word 0x4AE80413",  # NCDS
    ".word 0x0000107f": ".word 0x4B08041B",  # NCCS
    ".word 0x0000117f": ".word 0x4B400006",  # NCLIP
}
# gte_mvmva() adds its sf, mx, v, cv and lm arguments above the 0x13bf marker
# as bits 18-25, and GCC prints that word in decimal.
MVMVA_PARAMETER_BITS = 0x03FC0000
WORD = re.compile(r"\.word\s+(0[xX][0-9A-Fa-f]+|\d+)\b")


def is_marker(value: int) -> bool:
    """Official command markers set the low six bits and fit in sixteen
    bits once the MVMVA parameter field is removed."""
    return value & 0x3F == 0x3F and value & ~MVMVA_PARAMETER_BITS < 0x10000


def main() -> int:
    source = sys.stdin.read()
    if not any(placeholder in source for placeholder in PLACEHOLDERS):
        print("missing Psy-Q GTE placeholder", file=sys.stderr)
        return 1
    for placeholder, word in PLACEHOLDERS.items():
        source = source.replace(placeholder, word)
    for match in WORD.finditer(source):
        if is_marker(int(match.group(1), 0)):
            print(f"unsupported Psy-Q GTE placeholder: {match.group(0)}", file=sys.stderr)
            return 1
    sys.stdout.write(source)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
