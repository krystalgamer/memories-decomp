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
The one caller, `func_8003DC1C`, passes `1` and calls `MemCard_InitIOEvents`
immediately afterwards.

## High-level LIBMCRD dialog lifecycle

The matching modal state machine `MemCardDialog_Update` (`0x8003F454`) uses the
separate high-level interface from `libmcrd.h`. On first entering its active
state, it calls `MemCardStart`, creates the dialog object, and moves that
object into place before dispatching the selected operation through
`D_80090F9C[D_8009B3DE]`.

When state bit `0x1000` marks a pending high-level command, the update calls:

```c
MemCardSync(
    1,
    (long *)&D_8009B3F0,
    (long *)&D_8009B3F4
);
```

The two output words retain their address-based names because this caller
does not itself interpret their command and result values. It stores the
function return in `D_8009B3BC` and advances only when that return is `1`;
otherwise the modal update leaves the pending state intact and polls again.

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
byte `D_8009B3EF`. The update opens the box with `MemCardDialog_CreateObject`
and slides it in and out with `MemCardDialog_StepSlide`. The operation
handlers report through `MemCardDialog_SetMessage`, which replaces the
message `D_8009B3C6` and raises `MEM_CARD_DIALOG_FLAG_RESULT_READY` together
with the caller's presentation bits. Bit `0x20` makes the update pump the text
box until it reports that it has finished, and the handler then reads the
player's answer from `gDialog_bChoice`.

The step table `D_80090F9C` has five entries:

| Step | Entry | Runs |
|---:|---|---|
| `0` | `MemCardDialog_StepLoad` | `MemCardDialog_UpdateLoad` from state `0`, the confirmation prompt |
| `1` | `MemCardDialog_StepLoadUnprompted` | `MemCardDialog_UpdateLoad` from state `1`, with dialog flag `0x200`, which swaps the accept and success messages |
| `2` | `MemCardDialog_StepSave` | `MemCardDialog_UpdateSave` from state `0`, also a confirmation prompt |
| `3` | `MemCardDialog_StepNone` | nothing; no caller selects it |
| `4` | `func_8003EED0` | reads the save from each card slot, checks both with `SaveData_HasSameDuelistCode`, and writes them back |

`MemCardDialog_UpdateLoad` accepts the card with `MemCardAccept`, finds the
file with `MemCardGetDirentry` and reads it with `MemCardReadFile`.
`MemCardDialog_UpdateSave` adds the directory and free-space checks
(`MemCard_FindEntry`, `MemCard_CalcFreeBlocks`), an optional `MemCardFormat`,
`MemCardCreateFile`, and `MemCardWriteFile`. Step `4` keeps its address: it
belongs to the two-save workflow described under the selector table below,
whose operation has no settled name.

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
after the complete set is active. It also resets three surrounding
memory-card state values at `D_8009B43E`, `D_8009B44E`, and `D_8009B444`;
the first two broader roles remain address-based. `D_8009B444` is the current
directory-entry buffer: `MemCard_DoLoadDirectory` points it at `D_800F2888`,
stores the loaded entry count in `D_8009B440`, and passes both to
`MemCard_CalcFreeBlocks`. `MemCard_FindLoadedEntry` forwards the same
buffer/count pair to `MemCard_FindEntry` when it searches for a
caller-supplied filename.

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
`_card_write` are prepared against `D_800F2AF0`, the `HwCARD` set four handles
further on.

Matching `MemCard_ClearCard` (`0x80044038`) shows how the result drives
retries. It prepares the `HwCARD` set, calls `_card_clear(chan)`, waits while
the result is negative, and retries only result `1` (timeout), with at most
ten attempts. It has no caller either; the request path runs the same clear
without blocking inside `MemCard_DoLoadDirectory`, described next.

## Card check and directory load

`MemCard_DoLoadDirectory` (`0x80044608`) is the first stage of every request
the poll at `0x80044838` runs. It advances one step per call on the sub-state
byte `D_8009B43D`, returns `-1` while it has started a new event and is
waiting, and otherwise returns the shared result:

| Sub-state | Waits on | Next |
|---:|---|---|
| `0` | `_card_info` | Timeout re-issues `_card_info` until the retry byte `D_8009B43C` runs out, and an error finishes with `2`. A completed info on a card already listed (bit `0x80` of `D_8009B44E`) finishes with `0`, unless the request is `8`. Otherwise, and on a new-card event, request `1` finishes with `3`; every other request clears the card. |
| `1` | `_card_clear` | An error retries the clear. Completion starts `_card_load` and moves to `2`. |
| `2` | `_card_load` | An error retries the load. Then bit `0x80` is set, and on success `MemCard_FindFiles` lists `*` into `D_800F2888` and `MemCard_CalcFreeBlocks` stores the free count in `D_8009B438`. |

A new-card result at the end of the load stage is returned as `4` rather than
`3`; that conversion is internal state-machine bookkeeping, not a fifth event
callback. `D_8009B44E` is cleared by `MemCard_InitIOEvents` and nothing else
clears bit `0x80`, so once a listing has been attempted, a plain completed info
reuses it. A new-card event still forces the clear and reload, and so does
request `8`, the one that goes on to add a file.

