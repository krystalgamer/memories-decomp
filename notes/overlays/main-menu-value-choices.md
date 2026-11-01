# Main-menu value-setup choices

This component names the choice state across the complete seven-function
`value_setup.c` lifecycle, not the edited life-point values themselves.
The real resident caller is `Main_RunTwoPlayerDuelSetup` in
`src/game/main_mode_runners.c`; the shared boundary remains unchanged.

## Choices and overlapping native views

`gMainMenu_abValueSetupChoices` (formerly `D_801845BC`) remains an unsized
`u8[]`. Bytes 0 and 1 are the respective players' cursor choices; byte 2
is their one shared toggle choice. They are not a current value and its
starting value. The actual values, displayed values and output pointers
remain in the two existing `ValueSetupEntry` records.

`gMainMenu_bValueSetupToggleChoice` (formerly `D_801845BE`) remains a
separate `u8` scalar. The linker symbols put the array at
`gMainMenu_ValueSetupState+0x1C` and the scalar at `+0x1E`. Consequently
array byte 2 and the scalar view overlap, but the compiler still sees the
same two distinct C identities as before. Neither access is rewritten into
an aggregate-member expression or the other symbol.

The real initialized owner in `module_state.c` remains the complete
`0x3C`-byte `MainMenuValueSetupState` at NA `0x801845A0`. Its three-byte
`selections` member, pad, three widget pointers, both value records, toggle
pointer and initializer are unchanged. No storage, array bound, pointer
type, Boolean type, const qualifier, wrapper or linker definition is added.
Japanese bindings retain the same addresses; the European bindings
resolve to `0x801847E8` and `0x801847EA`. Regional maps remain authoritative.

Three header constants name only the already-proven choice codes:
`MAIN_MENU_VALUE_SETUP_CHOICE_LEFT` is 0, `..._RIGHT` is 1 and
`..._VALUE` is 2. The first two name physical positions, not unproved option
captions or card-visibility semantics. Numeric 2 used as the shared byte's
array index is not replaced by the unrelated numeric-row choice constant.
Unrelated numeric rolling thresholds and player indices are also unchanged.

## Complete input, draw and tween handoff

Start saves the caller's toggle pointer, sets shared byte 2 to
`(*toggle == 0)`, initializes both players to their value row and captures
both output pointers, target values and displayed values. The existing
right-before-left chained initialization store order is retained.

Update first handles displayed-value rolling and each cursor's active
callback. Only when both sides are idle can cancel return -1 or Start
return 1; cancel retains priority. The Japanese Cross versus ordinary
cancel-button distinction is unchanged.

An idle player whose choice is below 2 is on the shared toggle row.
Horizontal input stores 0 for Left or 1 otherwise, then updates that
player's choice and the other player's choice only if the latter is also
below 2. The other player's busy flag is not additionally checked.
Side 0 still runs before side 1, so two eligible edits can leave side 1's
choice last. Down starts the existing tween toward choice 2. On the value
row, horizontal input edits that player's target life points; Up starts a
tween back to the current shared choice. Input priority, step/clamp rules,
global reloads and all return paths are unchanged.

Draw places the shared marker at X 116 for choice 0 and X 220 otherwise,
at Y 74. A player in choice 2 uses the value-scaled bar position at
Y 111 or 139; the existing callback/renderable branches and coordinate
copying remain exact. This does not infer artwork or label text.

Tween start saves the player index in object byte `field_6B`, the requested
choice in `field_6C`, the ten-update countdown, callback and original
coordinates. Tween update follows the shared marker unless its target is
choice 2, in which case it follows the appropriate displayed-value bar.
After decrementing the countdown, completion writes the target into that
player's choice byte and clears the callback. Existing interpolation,
integer division, ordering and full-width-to-byte stores are untouched;
no real-time duration is inferred.

## Finish also commits on cancel

Finish writes both target values through their saved output pointers and
writes `(gMainMenu_bValueSetupToggleChoice != 1)` through the saved toggle
pointer. Thus 0 writes 1, 1 writes 0, and other byte values still write 1:
the comparison is not replaced by logical negation or Boolean storage.
The resident passes a byte view of the wider card-view mode; its type and
cast are unchanged.

The actual resident caller invokes Finish on either nonzero update result,
before distinguishing accept from cancel. Cancellation therefore does
**not** roll back edits. Releases, handle clears and callback-slot clearing
retain their original order, without adding null checks or ownership tests.

## Conservation and evidence scope

The NA complete unit contains seven functions and 3,984 instruction bytes
at `0x80180FD8..0x80181F68`. The ordinary named
`gcc_2_8_1_g0_split` profile remains unchanged. Production changes are the
two native-bound choice names, literal-equivalent choice constants and
directly related comments/documentation; no control flow, access shape,
callback, arithmetic, signature, section, profile or inventory changes.

Fresh native baselines and candidates cover all 15 actual source/profile
pairs used by the seven regional main-menu manifests: the complete NA unit
and seven existing Japanese plus seven existing shared-European include
wrappers. Compiler output, MASPSX-normalized assembly, symbol tables and
disassembly/relocations compare exactly. The Japanese region covers seven
functions / 3,984 instruction bytes; each European manifest covers seven /
4,208. This is actual-source native conservation, not a claim of executing
complete Japanese or European image builds.

The clean full NA resident and rebuilt full NA main-menu executable retain
their retail hashes:

- Resident: `84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`
- Main menu: `34e9421eb10dc3ff97f8810e4f595045d4847b2b54760e9895eb83266008bc97`

The actual compiled aggregate's data section and final section-defined
owner, rather than an ABS replacement, establish the storage boundary.
All native maps, manifests, wrappers, aggregate definitions and existing
controls remain unchanged. No new test, generated-C/source-string harness,
manual CI rerun or local cloud test is introduced.
