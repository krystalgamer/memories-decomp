# Duel presentation controls

These three existing bytes connect placement, field action and the shared
battle/resume/Exodia presentation sequences. They remain independent scalar
views, not a new packed structure.

| Former identity | Canonical name | Preserved view |
| --- | --- | --- |
| `D_8009B1B9` | `gDuel_bPresentationCursor` | `s8` |
| `D_8009B218` | `gDuel_bPlacementNeedsCardMoveSetup` | `s8` |
| `D_8009B21A` | `gDuel_bFieldActionAutoCommit` | `u8` |

## Shared presentation cursor

`DuelScene_UpdateCardPlacement` clears the byte and preincrements it through
work slots 1 through 5 while combining selected cards. Battle uses it both
to select a card/resource pair for guardian-star adjustment and to step
from 2 through 1 and 0 to the signed -1 terminator when presenting paired
outcomes and damage. The related display slots are indexed with `cursor + 2`.

`DuelScene_UpdateResume` independently traverses the two battle-removal
slots in the same reverse order. It does not turn this byte into a fixed
attacker/defender or player-side identity.

The Exodia sequence in `duel_draw_resolution.c` also uses the byte to cycle
effect texture regions with `counter & 3`, then to walk display work slots.
It is therefore a reusable presentation cursor/counter, not a permanently
bounded slot index. Byte wrapping, signed comparisons, the -1 sentinel,
texture cycling, offsets and all existing casts remain.

The byte is one `s8` object, defined once in `duel_field_card_render.c`
and declared once in `duel_presentation_controls.h`. Exodia's texture
cycle reads it as `(u8)` before masking with `& 3`; that unsigned read is
the only place the retail code zero-extends it. The former `u8` default
view and its `D_8009B1B9_AS_SIGNED` selector were removed.

## Placement's final card-move setup decision

Placement initializes its signed byte to zero. Hand-origin presentation or
a single monster can set it to 1, fusion success sets 1, and a failed
combination writes -1. The final monster handoff starts with scene flags
`0x4008`, replacing them with `8` when the byte is nonzero.

The consumer is `DuelScene_UpdateCardMove`: on initialization, `0x4000`
skips directly to `state_four`. Without that bit, it first runs its existing
AI guardian-choice path or human centering/guardian-dialog setup.
This establishes a request for that preliminary card-move setup, not a
Boolean fusion result, card-face flag or summon-success indicator.

The -1 store and zero/nonzero predicate are preserved. The name describes
the final handoff decision; an earlier single-object placement exit enters
phase 8 directly and is not rewritten to consult this byte.

## Field auto-commit mode

The field controller clears the byte on entry and on entry to the manual
selection/menu path. Scripted subcase 3 sets it before entering the shared
attack label. This is a mode/request marker, not proof that validation at
that label succeeds, and not a synonym for all AI control: scripted
defense/no-action paths do not set it.

After creating the target cursor, a nonzero byte selects case 5, which
decodes the existing scripted field target code, rather than the manual
case 6. For the card-use presentation in case 7, it skips the dimming and
confirmation path, waits for the tagged transition, removes the selected
card and continues. The raw zero/nonzero tests, card-validation predicates
and timing remain.

## Native providers and preservation

| Image | Cursor | Placement setup | Field auto-commit |
| --- | --- | --- | --- |
| NA | `8009B1B9` | `8009B218` | `8009B21A` |
| Japan | `8009B0A9` | `8009B108` | `8009B10A` |
| SLES_039.47 | `8009C0C1` | `8009C102` | `8009C0CC` |
| Other PAL | `8009C4E1` | `8009C522` | `8009C4EC` |

The auto-commit and placement-setup bytes are defined once per image under
their canonical names, in the duel storage unit (`duel_field_card_render.c`,
or the European wrapper); Japan and the non-English PAL images bind their
addresses in `symbols.txt` and `link_symbols.ld`. No `D_8009B218` alias
remains, and `duel_presentation_controls.h` holds the only declaration of
the placement-setup byte outside the storage unit.

All old headers, wrapper macros, overlays/imports, function records,
grouped sources and named profiles remain unchanged. The two existing
resource-meter/Exodia controls receive only literal namespace substitutions
and reconstruct their complete originals after reversal. Their 262,144
and 590,848 cases, assertions, mutations, generators and compiler/setup
logic are unchanged. No new harness, fixture, probe or testcase is added,
and cloud/regional/overlay/witness suites are not executed locally.

The nearby action substate and effect-work counter are separate scalars
(`gDuel_wActionSubstate`, `gDuel_sEffectWorkCounter`); see
[duel-working-word-views.md](duel-working-word-views.md).
