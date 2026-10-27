# Model-slot animation table lifecycle

These names describe resident game helpers, not recovered original symbols
or Psy-Q entry points. The supporting code is the matched
`src/game/model_slot_row_tables.c` group and its independently typed
`ModelSlot`, `ModelSlotRow` and `ModelSlotPart` records.

| Address | Name | Confidence |
|---|---|---|
| `0x8004D58C` | `Model_ResetSlotAnimationTables` | High |
| `0x8004D75C` | `Model_BuildSlotAnimationTables` | High |
| `0x8004D914` | `Model_LinkSlotAnimationRows` | High |

## Reset and import

`Model_InitSlot` maps the incoming model through `GsMapUnit`, then calls
`Model_ResetSlotAnimationTables`. The reset clears all ten accumulator rows,
their 58 per-part duration halfwords and row maxima, and initializes all
`animation_command_indices[row][part]` entry keys to `0xFFFF`. It clears the eight-byte part
bitfield, the four command/transfer pointers at `0xDD8..0xDE4`, and related
animation counters.

It also traverses the mapped owner chain at input `+0x10`, selects enabled
owner blocks of kinds 2 and 3, and imports their payload pointers. Kind 3
supplies the per-part flags, command list and source transfer pointer;
kind 2 supplies the other two transfer pointers. The name intentionally
does not promise to reset the entire model slot or just clear memory: it
resets and imports this animation lookup state.

## Build the indexed rows and lengths

`Model_BuildSlotAnimationTables` starts at each part's `start` key in
`animation_commands`, initially on row 1, and sets the part's `start_sid` to that row.
`ModelSlotPart` mirrors `GsSEQ`; the `GsLinkAnim` consumer and layout
assertions are documented in `notes/model-structures.md`.

Negative command words select a row through bits `16..22`; row zero ends
the scan. First visits install that row's entry key and follow its command
offset, whereas an already indexed row advances to the next word.
Nonnegative words contribute byte `+2` to that part's row duration.
The final pass stores each row's largest per-part duration in
`animation_duration_rows[row].max`.

The animation interpretation is established by consumers, not just the
record stride: `Model_ControlSlotAnimation` selects the corresponding
`animation_command_indices[animation][part]` entry keys and refuses a row with zero maximum;
`Model_GetSlotAnimationLength` returns the selected row's maximum.

## Link after building

`Model_LinkSlotAnimationRows` uses the populated entry-key table to walk
the command rows, copies row bits into their linked form, and rewrites
halfword command links, including paired termination links. It returns
without a command list or without parts. This is an in-place game command
relink pass, not `GsLinkAnim`'s binding of part sequence records.

The order is explicit in both callers:

- `Model_AdvanceSlotLoad` in `src/game/model_slot_control.c`: phase 1 loads/maps and
  resets, phase 2 builds, phase 3 links, then later phases prepare and rearm
  the parts.
- `Model_LoadSlot` in `src/game/model_slot_control.c`: build, link, derive
  transfer limits, then rearm each part from `start`/`start_sid`.

The former relinker header's statement that it ran *between* reset and
build was incorrect; both callers place it after build.

## Limits and matching

The functions' mechanical roles are high confidence. Individual command
opcode names, the full owner-block schema and the unrelated decoder at
`0x8004D134` remain address-based or offset-based where their semantics are
not established. No opcode, type, member layout or algorithm is changed.

The three functions remain in their original contiguous grouped source,
under the existing `gcc_2_8_1_g0` profile. The semantic registry records
their original North American addresses. Existing European macro adapters
are updated only on their shared-name side, retaining every regional
address-based target; no overlay or regional implementation is added.

## Resident lookup and playback views

The matching `ModelSlot` members now distinguish command indexing, accumulated
durations and the current playback position:

| Original member | Canonical member | Unchanged representation |
|---|---|---|
| `field_2C8` | `animation_command_indices` | `u16[10][58]`, `+0x2C8` |
| `field_750` | `animation_duration_rows` | `ModelSlotRow[10]`, `+0x750` |
| `field_DD8` | `animation_commands` | writable `s32 *G32`, `+0xDD8` |
| `field_E06` | `animation_position` | `u16`, `+0xE06`, 12.4 frame position |

