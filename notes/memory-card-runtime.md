# Memory-card Runtime Events

The resident memory-card layer uses Psy-Q BIOS events to convert asynchronous
card notifications into a small polling result. Matching
`MemCard_InitIOEvents` creates eight event handles in the array at
`gMemCard_aIOEventHandles` (`0x800F2AE0`): the same four event specifications
are registered once for `SwCARD` and once for `HwCARD`.

## Subsystem startup

Matching `MemCard_Init` (`0x80043E30`) establishes the game-owned startup
order. It forwards its argument to `InitCARD`, calls `StartCARD`, passes zero
to `ChangeClearPAD`, and then invokes `_bu_init`. This is the legacy
`InitCARD`/`StartCARD` path; the separately identified
`InitCARD2`/`StartCARD2` entry points are not part of this matching wrapper.
Event creation remains a separate step handled by `MemCard_InitIOEvents`.
The one caller, `MemCardWork_UpdateOperation`, passes `1` and calls `MemCard_InitIOEvents`
immediately afterwards.

## High-level LIBMCRD dialog lifecycle

The matching modal state machine `MemCardDialog_Update` (`0x8003F454`) uses the
separate high-level interface from `libmcrd.h`. On first entering its active
state, it calls `MemCardStart`, creates the dialog object, and moves that
object into place before dispatching the selected operation through
`gMemCardDialog_apfnSteps[gMemCardDialog_StepIndex]`.

When state bit `0x1000` marks a pending high-level command, the update calls:

```c
MemCardSync(
    1,
    (PSXLONG *)&gMemCardDialog_SyncCommand,
    (PSXLONG *)&gMemCardDialog_SyncResult
);
```

The output-word roles follow the actual SDK `cmds` and `rslt` parameters,
not guesses from their numeric values. The function return is separately
stored in `gMemCardDialog_SyncStatus`; the existing local is then tested
against `1`. Every other value leaves the pending state intact and returns
before operation dispatch. These words are not the final dialog outcome
or the legacy BIOS event result.

When the selected operation clears the modal state, or the dialog takes its
acknowledged exit path, the function calls `MemCardStop`, starts the object
moving off screen, and later destroys both the object and its text box. This
per-dialog `MemCardStart`/`MemCardSync`/`MemCardStop` lifecycle is distinct
from the subsystem-wide `InitCARD`/`StartCARD` startup and from the low-level
`_card_info`/`_card_clear`/`_card_load` event sequence below.

### Dialog API and steps

Callers never drive `MemCardDialog_Update` directly. `MemCardDialog_Request`
(`0x8003F758`) copies the filename into `D_800EFE18`, stages the buffer, the
byte size and the block count, and calls `MemCardDialog_Start` (`0x8003F740`),
which sets `MEM_CARD_DIALOG_FLAG_ACTIVE`, stores the step index and clears the
step's first-pass byte `D_8009B3C1`. Each frame the caller then calls
`MemCardDialog_Poll` (`0x8003F70C`), which runs one `MemCardDialog_Update` and
returns `0` while `gMemCard_wDialogFlags` is still nonzero, then the outcome
byte `gMemCardDialog_Outcome`. The update opens the box with `MemCardDialog_CreateObject`
and slides it in and out with `MemCardDialog_StepSlide`. The operation
handlers report through `MemCardDialog_SetMessage`, which replaces the
message `gMemCardDialog_MessageCode` and raises `MEM_CARD_DIALOG_FLAG_RESULT_READY` together
with the caller's presentation bits. Bit `0x20` synchronously pumps until
the channel's `field_30` object exists; this is not a text-done wait.
Later invocations use the original completion-flag test, and choice
handlers read the player's answer from `gDialog_bChoice`.

The NA step table `gMemCardDialog_apfnSteps` has five entries:

| Step | Entry | Runs |
|---:|---|---|
| `0` | `MemCardDialog_StepLoad` | `MemCardDialog_UpdateLoad` from state `0`, the confirmation prompt |
| `1` | `MemCardDialog_StepLoadUnprompted` | `MemCardDialog_UpdateLoad` from state `1`, with dialog flag `0x200`, which swaps the accept and success messages |
| `2` | `MemCardDialog_StepSave` | `MemCardDialog_UpdateSave` from state `0`, also a confirmation prompt |
| `3` | `MemCardDialog_StepNone` | nothing; no caller selects it |
| `4` | `MemCardDialog_UpdateTradeSave` | reads the save from each card slot, checks both with `SaveData_HasSameDuelistCode`, and writes them back |

`MemCardDialog_UpdateLoad` accepts the card with `MemCardAccept`, finds the
file with `MemCardGetDirentry` and reads it with `MemCardReadFile`.
`MemCardDialog_UpdateSave` adds the directory and free-space checks
(`MemCard_FindEntry`, `MemCard_CalcFreeBlocks`), an optional `MemCardFormat`,
`MemCardCreateFile`, and `MemCardWriteFile`. Step `4` is the trade
write-back described under the selector table below.

### Request, message and outcome publication contracts

Five existing identities complete the modal request/dispatch interface:
`gMemCardDialog_apfnSteps` at `0x80090F9C`,
`gMemCardDialog_StepIndex` at `0x8009B3DE`,
`gMemCardDialog_Outcome` at `0x8009B3EF`,
`gMemCardDialog_ChannelIndex` at `0x8009B3EE`, and
`gMemCardDialog_MessageCode` at `0x8009B3C6`. They are separate from the
legacy request/event work operation, its mode/state bytes and workspace.
The shared `D_8009B3C1`, tentative internal modal state and existing flag
halfword are not retyped or given a new universal interpretation.

`MemCardDialog_Start` retains exactly flag-halfword `ACTIVE`, the low
byte of its full-word step argument, and shared first-pass byte zero, in
that order. It does not clear the previous outcome, message, channel
selection, object pointer or whole modal workspace. `Request` keeps its
existing filename copy, raw size and buffer stores, offset `0x200`, rounded
block-count expression and later `Start` call. No new validation or
request-lifetime policy is introduced.

The step table definition and declaration remain unsized arrays of
`void (*)(void)`, with the original five NA initialized words/order.
Dispatch still uses the stored byte directly, after the original open,
message and I/O gates. Neither its byte type nor the five observed entries
justifies a widened table, mask, index guard or fallback callback. The
existing Japanese/European alternative to NA's empty step three remains
regional; no common replacement initializer is invented.

`SetMessage` keeps its full-word arguments. It loads the flag halfword
through the explicit volatile view, ORs `RESULT_READY` into the supplied
bits, writes the low message byte, masks the saved halfword with `0xFF87`,
then performs both original volatile flag stores: masked value, followed
by masked value OR the raw bits. Those stores are not collapsed or
replaced by an atomic snapshot. `gMemCard_wDialogFlags` itself remains
plain for other readers and writers.

The modal update still sets `RESULT_CREATED` before `TextBox_Create`.
That call consumes the selected channel byte and message byte with the
existing regional geometry. Presentation bit `0x20` pumps unboundedly
until `field_30` is nonnull and returns; bit `0x10` takes the original
regional `0x1008/0x1010` flag arm. The remaining arm calls the same
existing text helper and enters the existing completion block. Later
pumping retains its raw word flag read, regional completion mask and
exact equality; no new readiness, timeout or text-ownership meaning is
added.

Channel selection defaults to zero, then scans the four channel flags
for the first inactive slot. If all are active, zero survives; the scan
does not reserve the slot. Modal text creation/position/teardown and the
paired-load display paths keep sharing this byte. Its `u8` declaration,
Japanese `0x60` stride and all existing aliases/views remain.

The open-initialization path seeds outcome two. Operation handlers retain
their one/three stores, while `Poll` first calls the update and returns
literal zero whenever flags remain nonzero. That return does not write
the outcome byte. After a closing slide finishes, text destruction,
display-object release and pointer nulling occur in the existing order,
but closing flags still survive that invocation. A later null-object
pass clears flags, allowing that `Poll` to expose the stored outcome.
There is no immediate-on-release result, new result enum or Boolean.
`Poll` always pumps even if flags were initially zero; no idle/idempotence
guard is added.

The existing SDK session brackets and their regional ordering remain:
Japan runs its additional close helper before `MemCardStop`, while Europe
runs it afterwards. The display close phase and later flag clear are not
moved into those SDK calls.

### Native identities and naming-only acceptance

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Modal table | `0x80090F9C` | `0x80090E4C` | `0x80091FB4` | `0x800923C8` |
| Step index | `0x8009B3DE` | `0x8009B2CE` | `0x8009C350` | `0x8009C770` |
| Outcome | `0x8009B3EF` | `0x8009B2DF` | `0x8009C319` | `0x8009C739` |
| Channel index | `0x8009B3EE` | `0x8009B2DE` | `0x8009C31A` | `0x8009C73A` |
| Message code | `0x8009B3C6` | `0x8009B2B6` | `0x8009C337` | `0x8009C757` |

