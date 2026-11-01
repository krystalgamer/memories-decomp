# Frontend debug numeric-editor state

`src/game/frontend_debug_state.h` owns the numeric editor's shared data
contracts. `duel_interface_setup.h` includes it for the producer API;
the menu dispatcher, scene/sound states, cursor updater, async return path,
and matching `DebugMenu_UpdateValueEditor` consume the same declarations.
The raw-word `DebugMenu_UpdateCampaignEntry` also includes it without changing
any assembly.
The header still declares views only; it adds no C-owned backing storage.

## Configuration identities

The isolated `debug_value_editor_config.h` supplies canonical names while
the original declaration owners remain unchanged:

| NA address | Former identity | Canonical role |
| --- | --- | --- |
| `8009B2B4` | `tent_DebugInterfaceFieldB4` | `gDebug_abEditorCaretColumns` |
| `8009B2B5` | `tent_DebugInterfaceFieldB5` | `gDebug_bEditorCaretColumn1` |
| `8009B2B6` | `tent_DebugInterfaceFieldB6` | `gDebug_bEditorCaretColumn2` |
| `8009B2B8` | `tent_DebugInterfaceFieldB8` | `gDebug_bEditorLeadingLineCount` |
| `8009B2C0` | `tent_DebugInterfaceFieldC0` | `gDebug_abEditorDigitCounts` |
| `8009B2C1` | `tent_DebugInterfaceFieldC1` | `gDebug_bEditorDigitCount1` |
| `8009B2C2` | `tent_DebugInterfaceFieldC2` | `gDebug_bEditorDigitCount2` |
| `8009B2E0` | `tent_DebugInterfaceFieldE0` | `gDebug_bEditorRowCount` |
| `8009B2EA` | `tent_DebugInterfaceFlags` | `gDebug_bEditorFlags` |
| `8009B2EC` | `tent_DebugInterfaceData` | `gDebug_pszEditorFormat` |

`DebugMenu_OpenValueEditor` stores the caller's three caret anchors,
leading-line count and row count, initializes the three digit widths,
clears row/digit cursors and flags, and publishes the format pointer.
The editor places the caret at the active row's anchor minus the selected
digit. Digit widths govern the existing arithmetic mask and last-digit
selection during row transitions; the sound/language entries retain
their independent width overrides for rows 1 and 2.

Leading lines and editable rows are different counts. The former is cast
to `s8` and decremented while printing newline strings before the values;
the latter bounds/clamps the existing row traversal. All byte truncations,
signed reads, raw counts and invalid-input behavior remain unchanged.

The flags byte is cleared by setup. Bit `0x80` controls first-entry
initialization and bit `0x01` suppresses input during campaign preview.
The existing `0x40` branch performs decimal-nibble handling, but this
trace does not establish a live C setter of that bit. No new flag enum,
mode policy, input guard or normalization follows from the name.

The format pointer is passed through the original `(char *)` cast to
`FntPrint`, together with the three editor values. It is not the edited
value-storage pointer or the local row-pointer union. Its stored
`u8 *G32` view, mutability and printer boundary remain.

## Three-row evidence

`DebugMenu_OpenValueEditor` stores three caret columns at `0x8009B2B4..B2B6` and initializes
three digit counts at `0x8009B2C0..B2C2`. The sound editor `DebugMenu_UpdateSoundEntry`
requests three rows with columns `0x11, 0x19, 0x21`, then overrides rows 1
and 2 to three digits. Its format at `gDebugMenu_szSoundEditorFormat` contains three conversions:
`%04X`, `%03X`, `%03X`. The scene/menu/movie states request one row and
use their corresponding single-conversion strings from
`frontend_debug_tables.c`.

The editor indexes `gDebug_abEditorCaretColumns` and
`gDebug_abEditorDigitCounts` with `gDebug_bEditorRow`, advances
and clamps the row against `gDebug_bEditorRowCount`, and uses `gDebug_bEditorDigit` as the digit
cursor. It reads the columns signed, as the previous `s8 *` cast did.
`FrontendDebugColumns` and `FrontendDebugDigitCounts` therefore describe
three bytes, not the gaps up to the following globals.

Values occupy three halfwords beginning at `gDebug_nSceneOrSoundID`
(`0x8009B2C8`), with independently relocated interior labels `gDebug_nEditedBgmId`
and `gDebug_nEditedCdSoundId`. The editor steps by halfwords and passes these three
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
The canonical base names preserve the same selector: signed `s8[3]` caret
columns and unsigned `u8[3]` digit counts in the editor, unsigned scalar
row-0 labels in producers. Rows 1/2 retain their independent scalar labels.
Both triplets' interior bindings are verified in all seven images; the
new names do not fold those labels into base-plus-offset expressions or
add a packed configuration record.
The saved sound pair is selected by `FRONTEND_DEBUG_SOUND_ROW_VIEW` in
`DebugMenu_UpdateSoundEntry`; its four-byte size remains small data under that TU's profile.
No array is a new definition, and no interior label is folded into a base
relocation.

