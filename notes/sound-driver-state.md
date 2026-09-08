# Sound Driver State

`g_SDValue` is a global pointer to the main sound-driver work area. Its name is
present in the retail debug strings and is also used by the successor game's
sound subsystem. Local matching code confirms that the global itself stores a
pointer rather than an inline structure.

`src/game/sound.h` defines the currently verified `SDValue` layout through
offset `0x164B`. Unknown regions remain explicitly padded, and uncertain fields
retain offset-based names. The header does not claim that this is the original
Konami type or field naming.

## Confirmed fields

| Offset | Field | Evidence |
|---|---|---|
| `0x0040` | `flags_0040` | `Sound_InitFrontend` sets startup bits `0x0A` and polls bit `0x08` until it clears; other control and cleanup routines use the remaining status bits. |
| `0x0042` | `mix_scale` | CD mix calculation uses it as a 16-bit scale. |
| `0x0048` | `output_type` | `SD_SetOutputType`; live stereo/mono traces establish values 0 and 1. |
| `0x004A` | `flags_004A` | Initialization and command processing use independent bits. |
| `0x004C` | `command_count` | Bounds the 16-entry command scan. |
| `0x0080` | `commands[16]` | 16 records, each `0x30` bytes; command at `+0x00` and eight verified 32-bit argument/result slots at `+0x10`-`+0x2C`. |
| `0x0404` | `voice_ids[4]` | Four 16-bit voice identifiers. |
| `0x0424` | `voice_value[4]` | Four per-voice byte values reduced by `voice_step`. |
| `0x0428` | `voice_step[4]` | Four per-voice decrement values. |
| `0x042C` | `voice_timer[4]` | Four 16-bit countdown timers. |
| `0x0434` | `voice_active_mask` | One bit per voice entry. |
| `0x0438` | `field_0438` | Cursor initialized from the first link entry and advanced by each selected entry's second word. |
| `0x0442` | `field_0442` | Selected link-table index; reset to `0xFFFF` and used to suppress duplicate requests. |
| `0x0448` | `field_0448` | Pointer to the 8-byte `SDValueLink` table used by pending sound-data requests. |
| `0x0510` | `cd_volume` | Sound output changes recalculate and store this signed 16-bit value. |
| `0x0514` | `channel_volume[2]` | Two byte channel-volume scalars. |
| `0x0533` | `mix_multiplier` | Multiplies the shared CD mix scale. |
| `0x053C` | `buffer_053C[4][0x200]` | Four work buffers whose addresses are installed during sound initialization. |
| `0x153C` | `buffer_ptrs_153C[4]` | Pointers to the four work buffers. |
| `0x1560` | `field_1560` | Base pointer used to select a music/sequence table entry. |
| `0x1564` | `music_track` | Pointer defaults to `0x801EA800`; its first 16-bit value is initialized to `0xFFFF`. |
| `0x1618` | `busy` | Command registration tests and sets this byte. |

The remaining named `field_XXXX` members have verified offsets and widths but
insufficient semantic evidence for stronger names.

`SD_BGMFadeOut` writes `-8` and mode `0` through the sequence-control wrapper.
Live traces place that call in the same frame as every frontend screen
fade-out, establishing the standard BGM attenuation rate as eight units per
frame. `SD_BGMFadeOutWithStep` uses the same path after converting a positive
script or campaign-map step to its negative per-frame delta.

`SD_KeyOffVoiceSlots` calls Psy-Q `SpuSetKey` with key-off mode and the mask
covering the four dedicated voice slots, then polls their four status bytes.
It stops after they clear or after 24 iterations. `SD_StopAll` first sends the
stop path through both sequence-control selectors, then calls this voice
key-off helper; the debug sound screen uses it for Start-button cleanup.

`SD_BGMPlay` forces its input into the `SD_BGM_COMMAND_BASE` (`0x7000`) BGM
command class, dispatches it through the sequence path, and stores the value in
`gSD_dwCurrentBgmCommand`. Script and duel-effect handlers copy that value
when their bytecode requests replay of the current music command.

