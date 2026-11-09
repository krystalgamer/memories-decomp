# Duel action and effect working-word views

The matching hand/field action handlers and card-effect handlers establish
two independent working-word roles:

| NA address | Semantic scalar | Role |
|---|---|---|
| `0x8009B20C` | `u16 gDuel_wActionSubstate` | Packed action substate and entry/phase flags |
| `0x8009B20E` | `s16 gDuel_sEffectWorkCounter` | Mixed sweep/index/copy-offset/countdown work |

Each is one C-defined scalar under that name in every image:
`duel_scene.c` defines both, and each build's generated
`duel_scene_commons` subsegment places them at that image's addresses
(two adjacent halfwords in the North American and Japanese images). They were formerly one `s16 D_8009B20C[2]` array plus
linker aliases (and PAL `gEuropean_D_8009B20C`/`D_8009C10E` names);
replacing the array with the two scalars changes only relocation names.

## Action word

`DuelScene_UpdateHandActions` and `DuelScene_UpdateFieldActions` read and
write `gDuel_wActionSubstate` directly, dispatching low-nibble substates
and the `0x8000`/`0x4000` entry/phase flags. A scalar matters for the hand
path: sched lets a struct-member load through a pointer pass a store to a
fixed scalar, which an array-element spelling would hold back.

## Effect work

The second halfword supports several card effects. Some advance a signed
sentinel through field slots or use the step for positioning/copy offsets;
others count down from `0x10` or `32`. It is not solely a field cursor,
a monotonic animation counter, or a proven frame/time unit. The explicit
`*(u16 *)&gDuel_sEffectWorkCounter` loads before increments/decrements
are retail's `lhu`.

## PAL breaks the apparent adjacency

| Image | Action substate | Effect work |
|---|---|---|
| North America | `0x8009B20C` | `0x8009B20E` |
| Japan | `0x8009B0FC` | `0x8009B0FE` |
| United Kingdom | `0x8009C10E` | `0x8009C0C4` |
| France | `0x8009C52E` | `0x8009C4E4` |
| Germany | `0x8009C52E` | `0x8009C4E4` |
| Italy | `0x8009C52E` | `0x8009C4E4` |
| Spain | `0x8009C52E` | `0x8009C4E4` |

In PAL the halfword before the counter is `gDuel_wAiHandStackPending`, not
the action substate, so adjacency in NA/Japan is not evidence of a shared
semantic object.
