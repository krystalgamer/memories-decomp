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
| `0x0040` | `status_flags` | `Sound_InitFrontend` sets startup bits `0x0A` and polls bit `0x08` until it clears; other control and cleanup routines use the remaining status bits. |
| `0x0042` | `mix_scale` | CD mix calculation uses it as a 16-bit scale. |
| `0x0044` | `secondary_mix_scale` | Base scale multiplied by the two fade levels before publishing secondary stereo master levels. |
| `0x0048` | `output_type` | `SD_SetOutputType`; live stereo/mono traces establish values 0 and 1. |
| `0x004A` | `data_source_flags` | Initialization and command processing use independent bits. |
| `0x004C` | `command_count` | Bounds the 16-entry command scan. |
| `0x0080` | `commands[16]` | 16 records, each `0x30` bytes; command at `+0x00` and eight verified 32-bit argument/result slots at `+0x10`-`+0x2C`. |
| `0x0404` | `voice_ids[4]` | Four 16-bit voice identifiers. |
| `0x040C` | `field_040C[4]` | Per-slot byte priorities, assigned by `SD_StartSoundEffectVoice`; the allocator accepts an occupied slot when this value is no greater than the request's priority. |
| `0x0424` | `voice_value[4]` | Four per-voice byte values reduced by `voice_step`. |
| `0x0428` | `voice_step[4]` | Four per-voice decrement values. |
| `0x042C` | `voice_timer[4]` | Four 16-bit countdown timers. |
| `0x0434` | `voice_active_mask` | One bit per voice entry. |
| `0x0435` | `field_0435` | Four-slot rotating allocation cursor; fallback attempts increment and mask it even after selecting a voice. |
| `0x0438` | `field_0438` | Cursor initialized from the first link entry and advanced by each selected entry's second word. |
| `0x0442` | `field_0442` | Selected link-table index; reset to `0xFFFF` and used to suppress duplicate requests. |
| `0x0448` | `field_0448` | Pointer to the 8-byte `SDValueLink` table used by pending sound-data requests. |
| `0x044C` | `field_044C[2][32]` | Two halfword lookup banks, reset by `SD_InitVoiceState`, populated by `SD_LoadSequenceBankPair`, and selected by the indirect voice-code decoders. |
| `0x0510` | `cd_volume` | Sound output changes recalculate and store this signed 16-bit value. |
| `0x0514` | `channel_volume[2]` | Two byte channel-volume scalars. |
| `0x0533` | `mix_multiplier` | Multiplies the shared CD mix scale. |
| `0x0538` | `decoded_half` | `SD_MeasureCdAudioLevel` stores `SpuReadDecodedData`'s returned half here. |
| `0x053C` | `decoded_data` | Complete `0x1000`-byte SDK `SpuDecodedData` record. |
| `0x153C` | `decoded_cd_halves[4]` | Signed-halfword pointers to the first/second left-CD halves, then first/second right-CD halves. |
| `0x154C` | `output_level` | `SD_MeasureCdAudioLevel` sums squared CD samples into it and publishes its signed high halfword as the level. |
| `0x1560` | `field_1560` | Base pointer used to select a music/sequence table entry. |
| `0x1564` | `music_track` | Pointer defaults to `0x801EA800`; its first 16-bit value is initialized to `0xFFFF`. |
| `0x1618` | `busy` | Command registration tests and sets this byte. |
| `0x1649` | `cd_mix_default` | Cached CD mixer trailer byte, seeded to `0xFF` and restored into `mix_scale` by runtime reset. |
| `0x164A` | `secondary_mix_default` | Cached secondary mixer trailer byte, seeded to `0xD2` and restored into `secondary_mix_scale` by runtime reset. |

The remaining named `field_XXXX` members have verified offsets and widths but
insufficient semantic evidence for stronger names.

### Runtime status and data-source configuration

The status getter, cold initializer, source-LBA initializer, request
wrappers and active-command/stream callbacks establish two separate
flag domains in `SDValue`:

| Offset | Member | Retained storage and role |
|---|---|---|
| `+0x0040` | `status_flags` | `u16` runtime bookkeeping returned through the existing signed-halfword status getter. |
| `+0x004A` | `data_source_flags` | `u8` startup-bank and public request configuration, not current transfer status. |

`SD_GetStatusFlags` still returns `(s16)status_flags`, not a boolean or an
unsigned normalized result. `SD_InitState` clears the runtime word after
its existing subsystem initialization, and `SD_ResetRuntime` clears it
without resetting the configuration byte.

The observed status bits have different producers and lifetimes:

| Bit | Observed operations; limits on interpretation |
|---|---|
| `0x01` | CD play/seek wrappers set it and clear `0x04` after attempting enqueue. Queue-full failure is still ignored, so this is not proof of a submitted or playing request. |
| `0x02` | Data-transfer and value-link/package request paths set it. Command `0x20` clears the low two bits only when no queued stream command remains; it is not a hardware-DMA-busy oracle. |
| `0x04` | `SD_FadeInStreamAudio` clears `0x01`/`0x02` and sets it; `SD_FadeOutStreamAudio` clears it. Its lifetime is not the CD fade-step lifetime. |
| `0x08` | `SD_InitDataSourceFlags` raises it; `SD_VSync` gates bank-loader service with it and `Sound_InitFrontend` polls it. The loader default clears it only when the low three status bits are clear. |
| `0x80` | Command `0x48` sets it after a zero sequence-open result. It gates several sequence update, fade and cleanup paths, not an unconditional guarantee of SDK playback state. |

Keep each existing clear mask and polling comparison. In particular,
`SD_UpdateRuntime` tests `SD_PollSequenceState() != 1`, whereas
`SD_CancelMusicRequests` tests `== 1` before its stop call; these paths
are not normalized into one interpretation of the status bit.
`SD_MeasureCdAudioLevel` still computes/stores its measurements but returns
zero when `status_flags & 3` is nonzero.

The configuration bits are established by their actual gates:

| Bit | Observed gate |
|---|---|
| `0x01` | The SE-bank startup request in loader phase 0. |
| `0x02` | The music-table startup request in phase 6 and the public BGM preload/play wrappers. |
| `0x40` | Permits CD play/seek codes above `0x9FFF` through their existing wrapper checks. |
| `0x80` | The CD descriptor-bank request in phase 8 and the CD play, seek and stop wrappers. |

`SD_InitState` seeds the byte to 3 and optionally ORs `0xF0`.
`Sound_InitFrontend` passes the SE, BGM and MASTER.XA LBAs to
`SD_InitDataSourceFlags`; zero arguments clear `0x01`, `0x02` and `0x40`,
respectively, while nonzero arguments do not set them. That initializer
also resets `bank_load_phase` and ORs `0x0A` into the status word.
The XA-zero clear is deliberately `0x40`, not the loader/wrapper `0x80`
gate. Do not repair that difference or assign meanings to the other bits.

`tent_SD_SetDataSourceFlags` retains `(old & 0xF0) | value` before the
existing byte-store truncation. Its value is not masked to four bits:
the name does not promise a strict low-nibble setter, and the function
keeps its tentative qualifier. The field names introduce no enum,
bit-constant substitution, queue-success check, initialization change,
mask adjustment, cast removal or scheduling change.

## Secondary sequencer reverb controls

The two trailing control-byte mirrors are now reverb_mode_control
(former field_0844, u8 at +0x844) and reverb_depth_control
(former field_0845, u8 at +0x845). The complete secondary state still
has its original size and trailing padding. No storage or reset is added.

The private sequence-controller path establishes these roles, not a guessed
SDK register layout. SD_DispatchSequenceChannelEvent forwards the existing
Control Change message. Controller 0x62 selects the parameter; controller
0x63 selects the control mode. Data Entry controller six stores control_value
and invokes SD_ApplySequenceReverbParameter only when control_mode is neither
the loop-start code 0x14 nor loop-end code 0x1E. Existing loop handling occurs
before the call and is not folded into this reverb path.

Parameter selector 0x0F retains the complete SDK sequence: disable reverb
only if SpuGetReverb reports SPU_ON; conditionally release reserved work
area; submit SPU_REV_MODE and the channel's control value; conditionally
reserve after the diagnostic check; turn reverb on; THEN copy a fresh
control_value into reverb_mode_control through the freshly read state pointer.
All checks, return-value treatment, calls and repeated reads remain.

Selector 0x10 retains SPU_REV_DEPTHL | SPU_REV_DEPTHR, both original
control_value << 8 expressions, SpuSetReverbModeParam and SpuSetReverb(SPU_ON),
then a fresh unshifted control-byte copy into reverb_depth_control.
The stored byte is not the signed SDK depth halfword or a measured gain.
No clamp, saturation, common packet initializer or rewritten shift is added.

These are POST-CALL mirrors, not proof of accepted SPU settings, initialized
defaults or a successful allocation. SDK return values remain ignored where
they were ignored. Opaque calls separate packet preparation from the final
control-byte reads: do not cache the input or require a mirror to equal the
value previously put into the packet. No local reader of either mirror was
found, and naming does not invent a GUI, trace observation or gameplay use.

The channel's separate reverb_mode byte remains its existing note-on policy:
zero defers to the tone mode, one disables reverb, other values enable it.
It is not renamed, merged with the global mode parameter or normalized.
Unknown secondary-state/channel fields retain their offset names.

The whole channel-event source changes ONLY two qualified member tokens.
All fixture/control/oracle/scenario/assertion/mutation/numeric bytes,
frozen overlays, SDK declarations and existing profiles remain unchanged.
There is no new harness or local fixture/cloud execution.

## Sound-effect voice allocation

### Count and selected-slot start helpers

`SD_CountActiveSoundEffectVoices` (`0x80047FAC`) narrows its input ID to
`u16`, queries the envelope of each of the four sound-effect SPU voices,
and counts only matching `voice_ids` whose observed envelope is nonzero.
This is not a count of matching stored IDs alone: a decayed voice does not
count even if its old ID remains in the slot.

`SD_StartSoundEffectVoice` (`0x8004803C`) starts an already-selected slot.
It checks the note-index sentinel, derives left/right volumes and pan
attenuation, adds the pitch adjustment, prepares the SPU voice attribute,
and keys the voice off before publishing the slot's state and calling
`SpuSetKeyOnWithAttr`. The stored state includes the ID, mode/flags,
selection priority, volume pair, fade state and timer. A missing note-map
entry returns without starting playback.

These names do not move allocation into the start helper: the caller below
chooses the slot and performs voice stealing. The public parameter widths,
low-halfword ID behavior, envelope-call order and SPU-call order remain
unchanged. `sound_voice_selection.h` owns both declarations; the allocator's
scripted callee and declaration-visibility witness use the same start name.

### Slot allocator

`SD_AllocateSoundEffectVoice` owns a separate 936-byte matching translation unit. Its
`sound_voice_allocator.h` declaration retains a full-width incoming ID and
mode, signed halfword pitch and pan, and byte volume and selection value.
The matching `SD_SEPlay` request path consumes this declaration directly.
`sound_voice_selection.h` owns the
keyed-group query and the adjacent count/start pair.

The allocator rejects missing low-16-bit index-table entries before handling
bit `0x8000`. Otherwise it first selects the lowest keyed-group slot, then
the requested occurrence of an ID with a nonzero envelope, then searches
from the rotating cursor for a silent slot or an eligible occupied slot.
Group/occurrence selection leaves the cursor at the selected slot; the
fallback walks advance it after every attempted slot, including success.
Envelope calls precede the ID reads and can change which state is observed.

The saved ID is `u16`, independently of the original input's stop-bit test.
A clear bit `0x8000` does not bound a full-width input below `0x10000`; the
voice comparison must still use the low halfword. Sharing one scratch between
the envelope and ID observations keeps the narrowing inside the loop.
The signed halfword mode snapshot is masked through an unsigned view, and the
occurrence limit is a byte. Together with the shared voice mask, these choices
reproduce the register allocation under uniform `gcc_2_8_1_g8_split`.

The ILP32 witness exercises 1,004 cases at O0 and O2 and rejects five semantic
mutations. Its expected call/state observations were also compared with a
bounded retail MIPS execution. These are scripted callee models with valid
cursor and lookup storage, not SPU hardware or invalid-pointer emulation.

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
`gFile_anLba[4]`, `[5]`, and `[6]` to `SD_InitDataSourceFlags`. The runtime file table
identifies those positions as `SD_SE.DAT`, `SD_BGM.DAT`, and `MASTER.XA`.
`SD_InitDataSourceFlags` clears `bank_load_phase`, clears bits `0x01`, `0x02`, or `0x40` in
`data_source_flags` when the corresponding file position is zero, and sets
`status_flags |= 0x0A`. The frontend then calls `Main_AdvanceFrame` while
`SD_GetStatusFlags` continues to expose bit `0x08`, making that bit the
game-facing startup-busy condition.

