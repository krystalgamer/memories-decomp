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

## Component boundary

Ready-system event counts and command-retry/range-wait accounting join these
service gates in one component PR after cumulative clean acceptance.
Byte wrap, defined unsigned word wrap, original signed increments and
halfword countdown narrowing must remain distinct. None becomes a frame
timer or universal success/error count without its actual producer/reader
contract.

No individual-word PR is opened. Existing witness migrations, if needed,
are identifier-only; no harness is added and regional/cloud tests stay
PR-side.
