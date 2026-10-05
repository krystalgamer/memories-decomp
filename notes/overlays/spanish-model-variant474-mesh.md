# Spanish MODEL474 target-position Gouraud mesh

The closed game-owned renderer at `+0x1DC0..+0x2240` is independently
recovered as matching C in all four physical MODEL474/624 images.
This adds four C instances and 4,608 instruction bytes without replacing
the accepted [sheet renderer](spanish-model-variant474.md).
The existing [physical-instance ledger](spanish-model-variant474-instances.csv)
identifies MODEL121 stages 7/8 and MODEL431 stages 9/10, including their
distinct archive records, commands and load slots.

The first source and independent second-slot wrapper both matched with
the named `gcc_2_8_1_g0_split` profile: GCC 2.8.1 and MASPSX 2.81.
No accepted normalized C shape was found among 5,933 configured regional
C entries, including four same-size comparisons. This is not a
French/regional source port; no register forcing, padding or new flags
were used.

## Ownership and invocation caveat

The entry calls only the helpers at `+0xE38` and `+0x1738`.
No entry-reachable call or literal pointer to this mesh renderer was
observed. Its runtime execution and original caller are **not established**.
The evidence below proves internal parameter use and compatibility with
the entry's initialized storage, not an invented runtime call.

The entry preserves its original context in `s8` and forms the mesh
pointer at `+0x4C` as context `+0x6DC`. Initialization writes nine rows
of seventeen eight-byte vertices, using the `17` bound at `+0x518`,
eight-byte point stride at `+0x520`, `136`-byte row stride at `+0x564`
and nine-row bound at `+0x570`. The following nine four-byte color
records receive RGB stores at `+0x530/+0x54C/+0x550`.

The G4 pointer is formed at `+0x64` as context `+0x1A50` and saved
at stack `+0x98`. That stack slot is not overwritten before the actual
`SetPolyG4` call at `+0x248`; the same packet receives `SetSemiTrans(1)`
at `+0x254`. The helper keeps its own incoming context in `s6` and
the selected packet in `s2` throughout rendering.

| View | Offset or extent |
|------|------------------|
| Nine by seventeen `SVECTOR` mesh | `0x6DC..0xBA4` |
| Nine four-byte colors | `0xBA4..0xBC8` |
| `POLY_G4` | `0x1A50..0x1A74` |
| Signed-short target vector | `0x1B5C` |
| Word direction vector | `0x1B64` |
| Frame / step | `0x1B90` / `0x1B9C` |
| Size / intensity | `0x1BC4` / `0x1BD4` |
| Phase / partial state extent | `0x1C1C` / `0x1C20` |

Twenty-one target-compiled layout constants verify these views and
the four G4 coordinate-pair offsets. Unknown context regions remain
opaque. Both repeated include orders with the accepted sheet header
are compiled to check header isolation.

## Recovered behavior

Two direction-based `ratan2` calls remain, although their results are
unused. Even frames pulse the signed size by `size / 16`. Rotation is
zero, translation uses the signed-short target rather than the word
origin, and all three scale components use the pulsed size.

Before phase five, colors come directly from the mesh's nine rows.
From phase five onward, signed intensity scales each channel by
`intensity / 1024`. Eight row pairs each render sixteen strips. The
first two polygon corners use one row's color and the last two use
the following row's color.

Only **strictly positive depth** permits packet submission; there is no
flag test in this renderer. The depth is converted to an unsigned
halfword and passed to the existing draw-mode packet helper with flag
one. This intentionally differs from renderers accepting zero depth
or checking the projection flag.

At phase three or later, size below 4,096 grows by `step * 256` and
clamps to 4,096. In phase five, positive intensity decreases by
`step * 32`; reaching zero clamps intensity and advances phase to six.

## Exactness and preservation

All four complete 20KiB images match retail. Each image retains the
accepted sheet C and five assembly functions, its four-byte header and
`0x38BC..0x5000` raw tail. No modules are added or removed.

Each new object defines the 1,152-byte function, with nine external
calls to eight distinct addresses and two local jumps. Tests resolve
every relocation back into the original instruction bytes and check
the selected compiled object and linked owner. The added `ratan2`
alias preserves the old binding: 36 binding names still identify the
same 35 resident functions.

The [six-row attempt ledger](spanish-model-variant474-mesh-attempts.csv)
preserves both independent slot compilations and all four complete-image
terminals, including source and transitive-header fingerprints.
Three focused regressions complement the existing physical census,
closed-CFG, accepted-sheet and resident-owner checks.
