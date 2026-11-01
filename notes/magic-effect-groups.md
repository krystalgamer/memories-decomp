# Magic-effect group dispatch

The card-use runtime does not interpret card text.
`DuelEffect_StartCardEffect` maps the selected card ID into the 104-entry
effect-ID range, and `DuelEffect_UpdateCardEffect` uses
`gDuelEffect_abGroupByEffectId` to select one of fifteen two-handler slots in
`gDuelEffect_apfnGroupHandler`. Group 14 is allocated but unused by every
effect ID in the table. The complete producer/dispatcher state is documented
in [`card-effect-dispatch.md`](card-effect-dispatch.md).

The card membership already documented in
[`research/the-game.md`](research/the-game.md) and the handlers' complete
field, life-point, ritual, and presentation behavior establish these names:

| Group | Cards | Application handler |
| ---: | --- | --- |
| 0 | equips, traps, and effect IDs without play-time work | `DuelEffect_ClearCardEffect` |
| 1 | six terrain cards | `DuelEffect_ApplyTerrain` |
| 2 | five LP recovery cards | `DuelEffect_ApplyLifePointRecovery` |
| 3 | five direct-damage cards | `DuelEffect_ApplyDirectDamage` |
| 4 | Dark Hole; Dragon Capture Jar | `DuelEffect_ApplyBoardDestruction` |
| 5 | seven type/threshold monster-removal cards | `DuelEffect_ApplyMonsterRemoval` |
| 6 | Stop Defense | `DuelEffect_ApplyStopDefense` |
| 7 | Raigeki | `DuelEffect_ApplyRaigeki` |
| 8 | Dark-piercing Light | `DuelEffect_ApplyDarkPiercingLight` |
| 9 | Spellbinding Circle; Shadow Spell | `DuelEffect_ApplyStatPenalty` |
| 10 | Swords of Revealing Light | `DuelEffect_ApplySwords` |
| 11 | Cursebreaker | `DuelEffect_ApplyCursebreaker` |
| 12 | all 24 rituals | `DuelEffect_StartRitual`; `DuelEffect_ApplyRitual` |
| 13 | Harpie's Feather Duster | `DuelEffect_ApplyHarpiesFeatherDuster` |

## Terrain request view

Terrain's created `0xA` pool entry uses the actual `DuelEffectRequest`
local and the existing typed scene handle. The byte allocator API remains
explicit. The terrain value is still published through its forced-`.data`
byte declaration and read back before subtracting one and calling the
allocator; that sequence preserves both truncation and measured scheduling.
Request publication, the signed `field_1A` payload and sound keep their
original order. Canonical progress/command/active members replace only the
now-unneeded private prefix casts. Transfer flags/waits, command `-2`,
display variant update and the final card-terrain pass are unchanged.

## Stat-penalty request view

The stat-penalty sweep's `0xD` allocation uses its actual request type, not
the older payload-prefix type. Its three unsigned source halfwords are
explicitly converted to the signed request payload fields with identical
low bits. The word at `+0x14` still performs the original signed integer
address/tag calculation: convert `buffer` to `s32`, add the signed step
shifted by 14, and convert the result back. This is not pointer arithmetic
or scaling by a pointee size, and the resulting address is not dereferenced
by this handler.

The producer is `DuelEffect_AllocateRequest` in
`src/game/duel_effect_object_pool.c`: it returns the real pool record with
`buffer = gRuntime_pSharedWorkBase + 0x3800`. `DuelEffect_UpdateRequests` forwards this
word as `(s32)rec->buffer` to the effect dispatcher. The conversion does
not change that ABI or infer how the downstream effect interprets it.

The two 500/1000 penalty arms, signed modifier narrowing, command, delay,
slot/occupied checks, sound and final step update remain unchanged. A
raw-lvalue conversion changed store scheduling and added two delay nops;
using value conversions with the existing struct stores retains exact
GCC 2.8.1 output. The older shared prefix and all other consumers remain
untouched.

