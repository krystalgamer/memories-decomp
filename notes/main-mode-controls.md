# Main-mode state and frontend-menu request

| Former identity | Name | Views |
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

## Single declaration per byte

Each byte has one name and one declaration. `gMain_bReturnMode` and
`gMain_bActiveModeState` are declared only in `main_mode_controls.h`;
`gMain_bRequestedMenuID` only in `main_menu_request.h`. The former
`D_8009B269`, `D_8009B26C` and `D_8009B26D` extern views in
`main_mode_state.h` and `main_services.h`, their `D_8009B269_AS_*` /
`D_8009B26C_AS_*` legacy selectors, the Japanese
`gJapanese_bActiveMainMode` alias, and every linker-script alias back to the
address names were removed. Regional `symbols.txt`, `link_symbols.ld`,
`c_symbols.ld` and overlay linker-symbol files bind the semantic names to
each region's numeric address directly; USA overlays bind them explicitly
because splat would otherwise auto-name the address.

The `MAIN_MODE_STATE_NEXT_*` and `MAIN_MODE_STATE_ACTIVE_*` selectors still
choose the addressing form a caller needs. `.data` takes precedence; the
default return view is scalar and the default active view is an incomplete
array. These are genuine gp-relative versus absolute addressing splits, not
separate objects.

## COMMON definitions are not initialized ownership

Seven tentative byte definitions occur in six current resident units.
The selected `gcc_2_8_1_g8_comm` and `gcc_2_8_1_g8_split_comm` profiles
emit actual COMMON symbols. They are code-generation inputs overridden
by fixed image bindings, not new allocations or initialized C objects.

The tentative definitions retain their scalar `u8` forms and named
profiles. Source grouping, object order and physical storage remain.
The one-byte COMMON symbol size follows the canonical definition in the
linked ELF.

All 21 regional roles retain independently mapped numeric addresses.
PAL ordering differs from NA, so the three bytes do not become a packed
record or imply a region-independent stride.

The Japanese `symbols.txt` binds `gMain_bActiveModeState` at the Japanese
address, `0x8009B15C`.

Removing the four `gMain_bActiveModeState` tentative definitions was
measured: without the COMMON symbol the assembler drops load-delay nops and
the USA image shrinks by 0x20 bytes, so they remain.