Command indices address four-byte words, not byte offsets or frames.
The `0xFFFF` sentinel, row-one start, all ten reset rows, sparse entries and
load-bearing byte cursors stay intact. The duration rows retain their
`u16 values[58]` and `u16 max` members, halfword accumulation/truncation,
`0x76` stride and maximum calculation. The writable command list retains
the original import, in-place relinking, signed command tests and explicit
volatile base reload; no complete opcode or owner-block schema is inferred.

`Model_SeekSlotAnimation` stores `val << 4` into the position halfword and
reloads that narrowed value before visiting the parts.
`Model_GetSlotAnimationFrame` reads it with `>> 4`;
`Model_GetSlotAnimationLength` returns the unshifted row maximum.
Per-part seeking multiplies row totals and command byte `+2` by 16.
These arithmetic views do not establish a millisecond duration or permit
changing the original halfword conversions.

The per-frame update preserves its forward/reverse states, clamp/wrap
behavior, hold and queued-animation conditions, frame-step scheduling
and per-part seek order. The sound consumer can use position modulo row
length, but its alternate `animation_sound_position` cursor remains separate.
Queued tint requests retain their existing raw position capture.

A bounded observation of North American `MODEL.MRG` record 0 corroborates
the command-index interpretation: its kind-3 owner supplies a command-list
base, and its 26 sequence records have 28-byte strides and start indices
from 1 through 4463. The addresses formed as base plus index times four
fall before the next observed source-transfer payload. This is one
checksum-pinned sample, not a recovered command-table extent, an invariant
over all assets, or evidence about other regions.

Only corresponding member identifiers change in the existing model-control
native witness; all other complete-file bytes, oracle branches, mutation
controls and compiler flags remain unchanged. The French
`Variant393EntryState.field_DD8` scalar and its witness are unrelated and
retain their names. Other playback state, part flags, transfer pointers,
the tentative position helper, SDK declarations, source groups, profiles
and historical symbol evidence are not promoted or rewritten by association.

## Live parameters and row snapshots

The later pointer component names +0xDDC `animation_parameter_base` and
+0xDE0 `animation_snapshot_buffer`, both retaining `u8 *G32`. Kind 3
imports the former beside the command list; kind 2 imports the latter.
Command halfword word offsets select live parameter words. Row entry
saves three/five words to the contiguous buffer; restoration reverses
the copy. Missing entries consume no snapshot words. The remaining
+0xDE4 import is not promoted by association.

