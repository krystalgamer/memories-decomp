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
