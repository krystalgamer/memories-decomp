# Main-menu prompt and display-resource ownership

This component binds two proven ownership roles across the complete
frontend and value-setup consumers, without identifying unknown artwork.
It starts from accepted request-handoff master
`055429fd98e74e9852b2a65205e52f7d142b7373`.

| Former label | Canonical C name | Unchanged storage |
|---|---|---|
| `D_80184560` | `gMainMenu_pStartPrompt` | Four-byte stored `DisplayObject *G32` at `gMainMenu_FrontendState + 8` |
| `D_801AF800` | `gMainMenu_DisplayResources` | Existing unsized `u8[]` view of the separate `0x800`-byte SU phase |

## Start-input lifecycle, not a texture identification

`MainMenu_InitFrontendMenu` allocates the third singleton through the real
display-object pool, configures it from the SU resource bank, selects OT1
and writes its existing shade-step, wait and lifetime-accumulator fields.
It hides this singleton for a nonzero initial menu and leaves the original
nonzero-selection dereference unguarded. The name is not a new allocation
or null-safety guarantee.

`MainMenu_UpdateFrontendMenu` restores it after a negative fade, pulses its
three shade channels while it is renderable, and tests `PAD_BUTTON_START`
inside that visible branch. Start hides it, starts the entry transition
and raises the fade direction. Its separate accumulator can still return
-2 at the original `0xBB8` threshold. No time unit, sound-ID meaning or
new menu behavior is inferred. `MainMenu_DestroyFrontendMenu` releases
and clears the same stored handle in the same position among the three
singleton releases.

This proves a Start-input prompt role. It does not prove the printed
wording, image pixels or original Konami symbol. The first two row-241
singletons stay `D_80184558` and `D_8018455C`: their visual identities are
not established. Existing aggregate member names are not additional
visual evidence and the complete aggregate declaration/initializer remains
untouched.

## The resource bank is not executable padding

The resident `MainMenu_LoadPackageStage` loads SU sectors `98..113` as
the `0x8000`-byte executable at `0x80180000`, then sectors `114..114`
as a separate `0x800`-byte phase at `0x801AF800`. The phase hash is
`417734e33126c1071d4b990dfae0bb34162e6c0aefd9151c84a9448cf38abf27`.
The executable hash is independently
`34e9421eb10dc3ff97f8810e4f595045d4847b2b54760e9895eb83266008bc97`.

The native fixed-address binding in `c_symbols.ld` is unchanged.
`DisplayObject_ConfigureSpriteAtPositionWithResource` stores this argument
in the object's existing resource field, then delegates configuration.
The frontend uses it for all three singleton sprites and all eleven
entries; afterimages reuse it while preserving the entry's existing
variant; value setup supplies it to its separate sprite. These are the
complete direct main-menu C consumers. No resource-record format, image
identity or claim about other SU executable phases follows from that role.

The local `display_resources.h` retains the original unsized byte array
type; every main-menu linker symbol file binds the name directly at its
address in place of the former `D_801AF800` label. It deliberately adds no
array bound, `const`, storage definition, copied bytes or resident alias.
`frontend.h` declares the canonical prompt pointer with the same pointee
and `G32`; the main-menu symbol files bind it where `D_80184560` stood.

## Complete-source conservation boundary

Four complete real C units cover fourteen functions and 8,012 instruction
bytes: `frontend.c`, `frontend_update.c`, `frontend_background.c` and
`value_setup.c`. Their only edits are the two identifier substitutions,
plus the resource header include in value setup. Whole accepted sources
reconstruct those exact candidates; every original expression, cast,
global reload, allocation check, call, branch, scalar write and release
order is preserved.

Actual GCC2.8.1/MASPSX2.81 compiler/normalized assembly, native symbols and
disassembly/relocations are compared under the existing named
`gcc_2_8_1_g0_split` profile. Source profiles, regional include wrappers,
inventories, native maps, the complete loaded module-state owner and all
retail bytes remain unchanged. The scoped C bindings are inherited by
regional wrappers, without claiming local Japanese/PAL full-image execution.

No test or fixture is added. The complete existing frontend-update control
changes only its four prompt identifier occurrences; all model types,
expected values, mutations, scenarios and compiler setup remain exact.
No synthetic/source-string harness is added, and cloud tests are left to
ordinary PR CI. The entire accepted semantic registry is preserved before
two ownership rows.
All accepted duel lifecycle/data/header/guard work, sound/MODEL groupings,
Exodia scratch, native bolt/utility/history/regression and focused
null-offset probe/replay remain outside this component.
