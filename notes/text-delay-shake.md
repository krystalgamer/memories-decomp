# Shared text delay and viewport-shake state

`Text_WaitForDelay` and `Text_WaitWithViewportShake` are state-table
slots 15 and 16 in
[`text_box_state_callbacks.c`](../src/game/text_box_state_callbacks.c).
Their matching definitions stay together in
[`duel_effect_state_callbacks.c`](../src/game/duel_effect_state_callbacks.c).
The entire eighteen-entry table, callback ABI and source grouping remain.

| NA identity | Name | Existing role/storage |
|---|---|---|
| `0x800379F8` | `Text_WaitForDelay` | Word-operand delay callback |
| `0x80037A58` | `Text_WaitWithViewportShake` | Delay plus optional viewport jitter and origin restore |
| `0x8009B322` | `gText_WaitCounter` | Shared `s16` counter |
| `0x8009B348` | `gText_ShakeViewportOrigin` | Shared `u16[2]`, X then Y |

## Initialization and exact countdown behavior

Both callbacks test the channel's existing `0x80` initialization latch.
When clear, they set it and consume one little-endian word into the same
signed halfword counter. They then decrement the counter **in that call**,
not only on later calls, and clear the text state only for equality with
zero.

The target keeps the existing halfword narrowing and wrap. An input of one
completes on the initialization call; zero becomes `-1`, not immediate
completion. With no other shared-state interference, that zero value takes
the full target halfword cycle to return to zero. There is no new clamp,
nonpositive fast path, zero normalization, timeout or wider duration.

These count callback invocations, not guaranteed display frames. The pump
can run multiple builder steps, and both callbacks share the same counter
across channels. Another uninitialized wait can replace an active wait's
counter. No per-channel timer, synchronization or new initialization policy
is introduced.

This differs from the page-wait initialization delay and `s8` hold counter
described in [text-page-transition.md](text-page-transition.md).

## Viewport snapshot and conditional random work

The shake callback additionally captures viewport X then Y into the two
unsigned origin halfwords during initialization. It does so even when the
frame counter's low bit is clear.

`gGraphics_dwSyncedFrameCount` is the shared frame counter: `Graphics_SyncFrame` increments
it immediately after `VSync`. The `gGraphics_dwSyncedFrameCount & 1` condition is therefore
a frame-counter parity gate, not an independently configurable setting.
When that bit is set, the callback preserves this exact order:

1. call `rand`, mask with seven, subtract four, add to origin X and store X;
2. call `rand` again, mask with three, subtract two, add to origin Y and store Y;
3. decrement the shared counter and test for zero;
4. on completion restore origin X, restore origin Y and clear text state.

The offsets are around the saved origin, not accumulated from the previous
viewport. A set-bit invocation, including the completion call, still performs
both random calls and stores before restoration. When the parity bit is
clear, that random work is skipped, but snapshot, countdown and unconditional
completion restore remain active.

The unit selects the `.data` viewport arm `GRAPHICS_VIEWPORT_IN_DATA`;
the snapshot reads the coordinates unsigned (`*(u16 *)&gGraphics_sViewportX`)
and the low-halfword stores go through the same cast. The origin remains an unsigned
two-element array with plain GP addressing; it is not replaced by a signed
coordinate pair, structure, current-viewport alias or per-channel object.
The shared frame counter has other consumers and is not globally renamed
as a text-only shake-enable flag.

## Ownership and native identities

Only the existing `s16` and `u16[2]` extern declarations move from
`unmatched.h` into the callback owner. Their widths, signedness, array
extent, qualifiers, addressing attributes and absence of initializers
remain unchanged.

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Delay callback | `0x800379F8` | `0x800371B0` | `0x80037790` | `0x80037990` |
| Shake callback | `0x80037A58` | `0x80037210` | `0x800377F0` | `0x800379F0` |
| Counter | `0x8009B322` | `0x8009B212` | `0x8009C2BC` | `0x8009C6DC` |
| Origin array | `0x8009B348` | `0x8009B238` | `0x8009C2A4` | `0x8009C6C4` |

Japan retains the existing wrappers' native `D_8009B212/D_8009B238`
bindings. The unrelated `gDuel_bOutroWinnerSide` (formerly `gJapanese_D_8009B238`) at `0x8009B128` remains
untouched; a numeric stem is not evidence of the same cell. Remove only
the eight obsolete delay/shake/counter/origin wrapper macros after wiring
their actual native symbols. Other aliases, selection guards and profiles
stay fixed.

## Exact-match evidence

The ownership-only stage leaves all complete compiler/MASPSX outputs
raw-identical. The naming stage compares every whole artifact after
reversing only four identifiers, without filename, instruction, directive
or relocation filtering.

All seven native inventories preserve every non-name column and row;
unrelated symbols/imports, initialized table positions, manifests,
profiles, paths and layouts remain. The entire accepted ledger gains
exactly four evidence/caveat rows. Full clean retail matching remains
mandatory. No tests, fixtures, controls or runtime captures change, and
regional/cloud validation remains PR-side.
