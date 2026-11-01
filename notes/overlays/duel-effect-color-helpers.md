# Duel-effect color and fullscreen helpers

## Scoped semantic C views

The colour predicates, transitions and fullscreen helpers now use these
canonical C names. `color_semantic_views.h` declares them plainly, and
every regional `duel_effects_symbols.txt` binds each name at its own
address in place of the former label. No record/hash guard is regenerated
or weakened, and no new native exports, function storage or linker aliases
are introduced.

| Canonical C name | Former label | NA address | JP address | UK / FR / ES address |
|---|---|---|---|---|
| `DuelEffect_IsColorBlack` | `func_8014D378` | `0x80147B70` | `0x8015B3D0` | `0x8014D378` |
| `DuelEffect_HasNonzeroColor` | `func_8014D3AC` | `0x80147BA4` | `0x8015B404` | `0x8014D3AC` |
| `DuelEffect_DarkenColor` | `func_80153F28` | `0x80147CAC` | `0x80161FBC` | `0x80153F28` |
| `DuelEffect_StepColorToTarget` | `func_80153F98` | `0x80147D1C` | `0x8016202C` | `0x80153F98` |
| `DuelEffect_SetFogBaseColor` | `func_8015405C` | `0x80147DE0` | `0x801620F0` | `0x8015405C` |
| `DuelEffect_DrawFadingFullscreenColor` | `func_801556F4` | `0x8014D3D4` | `0x80163784` | `0x801556F4` |
| `DuelEffect_DrawFullscreenGradient` | `func_80155798` | `0x8014D478` | `0x80163828` | `0x80155798` |
| `DuelEffect_DrawFullscreenColor` | `func_80155864` | `0x8014D544` | `0x801638F4` | `0x80155864` |

The address column, not the numeric suffix of a former label,
identifies a function in each image. The eight registry rows therefore use
`overlay/duel_effects/function` and the actual NA inventory addresses, never
unqualified resident `function` records. All forty name cells in the five
configured bank inventories agree with the corresponding source views;
every non-name cell, row order, compiler profile and grouped extent remains.
The configured banks are NA, JP, UK, FR and ES, with seven verified terrain
copies apiece. German/Italian duel banks are not configured, and this pass
does not claim their coverage or add configuration for them.

The predicates inspect only the first three bytes. Actual callers pass
their colour triplets, skip geometry for a zero colour and use black
completion to finish a request. The command/padding byte of a `CVECTOR`
does not participate. The darkening and target-step helpers keep the exact
thresholds described below, including zero/large steps, byte narrowing and
explicit completion branches. They are not replaced with generic
saturation or interpolation.

The packed setter retains `high`, `middle`, `low` argument order and each
explicit byte cast. Its existing header alias writes
`gModel_dwFogBaseColor`; no new global is named and no RGB/BGR parameter
order is inferred from grayscale callers. The frozen Japanese-colliding
`D_8009B300` compatibility macro stays local and unchanged.

The fullscreen routines keep every SDK packet operation, ordering-table
argument, coordinate and submission. Width remains the existing default;
height remains selected by each wrapper rather than being normalized
between NA/JP and PAL. The fading flat draw submits **before** changing
the caller's colour. The gradient's two top vertices are black and its
two bottom vertices use the input colour. The other flat draw leaves
the input colour alone.

All actual bank callers use the same canonical C views without changing
arguments, casts, branches or sequencing. Pre-existing tests, fixtures,
source selectors and negative controls remain byte-for-byte unchanged;
no new C/source-string/native harness is added. Complete compiler/profile/CPP,
native-symbol and compiler/MASPSX comparisons account only for the eight
canonical declarations, eight native compatibility declarations and eight
function identifiers. NA executable/whole-ELF
identity remains mandatory. Regional
bank and archive-copy acceptance stays on pull request validation, not a
local cloud-test run.

The Japanese and UK reusable jobs also run exact matching for their selected
duel-effect banks, not just the resident images. The existing extraction and
build tools accept optional `--module NAME` selection alongside `--region`;
omitting it preserves the complete regional operation. Selection is exact,
retains the original module record and duplicate-copy checks, and rejects
missing or duplicate names before any extraction/build. Metadata-only tests
cover the selector and command dispatch; they add no C/native fixture.
The CI steps select just these two banks rather than building more than
3,500 unrelated configured modules in each region.

## Original matching groups

Two complete groups add six functions / 860 bytes with the existing
`gcc_2_8_1_g0_split` profile:

| Group | Range | Functions | Bytes |
|---|---|---:|---:|
| `color_transition.c` | `0x80153F28..0x80154084` | 3 | 348 |
| `screen_draw.c` | `0x801556F4..0x801558F4` | 3 | 512 |

The first routine decrements each channel only when it exceeds `step + 1`;
otherwise it writes zero. Do not replace this with a conventional saturating
subtraction: the boundary behavior differs, including for a zero step.
The second advances toward three caller-supplied channels, clamps each to
its target, and returns one only when all three targets have been reached.
The third packs three bytes into bits 16, 8 and 0 of the existing resident
word `D_8009B300`, using its declaration in `src/game/sorted_entry.h` and the
Spanish resident binding `0x8009C688`. It does not create overlay storage.

The fullscreen helpers retain the actual PAL `320 x 256` rectangle, SDK
`POLY_F4`/`POLY_G4` layouts, the existing ordering table and unsigned-halfword
priority. One draws a flat polygon before decrementing the caller's color;
one grades from a black top edge to the supplied bottom-edge color; one
draws a flat polygon without changing the color. Only the first has the
post-submission update.

## Expression and allocation experiments

Five functions matched immediately. The target-color routine required both
the halfword threshold expression and explicit completion branches:

| Candidate | Target-color result |
|---|---|
| `color < target + 1 - step`, direct conjunction return | 192 instead of 196 bytes; 34 differing words, including a factored `step - 1` temporary and compact boolean return. |
| `color <= target - step`, direct conjunction return | 180 bytes; 47 differing words and inverted comparison branches. |
| `color < target - step + 1`, direct conjunction return | 192 bytes; 21 differing words, with threshold arithmetic ordered differently. |
| Original threshold with explicit `if (...) return 1; return 0;` | Correct 196-byte size; 23 differing words remain from threshold factoring/allocation. |
| `color < (u16)(target + 1) - step`, explicit completion branches | All six functions match at exact sizes with unchanged compiler settings. |

The halfword cast does not truncate a valid target-plus-one: an unsigned
byte plus one is in `1..256`. It preserves the recovered expression boundary
without register pins, barriers, inline assembly or one-off flags.
Candidate sources, headers and precise comparisons remain under
`tmp/color-probe/`.

A private complete-bank link adds these two groups to the independently
verified 29-function batch and reproduces the exact 90,112-byte Spanish
image: 35 actual C functions / 5,824 bytes, leaving 50 provisional
boundaries as assembly. Production verification additionally covers every
terrain copy and all seven configured module images, with exact C object
and linked-ELF symbol ownership. No other release is claimed to match these
new fullscreen constants without its own complete-image proof.
After integrating the accepted textured-quad pair, the production bank
contains 37 C functions / 6,360 bytes and 48 provisional assembly boundaries,
preserving all 21 accepted entries unchanged.
