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

## Reset, build and relink local views

The three table-lifecycle definitions now distinguish their existing
slot, payload, table-offset and command-cursor locals. The reset still
reuses ONE index across the duration-row, mask-byte and part-flag walks.
Its duration and entry-key offsets retain their DIFFERENT strides;
the byte cursors are not replaced with indexed member accesses.
Keep all ten rows, 58 parts, eight mask bytes and existing store order,
including the casted pointer clears before the null owner-chain return.

owner and kind2_payload/kind3_payload name only the existing mapped
owner-chain selection: enabled records test +8, use kind byte +0xF
and fetch the payload at +4. The last matching enabled owner still wins.
Keep the null initial chain return and -1 chain terminator as distinct
tests; no new malformed-chain or null-next handling. Both payloads
remain independent, with their original byte advances and four final
byte-cursor stores. field_DE4 remains opaque.

part_flags_base/part_flags_cursor retain the original count halfword
and +4 flag read, including the volatile load and repeated count reads.
mask_byte_index, mask_bit_base and the old/new mask-byte views preserve
the signed division, shifts, bit0x100 test and byte narrowing. No owner
schema, payload extent, valid-part bound or stronger flag interpretation
is inferred from these local names.

The builder's part_sequence, first_command_index and command_cursor
retain row-one seeding, start_sid publication and the later separate
sequence-pointer reload for start. A null sequence still stops the
entire part scan. command_word remains u32 with the original signed
negative test; row bits stay16..22, zero ends the scan and the FFFF
entry sentinel distinguishes an advance from a first-visit jump.
Keep both halfword key reads, duration BYTE2 accumulation/truncation
and the final rows1..9 maxima pass over the original sequence count.
No new bounds, reset-before-build guarantee or opcode taxonomy.

The relinker's part_byte_offset/entry_byte_offset preserve the shared
running-byte-offset arithmetic and row stride. row_one_command is
formed without a new sentinel guard. row_command and target_command
are still constructed before the existing sentinel/row-zero checks;
pre_row_command remains the word immediately before the row start.
target_entry/target_command_index retain the exact halfword lookup.

command_word/target_word and target_row name only current reads.
Keep all row-bit masks, shifts, pointer-null tests, indefinite scans,
goto labels and target-row-one halfword test. row_bits_clear_mask and
row_one_bit are the unchanged constants used in the existing rewrite.
The hit path retains BOTH linked halfword stores; the zero-row path
retains its FIRST volatile command-base reload, pointer differences,
-1/+1 arithmetic and signed-halfword conversions. No SDK-linker claim,
cycle detection, null/sentinel repair or changed command interpretation.

Only scoped bare locals in these THREE complete definitions and the
matching identifiers in TWO existing explanatory comments change.
Arguments, member tokens, types, expressions, scopes and statement
order stay. The earlier tentative primitive decoder is wholly intact,
including every European branch and its upload-linkage caveats.
The entire four-definition source/profile group remains together.
Headers, callers, SDK declarations, fixtures and controls stay exact.

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

## Snapshot-span and row-copy local views

The immediate and staged loaders now distinguish the existing snapshot
span, mask-byte/part indices and sequence-rearm cursors. Both retain
the row-one FFFF test and 12/20-byte accounting ONLY for index<2;
the cache-base store and cache-end derivation still occur for EVERY
slot. These are signed address calculations, not a proven buffer size.
The immediate loader's cache_or_speed deliberately remains ONE binding
for cache-base then playback-speed work; the staged phase_value likewise
retains both uses. No split temporaries or changed register scheduling.

The immediate loader still initializes before the null-HMD return,
builds then links, sets the root transform, rearms all sequence fields,
publishes is_active=1 before drawing and leaves it set. Its playback
switch and index<2 tint gate remain. The staged phase instead clears
is_active after drawing, has its original later active publications,
and preserves all eleven switch phases, callbacks and partial-state exits.

The staged initial_load_step is only the early idle-gate snapshot.
Keep the separate load_phase read, original per-phase slot aliases and
per-loop part_index scopes, module-command reread/modulo and callback
state bracket. phase_work remains reused for command and timing work.
The non-Japanese VSync/printf path and Japanese omission stay distinct;
final next_load_step still follows the LIVE slot state after callbacks,
not the earlier snapshot or an unconditional phase increment.

