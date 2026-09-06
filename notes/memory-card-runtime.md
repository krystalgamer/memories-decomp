# Memory-card Runtime Events

The resident memory-card layer uses Psy-Q BIOS events to convert asynchronous
card notifications into a small polling result. Matching
`MemCard_InitIOEvents` creates eight event handles in the array at
`gMemCard_aIOEventHandles` (`0x800F2AE0`): the same four event specifications
are registered once for `SwCARD` and once for `HwCARD`.

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

`func_80043D48` first probes the four handles supplied by its caller and then
sets the shared result to `-1`, preparing the next asynchronous operation.
`func_80043DA0` provides a synchronous companion: it probes the same four
slots in order and returns `0` through `3` for the first signaled event, or
`-1` when its caller requests a single nonblocking pass and none is ready.

Matching `func_80044038` shows how the result drives retries. It starts an
operation, waits while the result is negative, and retries only result `1`
(timeout), with at most ten attempts. The larger memory-card state machine in
`func_80044608` consumes the same values; its later conversion of result `3`
to `4` is internal state-machine bookkeeping, not a fifth event callback.

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

## Evidence boundary

The descriptor values, event specifications, mode, and API prototypes are
direct matches for the imported Psy-Q 4.6 headers. The registration order,
callback mapping, result values, retry condition, and teardown are established
by local matching C. `MemCard_InitIOEvents` and `MemCard_CloseIOEvents` include
`libapi.h`, use its event interfaces, and spell the descriptors,
specifications, and interrupt mode with the SDK constants. This identifies
the runtime contract without claiming that the original game source used
those macro spellings rather than their numeric values.
