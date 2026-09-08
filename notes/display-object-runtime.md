# Display-object Runtime

The resident display-object system uses a fixed pool at `D_800EFE48`.
Matching initialization, allocation, list-management, update, and submission
functions establish the pool geometry and the seven independent processing
lists without requiring semantic names for the globals.

`src/game/display_object_layout.h` now centralizes the cross-function
constants: `DISPLAY_OBJECT_RECORD_SIZE` is `0x70`,
`DISPLAY_OBJECT_POOL_CAPACITY` is 96,
`DISPLAY_OBJECT_RESERVED_CAPACITY` is 16,
`DISPLAY_OBJECT_LIST_COUNT` is 7,
`DISPLAY_OBJECT_FLAG_CLIP_TEST` is `0x04`,
`DISPLAY_OBJECT_FLAG_SCREEN_SPACE` is `0x08`,
`DISPLAY_OBJECT_FLAG_RENDERABLE` is `0x40`,
`DISPLAY_OBJECT_FLAG_ALLOCATED` is `0x80`, and
`DISPLAY_OBJECT_RENDERABLE_MASK` is `0xC0`. The header deliberately defines
only shared geometry and flags, not a complete display-object structure.

## Pool and list geometry

`DisplayObject_ResetPool` clears the field at slot offset `+0x08` across 96
records with the `DISPLAY_OBJECT_RECORD_SIZE` stride. It also initializes
seven signed 16-bit entries at `D_800EFE38` and seven companion entries at
`D_800F2878` to `-1`.

The two allocation scans divide the pool:

| Function | Slots scanned | Role established by the scan |
|---|---:|---|
| `func_8004006C` | `0-95` | Searches the complete 96-slot pool. |
| `func_8004002C` | `16-95` | Skips the 16 reserved slots and searches the 80-slot general-use subrange beginning at `D_800F0548`. |

Both return the first slot whose `+0x08` flags do not contain
`DISPLAY_OBJECT_FLAG_ALLOCATED`. `func_800400AC` initializes a newly claimed
slot with `DISPLAY_OBJECT_RENDERABLE_MASK`, the combination of
`DISPLAY_OBJECT_FLAG_RENDERABLE` and `DISPLAY_OBJECT_FLAG_ALLOCATED`. Render
and update passes require both bits before submitting visible content.

Each slot begins with two signed 16-bit links at `+0x00` and `+0x02`.
`func_800400AC` inserts a slot at the head selected by its list key, records
that key at `+0x1E`, and fills the companion `D_800F2878` entry when the list
was empty. `func_8004020C` removes a slot by reconnecting both neighboring
links and clears its allocation flags. `func_800402A0` removes and reinserts
an existing slot under another list key while preserving its flags.

## Shared index and allocation API

[`display_object_api.h`](../src/game/display_object_api.h) is the sole C
declaration point for the general-use index scan and indexed allocator:

```c
s32 func_8004002C(void);
void *func_800400AC(s32 index, s32 key);
```

The defining `display_slot_lifecycle.c` and every current C caller include
this header. The migration removes 48 local getter declarations and 53 local
allocator declarations, including old-style unspecified-argument spellings.
Assembly-only word/relocation references in `func_800291E0.c` are not C
declaration sites and remain unchanged.

The getter returns an **integer slot index**, 16-95, or signed `-1` when
none is available. It does not reserve or mark the slot; another scan before
allocation can return the same index.

The allocator returns null for a negative index without touching pool state.
For a nonnegative index it increments `D_8009B412`, **including when the
slot was already allocated**. An existing allocated slot is returned without
reinitialization or relinking, and the supplied key does not move it to a
different list. On first allocation it links the slot to the previous
`D_800EFE38[key]` entry and updates that endpoint; `D_800F2878[key]` is also
initialized when the list was empty.

Only an explicit subset of record fields is initialized. This is not
`malloc`, does not zero the complete record and does not promise a fresh or
uniquely owned object. There is no upper-index or key validation: valid
nonnegative inputs must address the 96-slot pool and appropriate list.
The opaque `void *` result exposes an arena address without publishing the
private `DisplaySlot` layout or conflating callers' different prefix views.
Existing null checks and unchecked call sites are preserved.

The alternate whole-pool scanner `func_8004006C` remains outside this
header migration. Four callers still carry legacy pointer-shaped declarations
for that separate function and explicitly convert its returned word to
`s32` at the allocator boundary:

| Caller | Source |
|---|---|
| `Dialog_OpenChoice` | `dialog_choice_state.c` |
| `Dialog_UpdateChoice` | `dialog_update_choice.c` |
| `func_80018150` | `duel_card_object_helpers.c` |
| `func_8002E3FC` | `func_8002E3FC.c` |

These are index conversions, not dereferences or pointer-success tests.
Index zero is valid for that scanner and the `-1` sentinel must remain
signed. Other existing scalar declarations of the alternate scanner are
unchanged. The four boundaries are explicit pending a complete audit/migration
of that separate API, not evidence that its true return is a pointer.

## Shared coordinate configuration

The separate configurator migration extends the same header with:

```c
void func_800404CC(
    void *object, s32 x, s32 y, s32 field_67, s32 field_68,
    s32 field_69, s32 color, s32 texture
);
```

The defining `display_object_config.c` and all 21 C caller units use this
contract, including the formerly implicit calls in `func_8002BFCC.c`.
All 20 local configurator declarations are removed; this is not a
callers-only or partially shared signature.

