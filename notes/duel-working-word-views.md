# Duel action and effect working-word views

The matching hand/field action handlers and card-effect handlers establish
two independent working-word roles:

| NA address | Semantic scalar | Role |
|---|---|---|
| `0x8009B20C` | `u16 gDuel_wActionSubstate` | Packed action substate and entry/phase flags |
| `0x8009B20E` | `s16 gDuel_sEffectWorkCounter` | Mixed sweep/index/copy-offset/countdown work |

## Action word

`DuelScene_UpdateHandActions` uses the original scalar
`*(u16 *)D_8009B20C` form. Its source explains why replacing that with
array element zero changes scheduling. `DuelScene_UpdateFieldActions`
uses element zero with its original signed-array and explicit unsigned
views, dispatches low-nibble substates and retains the `0x8000`/`0x4000`
entry/phase flags.

The canonical scalar declaration describes the packed word. The matching
implementations retain their distinct local code-generation aliases rather
than normalizing the expressions or changing their reset/state behavior.

## Effect work

The second halfword supports several card effects. Some advance a signed
sentinel through field slots or use the step for positioning/copy offsets;
others count down from `0x10` or `32`. It is not solely a field cursor,
a monotonic animation counter, or a proven frame/time unit.

The raw `s16` storage, explicit `u16` loads/casts, increments/decrements,
sentinels and resets remain unchanged. Local lvalue aliases expand to the
same `D_8009B20C[1]` expression, including address-taking and unsigned views.

## PAL breaks the apparent adjacency

The actual action and effect providers are:

| Image | Action substate | Effect work |
|---|---|---|
| North America | `0x8009B20C` | `0x8009B20E` |
| Japan | `0x8009B0FC` | `0x8009B0FE` |
| United Kingdom | `0x8009C10E` | `0x8009C0C4` |
| France | `0x8009C52E` | `0x8009C4E4` |
| Germany | `0x8009C52E` | `0x8009C4E4` |
| Italy | `0x8009C52E` | `0x8009C4E4` |
| Spain | `0x8009C52E` | `0x8009C4E4` |

The PAL hand adapter selects `gEuropean_D_8009B20C`; the field adapter
keeps its existing `D_8009C10E` identity, which the localized scripts bind
to the same action address. Effect handlers retain the different
`D_8009B20C` transport base and its `+2` relocation.

The public semantic exports therefore use the action provider for the
action word and the effect provider plus two for the counter.
They are not both derived from the PAL action base.

## Preserve the transport and the witnesses

The legacy `s16 D_8009B20C[2]` declaration remains exactly four bytes.
Widening it, replacing it with a new struct, or splitting its existing
matching accesses would disturb the measured small-data/GP behavior.
Adjacency in NA/Japan is not evidence of a shared semantic object.

The owning header declares the two scalar exports. Context-local aliases
in the matching sources deliberately preserve the actual old expressions
and per-translation-unit regional remaps; broad header macros could bind
the wrong PAL object.

Only the existing guarded Cursebreaker fixture's scalar alias changes to
name its same guarded `state_words[1]`. Every other byte of that fixture,
its oracles, inputs, guards, mutation controls and flags remains unchanged.
The ritual fixture continues to use the actual source include prefix and
its unchanged backing words. No new harness, synthetic C, fixture or
mutation is introduced.

All native providers, data extents, wrappers, source/profile groups,
instruction bytes and executable layout remain unchanged. The final ELF
retains all old symbol metadata and gains only the two semantic aliases.
