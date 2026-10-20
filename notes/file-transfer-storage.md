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
request record. Its explicit `asm("D_801D4200")` label preserves the exact
preexisting linker identity. `D_801D4200_raw[]` and its same ASM label
remain unchanged, including the `FILE_TRANSFER_BYTE_VIEWS` selector.

No new native address binding is introduced. The NA copy instructions and
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

## Bundle boundary

The descriptor, source cursor and scalar/raw request views form one
component PR after cumulative clean acceptance. The source
cursor must retain its mode-specific advancement, including mode one's
unconditional advance and mode two/three's conditional software-copy paths.
Request-slot work must preserve the scalar first-slot view, separate raw
ASM alias, exact 0x20-byte copies and existing phase/request branching.

No descriptor/cursor/request-slot-only PR is opened. Existing controls stay
intact; any necessary witness migration remains identifier-only.