Coordinates remain **full-word inputs**. The implementation stores their
low halfwords at object `+0x30/+0x32`, then forwards the five configuration
words unchanged to `func_80040468`. Its old `s16` formals did not establish
that all callers had already narrowed their coordinates. Most declarations
were wide, and halfword source loads participate in promoted integer sums.
Keeping `s32` inputs preserves those expressions without adding caller-side
sign-extension or truncation. This does not clamp or validate coordinates.

The opaque object parameter is viewed through byte-pointer casts only at
the two existing stores. No new walking temporary, aggregate layout,
null check, allocation or rendering submission is introduced. The
delegate's existing selector-byte, packed-configuration and flag writes
are unchanged; parameter names do not establish new artwork identities.

## Parent-linked duel rows

Matching `func_80022F98` and `func_80022FF0` use a separate local parent view
to attach display objects to one reference position. For every non-null
object, `func_80022F98`:

- stores object coordinates `+0x30/+0x32` relative to the parent's reference
  object in fields `+0x28/+0x2A`;
- copies the parent's one-byte index to object halfword `+0x2C`;
- installs `func_80022EEC` as callback `+0x24`;
- sets object byte `+0x6C` to one.

`func_80022FF0` applies that setup first to the parent's standalone base
object, then to two object-pointer lanes in each of
`DUEL_FIELD_ROW_SIZE` five 12-byte rows. It always clears the standalone base
pointer after processing it. When the caller's clear argument is nonzero, it
also clears both pointers in every row after attaching their objects.

This proves an eleven-object traversal shape: one standalone base plus two
lanes across five duel columns. The parent, row, and object structures remain
local to `display_parent_links.c`; only the already established duel row count
is shared.

## Seven processing lists

`func_80041340` visits list keys `6` down to `0`. For every nonempty head it
sets geometry screen distance `150`, resets the geometry offset to `(0, 0)`,
and calls that key's handler from the seven-entry table at `D_80090FB0`.

Matching consumers establish these heads and pass shapes:

| Key | Head address | Matching consumer | Established behavior |
|---:|---:|---|---|
| `0` | `D_800EFE38` | `func_80040CAC` | Runs each slot's `+0x24` callback without a submission step. |
| `1` | `D_800EFE3A` | not yet identified in matching C | The dispatcher still treats it as one of the seven lists. |
| `2` | `D_800EFE3C` | `func_80040814` | Runs the callback, then `func_80041D60` and `func_8004158C` for slots matching `DISPLAY_OBJECT_RENDERABLE_MASK`. |
| `3` | `D_800EFE3E` | `func_80040BF8` | Runs the callback, then submits through `func_800408D0` for slots matching `DISPLAY_OBJECT_RENDERABLE_MASK`. |
| `4` | `D_800EFE40` | `func_80040DD8` | Builds and submits the `0x38` packet form described below. |
| `5` | `D_800EFE42` | `func_80041068` | Builds and submits the larger `0x3C` packet form described below. |
| `6` | `D_800EFE44` | `func_80040D14` | Runs the callback, then invokes the optional secondary callback at `+0x4C`. |

Every matched list walker reads the next-slot link before invoking the current
slot's callback. The callback can therefore remove or move the current slot
without losing the walk's continuation.

## Packet-building siblings

`func_80040DD8` and `func_80041068` share the same high-level path:

1. Walk the slot list with `DISPLAY_OBJECT_RECORD_SIZE` and run each `+0x24`
   callback.
2. Require `(flags_08 & DISPLAY_OBJECT_RENDERABLE_MASK) ==
   DISPLAY_OBJECT_RENDERABLE_MASK`.
3. Copy slot geometry into scratchpad packet storage at `0x1F800344`.
4. Subtract `gGraphics_sViewportX` and `gGraphics_sViewportY` unless
   `DISPLAY_OBJECT_FLAG_SCREEN_SPACE` is set.
5. When `DISPLAY_OBJECT_FLAG_CLIP_TEST` is set, run `func_80041E7C` with
   scratch workspace at `0x1F800398`; reject a nonpositive result or add
   `0x04000000` to the packet control word.
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

## Two-phase display-object fades

`src/game/display_object_fade.h` defines two bits in object byte `+0x13`:

| Flag | Value | Matching behavior |
|---|---:|---|
| `DISPLAY_OBJECT_FADE_FLAG_INITIALIZED` | `0x80` | `DisplayObjectFade_MarkInitialized` sets it on the first update and returns zero; later updates return one. |
| `DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE` | `0x40` | The three contiguous fade callbacks set or test it when moving from their first phase to their second. |

The callbacks at `0x80039AFC-0x80039C94` share this one-shot initialization
latch but use different byte lanes:

- `func_80039AFC` initializes state bytes `+0x14/+0x15`, advances bytes
  `+0x04/+0x06` until signed wrap selects the second phase, then advances
  `+0x05/+0x07` and clears the local fade state.
- `func_80039BE0` initializes bytes `+0x04`-`+0x07` to `0x80`, fades the first
  pair to zero, then fades the second pair and calls the shared completion
  helper.
- `func_80039C94` derives an initial delay from two halfwords, counts that
  delay down, then raises bytes `+0x08`-`+0x0A` by four until `0x40` before
  calling the same completion helper.

These are per-object callbacks and are independent of the
`FadeTransitionState` screen overlay documented in
[`fade-transition-state.md`](fade-transition-state.md). The shared fade header
names only the two proven state bits; the surrounding object layouts remain
local because the three callbacks use incompatible field views.

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
arguments above come from local matching C. Shared pool geometry and flag
names now live in `display_object_layout.h`; slot fields and list globals
remain address- or offset-based because their complete ownership and original
type names are not yet established.
