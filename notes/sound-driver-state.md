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
| `0x044C` | `field_044C[2][32]` | Two halfword lookup banks, reset by `func_80047480`, populated by `func_80048D08`, and selected by the indirect voice-code decoders. |
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
The loader now consumes the shared `SDNote` and `SDValue` definitions directly;
its former private entry and partial-state views described the same fields and
widths. `SD_PENDING_ENTRY_NONE` (`0xFFFF`) keeps
the lookup-clear and skipped-ID marker distinct from the link-cache sentinel.
These note/pending constants live in the type-free
`sound_pending_constants.h`, re-exported by `sound.h`, so raw state views can
share the same definitions without importing incompatible declarations.
The input ID list begins at `SD_PENDING_INPUT_IDS_BYTE_OFFSET` (`0x08`) and
advances `SD_PENDING_INPUT_ID_ENTRY_SIZE` (`2`) bytes per halfword ID. Note
payloads begin at `SD_PENDING_INPUT_PAYLOAD_BYTE_OFFSET` (`0x1A0`); neither
offset is a note-record stride or an inferred capacity. Both cursors retain
their original advancement even for skipped IDs. Register pins,
pointer/count reloads, and the halfword rate adjustment remain unchanged.

The shared `SDSeqBlock` in `sound_pending_entries.h` is consumed by both the
pending-entry loader and `func_80048D08`. It has
`SD_PENDING_INPUT_ENTRY_CAPACITY` IDs and note records. That capacity is
derived from the ID region: `(0x1A0 - 0x08) / 2 = 204`, with an explicit
assertion rejecting partial-ID remainders. Its size is
`SD_PENDING_INPUT_BLOCK_SIZE` (`0x800`), and assertions tie its ID/data members
to the raw reader's offsets. `SD_PENDING_INPUT_BLOCK_SHIFT`
(`11`) preserves the two destination blocks' spacing. The copy helper keeps
`SD_PENDING_INPUT_COPY_GROUP_COUNT` (`64`) iterations of its original eight
word assignments, covering the same block. These names describe storage, not
new bounds: count-driven processing and the separate voice-lookup table's
dimensions remain unchanged.

That voice-lookup table has `SD_VOICE_LOOKUP_BANK_COUNT` (`2`) banks of
`SD_VOICE_LOOKUP_BANK_ENTRY_COUNT` (`32`) halfwords. The index mask derives
from the bank extent, while flag `0x100` selects the second bank through each
routine's original Boolean calculation. The byte shift (`6`) and derived
stride (`64`) agree with a bank's compiled size. The shared two-dimensional
view and private flattened views use the same derived total of 64 entries.
This does not change code tags, sentinels, the four dedicated voice slots, or
the pending-input block's capacity and count-driven population loops.

The missing-entry marker is shared by the pending-input and indirect-lookup
paths: `func_80048D08` retains each input key in the selected lookup bank even
when that key is `SD_PENDING_ENTRY_NONE`. Mapping initialization, table reset,
and decoder missing-result tests therefore reuse that marker. This does not
merge the lookup stages or change which routines perform the second mapping.
The independent link-cache sentinel, low-16-bit masks, and SPU attribute-mask
values remain separate.

The indirect path is selected by `SD_VOICE_LOOKUP_CODE_MASK` (`0xF000`)
and `SD_VOICE_LOOKUP_CODE_TAG` (`0x4000`). Compile-time relations keep the
tag within its mask and the code, index, and bank selector fields disjoint.
The existing earlier high-bit dispatch and each decoder's branch order are
unchanged. This names the code class; it does not add index validation or
reinterpret masks used by other sound-command families.

`SD_VOICE_LOOKUP_BYTE_OFFSET` fixes the table base at `+0x44C`; its exclusive
`SD_VOICE_LOOKUP_END_BYTE_OFFSET` derives from the two bank strides and is
`+0x4CC`. Assertions tie both boundaries to the shared member, while raw
accesses and local padding views reuse the integer byte offsets without
changing address arithmetic, array extents, or the following fields.

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

