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
| `0x0040` | `flags_0040` | Multiple control and cleanup routines set/test driver status bits. |
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

## Structure safeguards

The header contains compile-time size assertions for:

- `SDCommand`: `0x30`;
- `SDValueLink`: `0x08`;
- `SDValue`: `0x164C`.

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

## Secondary state (`D_8009B458`)

`D_8009B458` is a second global pointer used by the sequence/stream side of the
sound driver. `func_800494F4` installs the pointer and clears exactly 530
32-bit words, establishing a total allocation size of `0x848` bytes.

`src/game/sound.h` defines the partial `SDSecondaryState`,
`SDSecondaryTransfer`, `SDSecondaryRecord`, and `SDSecondaryObject` views.
Offset-based names are retained except where several matched functions
establish a stable role. Unmodeled and overlapping regions remain padding or
explicit typed/raw views rather than speculative fields.

### Confirmed secondary-state fields

| Offset | Width | Field | Local matching-C evidence |
|---|---:|---|---|
| `0x0000` | `0x18` stride | `SDSecondaryRecord` view | `func_8004B49C`, `func_8004B6E8`, and `func_8004B70C` index the same records and establish byte fields at `+0x00`, `+0x01`, `+0x03`, `+0x05`-`+0x07`, and `+0x10`-`+0x13`. |
| `0x0180` | `0x28` stride | `objects[20]` | `func_8004A7C0`, `func_8004B49C`, and `func_8004C84C` establish the object base/stride; additional matched inline-assembly functions use the same view. Verified members are bytes at `+0x03` and `+0x0F`, and a `u16` at `+0x1E`. |
| `0x04A4` | `0x1C` | `transfer` | `func_80049434`, `func_800496C4`, `func_8004975C`, `func_800497E0`, and `func_800498F8`. Members are `s16 +0x00`, pointer `+0x04`, `s32 +0x08/+0x0C/+0x10`, pointer `+0x14`, and bytes `+0x18`-`+0x1B`. |
| `0x0500`-`0x0503` | `u8` | `flag_0500`-`flag_0503` | Initialization, playback, update, and shutdown routines independently read/write these flags. |
| `0x0504` | pointer | `field_0504` | `func_8004B910` passes it to both stop/cleanup calls. |
| `0x0508` | `u8` | `field_0508` | `func_8004B734` increments and wraps it at 11. |
| `0x0509` | `u8` | `field_0509` | `func_8004695C`, `func_80047050`, and `func_8004B734` set/test it. |
| `0x050C` | callback pointer | `field_050C` | `func_8004B734` conditionally invokes it. |
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

The header uses GCC-2.8.1-compatible negative-array assertions for the
`0x18`, `0x28`, and `0x1C` subview sizes, the complete `0x848` state size, and
the major top-level offsets.

### Migration status and exact-code exceptions

Every matching-C user outside the GCC inline-assembly exceptions below now
includes `sound.h` and uses the shared typed global. Every accepted migration
batch retained the exact full executable hash.

Six matching-C functions containing GCC inline assembly remain unchanged and
keep their local raw declarations: `func_80049CF8`, `func_80049DD8`,
`func_8004A2F8`, `func_8004A854`, `func_8004B734`, and `func_8004C77C`.

Three migrated functions retain explicit raw indexing where the shared type
cannot replace the exact source shape:

- `func_800496C4` casts the shared state pointer to a byte view while
  initializing the transfer window and preserving its required register
  allocation.
- `func_8004A7C0` calculates the `0x28`-byte object address explicitly.
  Replacing it with `&D_8009B458->objects[index]` changes the linked
  executable at `0x8004A7C8`.
- `func_8004B49C` retains byte-pointer iteration for the overlapping
  `0x18`-byte record and `0x28`-byte object views, while stable fields use the
  shared types.

`func_8004BE88` likewise keeps a byte pointer for the still-unmodeled
`0x2C`-stride work-record region, but uses `SDSecondaryState` members for its
verified scalar fields. These raw expressions are layout/code-generation
views, not competing global declarations.

Unchiga's generated `m2c_types.h` and focused decompilation sources corroborate
the `0x848` clear size and several offsets, but the declarations above were
derived and exact-tested from this repository's matched C.
