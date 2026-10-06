# Spanish MODEL469 24-quad ring

Helper `+0x3068..+0x34BC` is independently reconstructed matching C:
1,108 bytes, four physical instances, 4,432 instruction bytes, and one unique
routine. An independent second-slot compilation also matches. Screening 6,191
accepted regional C entries found twelve same-sized bodies, none with the same
normalized shape; this is not a French or other regional C port.

## Physical loads and ownership

An exhaustive scan identifies four loads with headers 469/619 and four distinct
complete-image hashes:

| Model | Record | Stages | Sectors | Command |
|---:|---:|---:|---:|---:|
| 129 | 129 | 9/10 | 35804/35814 | 635000 |
| 582 | 532 | 9/10 | 147032/147042 | 635000 |

The entry indexes 64-byte descriptors at `+0x35B8`. Six closed contiguous
functions begin at `+4`, `+0x1560`, `+0x1B98`, `+0x21F4`, `+0x2B30` and
`+0x3068`; raw data begins at `+0x34BC`. The functions at `+0x21F4` and
`+0x2B30` are not entry-reachable. Only the selected helper becomes C, leaving
twenty ASM instances.

## Behavior

The entry stores its context argument only once, to its stack home at `+0xC`,
and reloads it from there for the call at `+0x1390`. That call runs once the
time at `+0x3988` reaches the descriptor word at `+0x24`, while the phase at
`+0x39D8` is below eight.

One ring record at context `+0x1614` holds four 24-element `SVECTOR` corner
arrays and, at `+0x3D4`, 24 sizes; the helper keeps its own pointer to it.
All quads draw through the single gray `POLY_FT4` at `+0x3474`.

Three halfword phases advance per quad. The angle starts from `+0x39AC` and
advances by `512 - i * 128 / 24`. The wave starts from `+0x39B4` and advances
by 1300. The ripple starts at zero and advances by 1700. The quad position uses
the tilt `i * 1024 / 24`: horizontal radius `512 * sin(tilt)` around the
origin, with a vertical ripple of `32 * sin(ripple)`. The scale is
`(size + sin(wave) * (size / 8)) * scale / 1024`, using the signed word at
`+0x39B0`. Nonnegative depth and flag gate 16-bit narrowed submission.

Each size below 4096 grows by `step * 64`, clamping at 4096. When the last quad
reaches the clamp in phase zero, the phase becomes one. In phase four, a
positive scale shrinks by `step * 16`; at zero the phase becomes seven. The
angle and wave bases then advance by 32 and 128. Two `ratan2` results are
computed and discarded, as in the target.

The three gray color components are function-scope locals assigned once, so
each keeps its own register at the packet stores, as in the target.

## Stack frame

The target frame is 312 bytes, with the four halfword phases spilled to stack
slots. The 16 bytes directly after the 8-byte rotation are never accessed.
Following the accepted repository convention for observed stack gaps (see
`variant432_draw.c` and [Exodia helpers](exodia-helpers.md)), an unused 16-byte
`VECTOR` local declared after the rotation retains that gap. No runtime role
is asserted for it.

## Acceptance evidence

All four complete 20KiB images match using `gcc_2_8_1_g0_split`, the
authoritative GCC 2.8.1/MASPSX 2.81 pipeline. No forced registers or inline
assembly are present. The fifteen-row ledger preserves all nine rejected
source experiments, the exact source and second-slot compilations, and all
four terminal full-image results.

Regressions cover 27 layouts, repeated header inclusion, all physical loads
and descriptors, the single stack-home context store, every selected
relocation, all input/final function and data owners, and 39 resident bindings
with loader/context ownership. The private state is only the observed prefix
through `+0x39DC`.

Terminal dependency fingerprints hash the body, private header, shared
model-variant header, and `gpu_packets.h`, in that order.
