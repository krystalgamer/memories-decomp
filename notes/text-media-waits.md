# Text media waits and shared CD-sound polling state

Three callbacks occupy existing text-state slots 12, 13 and 17. Their
names describe the actual predicates and producers, not a new scheduler or
playback loop.

| Slot | API | Existing completion criterion |
|---|---|---|
| 12 | `DuelEffect_WaitForFileTransfer` | Masked request word OR full secondary-request word equals zero |
| 13 | `Text_WaitForBgmFade` | `SD_IsBgmFadeActive(object) != 1` |
| 17 | `Text_WaitForCdSoundTransfer` | Original phase/flag, signed-budget or byte-watchdog exit |

All eighteen initialized table positions and the callback parameter type
remain unchanged.

## File predicate and BGM caller ABI

The file wait preserves
`(D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | D_8009B134_abs`,
with mask `0x02000030`, and clears text state only for zero. It does not
start, cancel or service a transfer, and this predicate is not a promise
that every possible kind of device I/O is idle. The BGM control command's
high opcode bit selects this state after its existing operation.

The fade wait keeps the literal comparison with one. The current query
reads the sound driver's fade step, but the callback deliberately still
passes its effect object in `$a0`. Retain the
`SD_IS_BGM_FADE_ACTIVE_IGNORES_OBJECT` declaration arm and call expression;
do not strengthen the caller to the callee's zero-argument view. The fade
command's high operand bit selects this state.

The absolute file-state declaration views, widths and qualifiers stay
fixed. In particular, the transfer-step `_abs` halfword remains the
existing nonvolatile `.data` view, not the separate volatile GP view.

## CD-sound producer and initialization

When `DuelEffect_PlaySoundCommand` sees bit `0x8000` in its sound word, it
calls `SD_CdSoundPlay`, consumes a second little-endian word into shared
`s16 gText_CdSoundPollBudget`, selects state `0x11` and yields parsing.
The ordinary sound-effect branch does not take this path.

On an uninitialized wait call, the callback sets the existing `0x80`
latch, sets channel byte `delay_52` to `0xFF`, resets shared
`u8 gText_CdSoundWaitPhase` to zero and sets state bit `0x40` when the
signed budget is nonzero. It does not clear a preexisting `0x40` bit when
the budget is zero. Negative values still satisfy that nonzero arm.

The byte countdown is decremented before phase polling, including the
initialization call. This is callback progression, not a guaranteed frame
or playback duration.

## Deliberate phase fallthrough

| Shared phase | Existing operation |
|---|---|
| 0 | Return until sector-range flag `0x80000` is observed; then re-arm `delay_52` to `0xFF`, store phase 1 and fall through |
| 1 | Return until transfer-step bit `0x4000` is observed; then re-arm `0xFF`, store phase 2 and fall through |
| 2 | Exit if that bit is clear; otherwise return when state bit `0x40` is clear, or decrement the signed budget and return only while its stored result is positive |

The bit interpretation is grounded in the loader writer, not a guessed
busy/settings flag: `File_StepActiveTransfer` sets `0x4000` in range
substate 5 before its substate-6 monitoring, and clears it through
`0x3FFC` when that phase ends. `File_StopRangeTransfer` also clears it.
The wait retains the literal set/clear observations.

Phase 2 does not restart phase 0 or issue another transfer or playback
request. Its optional count is spent per qualifying callback while the
bit remains set, not per completed/reissued sound. The original broad
"repeat the whole run" description is therefore not the interface.

When the byte countdown becomes zero, phase polling is skipped and the
callback exits. Existing unhandled phase values likewise reach the
original fallthrough exit. Every exit clears text state and writes
`delay_52 = 1`. Incoming initialized zero-byte wrap, signed halfword
narrowing/wrap, both re-arm stores, both phase stores and the
post-decrement `> 0` comparison remain.

The phase and budget stay shared across channels, while the watchdog
byte stays in the channel. No per-channel phase allocation, new enum,
positive-only validation, unbounded-zero wait, extra retry or default
repair is introduced.

## Native identities and European collision

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| File wait | `0x8003798C` | `0x80037144` | `0x80037724` | `0x80037924` |
| Fade wait | `0x800379C4` | `0x8003717C` | `0x8003775C` | `0x8003795C` |
| CD wait | `0x80037B40` | `0x800372F8` | `0x800378D8` | `0x80037AD8` |
| Phase byte | `0x8009B335` | `0x8009B225` | `0x8009C290` | `0x8009C6B0` |
| Budget halfword | `0x8009B33C` | `0x8009B22C` | `0x8009C292` | `0x8009C6B2` |

The file-wait name is already accepted in Japan; canonicalize that same
identity everywhere without duplicating or relocating it.

Europe formerly used `European_func_800378D8` plus an unprototyped ASM-label
declaration for the CD callback: a macro to `func_800378D8` would be
rescanned into the different callback's `func_80037670` alias. The new
canonical CD name no longer collides, so only its obsolete bridge and
media aliases are removed. The unrelated display-effect callback mapping
and every other wrapper alias remain.

## Acceptance

Definitions, declarations, table references, producer state and seven native
bindings use the same identities. Existing `u8`/`s16` ownership and
declarations stay in place. Only the inaccurate shared-script-gate and
playback-repeat prose is corrected.

Every complete compiler/MASPSX output is compared after reversing only
five identifiers, without filename, instruction, directive or relocation
filtering. Full clean retail executable matching remains mandatory.
Inventories retain all other fields and rows; profiles, manifests, paths,
grouping, layouts and unrelated symbols/imports stay unchanged. The
complete accepted ledger gains five evidence/caveat rows. Tests, controls
and captures do not change; regional/cloud validation remains PR-side.

The original five-name proof precedes accepted #7626 integration. The
combined-tree comparison aligns only its `GameOver_Update` identity in
the saved baseline, then reverses these five media identities, retaining
all whole outputs. A fresh clean retail gate covers the integrated tree
and its complete accepted ordered ledger insertion.

## Shared countdown member, unchanged watchdog

The channel bytes now spell playback_state and delay_countdown rather
than state_51 and delay_52. The latter is the SAME u8 at +0x52 used by
normal glyph cadence, not a new CD-only timer or a promised frame duration.
glyph_delay at +0x53 remains the parser's reload byte, not this wait's
0xFF re-arm value.

Keep initialization's same-call decrement, both 0xFF re-arms, deliberate
phase fallthrough, incoming initialized-zero wrap, shared signed poll
budget, preserved 0x40 latch and every original exit. Completion still
clears playback_state and writes delay_countdown = 1. This naming neither
restarts sound playback nor changes phase/budget ownership or the file/BGM
wait predicates, sound-command stream operands or yield timing.
