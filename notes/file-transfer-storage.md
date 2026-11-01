# Ready-transfer and request storage contracts

This component names the ready callback's descriptor/source storage and
the loader's scalar/raw request-slot views. It does not allocate a transfer
manager, merge adjacent data or change queue, callback or sector behavior.

## Initialized ready descriptor

`gFile_ReadyTransferDescriptor` is the existing four-byte pointer at NA
`0x8009AF18`, initialized to `&gFile_PrimaryTransferDescriptor` in the
`.sdata` unit `data_8009af10.c`. The two preceding boot sizing constants
and the separate zero word at `0x8009AF1C` remain independent objects.
The initializer, section attribute, ordering and explicit padding remain.

Both definition and extern stay plain `FileTransferDescriptor *`, without
adding `G32`. This is the initialized-pointer case documented by the
existing pointer checker, not an uninitialized pinned pointer to retype.
No fixed NA linker assignment is introduced over the initialized C owner.

`File_TransferReadyCallback` keeps its initial mode selection and every
later reload through this pointer: destination advances, byte counters,
image geometry, GPU polling, SPU setup, callback effects and post-phase
completion checks. The pointer is not newly cached or assumed immutable
across a phase callback.

The existing CD-ready witness still changes the pointer to an alternate
descriptor from its phase callback and checks which descriptor is observed.
Only the global identifier changes in that existing witness; all callback
actions, setup, values, assertions, errors, stubs, fixtures and execution
controls remain byte-identical after reversing that one identifier.
No test is added and no cloud/regional suite is run locally.

| Identity | North America | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Ready descriptor pointer | `0x8009AF18` | `0x8009AE58` | `0x8009BE94` | `0x8009C2A8` |

Native bindings follow the actual target addresses, leaving unrelated
NA-looking regional labels untouched. Only the obsolete Japanese reader
macro retires. The mixed-subject `.sdata` source path is intentionally
retained, and its neighboring constants and padding are not renamed.

All 940 complete compiler/MASPSX outputs match after only this identity,
without instruction/directive/relocation/filename filtering. Complete C
files preserve every other byte except the one exact macro removal, and
the header preserves its complete declaration-token sequence. The entire
prior semantic ledger gains exactly one evidence/caveat row.

## Ready source cursor

`gFile_ReadySourceCursor` remains the existing stored `u32 *G32`. No
matching C initializer has been established, and naming adds no seed, reset,
ownership bound or source-buffer allocation.

Data mode keeps its original copy/skip gates and indexed word-copy loop,
then advances this cursor by one sector unconditionally, including the
hardware-sector and copy-skipped paths. Descriptor destination advancement
retains its separate gate. Neither action is folded into the other.

Image mode loads and advances the cursor only in its software-copy arm.
The hardware `CdGetSector` path still leaves it unchanged. Keep the
descriptor's alternating buffer selection, GPU poll, image stepping and
callback/counter ordering separate from source-cursor traversal.

SPU mode retains the signed selection of `n` against phase remaining,
the signed `n / 4` copy-loop bound and the exact byte-cast cursor advance
by `n` in its software arm. It does not become a rounded word advance,
unsigned loop bound, clamp or validation. The hardware path still leaves
the cursor unchanged, and total-byte accounting still subtracts a sector.

The existing executable witness retains every software/hardware choice,
partial count, expected pointer, gate, setup value and assertion after
reversing just this identifier. No source-string witness or control is
added, removed or weakened.

| Identity | North America | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Ready source cursor | `0x8009B0F8` | `0x8009AFE8` | `0x8009C084` | `0x8009C4A4` |

All 940 complete outputs and the entire callback source match after only
this identity. Pointer type and all native shadows remain unchanged. The
complete prior ledger gains one row; inventories, profiles, manifests and
all unrelated tests, fixtures, tooling and traces remain.

## Active and pending request views

`gFile_ActiveRequestSlot` names the existing scalar C view of the first
request record. Every image's symbol or linker file binds the name at
`0x801D4200` (formerly `D_801D4200`), with no assembler label. The two
whole-record copies take its address as `(u8 *)&gFile_ActiveRequestSlot`
in place of the former `D_801D4200_raw[]` byte view and its
`FILE_TRANSFER_BYTE_VIEWS` selector. The NA copy instructions and
tracked continental bindings identify the same 0x20-byte active/pending
pair; preserving the original linker identity also preserves the existing
Japanese/UK bindings without guessing unavailable retail evidence.

`File_RequestSecondarySlot` still copies `*p` to raw +0x20 before the
original unsigned member-address reload, flag change and descriptor setup.
`File_ActivateTransfer` still copies the secondary descriptor and promotes
the raw +0x20 record to raw +0, then retains its original flag publication.
`SD_ConfigureTransferPhase` merely names its scalar first-slot address.
All branches, signed lengths, skips, field views and completion calls stay.

The existing `FileRequestSlot` layout remains exactly 0x20 bytes. It is
not turned into an array or a larger queue struct, and the raw copy base
is not replaced with a new pointer expression. All 940 complete compiler
outputs are byte-identical without normalization for this checkpoint.
The complete header token change is the scalar name plus explicit old ASM
label, and the complete C change is the single address expression.

Every config file, native alias, inventory, profile, manifest, test,
witness, control, fixture, trace and tool remains unchanged. The entire
prior semantic ledger gains one row describing the named C view, not a
new allocation or renamed linker symbol.