The UK step index is the actual wrapper-selected native byte at
`0x8009C350`, not the unrelated `D_8009B3DE` alias at `0x8009C34A`.
Likewise the Japanese channel byte is not the unrelated
`gJapanese_D_8009B2DE` at `0x8009B1CE`. Existing French NA-number aliases
stay untouched. Exact native bindings replace only the eight obsolete
regional macros; all other SDK, out-parameter, geometry and channel
aliases remain.

All 940 complete compiler/MASPSX outputs match after reversing only
these five identifiers. No instructions, directives, relocations or
filenames are filtered. Entire changed C files preserve every other
byte except those identities and eight obsolete macros. Every native
function inventory, compiler profile, manifest, source/header path,
grouped object, layout and initialized table position remains. The whole
accepted semantic ledger gains exactly five evidence/caveat rows.
Tests, fixtures, controls, traces and captures are unchanged; no test or
synthetic/generated-C/source-string harness is added. Clean retail
matching remains mandatory and regional/cloud validation stays PR-side.

The original five-name modal proof precedes accepted #7642 integration.
Its complete legacy work-operation identities, bounded workspace and
raw mode/state declarations, native bindings, ordered ledger and existing
identifier-only witnesses remain intact. The combined proof explicitly
aligns only those five accepted work names in the saved baseline before
reversing the five owned modal names. All complete outputs stay in the
comparison, and a fresh clean gate covers the integrated tree.

## Registration matrix

All eight calls use interrupt mode `EvMdINTR` (`0x1000`). The descriptor and
event constants come from Psy-Q's `kernel.h`; `OpenEvent`, `EnableEvent`,
`CloseEvent`, and the critical-section interfaces are declared by
`libapi.h`.

| Handle slots | Descriptor | Specification | Callback | Result |
|---|---|---|---|---:|
| `0`, `4` | `SwCARD`, `HwCARD` | `EvSpIOE` (`0x0004`) | `MemCard_SetIOResultCompleteCB` | `0` |
| `1`, `5` | `SwCARD`, `HwCARD` | `EvSpTIMOUT` (`0x0100`) | `MemCard_SetIOResultTimeoutCB` | `1` |
| `2`, `6` | `SwCARD`, `HwCARD` | `EvSpERROR` (`0x8000`) | `MemCard_SetIOResultErrorCB` | `2` |
| `3`, `7` | `SwCARD`, `HwCARD` | `EvSpNEW` (`0x2000`) | `MemCard_SetIOResultNewCardCB` | `3` |

`MemCard_InitIOEvents` enters a critical section before opening the events,
enables all eight handles after creation, and exits the critical section only
after the complete set is active. It also resets three surrounding memory-card
state values: the request code `gMemCard_bRequest` (to idle), the directory
flags `gMemCard_bDirFlags`, and `gMemCard_pDirEntries`. `gMemCard_pDirEntries`
is the current directory-entry buffer: `MemCard_DoLoadDirectory` points it at
`gMemCard_aDirEntries`, stores the loaded entry count in
`gMemCard_nDirEntries`, and passes both to `MemCard_CalcFreeBlocks`.
`MemCard_FindLoadedEntry` forwards the same buffer/count pair to
`MemCard_FindEntry` when it searches for a caller-supplied filename.

The paired matching `MemCard_CloseIOEvents` teardown enters a critical section,
closes the same eight `gMemCard_aIOEventHandles` entries in order, and then
exits the critical section.

## Shared result

The four callbacks write the volatile `gMemCard_nIOResult` word at
`0x8009B450`. Their high-base
address expressions all resolve to that same word:

```text
0x800A0000 - 0x4BB0 = 0x8009B450
```

The callback and polling meanings are:

| Value | Meaning |
|---:|---|
| `-1` | An operation is pending; no event result has arrived. |
| `0` | I/O completed. |
| `1` | The operation timed out. |
| `2` | An error event fired. |
| `3` | A new-card event fired. |

`MemCard_ClearIOEvents` (`0x80043D48`) first calls `TestEvent` on the four
handles supplied by its caller, which consumes any signal left over from an
earlier operation, and then sets the shared result to `-1`, preparing the next
asynchronous operation. Every event-driven `_card_*` call in the driver is
preceded by one.
`MemCard_WaitIOEvent` (`0x80043DA0`) provides a synchronous companion: it
tests the same four slots in order and returns `0` through `3` for the first
signaled event, or `-1` when its caller requests a single nonblocking pass and
none is ready. Nothing in the executable calls it.

The driver picks the handle set by operation. `_card_info`, `_card_load` and
the file-level `read` and `write` are prepared against
`gMemCard_aIOEventHandles`, the `SwCARD` set; `_card_clear`, `_card_read` and
`_card_write` are prepared against `gMemCard_aHwIOEventHandles`, the `HwCARD`
set four handles further on.

Matching `MemCard_ClearCard` (`0x80044038`) shows how the result drives
retries. It prepares the `HwCARD` set, calls `_card_clear(chan)`, waits while
the result is negative, and retries only result `1` (timeout), with at most
ten attempts. It has no caller either; the request path runs the same clear
without blocking inside `MemCard_DoLoadDirectory`, described next.

## Card check and directory load

`MemCard_DoLoadDirectory` (`0x80044608`) is the first stage of every request
the poll at `0x80044838` runs. It advances one step per call on the sub-state
byte `gMemCard_bLoadStep`, returns `-1` while it has started a new event and is
waiting, and otherwise returns the shared result:

| Sub-state | Waits on | Next |
|---:|---|---|
| `0` | `_card_info` | Timeout re-issues `_card_info` until the retry byte `gMemCard_bRetries` runs out, and an error finishes with `2`. A completed info on a card already listed (bit `0x80` of `gMemCard_bDirFlags`) finishes with `0`, unless the request is `8`. Otherwise, and on a new-card event, request `1` finishes with `3`; every other request clears the card. |
| `1` | `_card_clear` | An error retries the clear. Completion starts `_card_load` and moves to `2`. |
| `2` | `_card_load` | An error retries the load. Then bit `0x80` is set, and on success `MemCard_FindFiles` lists `*` into `gMemCard_aDirEntries` and `MemCard_CalcFreeBlocks` stores the free count in `gMemCard_nFreeBlocks`. |

A new-card result at the end of the load stage is returned as `4` rather than
`3`; that conversion is internal state-machine bookkeeping, not a fifth event
callback. `gMemCard_bDirFlags` is cleared by `MemCard_InitIOEvents` and nothing
else clears bit `0x80`, so once a listing has been attempted, a plain completed
info reuses it. A new-card event still forces the clear and reload, and so does
request `8`, the one that goes on to add a file.

## Request slot

The low-level driver runs one request at a time. `MemCard_BeginRequest`
(`0x800440B4`) claims the slot: it returns `0` while `gMemCard_bRequest` is
nonnegative, which means an earlier request is still pending, and otherwise
stores the channel in `gMemCard_bChannel`, the request code in
`gMemCard_bRequest`, resets the retry byte `gMemCard_bRetries` to `10` and both
sub-state bytes, `gMemCard_bLoadStep` and `gMemCard_bRequestStep`, to `0`, and
sets `gMemCard_nIOResult` to `-1`. Every wrapper below calls it first and
returns its refusal unchanged; on success the wrapper stages its arguments in
the shared request globals, starts `_card_info(chan)` against
`gMemCard_aIOEventHandles`, and returns `1` without waiting.

The poll at `0x80044838` is matching C in `src/game/mem_card_driver.c`.
Its dispatch fixes what each code does.
It returns `-1` while the slot is idle and `0` while the request is still
running; when it finishes it writes the result first and then the request
code through its two output pointers, puts `gMemCard_bRequest` back to `-1`,
and returns `1`. This write order remains observable when the outputs alias.

The BIOS open, seek, and transfer loops allow eleven calls: the first attempt
plus ten retries. Creation rejects a free-block count plus requested size of
16 or more with result `7`, rejects an existing name with `6`, and reports
`2` when the creation retry byte is exhausted. Retry and timeout bytes wrap
before their signed tests; the controller does not clamp them.

The separate directory and transfer result lifetimes, explicit closes on
both file-failure paths, and the shared create-loop initializer preserve
the original instruction scheduling without register bindings or inline
assembly. The controller contributes 1,180 text bytes and a 48-byte,
twelve-entry request jump table. Its native ILP32 witness in
`tools/project/tests/test_mem_card_io_controller.py` covers 973 cases at
both optimization levels and rejects six behavioral mutations. The BIOS
and file interfaces are stubbed: these checks exercise the controller,
not memory-card hardware.

