# Frontend debug numeric-editor state

`src/game/frontend_debug_state.h` owns the numeric editor's shared data
contracts. `duel_interface_setup.h` includes it for the producer API;
the menu dispatcher, scene/sound states, cursor updater, async return path,
and matching `func_80030294` consume the same declarations.
The raw-word `DebugMenu_UpdateCampaignEntry` also includes it without changing
any assembly.
The header still declares views only; it adds no C-owned backing storage.

## Three-row evidence

`func_80030250` stores three caret columns at `0x8009B2B4..B2B6` and initializes
three digit counts at `0x8009B2C0..B2C2`. The sound editor `DebugMenu_UpdateSoundEntry`
requests three rows with columns `0x11, 0x19, 0x21`, then overrides rows 1
and 2 to three digits. Its format at `D_80090CB4` contains three conversions:
`%04X`, `%03X`, `%03X`. The scene/menu/movie states request one row and
use their corresponding single-conversion strings from
`frontend_debug_tables.c`.

The editor indexes those columns and counts with `D_8009B2DC`, advances
and clamps the row against `D_8009B2E0`, and uses `D_8009B2E9` as the digit
cursor. It reads the columns signed, as the previous `s8 *` cast did.
`FrontendDebugColumns` and `FrontendDebugDigitCounts` therefore describe
three bytes, not the gaps up to the following globals.

Values occupy three halfwords beginning at `gDebug_nSceneOrSoundID`
(`0x8009B2C8`), with independently relocated interior labels `D_8009B2CA`
and `D_8009B2CC`. The editor steps by halfwords and passes these three
values to `FntPrint`. `FrontendDebugValues.row[3]` describes that observed
window; size and row-offset assertions fix it to six bytes and offsets 2/4.
The sound state's saved values at `gDebug_nLastSoundID` (`0x8009B2C4`) and
the following halfword use a separate two-element view. This replaces its
out-of-bounds scalar `*(&gDebug_nLastSoundID + 1)` accesses without inventing
a semantic name for the second value.

## Addressing and ABI constraints

Scalar producers retain their original labels for the three column and
digit stores. `FRONTEND_DEBUG_ROW_VIEWS` gives the matching editor bounded
three-byte arrays; both remain inside its assembler's `-G8` limit.
The saved sound pair is selected by `FRONTEND_DEBUG_SOUND_ROW_VIEW` in
`DebugMenu_UpdateSoundEntry`; its four-byte size remains small data under that TU's profile.
No array is a new definition, and no interior label is folded into a base
relocation.

The values label deliberately remains `extern u16 gDebug_nSceneOrSoundID`.
Under uniform `gcc_2_8_1_g8_split`, its scalar size keeps the gp-relative base
used by retail. The editor takes bounded row views at that linker-backed
address; a local union prevents GCC from coalescing the first address with its
symbol base, while the second arm uses a direct one-use index.

`D_8009B2EC` and `func_80030250`'s first parameter now use `u8 *`, matching
the C-owned format arrays and the producer-to-`FntPrint` path. The editor
converts to the printer's existing `char *` contract only at that boundary.
No calling convention, pointer width, format contents, or storage changes.
`D_800EAED8` stays incomplete: the caret clear loop writes 40 bytes, but that
does not prove the extent of every possible indexed write.

The row/digit control labels stay unsigned bytes. Existing signed reads of
`D_8009B2DC` and `D_8009B2E9` remain explicit, as do low-byte reads of values.
`D_8009B2DE` remains a halfword and `D_8009B2EB` a byte of menu-state flags.
There are no tentative definitions, section-attribute changes, or new pins.

## Setup consumers and verification

`DebugMenu_Init` now keeps the created text box as `DuelEffectChannel *`
and writes its existing `field_5A`/`field_5B` members. The cursor object is
`DisplayObject *`, consistent with every consumer of `D_8009B2E4`.
The four byte stores at offsets `0x45, 0x3D, 0x35, 0x2D` address byte 1
within the existing `field_44`, `field_3C`, `field_34`, and `field_2C` words.
They must remain byte stores, not whole-word colour assignments.

Matching `func_80030294` owns its three initialized local tables as `0x38`
bytes of `.rodata` at ROM `0xA50`. Its uniform G8 object, declarations, and
full executable reproduce the retail SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