`func_80045054` (a candidate since #3859, src/candidates/func_80045054.c) imports the real `libspu.h` interface and calls:

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
`SD_VOICE_SLOT_FIRST_VOICE` ties that hardware-voice base to the existing
key-bit shift, and `SD_VOICE_SLOT_COUNT` bounds the four slot scans and their
per-slot arrays. The raw views and register pins remain local; the four-halfword
scratch buffer in `func_80048768` is still one envelope-call destination, not
a per-slot array. All four query routines use the real Psy-Q `libspu.h`
`void SpuGetVoiceEnvelope(int, short *)` declaration.

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

Eighteen accesses in nine files retain an explicit byte-pointer expression
(`git grep -nE '\(u8 \*\) *g_SDValue' -- src`), nine of them in resident
sources and nine in build-integrated candidates. Three forms, counted by
what the cast applies to:

- a cast on the pointer combined with an offset or index (twelve; six in
  `sd_init_state.c`, and one of the twelve is passed as a call argument
  rather than dereferenced);
- a base local assigned `(u8 *)g_SDValue` (five, three of them in
  `func_80045054.c`);
- a cast on a member's value (one, `func_80047DB0.c:30`).

This note records a code-generation rationale for exactly one of the
eighteen. `func_80045054`'s cast is quoted in the SPU section above for the
layout of `SpuDecodedData` rather than for its spelling, and the rest are
undocumented here. Of the three exceptions the note documents, only
`func_800493F8` is still a byte-pointer access at all:

- `func_800493F8` writes the music-track pointer through
  `((u8 *)g_SDValue + 0x1564)` because the direct member assignment changes
  register allocation. It is `src/game/sound_init.c:86`, and the source
  carries the measurement in a comment above the store.
- `func_80047FAC` was documented as indexing the four voice IDs as
  `((u8 *)g_SDValue + index * 2 + 0x404)`. The source spelled that
  `((u8 *)g_SDValue + s0 * 2 + 0x404)`, and `ac4e0662` ("Coalesce the
  sound-effect voice slots (#2740)") replaced it with
  `g_SDValue->voice_ids[s0]` -- `src/game/sound_effect_voices.c:26`, and
  the function matches. Only the description is stale; the three-instruction
  cost it claimed was not re-measured against the current source.

`func_800493F8`'s expression is an exact-code-generation view of a field
whose offset and type are defined by `SDValue`. The rest are byte-pointer
arithmetic over the same layout, with no reason for the spelling recorded
here.

All 26 resident `.c` files that name `g_SDValue` include `sound.h`, and so
do all 22 build-integrated candidate `.c` files naming it. None of them
declares the pointer itself any more. `func_80045514.c` still defines a private struct for the
*pointee* and reaches it by casting the header's declaration, which is a
different statement and is the subject of the paragraphs below.
`func_80046294.c` did the same until 2026-09-11; what removed it is
recorded there too.

As historical context, the sentence this replaces -- "All pure-C
`g_SDValue` users now include `sound.h`" -- was written on 2026-09-02, and
both of the candidates that broke it were built in afterwards,
`func_80046294` on the 9th (#2992) and `func_80045514` on the 10th
(#3359). Nine additional functions use the shared command queue,
buffer pointers, voice arrays, flags, and late control fields directly.

`func_80046294` took the header's declaration back but kept a private
struct for the pointee, and the reason was a measurement rather than a
preference. That is **resolved as of 2026-09-11**; the eliminations below
are kept because they are what made the answer findable, and the answer is
the last paragraph of this section. With `sound.h` included and its `G_SDVALUE_IN_DATA`,
`FUNC_80049F50_RETURNS_S16` and `SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG` arms
selected, so that the unit declares none of those three symbols itself, the
candidate's object is byte-identical. Pointing the same reads at `SDValue`
instead is not: the object changes. What that is not caused by was measured
one at a time. Every offset the unit reads -- `flags_0040` at 0x40,
`command_count` at 0x4C, `field_007C` and `field_007D`, `field_157E`, and
`commands` at 0x80 -- asserts at the offset the private struct places it, and
the three halfword widths assert equal. Padding the private struct out to
`SDValue`'s 0x164C changes nothing. Giving the private struct alignment 4,
by writing its leading `u8 pad00[0x40]` as `u32 pad00[0x10]`, changes
nothing either. And two spellings of the byte view over the command queue
give the same moved object as each other, which is this project's own tell
for a wrong axis.

A word-typed member in a region the unit never reads changes nothing
either. Writing `u8 pad4E[0x7C - 0x4E]` as `u8 pad4E_b[2];` followed by
`u32 pad4E_w[11];` -- bytes first, so the word array lands on its own
alignment at 0x50 and every later field keeps its offset -- leaves the
object byte-identical as well.

That last control has to state its field order, and the reason is that the
first version of it got the order wrong. Written the other way round, with
the word array first at 0x4E, natural alignment inserts two bytes before it
and everything after moves: `f7C` to 0x7E, `entries` to 0x82, `f157E` to
0x1580, and the struct from 0x1580 to 0x1584. That spelling does produce a
different object, and a paragraph here briefly said so as evidence that
unread member types matter. It was not evidence of that at all -- it moved
five live fields. The claim is withdrawn.

What settles it is seven assertions compiled by the target compiler rather
than a byte count: `f7C` at 0x7C, `f7D` at 0x7D, `entries` at 0x80,
`f157E` at 0x157E, `flags` at 0x40, `command_count` at 0x4C, and
`sizeof` 0x1580. All seven hold under the bytes-first spelling. Under the
words-first spelling five of them fail to compile, which is what makes them
a test rather than a formality.

Six eliminations and no positive result, and the seventh measurement is
what answered it -- by asking a different question. Every elimination above
varies the private struct while holding the *access* fixed. Holding the
access fixed the other way round, and varying the struct, is one build
each:

| access spelled | over the private struct | over `SDValue` |
| --- | --- | --- |
| `((u8 *)X)[j]` | `0bd53f65...` | `0bd53f65...` |
| `((SDCommand *)((u8 *)X + j))->command` | `c9c3f84b...` | `c9c3f84b...` |
| `X[i].command` | `6d0bf0da...` | `778a511e...` |

The first two rows are byte-identical across the two structs, so **the
pointee type never was the difference**. The paragraph above that reads two
agreeing byte-view spellings as "this project's own tell for a wrong axis"
had the inference backwards: the axis is the access, the two spellings
agreed because both are casts, and a third and fourth spelling of the same
access give two further distinct objects.

What the original load is, and what no cast reproduces, is an `ARRAY_REF`
of a `u8` member -- `p->entries[j]` on the private struct's byte array.
`SDValue` had no such member at 0x80, because it declares the queue as
`SDCommand commands[16]`. Giving it one does it: `commands` is now a union
of `c` (the typed array every dispatcher uses) and `b` (the byte view this
unit uses), which is the idiom `display_object.h` already uses eleven
times, and `p->commands.b[j]` builds `8d64e380...` -- the object the private
struct produced. Both private types are gone from that unit: `SoundEntry`,
its 0x30-byte copy record, is `SDCommand` and was neutral on its own before
any of this, and `SoundState` is `SDValue`. The `SOUND_STATE` macro that
cast the header's pointer to the private shadow is gone with them.

The eliminations were not wasted -- they are what left the access as the
only variable -- but the general lesson is cheaper than six of them: when a
type substitution moves an object, vary the ACCESS with the type held
fixed before varying the type any further.

`func_80049138` is a third deliberate exception and is no longer a raw
view. The global pointer is volatile in that routine, which the unit
selects by defining `G_SDVALUE_VOLATILE` -- an arm of `sound.h`'s own
declaration chain rather than a suppression of it -- and `sound.h` carries
the measurement. Its accesses are typed members (`p->music_track`,
`q->flags_0040`, `q->field_1560`); the `u8 * volatile` redeclaration and
the offset expressions this note described were removed by `98f79757`
("Take every g_SDValue declaration from sound.h (#2610)") and `548c78cb`
("Reach the sound driver's state block through SDValue, not byte offsets
(#2500) (#2759)").

Existing GCC constraints remain unchanged by layout migrations. The secondary
spatialization contract below also covers callers containing constraints,
without changing or adding those devices.

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
`SD_DispatchSequenceChannelEvent` selects a channel with the MIDI status byte's low nibble and
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

The matched pitch refresh also establishes object `voice_index` at `+0x00`,
`note` at `+0x06`, the positive and negative pitch-bend scales at `+0x10` and
`+0x11`, and the cached channel bend value as signed halfword
`cached_pitch_bend` at `+0x1A`. The bytes at `+0x12` and `+0x13` are passed to
the pitch conversion, but their individual roles remain unresolved and keep
offset-based member names.

The channel-message writers and matched gain/pitch readers establish these
channel controls without changing their byte storage:

| Offset | Member | Evidence |
|---|---|---|
| `+0x00` | `program` | Program-change dispatch calls `func_8004B6E8`, which stores its program byte here. |
| `+0x01` | `pan` | Controller `0x0A` writes it; `SD_SpatializeSecondaryObject` includes it in the pan sum. |
| `+0x03` | `volume` | Controller `7` writes it; `SD_SpatializeSecondaryObject` multiplies it into the level. |
| `+0x05` | `expression` | Controller `0x0B` writes it; `SD_SpatializeSecondaryObject` applies it as another level factor. |
| `+0x07` | `pitch_bend_msb` | Pitch-bend dispatch passes the second data byte to `func_8004B70C`; `func_8004A43C` caches it and obtains the pitch adjustment through `SD_CalcPitchBend`. |

The pan writer still substitutes `1` for an incoming zero. Pitch bend still
stores only the second data byte masked to seven bits; the first data byte
remains unused. These names do not add full fourteen-bit bend handling or
change the raw gain/pitch readers. In particular, the cached `+0x07` value is
pitch bend, not a bank byte.

The stored bend MSB uses `SD_SEQUENCE_PITCH_BEND_MSB_MASK` (`0x7F`), separate
from pan and other seven-bit fields. `SD_CalcPitchBend` narrows its input to a
byte and returns zero at `SD_SEQUENCE_PITCH_BEND_CENTER` (`64`). Below center
it uses the object's `pitch_bend_negative_scale` at `+0x11` and the distance
from `64`; above center it uses `pitch_bend_positive_scale` at `+0x10` and subtracts
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

### Shared secondary spatialization contract

`sound_spatialize_object.h` now declares both parameters with the existing
`SDSecondaryObject` and `SDSecondaryRecord` types from `sound.h`.
`SD_SpatializeSecondaryObject` at `0x8004A0FC` uses those members throughout,
including typed `SDSecondaryState` root reads. The two matching callers
(`SD_UpdateSecondaryObjectVolumes` and `func_8004B49C`) and retained
`func_8004ADE8` candidate consume the same prototype. Their original byte-stride
argument calculations remain where needed; the parameter ABI is still two
32-bit pointers.

The layout evidence predates this conversion: the controller writer stores
channel pan, volume and expression, while the note-start candidate fills
object `+0x08/+0x09` from program/tone gain bytes and `+0x0A/+0x0B` from
program/tone pan bytes, then stores velocity at `+0x0E`. The candidate now
uses the shared object members for those five stores and the two result
loads. The kernel writes pan at `+0x0C` and unsigned levels at `+0x14/+0x16`;
the caller forwards those levels to `SD_SetVoiceVolume`. Existing
offset-based names remain: this does not assign stronger VAB or transfer
semantics to the partial layouts.

Six additional compile-time checks cover fourteen offsets: the object
gain/pan/result members, state `transfer.field_0018` at `+0x4BC`,
`transfer.field_001B` at `+0x4BF`, the halfwords at `+0x512/+0x7E4/+0x7E6`,
and the override byte at `+0x815`. Existing checks cover the channel members,
object count, array bases, strides and complete state extent.
The kernel explicitly converts `field_0512` to `u16`, preserving its unsigned
retail load despite the shared signed storage view. The `+0x7E4/+0x7E6`
loads stay signed, and both volatile reads of object `field_000E` remain
separate. Root reloads, arithmetic order, shifts and truncating stores are
unchanged.

The remaining private `D_8009B458` declarations now live in `sound.h`.
Residents use the ordinary typed pointer; `SDSECONDARYSTATE_AS_BYTES` keeps
the candidate's original byte pointer. The opt-in
`SDSECONDARYSTATE_BYTE_ALIAS` and `SDSECONDARYSTATE_RELOAD_ALIAS` arms preserve
the existing timer byte-store and candidate root-reload compiler identities.
Both aliases still resolve to `D_8009B458`; no new alias, storage definition,
linker assignment or volatile global is introduced. The candidate also uses
the existing twenty-entry `D_80011434` contract instead of its private
incomplete declaration. Only those two obsolete private-extern dependencies
are removed from its metadata; all nineteen candidate object fingerprints,
targets and profiles remain unchanged, and the note-start candidate is still
a near miss.

The volume sweep retains four raw reads: two channel-index byte reads at
state-plus-byte-stride `+0x183`, and the unsigned result pair at
`+0x194/+0x196`. Its root, object-count reads and channel argument are typed.
Under the recorded `gcc_2_8_1_g0` profile, object-array pointer expressions
added eight bytes; indexing with the loop counter added sixteen; a
field-relative channel cursor added four. Keeping that channel cursor raw
but making the result loads field-relative restored size yet reversed the
two source registers of an address addition at `0x8004A36C`. The accepted
read expressions preserve both instruction bytes and the existing induction
variables/constraints. The kernel itself needs no raw offset access and
still uses its recorded `gcc_2_8_1_g0_no_cse_follow_jumps` profile.

The same header names the event codes consumed by `SD_ReadSequenceEvent`,
`SD_DispatchSequenceChannelEvent`, and `SD_HandleSequenceMetaEvent`. A status-present bit, a message-type mask,
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
`src/game/sound_sequence_parser.c` with the fixed-width readers and the header
reader, covering `0x8004BC2C` through `0x8004C420` in their original
definition order under `gcc_2_8_1_g0`.
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
| `0x07FA` | `u16` | `track_count` | `SD_HandleSequenceMetaEvent` and `SD_StartSequenceTracks` bound `0x2C`-byte work-record loops. |
| `0x07FC` | `u16` | `timebase` | `SD_HandleSequenceMetaEvent` and `SD_ScaleSequenceDelta` select timing conversions from it. |
| `0x0800` | `u8` | `field_0800` | Cleared by `SD_StartSequenceTracks`. |
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

The adjacent envelope setters `func_8004A6F8` and `func_8004A764` shared
`src/game/sound_voice_envelope.c` until #3859 moved them to
`src/candidates/func_8004A6F8.c` and `src/candidates/func_8004A764.c`,
because that profile's compiler and assembler thresholds disagree. They use
the same `SpuVoiceAttr` block at
`+0x4C0`: one fills ADSR values from the caller's tone record, while the other
sets the existing defaults. Both matched only at the common
`gcc_2_8_1_cc_g8_as_g0_split` profile, with the `const` table declaration
needed for the original address materialization.

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

The transfer entrypoint is `SpuWrite`. The signature catalogue proposes both
`SpuRead` and `SpuWrite` because their small public wrappers are byte-identical,
but the resident body calls confirmed internal `_spu_Fw`; adjacent `_spu_Fr`
is the read helper. Both matching callers set the SPU transfer-start address,
then submit a source buffer and require the full byte count back. The direction
therefore comes from the local call graph rather than an imported name.

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
`SD_UpdateSecondaryObjectVolumes`, `func_8004A854`, `SD_SequenceTimerCallback`, and
`SD_StartSequenceTracks`.

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

`SD_HandleSequenceMetaEvent` likewise keeps a byte pointer for the still-unmodeled
`0x2C`-stride work-record region, but uses `SDSecondaryState` members for its
verified scalar fields. These raw expressions are layout/code-generation
views, not competing global declarations.

Unchiga's generated `m2c_types.h` and focused decompilation sources corroborate
the `0x848` clear size and several offsets, but the declarations above were
derived and exact-tested from this repository's matched C.