| Code | Wrapper | Staged arguments | What the poll does |
|---:|---|---|---|
| `1` | `MemCard_ReqCardInfo(chan)` | none | Stops after the `_card_info` stage of `MemCard_DoLoadDirectory`, so it reports the card state without clearing it. |
| `2` | `MemCard_ReqLoadDirectory(chan)` | none | Finishes `MemCard_DoLoadDirectory`, which lists `*` into `gMemCard_aDirEntries` and counts free blocks into `gMemCard_nFreeBlocks`. |
| `3` | `MemCard_ReqReadFile(chan, name, buf, offset, size)` | path, `gMemCard_pRequestBuf`, `gMemCard_wRequestOffset`, `gMemCard_wRequestSize` | `open` with `O_RDONLY \| O_NOWAIT`, `lseek` to the offset, `read`. |
| `4` | `MemCard_ReqWriteFile(chan, name, buf, offset, size)` | path, `gMemCard_pRequestBuf`, `gMemCard_wRequestOffset`, `gMemCard_wRequestSize` | The same with `O_WRONLY \| O_NOWAIT` and `write`. |
| `8` | `MemCard_ReqCreateFile(chan, name, blocks)` | path, `gMemCard_wRequestSize` | Result `7` when `gMemCard_nFreeBlocks` plus the block count reaches `16`, `6` when `MemCard_FindFiles` already finds the name, otherwise `open` with `O_CREAT` and the block count in the high half of the mode. |
| `11` | `MemCard_ReqReadSector(chan, buf, sector)` | `gMemCard_pRequestBuf`, `gMemCard_wRequestOffset` | `_card_read(chan, sector, buf)`, bypassing the file system. |
| `12` | `MemCard_ReqWriteSector(chan, buf, sector)` | `gMemCard_pRequestBuf`, `gMemCard_wRequestOffset` | `_card_write(chan, sector, buf)`. |

The path is `gMemCard_szRequestPath`, formatted as `bu%02X:%s` from the channel
and the name. `gMemCard_wRequestOffset` is the byte offset for the file codes
and the sector number for the raw codes, and `gMemCard_wRequestSize` is a byte
count for reads and writes but a block count for creation, which is why those
two globals carry neutral names; the argument names in the table are the
wrappers' parameters.

The code-`8` capacity test is recorded as the retail instructions have it
(`addu`, `slti 0x10`) rather than interpreted. `gMemCard_nFreeBlocks` is the
*free* count `MemCard_CalcFreeBlocks` returns, so the sum does not compare the
request with the space left; the one caller makes its own free-count check
before it issues the request, as described below.

`MemCard_ReqLoadDirectory` is the one wrapper that blocks. Before it leaves
request `2` for the poll it runs `_card_info(chan)` against the primary
handle set, `_card_clear` against the alternate set using channel byte
`gMemCard_bChannel`, then `_card_load(chan)` against the primary set again,
resetting the shared result before every stage and waiting for a nonnegative
event result after each one. It does not reinterpret those three results.

Six of the seven wrappers have one caller, the `func_8003DC1C` controller, and
its arguments agree with the table: it issues `MemCard_ReqLoadDirectory`
straight after `MemCard_Init` and `MemCard_InitIOEvents`, reads `0x1E00` bytes
at offset `0x200` of `gMemCard_szSaveFileName` into `0x80200000`, writes a
`0xA00`-byte image at offset `0` that starts with `gSaveData_aHeaderTemplate`,
creates the file only when the free count is at least its block count, and
reads one sector to `0x80210000`, patches bytes `+0x7A..+0x7E`, recomputes the
XOR of the first `0x7F` bytes into `+0x7F`, and writes that sector back. The
last is the shape of a 128-byte directory frame with its check byte. The sector
number it passes is the directory entry's word at `+0x20`, the Psy-Q
`DIRENTRY.head` field, divided by `64`. `MemCard_ReqCardInfo` is the seventh:
the matching controller now expresses that read directly as
`gMemCard_pDirEntries[index].head`, rather than rebuilding the SDK record
address by byte offset. Nothing in the executable or the `DATA` files calls
`MemCard_ReqCardInfo` or stores its address, so its name rests on its body and
on the code-`1` branch alone.

The controller's bounded workspace, absolute directory view, frame backing,
and inherited `MemCard_FindLoadedEntry` return-register ABI are documented in
[memory-card-work-controller.md](memory-card-work-controller.md). The request
prototypes live in `mem_card.h`; event initialization remains in
`io_event_helpers.h`. The grouped driver remains separate from the matching
request controller in `src/game/mem_card_driver.c`.

## Directory enumeration

Matching `MemCard_FindFiles` (`0x80044470`) formats a `bu%02X:%s` device
path and enumerates into caller-owned Psy-Q `DIRENTRY` records. It calls
`firstfile` for the initial record and `nextfile` for subsequent records,
accepting success only when each function returns the same record pointer it
was given.

The initial call and each failed advance allow five retries after the first
attempt. A successful `nextfile` resets that retry budget, advances by one
40-byte `DIRENTRY`, and increments the count. Enumeration stops at 15 records,
matching the usable block count on a memory card, and optionally stores the
final count through the caller's output pointer.

The path's name part is a pattern. `MemCard_DoLoadDirectory` passes `*` to
list the whole card, while the create path in the poll passes the new file's
own name and treats a zero count as the name being free.

### Shared request and directory contracts

The matching producer in `mem_card_driver.c` and matching controller in
`src/game/mem_card_driver.c` consume the request declarations in `mem_card.h`,
the directory API in `mem_card_directory.h`, and the existing event API in
`io_event_helpers.h`.
The earlier ownership migration removed eight driver-local globals and
fourteen globals plus three function prototypes from the then-candidate
controller. The matching source retains those owning-header contracts without
private declarations. These are identified
memory-card objects, so their owning headers, not `unmatched.h`, carry the
contracts.

`MEM_CARD_REQUEST_POLL_VIEW` used to keep two compiler views of these five
objects, and this entry recorded that it did so "without asserting that every
difference is independently load-bearing". That was then measured. Four of the
five differences were not load-bearing, the fifth was measured in the other
direction, and the guard is gone:

| Object | Declaration | What fixes it |
|---|---|---|
| `gMemCard_bRequest` | `s8` | the producer tests `>= 0` and stores `-1`; the poll's target reads it `lb` five times and `lbu` once, and that single read is spelled `*(u8 *)&gMemCard_bRequest` |
| `gMemCard_bRequestStep` | `u8` | the producer only stores it (`:120`); the poll's target reads it `lbu` eight times |
| `gMemCard_wRequestOffset`, `gMemCard_wRequestSize` | `u16` | the producer only stores them, four and three assignments; the poll's target reads them `lhu`, three times and four |
| `gMemCard_szRequestPath` | incomplete `char` array | neither side loads it; the producer's three `sprintf` calls already cast to `char *` |

A store is `sb` or `sh` whichever sign the declaration carries, so for the
middle three the producer's arm never reached an instruction, and the poll's
unsigned loads -- which feed the sector, seek, transfer-size and create-mode
arguments -- are now the only declaration. `gMemCard_bRequest` is the row that
did reach one: through the `u8` arm `gMemCard_bRequest = -1;` materialised
255, where its target has `addiu $v1, $zero, -0x1` before the `sb`, so
collapsing the guard moved the earlier candidate one instruction closer. The
five `(s8)` casts the poll needed over the `u8` arm are redundant under `s8`,
and removing them preserved that candidate object's bytes at that stage.
`gMemCard_pRequestBuf` stays an `s32` address, matching the request wrappers'
integer buffer ABI. Event-handle elements stay `long`, and both asynchronous
users select the existing volatile result arm. The poll's `D_8009B436`
countdown keeps its address-based name and plain `u8` view; no request fields
are combined into a speculative struct or given new storage.

The directory type has stronger evidence than adjacency: `firstfile` and
`nextfile` populate `gMemCard_aDirEntries`, `MemCard_DoLoadDirectory` assigns
that exact buffer to `gMemCard_pDirEntries`, and the helpers consume the
pointer. The LIBMCRD save dialog also supplies its directory buffer to those
same helpers after `MemCardGetDirentry`. Both helpers therefore accept the
existing Psy-Q `struct DIRENTRY`, not a new game-owned approximation.
`MemCard_CalcFreeBlocks` reads `entry->size` at `+0x18`;
`MemCard_FindEntry` compares `entry->name` at `+0`; both step one record rather
than adding 40 bytes. The shared header checks those offsets and the 40-byte
stride. Its SDK include goes through `libapi.h`, whose relative prerequisite
includes work with the existing named profiles.