Non-null-HMD setup preserves the imported snapshot address or uses
`hmd + size`; the earlier null-HMD return does not clear these fields.
The original row-one work-span calculation and unconditional cache-base
store, including slot 2, remain intact. No buffer capacity, common
parameter schema, copy-overlap or readiness/lifetime guarantee follows.
See [the complete pointer protocol](model-structures.md#live-animation-parameters-and-saved-word-buffer).

## Playback selectors, extended part state and sound cues

These names follow the complete resident producer/consumer chains, not
new asset or runtime observations. All member types and offsets stay fixed.

| Original member | Canonical member | Unchanged representation |
|---|---|---|
| ModelSlot.field_BEC | extended_animation_part_mask | u8[8], +0xBEC |
| ModelSlot.field_BF4 | queued_animation_row | u8, +0xBF4 |
| ModelSlot.field_BF5 | current_animation_row | u8, +0xBF5 |
| ModelSlot.field_BF6 | queued_animation_row_latch | u8, +0xBF6 |
| ModelSlot.field_E08 | animation_sound_position | u16, +0xE08 |
| ModelSlot.field_E0D | playback_speed | u8, +0xE0D |
| ModelSlotSoundEntry.frame | animation_row | u8, +0 |
| ModelSlotSoundEntry.id | sound_id | u8, +1 |

`Model_SeekSlotAnimation` uses the current row when its selector is zero;
otherwise it writes the nonzero selector into the part sequences' `sid`
and the slot's current row. Queries, row-duration lookup, rendering,
control and sound use the same row. The tint allocator captures its low
five bits, and the temporary tint pass restores it separately from the
slot's frame position. No row bounds or new zero-selector meaning is added.

At the existing animation-end/hold gate, `Model_UpdateSlotAnimation`
compares the queued row to its latch, seeks when they differ, then copies
the queued value into the latch. Scene setup seeds the pair; rendering
can invalidate the latch with zero. `Model_ControlSlotAnimation` can also
copy the queue into the latch while restoring an earlier overlay, without
seeking that queued row. The latch is an acknowledgment cache, not a
second current row or a universal proof that a seek occurred. Keep the
four-byte reset seed and all partial reset/invalidation behavior.

`playback_speed` is the raw byte multiplied by `Model_GetFrameStep`.
The public setter retains its 4..32 check and fallback 8, and updates
part speeds through the original direction path. Other raw stores and
existing witnesses can supply values outside that range: the name does
not impose a global bound, new clamp, time unit or frame-rate conversion.

The table reset clears the eight-byte part mask and imports each enabled
kind-3 part word's bit 0x100. Loaders use it to reserve 20 rather than
12 snapshot bytes for a qualified part; overlay control copies five rather
than three words. The renderer combines it with the unchanged `aframe`,
row-duration and coordinate-owner conditions before its `ScaleMatrix`
path. This is not a new scale-only flag interpretation or a claim that
all 64 stored bits correspond to valid parts. Keep every bit calculation,
byte cursor, signed rounding path, command-key sentinel and loop bound.

`animation_sound_position` is a separate wrapping halfword, cleared by
table reset and the command transition. The sound pass normally uses
animation position modulo row length; its existing command branch instead
uses this cursor and increments it by the raw speed/frame-step product.
The non-Japanese diagnostic still reloads it through volatile and shifts
by four; Japan keeps its distinct print-free path. No cursor is merged
with the ordinary animation position or advanced on a newly added path.

The four-byte sound entry is animation row, sound ID and packed flags.
Row zero terminates the existing 64-record scan; other rows are compared
to the selected animation, not interpreted as cue times. The low fifteen
flag bits select the raw phase window; bit 15 selects the CD-tagged code
and its phase-zero seek. The byte ID still contributes to the unchanged
CD/SFX code packing. Preserve terminators, both cue tests, entry stride,
the metadata block copy, call order and every store narrowing.

## Control lifecycle, active command row and playback state

Four further raw bytes are named from their complete resident chains:

| Original member | Canonical member | Unchanged representation |
|---|---|---|
| ModelSlot.field_E0E | animation_control_state | u8, +0xE0E |
| ModelSlot.field_E0F | active_animation_command_row | u8, +0xE0F |
| ModelSlot.field_E10 | part_command_reset_pending | u8, +0xE10 |
| ModelSlot.field_E16 | playback_state | u8, +0xE16 |

`animation_control_state` combines lifecycle states and the narrowed
secondary callback result, not just the callback's last return value.
Initialization and the load path seed 6. The existing update promotes 6
to 7 for its selected/special row, otherwise to 2. With a selected handler,
state 7 invokes its initializer, stores 8 and clears the separate sound cursor.
The subsequent callback result is stored through the original byte before
dispatch: 0 continue, 5 back, 3 stone, 1 hit, 4 ouch and 2 termination are
corroborated by the existing trace strings. The default still traces WHY
and writes 2. Primary-callback invocation/ignored return, missing-handler
fallback, eligibility gates, special-model path and all transitions remain
unchanged. No enum, exhaustive range or callback-result validation is added.

`active_animation_command_row` selects the command-index/snapshot row in
`Model_ControlSlotAnimation`; it is distinct from the current seek row at
+0xBF5 and the queued row/latch. Zero is its existing inactive sentinel.
Starting a different row records command indices in each sequence's `ii`
and saves three/five words for each qualified part. A zero control request
copies the active row's saved words back and clears that row. Same-row
re-entry reaching the seek branch uses selector zero and position zero
without rearming the pending reset. Preserve the earlier state write before a zero-duration
return, flag-controlled replacement, raw row indexing and byte narrowing.

`part_command_reset_pending` is consumed by the ordinary draw's call to
`Model_ControlSlotAnimation(index, -1, 0)`. That branch clears the byte and
sets each linked sequence's `ii` to 0xFFFF. It does NOT restore snapshots,
clear the active command row, or change the lifecycle state. While pending,
the existing animation-end queue handoff is suppressed; a subsequent draw
can reconcile the active command row with the current seek row. Units-only
drawing returns before this consumption. Preserve the active-slot gate,
partial resets, all copy order and the separate queue/latch behavior.

`Model_SetSlotPlaybackDirection` maps positive, zero and negative arguments
to raw `playback_state` values 0x3E (forward), 0x23 (held) and 0x3C (reverse).
It supplies twice the raw speed, zero or the negated doubled speed to the
part-speed setter. Reset/load, per-frame advance, renderer/controllers and
part seeking use the same byte; the seek path's reverse-boundary behavior
is retained. These are observed codes, not a new restricted type or a new
policy for other raw values. Keep every comparison, arithmetic operation,
store conversion and existing frame-step/hold/wrap condition.

All four member types, offsets and widths remain fixed. Existing fixtures
receive identifier-only substitutions, including oracle and mutation-source
members; their numeric expectations, scenarios, controls, assertions and
compiler flags are retained. No new behavioral harness or runtime capture.

## Table entry arguments and scan accumulators

The reset/build/relink entries name their original slot argument
slot_index. Reset's second argument is hmd, not an animation-command
pointer: Model_InitSlot maps the complete input with GsMapUnit before
passing it unchanged, and reset follows the owner chain at hmd + 0x10.
The command list and transfer pointers are imported later from selected
kind-2/kind-3 owner payloads. No complete HMD/owner schema or new pointer
type follows from this distinction.

The existing unit scan and tentative primitive decoder name their two
signed accumulators max_lit_color_index and packet_bytes:

| Existing identifier | Role |
| --- | --- |
| Model_InitSlot.acc / decoder best | Running normal-derived lit-color-cache index maximum |
| Model_InitSlot.count / decoder total | Accumulated pre-cull packet budget in bytes |
| Decoder mode | The unchanged Model_InitSlot slot_index, not a new render mode |

Tag zero supplies the maximum pointer; tag one deliberately supplies
null. Both supply the same packet-byte accumulator. The maximum starts
at ZERO, not minus one; initialization still publishes maximum + i with
the existing i equal to one, even when no ordinary record raises it.
The packet accumulator starts at zero and retains its 0x20/0x28/0x34
per-record additions and final halfword store. The independently traced
cache and packet-space consumers are documented in
[model packet and cache accounting](model-packet-cache-accounting.md).
No emitted-polygon count, complete normal-array bound or buffer capacity
is inferred.

The decoder's slot_index name preserves the unsigned NA/JP below-two
gate and PAL below-three gate, all regional page/CLUT arithmetic and the
nullable-maximum behavior. tent_Model_RebaseTexturedPrimitives remains
tentative: the PAL slot-two source-upload linkage and uncovered selectors
are NOT resolved or promoted by naming these inputs and outputs.

Only parameter/local identifiers in the five complete definitions,
their two owning headers, three existing decoder-comment identifiers
and five existing semantic-registry NOTES cells migrate. Every expression,
type, statement order, signed test, cast, halfword conversion, volatile
reload, sentinel, byte cursor, source group, compiler profile and caller
stays unchanged. Other locals, fields and SDK declarations stay intact.
All existing fixtures, independent stub names, oracles, scenarios,
mutations, assertions and numeric controls are unchanged. No new
behavioral harness or local cloud-fixture execution is introduced.

## Playback and seek argument views

The complete five-definition playback/seek component now distinguishes
slot indices from slot pointers, part indices from animation rows, and
integer frame inputs from the narrowed sixteenth-frame clock.

| Entry | Definition argument names |
| --- | --- |
| Model_SetSlotPlaybackSpeed | slot_index / speed |
| Model_SetSlotPlaybackDirection | slot_index / sign |
| Model_SeekSlotAnimation | slot_index / animation_row / frame_position |
| Model_SeekSlotPartAnimation | slot / part_index / animation_row / scaled_position |
| tent_Model_GetSlotPartAnimationPosition | slot / part_index |

The speed setter keeps its unsigned 4..32 test and fallback eight, and
reapplies forward direction only in state 0x3E. The direction setter
keeps the sign branches, states 35/60/62 and original doubled-speed
velocity path; no new playback, pause or timing policy accompanies
these parameter names.

The whole-slot seek's animation_row is NOT a Boolean flag. Zero keeps
the slot's current row; a nonzero argument reaches each part's sid and
then the slot's current row. Its loop locals selected_row and part_index
name those existing values. frame_position is shifted left four,
stored to the u16 animation_position, then reloaded into scaled_position
BEFORE per-part calls. No widening, rounding, clamp or direct forwarding
of the pre-store value replaces that narrowing. Only cached_dimensions[3]
is cleared afterward, including the original zero-part path.

The part seek receives an unsigned scaled_position in sixteenth-frame
units, not the whole-slot integer frame input. Keep both materialized
part offsets, unchecked row/part indexing, missing-part/key/zero-duration
returns, repeated row and header reads, modulo arithmetic, exact-zero
boundary, playback-state-specific strict comparison and forward scan.
All ti/ci/tframe/rframe writes and the signed-halfword 0x6000 clamp stay
in their original order; no loop or command interpretation is rewritten.

The contiguous getter's row-scan local is also animation_row, but its
identity remains tentative. It still packs that row above a sum in
duration-byte units and uses its distinct table-base-relative header
comparison. It is NOT an exact inverse of the seek and no caller or
new reachability is established by argument naming. Its 0x6000/0x7000
exclusions and every zero-return path remain unchanged.

Only bounded bare identifiers in two complete C units, matching
prototype/comment identifiers in their two whole headers, this owning
note and five existing registry NOTES cells change. Qualified members,
types, arithmetic, statement order, accepted source groups, named
profiles, callers, initialized data and all existing controls stay exact.

## Part-command walk local views

The two contiguous part getter/seek definitions now name their command
walks without changing either interpretation. Both function names and
all argument names remain. The getter is STILL tentative and has no
found caller; the primitive-rebase helper is also still tentative.
Neither uncovered PAL upload linkage nor a new live getter path was
recovered. These local names do not promote either identity.

The getter's part_sequence reads the original GsSEQ view; command_base,
current_command and row_command distinguish the command-table base,
the part's current command and the row's advancing walk. Preserve its
null/negative-command returns and row scan1..9. first_command_index
still forms row_command BEFORE the 0xFFFF test; no new validation or
changed short-circuit order. Its header end comparison STILL adds the
header halfword to command_base, NOT to the row start. This is not the
seek's row-relative count and is NOT repaired into an exact inverse.

duration_units remains s32 and sums duration BYTE2 before the current
command. remaining_frame snapshots rframe; only values other than
0x6000/0x7000 add (tframe-remaining_frame)/16, with the original signed
division and truncation. Preserve the row-high-byte OR, unmasked sum,
no-match row zero and every zero path. No frame-unit normalization,
new packed-result type, bounds or observed playback claim.

The seek's part_pointer_offset and part_index_offset retain BOTH
up-front, otherwise unused part multiplications for old-GCC CSE.
first_command_index, command_base, part_sequence and command_cursor
retain their original declaration/load order and pointer construction
before the null/key checks. The two duration-table reads remain two
source expressions. row_frame_duration is the original s32 duration
times16; remaining_position retains the original unsigned-input modulo
and assignment to s32, not a widened or signed replacement.

row_header remains command_cursor-1. entry_number starts at zero;
the guard/loop reread the header, then entry_count snapshots it only
after the first walk. position_after_entry preserves both duplicated
duration expressions and mode-specific <=0 versus <0 break tests.
Keep exact-zero position handling, remnant subtraction, negative-next-
command test, pointer advance and all early returns. The goto scan's
stop_cursor advances BEFORE the count test, including the exit path;
it is not rewritten as a while or merged with command_cursor.

command_index remains ONE reused value for ti and the existing ci
selection. entry_frame_duration/remaining_frame keep the original
byte shift, subtraction, stores and final signed-halfword comparison.
Only mode0x3C can reach the original 0x6000 marker store. No new enum,
clamp, time conversion, row choice, callback or initialization.

Only scoped bare local identifiers in these TWO complete definitions,
this owning-note append and TWO existing semantic NOTES cells change.
Every member token, argument, type, comment, string, literal, expression,
scope and statement stays exact. Public headers, SDK declarations,
grouped ownership, profiles, manifests, fixtures, oracles, mutations,
assertions and numeric controls remain unchanged; no new harness.
