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

## Range read and pause hooks

`gFile_RangeReadCallback` and `gFile_RangePauseCallback` remain
nonvolatile `void (*G32)(void)` slots. Their default/`.data` arms and the
existing `D_8009B0F0_IN_DATA` / `D_8009B120_IN_DATA` selectors remain.
Sound initialization still clears its own work RAM before registering
`SD_FadeInStreamAudio` then `SD_FadeOutStreamAudio` through the absolute
views. Loader initialization clears pause then read at the original points.

The loader's range path still handles its position-query submission and
command-busy gate before processing the range state. In state one, a failed
pause-command submission returns without the pause hook. A successful
submission publishes command-busy, loads the pause hook and enters the
shared conditional-call tail. This is not a DS pause-completion callback.

State five still publishes state six, seeds the halfword countdown to
`0x258`, clears `0x1000`, sets `0x4000`, then loads and conditionally calls
the read hook. It falls through into state six in the same invocation.
No extra return, timer initialization call or frame-step multiplier is added.

State six keeps its same-call first decrement, signed halfword test and
signed descriptor-position comparison. The ending path clears the original
state bits, loads the pause hook, sets the low-field stop value and reaches
the original shared conditional-call tail. Both hooks can therefore run
in one invocation when the original ending conditions hold.

The local callback variables and call expressions are unchanged. Naming
does not cache the hooks across invocations, clear a hook after use, merge
the flag stores, or alter registration, null behavior or sound fade logic.

| Identity | North America | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Range read hook | `0x8009B0F0` | `0x8009AFE0` | `0x8009C07C` | `0x8009C49C` |
| Range pause hook | `0x8009B120` | `0x8009B010` | `0x8009C068` | `0x8009C488` |

Exactly six obsolete Japanese macros retire across the reset, runtime and
sound-initialization adapters. Whole C files and all 940 complete outputs
otherwise differ only by these two names; both headers retain every
noncomment token after the same identity substitutions. All native binding
files preserve unrelated address shadows, and the complete previous ledger
gains exactly two rows. Inventories, profiles, manifests and all existing
tests, witnesses, controls, fixtures, tools and traces remain unchanged.

## Component boundary

The pre-request and range hook checkpoints join the secondary phase-tail
slot in one component-level PR after cumulative clean acceptance.

The secondary completion slot also has literal-address producer views in
`SD_ArmBusyCallback`, with regional overrides. Those measured expressions
must not be replaced with a generic symbol store. In particular, callback
registration and completion do not imply that loader initialization clears
this fourth slot: its actual reset behavior must be preserved.

No individual callback-slot PR is opened. Regional/cloud validation stays
PR-side, and no new test harness is added.
