# Data layout by translation unit

The retail executable was linked object by object: every section (`.rodata`,
`.text`, `.data`, `.sdata`, `.sbss`, `.bss`) holds the objects' contributions in
the same object order. A global therefore belongs to the translation unit whose
place in that order covers its address, and the compiler's `-G` threshold, not
a section attribute, decides whether it lands in small or ordinary data. This is
the model #2602 maps toward; `__attribute__((section(...)))` forcing is being
retired in its favour.

## Zero-initialized globals are tentative definitions

A global written without an initializer (`s32 gFile_CommandRetryCount;`) is a
tentative definition. GCC 2.8.1 emits it as `.comm`, in the order the names are
**first declared** in the translation unit, and MASPSX allocates it in `.sbss`
(at or under the profile's `-G`) or `.bss`, in that order. An explicit `= 0`
would move it to `.sdata`/`.data` instead, so zero-initialized storage is never
written with an initializer. To reproduce a unit's retail order, its tentative
definitions come first in declaration order, ahead of any header that declares
them.

MASPSX exports only the large ones, so `build_baseline.py` adds a `.globl` for
every `.comm` name after MASPSX (`export_common_symbols`); `.lcomm` (static)
names stay local. A unit that owns small data is compiled without MASPSX's
`--use-comm-section` (the `_comm` profiles), which would leave its commons for
the linker to allocate in hash order.

Names that are interior bytes of an owned object are linker aliases of it
(`gGraphics_bOrderingTable1Length = gGraphics_abOrderingTableLengths + 1;` in
`link_symbols.ld`), and `bss_start` is the start of the first owned block,
`text_SBSS_START`.

The regional executables lay these blocks out in their own orders and still
place them through their link files; an assignment there wins over the shared
definition, so a shared unit's block is inert in a region whose names are all
assigned. The Japanese wrappers alias the names Japan spells differently.

## One code segment

`generate_build_config.py` folds the template's `initialized_data` and BSS-image
segments into the generated text segment, so each object's data sections follow
its text in one Splat code segment. The remaining unowned zero-image ranges are
`bin` subsegments ordered as `.sbss` below the large-object boundary
(`LARGE_BSS_START`, file offset `0x8BCA8`, address `0x8009B4A8`) and `.bss`
above it. A unit that owns a range is listed at that address with its section
type, for example `[0x8b8e8, .sbss, game/file_transfer_runtime]`.

## Large zero-initialized data

Objects larger than the `-G` threshold land in `.bss`. Each compiled unit's
`.bss` is listed once, at its explicit `split.yaml` position, and
`build_baseline.py --finalize-linker-script` (run after Splat) drops the
earlier listings Splat adds for an object whose `.rodata` sits in another
segment. The text output section forces `SUBALIGN(2)`, which code and small
data need, but the game's linker started each object's `.bss` on an 8-byte
boundary, so the same step aligns every compiled unit's `.bss` placement to 8.
Inside an object, MASPSX leaves `.bss` labels unaligned; `build_baseline.py`
aligns each tentative definition there by size as it does in `.sbss`. A gap
of fewer than eight bytes before the next unit's block is that padding, not
an object.

Names inside one large object are `link_symbols.ld` aliases of it: the file
LBAs, the movie stream base and the master XA LBA are one eight-word table,
`gFile_anLba`.

The regional builds still place every name through their own link files, so
`build_regional_baseline.py` links through a copy of the generated script
without compiled units' `.bss`, which is then discarded like the unlisted
`.sbss`.
Several regional wrappers include the same shared source, so the regional
build also makes each compiled object's zero-initialized definitions weak;
the copies are discarded and the link file supplies the address.

A `.bss` range is written as its typed objects larger than eight bytes, with
the bytes between them as `u8` blocks: a named eight-byte-or-smaller object in
`.bss` cannot be its own object at `-G8`, so it is part of the block that holds
it, and its name is a `link_symbols.ld` alias into that block. An object whose
start is not on the 8-byte boundary its size requires is likewise part of the
object before it.

## Converted so far

| Unit | Section | Range |
|---|---|---|
| `src/game/graphics_frame.c` | `.sbss` | `0x8009B090`-`0x8009B0E0` |
| `src/game/file_set_position_table.c` | `.sbss` | `0x8009B0E0`-`0x8009B0E8` |
| `src/game/file_transfer_runtime.c` | `.sbss` | `0x8009B0E8`-`0x8009B140` |
| `src/game/fade_runtime.c` | `.sbss` | `0x8009B140`-`0x8009B150` |
| `src/game/duel_field_card_render.c` | `.sbss` | `0x8009B150`-`0x8009B244` |
| `src/game/duel_effect_dialog_state.c` | `.sbss` | `0x8009B244`-`0x8009B258` |
| `src/game/library_card_grid.c` | `.sbss` | `0x8009B258`-`0x8009B260` |
| `src/game/duel_effect_object_pool.c` | `.sbss` | `0x8009B260`-`0x8009B268` |
| `src/game/script_command_busy.c` | `.sbss` | `0x8009B27C`-`0x8009B280` |
| `src/game/script_op_load_image_scene.c` | `.sbss` | `0x8009B280`-`0x8009B2B0` |
| `src/game/debug_menu_editor_entries.c` | `.sbss` | `0x8009B2B0`-`0x8009B2F8` |
| `src/game/build_deck_draw_card_list_page.c` | `.sbss` | `0x8009B2F8`-`0x8009B300` |
| `src/game/hmd_fog_textured_drivers.c` | `.sbss` | `0x8009B300`-`0x8009B304` |
| `src/game/sorted_entry_relink.c` | `.sbss` | `0x8009B304`-`0x8009B318` |
| `src/game/options_screen.c` | `.sbss` | `0x8009B37C`-`0x8009B390` |
| `src/game/save_data_payload.c` | `.sbss` | `0x8009B3B8`-`0x8009B3BC` |
| `src/game/mem_card_dialog.c` | `.sbss` | `0x8009B3BC`-`0x8009B400` |
| `src/game/sound_frontend.c` | `.sbss` | `0x8009B400`-`0x8009B410` |
| `src/game/display_object_core.c` | `.sbss` | `0x8009B410`-`0x8009B428` |
| `src/game/main_run_boot_sequence.c` | `.sbss` | `0x8009B428`-`0x8009B430` |
| `src/game/mem_card_driver.c` | `.sbss` | `0x8009B430`-`0x8009B458` |
| `src/game/movie_frame_pipeline.c` | `.sbss` | `0x8009B498`-`0x8009B4A8` |
| `src/game/graphics_frame.c` | `.bss` | `0x8009B4A8`-`0x800E9DF0` |
| `src/game/file_set_position_table.c` | `.bss` | `0x800E9DF0`-`0x800E9E18` |
| `src/game/file_transfer_runtime.c` | `.bss` | `0x800E9E18`-`0x800E9EC8` |
| `src/game/fade_runtime.c` | `.bss` | `0x800E9EC8`-`0x800E9EF0` |
| `src/game/duel_field_card_render.c` | `.bss` | `0x800E9EF0`-`0x800EA1E8` |
| `src/game/library_create_grid_section_sprites.c` | `.bss` | `0x800EA1E8`-`0x800EAD88` |
| `src/game/duel_effect_object_pool.c` | `.bss` | `0x800EAD88`-`0x800EAE88` |
| `src/game/main_run_campaign.c` | `.bss` | `0x800EAE88`-`0x800EAED8` |
| `src/game/debug_menu_editor_entries.c` | `.bss` | `0x800EAED8`-`0x800EAF08` |
| `src/game/duel_effect_entry_occupancy.c` | `.bss` | `0x800EAF08`-`0x800EAFF8` |
| `src/game/duel_effect_command.c` | `.bss` | `0x800EAFF8`-`0x800EB010` |
| `src/game/text_control_commands.c` | `.bss` | `0x800EB010`-`0x800EB0F8` |
| `src/game/text_box_runtime.c` | `.bss` | `0x800EB0F8`-`0x800EB288` |
| `src/game/duel_effect_process_entries.c` | `.bss` | `0x800EB288`-`0x800EF668` |
| `src/game/input_pads.c` | `.bss` | `0x800EF668`-`0x800EF6D0` |
| `src/game/mem_card_dialog.c` | `.bss` | `0x800EF6D0`-`0x800EFE38` |
| `src/game/display_object_core.c` | `.bss` | `0x800EFE38`-`0x800F2888` |
| `src/game/mem_card_driver.c` | `.bss` | `0x800F2888`-`0x800F2B20` |
| `src/game/model_scene_controllers.c` | `.bss` | `0x800F2B20`-`0x800F3938` |
| `src/game/model_slot_control.c` | `.bss` | `0x800F3938`-`0x800F39B0` |
| `src/game/model_slot_support.c` | `.bss` | `0x800F39B0`-`0x800F3A10` |
| `src/game/model_distance_queries.c` | `.bss` | `0x800F3A10`-`0x800F5750` |
| `src/game/movie_stream_requests.c` | `.bss` | `0x800F5750`-`0x800F5768` |
| `src/game/model_keyframe_update.c` | `.bss` | `0x800F5768`-`0x800F5918` |
| `src/game/model_handler_registry.c` | `.bss` | `0x800F5918`-`0x800F5B98` |
| `src/game/ai_script_vm.c` | `.bss` | `0x800F5B98`-`0x800F5C88` |

Bytes inside a unit's range that no code names are written as `static`
address-named objects (`static u32 D_8009B448;`): they keep the retail
layout without exporting a name another region's data could collide with.
The duel block shows the rule for data shared by many split files: the
original duel unit spans many repository sources, so its zero-initialized block
belongs to the first of them in text order that references it,
`duel_field_card_render.c`. Its consumers declare the block through
`duel_field_card_render_data.h`. Struct pointers are spelled with their tags
(`struct DuelSideState *G32`) so the definitions can precede the headers that
complete the types, and names for elements inside an owned array are
`link_symbols.ld` aliases (`gDuel_pSide1SaveData = gDuel_pSide0SaveData + 4;`).

Alignment fixes a block's start. An object's `.sbss` is aligned to its widest
member, so a block whose pointers sit at retail offsets only when counted from
a lower 4-byte boundary started there: `duel_effect_dialog_state.c` owns
`0x8009B244`-`0x8009B258` as one block, and the debug-menu block starts at
`0x8009B2B0`, not at its first named byte.

Where a unit's headers declare its globals in a different order than retail,
the unit is not converted yet; the declaration order has to be reconstructed
first.