`SD_SEPlayFull` masks its input to `SD_COMMAND_VALUE_MASK` and calls
`SD_SEPlay` with `SD_SE_VOLUME_MAX` (`0xFF`) and mode zero. The adjacent
flagged-effect wrapper at `0x8003FF88` sets command bit `0x8000` before using
the same full-volume arguments. This effect-call maximum is distinct from
`SD_CHANNEL_VOLUME_MAX` (`0x80`), which initializes and bounds the two
`channel_volume` bytes used by the internal mix path.

`Sound_InitFrontend` is the game-facing bridge into this lower-level state. It
sets `gSD_bOutputType` to the unresolved sentinel `-1`, then passes
`gFile_anLba[4]`, `[5]`, and `[6]` to `func_80046990`. The runtime file table
identifies those positions as `SD_SE.DAT`, `SD_BGM.DAT`, and `MASTER.XA`.
`func_80046990` clears `field_003C`, clears bits `0x01`, `0x02`, or `0x40` in
`flags_004A` when the corresponding file position is zero, and sets
`flags_0040 |= 0x0A`. The frontend then calls `func_80012D4C` while
`func_8004703C` continues to expose bit `0x08`, making that bit the
game-facing startup-busy condition.

Matching `func_80047788` and its adjacent callers establish the link-table
lifecycle. `func_8004763C` resets `field_0442` to `SD_VALUE_LINK_INDEX_NONE`
(`0xFFFF`) and initializes
`field_0438` to the first link's second word plus `0x1010`.
`func_80047AD0` ignores a requested index when that entry's second word is
zero or when the same index is already selected; accepted indices are stored
in `field_0442` before dispatch.

`func_80047788` applies `SD_VALUE_LINK_INDEX_MASK` (`0xFFFF`) and
`SD_VALUE_LINK_RECORD_SIZE` (`0x08`) to select one `SDValueLink`.
The mask does not add an index-validity check, and the cache sentinel remains
distinct from the pending-entry table's other `0xFFFF` markers. The stride is
an integer byte count, not the unrelated eight-byte pending-entry geometry.
The function multiplies the state halfword at offset `+0x02` by
eight, rounds that byte count up to `0x800`-byte units, adds one, and adds the
result to the entry's leading halfword. The adjusted value, current
`field_0438`, fixed buffer `0x801E6800`, and entry's second word feed the
existing six-argument command wrapper. It then queues a separate type-`0x51`
request carrying the same cursor and fixed buffer. Finally it advances
`field_0438` by the selected entry's second word. That repeated use proves the
field is an entry span/cursor increment, but not whether its original source
name described bytes, sectors, or another sound-container unit.

The separate pending-entry loader uses `SD_NOTE_RECORD_SIZE` (`0x08`) for
the `field_0444` note records, their raw copies, and the source payload stride.
The shared `SDNote` and private `SoundPendingEntry` views both guard that
extent without merging their fields. `SD_PENDING_ENTRY_NONE` (`0xFFFF`) keeps
the lookup-clear and skipped-ID marker distinct from the link-cache sentinel.
The source ID list still begins at `+8` and advances two bytes per ID; those
are not note-record strides. Register pins, pointer/count reloads, and the
halfword rate adjustment remain unchanged.

`SD_ArmBusyCallback` now expresses the registration path in pure C: it sets
`busy` and installs `SD_ClearBusyFlag` in the main callback slot. This replaces
the former register-pinned source while preserving exact code generation.

`SD_ProcessSequenceTracks` establishes that the region beginning at secondary
state offset `0x518` is traversed as `0x2C`-byte sequence-track records. Each
active record advances a 16-bit timer, dispatches MIDI commands when the timer
wraps past `0xFF`, and contributes to the secondary state's running total.

`SD_ResetSequenceTracks` independently confirms 16 records and the `0x2C`
stride. It sets each record's byte at `0x24` and clears its leading word; the
two exact functions remain separate because grouping changes resident text
size.

## SPU decoded data, reverb, and shutdown

Matching `func_80045054` now imports the real `libspu.h` interface and calls:

```c
SpuReadDecodedData(
    (SpuDecodedData *)((u8 *)g_SDValue + 0x53C),
    5
);
```

