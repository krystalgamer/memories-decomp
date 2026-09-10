# Frontend debug numeric-editor state

`src/game/frontend_debug_state.h` owns the numeric editor's shared data
contracts. `duel_interface_setup.h` includes it for the producer API;
the menu dispatcher, scene/sound states, cursor updater, async return path,
and retained `func_80030294` candidate consume the same declarations.
The raw-word `func_80030998` also includes it without changing any assembly.
This is declaration/type work, not a new C storage mapping or semantic rename.

## Three-row evidence

`func_80030250` stores three caret columns at `0x8009B2B4..B2B6` and initializes
three digit counts at `0x8009B2C0..B2C2`. The sound editor `func_800307B8`
requests three rows with columns `0x11, 0x19, 0x21`, then overrides rows 1
and 2 to three digits. Its format at `D_80090CB4` contains three conversions:
`%04X`, `%03X`, `%03X`. The scene/menu/movie states request one row and
use their corresponding single-conversion strings from
`frontend_debug_tables.c`.

The candidate indexes those columns and counts with `D_8009B2DC`, advances
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
digit stores. `FRONTEND_DEBUG_ROW_VIEWS` gives the candidate bounded
three-byte arrays; both remain inside its assembler's `-G4` limit.
The saved sound pair is selected by `FRONTEND_DEBUG_SOUND_ROW_VIEW` in
`func_800307B8`; its four-byte size remains small data under that TU's profile.
No array is a new definition, and no interior label is folded into a base
relocation.

The values label deliberately remains `extern u16 gDebug_nSceneOrSoundID`
even in the candidate. Declaring a six-byte array produced identical
normalized instructions but changed `.extern` size from 2 to 6. Under
`gcc_2_8_1_cc_g8_as_g4_split`, that crosses the assembler's small-data limit
and changes relocation selection, invalidating the fingerprint. Taking the
bounded `FrontendDebugValues` view at the scalar label's address instead
preserves the original relocations. This is a view of linker-backed storage,
not permission to index an ordinary standalone C scalar beyond its extent.

`D_8009B2EC` and `func_80030250`'s first parameter now use `u8 *`, matching
the C-owned format arrays and the producer-to-`FntPrint` path. The candidate
converts to the printer's existing `char *` contract only at that boundary.
No calling convention, pointer width, format contents, or storage changes.
`D_800EAED8` stays incomplete: the caret clear loop writes 40 bytes, but that
does not prove the extent of every possible indexed write.

The row/digit control labels stay unsigned bytes. Existing signed reads of
`D_8009B2DC` and `D_8009B2E9` remain explicit, as do low-byte reads of values.
`D_8009B2DE` remains a halfword and `D_8009B2EB` a byte of menu-state flags.
There are no tentative definitions, section-attribute changes, or new pins.

## Setup consumers and verification

`func_80030198` now keeps the created text box as `DuelEffectChannel *`
and writes its existing `field_5A`/`field_5B` members. The cursor object is
`DisplayObject *`, consistent with every consumer of `D_8009B2E4`.
The four byte stores at offsets `0x45, 0x3D, 0x35, 0x2D` address byte 1
within the existing `field_44`, `field_3C`, `field_34`, and `field_2C` words.
They must remain byte stores, not whole-word colour assignments.

The retained candidate is still a near miss, not promoted matching C.
All 19 candidate object fingerprints remain unchanged; its dependency
metadata removes only the six obsolete local extern entries now supplied
by the shared header. The original target bytes and compiler profiles stay
unchanged. Resident acceptance remains the full-executable retail SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.

Pending source moves must carry the new include (adjusting its relative path
when moving into `src/candidates/`) and the sound-view selector. Owning-function
header additions are independent of this state header; preserve both.
