# Resident transfer callback contracts

This component connects the CD ready-system entry, boot stage callbacks and
duel-result package staging through the existing file-transfer descriptor.
It changes declaration ownership and evidence-backed identities, not sector
processing, package selection or descriptor state transitions.

## Ready-system declaration ownership

`File_TransferReadyCallback` already names the matched implementation in
`file_transfer_runtime.c`. Its canonical `void(s32)` declaration now belongs
to `file_transfer.h`, which the implementation already includes. The
formerly separate `file_ready_sector.h` remains a compatibility forwarder to
that owner, without a second private prototype.

The old `func_80013C28` declarations are intentional fixed-address ABI
views, not another callback to rename or remove. Both the
`FUNC_80013C28_CALLBACK_VIEW` arm (`u8, u8 *, u32 *`) and its `s32`
alternative remain exactly as before. Their legacy linker labels and
policy metadata remain unchanged. A canonical implementation prototype
does not erase the three-argument SDK entry shape.

`DsStartReadySystem` still receives the original explicit `DslRCB` cast.
The implementation still masks the first argument to a byte, increments
its byte counter before the event gate and increments the separate
word counter only for event one. Its source remains the same grouped
sixteen-function unit with all original signatures, copy loops, callback
reloads, buffers, flags, signedness and compiler profiles.

The existing CD-ready witness includes both the owning header and the
compatibility path. Neither include, callback-view selector, generated
witness, assertion, stub, control nor fixture changes. In particular,
selecting the three-argument legacy view must not conflict with the
canonical `s32` implementation declaration.

This checkpoint preserves all 940 complete compiler/MASPSX outputs
byte-for-byte, with no symbol, instruction, directive, relocation or filename
normalization. All configuration, inventories, semantic ledger, attempts,
test controls and tooling stay unchanged. Only the existing include
dependency and declaration location move.

## Publication boundary

Ready declaration ownership, the canonical boot-image bridge and the
duel-result transfer callback form one component bundle. The ready entry is
already named; it does not receive a duplicate semantic-ledger row or a
new matched attempt. Boot and result work plus cumulative clean acceptance
precede one PR rather than publishing this header checkpoint on its own.

## Boot callback bridge and declaration owner

`main_boot_load_stages.h` owns the existing package and image callbacks.
The grouped implementation and `Main_RunBootSequence` both include it.
The declarations retain `FileTransferDescriptor *` and signed stage/mode
arguments; the generic transfer interface no longer carries a private boot
prototype or numeric `func_800434F4` callback spelling.

All three regional source branches now pass `Main_LoadBootImageStage`.
The existing request sectors/counts remain distinct: NA `0x1F85/0x22`,
Japan `0x1F90/0x22`, Europe `0x2503/0x25`. Boot package requests, mode
branches, waits, logo creation, executable handoff and frontend reset stay
in their original order.

Japan deliberately keeps the numeric native image identity
`func_80042B64`. The caller adapter now maps
`Main_LoadBootImageStage` to that identity, exactly as the implementation
adapter already does. Its inventory, fixed bindings and compiled definition
are not promoted or renamed. The European numeric-to-canonical caller
macro becomes obsolete and is removed; its other native aliases remain.
The unused NA fixed-address compatibility label remains untouched.

The provider's existing package/image bodies retain every stage, buffer,
mask, volatile load and geometry store, including the European extra image
stage and the unsigned member-address RECT stores in the other image body.
These are boot callbacks, not `Duel_LoadPackageStage` or the separate
duel-result image callback.

| Target | Package callback | Image callback |
|---|---|---|
| North America | `0x80043328`, size `0x1CC` | `0x800434F4`, size `0x168` |
| Japan | `0x80042A08`, size `0x15C` | `0x80042B64`, size `0x168` |
| UK | `0x80043458`, size `0x27C` | `0x800436D4`, size `0x1A4` |
| France / Germany / Italy / Spain | `0x80043658`, size `0x27C` | `0x800438D4`, size `0x1A4` |

All 940 complete resident outputs are conserved. Only the two complete
caller outputs at `c_80043960.o` require the declared legacy-reference
substitution; all other outputs, including the already canonical
`c_80043328.o` provider, remain byte-identical. No instruction, directive,
relocation or filename filtering is used. A global canonical-to-numeric
rewrite would incorrectly change the provider and is not the proof.

Every inventory, manifest, compiler profile, fixed binding and the complete
existing semantic ledger stays byte-identical. Its accepted package/image
rows already establish these names; no duplicate row or matched attempt is
created. Tests, controls, generated witnesses, fixtures, traces and tooling
are unchanged, with regional/cloud validation left to the bundled PR.
