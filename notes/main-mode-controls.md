# Main-mode state and frontend-menu request

| Former identity | Canonical name | Preserved views |
| --- | --- | --- |
| `D_8009B269` | `gMain_bReturnMode` | `u8` scalar, incomplete array, `.data` scalar |
| `D_8009B26C` | `gMain_bActiveModeState` | `u8` scalar, incomplete array, `.data` scalar |
| `D_8009B26D` | `gMain_bRequestedMenuID` | `u8` scalar, `.data` scalar |

## Dispatch and lifecycle occupy the whole active byte

`Main_Loop` snapshots the active byte after advancing the frame. If
`0x80` is clear, it sets that bit and resets the frontend runtime.
Otherwise it dispatches through `gMain_apfnModeRunner[v & 0x1F]`,
then observes the live state's `0x40` bit for the existing fade-out gate.

Mode runners keep their `0x40` initialization/lifetime behavior.
Animated battle separately sets `0x20`. These flags are not removed from
the stored byte, converted to a separate Boolean, or packed into a new
structure. The `0x1F` dispatch mask is not proof of 32 allocated handlers,
and no validation or fallback is added.

Whole-byte transitions retain their flag-clearing effect. Per-frame
callback order, the snapshot versus later live read, mode-specific
fallthrough and fades remain unchanged.

## Return target is separate from active state

Options, trade, name entry, build deck, animated battle and library paths
copy the return byte back into the active state at their existing exits.
Startup and menu handoffs seed mode 8; duel handoffs use mode 3.
Script and debug paths retain their other literal mode values.

This is the existing next/base return target, not a guarantee that every
path executes that mode. Game over retains its subsequent conditional
override and recovery jump. No new reset, normalization or state-machine
policy is introduced.

## Requested menu is not the active normalized index

`Main_RunMenu` passes the raw requested byte to
`MainMenu_InitFrontendMenu`. The tracked overlay computes the separate
`gMain_bMenuID` as `menu % 11` and uses that active index to configure
the eleven entries. That normalization stays solely in the initializer.

`Main_ApplyMenuSelection`, the save/return-to-menu script commands,
the title debug editor and game-over recovery preserve their original
stores and narrowing. The requested byte does not gain an enum, range
check or modulo operation.

The neighboring `D_8009B268` stays unclassified. Its corresponding
constructor argument is unused; adjacent 0/1 stores do not establish a
new flag meaning.

## Declaration ownership and first-include selection

The original `main_mode_state.h` and `main_services.h` retain every
legacy declaration, guard, selector, default and precedence. Each adds
one forwarding include after its old declarations. Canonical mode
declarations live in `main_mode_controls.h`; the requested-menu declaration
lives in `main_menu_request.h`.

Forwarding at the old owner preserves its first selected view and the
existing direct-owner/`unmatched.h` include orders. It avoids selecting a
different type by rereading flags later in a C source. `unmatched.h`
itself is unchanged, and old overlays retain their imports and aliases.

The two mode bytes remain independently selectable. Their existing
`MAIN_MODE_STATE_NEXT_*` and `MAIN_MODE_STATE_ACTIVE_*` controls and all
legacy address-based selectors keep their names. `.data` retains
precedence; the default return view is scalar and active view is an
incomplete array. Selecting only the return byte's `.data` view does not
make the active byte scalar.

Unsigned-character locals, incomplete bounds and expression forms remain.
No array is tightened to `[1]`, scalarized for style or made volatile.

## COMMON definitions are not initialized ownership

Seven tentative byte definitions occur in six current resident units.
The selected `gcc_2_8_1_g8_comm` and `gcc_2_8_1_g8_split_comm` profiles
emit actual COMMON symbols. They are code-generation inputs overridden
by fixed image bindings, not new allocations or initialized C objects.

The tentative definitions retain their scalar `u8` forms and named
profiles. Canonical native linker aliases point to the original external
providers; the initialized-string old-to-canonical direction does not
apply here. Source grouping, object order and physical storage remain.
The one-byte COMMON symbol size follows the canonical definition in the
linked ELF. Legacy aliases retain their address and absolute class, not
an additional independently sized allocation.

All 21 regional roles retain independently mapped numeric addresses.
PAL ordering differs from NA, so the three bytes do not become a packed
record or imply a region-independent stride.

The Japanese viewer keeps an unused legacy scalar `.data` declaration
whose consumer is compiled out. The Japanese `symbols.txt` binds the
shared `D_8009B26C` name at the Japanese address, `0x8009B15C`.
NA and Japanese debug-leave code likewise retain unused return-byte
declarations under their existing regional conditions.

## Existing controls and exact conservation

The main-mode declaration control retains all 36 consumer paths, selector
matrices, include orders, expected views, seven COMMON inputs and the
wrong-scalar-default negative case. Only variable identifiers and
canonical-owner paths migrate. Its two legacy-selector prefix filters
keep the literal address-based prefixes because those guard names have
not changed.

The campaign-command witness changes only its active-mode dictionary key.
Its offset `0x00C`, adjacent offsets, guarded bytes, cases, fixture, linker
generation and mutations remain unchanged.

Actual compiler-expanded comparison covers 85 source/profile contexts,
132 used-role declaration views and 18 tentative-definition views,
including all three retained declarations without compiled consumers.
All complete ordinary, compiler/MASPSX and fallback outputs are compared
with only scoped original identities reversed, without instruction,
directive, relocation, order or filename filtering.

No synthetic/generated-C or source-string harness, fixture, probe or
testcase is added. Existing cloud/regional/overlay/witness suites are
not executed locally. Fresh full-image regional PR gates remain required.