`test_stat_penalty_request.py` executes the extracted occupied-card setup
fragment with actual project record types and allocator/sound stubs, not
the entire sweep, allocator, dispatcher or rendering engine. At `-O0`
and `-O2`, each run checks 655,360 cases: every halfword bit pattern in
each payload/modifier, both penalty arms and all five valid sweep steps,
cycling four buffer words (including the real shared buffer address).
Independent byte oracles check the complete request, card record, display
object and guards. Negative controls reject a scaled word step, a byte-only
payload store and an incorrect penalty. Signed negative/overflowing shifts
outside the real sweep domain are not part of that witness.

## Cursebreaker request view

`DuelEffect_ApplyCursebreaker` allocates request `8`, including for empty
slots. Its local now uses the actual `DuelEffectRequest` owner, with
explicit signed value conversions preserving all bits of the unsigned
position loads. The Y payload stays zero. Its buffer update remains the
original signed integer word addition of `step * 0x3000`, followed by
conversion back to the four-byte member; it is not pointer arithmetic.
The allocator/coordinator contract described above applies unchanged.

The folded-side flat grid walk, single-pass loop, biased position index,
unsigned loads, timer and step narrowing remain intact. Allocation and
sound still occur before checking occupancy. The request's command is
always `3` at sound; only an occupied card with a negative stat modifier
subsequently gets a zero modifier and command `5`. Empty, zero-modifier
and positive-modifier cases do not enter that reset. Swords' sibling body
and both matching owners/profiles are untouched. The supported compiler
assembly is byte-for-byte identical without changing any flags.
The shared prefix remains for trap resolution and the Exodia-result
accumulator in `duel_draw_resolution.c`; neither consumer is globally converted.

`test_cursebreaker_request.py` executes the **entire extracted handler**
and actual `DuelEffect_MarkInitialized` helper using project types and
guarded fixture storage. Only allocation and sound are stubbed; this is
not real pool, engine, rendering, overlay or frame-loop execution. Each
`-O0`/`-O2` run checks 983,160 independent calls: every halfword payload
and modifier pattern across both sides and five valid steps, every timer
halfword pattern with valid prior steps, explicit occupancy/sign/zero and
completion boundaries, and first/repeated initialization with all flag
patterns apart from the deliberately selected initialization bit. Four
buffer words are cycled, including the real allocator address. Complete
state/array/guard byte oracles and stub event checks reject scaled word
steps, positive-modifier resets and moving sound after the reset.
These are individual callback invocations with controlled state, not a
claim to execute the game's continuous asynchronous sweep.

## Shared reveal callback

`DuelEffect_ApplyDarkPiercingLight` and `DuelEffect_ApplySwords` both assign
`DuelEffect_UpdateRevealCard` to occupied field-card display objects. The
callback clears the temporary display-marker state, reapplies the card-object
flags, and removes itself. This is presentation work shared by the two reveal
effects, not another dispatch-table group.

The Dark-piercing Light apply handler keeps these two ownership roles
separate. Its created `0x13` pool entry is a `DuelEffectRequest`, with the
unchanged `0xA0/0x68` payload in `field_00/field_02` and unsigned progress
in `field_1D`. The X store still precedes publishing the scene handle, the
Y store follows it, and sound follows both. Its `target` cursor continues
to view the real card display objects and install the reveal callback and
active flag. The larger shared view is not globally narrowed, and the
non-struct occupied-card mask, scan bounds and completion cleanup remain
unchanged.

## Evidence boundaries

The group names come from exclusive card membership where a group contains one
card, and from the complete card-ID branch or data table where a group contains
several cards. `DuelEffect_ApplyRitual` remains unmatched assembly, but its
identity is constrained by the group-12 slot and its calls that request the
three `Duel_CheckRitual` output records, remove those tributes, and stage the
result monster. The address-based source and target filenames for integrated
candidates remain unchanged.

The Swords countdown and lifetime are documented separately in
[`swords-of-revealing-light.md`](swords-of-revealing-light.md).