The typed pointer reaches `MemCard_FindLoadedEntry` through its existing
header, and the poll now uses the producer's actual `MemCard_FindFiles`
prototype, including its optional `s32 *out_count` (the poll passes null).
The high-level dialog's `gMemCardDialog_DirectoryBuffer` remains an incomplete byte array with
explicit casts at its directory API boundaries; its storage/addressing is
not normalized. The low-level wildcard `D_8009AF7C` gains a shared declaration
consumed by its `.sdata` owner, but keeps its four-byte definition and remains
distinct from the high-level wildcard `D_8009AF70`.

The matching controller consumes all seventeen formerly bypassed dependencies
directly from headers. Before promotion, that migration emptied the candidate's
schema-2 private-extern dependency map without disabling it and preserved its
then-current object fingerprint
`34bde1bb8b430da186303a995a676dd02ba17a5f5df3d1bae5a7af5e542f6970`.
That historical declaration-only step did not change the candidate target,
profile, measured near miss, or assembly fallback. The subsequent matching
promotion retired that candidate and its fallback; the earlier failed
measurements remain historical evidence.

## Save payload staging

### Primary live-identity allowance and regional save state

`gSaveData_AllowIdentityBypass` is the existing raw byte at `0x8009B3D4`,
not a newly enabled feature, retry count or general I/O switch. The
new-game menu path stores one after its existing `NameEntry_Main` call.
The European effect-state handler also stores one after replacing the live
save's `duelist_code`. These producers establish an allowance for the
current live identity to reach the existing save-confirmation paths even
when the destination has no matching identity/sequence.

The flag does not write a card by itself. Save handling retains its
readback request, I/O result checks, new-card/missing-file branches,
free-block handling, duplicate-state fallback and overwrite question.
The nonzero allowance reaches the existing question without requiring
`SaveData_MatchesDuelistAndCurrentSequence` to succeed. The independent
modal flag `0x100` keeps its original alternate-save behavior; it is not
folded into this byte.

The clears are deliberately conditional. Successful ordinary primary load
clears the allowance only on the branch without modal flag `0x200`.
Successful ordinary primary save clears it and advances the save sequence
only without flag `0x100`. The paired unprompted-load and alternate-save
paths do not gain a clear. Cancelled or failed requests do not acquire a
new reset either. Preserve the raw `u8`, full zero/nonzero tests and
shared lifetime rather than introducing a Boolean, setter or new policy.

The two original `.data`/plain declaration arms move intact from
`mem_card.h` to `save_data.h`. Their numeric
`D_8009B3D4_IN_DATA` selector stays unchanged: the main-menu and European
effect writers keep their absolute views while modal readers keep their
plain views. The ownership-only checkpoint reproduces all 940 complete
compiler/MASPSX outputs raw-identically before naming.

`DuelEffect_UpdateIdentitySave` names the existing state-table entry four.
Europe's first callback invocation uses the separate handler-entry latch,
then keeps this exact order: one `rand()` call, original
`(rand() << 8) + 1` word store to the live duelist code, allowance store,
`SaveData_RequestWrite`, and the same invocation's `MemCardDialog_Poll`.
Any nonzero poll result, not only success one, ORs the existing completion
bit. `DuelEffect_UpdateState` consumes that bit on its later invocation,
clears effect state and returns idle; the callback itself does not perform
that clear or reinterpret cancellation/failure.

NA and Japan deliberately keep their eight-byte empty versions. No RNG,
request, polling or completion behavior is added to those stubs. The
other empty function, both contiguous definitions, fixed five-entry table,
all numeric state constants and original profiles remain. The European
body remains `0x74` bytes. This is regional identity/save behavior, not a
new common implementation or a replacement for name-entry identity
generation.

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Identity allowance | `0x8009B3D4` | `0x8009B2C4` | `0x8009C330` | `0x8009C750` |
| Regional save callback | `0x800289B4` | `0x80028794` | `0x800288E8` | `0x80028A2C` |

Only these identities and the exact declaration ownership change in this
checkpoint. Function inventory non-name fields, every other native/import
binding, initialized table positions, source/header paths, grouping,
layouts and compiler profiles remain. The complete accepted ledger gains
one global and one regional-family function evidence row. No tests,
fixtures, controls, traces or captures change; no new harness is added.

### Typed synchronous name-entry boundary

The new-game branch now includes the real overlay owner
`name_entry_main.h` and calls the already matched
`void NameEntry_Main(void)` export. The redundant numeric
`func_8016AA6C` declaration leaves `external_funcs.h`; no compatibility
wrapper, extra call or new body is introduced. The overlay definition
includes the same public header, while its starter-deck helper retains
its separate existing owner.

The existing `File_RequestNameEntryPackage` call still completes before
entry. Its stage loader transfers the final code phase to the address
stored at `gFile_pScreenBankBase`; the NA retail word is `0x80168000`, and the
unchanged phase size is fifteen sectors. All entry targets below are
inside that existing code window. The loader, wait, VRAM stages and
package constants do not change.

| Region | Resident binding | Existing module entry |
|---|---|---|
| NA | `0x8016AA6C` | Password `NameEntry_Main`, `0xCC` bytes |
| Japan | `0x8016A78C` | Password `NameEntry_Main`, `0xB4` bytes |
| UK / France / Germany / Italy / Spain | `0x8016A850` | Both existing Password A/B entries, `0xB4` bytes |

The six preexisting non-NA resident bindings keep their numeric target
and only replace the caller alias with the canonical name. NA adds one
`PROVIDE` in the existing resident overlay-import section, following its
Main Menu entrypoint pattern. All twelve module inventories, matching
manifests, exports and variant bodies remain unchanged.

`NameEntry_Main` still clears the `0x3000`-byte workspace, runs its
existing frame/RNG name-entry loop, builds the starter deck, XORs the
existing twelve name bytes and combines that checksum with the original
shifted word. Only a zero combined identity triggers the existing RNG
retry condition. The NA-only load-buffer trace and its absence in
Japan/Europe remain. This formula is not merged with the European
effect handler's different `(rand() << 8) + 1` formula.

The caller still stores both campaign scene values, the identity allowance,
load-status byte and next main mode in their original order after entry
returns. The asynchronous `Main_RunNameEntry` path continues to use
`NameEntry_Init`/`NameEntry_PollCompletion` directly; it is not redirected
through this synchronous workspace/deck initializer.

For this API checkpoint, all 940 complete resident compiler/MASPSX
outputs match after reversing only `NameEntry_Main` to the old caller
alias. No instruction/directive/relocation/filename is filtered.
The complete overlay source differs only by its owner-header include;
regional/overlay acceptance remains PR-side. No test or new harness is
added. This remains part of the larger unpublished save-identity bundle.

### Save-prompt scene context and payload handoff

`gScript_SavePromptSceneOperand` retains the second little-endian
halfword decoded by `Script_OpSavePrompt`. Its exact plain `u16`
declaration moves from the shared `script_state.h` to the command's
existing `script_op_save_prompt.h`. The neighboring event-script flags
stay in their original owner and location; no shared allocation or packed
command record is inferred from the adjacent addresses.

The first command invocation still performs both original cursor stores,
decodes the text ID and scene operand through the original byte accesses,
opens its initial box and returns. The context persists over subsequent
UI and modal-save phases; it is not replaced with a local, reader helper,
byte scene index or new context object.

The confirmed-choice path still writes the retained halfword to
`D_801D0000[(SAVE_DATA_HEADER_SIZE +
SAVE_DATA_CAMPAIGN_SCENE_INDEX_OFFSET) / sizeof(s16)]` *before* inspecting
the selected choice. The save case repeats that same store before
`SaveData_RequestWrite`. Both stores remain even though their values can
match. Other confirmed choices do not lose the first live-workspace write,
and cancelled/failed modal outcomes do not gain a rollback.

This destination is workspace `+0x7DC`, the live state at header offset
`0x200` plus state offset `0x5DC`. `campaign_scene_index` itself is a byte;
the halfword store also writes the following opaque `field_5DD`.
The operand's high byte has no newly inferred interpretation and is not
discarded or moved to another field. The signed-halfword workspace view
and separate interior relocation labels remain as before.

Only the existing build-deck handoff explicitly casts the operand to
`u8` for `gCampaignSceneIndex`. That active scene is distinct from the
two-byte saved-workspace write. No range check, mask, enum, scalar-member
replacement or store coalescing is added. The payload copy/build path
therefore receives exactly the live bytes the original command stages.

The command still waits for any nonzero `MemCardDialog_Poll` outcome and
restores its original choice/count/command-bit state without translating
the outcome. Preserve the one-pass choice loop, reused value local,
hoisted pointers, unbounded readiness loops and all regional input,
geometry, channel-stride, flag and preparatory-call differences.

| Retained operand | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Native address | `0x8009B2A6` | `0x8009B196` | `0x8009C200` | `0x8009C620` |

