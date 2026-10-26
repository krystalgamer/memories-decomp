# Model-slot load and animation lifecycle views

The slot loader, SDK animation linker, update/render gates and readiness
query establish five related `ModelSlot` views and an interior byte alias:

| Original member | Canonical member | Preserved representation |
| --- | --- | --- |
| `field_E14` | `load_step` | `u8`, `+0xE14` |
| `field_E1F` | `is_active` | `u8`, `+0xE1F` |
| `field_E1A` | `hmd_unit_count` | `u8`, `+0xE1A` |
| `field_E1B` | `animation_sequence_count` | `u8`, `+0xE1B` |
| `field_1E0` | `animation_sequences` | existing `ModelSlotPart *G32[MODEL_SLOT_PART_COUNT]`, `+0x1E0` |
| `D_800F5694` | `gModel_bAuxiliaryLoadStep` | existing incomplete `u8[]` view of slot 2's step |

The record stays `0xE20` bytes. The active field stays a raw byte, not a
Boolean type. Counts, pointer annotation, array extent, casts and original
offset assertions remain unchanged.

## Loading and activation are not the same state

`Model_ResetSlot` clears activity and sets `load_step` to `0xFF`.
Load requests select step 0; transfer completion publishes step 1.
`Model_AdvanceSlotLoad` leaves steps 0 and `0xFF` alone, performs the
existing step-specific operation, and either advances the byte or stores
`0xFF` when the active gate is set.

The active gate is not proof that every load step ran. Step 6 temporarily
sets it for a draw and clears it again. Step 7 can activate non-duelist
slots and end their sequence early; duel-side loads retain their later
steps and final activation. Scene transitions may also deactivate slots
without normalizing the load byte.

`Model_GetSlotReadiness` preserves its phase-before-active observation:
it returns 2 whenever `load_step != 0xFF`, otherwise the active byte's
nonzero test. It remains an `int` readiness query, not a Boolean predicate.
The new field names do not collapse pending, idle-inactive and active
states or change callback timing.

The existing inline `load_phase` getter keeps its name and signed return
view. Debug output, update/control paths, scene transitions, background
drawing and slot rendering adopt the same member identifiers without
changing their conditions or memory accesses.

## The auxiliary transfer's independent completion label

`Model_LoadAuxiliaryPackageStage` phase 5 writes 1 through `D_800F5694[0]`.
This is not an unexplained completion Boolean: every actual regional
provider equals `gModel_aSlots + 2 * 0xE20 + 0xE14`, the auxiliary slot's
`load_step`. The offset is `0x2A54` in all seven images:

| Image | Slots base | Retained load-step provider |
| --- | --- | --- |
| NA | `0x800F2C40` | `0x800F5694` |
| JP | `0x800F04E0` | `0x800F2F34` |
| UK | `0x800F3D70` | `0x800F67C4` |
| FR/DE/IT/ES | `0x800F4178` | `0x800F6BCC` |

The slot initializer's byte offset and record size, the tracked regional
assignments, and the NA native ELF independently agree. Aggregate readers
therefore close the phase-5 producer's role: the loader advances that step,
and the readiness query distinguishes it from the active gate.

`file_transfer_steps.h` retains the old extern and adds
`gModel_bAuxiliaryLoadStep[]` with `__asm__("D_800F5694")`; regional linker
inputs export the canonical alias to the old provider. The incomplete array
and `[0]` store remain. No allocation, extent, Boolean conversion, NA
address pin in another region, or new overlay import is introduced.
In particular, the store is not folded to an indexed aggregate access:
its original independent relocation and fixed auxiliary-slot behavior stay
exact, including unusual caller inputs.

## HMD units and animation sequences have separate counts

For a non-null HMD, `Model_InitSlot` reads the original low byte at header
`+0x0C` and applies the existing 60-unit cap. `hmd_unit_count` drives the
`GsUNIT` walks and coordinate-owner search. It is not the count of animation
parts, coordinates or rendered polygons.

The null-HMD path clears activity and marks loading idle, but retains the
existing state of fields it never reaches, including the HMD-unit count.
No new zeroing or live-model invariant is added.

`Model_LinkSlotAnimationUnit` passes the existing sequence-array tail to
`GsLinkAnim`. Only its original successful scan path adds the returned
count to `animation_sequence_count`. The array is the existing pointer
view; it is not replaced with an SDK declaration, moved, or resized.

The sequence count bounds the established reset, seek, playback-speed,
duration and animation-row loops. It remains a byte with the original
promotion, truncation and overflow behavior. No new limit or assumption
that it equals `hmd_unit_count` is introduced. Resetting a count does not
newly clear old sequence pointers.