`SpuDecodedData` is four arrays of `0x200` signed halfwords (`cd_left`,
`cd_right`, `voice1`, and `voice3`), so this call establishes a
`0x1000`-byte SDK view over `g_SDValue+0x53C` through `+0x153B`. The shared
`SDValue` definition retains its existing byte-array and padding split because
other matching functions use narrower buffer views; the cast documents an
additional ABI-compatible interpretation rather than changing their source
shape.

### Reverb work-area queries

Matching `func_8004ACE4` handles two sound-sequence control entries:

| Entry byte `+0x11` | Reverb operation |
|---:|---|
| `0x0F` | Disables active reverb, adjusts work-area reservation, submits `SPU_REV_MODE` with the requested mode byte at `+0x13`, re-reserves when required, enables reverb, and caches the byte at secondary-state offset `0x844`. |
| `0x10` | Submits `SPU_REV_DEPTHL \| SPU_REV_DEPTHR` with the byte at `+0x13` shifted left by eight for both channels, enables reverb, and caches the byte at offset `0x845`. |

The mode path uses the imported `SPU_CHECK` (`-1`) before release and
`SPU_DIAG` (`-2`) before reservation. The retail SDK implementation at
`0x800767E0` establishes the distinction:

- Exactly `-1` returns the stored reservation flag at `D_80092B20`.
- Every other argument loads `D_80092B24`, calls `_SpuIsInAllocateArea_`,
  and returns whether that probe returned zero.

Thus the game's `SPU_DIAG` call is an allocation-area diagnostic, not another
read of the reservation flag. The implementation does not uniquely recognize
`-2`; the SDK name labels the game's existing operand without narrowing the
observed contract. The query itself does not set or clear the reservation
flag.

`SpuReserveReverbWorkArea` at `0x80076790` corroborates this: a nonzero
reservation request uses the same probe before setting `D_80092B20` to one;
an off request or unsuccessful probe clears it. The 64-byte query body and
the relevant reserve/probe instruction bodies were compared directly with
the retail executable. No SDK implementation or global names are changed,
and no unit interpretation is assigned to `D_80092B24`.

Separate initialization paths call `SpuSetReverbModeType(SPU_REV_MODE_OFF)`
while resetting sound state.

SPU shutdown is now explicit at both game-owned boundaries. `SD_Term` performs
its secondary-state cleanup and then calls `SpuQuit`. The output teardown path
at `0x80046F58` disables the SPU IRQ with `SpuSetIRQ(0)` immediately before
its own `SpuQuit` call.

## SPU transfer and voice interfaces

Matching sound initialization selects transfer mode zero with
`SpuSetTransferMode(0)`. The secondary transfer path then calls
`SpuSetTransferStartAddr` with either the configured SPU RAM base or that base
plus `bytes_consumed` before advancing a transfer window. The status wrapper
at `0x800498BC` forwards caller value zero as
`SpuIsTransferCompleted(0)` and every nonzero value as
`SpuIsTransferCompleted(1)`; local evidence does not assign stronger names to
those two modes.

Voice setup uses `SpuSetVoiceAttr` through two layout-compatible local views:
the main driver submits the attribute block rooted at `g_SDValue+0x3C4`, while
secondary sequence playback builds temporary attribute packets before each
call. Those casts establish SDK call compatibility without claiming that the
game-owned records are complete `SpuVoiceAttr` structures.

Cleanup and slot-reuse paths pair `SpuSetKey(0, mask)` with
`SpuGetKeyStatus(mask)`. Several loops wait until the returned status is `2`
or `0`, while another path recognizes status `3` before forcing a key-off;
the code therefore treats the values as distinct lifecycle states rather
than a Boolean active flag. Matching envelope queries call
`SpuGetVoiceEnvelope` for voices `20` through `23` and use the returned
halfword when deciding whether a tracked voice entry remains active.

The output-transition helper at `0x8004671C` also submits its 40-byte local
record through `SpuSetCommonAttr`. As with the voice packets, the explicit
`SpuCommonAttr *` cast records the verified SDK boundary while retaining the
source shape required by the matching compiler.

