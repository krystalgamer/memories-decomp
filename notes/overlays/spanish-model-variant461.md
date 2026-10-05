# Spanish MODEL461 sheet-set helper

This independently recovered helper adds new matching C, not a port of an
accepted regional body. MODEL705 (compact record 605) loads header 461 at stage 9
and header 611 at stage 10, in the two model slots. A scan of every physical model
record finds exactly these two occurrences.

| Slot | Stage | Sector | Load address | Selected helper | Bytes |
| --- | --- | --- | --- | --- | --- |
| 0 | 9 | 167180 | `0x8013B000` | `func_8013CF54` | 1,348 |
| 1 | 10 | 167190 | `0x8017B000` | `func_8017CF54` | 1,348 |

Both complete 20,480-byte images reproduce their recorded retail hashes with
`gcc_2_8_1_g0_split` (GCC 2.8.1/MASPSX 2.81). The second source is a symbol-only
wrapper. The attempt ledger records the initial descriptor-reload mismatch,
the twelve-word induction-register mismatch, both exact slot comparisons, and
the final source/header fingerprints.

## Ownership and remaining work

Each image contains four closed, fully visited function CFGs:

| Offset range | Bytes | Selection |
| --- | --- | --- |
| `0x0004..0x1508` | 5,380 | Entry, generated assembly |
| `0x1508..0x1F54` | 2,636 | Helper, generated assembly |
| `0x1F54..0x2498` | 1,348 | Sheet-set helper, matching C |
| `0x2498..0x28D4` | 1,084 | Spiral helper, generated assembly |

The entry calls all three helpers. Delay-slot-aware reaching definitions prove
that the original context assignment at image `+0xC` reaches the sheet call at
`+0x135C`, despite entry initialization reusing the same saved register.
The four-byte header and 10,028-byte suffix starting at `+0x28D4` remain raw
owners. The suffix is **unclassified**, not a claim that every byte is data.
The unresolved spiral stack/allocation experiments are not selected as C.

Regression coverage checks actual input-object and final linked owners for
all eight function instances and four raw ranges, all selected MIPS call
relocations, 34 resident callee bindings, four loader/dispatcher owners, and
eight resident pointer-storage owners. Contexts at `0x80136000/0x80176000`
do not overlap the known slot payloads. The entry-observed minimum extent
`0x33C0` and helper view `0x33B0` do **not** establish allocation capacity.

## Recovered layout and behavior

The private header contains only locally supported partial views. Five
592-byte primary records begin at context `+0`; their active/size fields are
`+0x188/+0x18C`. Ten 156-byte sheet records occupy `+0x2144..+0x275C`.
The helper selects the second of two GT4 packets at `+0x2DCC`, hence
`+0x2E00..+0x2E34`. Five-element VECTOR arrays begin at `+0x2F3C` and `+0x2F8C`.
Frame, unsigned time, unsigned step, timing pointer and phase are respectively
`+0x3358`, `+0x335C`, `+0x3364`, `+0x336C`, and `+0x33AC`.
The stored timing pointer has the required `G32` annotation.
Thirty-one target-compiled constants check these declarations.

The actual stage 9/10 request is 627001, stored at model-record sector 275
offset `0x114`. Its normalized index 1 selects a 64-byte descriptor at
image `+0x29D0 + 1*64 = +0x2A10`: first count 5, paired 1, fade 550..560.
Paired requests use twice the first count; unpaired requests use first count
plus one. These physical images therefore process ten sheets, with five in
each position array and five corresponding primary records.

Each sheet projects four textured Gouraud quads, using inner RGB on the first
three vertices and outer RGB on the fourth. Nonnegative depth and projection
flags permit sorting; priority narrows to 16 bits without a depth multiplier.
Odd frames add signed size/8: first-group axes use factors 10/6/10, whereas the
remaining sheets use uniform bias. The original ReadRotMatrix/RotMatrix
sequence is retained.

During phase 1, first-group sheets grow by `(step >> 1) << 11`, clamp at 4096,
and the last first-group sheet advances phase 2 when it reaches the clamp.
At phase 4 or later they fade using the unsigned descriptor interval and clamp
at zero. Remaining sheets follow their primary active/size fields and their
own shown flag: first appearance uses 16384, subsequent size follows primary
size times 16, and nonpositive size clears shown only before phase 5.
The primary cursor advances only in this remaining-sheet branch.
