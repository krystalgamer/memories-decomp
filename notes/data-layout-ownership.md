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
names stay local.

## One code segment

`generate_build_config.py` folds the template's `initialized_data` and BSS-image
segments into the generated text segment, so each object's data sections follow
its text in one Splat code segment. The remaining unowned zero-image ranges are
`bin` subsegments ordered as `.sbss` below the large-object boundary
(`LARGE_BSS_START`, file offset `0x8BCA8`, address `0x8009B4A8`) and `.bss`
above it. A unit that owns a range is listed at that address with its section
type, for example `[0x8b8e8, .sbss, game/file_transfer_runtime]`.

## Converted so far

| Unit | Section | Range |
|---|---|---|
| `src/game/file_transfer_runtime.c` | `.sbss` | `0x8009B0E8`-`0x8009B140` |
| `src/game/file_set_position_table.c` | `.sbss` | `0x8009B0E0`-`0x8009B0E8` |
| `src/game/duel_effect_dialog_state.c` | `.sbss` | `0x8009B244`-`0x8009B246` |
| `src/game/library_card_grid.c` | `.sbss` | `0x8009B258`-`0x8009B260` |
| `src/game/duel_effect_object_pool.c` | `.sbss` | `0x8009B260`-`0x8009B268` |
| `src/game/script_command_busy.c` | `.sbss` | `0x8009B27C`-`0x8009B280` |
| `src/game/hmd_fog_textured_drivers.c` | `.sbss` | `0x8009B300`-`0x8009B304` |
| `src/game/sorted_entry_relink.c` | `.sbss` | `0x8009B304`-`0x8009B318` |
| `src/game/options_screen.c` | `.sbss` | `0x8009B37C`-`0x8009B390` |
| `src/game/save_data_payload.c` | `.sbss` | `0x8009B3B8`-`0x8009B3BC` |
| `src/game/mem_card_dialog.c` | `.sbss` | `0x8009B3BC`-`0x8009B400` |
| `src/game/display_object_core.c` | `.sbss` | `0x8009B410`-`0x8009B428` |
| `src/game/main_run_boot_sequence.c` | `.sbss` | `0x8009B428`-`0x8009B430` |
| `src/game/mem_card_driver.c` | `.sbss` | `0x8009B430`-`0x8009B458` |
| `src/game/movie_frame_pipeline.c` | `.sbss` | `0x8009B498`-`0x8009B4A8` |

Bytes inside a unit's range that no code names are written as `static`
address-named objects (`static u32 D_8009B448;`): they keep the retail
layout without exporting a name another region's data could collide with.
Where a unit's headers declare its globals in a different order than retail,
the unit is not converted yet; the declaration order has to be reconstructed
first.
