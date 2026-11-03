# File-transfer state and cancellation handshake

These two existing words connect descriptor setup, asynchronous CD/DS
callbacks, the per-frame transfer service and package/game wait predicates.
Each word has one name; a translation unit selects its addressing with
`FILE_TRANSFER_WORDS_IN_DATA`, which adds no storage.

| Former identities | Canonical name | Type |
| --- | --- | --- |
| `D_8009B0F4`, `D_8009B0F4_abs` | `gFile_dwTransferState` | volatile `u32` |
| `D_8009B134`, `D_8009B134_abs` | `gFile_dwCancelRequestState` | nonvolatile `u32` |

## Transfer/request state

`File_InitTransferState` clears the word. Primary setup and
`File_ActivateTransfer` publish the descriptor's original `status_flags`
combined with `FILE_TRANSFER_STATE_PRIMARY_ACTIVE`. Secondary setup clears
and then raises `FILE_TRANSFER_STATE_SECONDARY_PENDING` around descriptor
initialization. The request-lock bit retains its existing promotion guard.

Command submission/completion pairs raise and clear
`FILE_TRANSFER_STATE_COMMAND_BUSY`. Range-read completion raises the
position-query pending bit; the stepper submits `GetlocL` and raises its
busy bit; the logical-position callback clears that busy bit.

The ready callback, package/model callbacks and sound transfer phase retain
their existing source, image, range and descriptor flags. Movie playback
also contributes the existing `0x02000000` busy bit. This is therefore a
shared transfer/request-state word, not just CD status, one command's
result or a normalized ready Boolean. No complete flag enumeration is
inferred: opaque high bits, signed whole-word tests and all raw masks stay.

The cross-component wait predicate remains exactly
`(state & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | cancel_state`, including
callers that spell the same mask numerically. No predicate consolidation
or ordering change is introduced.

## Cancellation is distinct from secondary pending

`File_CancelTransfers` writes `0x80` when work is outstanding, after its
existing active-range stop guard. The per-frame service detects a nonzero
word without bit `0x40`, latches that bit and sets the primary descriptor's
`done` to 5 and `substate` to 0 before stepping it.

For a non-range transfer, `done == 5` ends ready callbacks and submits
DS Pause (9). `File_HandleCancelPauseEvent` handles the existing retry
event, then on completion sets `substate` to 1 and clears command-busy.
The next step finishes the active transfer.

`File_FinishActiveTransfer` propagates an outstanding cancellation through
a promoted secondary request, restoring `0x80` and applying the range
stop guard when appropriate. Otherwise it clears the cancellation word;
idle service clears it too. Range transfers retain their separate
stop/step-flag path rather than being forced through the non-range case.

This establishes a cancellation handshake, not the secondary-pending
request itself: that pending bit belongs to the transfer-state word.
The legacy header's description is retained for compatibility, not used to override the actual producer/consumer evidence.
The new name does not promise immediate cancellation, normalize arbitrary
values or change the retry/queue policy.

## Declaration and native identity preservation

`file_transfer.h` preserves the measured distinction: transfer state is
volatile and cancellation state is not, in both addressing arms. A unit
that defines `FILE_TRANSFER_WORDS_IN_DATA` before its first include sees
`.data`-section declarations and gets absolute `%hi`/`%lo` accesses; other
units keep small-data eligibility and gp-relative accesses. This preserves volatile
read-modify-write order and the cancellation value held live across its
test/update sequence.

No link aliases remain: each region's storage carries the canonical name
directly, and the European runtime wrapper defines both words. No guessed
allocation or initializer is added.

Regional wrappers that include a header before the North American source
repeat the selector at their top so the first include sees it.

In the Japanese image `0x8009B134` holds the dialog text ID
`gDuel_wEffectDialogTextID`, not logical cancellation state; that image
binds `D_8009B134` at `0x8009B024`.
The unrelated sound-parser historical comment also remains unchanged.

## Scope and controls

The component changes only scoped names and an isolated include in the
actual consumers. Old headers, regional wrappers, overlays/imports,
function records, named profiles, callback ABIs and all other identities
remain frozen. Existing unusual source expressions and the primary
request's null-result dereference path are not repaired as part of naming.

Eight existing root controls receive literal namespace substitutions.
The package-stage witness additionally includes the canonical declarations
for its existing plain/absolute address-equality check. Whole-file inverse
reconstruction removes only that include and reverses the names; cases,
assertions, mutations, generators, compiler/setup and link-alias behavior
remain unchanged.

The French and Spanish Options controls retain their old import names
because their overlay sources are unchanged. No harness, fixture, probe,
testcase or weakened control is added. Cloud/regional/overlay/witness
suites are not executed locally; native compiler conservation does not
replace fresh PR-side full regional-image checks.