## Structure safeguards

The header contains compile-time size assertions for:

- `SDCommand`: `0x30`;
- `SDValueLink`: `0x08`;
- `SDValue`: `0x164C`.

The queue submitter reuses `SD_COMMAND_QUEUE_COUNT` for its upper-bound test.
It retains the signed count, leading command-byte store, whole-record copy,
and repeated base/count reads; no lower-bound check or clamping is added.
The local sound-code, refill, forwarded, and sequence request views reuse
`SD_COMMAND_RECORD_SIZE` for their existing `0x30`-byte extents and have size
assertions. This shares only their storage extent, not their command-specific
payload meanings, and does not initialize previously untouched bytes.

Some translation units may continue using raw pointer views when required to
preserve GCC relocation shape. The shared header remains the layout reference,
while exact executable matching decides whether a typed field access is safe
for a particular function.

The contiguous output/control block at `0x80046F58-0x80047278` now builds as
`src/game/sound_output.c`. Its ten functions use `SDValue` and `SDCommand`
directly, including output-type reads/writes, driver flags, CD-volume reuse,
and construction of three command variants.

An additional scalar/pointer pass converts 17 pure-C functions to named
`SDValue` fields covering channel volume, CD volume, driver flags, the
four-voice tables, late control fields, and the music-track pointer.

Two accesses deliberately retain an explicit byte-pointer expression:

- `func_80047FAC` indexes the four voice IDs as
  `((u8 *)g_SDValue + index * 2 + 0x404)` because direct structure-array
  indexing changes GCC's address calculation and adds three instructions.
- `func_800493F8` writes the music-track pointer through
  `((u8 *)g_SDValue + 0x1564)` because the direct member assignment changes
  register allocation.

Both files include `sound.h`; the raw expressions are exact-code-generation
views of fields whose offsets and types are defined by `SDValue`.

All pure-C `g_SDValue` users now include `sound.h`. Nine additional functions
use the shared command queue, buffer pointers, voice arrays, flags, and late
control fields directly.

`func_80049138` is the third deliberate raw-view exception. The global pointer
is volatile in that routine, and typed member expressions change its repeated
load/register schedule. It suppresses the default extern declaration from
`sound.h`, redeclares the pointer as `u8 * volatile`, and retains the verified
offset expressions while still using the shared header as the layout source.

Functions containing GCC inline assembly remain unchanged. Migrating their
declarations is deferred until the inline assembly itself can be replaced with
matching C.

The contiguous initialization block at `0x80049200-0x800495EC` now builds as
`src/game/sound_init.c`. It preserves the explicit raw music-pointer write in
`func_800493F8` while sharing `SDValue` declarations across the other
music/sequence helpers.

## Spatial volume and pan

Matching `func_8005A6A8` converts one sound source's X/Z position into a
volume byte and signed pan byte using the 32-byte reference-view record at
`D_800F56F0` documented in `notes/model-structures.md`. Let `(sx, sz)` be the
source, `(vx, vz)` the viewpoint, and `(rx, rz)` the reference point.

The volume uses the XZ-plane distance from the viewpoint:

```text
distance = sqrt((vx - sx)^2 + (vz - sz)^2)
volume   = 255                                      if distance <= 700
           max(16, 255 - (distance - 700) / 12)    otherwise
```

The pan uses signed perpendicular distance from the view axis:

```text
axis_length = sqrt((rz - vz)^2 + (vx - rx)^2)
side = (sx * (rz - vz) + sz * (vx - rx) + rx * vz - vx * rz)
       / axis_length
pan = clamp(side / 16, -127, 127)
```

All operations are integer operations. A zero-length view axis leaves pan at
zero. The function never reads the Y coordinates, never lowers volume below
`0x10`, and stores negative pan through the caller's byte pointer. This is a
game-side spatial-audio helper built on camera geometry, not a field in either
sound-driver state structure. Its matching source now imports `libgte.h` for
the canonical `SquareRoot0` interface while retaining local offset reads for
the game-owned reference-view storage.

## Secondary state (`D_8009B458`)