## Request slot

The low-level driver runs one request at a time. `MemCard_BeginRequest`
(`0x800440B4`) claims the slot: it returns `0` while `D_8009B43E` is
nonnegative, which means an earlier request is still pending, and otherwise
stores the channel in `D_8009B437`, the request code in `D_8009B43E`, resets
the retry byte `D_8009B43C` to `10` and both sub-state bytes to `0`, and sets
`gMemCard_nIOResult` to `-1`. Every wrapper below calls it first and returns
its refusal unchanged; on success the wrapper stages its arguments in the
shared request globals, starts `_card_info(chan)` against
`gMemCard_aIOEventHandles`, and returns `1` without waiting.

The poll at `0x80044838` is still assembly (the tracked candidate is
`src/candidates/func_80044838.c`), but its dispatch fixes what each code does.
It returns `-1` while the slot is idle and `0` while the request is still
running; when it finishes it writes the code and the result through its two
output pointers, puts `D_8009B43E` back to `-1`, and returns `1`.

| Code | Wrapper | Staged arguments | What the poll does |
|---:|---|---|---|
| `1` | `MemCard_ReqCardInfo(chan)` | none | Stops after the `_card_info` stage of `MemCard_DoLoadDirectory`, so it reports the card state without clearing it. |
| `2` | `MemCard_ReqLoadDirectory(chan)` | none | Finishes `MemCard_DoLoadDirectory`, which lists `*` into `D_800F2888` and counts free blocks into `D_8009B438`. |
| `3` | `MemCard_ReqReadFile(chan, name, buf, offset, size)` | path, `D_8009B430`, `D_8009B44C`, `D_8009B434` | `open` with `O_RDONLY \| O_NOWAIT`, `lseek` to the offset, `read`. |
| `4` | `MemCard_ReqWriteFile(chan, name, buf, offset, size)` | path, `D_8009B430`, `D_8009B44C`, `D_8009B434` | The same with `O_WRONLY \| O_NOWAIT` and `write`. |
| `8` | `MemCard_ReqCreateFile(chan, name, blocks)` | path, `D_8009B434` | Result `7` when `D_8009B438` plus the block count reaches `16`, `6` when `MemCard_FindFiles` already finds the name, otherwise `open` with `O_CREAT` and the block count in the high half of the mode. |
| `11` | `MemCard_ReqReadSector(chan, buf, sector)` | `D_8009B430`, `D_8009B44C` | `_card_read(chan, sector, buf)`, bypassing the file system. |
| `12` | `MemCard_ReqWriteSector(chan, buf, sector)` | `D_8009B430`, `D_8009B44C` | `_card_write(chan, sector, buf)`. |

The path is `D_800F2B00`, formatted as `bu%02X:%s` from the channel and the
name. `D_8009B44C` is the byte offset for the file codes and the sector
number for the raw codes, and `D_8009B434` is a byte count for reads and
writes but a block count for creation; the names in the table are the
wrappers' parameters, not roles for the globals.

The code-`8` capacity test is recorded as the retail instructions have it
(`addu`, `slti 0x10`) rather than interpreted. `D_8009B438` is the *free*
count `MemCard_CalcFreeBlocks` returns, so the sum does not compare the
request with the space left; the one caller makes its own free-count check
before it issues the request, as described below.

`MemCard_ReqLoadDirectory` is the one wrapper that blocks. Before it leaves
request `2` for the poll it runs `_card_info(chan)` against the primary
handle set, `_card_clear` against the alternate set using channel byte
`D_8009B437`, then `_card_load(chan)` against the primary set again,
resetting the shared result before every stage and waiting for a nonnegative
event result after each one. It does not reinterpret those three results.

Six of the seven wrappers have one caller, the unmatched `func_8003DC1C`, and
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
nothing in the executable or the `DATA` files calls it or stores its address,
so its name rests on its body and on the code-`1` branch alone.

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

## Save payload staging

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
unchanged at `D_8009B3DE` through `MemCardDialog_Start` while setting the
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
| `1` | Two paths in assembly `func_8003F8D4` | `0x801D1200` or `0x801D2200`, each `0x680` |
| `2` | `SaveData_RequestWrite` | `0x801D3200`, `0xD00` |
| `4` | `func_8003FE14` in the two-player save setup | `0x801D1880`, `TWO_PLAYER_SAVE_TRANSFER_SIZE` (`0x400`) |

The named callers establish selectors `0` and `2` as the normal single-save
load and write requests. Selectors `1` and `4` belong to distinct parts of the
two-save workflow, but their broader state-machine operation names remain
unassigned. The two working slots and their parallel player-name buffers are
separated by `TWO_PLAYER_SAVE_SLOT_STRIDE` (`0x1000`). The staged write still
fits within one `0x2000`-byte memory-card block; these calls do not establish
how the remaining on-card bytes are populated.

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
