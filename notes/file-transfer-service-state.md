# Transfer service gates and event accounting

These names describe the existing service controls and callback counters.
They add no scheduler, synchronization primitive, reset policy, retry limit
or new time unit.

## Service call gate and reentry latch

`gFile_ServiceCallGate` remains `volatile u16`.
`gFile_ServiceReentryLatch` remains `volatile s32`. Loader initialization
clears the call gate before the latch at their original points.

`File_ServiceTransfers` keeps the complete sequence:

1. A nonzero call gate is cleared, then that invocation returns.
2. Otherwise the gate becomes one before the reentry-latch load.
3. A nonzero latch returns without clearing the newly published gate.
4. Otherwise the latch becomes one, pending promotion and transfer stepping
   retain their original conditions, and the common tail clears the latch.

The current frontend service passes zero through the existing signed
parameter; the implementation still does not use it. No call budget, frame
counter, Boolean normalization, atomic lock or exception-style cleanup is
inferred from these names.

Volatility and widths are measured inputs. The gate store remains before
the latch load, preserving the load-delay NOP documented by the original
workflow experiment. The words are not packed together, reordered or
cached as locals, and the existing early returns are not rewritten.

| Identity | North America | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Service call gate | `0x8009B124` | `0x8009B014` | `0x8009C070` | `0x8009C490` |
| Service reentry latch | `0x8009B0E8` | `0x8009AFD8` | `0x8009C074` | `0x8009C494` |

Exactly four obsolete Japanese macros retire in reset and runtime
adapters. Native bindings follow the real regional addresses, preserving
unrelated French labels at NA-looking addresses.

All 940 complete compiler/MASPSX outputs match after only these identities.
Full C files retain every other byte after the four exact macro removals;
the header retains its ordered noncomment tokens. The complete prior ledger
gains two evidence/caveat rows. Inventories, profiles, manifests, source
paths, tests, witnesses, controls, traces and tooling remain unchanged.

## Ready-system event counters

`gFile_ReadyEventCount` remains plain `u8` and increments before the ready
callback's low-byte event-one gate. Non-data events still count and still
return before descriptor work. Byte wrap remains the original unsigned
narrowing, not a widened or saturating count.

`gFile_ReadyDataEventCount` remains plain `s32`, incremented only after the
event-one gate through the original explicit `u32` addition and cast. This
retains the defined `0x7FFFFFFF` to `0x80000000` transition. It precedes
descriptor-mode dispatch, so unsupported modes can count an event without
transferring a sector. Neither counter is a byte/sector-success total.

On the read command's completion event, the original ready-system mode and
start calls still happen before byte-counter reset, then word-counter
reset, then command-busy clearing. No reset is moved before registration.
Other command/event values do not gain reset behavior.

The existing executable witness keeps every setup, event choice, wrap
boundary, callback mutation, pointer expectation, assertion, error and stub.
Its complete file differs only by the two identifiers. No new witness or
control is introduced.

| Identity | North America | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Ready event byte count | `0x8009B114` | `0x8009B004` | `0x8009C08C` | `0x8009C4AC` |
| Data-ready event word count | `0x8009B138` | `0x8009B028` | `0x8009C058` | `0x8009C478` |

All 940 complete outputs and the entire callback source/header retain every
other byte after the two identities. The complete prior ledger gains two
rows; all native shadows, inventories, profiles, manifests and unrelated
test/control/tooling/trace content remain.

## Command retry and range-wait accounting

`gFile_CommandRetryCount` remains plain `s32`. The six read/seek/pause/filter
command callbacks retain their exact disk-error event tests, increment before
the same reissued command and preserve all arguments. Completion and other
events do not gain increments, and loader initialization remains the reset
point. No new overflow policy, retry cap, backoff or success accounting.

`gFile_RangeWaitCountdown` remains plain `u16`. State five stores `0x258`
before its original flag publication and range-read hook, then falls through
to state six. The first decrement therefore remains in that same invocation.
State six retains the signed halfword comparison and separate signed
descriptor-position test before its original pause-tail path.

The countdown is not renamed as a frame count: service-call gating and
command-busy returns can prevent an eligible step. It gains no frame-step
multiplier, clamp, wider temporary, extra return or reset in
`File_InitTransferState`, which does not clear it.

The existing ready witness's retry-counter definition receives only its
identifier migration; no assertion, stub, event choice, fixture or control
changes. All four C files retain their entire contents after the two names
and two exact obsolete Japanese aliases, while the header keeps its ordered
declaration tokens. All 940 complete outputs match after those names.

| Identity | North America | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Command retry count | `0x8009B130` | `0x8009B020` | `0x8009C060` | `0x8009C480` |
| Range wait countdown | `0x8009B0EC` | `0x8009AFDC` | `0x8009C078` | `0x8009C498` |

The complete prior ledger gains two evidence/caveat rows. Profiles,
inventories, manifests, all unrelated native shadows and every other
test/control/tooling/trace byte remain unchanged.

## Component boundary

Service gates, ready event counts and retry/range-wait accounting form one
component PR after cumulative clean acceptance.
Byte wrap, defined unsigned word wrap, original signed increments and
halfword countdown narrowing must remain distinct. None becomes a frame
timer or universal success/error count without its actual producer/reader
contract.

No individual-word PR is opened. Existing witness migrations, if needed,
are identifier-only; no harness is added and regional/cloud tests stay
PR-side.