The row controller captures parameter_base, snapshot_cursor and
part_sequences BEFORE the active gate. command_halfwords addresses the
selected command; restore_command_index/save_command_index are COMMAND
indices, not row selectors. Keep pointer formation before FFFF tests.
The restore/save word counts and rounded-part locals remain SEPARATE
bindings: the original shift expressions still use a count of three
while selecting an extended-mask bit, then choose three/five words.

Missing entries advance neither snapshot cursor. Restoration copies
saved words TO parameters and clears the active row; saving reverses
the copy and publishes ii before it. Preserve selector-1 behavior,
all zero-duration/replacement gates, the pre-recursion previous_row,
queue-latch update, recursive restore and same-row seek ordering.
No new null/bounds guard, row validation, overlap or lifetime promise.

Only scoped bare locals in these THREE complete definitions change.
Arguments, member tokens, comments, types, expressions, scopes and
statement order stay exact. The existing staged-load witness changes
ONLY identifiers inside its eight mutation-source/replacement strings;
all four mutation labels, operators, numbers, scenarios, oracles and
assertions stay. No new harness or local cloud-witness execution.

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

## Playback position and part-speed local views

The per-frame tick now distinguishes animation_row, row_end_position,
position_step and forward/reverse position candidates. The row maximum
is still shifted by four, and the step is still raw playback_speed
times Model_GetFrameStep. These are existing scaled-position arithmetic
views, not milliseconds, a new timing unit or a global speed bound.

Keep the initial active/command gates and row/length/step snapshots
before the row-six direction handoff and duelist-slot sound call.
Later playback-state and position reads remain live. Forward handling
still resets a position at/above the end before adding and clamping;
reverse handling still substitutes the end for zero before subtracting
and clamping at zero. Preserve every halfword store/narrowing.

The pending-reset gate, mode-specific completion test, tint[7]==6
queue override, queue/latch comparison, seek-before-latch write,
optional active-row restore and effect-endpoint/tint/queue publications
remain ordered exactly. No new tint-byte interpretation or transition
atomicity is inferred. The final early-return test STILL uses
state!=0x23, raw speed0x10 and a SEPARATE Model_GetFrameStep()==2 call,
including the no-command path. scaled_position is captured only after
that test; the part loop keeps its original count and current-row reads.

Direction setup retains both slot aliases, sign-selected states
60/35/62, the second speed/state capture, double_speed and its separate
saved copy. part_speed is still zero/negative-double/positive-double
through the original goto/comparison paths. Labels and duplicate
assignments stay; no enum, Boolean direction or merged speed expression.

The public seek's part_count remains mutable: it snapshots the count,
stores frame_position<<4 into the position HALFWORD, reloads that narrowed
position, then reloads count after EVERY per-part seek. Preserve each
current-row read, conditional sid publication, final nonzero-row
publication and cached_dimensions[3] clear, even with no parts.

The part-speed writer still walks stored sequence pointers and writes
the original signed argument, narrowed to s8, into each speed field.
Keep initial !=0/do-while and the original live u8 count reads;
no new null guard, bounds check or normalized direction is introduced.

Only scoped bare locals in these FOUR complete definitions change.
All arguments, member tokens, types, comments, literals, expressions,
scopes, labels and source order stay. All original source groups and
profiles remain; headers, callers, SDK declarations and existing
fixtures/controls are unchanged. No new harness or local cloud tests.

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

## Action-row camera and sound-cue local views

The contiguous Model_StartBattleSlotAction/Model_PlaySlotAnimationSounds
definitions now name only their existing local views. action_row is
the initial guardian-star selector plus three; camera_duration captures
its row maximum BEFORE the optional row-control call. Camera setup
still runs even for a zero-length row. Keep signed duration*2/3,
minimum60 and the later nonzero u16 override, which can replace that
minimum with a smaller value. Repeated selector/option reads stay live.
The first hold-state call still receives0/2 from bit80, followed by
a SECOND call with1 when bit40 is set. No time-unit or action-content
interpretation is added.

The cue pass keeps active, exact tint-black and sound-enabled gates,
then captures animation_row, row_end_position and position_step.
Normal cue_position remains position modulo length, with zero-length
fallback0. use_command_cursor names ONLY the original signed-command/
control-state predicate. Its alternate path keeps a SECOND frame-step
call, captures the old sound cursor, publishes the narrowed increment
and selects guardian_star+3; cue windows STILL use the FIRST step.

