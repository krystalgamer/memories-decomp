# French MODEL images 12048/12058 travelling ribbons

`func_8013D22C` and its slot-1 counterpart `func_8017D22C` occupy
image offset `0x222C..0x2C98`, 2,668 bytes each. The slot-0 entry saves
the original context at entry+`0x8`, reloads it at `0x8013BDDC`, and
calls the helper at `0x8013BDE0` when phase is at least 5.

## Independently recovered layout and behavior

The helper uses three `0x1DC`-byte records at context+`0xA78`.
Each contains nine spine points, packed screens, screen angles, sideways
edge points/screens, projected widths, per-segment colors and depths,
unsigned halfword screen offsets, travel bounds, two bearings and two wave
phases. Unaccessed bytes remain opaque. This is a local helper view, not
a replacement for the existing sheet view or a claim about storage capacity.

The first spine point lies on a sphere of radius 384; the record stores
its negated coordinates as the travel direction. Later points add
`direction * j / 4` and a signed16 pulse from two cosine contributions.
The first pulse argument is the stored wave phase; subsequent arguments
are `j * 768`, while the other phase advances by 1700 per point. The
sideways projection copy is 24 units along the view.

The native frame is 400 bytes, including a real `flags[3][9]` array.
Identity rotation and unit scale use the signed16 translation at
context+`0x11D4`. Every spine/edge point is projected; the final spine point
reuses the preceding pair. Screen normals and projected widths determine
the halfword offsets.

Alternating `POLY_FT4` packets at context+`0x10B4` draw the selected
segment interval. Both ends taper to their unoffset spine point.
Submission requires nonnegative depth and projection flags, with low16
depth as the sorting priority. The interval grows and then advances by
two segments; completion resets it, changes both bearings and, from phase
6, counts cycles. The third ribbon's fourth cycle advances phase to 7.
Both stored wave phases advance after every ribbon update.

## Exact source and image evidence

The [attempt ledger](french-model-image12048-ribbons-attempts.csv) records
four real source experiments and both final production images.
Separately capturing the first cosine contribution preserves its native
evaluation order. A genuine halfword edge-length local retains the native
multiplications; packed-coordinate Y extraction retains signed loads.
The final one-word difference was the operand order of the point-address
`ADDU` at helper+`0x18C`; index-plus-base lvalues recover it without
extra accesses, fabricated arithmetic or register constraints.

The unchanged named `gcc_2_8_1_g0_split` profile uses GCC2.8.1 and
MASPSX2.81. Both complete 20,480-byte images were rebuilt before and after
promotion, and both linked C owners retain their original sizes:

| Image | SHA-256 |
|---|---|
| French MODEL12048, slot 0 | `13ccda7be07b326dcd1e47bddd785728af0a2368e37f6605c36254d5179d5345` |
| French MODEL12058, slot 1 | `d1a0fda70645085fa257b3b86d69575ad7b86b95ff0329da9354c2b4ebf35dfb` |

This adds 5,336 matching instruction bytes. The existing
[sheet C](french-model-image12048-sheets.md), its profiles and inventory
rows are unchanged. The loader entry, other helper and raw tail remain
their original assembly/binary owners. No other regional registration is
changed.