`D_8009B458` is a second global pointer used by the sequence/stream side of the
sound driver. `func_800494F4` installs the pointer and clears exactly 530
32-bit words, establishing a total allocation size of `0x848` bytes.

`src/game/sound.h` defines the partial `SDSecondaryState`,
`SDSecondaryTransfer`, `SDSecondaryRecord`, and `SDSecondaryObject` views.
Offset-based names are retained except where several matched functions
establish a stable role. Unmodeled and overlapping regions remain padding or
explicit typed/raw views rather than speculative fields.

The leading channel records and the later track records are distinct layouts.
`func_8004C114` selects a channel with the MIDI status byte's low nibble and
indexes the leading records with a `0x18`-byte stride; `func_8004A518`
initializes all sixteen. `SD_SEQUENCE_CHANNEL_COUNT` and
`SD_SEQUENCE_CHANNEL_RECORD_SIZE` describe this `0x180`-byte prefix.
The separate `SD_SEQUENCE_TRACK_COUNT` and `SD_SEQUENCE_TRACK_RECORD_SIZE`
describe the sixteen `0x2C`-byte track records starting at `+0x518`.
Both pairs live alongside the separate `SD_SECONDARY_OBJECT_COUNT` and
`SD_SECONDARY_OBJECT_SIZE` in the constant-only `sound_sequence_constants.h`.
The legacy raw views can share this geometry without changing their state
declarations. Channel and track constants must not stand in for the twenty
`0x28`-byte objects or a 24-voice SPU status buffer.

`SDSecondaryObject.channel_index` is the byte at object `+0x03`. Selection
uses it to index the leading channel records, and the per-object guards
compare it with `SD_SEQUENCE_CHANNEL_COUNT`. `SD_SECONDARY_RECORD_NONE`
(`0x63`), already used by the allocator, is the shared no-record marker used
by object initialization and retirement. This marker is unrelated to the
similarly numbered controller ID. `SDSecondaryRecord.volume` at channel
record `+0x03` is a separate field; no raw object view or out-of-range behavior
is rewritten.

The channel-message writers and matched gain/pitch readers establish these
channel controls without changing their byte storage:

| Offset | Member | Evidence |
|---|---|---|
| `+0x00` | `program` | Program-change dispatch calls `func_8004B6E8`, which stores its program byte here. |
| `+0x01` | `pan` | Controller `0x0A` writes it; `func_8004A0FC` includes it in the pan sum. |
| `+0x03` | `volume` | Controller `7` writes it; `func_8004A0FC` multiplies it into the level. |
| `+0x05` | `expression` | Controller `0x0B` writes it; `func_8004A0FC` applies it as another level factor. |
| `+0x07` | `pitch_bend_msb` | Pitch-bend dispatch passes the second data byte to `func_8004B70C`; `func_8004A43C` caches it and obtains the pitch adjustment through `func_8004A3BC`. |

The pan writer still substitutes `1` for an incoming zero. Pitch bend still
stores only the second data byte masked to seven bits; the first data byte
remains unused. These names do not add full fourteen-bit bend handling or
change the raw gain/pitch readers. In particular, the cached `+0x07` value is
pitch bend, not a bank byte.

The stored bend MSB uses `SD_SEQUENCE_PITCH_BEND_MSB_MASK` (`0x7F`), separate
from pan and other seven-bit fields. `func_8004A3BC` narrows its input to a
byte and returns zero at `SD_SEQUENCE_PITCH_BEND_CENTER` (`64`). Below center
it uses the object's `+0x11` coefficient and the distance from `64`; above
center it uses `+0x10` and subtracts
`SD_SEQUENCE_PITCH_BEND_POSITIVE_BIAS` (`63`). Both coefficients are doubled,
and the result is narrowed to a signed halfword. The positive-side bias is
not changed to `64` to make the two branches look symmetric.

The matched gain routine uses the separate `SD_SECONDARY_PAN_*` constants
for its pan domain. It sums four byte contributions, subtracts three center
values, and clamps to `0` through `127`; the existing override flag instead
chooses center `64`. The right-half calculation keeps its original mask and
its distinct center branch. The exclusive upper bound is `128`, not a new
selectable endpoint. These constants do not replace the pitch-bend center,
the primary voice's signed-pan domain, or the gain/velocity normalization
fields that happen to contain similar values.