Matching `SD_QueueValueLinkTransfer` and its adjacent callers establish the link-table
lifecycle. `SD_ResetPendingEntries` resets `field_0442` to `SD_VALUE_LINK_INDEX_NONE`
(`0xFFFF`) and initializes
`field_0438` to the first link's second word plus `0x1010`.
`SD_ReplaceValueLink` ignores a requested index when that entry's second word
is zero or when the same index is already selected. Otherwise it keys off
every voice whose `voice_ids` entry is at or above `dynamic_note_start` through
`SD_KeyOffDynamicVoices`, calls `SD_ResetPendingEntries`, and stores the
accepted index in `field_0442` before dispatch. `SD_RequestValueLink` is the direct request
path: it suppresses only a duplicate selected index, then stores the new
index, marks flags bit 1, and calls `SD_QueueValueLinkTransfer`.

`SD_QueueValueLinkTransfer` applies `SD_VALUE_LINK_INDEX_MASK` (`0xFFFF`) and
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

### Sound-note priority and mode

`SD_SEPlay` now holds its selected `field_0444` entry as `SDNote *` instead
of a byte pointer and reads `priority` and `mode`, not offsets 2 and 3.
The former `pad0002[2]` is two observed fields, with asserted byte offsets
and the unchanged eight-byte record size. Entry selection retains the
integer address-sum expression and uses `SD_NOTE_RECORD_SIZE`: ordinary
array indexing changed four instructions in the direct-ID branch despite
preserving the executable size.

The names follow the complete resident call chain:

- `SD_SEPlay` passes `priority` as the sixth argument of `SD_AllocateSoundEffectVoice`.
  When no silent voice is available, that allocator accepts an occupied
  slot only if the incoming value is greater than or equal to the byte in
  `field_040C[slot]`. `func_8004803C` stores the accepted value there.
  Thus a larger value can replace an equal- or lower-priority voice; this
  field is not padding or a volume component.
- `mode` is the fifth argument of the allocator. Its low nibble requests a
  keyed group through `SD_KeyOffVoiceGroup`; its high nibble chooses an occurrence
  of the same sound ID. The byte is also forwarded to the voice-start
  helper and stored in `voice_flags`. The name does not claim that every
  bit is an independent Boolean or equate this byte with an SPU register.

Both the direct-ID and indirect-lookup paths use the same typed entry.
Their consumed-ID snapshot chain, stop-bit handling, lookup sentinels,
volume narrowing, pan narrowing, and byte load widths are unchanged.
The separate state-base-first halfword lookup in `SD_SESetPan` retains
its documented code-generation constraint.
The input ID list begins at `SD_PENDING_INPUT_IDS_BYTE_OFFSET` (`0x08`) and
advances `SD_PENDING_INPUT_ID_ENTRY_SIZE` (`2`) bytes per halfword ID. Note
payloads begin at `SD_PENDING_INPUT_PAYLOAD_BYTE_OFFSET` (`0x1A0`); neither
offset is a note-record stride or an inferred capacity. Both cursors retain
their original advancement even for skipped IDs. Register pins,
pointer/count reloads, and the halfword rate adjustment remain unchanged.

The shared `SDSeqBlock` in `sound_pending_entries.h` is consumed by both the
pending-entry loader and `SD_LoadSequenceBankPair`. It has
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
paths: `SD_LoadSequenceBankPair` retains each input key in the selected lookup bank even
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

`SD_MeasureCdAudioLevel` (grouped in `src/game/sound_output_state.c`) imports the real
`libspu.h` interface and calls:

```c
SpuReadDecodedData(
    &g_SDValue_output_level->decoded_data, SPU_CDONLY
);
```

It stores the returned half, `SPU_DECODED_FIRSTHALF` or the second half, in
the word `decoded_half` at `+0x538`, and sums the selected half's 256 signed
sample squares, each shifted right by eight, into `output_level` at `+0x154C`.
It then publishes that sum's signed high halfword as both the new
accumulator and the return value, unless `status_flags & 3` mutes it to zero.
The neighbouring word `field_1550` gets the same treatment from its own high
halfword, but nothing in C accumulates into it. Both words are `SDLevelWord`
unions (`sum` and `halves[2]`), so the high-halfword reads stay member reads.

`SpuDecodedData` is four arrays of `0x200` signed halfwords (`cd_left`,
`cd_right`, `voice1`, and `voice3`). `SDValue.decoded_data` now records that
complete `0x1000`-byte object at `+0x053C` through `+0x153B`, replacing the
old `0x800`-byte CD array and following `0x800` bytes of padding. The voice
arrays occupy that former padding; this path still requests `SPU_CDONLY`,
without newly initializing or reading those voice arrays.

The old four `0x200`-byte buffers were halves of two CD channels, not four
complete SDK channels. `SD_InitBufferState` keeps their exact addresses in
the four stored `s16 *G32 decoded_cd_halves` pointers:

| Pointer | SDK samples | Relative state address |
|---|---|---|
| `[0]` | `cd_left[0..255]` | `+0x053C` |
| `[1]` | `cd_left[256..511]` | `+0x073C` |
| `[2]` | `cd_right[0..255]` | `+0x093C` |
| `[3]` | `cd_right[256..511]` | `+0x0B3C` |

The initializer still resets pan, applies CD volume 255, stores the first
three pointers, clears `cd_fade_step`, and only then stores pointer three.
The measurement chooses pointer zero only for `SPU_DECODED_FIRSTHALF` and
pointer one otherwise, including any non-first return value. It still
measures only the left CD channel; the two right-channel pointers remain
installed but unused by the tracked measurement. This is not a stereo mix
or a new error-handling path.

The SDK call takes the typed record address without a cast. The sample
walkers also no longer need casts because their stored pointers already
have the existing signed-halfword sample type. The volatile pointer view,
snapshot/reload boundaries, sample count and increment, unsigned square
shift, accumulator initialization/publication, high-halfword ordering,
single-iteration scope and low-two-bit output gate are unchanged.
`field_1550` remains unnamed: recovering the SDK record does not establish
that otherwise unaccumulated word as a right-channel level.

Assertions pin all four SDK member addresses, the full record size/end at
the pointer array, and the SDK channel's two `SD_MIX_SAMPLE_COUNT` halves.
Existing level and complete-state offsets and sizes remain pinned. No
extra union, scratch buffer, SDK declaration, local raw view, bound, clamp,
callback contract, test case or harness is introduced.

### Reverb work-area queries

Matching `SD_ApplySequenceReverbParameter` handles two sound-sequence control
entries:

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
`SD_Shutdown` calls `SD_ResetSecondaryRuntime`, disables the SPU IRQ with
`SpuSetIRQ(0)`, and then calls `SpuQuit`.

## SPU transfer and voice interfaces

Matching sound initialization selects transfer mode zero with
`SpuSetTransferMode(0)`. The secondary transfer path then calls
`SpuSetTransferStartAddr` with either the configured SPU RAM base or that base
plus `bytes_consumed` before advancing a transfer window. The status wrapper
`SD_VabTransCompleted` (`0x800498BC`) forwards caller value zero as
`SpuIsTransferCompleted(SPU_TRANSFER_PEEK)` and every nonzero value as
`SpuIsTransferCompleted(SPU_TRANSFER_WAIT)`, the poll-or-block contract of
libsnd's `SsVabTransCompleted`. Nothing in the executable or the overlay
archives calls it.

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
scratch buffer in `SD_SESetPan` is still one envelope-call destination, not
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

The contiguous output/control block at `0x80046DE8-0x80047458` now builds as
`src/game/sound_output.c`. Its eighteen functions use `SDValue` and `SDCommand`
directly, including output-type reads/writes, driver flags, CD-volume reuse,
construction of three command variants, packed request dispatch, and
sequence-state control.

An additional scalar/pointer pass converts 17 pure-C functions to named
`SDValue` fields covering channel volume, CD volume, driver flags, the
four-voice tables, late control fields, and the music-track pointer.

Some accesses still use an explicit byte-pointer expression
(`git grep -nE '\(u8 \*\) *g_SDValue' -- src`). The list changes as
members are named, so this note does not keep a count. `SD_MeasureCdAudioLevel` no
longer has any: its former base-local byte views are now the
`decoded_half`, `output_level` and `field_1550` members.

None of the three exceptions this note used to document is still a
byte-pointer access. The first, `SD_MeasureCdAudioLevel`'s decoded-data argument,
is now the `decoded_data` member quoted in the SPU section above. The other
two:

- `SD_ResetMusicTrackBuffer` writes the music-track pointer through
  `((u8 *)g_SDValue + 0x1564)` because a plain member assignment changes
  register allocation. Since #4457 it stores through the member's address,
  `*(void **)&g_SDValue->music_track` in `src/game/sound_init.c`, and the
  comment above that store carries the measurement.
