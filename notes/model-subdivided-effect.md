# Subdivided model effect at 0x8006AF74

`src/game/model_subdivided_effect.c` reproduces all 844 instructions
(3,376 bytes) of `func_8006AF74` with `gcc_2_8_1_g8_split`. The complete
executable retains SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
There are no new compiler profiles, register bindings, instruction assembly,
or private jump tables.

## Measured record and configuration

The record is 0x1318 bytes on the target's ILP32 ABI. Its initialization
diagnostic prints that same size in decimal and hexadecimal.

| Offset | Extent | Use |
| --- | --- | --- |
| 0x0000 | 4 | Captured configuration pointer |
| 0x0004 | 6 x 8 | Six axial SVECTOR corners; padding halfwords are preserved |
| 0x0034 | 24 x 4 | Eight ordered triples of corner pointers |
| 0x0094 | 384 x 8 | Subdivided corners for 128 triangles |
| 0x0C94 | 6 x 4 | Six RGB triplets; fourth bytes are preserved |
| 0x0CAC | 24 x 4 | Eight ordered triples of colour pointers |
| 0x0D0C | 384 x 4 | Subdivided colour triplets |
| 0x130C | 2 | Two bytes cleared by initialization; semantics remain unnamed |
| 0x130E | 2 | Preserved padding |
| 0x1310 | 4 | Elapsed counter |
| 0x1314 | 4 | Remaining counter |

`D_800915E8` retains its existing 28-byte initialized-data owner. The
consumer's measured view consists of three six-byte colour channels, signed
height and radius minima, signed growth and fade durations, and one trailing
halfword. The retail minima are 300, with growth 10 and fade 90.

The two dimensions start from the signed maximum halfword returned by
`func_80057E20`, independently divided by two with truncation toward zero.
Each clamp tests its lower bound before the upper bound of 700: a configured
minimum above 700 is not silently changed into an upper-bound-first clamp.
The six axial corners form four faces above and four below the equatorial
ring. Both subdivision helpers are called at depth two on every face.
Their real vertex and colour strides are eight and four bytes respectively.

## Rendering and lifetimes

Nonnegative modes initialize; negative modes render. Every invocation keeps
the initial ignored slot-getter call and captures the full timing result.
The renderer captures the configuration pointer before the ordering-table
getter and captures both counters before projection callbacks. It retains
the otherwise unused quad setup call.

The matrix sequence is base publication, four-halfword position copy, base
publication again, translation, rotation, `MulMatrix2`, scaling, and final
publication. `RotTrans` writes three words, so its output fits the twelve
bytes of `MATRIX.t`; this is not a claim that a sixteen-byte VECTOR fits
there. The rotation narrows a wrapped 32-bit shift to its halfword.

Fading takes priority over growth. Fade scales all three vertex colours
independently. During growth, the second subdivided colour is deliberately
used for both the second and third triangle vertices, while all three input
triplets are consumed. Outside either active interval, the three original
colours are copied. Projection rejection uses the signs of depth and flags
and requires a positive clipping result; submitted depth is narrowed to
sixteen bits. The eight original faces are projected again for three outline
lines apiece.

After `PopMatrix`, the elapsed counter is reloaded and advanced. Only mode
-2 decrements the remaining counter. The return becomes 2 when the remaining
counter is **negative**, not when it is zero.

## Source discriminators and evidence

Separating the 128-triangle iteration counter from the construction/outline
counter recovers the configuration register and measured spill order.
Explicit link-cursor increments and next-corner expressions preserve the
modulo-four topology and address construction.

The setup extent and projection result share a nonoverlapping work value.
The assignment `work = height = bounds.max`, followed by the two halvings,
retains the target's copies and scheduling through the independent clamps.
The reverse assignment chain was four bytes too long. These are ordinary C
assignments, not physical-register constraints.

The original post-terminal matching result remains in the attempt ledger.
A separate inline-refinement record retains the final owning-header layout
and explicit unsigned rotation shift. Temporary candidates, compiler traces,
reference-byte receipts, and mismatch records stay under `tmp/`.

## Behavioral witness

`tools/project/tests/test_model_subdivided_effect.py` compiles the actual
effect and actual accepted subdivision helpers separately as freestanding
ILP32 programs at host `-O0` and `-O2`. Its caller oracle uses independent
numeric record offsets and face topology, rather than the implementation's
record type or clamp macro. Each optimization executes:

- 638 cases and 99,033 ordered callbacks;
- 62,968 projections, 19,755 triangle submissions, and 3,714 line submissions;
- 62 callback-mutation scenarios, with complete object/configuration
  snapshots, before/after guards, pointer identities, and bounded outputs.

The corpus crosses signed extent boundaries, lower bounds above 700, zero
and negative durations, fade/growth endpoints, zero retirement, full-word
timing values, depth narrowing, and projection rejection. Four actual-source
mutations (colour stride, early retirement, face wrap, and late configuration
capture) must reject with semantic exit 79 at both optimization levels.

SDK operations are deterministic stubs, not a hardware GTE accuracy test.
They respect the measured output footprints; matrix padding is excluded
from observations, packet setup seeds otherwise irrelevant bytes, and
triplet/SVECTOR padding remains part of object preservation checks.
Callback mutations deliberately exceed ordinary read-only SDK/getter
behavior: they establish the caller's capture/reload order, not a current
game bug in another implementation. Negative clock and extended timing
inputs are similarly boundary probes, not claims about normal gameplay.