## Sound setup and animation-cue gates

The later audio component identifies two independent raw bytes:

| Original member | Canonical member | Preserved representation |
| --- | --- | --- |
| `field_E1D` | `skip_sound_setup` | `u8`, +0xE1D; packed-slot option retains 0x80 |
| `field_E1E` | `animation_sound_enabled` | `u8`, +0xE1E; existing nonzero cue gate |

`Model_LoadMonsterMerge` captures `slot & 0x80`, then retains the original
`slot &= 0x7F`. Only the zero-option path calls `SD_KeyOffVoiceSlots`.
It waits for transfers and calls `Model_InitSlot` with a null HMD before
storing the original raw option byte. The only traced flagged caller is
`Model_UpdateCreditsMonsterSlot`, which passes `arg0 | 0x80` while cycling
random models. Ordinary debug, Library and special-battle calls retain
their unflagged arguments. No new slot bounds, Boolean conversion or
caller-domain restriction is introduced.

In `Model_LoadMonsterMergeStage`, nonzero skip state consumes phase 13's
one sector and phase 14's fifty sectors through the original mode-1
skip-RAM-write path. The unflagged phase 13 writes the sequence block to
`D_801A8000 + index * FILE_SECTOR_SIZE`. Unflagged phase 14 keeps both
descriptor destination assignments and uses mode 3 with SPU address
`0xD810 + index * 0x19000`. This is sample audio, NOT an image upload:
`File_TransferReadyCallback` case 3 uses `SpuSetTransferStartAddr` and
`SpuWrite`, whereas image writes are its separate case 2. The SPU bases
and 0x19000-byte sample regions independently agree with
`SD_LoadSequenceBankPair`'s two-side note-address rebasing.
Mode 3 reads `sector_destination`; assigning the alternate pointer is
not evidence that sample writes alternate buffers like image mode 2.
All phase sizes, masks, descriptor writes and sector consumption stay exact.

Phase 16 still copies every sound entry and the complete CF8 block, applies
the existing comparison overrides, and only then resets all THREE CF8
command words to -1 for a skipped setup. This option therefore also
disables those command requests; it is not permission to discard copied
metadata or simplify the control callbacks. The loader still publishes
step 1, and the mapped-HMD initialization retains the skip byte.

Staged-load step 10 registers the sequence bank only when skip state is
zero. Step 11 sets `animation_sound_enabled` to 1 only in that same
unflagged case, but sets `is_active` unconditionally. The flagged arm
does NOT add an explicit clear of the cue byte. `Model_InitSlot` already
clears that byte before testing HMD nullness; it clears skip state only
in the null-HMD branch, before the loader restores its captured option.
`Model_ResetSlot` retains its whole-record clear and explicit skip-byte
zero. Do not infer a global inverse relationship between these bytes
from one complete load path or alter partial/reset/direct-load behavior.

`Model_PlaySlotAnimationSounds` keeps its active test first, its exact
tint-mode/black-RGB gate next, then its nonzero animation-sound gate.
The ordinary entry walk and special command branch retain animation
selection, duration/frame windows, spatial volume/pan, CD seek and sound
dispatch, repeated selector reads, the volatile diagnostic cursor reload
and Japan's absent debug print. Neither byte is a ready/visible predicate.
Skipping setup does not mute existing SPU voices or guarantee silence:
the flagged request deliberately avoids the original voice key-off.
Enabling cues does not promise that a bank, entry or command is valid.

The existing offset-based model-transfer oracle, its mutations, exact
byte expectations and every other fixture are entirely unchanged.
No new behavioral harness, fixture execution, SDK declaration, profile,
manifest, source group or runtime invariant is introduced.

The tentative textured-primitive helper, palette conversion research,
packet/cache fields and unrelated records are not changed.

## Existing native consumers

The background, model-control, model-debug, background-ownership and
scene-transition witnesses use the production type. Their corresponding
member names change in their existing C accesses and mutation strings.
Each complete file retains every other byte, preserving independent
oracles, byte offsets, guard mutations, unique-anchor checks, failure
expectations, input matrices, compiler flags and runners.

No new synthetic/generated-C/source-string harness or fixture is added,
and no existing control is skipped or weakened. Source/profile/header
closure, native compilation artifacts, whole executable and ELF identity
remain the local conservation gates; actual runtime witness and regional
image execution remain normal PR validation, not local execution.

The five fields are runtime record members and do not get invented
fixed-VMA rows. The one proven existing interior byte view gets one
evidence-backed global row and a canonical export in each image. All
legacy native symbols, linker prefixes, source groups, profiles, SDK
headers and stored-pointer conventions stay frozen.