Only the two obsolete Japanese/European operand macros retire. The
unrelated French NA/UK-number alias remains. Ownership alone leaves all
940 complete compiler/MASPSX outputs raw-identical; the naming checkpoint
matches all 940 after reversing only this one identity. No instruction,
directive, relocation or filename is filtered. No test or harness is
added and all existing controls remain.

`SAVE_DATA_STATE_SIZE` fixes the live persistent state at `0x680` bytes,
beginning at `gDuel_awPlayerDeck` (`0x801D0200`). `SaveData_RequestWrite`
copies that state to `gSaveData_aTransferBuffer` (`0x801D3200`), then calls
`SaveData_BuildPayload` with the header staging base at `0x801D3000`.

The builder lays out:

| Staging offset | Size | Contents |
|---:|---:|---|
| `+0x000` | `0x200` | Header copied from the template at `0x801D4000`. |
| `+0x200` | `0x680` | Primary normalized game state. |
| `+0x880` | `0x680` | Duplicate copied from the completed primary state. |

Before the final duplicate copy, `SaveData_BuildPayload` normalizes the
primary state in a fixed order. At state-relative offsets, it clears the word
at `+0x400`, snapshots the runtime VBlank counter at `+0x408`, changes a
negative `gSD_bOutputType` to zero, and stores the resulting byte at `+0x5DE`.
It writes `gSaveDataSequence + 1` at `+0x404` in both the primary and duplicate
sequence slots, generates all three integrity records over the primary state,
and clears the `SAVE_DATA_RESERVED_TAIL_OFFSET` through state-end range
(`+0x628..+0x67F`). Its offset and `0x58`-byte size are derived from the end
of the tertiary mask record and `SAVE_DATA_STATE_SIZE`. The final `0x680`-byte
copy from `+0x200` to `+0x880` supersedes the early duplicate-sequence write
and makes the two state records identical, including their integrity words and
normalized tail.

The complete staged region through the duplicate is therefore `0xF00` bytes.
The subsequent `MemCardDialog_Request` call receives pointer `0x801D3200` and
length `0xD00`, exactly the contiguous pair of `0x680`-byte state copies. Its
final argument is the request selector. `MemCardDialog_Request` stores it
unchanged at `gMemCardDialog_StepIndex` through `MemCardDialog_Start` while setting the
active marker `gMemCard_wDialogFlags` to `MEM_CARD_DIALOG_FLAG_ACTIVE`.

The operation handlers add `MEM_CARD_DIALOG_FLAG_RESULT_READY` when their
result code is ready for display. Once the opening phase has completed,
`MemCardDialog_Update` creates the result object and records that with
`MEM_CARD_DIALOG_FLAG_RESULT_CREATED`; it clears the ready bit after the
result animation completes.

The exact callers use four selector values:

| Selector | Caller context | Buffer and length |
|---:|---|---|
| `0` | `SaveData_RequestLoad` | `0x801D3200`, `0x680` |
| `1` | Two paths in `SaveData_UpdateLoadPair` | `0x801D1200` or `0x801D2200`, each `0x680` |
| `2` | `SaveData_RequestWrite` | `0x801D3200`, `0xD00` |
| `4` | `SaveData_RequestTradeWrite` | `0x801D1880`, `TWO_PLAYER_SAVE_TRANSFER_SIZE` (`0x400`) |

The named callers establish selectors `0` and `2` as the normal single-save
load and write requests. Selectors `1` and `4` are the two halves of the
two-card workflow described next. The two working slots and their parallel
player-name buffers are separated by `TWO_PLAYER_SAVE_SLOT_STRIDE` (`0x1000`).
The staged write still fits within one `0x2000`-byte memory-card block; these
calls do not establish how the remaining on-card bytes are populated.

The canonical workspace, second-state, SJIS-source and trade-staging views
are documented in [save-pair-workspaces.md](save-pair-workspaces.md). Those
names retain the independent original providers and incomplete byte-array
declarations; the interior views do not introduce separate allocations.

The shared transition/modal callback byte is now named
`gDialog_bCallbackState`, separately from the controller operation state,
modal operation state and modal step index. Its preserved packed-state
contract is documented in
[dialog-runtime-controls.md](dialog-runtime-controls.md).

### Trade and two-player duel

Main-menu items `3` and `2` both need a save from each memory card slot.
`SaveData_UpdateLoadPair` (`0x8003F8D4`) is the shared per-frame loader: it
opens its own box with `MemCardDialog_CreateObject` and waits for the confirm
or cancel button. It then issues step `1` into `0x801D1200` for slot 1 and
into `0x801D2200` for slot 2 (`gMemCardDialog_Port = 0x10`). It returns `1` only when
the two saves have different duelist codes (`SaveData_HasSameDuelistCode`
fails). A matching pair shows the message in `gSaveData_PairMessageCode` and returns `2`.
The two wrappers differ in that message and in what they check afterwards:

| Function | Message | After a good pair | Callers |
|---|---:|---|---|
| `SaveData_UpdateTradeLoad` (`0x8003FCD8`) | `0x29` | nothing more | main-menu item `3`; `DebugMenu_UpdateTradeEntry`, which then enters mode `14`, `Main_RunTrade` |
| `SaveData_UpdateDuelLoad` (`0x8003FD14`) | `40`, `36` | all 40 deck slots nonzero in both saves, then both player names converted to glyph codes | main-menu item `2`; `DebugMenu_UpdateTwoPlayerDuelEntry`, which then enters mode `16`, the two-player duel setup |

`SaveData_UpdateDuelLoad` sets its message before it calls
`SaveData_UpdateTradeLoad`, whose one-shot latch then leaves it alone. That
reuse is why the trade wrapper sits under the duel one.

### Paired-load state, setup and presentation ownership

Three independent byte identities connect the shared loader, its nested
wrappers and the actual frontend/debug reset paths:

| NA identity | Name | Role |
|---|---|---|
| `0x8009B3EA` | `gSaveData_LoadPairState` | Low-nibble loader state plus the existing `0x80/0x40` progress bits |
| `0x8009B3ED` | `gSaveData_PairSetupState` | Shared wrapper entry guard, with only bit `0x80` established |
| `0x8009B3C0` | `gSaveData_PairMessageCode` | Raw paired-workflow presentation byte |

Their declarations belong to `two_player_save_setup.h`, which already
owns the duel/trade setup API, rather than the modal-card header.
`frontend_scene_states.c` explicitly includes that owner; the other
resident and Main Menu consumers already include it. All three identities
remain `u8`. Both original `.data`/plain declaration arms and their
numeric `D_8009B3EA_IN_DATA` / `D_8009B3ED_IN_DATA` selector spellings
remain unchanged. Debug entry stores retain their absolute-addressing
views; the shared loader keeps its plain view and Main Menu keeps G0.
Ownership alone leaves all 940 complete resident compiler/MASPSX outputs
raw-identical, before names change.

Frontend/debug entry paths still clear setup before loader state.
Neither the wrappers nor loader gains an automatic reset on return.
The duel wrapper still marks the shared setup byte and stores message 40
before calling the trade wrapper; that same guard prevents the nested
call from replacing it with `0x29`. All other incoming setup bits remain.
After a good pair, duel setup still stages state ten and message 36
before checking each deck slot, testing left before right and publishing
the existing invalid-side byte. Name conversion occurs only after those
checks. These messages are not a new enum or translated text.

The shared loader retains its prompt/open/close phases, cancellation
byte `0x82`, literal phase stores and all bit-`0x40` reuse. Its second
modal request still precedes the separate `0x10` port store through the
existing zero local/shared-return shape. Distinct duelist codes still
produce the original success return; other modal results propagate
without new translation.

The paired message path remains different from the modal bit-`0x20`
readiness path: it pumps until `TEXT_BOX_FLAG_DONE`, retains the hoisted
message/channel loads before its phase store, slides the box, then uses
the original regional channel-bit test before closing. Channel destroy,
display-object release, null store and return order remain. No new
timeout, ownership transfer, guard, state enum or callback is introduced.

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Loader state | `0x8009B3EA` | `0x8009B2DA` | `0x8009C35A` | `0x8009C77A` |
| Setup state | `0x8009B3ED` | `0x8009B2DD` | `0x8009C318` | `0x8009C738` |
| Message | `0x8009B3C0` | `0x8009B2B0` | `0x8009C35B` | `0x8009C77B` |

All seven Main Menu import sets name the two reset bytes at those exact
resident addresses. The six preexisting regional imports are renamed;
NA gains two explicit imports because named resident references are not
automatically inherited by its overlay linker. The Main Menu source
keeps every token after the two reset identities, and the existing
frontend witness changes only those identifiers: every assertion,
fixture, diagnostic and negative control remains.