## Staged sound request members

The complete producer, submission, promotion, phase and ready-sector chain
establishes all nine non-padding members of the existing FileRequestSlot:

| Former member | Canonical member | Retained representation |
| --- | --- | --- |
| field_00 | file_selector | s32 at +0x00 |
| field_04 | sector_offset | s32 at +0x04 |
| field_0C | spu_destination | s32 address word at +0x0C |
| field_10 | ram_destination | s32 address word at +0x10 |
| field_14 | spu_transfer_size | s32 at +0x14 |
| field_18 | ram_transfer_size | s32 at +0x18 |
| field_1C | range_sector_count | s16 at +0x1C |
| field_1E | cd_filter_file | u8 at +0x1E |
| field_1F | cd_filter_channel | u8 at +0x1F |

The record stays 0x20 bytes. pad_08 stays four uninterpreted bytes:
neither submission nor naming initializes it, and both whole-record copies
still transport it. Addresses stay signed integer words, not new pointers.

SD_StepActiveCommand case 32 preserves the exact active-tail assignments,
including the fresh request-pointer reloads. Its source-selector cases
0x10/0x20/0x40 still rewrite file_selector to 4/5/6 and choose the existing
SE/BGM/MASTER.XA filename buffer. No default case, mask, validation, failure
handling or new dispatch is added. The general submission path still ORs
the original selector word with 0x1400000; the name is not a promise that
the input is a pure masked file index or that every value is supported.
sector_offset retains its signed value and the existing location helper's
negative/absolute-LBA treatment; it is not converted to a byte offset.

File_RequestSecondarySlot keeps FIVE distinct non-null branches:
all phase sizes and range count zero attempt the existing seek; negative
range count only requests a stop through the shared range flags and returns
one; positive range count adds MASTER.XA's base and submits start/end plus
the channel/file bytes; zero range count with a cancelling signed phase-size
sum returns zero; otherwise the full pending record is copied before the
unsigned sector-offset member-address reload and descriptor initialization.
A null pointer remains only the pending-bit poll, not queue submission.
No branch is collapsed into a Boolean success result or a common busy gate.

SD_ConfigureTransferPhase preserves its mode dispatch and gotos. Phase zero
uses spu_transfer_size and spu_destination for mode-three staged SPU writes;
zero size redirects phase_index to two before trying the RAM phase. The RAM
phase uses ram_transfer_size and ram_destination with mode one and the
original global flag mask. A positive phase size is bytes, but a negative
signed size is transformed by the ORIGINAL -(value << FILE_SECTOR_SHIFT).
The fields are not universally byte counts, sector counts or nonnegative
lengths. Preserve the SPU-first/RAM-second order, skipped-phase cursor,
staging buffer pair, callback and mode-specific source-cursor advancement.

Range command 0x24 retains `(u16)end - (u16)start + 0x10` narrowed to the
signed range_sector_count. Zero chooses seek, negative chooses range-stop,
and positive is the submitted sector span; there is no repaired endpoint,
range/order check, saturation or monotonicity invariant. Command 0x21
still builds a seek request. cd_filter_channel at +0x1F and cd_filter_file
at +0x1E reach callback_data bytes zero/one and the existing Setfilter
file/channel order. Their names do not change masks or normalize bytes.

The scalar first-slot ASM view, raw active/pending aliases, 0x20-byte
copies, request pointer and native identities are unchanged. Every fixture,
control, assertion, mutation, scenario and numeric expectation remains
byte-for-byte unchanged. No new harness or local cloud-fixture execution.

## Bundle boundary

The descriptor, source cursor and scalar/raw request views form one
component PR after cumulative clean acceptance. The source
cursor must retain its mode-specific advancement, including mode one's
unconditional advance and mode two/three's conditional software-copy paths.
Request-slot work must preserve the scalar first-slot view, separate raw
ASM alias, exact 0x20-byte copies and existing phase/request branching.

No descriptor/cursor/request-slot-only PR is opened. Existing controls stay
intact; any necessary witness migration remains identifier-only.

## Sound-command transfer envelope

Command 0x20's queued fields now follow their proven destination roles.
This is a scoped view of SDCommand, not a new request type or universal
interpretation of every queued command:

| Queued member | Active tail word | Existing FileRequestSlot destination |
|---|---:|---|
| transfer_selector | 0 | file_selector, before the original source rewrite |
| transfer_sector_offset | 1 | sector_offset |
| transfer_spu_destination | 3 | spu_destination |
| transfer_ram_destination | 4 | ram_destination |
| transfer_spu_size | 5 | spu_transfer_size |
| transfer_ram_size | 6 | ram_transfer_size |

All six queued members remain signed s32. The active snapshot remains
eight unsigned words with its existing signed reloads and whole-tail copy.
The selector keeps its original full-word producer values, strict 0x20
cancellation comparison and independent masked 0x10/0x20/0x40 dispatch.
No default branch, new validation or reordered request/pointer reload.

The size names do NOT assert byte-only or nonnegative lengths: the existing
phase consumer retains positive byte sizes and negative shifted encodings.
Sector offsets keep the original negative absolute-LBA behavior; addresses
stay integer words. SPU-first/RAM-second phases, skipped-phase behavior,
zero-size seek/stop/sum branches, raw callbacks and cursor arithmetic stay.
No FileRequestSlot, descriptor or transfer implementation changes here.