The non-Japanese trace keeps separate selector/command reads, quotient
and remainder, the later volatile sound-cursor reload and sprintf/FntPrint
order. Japan retains its print-free path and omits the trace buffer.
Only the normal path returns for playback_state0x23. No cursor merging,
new scheduling path or changed diagnostic/command encoding.

spatial_bytes remains the eight-byte automatic buffer passed to the
unchanged spatial-volume/pan call. Its outputs are STILL NOT supplied
to SD_SEPlay: volume0xFF/pan0 remain. The call stays before cue scanning.
cue, cue_index, sound_id, slot_sound_tag, cd_seek_flag and sound_command
retain original pointer/count/code views. The high bit selects the
CD-tagged packing and phase-zero seek, not a new complete flag schema.
Code construction still precedes the row-zero return. Preserve the
row match, independent CD-seek and low15-bit [position,position+step)
tests, possible seek-then-play order, four-byte advance, one-pass block,
goto and64-record bound. No new cue bounds, ID or terminator policy.

Only scoped bare locals in these TWO complete definitions and ONE
identifier in an existing scheduling comment change. All arguments,
member tokens, types, constants, expressions, scopes and order remain.
Headers, callers, SDK declarations, grouped ownership and all existing
fixtures/controls stay exact. No new harness or local cloud tests.

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
tentative overall: these earlier input/output names alone did not resolve
the upload linkage. Later measured PAL palette evidence is documented in
[primitive context views](model-primitive-context.md); unexercised low-X
and uncovered selector/input cases are not promoted into observed behavior.

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

## Per-sequence interpolation local views

The two complete resident translation/rotation and translation/rotation/
scale callbacks now distinguish only their existing local views. They
consume the current/target/output parameter tail populated by the animation
unit dispatcher; they are not new SDK implementations or original names.
Both preserve tail-pointer formation before the zero-duration return1,
then coordinate decoding and all writes after that guard.

The first callback's rewrite_work deliberately remains ONE binding for
the packed rewrite index THEN its masked word offset. rewrite_section
retains the top-byte selection. The scaled callback keeps its distinct
table-index-plus-five and shifted-word-offset arithmetic. No common
lookup helper, new section bound or pointer-lifetime promise.

remaining_duration and total_duration retain rframe/tframe captures,
signed widths and original source*remaining plus target*(total-remaining)
weighting. The scaled callback keeps SEPARATE translation_elapsed and
rotation_scale_elapsed bindings. The latter is assigned INSIDE the first
rotation expression at its original later point and reused for Y/Z and
scale. Do not merge these equal-looking arithmetic paths or widen them.

Keep the original target/source angle load order. The first callback's
six angle temporaries remain u16; the scaled callback's remain s32 with
explicit s16 interpretations. source_rx/ry/rz and target_rx/ry/rz name
those existing views, not normalized angles. Wrap still occurs ONLY at
duration16 and the original threshold, with zero target selecting the
original positive-full-turn adjustment. All labels, empty statements,
axis-specific comparisons, target narrowing and signed divisions stay.
adjusted_target_angle and signed_angle_delta add no wider angle policy.

The scaled rotation_halfwords is the existing s16 pointer to node rotation
passed as SVECTOR, NOT the optional output record. Keep the original SDK
casts, scale s32[3], rotation-then-scale call order and later flag clear.
No pad initialization, structure replacement or matrix-layout inference.
The first output_keyframe remains a separate null-test capture; its six
stores STILL reload through interpolation_parameters->out. The scaled
nine stores remain raw u16 pointer loads of translation/rotation/scale
low halfwords, followed by the same common finished label and return0.
No publication consolidation, cast rewrite or new output readiness.

Only scoped bare locals in these TWO complete definitions change.
Arguments, member tokens, comments, types, constants, expressions, scopes,
labels and order remain. The inline scanner, three earlier binding/
dispatch definitions, headers, SDK declarations and profiles stay exact.
The existing angle-wrap witness changes ONLY corresponding identifiers
in its selector/mutation literals and existing actual-helper fragments.
Its reference oracle, all operators, thresholds, negative controls,
1,179,648 cases, assertions, scenarios and compiler flags stay exact.
The unrelated witness out parameter stays out. No fixture is imported
or executed locally and no new generated-C/source-string harness is added.
The whole five-function source/profile group and both macro adapters remain.