All 940 complete resident outputs match after reversing only three
names, with no instruction/directive/relocation/filename filtering.
Complete changed C files retain every other byte except the one explicit
owner include. Every native function inventory, profile, manifest,
source/header path, grouped object, layout and initialized data remains.
The whole accepted semantic ledger gains three evidence/caveat rows.
No test or synthetic/generated-C/source-string harness is added;
regional/overlay/cloud validation stays PR-side. Final clean resident
retail matching remains mandatory.

The original paired-state proof precedes accepted #7649 integration.
Its complete modal operation/sync identities, exact ownership, native
shadow bindings, European legacy ASM prefixes and ordered evidence remain.
The combined comparison aligns only those four accepted sync names in
the saved resident baseline before reversing the three owned paired
names; every full output remains. A fresh clean gate covers the combined
tree. Accepted #7648 slot-animation sound naming was already in the
baseline; all overlay/cloud checks remain PR-side.

When the trade screen in the `main_menu` overlay has moved the traded card
counts between the two saves, it calls `SaveData_RequestTradeWrite`
(`0x8003FE14`). That call stamps the primary and secondary integrity records
of both slots at `0x801D1880` and `0x801D2880`, records the second slot in
`gMemCard_pSecondaryTransferCursor`, and issues step `4`.
`MemCardDialog_UpdateTradeSave`
(`0x8003EED0`) first re-reads the save on each card and refuses unless each
still has the duelist code that was loaded from it, trying the duplicate state
copy at `+0x680` once before it gives up. Then it writes the two `0x400`-byte
blocks in alternating `0x80`-byte chunks, one card then the other, so a failure
partway leaves the cards at most one chunk apart.

## Save integrity and successful-load application

The `0x680`-byte state contains three independently protected regions.
`SaveData_CalcCrc16` computes CRC-16/XMODEM with polynomial `0x1021` and a zero
initial value. `save_data.h` names the polynomial as
`SAVE_DATA_CRC16_POLYNOMIAL` and the width as `SAVE_DATA_CRC16_BITS`.
The value mask (`0xFFFF`) and high-bit test (`0x8000`) are derived from that
width as `SAVE_DATA_CRC16_MASK` and `SAVE_DATA_CRC16_HIGH_BIT`. The writers and
validator also use the same width when repeating the CRC in each seed word.
The writers duplicate each 16-bit CRC into two adjacent
halfwords, copy that CRC into both halves of the two-word mask state, and fill
the region's mask words by repeatedly calling `SaveData_NextMaskWord`:

| Region | CRC input | Generated mask words | Duplicated CRC |
|---|---|---|---|
| Primary | `+0x000..+0x33F` (`0x340` bytes) | 15 words at `+0x340..+0x37B` | `+0x37C`, `+0x37E` |
| Secondary | `+0x380..+0x3EB` (`0x6C` bytes) | 4 words at `+0x3EC..+0x3FB` | `+0x3FC`, `+0x3FE` |
| Tertiary | `+0x400..+0x603` (`0x204` bytes) | 8 words at `+0x608..+0x627` | `+0x604`, `+0x606` |

`SaveData_SetMaskSeed` writes the same 32-bit value to both mask-state words.
`SaveData_NextMaskWord` treats `gSaveData_dwMaskStateLow` as the low word and
`gSaveData_dwMaskStateHigh` as the high word of its private recurrence,
updates both, and returns the new low word. The integrity paths seed each word
with the 16-bit CRC repeated in both halves. Fleet's `rng_seed` label describes
that recurrence mechanism only: these words are used exclusively by
save-integrity generation and validation and are separate from the game's
`gRand_dwSeed`.
`SaveData_WritePrimarySecondaryIntegrity` writes the primary and secondary
records, while `SaveData_WriteTertiaryIntegrity` writes the tertiary record.
`SaveData_ValidateIntegrity` recomputes all three CRC seeds and compares every
generated mask word in descending address order; it returns false on the first
mismatch.

`SaveData_RequestLoad` reads one `0x680`-byte state into `0x801D3200`.
`SaveData_PollLoad` waits for the request result and, only when the result is
`1`, copies that state over the live block at `gDuel_awPlayerDeck` before
calling `SaveData_ApplyRuntimeState`. That final step rebuilds the player-name
glyph string from save offset `+0x40C`, restores the save sequence at `+0x404`,
the VBlank counter at `+0x408`, and `gCampaignSavedSceneIndex` at `+0x5DC`
into the runtime `gCampaignSceneIndex`, and reapplies the saved sound output
type at `+0x5DE` when `gSD_bOutputType` is negative.

The two-save comparison path first uses `SaveData_HasSameDuelistCode` on the
32-bit field at `+0x334`. `SaveData_MatchesDuelistAndCurrentSequence` accepts
the pair only when that code matches and the second save's sequence at `+0x404`
equals the current `gSaveDataSequence` value.

## Evidence boundary

The descriptor values, event specifications, mode, and API prototypes are
direct matches for the imported Psy-Q 4.6 headers. The registration order,
callback mapping, result values, retry condition, and teardown are established
by local matching C. `MemCard_InitIOEvents` and `MemCard_CloseIOEvents` include
`libapi.h`, use its event interfaces, and spell the descriptors,
specifications, and interrupt mode with the SDK constants. This identifies
the runtime contract without claiming that the original game source used
those macro spellings rather than their numeric values.

## Modal operation state and SDK sync ownership

The internal operation byte and three independent sync words now belong
to the existing modal owner, without introducing a packed result record.

| NA identity | Name | Retained declaration |
|---|---|---|
| `0x8009B3EB` | `gMemCardDialog_OperationState` | `u8` |
| `0x8009B3BC` | `gMemCardDialog_SyncStatus` | `s32` |
| `0x8009B3F0` | `gMemCardDialog_SyncCommand` | `s32` |
| `0x8009B3F4` | `gMemCardDialog_SyncResult` | `s32` |

The operation byte, sync status and sync result declarations move from
`src/unmatched.h` to `src/game/mem_card.h` with exactly those plain
types. The command word already belonged there. No qualifier, size,
pointer cast, small-data arm, initialized definition or SDK prototype
changes. Ownership alone leaves all 940 complete compiler/MASPSX outputs
raw-identical before naming.

### Internal state is not the callback index or shared entry latch

Load/save/trade work keeps its existing low-nibble switch, packed
`0x80`/`0x40` progress bits, whole-byte assignments, independent cases,
fallthrough and invalid-state behavior. Reused flag constants do not
turn these per-case meanings into a new global enum.

`gMemCardDialog_StepIndex` selects the outer modal callback. The separate
shared `D_8009B3C1` guards each callback's initial setup. Prompted load/save
seed internal state zero, unprompted load seeds one, trade seeds zero and
the existing regional save initializer retains its own offset/state/flag
stores. `Start` does not reset this operation byte directly; the selected
callback initializes it later under that shared guard. The legacy
`gMemCardWork_OperationState` and modal flag halfword remain distinct.

### Poll return, command and result keep independent lifetimes

The authoritative `src/psyq/libmcrd.h` prototype names the two output
arguments `cmds` and `rslt`. The modal pump keeps mode one, the two
separate existing `PSXLONG *` casts, the full return store and the later
comparison of its local against one. Only that original completion value
clears `IO_PENDING` and reaches dispatch; other raw returns do not gain
a translated error, fallback, result reset or new retry policy.

Operation handlers retain every command/result test, including the raw
command equality with two in save handling and all result switches.
The byte dialog outcome remains independently published by operation
handlers and later exposed by `Poll`. Likewise the legacy event result
and the Japanese SDK probes' local command/result variables are not
merged with these globals. No new enum or interpretation of otherwise
unproved result values is imposed.

### European command shadow and ASM labels remain exact

NA command/result addresses happen to be adjacent. In the UK the modal
command is `0x8009C320` and result is `0x8009C328`; the legacy work
controller's separate out-parameter at `0x8009C324` lies between them.
Continental Europe has the same relationship at
`0x8009C740/0x8009C744/0x8009C748`.

The two regional wrappers' complete legacy out-parameter prefixes,
historical explanatory comments and
`asm("D_8009B3F0")` strings remain byte-identical. That ASM label still
binds the legacy word, not `gMemCardDialog_SyncCommand`. Their former
`gEuropean_D_8009B3F0` modal alias becomes the canonical command binding;
the distinct direct `D_8009B3F0` linker name stays untouched. The same
address-based discipline retains the unrelated direct UK status alias
at `0x8009C32C`, while the modal sync status is `0x8009C314`.
No adjacent-word struct, storage merge or broad textual label rewrite is
permitted by this evidence.

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Internal operation | `0x8009B3EB` | `0x8009B2DB` | `0x8009C351` | `0x8009C771` |
| Sync status | `0x8009B3BC` | `0x8009B2AC` | `0x8009C314` | `0x8009C734` |
| Sync command | `0x8009B3F0` | `0x8009B2E0` | `0x8009C320` | `0x8009C740` |
| Sync result | `0x8009B3F4` | `0x8009B2E4` | `0x8009C328` | `0x8009C748` |

