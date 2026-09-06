# Display-object Runtime

The resident display-object system uses a fixed pool at `D_800EFE48`.
Matching initialization, allocation, list-management, update, and submission
functions establish the pool geometry and the seven independent processing
lists without requiring semantic names for the globals.

## Pool and list geometry

`DisplayObject_ResetPool` clears the field at slot offset `+0x08` across 96
records with a `0x70`-byte stride. It also initializes seven signed 16-bit
entries at `D_800EFE38` and seven companion entries at `D_800F2878` to `-1`.

The two allocation scans divide the pool:

| Function | Slots scanned | Role established by the scan |
|---|---:|---|
| `func_8004006C` | `0-95` | Searches the complete 96-slot pool. |
| `func_8004002C` | `16-95` | Searches the 80-slot general-use subrange beginning at `D_800F0548`. |

Both return the first slot whose `+0x08` flags do not contain bit `0x80`.
`func_800400AC` initializes a newly claimed slot with flags `0x00C0`, so bit
`0x80` marks allocation and the render/update passes require the complete
`0xC0` mask before submitting visible content.

Each slot begins with two signed 16-bit links at `+0x00` and `+0x02`.
`func_800400AC` inserts a slot at the head selected by its list key, records
that key at `+0x1E`, and fills the companion `D_800F2878` entry when the list
was empty. `func_8004020C` removes a slot by reconnecting both neighboring
links and clears its allocation flags. `func_800402A0` removes and reinserts
an existing slot under another list key while preserving its flags.

## Seven processing lists

`func_80041340` visits list keys `6` down to `0`. For every nonempty head it
sets geometry screen distance `150`, resets the geometry offset to `(0, 0)`,
and calls that key's handler from the seven-entry table at `D_80090FB0`.

Matching consumers establish these heads and pass shapes:

| Key | Head address | Matching consumer | Established behavior |
|---:|---:|---|---|
| `0` | `D_800EFE38` | `func_80040CAC` | Runs each slot's `+0x24` callback without a submission step. |
| `1` | `D_800EFE3A` | not yet identified in matching C | The dispatcher still treats it as one of the seven lists. |
| `2` | `D_800EFE3C` | `func_80040814` | Runs the callback, then `func_80041D60` and `func_8004158C` for slots with flags `0xC0`. |
| `3` | `D_800EFE3E` | `func_80040BF8` | Runs the callback, then submits through `func_800408D0` for slots with flags `0xC0`. |
| `4` | `D_800EFE40` | `func_80040DD8` | Builds and submits the `0x38` packet form described below. |
| `5` | `D_800EFE42` | `func_80041068` | Builds and submits the larger `0x3C` packet form described below. |
| `6` | `D_800EFE44` | `func_80040D14` | Runs the callback, then invokes the optional secondary callback at `+0x4C`. |

Every matched list walker reads the next-slot link before invoking the current
slot's callback. The callback can therefore remove or move the current slot
without losing the walk's continuation.

## Packet-building siblings

`func_80040DD8` and `func_80041068` share the same high-level path:

1. Walk a `0x70`-byte slot list and run each `+0x24` callback.
2. Require `(flags_08 & 0xC0) == 0xC0`.
3. Copy slot geometry into scratchpad packet storage at `0x1F800344`.
4. Subtract `gGraphics_sViewportX` and `gGraphics_sViewportY` unless flags bit
   `0x08` is set.
5. When flags bit `0x04` is set, run `func_80041E7C` with scratch workspace
   at `0x1F800398`; reject a nonpositive result or add `0x04000000` to the
   packet control word.
6. Select the ordering table through slot byte `+0x17` and submit through
   `func_80042188`.

The concrete differences are:

| Function | List | Packet length byte | Primitive code byte | Priority bits | Optional second packet |
|---|---:|---:|---:|---:|---|
| `func_80040DD8` | `4` | `8` | `0x38` | `0x40000` | Controlled by slot byte `+0x5A`. |
| `func_80041068` | `5` | `12` | `0x3C` | `0x50000` | Controlled by object-relative byte `+0x72`. |

Both combine those priority bits with the slot's `+0x14` value. Their
different source geometry offsets and packet sizes show that list keys `4`
and `5` are distinct primitive paths rather than interchangeable ordering
groups. The observed `+0x72` test lies two bytes beyond the nominal `0x70`
slot stride, so its ownership remains unresolved rather than being modeled as
a normal `DisplaySlot` field.

## Script-driven viewport tween

The duel-event script dispatch table at `D_80090C50` pairs slots `6` and `7`
for viewport movement. `Script_OpViewportTween` reads target X, target Y, and
frame count as three little-endian 16-bit operands, then switches the active
script command to state `7`.

`Script_UpdateViewportTween` derives signed 16.16 per-frame deltas from the
current `gGraphics_sViewportX/Y` values on the state's first frame. Each update
advances the two 16.16 accumulators, publishes their high halves as the
viewport origin, and decrements the frame count. The last update clears the
active command and snaps both viewport coordinates to the exact targets.

## Evidence boundary

The counts, strides, links, flags, list heads, callback order, scratchpad
addresses, camera adjustment, clip gate, packet bytes, and submission
arguments above come from local matching C. The slot fields and list globals
remain address- or offset-based because their complete ownership and original
type names are not yet established.
