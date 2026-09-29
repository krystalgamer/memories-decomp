# Spanish duel-bank effect 19

`func_80153ADC` (`80153ADC..80153F28`, 1,100 bytes) is complete matching C
under `gcc_2_8_1_g0_split`. The dispatcher's case 19 establishes the numeric
effect identity; no card or visual-effect name is inferred.

## State and lifecycle

The effect-specific work record is independently recovered from its loads,
stores and helper arguments. A target-GCC probe verifies its `0x718` size:

| Offset | Member |
| --- | --- |
| `000` | 64 SDK rotation vectors |
| `200` | Three rings of 32 SDK vectors |
| `500` | 32 particle positions |
| `600` | 32 particle velocities |
| `700` | Three unsigned width halfwords |
| `708` | Unsigned scale word |
| `70C` | Frame word |
| `710` | Fade-state halfword |
| `712` | Spawn-count halfword |
| `714` | SDK color record |

Nonnegative phase initializes widths, randomized rotations, three rings,
particle positions/velocities and the color. Negative phase sets the frame
step, saves geometry state and draws the expanding rings and rotating
elements. The spawn count is capped at 64. Color interpolation starts the
fade; scale grows by `0x100` and is capped at `0x5000`.

The color predicate returns are explicitly narrowed to 16 bits, as in the
target. The fade-state recheck after the rotation loop is retained. So is
the seemingly unused local color copy and RGB division: the actual draw
still receives the original work color. Particle and rotation updates leave
SDK vector padding untouched. Frame advancement and matrix restoration
occur even when the first color predicate is zero.

The active resident request is the existing 32-byte `DuelEffectRequest`,
declared centrally through `D_8009B264_VISIBLE` and `src/unmatched.h`.
The fade writes its existing `field_1D`; completion writes the centrally
declared `D_8009B261`. No guessed duplicate resident record or global owner
is introduced. Model-step calls and SDK memory/matrix calls reuse canonical
Spanish resident bindings and retain their existing classifications.

The initial scale is copied from the real 16-byte `D_801461C8` object in
generated `header.data.o`. Its explicit extent does not make it a C-owned
definition or an absolute alias. Both the input storage section and final
symbol/retail bytes are checked independently.

## Exact-match evidence

The first complete candidate compiled to 1,100 bytes with zero differing
instruction words. An independent whole-bank link then reproduced all
90,112 bytes, SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Private source/profile snapshots and receipts remain under
`tmp/effect-update-probe/` and `tmp/effect-update-full/`.

Production Spanish overlay matching, all seven retail bank copies and
metadata checks pass. The compiled object and final ELF have the exact
function address, size and executable-section ownership. All 16 distinct
overlay callees retain real code definitions. The target-GCC layout probe
also verifies the canonical resident request size.

The initial independent integration against `650df1e17` preserves every
prior 54 Spanish entry, reaching 55/85 bank C functions / 13,984 bytes.
After integrating accepted #6555 (`1aafeffd6`), all 56 prior entries are
preserved: **57/85 bank C functions / 15,420 bytes**, 28 explicit assembly
boundaries, and **181/209 configured-overlay instances / 71,272 bytes**.
The dispatcher also reuses the canonical request record without changing
its instruction bytes. That effect-and-dispatch follow-up passes 68 targeted tests.
The regional-sharing test accepts either exact French or Spanish manifest
entry, rather than letting French lookup order incorrectly reject the
still-shared Spanish standalone circle/random translation units.
The combined unpublished batch also adds the independently verified
384-byte [offset-height polygon](duel-effect-polygon.md), reaching
**58/85 C / 15,804 bytes**, 27 assembly boundaries, and **182/209 configured
overlay instances / 71,656 bytes**. Together the two new complete functions
add 1,484 bytes; all 56 previously accepted manifest entries remain intact.
The final combined batch passes 69 targeted tests and full Spanish, French,
English PAL, Japanese and North American overlay-image matching. The missing
local North American `SU.MRG` input was restored from the already provided
retail input before rerunning that region; no retail data is tracked.
Only this complete function is newly promoted. Other unmatched routines,
the boot module and the wider dynamic-load census remain open.

The existing promotion helper is hard-coded to North American inventories;
regional metadata is therefore integrated with the same guarded
whole-image-first procedure used by the preceding Spanish overlay work.