The values label deliberately remains `extern u16 gDebug_nSceneOrSoundID`.
Under uniform `gcc_2_8_1_g8_split`, its scalar size keeps the gp-relative base
used by retail. The editor takes bounded row views at that linker-backed
address; a local union prevents GCC from coalescing the first address with its
symbol base, while the second arm uses a direct one-use index.

`gDebug_pszEditorFormat` (legacy `D_8009B2EC`) and
`DebugMenu_OpenValueEditor`'s first parameter use `u8 *`, matching
the C-owned format arrays and the producer-to-`FntPrint` path. The editor
converts to the printer's existing `char *` contract only at that boundary.
No calling convention, pointer width, format contents, or storage changes.
`gDebug_szEditorCaretLine` stays incomplete: the caret clear loop writes 40 bytes, but that
does not prove the extent of every possible indexed write.

The row/digit control labels stay unsigned bytes. Existing signed reads of
`gDebug_bEditorRow` and `gDebug_bEditorDigit` remain explicit, as do low-byte reads of values.
`D_8009B2DE` remains a halfword and `gDebugMenu_bStep` a byte of menu-state flags.
There are no tentative definitions, section-attribute changes, or new pins.

All seventy logical bindings retain the original providers across seven
images, including the independently labeled interior rows. The ten
original NA addresses have no earlier global-map rows, so promotion appends
new evidence without rewriting accepted history. The flags byte is at
`0x8009B2EA`, not the neighboring digit or padding byte.

All existing test modules, old headers, wrappers, overlays/imports, source
groups and profiles stay frozen. The two C files reconstruct their complete
originals after removing only the canonical include and reversing the
scoped global names. No harness, fixture, probe or testcase is added, and
cloud/regional/overlay/witness suites are not executed locally. Native
compiler conservation does not replace fresh PR-side full image checks.

## Setup consumers and verification

`DebugMenu_Init` now keeps the created text box as `DuelEffectChannel *`
and writes its existing `field_5A`/`field_5B` members. The cursor object is
`DisplayObject *`, consistent with every consumer of `gDebugMenu_pCursorQuad`.
The four byte stores at offsets `0x45, 0x3D, 0x35, 0x2D` address byte 1
within the existing `field_44`, `field_3C`, `field_34`, and `field_2C` words.
They must remain byte stores, not whole-word colour assignments.

Matching `DebugMenu_UpdateValueEditor` owns its three initialized local tables as `0x38`
bytes of `.rodata` at ROM `0xA50`. Its uniform G8 object, declarations, and
full executable reproduce the retail SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.

## PAL language entry and later asset selection

`DebugMenu_UpdateLanguageEntry` (formerly native `func_80030808`) is 232
bytes at UK `0x80030808` and the other four PAL releases' `0x800309F4`; each
PAL symbols.txt binds the canonical name there. The existing editor owner
header declares it, with no assembler label, only under `VERSION_EUROPE`.
NA/Japan still have no such function.
The common source changes only the definition's identifier; the native
owner, its address, signature and every instruction remain unchanged.

First entry sets the entered bit, dims the primary display object, copies
the current language byte into the shared halfword editor value and opens
the existing one-row, one-digit language editor. It still writes three
to the separate row-one/two digit counts even though this entry selects
one row. A zero editor result keeps waiting. A negative result clears the
step byte and restores the primary display object without changing the
language.

A positive result compares the whole edited halfword with the current
language byte. Only when unequal does it store the value into the byte,
then compare that narrowed byte against five and substitute the build
language if needed. Preserve that comparison/store/test order, not a
halfword clamp before narrowing. The fallback remains zero in the UK
wrapper and one/two/three/four in the existing localized wrappers.
Confirmation does not clear the step, restore the object, request a boot
package or wait for any transfer. This is not the Options screen's
separate language-switch/loading lifecycle.

The later `File_RequestOptionsPackage` computes its start sector from the
then-current language byte and supplies `Options_LoadPackageStage`. The
stage provider is the former native `func_8003C56C`, not either of the two earlier
lookalike package callbacks. See the PAL addendum in
`options-screen.md` for the exact phase and binding scope.

Complete native conservation covers all seven actual editor wrappers,
the five actual frontend-package units and the initialized NA frontend
step-table owner: thirteen unique source/profile pairs. The earlier two
unclassified PAL callbacks are included in that comparison but are not
renamed or assigned an inferred screen. No table entries, bounds, layout,
format bytes, input policy, profile, wrapper language default or existing
test/control is changed, and no local PAL runtime observation is claimed.