The same header names the event codes consumed by `func_8004C420`,
`func_8004C114`, and `func_8004BE88`. A status-present bit, a message-type mask,
and a channel mask have separate roles even when their values match an event
code. The parser retains its running-status and argument-count-table behavior;
the channel dispatcher handles note off/on, control change, program change,
and pitch bend, including the existing zero-velocity note-on path to note off.
The stream's `0xFF` marker routes meta events, while `0xF0` and `0xF7` delimit
the existing SysEx handling. End-of-track, tempo, SMPTE-offset, time-signature,
and key-signature selectors are named without changing their byte consumption.
Custom controller numbers and loop handling remain separate from these event
classifications.

The adjacent meta/SysEx handlers and channel dispatcher share
`src/game/sound_sequence_events.c`, covering `0x8004BE6C` through
`0x8004C420` in their original definition order under `gcc_2_8_1_g0`.
The following running-status parser remains separate because its
`gcc_2_8_1_g8_split` profile differs. Grouping does not change the event bodies,
their local track view, or their external declarations.

`SD_ReadVariableLengthValue` uses the separate `SD_SEQUENCE_VLQ_*` constants
for seven-bit payload groups and their continuation bit. Its
`SD_SEQUENCE_VLQ_INITIAL_STOP` (`0xFF`) check applies only to the first byte:
that path marks the reader exhausted and returns zero. An `0xFF` encountered
inside the continuation loop contributes payload `0x7F` and continues
reading. The initial zero fast path and the original shift/add order remain
unchanged; these names do not turn the routine into a general validated VLQ
decoder or conflate its stop marker with a meta-event byte.

The driver-specific loop protocol uses `SD_SEQUENCE_CONTROL_MODE` (`0x63`)
with `SD_SEQUENCE_LOOP_START` (`0x14`) to save positions/registers and
`SD_SEQUENCE_LOOP_END` (`0x1E`) to restore them while a loop count remains.
The table-backed start path copies the incoming start byte into each track's
counter; the single-track path instead initializes `SD_SEQUENCE_LOOP_UNCOUNTED`
(`0x7F`). Positive counts below that threshold are decremented before the
restore, while counts at or above it are left unchanged. Zero bypasses the
restore.

`SD_SEQUENCE_CONTROL_DATA_ENTRY` (`6`) supplies a loop count while the channel
is in loop-start mode. The single-track case still exits before forwarding
that controller; the table-backed case keeps the existing forwarding path.
The channel-state handler excludes the two loop modes from its ordinary
data-entry dispatch. These mode values must not be confused with unrelated
controller IDs or object-state markers that happen to use the same numbers.

The controller staging bytes in `SDSecondaryRecord` are `parameter_selector`
at `+0x11`, `control_mode` at `+0x12`, and `control_value` at `+0x13`.
Controller `SD_SEQUENCE_CONTROL_PARAMETER_SELECTOR` (`0x62`) stores the
selector, `SD_SEQUENCE_CONTROL_MODE` stores the mode, and data entry stores
the value before the existing loop-mode exclusions are tested.
`func_8004ACE4` retains its raw byte view: selector
`SD_SEQUENCE_PARAMETER_REVERB_MODE` (`0x0F`) applies the staged value as the
reverb mode, and `SD_SEQUENCE_PARAMETER_REVERB_DEPTH` (`0x10`) applies its
existing left/right depth conversion. Unhandled selectors remain unchanged;
the work-area queries retain the check-versus-diagnostic contract described
above.

### Confirmed secondary-state fields