- `SD_CountActiveSoundEffectVoices` was documented as indexing the four voice IDs as
  `((u8 *)g_SDValue + index * 2 + 0x404)`. The source spelled that
  `((u8 *)g_SDValue + s0 * 2 + 0x404)`, and `ac4e0662` ("Coalesce the
  sound-effect voice slots (#2740)") replaced it with
  `g_SDValue->voice_ids[s0]` -- `SD_CountActiveSoundEffectVoices` in
  `src/game/sound_voice_selection.c`, and
  the function matches. Only the description is stale; the three-instruction
  cost it claimed was not re-measured against the current source.

`SD_ResetMusicTrackBuffer`'s expression is an exact-code-generation view of a field
whose offset and type are defined by `SDValue`. The rest are byte-pointer
arithmetic over the same layout, with no reason for the spelling recorded
here.

All 26 resident `.c` files that name `g_SDValue` include `sound.h`, and so
do all 22 build-integrated candidate `.c` files naming it. None of them
declares the pointer itself any more. **No candidate defines a private struct for the *pointee* any
more either.** `sd_cancel_music_requests.c` and `sd_step_active_command.c` were the last two and
both went on 2026-09-11; what removed each is recorded in its own paragraph
below, and the two answers are different.

`sd_step_active_command.c` was the larger of the two and the note above calls it the
only exception left. Its private `SD` named 36 offsets. An exact pairing
against `SDValue`, taken by compiling the header and reading `offsetof`
rather than by matching names, put them in three buckets: 19 offsets the
canonical already named, 6 covered by `active_command_tail[8]`, and 11 that fell
inside five padding regions -- `pad04CC`, `pad0524`, `pad0534`, `pad157C`
and `pad1619`.

The 6 looked like the hard bucket and were not. Rewriting them as array
indices moves the object, and the bisection says why: the array form alone
is byte-identical, and the *signedness* alone reproduces the whole move --
`u32` where the unit reads signed. The cast at the use closes it, the same
answer `display_object_runtime.c` already gives for a signed read of a
canonical `u16`.

Sharing an offset is not sharing a type, and the 19 are one group only in
the first sense: 14 of them also agree with the canonical in C type and
five do not. Those five are four different questions rather than four more
of the 6.

  * **0x50**, private `s32` against canonical `u32`, *is* the 6's question
    and takes the 6's answer -- `(s32)` at each read.
  * **0x54**, private `u16` against canonical `u32`, is a narrowing rather
    than a signedness change. The unit reads it once, and the `(u16)` sits
    at that read.
  * **0x58 and 0x1564** are pointer views. The canonical spells 0x58 `u32`
    and 0x1564 `u16 *music_track`; this unit reaches the first as `u8 *`
    and the second as both `u8 *` and `List *`. The cast is at the use, so
    the canonical declaration and this unit's readings are both kept.
  * **0x1588**, private `u16` against canonical `s16`, takes no treatment
    at all, and that is why it is not in the list above. The unit only ever
    *stores* this halfword. The sixteen bits written are the same under
    either declaration, and there is no read here whose signedness could
    differ.

The 11 are a header addition, and the evidence behind each width is of
three different kinds -- which is worth saying plainly, because "it has a
use in this unit" flattens them into one.

  * **Five are read here**: `field_04CC`, `cd_start_sector_offset`, `cd_end_sector_offset`,
    `cd_filter_channel` and `cd_filter_file`. The read's own width is the evidence.
  * **Three are only written**: `field_0532 = b >> 31`,
    `cd_request_code = 0xFFFF` and `field_157C = active_command_value`. What the width
    rests on is the store and the private declaration this unit already
    carried, not a measured read.
  * **Three are passed by address**: 0x1619, 0x1629 and 0x1639. They are
    worth naming separately rather than as one 0x30 region, because the
    unit hands each of the three to `File_RequestSecondarySlot` on its own, selected by
    `active_command_tail[0] & 0xF0`. Three distinct call arguments 0x10 apart fix
    where each buffer *starts*; the 0x10 extent is the private
    declaration's, carried across, and is not proved here.

As historical context, the sentence this replaces -- "All pure-C
`g_SDValue` users now include `sound.h`" -- was written on 2026-09-02, and
both of the candidates that broke it were built in afterwards,
`SD_CancelMusicRequests` on the 9th (#2992) and `SD_StepActiveCommand` on the
10th (#3359). Nine additional functions use the shared command queue,
buffer pointers, voice arrays, flags, and late control fields directly.

`SD_CancelMusicRequests` took the header's declaration back but kept a private
struct for the pointee, and the reason was a measurement rather than a
preference. That is **resolved as of 2026-09-11**; the eliminations below
are kept because they are what made the answer findable, and the answer is
the last paragraph of this section. With `sound.h` included and its `G_SDVALUE_IN_DATA`,
`SD_POLL_SEQUENCE_STATE_RETURNS_S16` and `SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG` arms
selected, so that the unit declares none of those three symbols itself, the
candidate's object is byte-identical. Pointing the same reads at `SDValue`
instead is not: the object changes. What that is not caused by was measured
one at a time. Every offset the unit reads -- `status_flags` at 0x40,
`command_count` at 0x4C, `active_command` and `active_command_phase`, `field_157E`, and
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

`SD_RequestMusicPackageLoad` is a third deliberate exception and is no longer a raw
view. The global pointer is volatile in that routine, which the unit
selects by defining `G_SDVALUE_VOLATILE` -- an arm of `sound.h`'s own
declaration chain rather than a suppression of it -- and `sound.h` carries
the measurement. Its accesses are typed members (`p->music_track`,
`q->status_flags`, `q->field_1560`); the `u8 * volatile` redeclaration and
the offset expressions this note described were removed by `98f79757`
("Take every g_SDValue declaration from sound.h (#2610)") and `548c78cb`
("Reach the sound driver's state block through SDValue, not byte offsets
(#2500) (#2759)").

Existing GCC constraints remain unchanged by layout migrations. The secondary
spatialization contract below also covers callers containing constraints,
without changing or adding those devices.

The contiguous initialization block at `0x80049200-0x800495EC` now builds as
`src/game/sound_init.c`. It preserves the explicit raw music-pointer write in
`SD_ResetMusicTrackBuffer` while sharing `SDValue` declarations across the
other music/sequence helpers.

## Spatial volume and pan

Matching `SD_CalcSpatialVolumePan` converts one sound source's X/Z position
into a volume byte and signed pan byte using the 32-byte reference-view record
at `D_800F56F0` documented in `notes/model-structures.md`. Let `(sx, sz)` be
the source, `(vx, vz)` the viewpoint, and `(rx, rz)` the reference point.

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
sound driver. `SD_InitSecondaryWorkArea` installs the pointer and clears exactly
530 32-bit words, establishing a total allocation size of `0x848` bytes.

`src/game/sound.h` defines the partial `SDSecondaryState`,
`SDSecondaryTransfer`, `SDSecondaryRecord`, and `SDSecondaryObject` views.
Offset-based names are retained except where several matched functions
establish a stable role. Unmodeled and overlapping regions remain padding or
explicit typed/raw views rather than speculative fields.

The leading channel records and the later track records are distinct layouts.
`SD_DispatchSequenceChannelEvent` selects a channel with the MIDI status byte's low nibble and
indexes the leading records with a `0x18`-byte stride; `SD_ResetSecondaryPlayback`
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
| `+0x00` | `program` | Program-change dispatch calls `SD_SetSequenceChannelProgram`, which stores its program byte here. |
| `+0x01` | `pan` | Controller `0x0A` writes it; `SD_SpatializeSecondaryObject` includes it in the pan sum. |
| `+0x03` | `volume` | Controller `7` writes it; `SD_SpatializeSecondaryObject` multiplies it into the level. |
| `+0x05` | `expression` | Controller `0x0B` writes it; `SD_SpatializeSecondaryObject` applies it as another level factor. |
| `+0x07` | `pitch_bend_msb` | Pitch-bend dispatch passes both data bytes to `SD_SetSequenceChannelPitchBendMsb`, which ignores the LSB and stores the masked second byte; `SD_UpdateSecondaryObjectPitch` caches it and obtains the pitch adjustment through `SD_CalcPitchBend`. |

`SD_ResetSecondaryPlayback` writes every channel record through these members. Its other
stores name four more channel members: the byte `field_0004`, which
`func_8004B374` also clears, the two words `field_0008` and `field_000C`,
both reset to `0x7F`, and the halfword `field_0014`. In the other layouts it
names object halfword `field_001C` at `+0x1C`, which is set to `0x40` beside
`cached_pitch_bend`, and track byte `field_002B`. Nothing else in C reads
them, so they keep offset-based names. Replacing the `u8` padding with `s32`
gives `SDSecondaryRecord` 4-byte alignment. Its size and offsets are
unchanged, and its only container is the 4-aligned `SDSecondaryState`.

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
including typed `SDSecondaryState` root reads. The three matching callers
(`SD_UpdateSecondaryObjectVolumes`, `SD_HandleSequenceControlChange` and
`func_8004ADE8`) consume the same prototype. Their original byte-stride
argument calculations remain where needed; the parameter ABI is still two
32-bit pointers.

The layout evidence predates this conversion: the controller writer stores
channel pan, volume and expression, while the note starter `func_8004ADE8` fills
object `+0x08/+0x09` from program/tone gain bytes and `+0x0A/+0x0B` from
program/tone pan bytes, then stores velocity at `+0x0E`. It now
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
the existing twenty-entry `gSD_adwVoiceMasks` contract instead of its private
incomplete declaration. Only those two obsolete private-extern dependencies
are removed from its metadata; all nineteen candidate object fingerprints,
targets and profiles remain unchanged; the note starter was still a near
miss at that point (it has since matched and now lives in
`src/game/sound_sequence_channel_events.c`).
The matched note starter no longer needs `SDSECONDARYSTATE_AS_BYTES`
either: its channel-record, object, transfer-pointer, `voice_attr` and
`master_level_left/right` accesses are typed members, with identical bytes in every
region, and the byte-pointer arm has been removed from `sound.h`.

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
`SD_ApplySequenceReverbParameter` retains its raw byte view: selector
`SD_SEQUENCE_PARAMETER_REVERB_MODE` (`0x0F`) applies the staged value as the
reverb mode, and `SD_SEQUENCE_PARAMETER_REVERB_DEPTH` (`0x10`) applies its
existing left/right depth conversion. Unhandled selectors remain unchanged;
the work-area queries retain the check-versus-diagnostic contract described
above.

### CD descriptor tables, sector offsets and XA filters

The bank loader, CD request wrappers, active-command dispatcher and file
runtime establish this component without changing their shared request ABI:

| Location | Field | Established role |
|---|---|---|
| `SDValue +0x0518` | `cd_descriptor_banks[3]` | Three table bases selected by the play/seek wrappers. |
| `SDValue +0x0528` | `cd_start_sector_offset` | Selected entry's low-24-bit start plus its bank's base sector offset. |
| `SDValue +0x052C` | `cd_end_sector_offset` | Entry's low-24-bit end plus the same base, before the request's span adjustment. |
| `SDValue +0x0530` | `cd_filter_channel` | Entry word B bits 24-28, routed to the XA channel filter byte. |
| `SDValue +0x0531` | `cd_filter_file` | Entry word A bits 24-28, routed to the XA file filter byte. |
| `SDValue +0x0534` | `cd_request_code` | Gated play/seek input's low halfword, written before queue submission; stop writes `0xFFFF`. |

`SD_UpdateBankLoad` installs the three bases `0x801E8800`, `0x801E9000` and
`0x801EA000` after inspecting the CD bank's trailer, even if its signature
does not match. The wrappers keep their `0x8000`/`0x9000`/other selection
of banks 1/0/2, original code narrowing and arithmetic, first-word base
read, and eight-byte header skip before the paired descriptor words.
The pointer declarations, casts and unusual conditional base read remain.

Active command `0x24` stages both offsets and filter bytes. The offsets
are relative to `MASTER.XA`, not absolute LBAs: `File_RequestSecondarySlot`
adds `gFile_nMasterXaLba` to the staged start. It receives the original
`(u16)end - (u16)start + 0x10` assignment to signed `FileRequestSlot +0x1C`.
The staged end therefore is not the submitted exclusive range end.
Both halfword truncations, final narrowing and the extra sixteen sectors
remain. Zero span still selects the seek path; negative span still takes
the existing range-control branch. No order, extent or validity check is added.

The request's byte `+0x1F` supplies channel and `+0x1E` supplies file.
`File_RequestSecondaryRangeTransfer` carries them in bytes 0 and 1 of
`callback_data`; `File_StepActiveTransfer` places file before channel in the
two-byte `DslSetfilter` payload. This complete byte route, rather than the
order of the struct members or guessed table types, establishes the names.

Active command `0x21` computes its seek offset from the new entry's word A,
but copies the retained filter bytes without refreshing this staging state.
Its zero span selects seek, not a new filter submission. Both commands
retain their existing busy-slot rejection and command-clear order.

The two wrappers store `cd_request_code` through their original signed
halfword views before attempting to enqueue; no full-width seek token is
retained. Play still reports an accepted
wrapper path even when the queue rejects submission, and seek still has no
return value. Command `0x11` clears this marker only at its existing stop
phase. No tracked C reader is established: this is not a deduplication
cache, successful-load token or guarantee of the most recently playing code.

The write-only byte `field_0532` still receives word B shifted by 31.
No loop/repeat meaning is inferred for it. The separate `mix_multiplier`
extraction, generic command payloads, raw request fields, pointer reloads,
flag updates and SDK ordering remain unchanged. No enum, new initializer,
struct view, replay or behavioral harness is introduced.

### Active-command payload snapshot

`SD_UpdateRuntime` stages the queue head into a separate payload before
calling `SD_StepActiveCommand`. The copy establishes the following names;
the argument numbers describe queue-word positions, not one shared
operation-specific meaning:

| Queue source | `SDValue` destination | Name |
|---|---|---|
| `+0x02`, signed halfword | `+0x004E`, unsigned halfword | `active_command_value` |
| `+0x04`, signed word | `+0x0050`, unsigned word | `active_command_arg0` |
| `+0x08`, signed word | `+0x0054`, unsigned word | `active_command_arg1` |
| `+0x0C`, signed word | `+0x0058`, unsigned word | `active_command_arg2` |
| `+0x10..+0x2F`, eight words | `+0x005C..+0x007B`, eight unsigned words | `active_command_tail` |

The halfword still uses the explicit `(u16)` conversion. The three word
assignments retain the canonical unsigned storage and each consumer's
existing signed, narrowing or pointer view. In particular, argument 2
remains a raw `u32`, not a newly typed or stored C pointer. The tail stays
the same `u32[8]`, copied through `SDCommandTail` in one block assignment;
the queue and active-state layout checks pin both ends and the copy size.

Command `0x51` reads argument 2 as an `SDSeqBlock *` and argument 0 as
the SPU base address. The callee adds `active_command_arg0 >> 4` to
each copied note's `spu_address_div16`; its loop count comes from
`SDSeqBlock::count` in the block addressed by argument 2.
Commands `0x21` and `0x24` instead use
argument 2 as the descriptor-pair table base, the value halfword as its
index, and argument 0 as the common sector-base offset. Command `0x48`
uses the value's low nibble to select the sequence index, copies the
complete value to `opened_sequence_command` only after a zero open result,
and narrows argument 1 to the sequence fade step. None of these views
renames a generic argument into a universal count, offset or pointer.

Command `0x20` reads tail words 0, 1, 3, 4, 5 and 6 for the existing
source selector, sector offset, SPU destination, RAM destination, SPU
phase size and RAM phase size. The selector rewrite and filename-buffer
handoffs retain the separately documented `0x10`/`0x20`/`0x40` branches.
Queue-head fade preparation for `0x29`/`0x2A` and `0x44`/`0x45` still
reads the queue record, not a substituted active-payload view.

The producer keeps opcode/phase setup, separate global reloads, payload
copies, wait-byte copy, tail copy, the unexplained `field_15F4 = 8`,
count decrement, first active step and later queue compaction in their
original order. The value 8 does not establish a role for `field_15F4`.
`SD_ResetRuntime` clears only the four prefix members; it does not clear
the tail. Active-command completion still clears only its original
opcode/phase bytes. Commands populate only the fields they need, so a
snapshot does not promise that every copied word is initialized or
meaningful for every opcode. No zeroing, union, enum, validation, alias
rewrite or scheduling change accompanies the names.

### Active command staging and CD-fade wait

Four names connect the queue record to its active work state without
changing storage: `SDCommand.wait_for_cd_fade` at `+0x01` and
`SDValue.active_command`, `active_command_phase`, `active_cd_fade_wait`
at `+0x7C/+0x7D/+0x7E` all remain `u8`. The record stays `0x30` bytes,
the queue stays at `+0x80`, and its byte/record/trailing-copy views remain.
One new record-byte assertion and the expanded active-state assertion
pin these four offsets. No generic payload fields are renamed.

After the existing fade update, `SD_UpdateRuntime` services a nonzero
active command and returns without dequeuing another command that update,
even when the active step clears it. Otherwise, if the queue is nonempty,
it retains the command-specific fade preparation, copies the first
record's command into active state, clears the phase and stages the
payload fields and wait byte in their existing order.

The active step runs before the existing queue compaction. Preserve
the separate state-pointer reloads, command-count snapshot/update,
eight-word tail copy and both byte-store and whole-record queue copies.
Zero remains the inactive/clear marker; do not change queued-zero
handling, queue limits or command dispatch to a new enum or drain loop.

The phase byte is command-local state, not a timer or transfer counter.
Command `0x20` uses phase zero to attempt the secondary-slot request
and phase one to wait for the separate `busy` byte to clear. Its
zero-length path, request selection, callback arming, phase increment,
early returns and post-completion flag masks remain exact.
Command `0x11` uses phases zero/one around the existing CD fade-to-zero
and queued-stream checks. Other phase values retain their early returns;
no phase normalization, new initialization or generic completion rule.

The runtime copies `wait_for_cd_fade` into `active_cd_fade_wait` without
normalizing either byte. Only active commands `0x29/0x2A` use the wait
byte among matched C readers: nonzero plus a nonzero current
`cd_fade_step` retains the active command; otherwise its existing
command/phase clears run. This waits on the driver's current CD fade,
not an allocation, callback token or proof that this command started
a new fade or reached its requested target. Keep the dequeue-time
condition that only installs a fade while its existing step is zero.

`SD_EnqueueCdFade` narrows its third argument into the queued byte.
Its existing no-retail-caller caveat remains; this naming does not
claim a new reachable wrapper or human visual route. No record memset,
new field initialization or changed unused argument is introduced.
Runtime reset clears command, phase and wait at their existing points.

CD-command dropping and music-request cancellation clear only their
selected active command/phase pair, not the retained wait byte. The
North American and separate Japanese drop units keep their own exact
loop forms, command sets, decrement and trailing-record-copy behavior.
All command numbers, generic payload views, partial clears, fade
targets/steps, flags and unrelated driver state remain unchanged.

### VAB transfer state, sizes and address bases

Six names recover the existing `SDSecondaryTransfer` core without changing
its `0x1C` layout or the secondary-state transfer offset `0x4A4`:

| Transfer offset | Existing type | Name | Established use |
|---|---|---|---|
| `0x00` | `s16` | `state_token` | Zero on accepted open, `-1` on reset; compared with caller tokens by both body writers. |
| `0x04` | `u8 *G32` | `vab_header` | Retained header byte pointer used by note-on to locate programs, tones and the VAG size table. |
| `0x08` | `s32` | `header_size_bytes` | `(program_count << 9) + 0xA20`, used to derive the body size from the VAB file size. |
| `0x0C` | `s32` | `full_body_source_address` | Source integer address retained only after a successful whole-body write; no matched C reader is established. |
| `0x10` | `s32` | `body_size_bytes` | Header-derived body byte count used by whole-body and chunked writes. |
| `0x14` | `u8 *G32` | `spu_body_base` | SPU byte-address base used for transfers and note-on sample addresses, not a dereferenced RAM buffer. |

`SD_VabOpenHead` clears `bytes_consumed` before its possible rejection.
Only a requested token of `-1` checks the existing token and can reject
an occupied slot; other requested values bypass that check. Accepted
open always stores zero, not the requested ID. Both established callers
pass zero. This does not establish a multi-bank index or an incrementing
transfer identifier. Reset changes the token to `-1` without clearing
the retained header, sizes, source snapshot or SPU base.

The header-size expression includes the fixed header/program/offset-table
areas and the program-count-dependent tone area. Body size keeps the
existing unsigned-file-size subtraction into signed storage; no new
underflow check, allocation extent or validated buffer length is inferred.
The header remains a byte pointer with the note-on cast to `SDVabHeader`;
do not replace its stored type or the existing pointer views.

The whole-body writer compares its signed-halfword token, sets the SPU
base, then writes the complete retained body size. A short write returns
`-1` before updating `full_body_source_address`; a successful write stores
the integer source and returns the current token. It does not advance
`bytes_consumed`. Neither open nor reset newly clears this source snapshot,
and the chunk writer does not maintain it. No reader or general-purpose
source cursor is inferred from the name.

The chunk writer compares `(s16)state`, sets the destination to SPU base
plus the current byte count, computes remaining bytes and retains its
unsigned clamp comparison. Only a successful exact-length write advances
the unsigned counter. Reaching the unsigned-view body size returns the
signed-halfword token; otherwise it returns `-2`. A mismatch or short
write returns `-1`. Preserve signedness, narrowing, reloads, pointer
arithmetic and the lack of new negative-count or bounds guards.

Note-on also consumes the SPU base: it sums the existing VAG size-table
entries, keeps `sum + sum * 7` to convert their units to bytes, then adds
that byte displacement to the integer-view base for `voice_attr.addr`.
This is distinct from the transfer counter's byte displacement. Keep
the table location, signed VAG test, summation order and address casts.
The completion helper still only queries the SDK; it does not change
this token or counter. Existing ambient-argument reset caller views,
SDK contracts, control bytes and all other owners' fields remain exact.

### Confirmed secondary-state fields

| Offset | Width | Field | Local matching-C evidence |
|---|---:|---|---|
| `0x0000` | `0x18` stride | `SDSecondaryRecord` channel view | `SD_HandleSequenceControlChange`, `SD_SetSequenceChannelProgram`, and `SD_SetSequenceChannelPitchBendMsb` establish `program`, `pan`, `volume`, `expression`, `pitch_bend_msb`, and staged controller selector/mode/value bytes; `+0x06` and `+0x10` retain offset-based names. |
| `0x0180` | `0x28` stride | `objects[20]` | `func_8004A7C0`, `SD_HandleSequenceControlChange`, and `func_8004C84C` establish the object base/stride; additional matched inline-assembly functions use the same view. Verified members are `channel_index` at `+0x03`, a byte at `+0x0F`, and a `u16` at `+0x1E`. |
| `0x04A4` | `0x1C` | `transfer` | `SD_ResetSecondaryState`, `SD_VabOpenHead`, `SD_VabTransBody`, `SD_VabTransBodyChunk`, and `SD_ResetVabTransferState`. Members are `s16 +0x00`, pointer `+0x04`, `s32 +0x08/+0x0C/+0x10`, pointer `+0x14`, `u8 vab_master_volume +0x18`, unnamed bytes `+0x19/+0x1A`, and `u8 vab_pan +0x1B`. |
| `0x0500` | `u8` | `timer_update_inhibit` | Nonzero makes the timer callback return before processing; the shared updater does not check it. |
| `0x0501` | `u8` | `update_in_progress` | Plain byte tested/set/cleared around both timer and shared secondary updates, including their maintenance calls. |
| `0x0502` | `u8` | `shared_track_updates_enabled` | Gates only the shared updater's ten track calls; neither its maintenance nor the timer's eight-call path uses this gate. |
| `0x0503` | `u8` | `event_guard` | `SD_OpenSequenceTimerEvent` prevents duplicate setup with it; shutdown leaves it set to block further event setup. |
| `0x0504` | `long` | `event_handle` | `SD_OpenSequenceTimerEvent` stores the `OpenEvent` result; `SD_CloseSequenceTimerEvent` disables and closes the same handle. |
| `0x0508` | `u8` | `timer_maintenance_counter` | Counts inner timer sequencer calls; reaching 11 triggers object maintenance and the optional callback. |
| `0x0509` | `u8` | `vsync_update_mode` | Nonzero selects the `SD_VSync` secondary-update path and suppresses timer-callback work. |
| `0x050C` | callback pointer | `timer_maintenance_callback` | `SD_InitState` installs `SD_UpdateVoiceSlots`; called only at the timer maintenance threshold. |
| `0x0510` | `s16` | `object_count` | Initialized by `SD_ResetSecondaryState` and validated/set by `SD_SetSecondaryObjectCount`; bounds the `0x28`-byte object scans in several matched functions. |
| `0x0512` | `s16` | `sequence_level` | Sequence-wide gain: controller `0x1E` stores its byte value here; spatialization multiplies it with the retained VAB master volume. |
| `0x0514`, `0x0516` | `u16` | `master_level_left`, `master_level_right` | Stored stereo master levels applied by secondary note-on and `SD_SetVoiceVolume`; setter parameters remain signed halfwords. |
| `0x07DC` | pointer | `sequence_read_base` | Copied from `sequence_source` on play/fast-forward; base for the byte reader, `MTrk` search and track read-address calculation. |
| `0x07E0` | `s16` | `sequence_open_token` | Retained `SD_OpenSequence` second argument; `-1` gates the closed state, with no bank-index use established. |
| `0x07E2` | `s16` | `sequence_state` | Open/play/stop/close and mute/restore state; all-track-ended status is latched only by polling. |
| `0x07E4`, `0x07E6` | two `s16` | `spatial_level_left`, `spatial_level_right` | Independent post-pan spatial gains, masked to seven bits by `SD_SetSecondarySpatialLevels`; separate from the output masters at `0x0514/0x0516`. |
| `0x07E8` | pointer | `sequence_source` | Input pointer retained by an accepted `SD_OpenSequence`; copied to the read base only on play/fast-forward. |
| `0x07EC` | `s32` | `sequence_read_limit` | Fixed `0x10000` offset guard for the byte reader and `MTrk` search, not a recovered source-buffer length. |
| `0x07F0` | `s32` | `sequence_tick_count` | Logical tick counter incremented on the selected track's tempo carry; nonzero-selector loop handling can restore it. |
| `0x07F4` | `s32` | `sequence_tick_count_saved` | Snapshot of that counter taken and restored alongside the nonzero-selector all-track loop state. |
| `0x07F8` | `u16` | `field_07F8` | Equality selector for the counted track and zero/nonzero loop-scope gate; not renamed here. |
| `0x07FA` | `u16` | `track_count` | `SD_HandleSequenceMetaEvent` and `SD_StartSequenceTracks` bound `0x2C`-byte work-record loops. |
| `0x07FC` | `u16` | `timebase` | `SD_HandleSequenceMetaEvent` and `SD_ScaleSequenceDelta` select timing conversions from it. |
| `0x0800` | `u8` | `field_0800` | Cleared by `SD_StartSequenceTracks`. |
| `0x0804` | `s32` | `delta_scale_gate` | Retained timebase word tested for nonzero before delta scaling; the header's final unconditional copy overrides its preceding conditional stores. |
| `0x0808` | `s32` | `tempo_us_per_quarter` | Retains the header's upper 24-bit tempo or meta-event `0x51` tempo before conversion; no matched C reader is established. |
| `0x080C` | `s32` | `fast_forward_target_address` | Zero disables countdown skipping; a ticking track clears the global target when its unsigned cursor address reaches it. |
| `0x0810` | `s32` | `last_track_cursor_address` | Latest ticking track's `sequence_read_base + pos` snapshot, even on a countdown-only step; no matched C reader is established. |
| `0x0814` | `u8` | `sequence_updates_enabled` | `SD_ResetSecondaryState` clears it; enabling sets it after optional event setup, and both timer and explicit secondary-update paths require it. |
| `0x0815` | `u8` | `pan_center_override` | Panning enable clears it and disable sets it; nonzero forces centre pan before the independent left/right spatial gains, not equal final outputs. |
| `0x0818` | `u32` | `bytes_consumed` | `SD_VabOpenHead` clears it and `SD_VabTransBodyChunk` advances it across a transfer window. |
| `0x081C` | `s32` | `sequence_tick_mode` | Stored tick selector: initialization uses `2` (`SS_TICK240`), reset uses `0x1000` (`SS_NOTICK`), and enable/reset-runtime paths consider timer open/close only for values `1` through `3`. |
| `0x0844`, `0x0845` | `u8` | offset-based fields | `SD_ApplySequenceReverbParameter` stores two control-event byte values. |

`SD_OpenSequenceTimerEvent` registers `SD_SequenceTimerCallback` for an
interrupt event on `RCntCNT2` with specification `EvSpINT` and mode `EvMdINTR`,
stores the event handle at `+0x504`, and enables it.
`SD_StopSequenceTimer` stops the counter while leaving that event registered;
`SD_CloseSequenceTimerEvent` stops the counter and also disables and closes the
event handle. All three lifecycle paths use the imported Psy-Q `libapi.h`
declarations and `kernel.h` constants.

The callback now follows the three contiguous secondary command handlers in
`src/game/sound_secondary_commands.c`, restoring the complete `0x8004B49C`-
`0x8004B854` translation unit after its pure-C promotion. The command handlers
update the channel/object state that the interrupt callback advances and
periodically maintains; the different-profile event setup at
`SD_OpenSequenceTimerEvent` fixes the upper boundary.

The header uses GCC-2.8.1-compatible negative-array assertions for the
`0x18`, `0x28`, and `0x1C` subview sizes, the complete `0x848` state size, and
the major top-level offsets.

The adjacent envelope setters `SD_SetVoiceEnvelopeFromTone` (`0x8004A6F8`) and
`SD_ResetVoiceEnvelope` (`0x8004A764`) are pure C in
[`sound_voice_setup.c`](../src/game/sound_voice_setup.c), following the pitch and reset helpers, since post-terminal refinements removed
their hard-register and inline-assembly dependency. They use the same
`SpuVoiceAttr` block at `+0x4C0`: the first copies ADSR values from the
caller's tone record, while the second sets the initialization defaults before
the caller turns the voice off. Both now match as pure C at
`gcc_2_8_1_g8_split`, with the `const` table declaration still preserving the
original address materialization.

### Secondary spatial gain and pan inputs

Six names follow the existing spatialization chain without changing storage:
transfer-relative `vab_master_volume` at `+0x18` and `vab_pan` at `+0x1B`
remain `u8`; secondary-state `sequence_level` at `+0x512` and
`spatial_level_left` / `spatial_level_right` at `+0x7E4/+0x7E6` remain
`s16`; `pan_center_override` at `+0x815` remains `u8`. The existing
transfer, spatial-level and override assertions now use those member names.
Unrelated command/track `field_0018` owners are not renamed by this change.

`SD_VabOpenHead` copies the local `SDVabHeader.master_volume` and `.pan`
bytes into the transfer record. These are retained VAB inputs, not a
transfer byte count or one of the later output masters. Secondary-state
reset sets `vab_pan` to 64 but does not initialize `vab_master_volume`;
the adjacent unnamed transfer bytes still receive their own 127 stores.
No default master-volume store or new bank-open validation is added.

`SD_HandleSequenceControlChange` controller `0x1E` stores its `u8` value
into the signed halfword `sequence_level`, then refreshes object volumes
without a seven-bit mask or the spatial setter's state check. This is
sequence-wide state, not a field in the selected channel. Both secondary
reset and playback reset restore 127. Spatialization preserves the
explicit `(u16)` view when multiplying it by the VAB master volume.

With no override, pan is VAB pan plus program pan, tone pan and channel
pan minus the existing three-centre bias, clamped to `0..127`. A nonzero
override selects centre 64 instead. Enable-panning stores zero and
disable-panning stores one; neither setter newly refreshes live volumes.
This is a centre-pan override, not a promise to make final stereo
levels equal: the later left/right spatial gains can still differ.

The gain chain retains its exact signed arithmetic and staging: VAB
master volume times the unsigned-view sequence level, then channel
expression and volume, shift right 14, program volume, tone volume,
shift right 14, then the existing piecewise pan calculation. After pan,
left and right multiply their corresponding signed spatial halfwords,
shift right 7, separately reload the volatile velocity byte masked to
seven bits, multiply and shift right 7 into the object's `u16` levels.
Do not combine the two shifts, reorder products, widen intermediates,
merge velocity reads or normalize the special centre-pan branch.

`SD_SetSecondarySpatialLevels` masks each word argument with `0x7F`
before storing the signed halfwords. It snapshots `sequence_state`
before both stores and skips the refresh only for state 2. Stop and
close restore the pair to 127 in their existing right-before-left order,
as does secondary-state reset; playback reset does not newly reset them.
These gains remain distinct from `master_level_left/right`, applied
later by the output-volume path. No new clamp, enum, public API,
hardware-rate claim, callback, test or witness migration is introduced.

### The sequencer state halfword (`sequence_state`)

Every value the state halfword at `0x07E2` takes has a matching-C writer, and
all of them are in `src/game/`:

| Value | Meaning | Written by |
|---:|---|---|
| `0` | no sequence open | `SD_ResetSecondaryState`, beside `sequence_open_token = -1`, and the close step `SD_CloseSequence`, which also restores `sequence_open_token` to `-1` |
| `1` | playing | `SD_PlaySequence` when the low byte of its argument is nonzero, `SD_PlaySequenceFastForward` always, and `SD_RestoreSecondaryVoiceVolumes` |
| `2` | staged or stopped | `SD_OpenSequence` once it has staged an input, and `SD_StopSequence` |
| `3` | every track ended | `SD_PollSequenceState`, and nothing else |
| `4` | paused | `SD_PlaySequence` when the low byte of its argument is zero, and `SD_MuteSecondaryVoices` |

The readers make `3` the interesting one. `SD_ProcessSequenceTracks` returns
`0` immediately unless the halfword is `1`, and it never writes the halfword
itself, so the sequencer stops advancing at the end of a sequence but leaves
the state reading `1`. `SD_GetSequenceStatus` returns `3` exactly when no
track has its `ended` flag clear -- including when `track_count` is zero,
where its loop does not run -- and `SD_PollSequenceState` is what asks it and
latches the answer. So a finished sequence is only observable after something
polls it.

Its three callers all pair that with `SDValue`'s flag `0x80` and
`sequence_open_result != -1`: `SD_UpdateRuntime` and `SD_ResetRuntime` stop the sequence
when the poll is not `1`, and `SD_CancelMusicRequests` stops it when the poll
is `1`.
`SD_SetSecondarySpatialLevels` and `SD_SetSecondaryMasterLevels` read the same
halfword for a different question, skipping `SD_UpdateSecondaryObjectVolumes`
while it is `2`.

### Sequence delta rescaling and retained tempo

Four names preserve existing storage: `SDSequenceTrack.delta_scale_remainder`
at `+0x18` and `delta_scale_remainder_saved` at `+0x1A` remain `u16`;
`SDSecondaryState.delta_scale_gate` at `+0x804` and
`tempo_us_per_quarter` at `+0x808` remain `s32`. Two assertion blocks
pin these four offsets without changing either record's size or layout.
Unrelated command/transfer members named `field_0018` are not renamed.

`SD_ScaleSequenceDelta` selects on the separate `timebase` halfword.
Its arithmetic uses the existing `u32 delta_remaining`, staged operations
and `u16` remainder stores:

| Timebase | Existing rescaling |
|---|---|
| `0x30` | Multiply by 10, add remainder, save low 2 bits, shift right 2. |
| `0x60` | Multiply by 5, add remainder, save low 2 bits, shift right 2. |
| `0xC0`, `0xF0` | Add remainder, save low bit, shift right 1. |
| `0x120`, `0x168` | Divide by 3; neither read nor update the remainder. |
| `0x180`, `0x1E0` | Add remainder, save low 2 bits, shift right 2. |
| `0x300`, `0x3C0` | Add remainder, save low 3 bits, shift right 3. |
| Other values | Leave delta and remainder unchanged. |

Do not rationally fold the multiply/add/mask/shift stages, widen their
intermediates or invent fractional carry for the divide-by-three cases.
`SD_StartSequenceTracks` clears the gate before header parsing, reads each
track's first delta, clears its live remainder, then conditionally scales.
`SD_ResetSecondaryPlayback` also clears only the live remainder. Neither
path newly initializes the saved remainder. The normal track pump scales
new nonzero deltas only when the retained gate is nonzero.

The header first conditionally copies `timebase` into the gate for values
at least `0x60`, otherwise storing zero, but then unconditionally copies
`timebase` again. That final store overrides the apparent high-resolution
filter. This is a retained signed word used as a nonzero predicate, not
a normalized boolean or a high-resolution-only flag. Every store, reload
and comparison remains; the scaler reads `timebase`, not the gate's value.

The header stores the upper 24 bits returned by `SD_ReadSequenceU32BE`
as `tempo_us_per_quarter`. Meta-event `0x51` reads and stores three tempo
bytes in the same word. Standard tempo units are microseconds per quarter
note, not BPM, a tempo step or an observed hardware frequency. Both paths
calculate from their existing locals, not a reload of this retained word;
no matched C reader is established. Keep `60000000 / tempo`, `*100/115`,
the `255` clamp and timebase-dependent shifts, including zero-tempo
behavior without a new guard. Meta tempo updates every track's
`tempo_step` and `tempo_accumulator` but does not reset its delta remainder.

The nonzero `field_07F8` controller-loop path saves and restores each
track's remainder beside its existing parser snapshots, with unchanged
counts and global reloads. The zero-selector path saves/restores only
the current track position; no remainder snapshot, reset or clock rewind
is added. The selector remains unnamed, and all accepted clock/cursor,
timer, source/state and mixer semantics remain unchanged.

### Sequencer clock, loop snapshot and fast-forward cursors

Four signed words connect the logical sequencer clock to loop restoration
and address-based fast-forwarding: `sequence_tick_count` at `+0x07F0`,
`sequence_tick_count_saved` at `+0x07F4`, `fast_forward_target_address` at
`+0x080C`, and `last_track_cursor_address` at `+0x0810`. No type or
signedness changes; the address-valued fields remain integer words, not
new stored-pointer declarations.

The clock increments only when an unended track's tempo accumulator
produces its existing carry and that track index equals `field_07F8`.
It is not an interrupt, frame, byte or event count. The pump retains its
track-array-relative `STATE_WORD(s32, 0x7F0)` increment and the adjacent
raw `0x7F8` selector access; only the clock's member-name comment changes.
Header parsing still clears the clock, its saved word and `field_07F8`
in the original order.

The selector deliberately remains unnamed. Its equality use selects
which track increments the clock, but its nonzero tests also choose the
all-track loop-save/restore/count path instead of the current-track path.
Only the zero initializer is established among matched C writers. This
does not justify declaring it a MIDI-format number or naming only one
of its two roles.

For the nonzero-selector path, the existing loop-start controller copies
the current clock into `sequence_tick_count_saved` after saving the
tracks' parser state; loop-end restores that clock with the track state.
The zero-selector path only saves/restores the current track's position
and does not newly snapshot or rewind this global clock. Therefore the
clock is not promised monotonic, and the saved word is not a universal
loop-start time. Loop counts, saved fields, reloads and ordering remain.

`SD_PlaySequenceFastForward` stores its existing signed target argument
after starting the tracks. A nonzero `fast_forward_target_address` bypasses
the ordinary nonzero-delta countdown; it does not bypass playback-state,
ended-track or tempo-carry gates. At the existing accumulation label,
the pump computes `(int)sequence_read_base + pos`, publishes the cursor,
then compares it and the target with the original unsigned casts.
The first ticking track reaching the address clears the one global target;
later tracks in the same pump call can consequently resume ordinary
countdown behavior. This is not a per-track or all-tracks seek-complete
test, an offset target, or a newly validated destination.

`last_track_cursor_address` is a snapshot of whichever track most recently
reached that accumulation point. The countdown-only branch reaches it
without reading a new event, so publication does not prove new bytes were
consumed or that the address increased. No matched C reader is established;
do not infer a debug consumer, completion flag or public progress API.
Secondary-state reset clears the two address words; header parsing clears
the clock words. Stop and close do not newly clear either address.

One assertion block pins all four offsets. Pointer-to-integer addition,
signed storage, unsigned comparison, zero-target behavior, raw displacement
views, every branch and literal remain exact. No new callback, bounds
check, clamp, enum, witness migration, test or harness is introduced.

### Separate timer, shared-update and track-call gates

The three adjacent bytes now name their distinct matched-C roles:
`timer_update_inhibit` at `+0x500`, `update_in_progress` at `+0x501`,
and `shared_track_updates_enabled` at `+0x502`. All remain `u8`;
the existing single-offset assertion becomes one three-offset assertion.
They are separate from `sequence_updates_enabled` at `+0x814`,
`vsync_update_mode` at `+0x509` and playback's `sequence_state`.

Only the timer callback reads `timer_update_inhibit` among matched C
consumers. Nonzero returns one after the global-enable test and before
the VSync-mode test. Playback, mute and restore paths bracket their
existing work with inhibit stores, but the shared updater does not
test it. This is not a general mutation lock, hardware interrupt mask
or guarantee that every update path is excluded.

`SD_OpenSequence` sets inhibition before checking the input tag. Its
unsupported-tag return leaves inhibition set; valid-tag accepted and
occupied-slot returns clear it at their original points. Do not add
a cleanup store. Timer open clears inhibition after its event-setup
calls, while its event-guard early return does not; timer stop
and close retain their final clear after the critical-section calls.

The VSync setter first narrows its word argument into the stored mode
byte, reloads that byte, then sets or clears inhibition from the stored
value. Keep this order rather than testing the original argument.
Later playback or timer-lifecycle calls may clear inhibition without
changing VSync mode; the callback independently checks both bytes.

Both timer and shared update paths test `update_in_progress`, then
set it for their existing track and object-maintenance work and clear
it afterward. The timer retains its ignored `GetRCnt` call before the
store and its return-zero path when the byte is already set. The
shared updater returns immediately when it is set. This remains a
plain non-atomic byte with the original tests and stores, not a new
mutex, nesting counter or stronger synchronization contract.

`shared_track_updates_enabled` guards exactly ten explicit track calls
inside the shared updater; object reuse and voice maintenance still
run once when that byte is zero, provided the update guard was clear.
The timer's separate eight calls and maintenance counter do not test
this byte. The track pump still applies its own state/ended/tempo gates.
Do not unify these paths or equate this byte with active playback.

Secondary-state reset clears all three bytes. Nonzero-start playback
and fast-forward set the shared track gate; stop clears it only in its
existing open-token branch. Zero-start playback, close, mute and restore
do not newly clear or set it. These retained values do not supersede
the pump's independent state checks. Every branch, return, literal,
count, callback, SDK operation and state-pointer reload remains exact.

### Timer/VSync dispatch and maintenance cadence

The five timer-dispatch names describe separate controls, not one playback
boolean or a newly inferred hardware frequency. The stored callback remains
`void (*G32 timer_maintenance_callback)(void)`; the SDK event handler still
returns `PSXLONG`, and no callback ABI or event registration changes.

`SD_SequenceTimerCallback` first checks `sequence_updates_enabled`, the
existing mutation guard, and `vsync_update_mode`, in that order. Those
early exits still return one. If the separate update guard is already held,
the handler still returns zero without processing. Otherwise it retains
the ignored `GetRCnt(RCntCNT2)` call and holds the update guard across its
eight inner iterations.

Each iteration calls `SD_ProcessSequenceTracks` and then increments the
byte `timer_maintenance_counter`, even if that call immediately returns
because playback is inactive or no tracks exist. Reaching 11 clears the
counter, updates secondary-object reuse countdowns and voices, then
reloads and conditionally calls `timer_maintenance_callback`. Cold init
installs `SD_UpdateVoiceSlots` there. Thus this is not an interrupt count,
a count of successful track advances, or an asserted 11-frame interval.
The counter carries between interrupts; secondary-state reset does not
newly reset it, the callback pointer, or the VSync selector. Their cold
zeroing and later initialization remain at the original points.

The VSync path is intentionally different. A nonzero `vsync_update_mode`
makes `SD_VSync` call `SD_UpdateSecondarySequenceIfEnabled` before the
existing runtime/bank work. That helper requires the enabled byte, then
the shared secondary updater uses ten explicit track calls only when its
existing run flag is set, followed by object maintenance once. It neither
advances this timer counter nor invokes this stored callback. These two
paths are not combined, equalized or assigned a common frame rate.

`SD_SetSequenceVSyncMode` still stores its word argument into the byte,
reloads that byte, then sets or clears `timer_update_inhibit` from the stored value.
It does not stop/restart a counter or open/close an event. Do not replace
this with a test of the untruncated argument or normalize it to a boolean.

`sequence_updates_enabled` is cleared by `SD_ResetSecondaryState`, not
`SD_Init`. Enabling first considers event setup for signed tick modes
greater than zero and less than four, then always stores one, even if the
event guard causes setup to return early. `SD_SetSequenceTickMode` only
stores the signed word. `SD_ResetSecondaryRuntime` uses the same range
before closing the event and resetting state. Startup selects mode two;
reset restores `0x1000`. The local SDK labels these `SS_TICK240` and
`SS_NOTICK`, but neither the names nor this change establish an observed
interrupt frequency or make the selector itself reconfigure the timer.

All numeric loop counts, threshold comparisons, pointer reloads, null
checks, flag-store ordering, event guards and SDK calls remain exact.
One new assertion block pins counter, selector, callback and enable-byte
offsets; the existing tick-mode assertion follows its renamed field.
No witness contains these old member names, and no new test or harness is
added. Cloud and witness suites remain for ordinary PR validation.

### Transfer-window state and results

The leading halfword of `transfer` uses `SD_TRANSFER_STATE_INACTIVE` (`-1`)
as its inactive marker. `SD_ResetSecondaryState` initializes it and
`SD_ResetVabTransferState` restores that marker. This is a software state
marker, not a claim that a hardware DMA transfer has completed.

`SD_VabOpenHead` checks for that marker when its caller requests an inactive
window, then prepares the window with state zero. `SD_VabTransBody` and
`SD_VabTransBodyChunk` retain their caller-supplied state comparisons. An
operation failure is `SD_TRANSFER_ERROR` (`-1`), while
`SD_VabTransBodyChunk` returns
`SD_TRANSFER_INCOMPLETE` (`-2`) when its consumed-byte count has not reached
the window length. A completed window returns the matched state token;
the state marker and result codes are separate roles despite sharing `-1`.

The transfer entrypoint is `SpuWrite`. The signature catalogue proposes both
`SpuRead` and `SpuWrite` because their small public wrappers are byte-identical,
but the resident body calls confirmed internal `_spu_Fw`; adjacent `_spu_Fr`
is the read helper. Both matching callers set the SPU transfer-start address,
then submit a source buffer and require the full byte count back. The direction
therefore comes from the local call graph rather than an imported name.

### Embedded music VAB header

The music package prefix now embeds the verified `SDVabHeader` record at
`+0x50`, instead of declaring a one-byte placeholder and casting it at both
VAB-open call sites. Its SPU destination remains the word at `+0x0C`.
`sound.h` imports the existing type from its owner,
`sound_transfer_lifecycle.h`; the layout is not duplicated or inferred from
an external project. The header's `0x20` bytes end at package offset `0x70`.
This is a measured package prefix, not the total size of its variable
program/tone/sample data, and no code uses it as an array stride.

Both `SD_OpenMusicVab` and command 72 of `SD_StepActiveCommand` now pass
`&package->vab_header` directly to the existing typed `SD_VabOpenHead`
contract. Header checks compile that member address without a cast and
reject a byte-pointer function contract; the music initialization source
also compiles with GCC 2.8.1 warnings treated as errors.

### Sequence-input header tags

`SD_OpenSequence` reads the input's first word and accepts these exact values
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

### Command-state pump and indexed sequence header

`SD_StepActiveCommand` owns the 82-entry dispatch table at `0x80010578`:
`split.yaml` assigns its `0x148` bytes at file offsets `0xD78..0xEC0` to
`src/game/sd_step_active_command.c`. The remaining prefix of `initial_data_1e3`
and the following `SD_CancelMusicRequests` table retain their existing owners.

The command pump keeps the target's cached-versus-reloaded state pointers.
In particular, it reads packed command words and transfer offsets before
request writes that could otherwise change a later read. The shared word
temporaries span commands 33 and 36; the state temporary also serves command
72. Their in-place mask, offset-add, and high-bit updates preserve the
separate transfer-call tails without register bindings.

Command 72's sequence header has a count at `+2`, a payload size at `+0xC`,
and a payload beginning at `+0x50`. Its sixteen index records occupy
`+0x10..+0x50`, with a **four-byte stride**; this reader consumes only each
record's low halfword. `sound_command_index.h` owns `CommandIndexTable`,
which bounds all 32 halfwords and asserts the header size and table offset. It does not use
the historical one-element `SoundIndexList.indices` view or silently treat
the records as a packed two-byte index array.

The promoted caller takes its declarations from sound-owned headers.
`SD_HasQueuedStreamCommand` retains its explicit byte mask, and
`SD_OpenSequence` retains
the signed-halfword store and test after the canonical word-sized result.
`SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG` selects the measured two-argument
`SD_PlaySequence` caller view while its definition keeps the one-argument
view. Native compiler controls check these declarations and reject
incompatible views.

The complete five-function secondary playback lifecycle now builds from
`src/game/sound_secondary_playback.c`, covering `0x80049A64` through
`0x80049CF8`. Its two sequence-start paths retain distinct declaration views:
`SD_PlaySequence` calls the canonical `SD_StartSequenceTracks(void)`, while
`SD_PlaySequenceFastForward` uses a narrow same-symbol no-argument alias
matching its original translation unit. The adjacent functions on both sides
require `gcc_2_8_1_g8_split`, fixing the restored unit's boundaries.

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

- `SD_VabOpenHead` casts the shared state pointer to a byte view while
  initializing the transfer window and preserving its required register
  allocation.
- `func_8004A7C0` calculates the `0x28`-byte object address explicitly.
  Replacing it with `&D_8009B458->objects[index]` changes the linked
  executable at `0x8004A7C8`.
- `SD_HandleSequenceControlChange` retains explicit byte-pointer arithmetic for
  the channel records and secondary objects, while stable fields use the
  shared types.

`SD_HandleSequenceMetaEvent` likewise keeps a byte pointer for the still-unmodeled
`0x2C`-stride work-record region, but uses `SDSecondaryState` members for its
verified scalar fields. These raw expressions are layout/code-generation
views, not competing global declarations.

Unchiga's generated `m2c_types.h` and focused decompilation sources corroborate
the `0x848` clear size and several offsets, but the declarations above were
derived and exact-tested from this repository's matched C.

## Three active fade ramps

The following `SDValue` members name the existing producer-to-output paths,
without changing their types, offsets, arithmetic or ownership:

| Offset | Field | Existing type and role |
| --- | --- | --- |
| `0x0049` | `cd_fade_target` | `u8`, endpoint for the existing `cd_volume` ramp. |
| `0x0512` | `cd_fade_step` | `s16`, signed increment applied to `cd_volume`. |
| `0x1580` | `bgm_fade_level` | `s16`, first multiplier of the secondary master level. |
| `0x1582` | `bgm_fade_step` | `s16`, increment set by the BGM fade setters and playback request. |
| `0x1584` | `bgm_fade_target` | `u8`, endpoint of that first multiplier. |
| `0x1586` | `sequence_fade_level` | `s16`, second secondary-master multiplier. |
| `0x1588` | `sequence_fade_step` | `s16`, increment used by sequence start/stop and streaming callbacks. |
| `0x158A` | `sequence_fade_target` | `u8`, second multiplier's endpoint. |

`SD_UpdateFades` updates the sequence ramp first, the BGM ramp second and
the CD ramp last. Each nonzero step is added through the original signed
halfword store before the original negative/positive endpoint tests.
Reaching the endpoint clears that ramp's step and stores its target level.
Zero step means no update; changing a target alone does not start a ramp.
These are increments per updater invocation, not durations or guaranteed
once-per-frame counters. No widening, clamp, range check or normalization
is added.

When either secondary-master ramp is active, the existing arithmetic
multiplies the base level by `(bgm_fade_level + 1)`, shifts eight, then
multiplies by `(sequence_fade_level + 1)` and shifts again, retaining
the original zero branches and narrow intermediates before publishing
both channels through `SD_SetSecondaryMasterLevels`. The CD path remains
separate and calls `SD_SetCdVolume` after its own update.

The playback request seeds the BGM step and target `255`; sequence start
sets the sequence target to `255` and imports its existing step. Reset and
initialization keep their distinct stores and pointer reloads. The immediate
CD-volume setter still sets current and target together and clears the step.
Queued CD fades still use command fields `0x0008` and `0x0002`, and their
existing optional wait still tests the CD step.

Do not repair the cross-ramp writes: commands `0x44`/`0x45` test and set
the sequence step but store the BGM target. The stream callbacks likewise
write a sequence step while retargeting the BGM ramp to `220` or `255`;
they do not change the sequence target. The previously documented clamp
behavior, branch polarity and repeated alias-sensitive reads remain exact.

`SDSecondaryState.field_0512` is a different signed halfword at the same
relative offset in a different backing object. Its initialization, spatial
scaling, command consumers and layout assertion remain untouched. The
`sound_init.c` secondary-state store is not a CD-fade-step store.

The six tentative sound-function names remain tentative. Their existing
uncalled-duplicate and voice-ID-domain caveats are not resolved by naming
the active ramp members. No caller, reachability, historical naming evidence,
callback signature, initialized table, compiler profile or executable
control is changed.

## Complete SPU voice-key status snapshots

`SDValue.voice_key_status` is one `u8[24]` at `+0x15D8`, replacing the
old twenty-byte `field_15D8` followed by the four byte members
`field_15EC` through `field_15EF`. No byte is added, removed or reordered;
the following padding and all later members stay at their original offsets.
The existing `+0x15EC` layout assertion now checks array element twenty.

The extent follows the actual producer, not the four sound-effect slots.
The retained SDK `SpuGetAllKeysStatus` at `0x800773C4` sets its loop limit
to twenty-four, writes exactly one byte per iteration, increments the
destination by one and returns after all twenty-four voices. Its four
stored values are the existing SDK key/envelope status codes, not Boolean
active flags. The SDK implementation and declaration remain unchanged.

`SD_UpdateRuntime` and `SD_KeyOffVoiceSlots` pass this entire buffer to
that SDK function. Runtime then tests elements twenty through twenty-three
for the existing literal status three and forms the original four SPU
voice masks. No new loop, symbolic-status substitution or cached pointer
replaces those individual reads.

The key-off retry path intentionally keeps the original sum:
`status[23] + status[21] + status[22] + status[23]`. It still reads element
twenty-three twice and does not include element twenty. The all-slot key
mask, SDK call order, nonzero-sum test, increment and twenty-four-attempt
limit remain exact. This type recovery does not repair that asymmetry or
assert that retry completion proves all four voices are off.

`SD_UpdateSecondaryObjectVoices` keeps its separate stack-local
twenty-four-byte snapshot; only its old `sp10` identifier becomes
`voice_key_status`. Its cursor walk, object-count bound, lifecycle stages,
channel ownership and key-off cleanup remain unchanged. This local buffer
is not merged with the retained `SDValue` buffer or with the twenty
secondary objects, sixteen channels or sixteen sequence tracks.

All three actual SDK call sites retain their original `char *` casts,
storage duration and pointer reloads. There is no new structure, union,
allocation, status refresh, bound, test harness or callback signature.

### Music-package table and cached open results

The package request, VAB-open, sequence-open and cleanup paths establish one
connected set of fields. These are local usage-derived names, not recovered
original declarations:

| Location | Field | Established role |
|---|---|---|
| `SDValue +0x1560` | `music_table` | Pointer to eight-byte `SDMusicTableEntry` records, initialized to `0x801E2000`. |
| table entry `+0x00` | `spu_sector_count` | Byte count of sectors expanded to bytes for the SPU phase. |
| table entry `+0x01` | `ram_sector_count` | Byte count of sectors expanded to bytes for the RAM phase. |
| table entry `+0x02` | `sector_offset` | Unsigned halfword passed unshifted as the file-relative sector offset. |
| table entry `+0x04` | `spu_address` | Signed word passed as the SPU destination, not a CPU pointer. |
| `SDValue +0x1578` | `opened_music_package_id` | Package's leading halfword copied by `SD_OpenMusicVab` after its VAB-open path succeeds. |
| `SDValue +0x157A` | `music_vab_open_result` | Narrowed return from `SD_VabOpenHead`; also passed as the VAB argument to `SD_OpenSequence`. |
| `SDValue +0x157C` | `opened_sequence_command` | `active_command_value` token copied after sequence-open returns zero. |
| `SDValue +0x157E` | `sequence_open_result` | Narrowed return from `SD_OpenSequence`, tested by the stop/close paths. |

`sound.h` imports the existing `SDMusicTableEntry` owner instead of retaining
an untyped byte pointer for its table. The initializer uses that pointer type.
The request still selects the record through its original integer byte sum:
`((arg0 & 0xFF0) >> 4) * sizeof(SDMusicTableEntry)` plus the table base.
It does not become typed pointer arithmetic with an extra eight-byte scale.
The signed `arg0 >> 4` cache comparison remains distinct from that mask, and
the caller's original signed-halfword argument and lack of bounds checks remain.

The earlier RAM/SPU count labels were reversed. The corrected names follow
the complete `SD_EnqueueSpuRamTransfer` command-tail and transfer path:

| Wrapper argument | Queued `SDCommand` | Active `SDValue` | `FileRequestSlot` | Consumer |
|---|---|---|---|---|
| fourth (`a3`) | `field_0024` | `active_command_tail[5]` | `field_14` at `+0x14` | SPU phase size, transfer mode 3 |
| fifth (`a4`) | `field_0028` | `active_command_tail[6]` | `field_18` at `+0x18` | RAM phase size, transfer mode 1 |

`SD_EnqueueCommand` copies the complete command without rearranging its
members. `SD_UpdateRuntime` copies the eight tail words from `+0x10`
unchanged, and `SD_StepActiveCommand` assigns the indexed words above.
`SD_ConfigureTransferPhase` selects SPU mode 3 before RAM mode 1;
`File_TransferReadyCallback` confirms mode 3 through `SpuSetTransferStartAddr`
and `SpuWrite`, rather than a CPU destination copy. The first wrapper
argument becomes the SPU address at `+0x0C`, and the third becomes the
sector offset at `+0x04`. Both table counts still shift by
`FILE_SECTOR_SHIFT`; swapping both declarations and their request-site names
preserves the original field offsets, loaded bytes and transfer behavior.
Assertions pin the SPU count at zero and RAM count at one.
`File_RequestSecondarySlot` passes the offset to `File_InitTransferDescriptor`,
whose `File_SetTransferLocation` adds it to the selected file's LBA without
another unit conversion. Sector-rounded transfer lengths do not establish
exact logical payload lengths.

Both open-result fields remain signed halfwords, not Boolean flags or newly
invented hardware handles. The local implementations return zero on success
and minus one on rejection; existing callers retain their different zero and
minus-one tests, narrowing, flags and ambient-argument call forms. In
particular, the stop/close implementations do not consume the apparent
sequence argument passed by these callers. No public contract is changed.

The two copied tokens are not fresh playback-status observations.
`opened_music_package_id` has no reader in the tracked resident C.
`SD_ResetMusicState` leaves `opened_sequence_command` unchanged even after
closing the sequence, whereas `SD_ResetRuntime` and secondary-runtime
initialization clear it. Command 72 still does not write the package-ID cache.
All these asymmetric stores, stale-token lifetimes, call orders and pointer
reloads are retained. Existing fade fields and the full 24-voice status
array are unchanged; no package buffer, command record or public API is retyped.

### Sound-effect slot allocation and volume attribute packet

The active sound-effect chain supplies both allocation and output evidence:
`SD_SEPlay` reads `SDNote.priority`, passes it to
`SD_AllocateSoundEffectVoice`, and the selected-slot start helper stores it
in `SDValue.voice_priority[4]` at `+0x040C`. The fallback allocator accepts
an occupied slot when the incoming byte is greater than or equal to that
slot's priority. This is not the per-slot volume/gain byte.

`voice_allocation_cursor` names the existing byte at `+0x0435`.
Group/occurrence selection stores the chosen slot there and returns after
starting it. Fallback scans advance and mask the cursor after every attempted
slot, including a successful start. It is therefore not always the last
selected voice. Silent-slot search still precedes priority stealing;
equal priority remains eligible. Both initialization and runtime reset clear
the cursor and priorities in their original order.

`SD_ApplyVoiceSlotVolume` uses a separate SDK `SpuVoiceAttr` packet at
`+0x03C4`, now `voice_volume_attr`, immediately after the existing key-on
packet `voice_attr`. The four former scalar fields and following `0x34`
padding bytes occupy exactly one `0x40`-byte SDK record, ending at the
existing voice-ID array at `+0x0404`. Its observed fields are:

| Offset | SDK member | Use |
|---|---|---|
| `0x03C4` | `voice_volume_attr.voice` | Conditional shift of the existing four-slot SPU mask. |
| `0x03C8` | `voice_volume_attr.mask` | `SPU_VOICE_VOLL \| SPU_VOICE_VOLR`, not a key-on request. |
| `0x03CC` | `voice_volume_attr.volume.left` | Low halfword of the unsigned left-volume/gain product shifted by eight. |
| `0x03CE` | `voice_volume_attr.volume.right` | Corresponding right-volume/gain product. |

The SDK declaration supplies unsigned 32-bit voice/mask words and signed
16-bit volume members. The old view used signed words and unsigned volume
halfwords, but these sites only store and submit them: the unsigned product
arithmetic, shifts and stored bits remain unchanged. No new read, clamp,
sign extension or reinterpretation of the slot-volume arrays is introduced.
`SpuSetVoiceAttr` now receives the typed member address without a cast.

The mask assignment still precedes the voice assignment in C; reversing
them changes the retail register allocation. The temporary product reuse,
global-pointer observation, conditional mask shift and SDK-call order remain.
The key-on packet, its initialization and the unused volume-packet fields are
not merged or newly initialized. Both complete packet boundaries, the four
observed SDK offsets, allocation fields and total `SDValue` size are pinned.

All existing call contracts, numeric literals, native bindings, grouped
sources and named compiler profiles remain. The existing allocator witness
changes only the two member identifiers, including both sides of its cursor
mutation; inputs, state oracles, event sequences and negative controls stay
identical. No test case or harness is added or executed locally.
The incidental cursor spelling in `tent_SD_KeyOffVoiceById` follows the same
member rename without promoting that helper or removing its reachability
caveat.

### Secondary-transfer filename buffers

The initializer and command-`0x20` request branches establish the three
separate filename buffers in `SDValue`:

| Offset | Member | Initial text | Copied bytes, including NUL |
|---|---|---|---|
| `+0x1619` | `bgm_file_name` | `SD_bgm.dat` | 11 |
| `+0x1629` | `se_file_name` | `SD_se.dat` | 10 |
| `+0x1639` | `master_xa_file_name` | `MASTER.XA` | 10 |

The literal bytes are independently checked in the retail loaded image at
`0x80010784`, `0x80010790` and `0x8001079C`, respectively, not inferred
from the existing `gSD_szBgmFileName`, `gSD_szSeFileName` and
`gSD_szMasterXaFileName` symbol spellings. Preserve the mixed-case text.
`SD_InitState` first clears the resident state through `0x801EA7FF`, then
copies these strings with its existing `SDInitBlk11`/`SDInitBlk10`
assignments. Those block types contain exactly eleven or ten bytes;
the copies include each terminator, not a sixteen-byte string copy.

The existing `u8[0x10]` declarations remain, with their offsets and extents
pinned. The historical extent caveat above still applies: separately
passed addresses establish three starts, not the complete ownership or
capacity of each interval. Neither the string lengths nor this naming
supplies new evidence for the retained sixteen-byte extents.

In `SD_StepActiveCommand`, the three existing branches of
`active_command_tail[0] & 0xF0` select SE at `0x10` with request file index 4,
BGM at `0x20` with index 5, and MASTER.XA at `0x40` with index 6.
Each passes its buffer as the second argument to
`File_RequestSecondarySlot`. The ordinary nonzero-size data-transfer path
forwards that pointer to `File_InitTransferDescriptor`, which stores it
in `FileTransferDescriptor.loader_argument` at `+0x18`.
This establishes initialized filename storage and a descriptor handoff,
not a downstream text lookup, open operation, diagnostic display or DMA
destination. No such reader is established by this route; file-index
selection remains independent of the pointer's text.

Only the three member names change in the two C units. Preserve the block
assignments, copy widths, array types, request payloads, masks, guards,
busy callback, phase updates and all reloads. The initializer's old
"three tables ... from ROM" comment is corrected to loaded-image file
names; no initialization or transfer behavior changes.

### Sound-bank header geometry and startup phases

The packed eight-byte header copy in `SD_UpdateBankLoad`, the initializer,
pending-note reset and dynamic-voice key-off establish these shared members:

| Location | Field | Established role |
|---|---|---|
| `SDValue +0x0000` | `note_id_count` | Number of halfword ID-map entries initialized before the aligned note-table base. |
| `SDValue +0x0002` | `value_link_count` | Number of eight-byte link records used for the table load and source-sector calculation. |
| `SDValue +0x0004` | `dynamic_note_start` | Shared reset/key-off boundary for sound IDs and reset value of the note-table append index. |
| `SDValue +0x003C` | `bank_load_phase` | Unsigned startup bank-loader phase, separate from active-command and transfer phases. |

`SD_InitVoiceState` clears ID-map entries `[0, note_id_count)` to the
existing `0xFFFF` marker. It locates `note_table` after that many halfwords
at `0x801E4000`, using the original sixteen-byte alignment expression.
The name does not turn this ID-map extent into a separately measured
note-table capacity or add an import-key check.

`SD_UpdateBankLoad` copies the entire eight-byte header from `0x801E6800`
with the existing packed-word assignment, then loads `value_link_count * 8`
bytes into `0x801E2800`. `SD_QueueValueLinkTransfer` uses the same count to
skip `ceil(value_link_count * 8 / 0x800) + 1` source sectors before adding
the selected link's `sector_offset`. The original integer operations,
fixed bases, unsigned halfwords and unchecked masked link selection remain.
The header's remaining two bytes are not assigned a new meaning.

`SD_ResetPendingEntries` invalidates only IDs in
`[dynamic_note_start, note_id_count)` and resets `next_note_index` to that
same raw boundary. `SD_KeyOffDynamicVoices` compares each slot's sound ID,
not its mapped note index, against it. The two domains deliberately keep
their existing shared value; the name does not claim a count of live
resident notes or require a populated prefix. The first-link SPU cursor
reset, duplicate-request behavior and importer ordering remain unchanged.

`SD_InitDataSourceFlags` resets `bank_load_phase` to zero and retains all
existing flag masks. The loader keeps cases 0 through 9, conditional jumps
by six or two, per-arm increments, shared transfer-call block and `0x3E7`
skip path. Its default arm clears startup-busy bit `0x08` only when the
low three `SD_GetStatusFlags` bits are clear; it does not normalize the
phase. A failed mixer-signature check does not block the case-7/9
increments. In particular, the initializer's `0x40` clear and loader's
`0x80` test are not made consistent. No enum, bound check, initialization,
transfer or scheduling change is added.

### Note import, sample addresses and value-link transfer state

The existing initializer, two note importers, playback/parameter consumers
and value-link request paths establish one connected memory lifecycle:

| Location | Field | Established role |
|---|---|---|
| `SDValue +0x0438` | `next_link_spu_address` | SPU destination passed to the next value-link transfer and its queued note-import command, then advanced by the selected link's raw size word. |
| `SDValue +0x043C` | `note_index_by_id` | Halfword lookup from sound ID to note-table index, with the existing `0xFFFF` missing-entry marker. |
| `SDValue +0x0440` | `next_note_index` | Halfword append index assigned to each imported live key, incremented only for non-sentinel input keys. |
| `SDValue +0x0442` | `value_link_request_index` | Individual-request duplicate-suppression cache, not proof of a completed load or the last link in a list. |
| `SDValue +0x0444` | `note_table` | Pointer to the existing eight-byte `SDNote` records. |
| `SDValue +0x0448` | `value_links` | Pointer to the existing eight-byte `SDValueLink` records. |
| `SDNote +0x06` | `spu_address_div16` | Halfword sample address in sixteen-byte units, relocated by the importers and shifted back by playback. |
| `SDValueLink +0x04` | `spu_size` | Raw SPU phase-size word, also used as the destination cursor increment. |

`SD_InitVoiceState` keeps the link table at `0x801E2800`, ID map at
`0x801E4000`, and its original sixteen-byte-aligned integer address
calculation for the note-table base. `SD_ResetPendingEntries` still clears
only the original ID range from `dynamic_note_start` to `note_id_count`, resets the
request cache to `0xFFFF`, copies `dynamic_note_start` into the append index and
sets the SPU cursor to the first link's size plus `0x1010`. These header
roles are detailed above; no new bound or capacity check is introduced.

`SD_RegisterSeqBlockNotes` binds each non-sentinel input key to the append
index, copies its note, adds `spu_addr >> 4` to the copied halfword and then
increments the index. `SD_LoadSequenceBankPair` retains the other-bank-first
copy order, its two fixed SPU bases, integer address-sum spelling, halfword
narrowing and per-key cursor advancement. `SD_StartSoundEffectVoice` reads
the mapped note's same halfword and shifts it left by four into the SDK
voice address. Relative input and relocated records share the existing
type; no new conversion, overflow handling or alignment guard is added.

`SD_QueueValueLinkTransfer` passes `spu_size` as argument four of
`SD_EnqueueSpuRamTransfer`: command `+0x24`, active tail word five,
`FileRequestSlot +0x14`, then SPU transfer mode three. It also queues type
`0x51` with the current SPU destination and the same RAM staging buffer,
then adds the original raw size word to the cursor. The name deliberately
does not promise an unconditional byte count: the shared phase code uses
positive sizes as bytes but transforms a negative signed value through
`-(value << FILE_SECTOR_SHIFT)`. The unsigned table field, signed call
conversion and raw cursor addition remain exact, not normalized.

The individual replace/request helpers retain their different zero-size
checks and original cache-store ordering. `SD_QueueValueLinkList` resets
the cache and queues its non-sentinel items without storing each list item
there. It therefore does not turn this cache into a last-loaded-link field.

Existing consumers are not made artificially consistent by the new names.
Play retains its map validation while passing the resolved sound ID to the
allocator. Existing setters retain their returned-index comparisons; pan
setters still read `note_table[voice_id]` directly. No extra map lookup,
comparison repair, SDK observation reordering or global-pointer caching is
introduced. All literal shifts, sentinels, integer sums and reloads remain.

The six state offsets, note address offset and link size offset are pinned
alongside the existing record and state size assertions. Stored pointers
retain their original types and `G32`. The two existing dispatch/allocator
witnesses change only member identifiers; every input, fixture, stub,
state oracle, event and negative control remains unchanged. No new test or
harness is added, and neither witness is executed locally.

### Mixer defaults, fade scale and secondary stereo masters

The cached bank bytes, reset path, fade calculation and both secondary
voice-volume submission paths establish five related fields:

| Owner and offset | Field | Established role |
|---|---|---|
| `SDValue +0x0044` | `secondary_mix_scale` (`u16`) | Active base scale for the BGM/sequence fade product. |
| `SDValue +0x1649` | `cd_mix_default` (`u8`) | Cached default restored into the existing CD `mix_scale`. |
| `SDValue +0x164A` | `secondary_mix_default` (`u8`) | Cached default restored into `secondary_mix_scale`. |
| `SDSecondaryState +0x0514` | `master_level_left` (`u16`) | Left per-voice master scale for the secondary sound engine. |
| `SDSecondaryState +0x0516` | `master_level_right` (`u16`) | Corresponding right scale. |

`SD_InitState` seeds the cached defaults to `0xFF` and `0xD2`, then copies
them into the two active scales. `SD_ResetRuntime` restores those same
cached bytes, not unconditional literals, before its existing cleanup
calls. `SD_UpdateBankLoad` replaces each cache and its active scale only
after the existing `VolInfo` trailer check succeeds. The `0x801E2000`
staging bank feeds the secondary default and the retail
`Set SMF Mixer Out:%d` diagnostic; `0x801E8800` feeds the CD default and
`Set CD Mixer Out:%d`. The source route and actual consumers establish
these roles, not the legacy SE/SMF names of the leading-signature symbols.
Those global symbols and their relocations are not renamed here.

The cache is a reset default, not a guarantee that a new SDK volume has
already been submitted. The bank-loader stores do not themselves invoke
`CdMix` or refresh the secondary voices. Existing refresh triggers and
signature-failure, disabled-source and pending-transfer paths remain exact.

`SD_UpdateFades` uses `secondary_mix_scale` as its existing `u16`
intermediate, multiplies by `(bgm_fade_level + 1)`, narrows and shifts
eight, then does the same with `(sequence_fade_level + 1)`. It retains
both zero branches and passes the narrowed result to both channels of
`SD_SetSecondaryMasterLevels`. This path is separate from
`SD_SetCdVolume`, which continues to combine `mix_scale`,
`mix_multiplier`, the requested CD level and channel-volume bytes.
No product is widened, folded, clamped or made newly unconditional.

`SD_ResetSecondaryState` still initializes the two stereo masters to
127, not 128. `SD_SetSecondaryMasterLevels` still accepts signed `s16`
arguments, stores their bits into the existing unsigned halfwords, and
skips object-volume refresh in sequence state two. Both note-on and
`SD_SetVoiceVolume` multiply the object's channel level by its master
and shift seven before submitting SDK voice volumes. The latter's
explicit `u16` casts remain. These are per-voice scales, not the SPU's
global hardware master-volume registers or a gain for the separate
four-slot sound-effect engine.

The earlier field table's blanket `s16` description for all three
`+0x0512/+0x0514/+0x0516` fields is corrected: only `field_0512` is signed
in the existing shared declaration. No member type or signedness changes.
Existing main-state offset assertions follow the new names; new assertions
pin the stereo master offsets. Every reload, numeric literal, SDK call,
return gate, compiler profile and witness/control remains unchanged.

### Sequence source, open token and read-base lifecycle

Five named secondary-state fields connect sequence opening to playback,
track parsing and closing without changing their declarations' types:
`sequence_read_base` at `+0x07DC`, `sequence_open_token` at `+0x07E0`,
`sequence_state` at `+0x07E2`, `sequence_source` at `+0x07E8`, and
`sequence_read_limit` at `+0x07EC`. The two stored pointers retain `G32`,
the token and state remain signed halfwords, and the limit remains `s32`.

`sequence_open_token` is deliberately not called a VAB index or a sequence
access number. `SD_OpenSequence` copies its second argument, currently
named `vab_id`, into it; the command pump supplies the main state's
`music_vab_open_result`. The actual `SD_VabOpenHead` returns zero on
success and `-1` on failure, and no matched consumer indexes a bank with
the retained token. Its established use is the `-1` eligibility gate for
open, play, fast-forward, stop and close. The separate open call return
is still stored in `SDValue.sequence_open_result`; it is not this field.

After the existing tag and open-token checks, opening stores the input
as `sequence_source` and sets state two. It does not also replace
`sequence_read_base`. Play and fast-forward set the limit to `0x10000`,
copy the retained source into the read base, then start the tracks.
The byte reader uses that base plus each track's `pos`; the pointer itself
does not advance. `SD_FindMidiTrackChunk` and the track pump's fast-forward
address calculation use the same base. Stop and close do not clear either
pointer; closing restores the token to `-1` and state zero without adding
allocation, copying, deallocation or a new lifetime guarantee.

The existing state-value table above remains the complete writer map.
Initialization's zero writer is `SD_ResetSecondaryState`, not its caller
`SD_InitState`. Polling alone changes playing state one into ended state
three, including the zero-track case; the track pump never publishes that
transition itself. Its track-array-relative `STATE_WORD(s16, 0x7E2)`
access remains raw to preserve register allocation; only its explanatory
member-name comment changes. Mute/restore states four/one, staged/stopped
state two, both volume-refresh gates and all flag-store ordering remain.

`sequence_read_limit` is a fixed software offset guard, not an actual
allocation size or a newly enforced pre-read bound. The byte reader still
loads first, increments `pos`, and only then compares the incremented
offset against the limit using the existing unsigned casts. The `MTrk`
search still compares four bytes before incrementing and testing its loop
bound. Header parsing still writes `0x10000` at its existing point.
No precheck, clamp, extent inference or error-return change is introduced.

Existing assertions pin the renamed read base and token; one new block
pins state, source and limit. Pointer reloads, alias views, the invalid-tag
return before clearing `timer_update_inhibit`, ambient-argument declarations, compiler
profiles and every runtime literal remain exact. No witness needs a
member-name migration; no new test or harness is added, and no cloud or
witness suite is run locally.
