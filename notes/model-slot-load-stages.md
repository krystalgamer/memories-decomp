# Model slot loading stages and readiness

The slot loader's three transfer callbacks and readiness query have names
supported by their installed requests, stage consumers and initialization
producers. No underlying slot fields, descriptor types, callback signatures,
phase values or bounds are changed.

## MODEL.MRG record

`Model_LoadMonsterMergeStage` is the former `func_80056D7C`.
`Model_LoadMonsterMerge` installs it for one `0x114`-sector MODEL.MRG record
in duel slots. Its seventeen stages select the slot payload, image/palette
uploads, position-selected variant module and slot-selected primary module,
sound data and final metadata. The consuming stages total `0x114` sectors;
stage sixteen copies the staged metadata and publishes load phase one.

The callback captures the slot and alternate selector before dispatch.
Final overrides are read after copying the metadata and the existing
post-copy call. Cleared/reloaded transfer-flag captures, signed/address
views, source-cursor advancement, rectangle stores and descriptor writes
retain their exact order.

The [high-memory address cohorts](high-memory-load-addresses.md) distinguish
module destinations from module data arguments. Their eight semantic scalar
names preserve the two pointer and six signed-word declaration views;
the general payload prefix and SU-specific words are not renamed or merged.

## Auxiliary and special packages

`Model_LoadAuxiliaryPackageStage` is the former `func_80057544`.
The loader's non-duel path selects it after the existing seven-entry lookup.
Its six stages load the third payload destination, upload the original
image/palette regions and publish the existing shared completion byte.
It does not publish the slot load-phase byte; that distinction is retained.

`Model_LoadSpecialBattlePackageStage` is the former `func_800577B0`.
The loader selects its eleven stages for `MODEL_SPECIAL_BATTLE_ID`.
It loads the slot-zero payload, both variant destinations and sequence/image
data, then copies slot-zero metadata, clears the three existing callback
command words and publishes load phase one. The special ID does not imply
a newly identified monster or a generic per-slot operation.

All three callbacks remain in each native ten-function slot-control object
with the original `gcc_2_8_1_g8_split` profile, source order and complete
jump-table/rodata layout. MODEL.MRG callback size is `0x7C8` in NA/Japan
and `0x7CC` in Europe; special-package size is `0x344` in NA/Japan and
`0x34C` in Europe. The auxiliary callback remains `0x26C`.
European rectangle-store order and wrapper constants remain distinct.

## Readiness, not a Boolean

`Model_GetSlotReadiness` is the former `func_80058DD8`.
It returns two while the byte at `+0xE14` differs from `0xFF`, otherwise
the nonzero test of the byte at `+0xE1F`.

Load requests publish phase zero; ordinary and special transfer completion
publish phase one. `Model_AdvanceSlotLoad` walks the initialization phases
and publishes `0xFF` when the slot is activated. Reset and null-input
initialization establish a settled inactive slot. These producers support
the query's name without renaming or retyping either underlying field.

The exact result set remains `2/0/1`, with phase read before the active
byte. The unchecked `int` index/return ABI, `0x44` extent and named
`gcc_2_8_1_g0` profile are unchanged in every native image. The shared C
source and public header move to semantic paths, with all matching manifests
and regional static splits selecting the same original function.

Existing regression controls migrate identifiers/include paths only.
The numeric model-slot Lua runtime labels, addresses, messages and trace
control remain byte-identical; no new trace run is claimed. No synthetic
or source-string harness is added.

## Runtime observation

Across two 3-D battles traced under #418 (interpreter CPU, setup
confirmed), every `Model_GetSlotReadiness` call -- from `0x8005DD68`,
`0x8005DFE0` and `0x8005E928`, 1,978 in all, for slots 0 and 1 -- found
`+E14` at `0xFF` and `+E1F` set and returned one. Phases zero and two
were not observed; this run only covers slots that had already finished
loading.