| Offset | Width | Field | Local matching-C evidence |
|---|---:|---|---|
| `0x0000` | `0x18` stride | `SDSecondaryRecord` channel view | `func_8004B49C`, `func_8004B6E8`, and `func_8004B70C` establish `program`, `pan`, `volume`, `expression`, `pitch_bend_msb`, and staged controller selector/mode/value bytes; `+0x06` and `+0x10` retain offset-based names. |
| `0x0180` | `0x28` stride | `objects[20]` | `func_8004A7C0`, `func_8004B49C`, and `func_8004C84C` establish the object base/stride; additional matched inline-assembly functions use the same view. Verified members are `channel_index` at `+0x03`, a byte at `+0x0F`, and a `u16` at `+0x1E`. |
| `0x04A4` | `0x1C` | `transfer` | `func_80049434`, `func_800496C4`, `func_8004975C`, `func_800497E0`, and `func_800498F8`. Members are `s16 +0x00`, pointer `+0x04`, `s32 +0x08/+0x0C/+0x10`, pointer `+0x14`, and bytes `+0x18`-`+0x1B`. |
| `0x0500`-`0x0502` | `u8` | `flag_0500`-`flag_0502` | Initialization, playback, update, and callback routines independently read/write these flags. |
| `0x0503` | `u8` | `event_guard` | `func_8004B854` prevents duplicate setup with it; shutdown leaves it set to block further event setup. |
| `0x0504` | `long` | `event_handle` | `func_8004B854` stores the `OpenEvent` result; `func_8004B910` disables and closes the same handle. |
| `0x0508` | `u8` | `field_0508` | `SD_SequenceTimerCallback` increments and wraps it at 11. |
| `0x0509` | `u8` | `field_0509` | `func_8004695C`, `func_80047050`, and `SD_SequenceTimerCallback` set/test it. |
| `0x050C` | callback pointer | `field_050C` | `SD_SequenceTimerCallback` conditionally invokes it. |
| `0x0510` | `s16` | `object_count` | Initialized/set by `func_80049434` and `func_80049600`; bounds the `0x28`-byte object scans in several matched functions. |
| `0x0512`, `0x0514`, `0x0516` | `s16` | `field_0512`, `field_0514`, `field_0516` | Initialization and parameter-update functions establish signed halfword accesses. |
| `0x07DC` | pointer | `field_07DC` | Playback copies `field_07E8` here; `SD_ReadSequenceByte` reads indexed stream bytes through it, and `SD_FindMidiTrackChunk` scans for `MTrk`. |
| `0x07E0`-`0x07E6` | four `s16` | `field_07E0`-`field_07E6` | Playback setup/reset and parameter functions consistently use halfword accesses. |
| `0x07E8` | pointer | `field_07E8` | `func_80049A64` stores the sequence/stream input pointer. |
| `0x07EC` | `s32` | `field_07EC` | Playback initializes the bound to `0x10000`; `SD_ReadSequenceByte` and `SD_FindMidiTrackChunk` compare reader offsets against it. |
| `0x07FA` | `u16` | `field_07FA` | `func_8004BE88` and `func_8004C77C` bound `0x2C`-byte work-record loops. |
| `0x07FC` | `u16` | `timebase` | `func_8004BE88` and `func_8004C5C8` select timing conversions from it. |
| `0x0800` | `u8` | `field_0800` | Cleared by `func_8004C77C`. |
| `0x0804`, `0x0808`, `0x080C`, `0x0810` | `s32` | offset-based fields | Timing/playback routines establish word accesses; their broader roles remain uncertain. |
| `0x0814`, `0x0815` | `u8` | offset-based fields | Initialization and update/output controls set/test these bytes. |
| `0x0818` | `u32` | `bytes_consumed` | `func_800496C4` clears it and `func_800497E0` advances it across a transfer window. |
| `0x081C` | `s32` | `field_081C` | Initialized to `0x1000`, read by update/termination paths, and set by `func_80049594`. |
| `0x0844`, `0x0845` | `u8` | offset-based fields | `func_8004ACE4` stores two control-event byte values. |

`func_8004B854` registers `SD_SequenceTimerCallback` for an interrupt event on
`RCntCNT2` with specification `EvSpINT` and mode `EvMdINTR`, stores the event
handle at `+0x504`, and enables it. `func_8004B910` later disables and closes
that handle. Both lifecycle paths now use the imported Psy-Q `libapi.h`
declarations and `kernel.h` constants; the remaining unnamed counter-control
wrappers retain their address-based identities.