All unrelated Japanese, French and European aliases remain. Only seven
obsolete regional sync macros retire. All 940 complete outputs match
after reversing only the four owned identities; the complete changed C
files otherwise differ only by those names and macro removals. No
instruction, directive, relocation or filename is filtered. Every native
function inventory, compiler profile, manifest, source/header path,
grouping, layout and initialized data remains. The whole accepted semantic
ledger gains four evidence/caveat rows.

No tests, fixtures, controls, traces or captures change, and no test or
synthetic/generated-C/source-string harness is added. Clean retail
matching remains mandatory; regional/cloud validation stays PR-side.

The original sync proof precedes accepted #7647 integration. Its full
transfer-field identities, ownership/types, native bindings, unsigned
narrowing/wrap and complete ordered evidence are retained. The combined
comparison explicitly aligns only those five accepted transfer names in
the saved baseline before reversing the four owned operation/sync names.
Every complete artifact remains; a fresh clean gate covers the integrated
tree. Accepted #7646 hardware/debug-loop names and #7645 CD-header
retirement were already part of the baseline.

## Modal transfer request fields and ownership

The shared filename, card port, transfer halfwords and block byte now have
confirmed modal-request identities. These are still independent globals,
not a packed descriptor, new allocation or immutable request snapshot.

| NA identity | Name | Retained declaration |
|---|---|---|
| `0x800EFE18` | `gMemCardDialog_FileNameBuffer` | unsized `u8 []` |
| `0x8009B3F9` | `gMemCardDialog_Port` | `u8` |
| `0x8009B3C2` | `gMemCardDialog_TransferSize` | `u16` |
| `0x8009B3C4` | `gMemCardDialog_TransferOffset` | `u16` |
| `0x8009B3DC` | `gMemCardDialog_BlockCount` | `u8` |

The port and two halfword declarations move from `src/unmatched.h` to
their existing consumer owner, `src/game/mem_card.h`, with their exact
plain types. The filename and block byte already belonged there. No
qualifier, initialized definition, SDK signature, pointer view or
small-data arm changes. The ownership-only stage leaves all 940 complete
compiler/MASPSX artifacts raw-identical before any symbol promotion.
The historical central-header width evidence stays, with an ownership
cross-reference; unrelated declarations and tentative internal state stay.

### Staging preserves two different truncation boundaries

`MemCardDialog_Request` retains this order: copy the filename, store port
zero, store the low halfword of its full-word size argument, store offset
`0x200`, compute/store the block byte from the original full-word
`(size + MEM_CARD_BLOCK_SIZE - 1) / MEM_CARD_BLOCK_SIZE` expression, store
the primary transfer cursor, then call `MemCardDialog_Start`.

In particular, block count is not recomputed from the already narrowed
transfer-size halfword. The public arguments remain full words, the
globals remain `u16`/`u8`, and the original casts, signed expression,
store truncation and unvalidated input behavior remain. No clamp,
positive-size precondition, overflow workaround or new rejection is added.

The filename remains an unsized byte array, copied by the same existing
`strcpy` and passed through the existing SDK casts. The next tracked NA
name thirty-two bytes later does not establish a safe or required buffer
bound. No new bound, copy policy, encoding conversion or storage is
invented.

### Card port is not the text-channel index

The raw port byte is passed to `MemCardAccept`, directory lookup and
read/write calls; `(port >> 4) + 1` selects the displayed card number.
For its second save, the pair-load path calls `Request`, then writes
`0x10` after that call has reset the port to zero. Trade setup/validation and
write-back retain their XOR-`0x10` transitions and whole-byte tests.
Other existing save calls still pass their literal zero port.

This is neither `gMemCardDialog_ChannelIndex` nor the legacy event
driver's `gMemCard_bChannel`. No Boolean type, `0/1` conversion,
per-request copy or new range policy is imposed on its complete byte.
The existing paired-load zero local and shared-return control-flow shape
are retained.

### Mutable offset and size keep unsigned-halfword wrap

Alternate-load recovery retains its stored offset increment by
`SAVE_DATA_STATE_SIZE`. The trade validation read retains its separate
`offset + SAVE_DATA_STATE_SIZE` full-word call argument rather than a
new intermediate halfword store. Header-inclusive save creation retains
offset zero, primary-cursor subtraction by `SAVE_DATA_HEADER_SIZE` and
size addition by that same amount, under the original flag condition.
The existing Japanese/European save initializer separately clears the
offset. No cursor, branch or store is merged with these cases.

Trade writes still alternate the original primary/secondary buffers in
`0x80`-byte chunks. Only after both ports complete the current chunk does
the runtime advance both pointers, add `0x80` to the stored offset,
subtract `0x80` from the stored size and test that stored size for exactly
zero. No less-than test, early-zero exit, saturation, widened remaining
counter or forced final partial chunk is added. Initial zero,
nonmultiple, oversized and wrapping inputs retain the original behavior.
The mutable size is not an immutable initial request length.

### Native identities and conservation

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Filename | `0x800EFE18` | `0x800ED4F0` | `0x800F0890` | `0x800F0CB0` |
| Port | `0x8009B3F9` | `0x8009B2E9` | `0x8009C32D` | `0x8009C74D` |
| Size | `0x8009B3C2` | `0x8009B2B2` | `0x8009C332` | `0x8009C752` |
| Offset | `0x8009B3C4` | `0x8009B2B4` | `0x8009C334` | `0x8009C754` |
| Blocks | `0x8009B3DC` | `0x8009B2CC` | `0x8009C348` | `0x8009C768` |

Only those exact native bindings change names. Every function inventory
remains byte-identical; unrelated aliases, SDK views, regional branch
differences, profiles, manifests, source/header paths, grouping, layouts
and initialized data remain. The complete accepted semantic ledger gains
five high-confidence evidence/caveat rows for the formerly tentative
names, without rewriting existing records.

After the raw ownership proof, all 940 complete artifacts match after
reversing only the five promoted identifiers. The entire changed C file
is otherwise byte-identical. No instruction, directive, relocation or
filename is filtered. No tests, fixtures, controls, traces or captures
change, and no test or synthetic/generated-C/source-string harness is
added. Final clean retail matching remains mandatory; regional/cloud
validation stays PR-side.

The original transfer proof precedes accepted #7644 integration. Its modal
table/index/outcome/channel/message identities, exact native alias
boundaries, corrected readiness prose and full ordered evidence are
preserved. The combined comparison explicitly aligns only those five
accepted modal names in the saved baseline before reversing the five
promoted transfer names. Every whole output remains; a fresh clean gate
covers the integrated tree. Accepted #7643's four header moves and
historical documentation were already part of this component's baseline.

## Modal directory, retry and text-argument workspace

Three remaining module views connect directory enumeration, duplicate
validation and the existing card-number text inputs:

| NA identity | Name | Retained declaration |
|---|---|---|
| `0x800EFBC0` | `gMemCardDialog_DirectoryBuffer` | unsized `u8 []` |
| `0x8009B3EC` | `gMemCardDialog_RetryCounter` | `u8` |
| `0x801D5648` | `gMemCardDialog_CardNumberArg` | unsized `s32 []` |

The directory backing is still passed through each original
`struct DIRENTRY *` cast to `MemCardGetDirentry`, then to lookup/free-block
helpers beside the existing local count. The SDK's actual record and
maximum-fifteen argument stay. There is no new typed array, bound,
allocation, clearing pass, count validation or scan policy. This modal
buffer is not the legacy event driver's `gMemCard_aDirEntries` at
`0x800F2888` or its `gMemCard_pDirEntries` pointer. The distinct native
layouts do not justify a shared work-area struct.

The retry byte deliberately has more than one meaning. Save/trade
duplicate-record validation clear it, test whether a fallback has already
been attempted, increment it and read the duplicate at the derived
offset. The Japanese/European save SDK-probe phase instead seeds three
and predecrements before deciding whether to reissue its original probe.
NA retains its seed-three store without those regional polling calls.
All original byte wrap, separate tests, API/local-result handling and
message branches remain. No remaining-retries type, unified retry helper
or new initialization is introduced.

The card-number argument remains the modal producer's view of shared
text staging at `D_801D5608 + 0x40`. Existing branches write
`(gMemCardDialog_Port >> 4) + 1` through element zero before their text
requests. That does not make the word permanently card-owned:
`TextStagingValues.deck_validation.invalid_side` and the rank-row view
overlap the same address. No producer, including the distinct deck
validation path, is redirected through another view.

