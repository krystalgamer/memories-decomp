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

## Secondary payload completion slot

`gFile_SecondaryPayloadCompleteCallback` names the existing word read at
`SD_ConfigureTransferPhase`'s tail. Its nonvolatile
`void (*G32)(void)` declaration moves unchanged from `unmatched.h` into
`file_transfer.h`, the reader's existing owner dependency. The local load,
null test and ordinary local callback call remain unchanged.

The normal `File_RequestSecondarySlot` payload path still installs
`SD_ConfigureTransferPhase` after copying the request's 0x20-byte record.
That phase handler retains its zero-length skip, result change, mode
branches, source buffers, signed size conversion and shared tail. Seek and
range request paths do not gain this callback; the name is not a promise
that every secondary request calls the slot.

Sound command 32 still fills and submits its request before calling
`SD_ArmBusyCallback`. The arming helper sets sound busy first and then
stores `SD_ClearBusyFlag`; completion clears busy through that existing
handler. Registration is not moved earlier and no new race policy or clear
after invocation is introduced. `File_InitTransferState` still does not
clear this slot.

All producer accesses retain their literal-address lvalue macros and exact
`void (**)(void)` expression types. The NA fallback guard, all regional
overrides and busy-before-pointer store order remain. They do not become
symbol-relative stores or acquire a new owner-header include. The complete
producer include graph does not include `file_transfer.h` or `unmatched.h`,
so predefined lvalue macros cannot expand the moved extern declaration.

| Target | Slot address | Existing literal producer view |
|---|---|---|
| North America | `0x8009B128` | Common source fallback |
| Japan | `0x8009B018` | Japanese adapter |
| UK | `0x8009C050` | European adapter |
| France / Germany / Italy / Spain | `0x8009C470` | Spanish adapter selected by all four manifests |

The same exact native addresses bind the reader symbol. Only the obsolete
Japanese runtime macro mapping to `D_8009B018` retires; producer macros do
not retire. The NA producer's two complete compiler outputs stay
byte-identical, including the absence of a new callback-slot relocation.
All 940 complete outputs match after only the slot identity, and all six C
files retain their full contents after that identity and the one macro
removal. Header tokens preserve the moved declaration, with no retyping.
The full prior ledger gains one evidence/caveat row. Inventories, profiles,
manifests, literal constants, tests, witnesses, controls and tooling remain.

## Component boundary

The pre-request, range and secondary payload-tail checkpoints form one
component-level PR after cumulative clean acceptance. Literal producer views
remain separate from symbol-based readers, and callback registration never
justifies adding a loader reset for the fourth slot.

No individual callback-slot PR is opened. Regional/cloud validation stays
PR-side, and no new test harness is added.


## Submission argument views

The three async request wrappers forward the same seven operands unchanged
to File_InitTransferDescriptor: flags, loader_argument, sector_offset,
size, callback, initial_phase_index and target. The initializer's leading
transfer argument selects the descriptor. File_TryStartPrimaryTransfer
uses the same file_index/sector_offset spelling as its existing prototype.
These are argument names only, not new APIs, parameter types or validation.

Flags retain both the low masked file index and all higher control bits.
The initializer stores loader_argument verbatim in the identically named
descriptor member; its u8 pointer is not retyped or claimed to be a path.
File_SetTransferLocation masks the file index even for an absolute request.
A negative sector_offset sets file_bytes to zero and uses its negation as
absolute_lba; otherwise the offset is shifted into file_bytes and added to
the indexed LBA. The primary try-start helper retains its direct unmasked
table lookup and addition, rather than acquiring the location helper's
mask or absolute-location convention.

Size deliberately has opposite signed conventions at the two layers.
File_SetTransferLocation stores a nonnegative size directly as bytes;
a negative size becomes -(size << FILE_SECTOR_SHIFT). The initializer
passes -size to that helper, so its ordinary positive sizes represent
sectors and its negative sizes represent the negated byte count.
The request wrappers do not convert size themselves. Existing signed
negation, shifts, zero cases and overflow behavior are not normalized.

Target replaces the misleading length/arg6 spelling without claiming a
single unit or pointer type. Zero leaves the phase_size zero and skips the
target setup. With flag 0x1000000, a nonzero target is copied unchanged to
direct_destination. Otherwise phase_size receives total_bytes: a negative
target becomes both RAM sector destinations in mode 1, while a positive
target is split into the existing two field_30 halfwords, enables 0x10000
and mode 2, and selects the two shared sector buffers and default image
dimensions. The packed image-position halfwords, truncation and all
initialization order remain. This does not name unrelated uses of
direct_destination or make target a universal destination address.

Callback and signed initial_phase_index retain their existing names and
ABI. The ordinary request still raises its lock before hook arbitration
and retains its original result/common-tail dereference, including the
blocked null-result path. The try request still returns null directly
without adding that lock. Secondary submission still clears pending,
conditionally stops the active range, initializes its own descriptor and
sets pending again. No guard, wait, reset, callback policy or error repair
is introduced. Range submission and its separate byte arguments are
outside this argument-forwarding component.
