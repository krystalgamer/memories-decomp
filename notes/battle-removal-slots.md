# Animated-battle field-removal slots

`gDuel_bBattleRemovalSlot0/1` name the two signed bytes formerly
`D_8009B208/209`. Their role follows the animated-battle producer through
the resume consumer's actual field removal, not the older header's
attacker/defender shorthand.

## Producer and consumer

In `DuelScene_UpdateBattle`'s animated-battle handoff, both bytes first
receive `-1`. The existing comparison branches select card-record indices:
the `result == -1` branch stores both work-slot indices, while the other
branches may place either participant in slot 0. Slot 0 therefore is not
always the attacker, and slot 1 is not a generally valid defender field.
The original signed comparison and defense-condition tests remain verbatim;
no gameplay-rule interpretation or repair is added.

`DuelScene_UpdateResume` reconstructs the field and waits for transfer/fade
gates, then sets its shared cursor to two. It decrements before scanning,
so it consumes slot 1 then slot 0, skipping negative sentinels. Each
nonnegative value selects a staged replay card record. The consumer creates
request `0xB` using that object's position and variant, calls
`DuelCard_RemoveFromField` for the live record, and plays sound `0x1F`.
This establishes two queued field-removal record slots. The values are not
Boolean flags, pointers, card IDs or an eight-entry removal list.

The shared cursor has other battle/effect/model uses; it is not renamed as
an exclusive removal-count global. Slot contents are phase-dependent, not
a claim of globally valid pending work at all times.

## Storage and declaration views

The battle unit keeps **two separate tentative `s8` definitions** under
`gcc_2_8_1_g8_split_comm`. Their COMMON sizes, order, GP-relative stores and
load-delay behavior remain unchanged. They are not replaced by an allocated
array or a new structure.

The object-aware ownership check counts COMMON symbols that collide with
tracked linker assignments, not total allocations. The two new canonical
aliases add two reported exclusions; real object symbol sizes and complete
compiler/MASPSX outputs still preserve the original COMMON definitions.

The resume unit retains its measured `s8[8]` view under
`gcc_2_8_1_g8_split`. The isolated header mirrors the existing
`D_8009B208_AS_REPLAY_BYTES` selector. The consumer's actual two-slot bound
does not authorize changing that declaration or asserting a new allocation
extent. Old headers, source grouping and definition order are unchanged.

Canonical root aliases refer to the **original symbolic providers**.
NA uses Splat's automatic absolute `D_8009B208/209` labels; Japan retains its
explicit bindings. PAL slot 1 follows the producer's original CPP identity,
`D_8009C0B9`, not an invented numeric provider or a French literal alias.
The builders link automatic undefined-symbol providers before the tracked
extra linker script. Generated provider files must not be edited.

| Image | Slot 0 | Slot 1 provider |
| --- | --- | --- |
| North America | `0x8009B208` | `D_8009B209` at `0x8009B209` |
| Japan | `0x8009B0F8` | `D_8009B209` at `0x8009B0F9` |
| SLES_039.47 | `0x8009C0B8` | Original automatic/COMMON `D_8009C0B9` |
| Other PAL | `0x8009C4D8` | `D_8009C0B9` at `0x8009C4D9` |

The English slot-1 provider and full regional image remain PR-side
acceptance requirements; no missing retail input is replaced or inferred
from NA bytes. The unrelated French literal
`D_8009C0B9_French = 0x8009C0B9` stays distinct.

## Deliberate scope boundary

Only battle and resume/phase source views change. Hand actions continue to
use their legacy view: NA's byte at base `+2` is an AI target, not another
removal entry. PAL is more distinct still: the PAL symbol files bind the
hand-action `D_8009B208` base at `0x8009C10A` in English Europe and
`0x8009C52A` in other PAL images, and bind `gDuel_bBattleRemovalSlot0`
directly at the removal bases `0x8009C0B8/0x8009C4D8`. The two must not be
collapsed just because a common source once used the same numeric spelling.

Signed sentinels, scan order, request creation, removal, sound and cleanup
behavior remain. All old headers, hand-action sources, overlays/imports,
function registrations and existing tests remain unchanged. No new harness,
C fixture, probe or case is added; cloud/regional/overlay/witness execution
remains PR-side validation.
