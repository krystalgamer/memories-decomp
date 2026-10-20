# File-transfer arbitration and phase hooks

The loader owns these mutable callback slots while initialization routines
register their handlers. The slots are not DS command-event callbacks and
do not become a new callback registry or transfer manager.

## Pre-primary-request callback

`gFile_BeforePrimaryRequestCallback` names the existing word at NA
`0x8009B10C`. `File_InitTransferState` clears it in the original loader
reset sequence. `File_SetPositionTable` then installs
`File_WaitForTransfers` after that initialization, before its sprite/table
setup and file-position scan.

Both `File_RequestAsyncTransfer` and `File_TryRequestAsyncTransfer` test
the slot. When nonzero they retain the original
`CALL32(void (*)(void), hook)()` before descriptor initialization. When
zero they retain the original blocked-mask/secondary-request path instead.
The ordinary request raises its primary-request lock before this decision;
the try path does not gain that store.

No new wait, early return, guard, callback reset or null handling is
introduced. The registered function is not inlined or assumed immutable.
All existing repeated loads, descriptor setup, common tails and state-word
publication remain in their original order.

The declaration stays `void (*G32)(void)`, nonvolatile, with both its
ordinary and `.data` arms. The selector
`D_8009B10C_IN_DATA` deliberately keeps its existing name and position.
File-position initialization retains its measured absolute store; reset
and request consumers retain their original small-data forms.

| Identity | North America | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Before-primary-request callback | `0x8009B10C` | `0x8009AFFC` | `0x8009C088` | `0x8009C4A8` |

Native binding edits follow these addresses, not the apparent NA address in
an existing regional identifier. Unrelated French compatibility labels at
the NA-looking address remain unchanged. Exactly three obsolete Japanese
macros retire, in the position-table, stream-reset and request adapters.

The checkpoint retains all 940 complete compiler/MASPSX outputs after only
this identity, without filtering instructions, directives, relocations or
filenames. Full C files otherwise differ only by those three exact macro
removals, and header noncomment tokens preserve all declaration forms.
All inventories, compiler profiles, manifests, source paths, tests,
fixtures, witnesses, controls, traces and tooling remain unchanged. The
complete prior semantic ledger gains one evidence/caveat row.

## Component boundary

The range read/pause hooks and secondary-request completion slot join this
pre-request hook in one component-level PR after cumulative clean acceptance.
Range hooks must preserve the sound registration's `.data` arms, loader
reset and GP reads, conditional calls and state-machine fallthrough.

The secondary completion slot also has literal-address producer views in
`SD_ArmBusyCallback`, with regional overrides. Those measured expressions
must not be replaced with a generic symbol store. In particular, callback
registration and completion do not imply that loader initialization clears
this fourth slot: its actual reset behavior must be preserved.

No individual callback-slot PR is opened. Regional/cloud validation stays
PR-side, and no new test harness is added.
