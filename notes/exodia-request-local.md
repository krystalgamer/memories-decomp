# Exodia presentation request ownership

`DuelScene_UpdateExodiaResult` in `src/game/duel_draw_resolution.c` retains one
actual `DuelEffectRequest *fx` local across three assigned allocations:

| Path | Request | Payload and word operation |
| --- | --- | --- |
| `0x4000` sparkle path | `0x13` | Publish the scene handle, set random X/Y, add `(n & 3) << 13` to the signed buffer word, increment the byte counter |
| `0x2000` centre transition | `0x13` | Set `0xA0/0x78`, publish the handle, add `0x8000` to the signed buffer word |
| `0x2000` per-card spark | `0` | Keep both slot reads, set offset X/Y, add unsigned `sum` to the buffer word through `anim`, set command `9`, sound, increment the byte counter |

The final unassigned `DuelEffect_CreateRequest(0x18)` is a fourth request
creation, **not** a fourth assignment to this local. The per-card request
is not published to `D_8009B17C`.

## Retained representation and sequencing

The allocator still returns its byte API, backed by a real request pool
entry whose `buffer` starts at `gRuntime_pSharedWorkBase + 0x3800`. The coordinator
already forwards this member as an integer word. Here, explicit
`s32`/`void *` value conversions preserve the existing signed additions
and pointer representation without introducing pointer arithmetic.
In the per-card path `sum` remains **u32**, while `anim` and `t14` remain
**s32**: `anim = (s32)fx->buffer + sum` retains the original unsigned
addition and target signed-word assignment, including word wrap.

Signed request payload members receive explicit value conversions of
the old unsigned halfword stores. The display-position additions remain
before 16-bit narrowing, including their wrap at `0xFFFF`. No coordinate
clamp, alternate owner, widened store, early counter update or common
scene publication was introduced. In particular, rand and interval calls,
both repeated display-slot reads, sound, commands and counter wrapping
retain their original order.

The rest of the complete state machine is unchanged: its display-object
cursor, signed address-of-member position store, cross-path `side`
reuse, gates, masks, completion-byte reads, final request and result handoff
remain intact. The old shared prefix remains for trap resolution, with
unchanged declarations and layout.

## Measured controls and scope

The first candidate uses the existing `gcc_2_8_1_g8_split` profile for the
complete `0x460` function. Its supported compiler assembly is identical
to the unchanged matching baseline; incremental full-executable matching
also succeeds without profile or compiler-flag changes. Candidate patch,
compiler/MASPSX baseline, diff and attempt record stay under `tmp/`.

`test_exodia_request_local.py` compiles the **three actual extracted request
fragments** with the source's real header/macro prefix and project types,
not a reimplementation or the whole state machine. Allocation, rand,
interval and sound are stubbed; no rendering, engine, GTE, overlay, real
request pool or continuous presentation is executed.

Each `-O0`/`-O2` run executes 590,848 fragment calls. Sparkle covers every
low random byte and byte counter, valid interval results and four safe
signed buffer words. Centre covers all byte counters and the same four
words. Per-card covers every X/Y halfword pattern at all five valid
slots, cycling eight buffer words including signed-boundary and
unsigned-wrap cases. It deliberately excludes negative/out-of-range slot
indices and signed-overflowing inputs to the two signed-add paths.

Independent byte oracles cover the complete request, scene handle,
counter, all display objects/slots and guards. Stub event checks cover
publication before rand, X before interval, and full per-card setup before
sound but counter update afterward. Negative controls reject scaled
per-card word stepping, a missing position offset and missing sparkle
publication. The surrounding state machine is protected separately by
the surgical source audit and exact complete-executable matching.
