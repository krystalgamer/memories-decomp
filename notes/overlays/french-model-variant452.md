# French MODEL452 gradient-line grids

## Scope and ownership

The six independently hashed 20,480-byte images are MODEL1, MODEL360 and
MODEL550, stages 9/10 in both runtime slots. Their headers are 452/602 and their
load addresses are `0x8013B000`/`0x8017B000`. The instance ledger records the
compact archive record, loader command, sector and complete image SHA-256.
The archive SHA-256 is
`0c3f90cf4a3b4776d1188054a6cd5d89c5bc364215dac15c4730ef74edbc8da3`.

| Image range | Bytes | Ownership |
| --- | ---: | --- |
| `0x0000..0x0004` | 4 | Raw module header |
| `0x0004..0x10E0` | 4316 | Entry, generated assembly |
| `0x10E0..0x151C` | 1084 | Entry-called helper, generated assembly |
| `0x151C..0x2010` | 2804 | Entry-called helper, generated assembly |
| `0x2010..0x23E4` | 980 | Matching gradient-line C helper |
| `0x23E4..0x5000` | 11292 | Raw, unclassified suffix |

All 24 active function instances have closed instruction-level control-flow
graphs, including delay slots, without holes or indirect calls. Entry calls
the three helpers at image offsets `0xF58`, `0xF60` and `0xF78`, passing its
context in each call's delay slot. The 34 external direct-call destinations
are resident inventory starts and are declared in the family linker file.

The suffix contains additional code-like material but is outside these entry
CFGs. Preserving it as raw storage is not an SDK, dead-code or other ownership
exclusion. This work does not finish MODEL452 or the French overlay scope.

## Independently measured view

Each of three grid records has stride `0x260`: two `SVECTOR[6][6]` endpoint
arrays at `0` and `0x120`, RGB bytes at `0x240`, and signed count at `0x254`.
The minimum context view has `GsGLINE` at `0x32CC`, translation at `0x3308`,
two delta integers at `0x331C`/`0x3320`, a screen delta at `0x3338`, a vector
delta at `0x333C`, step at `0x3360`, and phase at `0x33A8`.
Its `0x33AC` extent is only the minimum accessed by this helper, not evidence
of allocation capacity or a complete description of the entry's state.
Unknown bytes remain uninterpreted.

The SDK declarations come from the repository's native Psy-Q headers.
Target-compiler layout assertions cover the structures and measured offsets.
The source uses three signed 16-bit loop indices, matching the explicit
sign-extension sequences, and retains four retail `ratan2` calls whose
results are unused.

For each record the helper clamps the scale to zero below zero, keeps the
original color through 2048, and otherwise multiplies it by
`(4096 - scale) / 2048` with the observed signed-division rounding.
It projects 36 endpoint pairs per grid, repeating the pair for
`RotTransPers4`, and sorts a gradient line only when depth and flags are
nonnegative. The second endpoint's RGB is zero.

Counts below 4096 advance by `step << 7`. On crossing the threshold they
wrap if phase is below four, otherwise clamp to 4096. A count that remains
below the threshold clears the local completion flag; an already complete
count does not clear it. After the third record, phase four advances to five
only if the flag is still set. The two branch destinations at helper-relative
`0x2DC` and `0x300` distinguish this behavior from the initial incorrect
outer-`else` reading and have explicit regression assertions.

## Matching evidence

GCC 2.8.1 and MASPSX 2.81 use the existing `gcc_2_8_1_g0_split` profile.
The attempt ledger retains four paired experiments:

| Experiment | Bytes | Frame | Differing words per slot |
| --- | ---: | ---: | ---: |
| Single clamped scale value | 968 | 296 | 205 |
| Separate scale and fade lifetimes | 972 | 296 | 204 |
| Branch-local assignments | 980 | 296 | 2 |
| Correct completion branch placement | 980 | 296 | 0 |

Both slot bodies are compared at their actual link addresses, including
absolute local jumps, rather than with relocation bytes masked. All three
complete images per slot are independently hashed and have identical helper
bodies within that slot. Slot-zero helper SHA-256:
`1855be463b1e995524d5d5c871d67322e6b9b8379a881f4cc74e367fbf805dd4`;
slot-one:
`b3f9c892287204f7f77620addcb12fc8bdd90c59a0830ab0375dc0c1697264cc`.

The shared C body and symbol-only slot wrapper are selected separately in
each image manifest. Full production-image equality and sized linked C
ownership, not isolated text matching alone, are required for terminal
`matched` ledger records.

Clean production builds match the complete French resident and all 323
configured French overlays. Independent map-producing relinks equal the six
production ELFs. Their 36 unique input owners account for 122,880 bytes:
six C functions / 5,880 bytes, eighteen assembly functions / 49,224 bytes,
and twelve raw regions / 67,776 bytes. The selected C objects have the same
text as the frozen exact candidates. All 34 resident callees were also
checked against their sized linked functions, input objects and retail
bodies. The prior 317 registrations are unchanged.
