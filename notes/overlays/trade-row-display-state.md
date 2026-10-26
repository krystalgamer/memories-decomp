# Trade inventory-row and stats-display state

This component names two connected state roles across the complete Trade
initialization, input, inventory refresh, row rebuild and release consumers.
It does not assign artwork or merge the independently recovered types of
the shared staging buffer.

## Stats-mode display

`gTrade_pStatsModeDisplay` is the original `MainMenuWidget *G32` view of
native `D_801845E0`. Init allocates a kind-2 display object, configures its
sprite with resource variant 4, sets the existing texture/screen flags and
depth, and stores the handle. Its `MainMenuWidget.frame` byte at `+0x69`
is the resource-variant byte, not evidence of an animation timeline:
`DisplayObject_ConfigureSpriteAtPosition` delegates to the configurator
that stores it, and `DisplayObject_SetResourceVariant` writes the same byte.

Trade update reads `frame - 4` into the existing full-width flag locals.
Player 0's shoulder-button branches clear/set bit 0; player 1's branches
clear/set bit 1. If changed, it calls the original variant setter with
`flags + 4`, before rebuilding dirty rows. The setter also clears the
original flag bit `0x10`; this is not replaced by a direct byte store.

`MainMenu_RebuildTradeInventoryRows` independently reads the same variant
minus four and passes `flags & (1 << side)` plus the full flags word to
native `func_80060E70`, already named
`CardList_BuildRowSetTextBoxes` at its resident definition. Side 1 can pass
2 rather than normalized 1. The provider publishes that raw flag to the
row set's `enabled` halfword and object byte `field_68`, selects the
existing text style, and retains its ignored fourth argument.
`CardList_RenderDeckBoxStats` uses `enabled` to gate the per-card ATK/DEF
digits. This establishes the stats-mode role without identifying captions
or pixels of the display resource.

Release passes the same handle to `DisplayObject_ReleaseIfPresent` and
clears it, after the still-native `D_801845DC` and before clearing callback
slot 1. The other display's visual identity remains unassigned. Existing
allocation checks, unchecked later dereferences, input/dirty ordering,
full-width subtraction and setter semantics are unchanged.

The real initialized `gMainMenu_TradeState` in `module_state.c` owns this
four-byte pointer at `+4`, NA `0x801845E0`; its full `0x16F8`-byte
definition and initializer remain intact. Native Japanese addressing
retains the shared map; European `D_801845E0` is `0x8018480C`.
The canonical declaration uses an explicit extern/native asm label and
preserves the original private `MainMenuWidget` type, `G32`, casts and
native compatibility declaration. No new storage, override or
DisplayObject retyping is introduced.

## Independent Trade row-set view

`gTrade_aInventoryRowSets` retains native `D_801A8000` and the original
unsized `MainMenuState[]` declaration, available only under
`MAIN_MENU_TRADE_STATE_BUFFER`. That private record still has a leading
`u8 *G32 object` and the original five padding words, or six in Europe:
strides remain 24/28 bytes. Neither the pads nor the pointer are retyped
as the resident's independently recovered `CardListRowSet`.

`CardList_CreateDeckBoxStatRows`, called once during Trade initialization,
initializes the two logical row sets and their display objects. It assigns
each object's side byte `field_6A`, clears bytes `field_68/69` and attaches
`CardList_RenderDeckBoxStats`. The refresh later writes
`gTrade_abSortMode[slot]` into that row object's byte `+0x69` through the
original `object[0x69]` byte access. The resident renderer uses that byte
to brighten the selected sort icon. These sort choices are not the
stats-mode bits held in the separate display's variant.

All refresh behavior remains exact: forced or mode-zero reload, zero-ID
holes, both sides' offer-deduction loop, optional 722-record qsort and
unvalidated comparator indexing, followed by the sort-icon byte write.
No exclusive two-record allocation, new bound, bool, extent or ownership
claim follows from the two logical users.

This is a Trade-only typed view of phase-dependent shared RAM, not a
project-wide rename. The resident `gCardList_aRowSets` view, script
bytecode, model payload/DMA, raw-word and other overlay interpretations
retain their own declarations and aliases. NA/Europe staging remains
`0x801A8000`; Japan maps the native label to `0x801B8000`. It is external
staging, not a new C-owned array in the main-menu image. Original maps,
loader addresses, resident text-provider aliases and overlay import
counts are untouched.

## Complete-source and image conservation

The complete actual C units are `trade_update.c` (which now also holds the
former `trade_inventory.c`), `trade_offers.c` and `trade_screen_helpers.c`: eleven NA functions and
7,680 instruction bytes, under the unchanged named
`gcc_2_8_1_g0_split` profile. Their only changes are the two native-bound
identifiers, including the directly related helper comment.
The scoped header adds the two canonical externs while preserving every
existing type, conditional declaration and native compatibility spelling.
All existing tests and controls remain unchanged; no new harness or
synthetic/generated C is added.

Actual native conservation covers all 26 source/profile units used by
the seven regional manifests: four NA grouped units, eleven existing
Japanese wrappers and eleven shared-European wrappers. Fresh untouched
baselines and candidates compare complete compiler output, MASPSX assembly,
symbols and disassembly/relocations. NA/Japan cover eleven functions /
7,680 bytes; each European manifest covers eleven / 7,892. This is not a
claim of running full Japanese or European images or cloud tests locally.

On the actual component tree, the clean full NA resident and genuinely
rebuilt complete NA main-menu image retain their original retail hashes:

- Resident: `84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`
- Main menu: `34e9421eb10dc3ff97f8810e4f595045d4847b2b54760e9895eb83266008bc97`

The actual compiled Trade aggregate's data section and final section-defined
owner establish pointer ownership; the original external staging alias is
verified separately. Neither is inferred from a substitute allocation.