The separate incomplete `s32` symbol stays non-small-data under the
existing G8 profile. Replacing it with a scalar or folding its stores into
the union member would change relocation materialization and register
lifetimes. Its new name changes neither the numeric address nor the
module-specific scope of that view, and asserts no array extent or new
allocation. See [text-staging.md](text-staging.md) for the overlapping
producer contracts.

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Directory | `0x800EFBC0` | `0x800ED298` | `0x800F0638` | `0x800F0A58` |
| Retry byte | `0x8009B3EC` | `0x8009B2DC` | `0x8009C310` | `0x8009C730` |
| Card-number argument | `0x801D5648` | `0x801D5648` | `0x801BF8CC` | `0x801BF8CC` |

Japan retains the original numeric fallback for the interior argument:
its shared text base remains `0x801D5608` and the measured offset remains
`0x40`. European staging uses base `0x801BF88C` and the same offset.
Only the two obsolete Japanese directory/counter macros retire.
Unrelated French NA-number aliases and every other native/import binding
remain.

All 940 complete resident compiler/MASPSX outputs match after reversing
only these three identifiers, without instruction/directive/relocation
or filename filtering. Complete changed C files otherwise differ only
by the names and two retired macros. All declaration spellings, native
function inventories, profiles, manifests, source/header paths, grouping,
layouts, initialized data and generic staging views remain. The entire
accepted semantic ledger gains exactly three evidence/caveat rows.

No tests, fixtures, controls, traces or captures change, and no test or
synthetic/generated-C/source-string harness is added. Final clean retail
matching remains mandatory; regional/cloud validation stays PR-side.

The original auxiliary-workspace proof precedes accepted #7651
integration. Its paired-state declarations/data selectors, nested setup
lifetime, full frontend/debug reset behavior, seven Main Menu import sets
and identifier-only witness updates remain intact. The combined comparison
aligns only those three accepted paired names in the saved resident
baseline before reversing the three owned workspace names. Every complete
output remains, followed by a fresh clean retail gate.

## PAL script prompt and modal providers

Three existing canonical interfaces now carry their PAL native assembler
bindings in the owning headers rather than private wrapper name macros:

| Interface | UK address / bytes | France, Germany, Italy, Spain address / bytes |
|---|---|---|
| `Script_OpSavePrompt` | `0x8002EF3C` / `0x56C` | `0x8002F128` / `0x56C` |
| `MemCardDialog_UpdateLoad` | `0x8003E3E4` / `0x344` | `0x8003E5E4` / `0x344` |
| `MemCardDialog_Update` | `0x8003F568` / `0x2C8` | `0x8003F768` / `0x2C8` |

`VERSION_EUROPE` selects explicit `extern void(void)` bindings to native
`func_8002EF3C`, `func_8003E3E4` and `func_8003F568`. They remain the actual
compiled owners, not absolute aliases or replacement definitions. The NA
and Japanese declarations keep their existing signatures and labels.
Five inventories change only these three name fields; existing addresses,
sizes, statuses, modules and old notes remain intact.

The prompt consumes two little-endian halfwords only on its initial
command phase and keeps the existing retained scene operand. Its `0x80`
phase polls the modal dialog before restoring choice count four, choice
zero and control zero. Every confirmed choice stages the scene halfword
before dispatch, including slide-out/cancel; the save arm repeats that
store before `SaveData_RequestWrite`. Build-deck, return-to-menu,
incomplete-deck and slide-out paths remain separate. Preserve the initial
busy latch, one-pass choice loop, synchronous object-creation pumps,
PAL box geometry/flags and original signed slide narrowing. No script
stream, choice timing or save behavior is changed.

The modal update remains the sole pump called by `MemCardDialog_Poll`.
Polling returns zero while modal flags survive, not the outcome byte.
Closing still destroys text, releases the display object and nulls its
pointer before a later null-object invocation clears the flags. PAL
session startup keeps its extra helper after `MemCardStart`; shutdown
keeps `MemCardStop` before its regional helper. The SDK sync return,
command output, result output and final dialog outcome stay distinct.

Both load entry steps still seed operation state zero or one and call the
same load machine. Preserve its fallthroughs, shared I/O-pending tail,
retry/validation paths, separate error targets and outcome one/three
stores. The unsized card-number argument, absolute choice view and
volatile flag stores retain their old addressing and order. The PAL
controller's out-parameter and modal sync word still have different
addresses despite their historical `D_8009B3F0` spellings; neither is
merged, retyped or rebound.

Only the five private name macros are removed from the three existing
PAL wrappers. Every shared function body, private layout, initialized
command/step table, other wrapper selector, compiler profile, manifest,
symbol/import map, save workspace and existing test/control is unchanged.
The actual native comparison covers eleven unique source/profile units:
the seven regional prompt/dialog units, both deck-branch consumers and
the two NA initialized command/step tables. Complete compiler assembly,
MASPSX output, symbol tables and disassembly/relocations remain identical.
The three PAL functions retain all `0xB78` instruction bytes per image.

The clean complete NA resident retains retail SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
This is not local full PAL-image execution; ordinary applicable exact-head
regional image/regression gates remain mandatory. No synthetic/generated-C
or source-string harness, new testcase or local cloud suite is added.

## Regional alternate-save step and Japanese Options request

The Japanese and PAL step-three callback is
`MemCardDialog_StepAlternateSave`, not NA's empty `MemCardDialog_StepNone`.
It latches the existing callback entered bit once, clears both the transfer
offset and operation state, raises modal flag `0x100`, and calls
`MemCardDialog_UpdateSave` on every pass, including the first. Native
declarations belong to `mem_card_dialog.h`, selected by the existing regional
defines; no replacement owner, adapter or runtime gate is introduced.

| Function | Japan | UK | France / Germany / Italy / Spain |
| --- | --- | --- | --- |
| `MemCardDialog_StepAlternateSave` | `0x8003E56C` | `0x8003EF9C` | `0x8003F19C` |
| `SaveData_RequestOptionsWrite` | `0x8003EFA8` | not built here | not built here |
| `SaveData_RequestLoad` | `0x8003EEA8` | existing canonical name | existing canonical name |

The alternate step retains all `0x48` bytes per regional image. The Japanese
Options request retains `0x30` bytes, and its already-canonical ordinary load
request retains `0x3C`. Only the latter's stale Japanese inventory name
changes: its C definition, shared declaration and native linker symbol
already agree on `SaveData_RequestLoad`.

`SaveData_RequestOptionsWrite` takes no arguments. Its existing
`data_transfer_request.h` owner gives the Japanese-only declaration the
native `func_8003EFA8` assembly label. The real `Options_Update` caller
sets its entered bit before calling once in state two, polls
`MemCardDialog_Poll`, stays there on zero, and otherwise falls through
state three to restore the active state. This is not an unconditional
per-frame request, a success-only test or an exit from Options.

The request itself passes the fixed pointer `0x80140000`, size `0x8000`,
existing file-name pointer `D_8001032C` and step three. It does not copy the
ordinary `0x680`-byte save state or build its replicated payload. Options
identifies the observed caller, not a recovered format for those 32 KiB:
no settings-only payload, regional equivalent or PAL reachability is claimed.
The ordinary load request still clears its original numeric status byte
and requests `SAVE_DATA_STATE_SIZE` through step zero.

`MemCardDialog_Request` first seeds offset `0x200`; the alternate step
subsequently resets it to zero. Flag `0x100` retains its distinct messages,
SDK-probe route, read-back bypass and no header-pointer/size adjustment
on file creation. Successful alternate saves still neither clear the
independent identity allowance nor advance the normal save sequence.
All request, write, retry, failure, cancel and delayed modal-outcome paths
remain unchanged. A canonical name does not enable this request outside Japan
or make the PAL callback reachable from an Options screen.

Two definition identifiers and the real Japanese caller identifier change;
all function bodies otherwise remain identical. The two PAL wrappers lose
only the obsolete private callback mapping and its comment. Explicit native
labels preserve original definitions, calls and generated table references.
NA keeps its empty step-three function and initialized table unchanged.
The Japanese compact text-channel stride and all SDK aliases remain.

Native conservation uses seven actual source/profile routes: four complete
regional dialog units, the real NA/Japanese Options units and the initialized
NA dialog-step-table owner. Dialog/Options use named `gcc_2_8_1_g8_split`;
the data owner keeps `gcc_2_8_1_g0`. Complete compiler/MASPSX assembly,
native symbols and disassembly/relocations are compared. Coverage is
27 functions / 9,924 instruction bytes for NA, 28 / 10,368 for Japan and
22 / 9,140 for each PAL image. No synthetic/generated-C/source-string
harness or new testcase is added; regional execution stays on the PR.
