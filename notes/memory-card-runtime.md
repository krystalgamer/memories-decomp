# Memory-card Runtime Events

The resident memory-card layer uses Psy-Q BIOS events to convert asynchronous
card notifications into a small polling result. Matching
`MemCard_InitIOEvents` creates eight event handles in the array at
`gMemCard_aIOEventHandles` (`0x800F2AE0`): the same four event specifications
are registered once for `SwCARD` and once for `HwCARD`.

## Subsystem startup

Matching `func_80043E30` establishes the game-owned startup order. It forwards
its mode argument to `InitCARD`, calls `StartCARD`, passes zero to
`ChangeClearPAD`, and then invokes `_bu_init`. This is the legacy
`InitCARD`/`StartCARD` path; the separately identified
`InitCARD2`/`StartCARD2` entry points are not part of this matching wrapper.
Event creation remains a separate step handled by `MemCard_InitIOEvents`.

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
their broader roles remain address-based.

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

`func_80043D48` first calls `TestEvent` on the four handles supplied by its
caller and then sets the shared result to `-1`, preparing the next asynchronous
operation. `func_80043DA0` provides a synchronous companion: it tests the same
four slots in order and returns `0` through `3` for the first signaled event,
or `-1` when its caller requests a single nonblocking pass and none is ready.

Matching `func_80044038` shows how the result drives retries. It prepares the
alternate four-handle set, calls `_card_clear(value)`, waits while the result
is negative, and retries only result `1` (timeout), with at most ten attempts.
The larger memory-card state machine in `func_80044608` consumes the same
values; its later conversion of result `3` to `4` is internal state-machine
bookkeeping, not a fifth event callback.

The request wrappers establish two other low-level sequences. `func_800440F0`
prepares `gMemCard_aIOEventHandles` and starts `_card_info(channel)` without
waiting. After its mode-2 request gate succeeds, `func_8004413C` runs the
blocking three-stage path: `_card_info(channel)` against that primary handle
set, `_card_clear` against the alternate set using channel byte
`D_8009B437`, then `_card_load(channel)` against the primary set again. It
resets the shared result before every stage and waits for a nonnegative event
result after each call; the function does not reinterpret those three results
before returning `1`.

## Directory enumeration

Matching `func_80044470` formats a `bu%02X:%s` device path and enumerates into
caller-owned Psy-Q `DIRENTRY` records. It calls `firstfile` for the initial
record and `nextfile` for subsequent records, accepting success only when
each function returns the same record pointer it was given.

The initial call and each failed advance allow five retries after the first
attempt. A successful `nextfile` resets that retry budget, advances by one
40-byte `DIRENTRY`, and increments the count. Enumeration stops at 15 records,
matching the usable block count on a memory card, and optionally stores the
final count through the caller's output pointer.

## Save payload staging

`SAVE_DATA_STATE_SIZE` fixes the live persistent state at `0x680` bytes,
beginning at `gDuel_awPlayerDeck` (`0x801D0200`). `SaveData_RequestWrite`
copies that state to `0x801D3200`, then calls `func_8003D03C` with staging base
`0x801D3000`.

The builder lays out:

| Staging offset | Size | Contents |
|---:|---:|---|
| `+0x000` | `0x200` | Header copied from the template at `0x801D4000`. |
| `+0x200` | `0x680` | Primary normalized game state. |
| `+0x880` | `0x680` | Duplicate copied from the completed primary state. |

The complete staged region through the duplicate is therefore `0xF00` bytes.
The subsequent `func_8003F758` call receives pointer `0x801D3200` and length
`0xD00`, exactly the contiguous pair of `0x680`-byte state copies. Its final
argument is the request selector. `func_8003F758` stores it unchanged at
`D_8009B3DE` through `func_8003F740` while setting the pending flag
`D_8009B3FA` to `0x8000`.

The exact callers use four selector values:

| Selector | Caller context | Buffer and length |
|---:|---|---|
| `0` | `SaveData_RequestLoad` | `0x801D3200`, `0x680` |
| `1` | Two paths in assembly `func_8003F8D4` | `0x801D1200` or `0x801D2200`, each `0x680` |
| `2` | `SaveData_RequestWrite` | `0x801D3200`, `0xD00` |
| `4` | `func_8003FE14` in the two-player save setup | `0x801D1880`, `0x400` |

The named callers establish selectors `0` and `2` as the normal single-save
load and write requests. Selectors `1` and `4` belong to distinct parts of the
two-save workflow, but their broader state-machine operation names remain
unassigned. The staged write still fits within one `0x2000`-byte memory-card
block; these calls do not establish how the remaining on-card bytes are
populated.

## Save integrity and successful-load application

The `0x680`-byte state contains three independently protected regions.
`func_8003CEB8` computes CRC-16/XMODEM with polynomial `0x1021` and a zero
initial value. The writers duplicate each 16-bit CRC into two adjacent
halfwords, copy that CRC into both halves of the two-word mask state, and fill
the region's mask words by repeatedly calling `SaveData_NextMaskWord`:

| Region | CRC input | Generated mask words | Duplicated CRC |
|---|---|---|---|
| Primary | `+0x000..+0x33F` (`0x340` bytes) | 15 words at `+0x340..+0x37B` | `+0x37C`, `+0x37E` |
| Secondary | `+0x380..+0x3EB` (`0x6C` bytes) | 4 words at `+0x3EC..+0x3FB` | `+0x3FC`, `+0x3FE` |
| Tertiary | `+0x400..+0x603` (`0x204` bytes) | 8 words at `+0x608..+0x627` | `+0x604`, `+0x606` |

`func_8003CF14` writes the primary and secondary records, while
`func_8003CFC8` writes the tertiary record. Matching `func_8003D174`
recomputes all three CRC seeds and compares every generated mask word in
descending address order; it returns false on the first mismatch.

`SaveData_RequestLoad` reads one `0x680`-byte state into `0x801D3200`.
`SaveData_PollLoad` waits for the request result and, only when the result is
`1`, copies that state over the live block at `gDuel_awPlayerDeck` before
calling `SaveData_ApplyRuntimeState`. That final step rebuilds the player-name
glyph string from save offset `+0x40C`, restores runtime fields from `+0x404`,
`+0x408`, and `+0x5DC`, and reapplies the saved sound output type at `+0x5DE`
when `gSD_bOutputType` is negative.

The two-save comparison path first uses `SaveData_HasSameDuelistCode` on the
32-bit field at `+0x334`. Its contiguous companion at `0x8003D2B8` accepts the
pair only when that code matches and the second save's word at `+0x404` equals
the current `D_8009B3B8` value. The role of that latter word remains
address-based.

## Evidence boundary

The descriptor values, event specifications, mode, and API prototypes are
direct matches for the imported Psy-Q 4.6 headers. The registration order,
callback mapping, result values, retry condition, and teardown are established
by local matching C. `MemCard_InitIOEvents` and `MemCard_CloseIOEvents` include
`libapi.h`, use its event interfaces, and spell the descriptors,
specifications, and interrupt mode with the SDK constants. This identifies
the runtime contract without claiming that the original game source used
those macro spellings rather than their numeric values.
