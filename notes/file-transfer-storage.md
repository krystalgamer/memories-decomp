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

## Bundle boundary

The ready source cursor and the scalar/raw request-slot views join this
pointer in one component PR after cumulative clean acceptance. The source
cursor must retain its mode-specific advancement, including mode one's
unconditional advance and mode two/three's conditional software-copy paths.
Request-slot work must preserve the scalar first-slot view, separate raw
ASM alias, exact 0x20-byte copies and existing phase/request branching.

No descriptor/cursor/request-slot-only PR is opened. Existing controls stay
intact; any necessary witness migration remains identifier-only.