The header uses GCC-2.8.1-compatible negative-array assertions for the
`0x18`, `0x28`, and `0x1C` subview sizes, the complete `0x848` state size, and
the major top-level offsets.

The adjacent envelope setters `func_8004A6F8` and `func_8004A764` share
`src/game/sound_voice_envelope.c`. They use the same `SpuVoiceAttr` block at
`+0x4C0`: one fills ADSR values from the caller's tone record, while the other
sets the existing defaults. Their original definition order and common
`gcc_2_8_1_cc_g8_as_g0_split` profile are preserved, as is the `const` table
declaration needed for the original address materialization.

### Transfer-window state and results

The leading halfword of `transfer` uses `SD_TRANSFER_STATE_INACTIVE` (`-1`)
as its inactive marker. `func_80049434` initializes it and `func_800498F8`
restores that marker. This is a software state marker, not a claim that a
hardware DMA transfer has completed.

`func_800496C4` checks for that marker when its caller requests an inactive
window, then prepares the window with state zero. `func_8004975C` and
`func_800497E0` retain their caller-supplied state comparisons. An operation
failure is `SD_TRANSFER_ERROR` (`-1`), while `func_800497E0` returns
`SD_TRANSFER_INCOMPLETE` (`-2`) when its consumed-byte count has not reached
the window length. A completed window returns the matched state token;
the state marker and result codes are separate roles despite sharing `-1`.

The transfer entrypoint remains `func_80077150`. The comparison material has
a documented [SpuRead/SpuWrite naming conflict](research/Unchiga_Symbols/NAMING.md#conflicts-31-rows-where-two-names-claim-one-address)
for that address. Neither the wrapper's old `SpuRead` comment nor an imported
name list resolves the direction; keep the address-based declaration until
the SDK identity is independently established.

### Sequence-input header tags

`func_80049A64` reads the input's first word and accepts these exact values
before recording a pending sequence input:

| Constant | Word value | Bytes on the little-endian target |
|---|---|---|
| `SD_SEQUENCE_TAG_SEQ` | `0x53455170` | `70 51 45 53` (`pQES`) |
| `SD_SEQUENCE_TAG_MIDI` | `0x6468544D` | `4D 54 68 64` (`MThd`) |
| `SD_SEQUENCE_TAG_KDT` | `0x2054444B` | `4B 44 54 20` (`KDT` followed by a space) |
| `SD_SEQUENCE_TAG_KDT1` | `0x3154444B` | `4B 44 54 31` (`KDT1`) |

These constants preserve the original word comparisons rather than relying
on multicharacter-literal byte order. The tag check is not full format
validation; its existing state guard, comparison order, and failure path
remain unchanged.

### Migration status and exact-code exceptions

Every matching-C user outside the GCC inline-assembly exceptions below now
includes `sound.h` and uses the shared typed global. Every accepted migration
batch retained the exact full executable hash.

Six matching-C functions containing GCC inline assembly remain unchanged and
keep their local raw declarations: `func_80049CF8`, `func_80049DD8`,
`func_8004A2F8`, `func_8004A854`, `SD_SequenceTimerCallback`, and
`func_8004C77C`.

Three migrated functions retain explicit raw indexing where the shared type
cannot replace the exact source shape:

- `func_800496C4` casts the shared state pointer to a byte view while
  initializing the transfer window and preserving its required register
  allocation.
- `func_8004A7C0` calculates the `0x28`-byte object address explicitly.
  Replacing it with `&D_8009B458->objects[index]` changes the linked
  executable at `0x8004A7C8`.
- `func_8004B49C` retains explicit byte-pointer arithmetic for the channel
  records and secondary objects, while stable fields use the shared types.

`func_8004BE88` likewise keeps a byte pointer for the still-unmodeled
`0x2C`-stride work-record region, but uses `SDSecondaryState` members for its
verified scalar fields. These raw expressions are layout/code-generation
views, not competing global declarations.

Unchiga's generated `m2c_types.h` and focused decompilation sources corroborate
the `0x848` clear size and several offsets, but the declarations above were
derived and exact-tested from this repository's matched C.
