# Model draw state and shadow fan

## Dimension queries and requested draw mode

`Model_GetSlotDimensionsWithMinimums` is the former `func_80057E20`.
The renderer derives padded, smoothed dimensions from eligible unit origins,
not an exact mesh AABB. The query copies all four cached halfwords, floors
each axis against its nonzero metadata byte times sixteen, then replaces
the copied fourth metric with the largest nonnegative output axis.

`Model_GetSlotDimensionsWithOverrides` is the former `func_80059000`.
It copies the same cached words but replaces an axis whenever its metadata
byte is nonzero, even when the replacement is smaller. It then recomputes
the maximum. This is not the minimum-floor policy; the two implementations,
signed comparisons and distinct output pointer types remain separate.
The metadata fields and `ModelEffectAdjustment` type are not renamed.

`Model_SetSlotDrawMode` is the former `func_80059284`. Its unchecked integer
slot/value arguments still narrow through the original byte store at
`+0xE11`. The `int` definition and compatible `s32` declaration are retained.
No enum, Boolean interpretation or input validation is added.

The minimum query retains `gcc_2_8_1_g8` and `0x118`; the override query
retains its existing grouped source, `gcc_2_8_1_g8_split` and `0xDC`;
the setter retains `gcc_2_8_1_g0_split` and `0x28` in every native image.
The two standalone C sources and minimum-query header move to semantic
paths without extracting or reordering the grouped source.

French variant199/829 C bodies retain their historical bytes and attempt
fingerprints through one old-spelling bridge in the owning minimum-query
header. Their headers change only the include path. Existing regression
controls change identifiers/include paths only; the full numeric Lua runtime
and trace-control tree remains unchanged.

## Captured mode and tint

`gModel_bCurrentDrawMode` is the former `D_8009AFE4`.
`Model_DrawSlot` captures the slot's `+0xE11` byte before clearing that byte
unless it is four. Template tinting can publish mode three independently
of the slot byte. Units-only, shadow and continuation paths read the captured
mode; it is not a Boolean flag or a newly classified enum.

The initialized `u8` `.sdata` owner and the renderer's tentative COMMON
declaration remain separate. The renderer retains its exact
`gcc_2_8_1_g8_split_comm` profile, including MASPSX's COMMON handling.
No storage, qualifier, field layout or publication-order change is implied.

`gModel_wDrawTintRedGreen` and `gModel_bDrawTintBlue` are the former
`D_8009AFE6` and `D_8009AFE8`. The template stamp writes red and green
through byte views of the `u16` word and writes blue separately. The neutral
path publishes `0x80` per channel. The shadow fan reads those same values
for its centre and divided rim colours. Their initialized `.sdata` types,
byte ordering, unknown neighbours and byte-view accesses remain unchanged;
no RGB structure or wider allocation is introduced.

`gModel_aShadowFanSteps` is the former `D_800914E8`. Its unsized `u16`
declaration and sixteen initialized halfwords stay exact. The renderer still
reads eight signed x/z Q12 pairs through `ModelShadowFanStep`; storage is
not retyped to a complete array of that view.

## Native bindings

The NA owner labels and explicit Japanese provider bindings acquire the
semantic names. The eight obsolete Japanese/European renderer macros retire
so the C objects publish canonical references.

European link scripts expose the semantic names as aliases to their original
providers: `D_8009BF6C`, `D_8009BF6E`, `D_8009BF70` and `D_800925CC`.
The existing providers and their per-image bindings remain unchanged.
This preserves the original resolution instead of guessing a numeric UK
address or copying a continental address. No new object or storage follows
from a linker alias. Full applicable native image acceptance remains a
PR-side gate; no local UK retail verification is claimed.

All ordering-table identities and renderer views accepted before this
component are retained. The unrelated primitive pointer, remaining handler
bytes and diagnostics stay address-named.

## Runtime observation

In two traced 3-D battles (#418, interpreter CPU, setup confirmed),
`Model_SetSlotDrawMode` only ever stored three: into slot 2 from
`0x8004ED1C`, `0x8004FDC4` and `0x8004FDE4`, and into slot 0 from
`0x8004EEA4`, `0x8004EF18` and `0x8004EFB4`. Polling slot 2's `+E11` saw
it return to zero each frame after a store, as `Model_DrawSlot` clears
it. Slot 2 also read four at each switch into duel main mode `0x03` and
back to zero on the way into a battle, without a setter call, so another
writer stores four directly.

A later five-scene run (#7930; four 3-D battles after the board scene)
identified that other writer. `Model_InitSlot` stores four from its
`flags & 0x7F` argument at NA `0x8004CBA4` for slot 2 while the board
scene (scene mode `195`) is active. It stores zero for every slot in battle
scenes (`193`). `Model_DrawSlotUnitsOnly` stores one around its units-only
draw and then restores the earlier byte, including four.
`Model_SetSlotAmbient` stores three 607 times in battles and zero twice.
`Model_DrawSlot` cleared the byte 10,234 times. All 928 setter calls again
stored three, from the same six return addresses inside
`Model_UpdateAnimatedBattle`. The run queued no tint requests, and the
human saw no colour change in any battle.

The code consumers fit this split. Zero takes the neutral template path,
one returns before the tint/shadow stages, three is the value that the
non-neutral template stamp also publishes, and four survives the per-frame
clear. These are observed values. They do not add an enum, and they do not
show that no other value is ever requested.
