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

Interior bytes of an owned object are reached through the object itself
(`gGraphics_abOrderingTableLengths[1]`), not through separate linker names,
and `bss_start` is the start of the first owned block,
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
| `src/game/main.c` | `.sbss` | `0x8009B090`-`0x8009B0E0` |
| `src/game/file_set_position_table.c` | `.sbss` | `0x8009B0E0`-`0x8009B0E8` |
| `src/game/file_transfer_runtime.c` | `.sbss` | `0x8009B0E8`-`0x8009B140` |
| `src/game/fade_runtime.c` | `.sbss` | `0x8009B140`-`0x8009B150` |
| `src/game/duel_field_card_render.c` | `.sbss` | `0x8009B150`-`0x8009B244` |
| `src/game/duel_effect_dialog_state.c` | `.sbss` | `0x8009B244`-`0x8009B258` |
| `src/game/library.c` | `.sbss` | `0x8009B258`-`0x8009B260` |
| `src/game/duel_effect_object_pool.c` | `.sbss` | `0x8009B260`-`0x8009B268` |
| `src/game/script_image_scene.c` | `.sbss` | `0x8009B270`-`0x8009B2B0` |
| `src/game/debug_menu_editor_entries.c` | `.sbss` | `0x8009B2B0`-`0x8009B2F8` |
| `src/game/build_deck_draw_card_list_page.c` | `.sbss` | `0x8009B2F8`-`0x8009B300` |
| `src/game/sorted_entry_render.c` | `.sbss` | `0x8009B300`-`0x8009B318` |
| `src/game/options_screen.c` | `.sbss` | `0x8009B37C`-`0x8009B390` |
| `src/game/input_pads.c` | `.sbss` | `0x8009B390`-`0x8009B3B8` |
| `src/game/mem_card.c` | `.sbss` | `0x8009B3B8`-`0x8009B400` |
| `src/game/sound_frontend.c` | `.sbss` | `0x8009B400`-`0x8009B410` |
| `src/game/display_object_system.c` | `.sbss` | `0x8009B410`-`0x8009B428` |
| `src/game/main_boot_frontend.c` | `.sbss` | `0x8009B428`-`0x8009B430` |
| `src/game/mem_card_driver.c` | `.sbss` | `0x8009B430`-`0x8009B458` |
| `src/game/sound_driver.c` | `.sbss` | `0x8009B458`-`0x8009B468` |
| `src/game/model.c` | `.sbss` | `0x8009B468`-`0x8009B498` |
| `src/game/movie_pipeline.c` | `.sbss` | `0x8009B498`-`0x8009B4A8` |
| `src/game/main.c` | `.bss` | `0x8009B4A8`-`0x800E9DF0` |
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
| `src/game/mem_card.c` | `.bss` | `0x800EF6D0`-`0x800EFE38` |
| `src/game/display_object_system.c` | `.bss` | `0x800EFE38`-`0x800F2888` |
| `src/game/mem_card_driver.c` | `.bss` | `0x800F2888`-`0x800F2B20` |
| `src/game/model.c` | `.bss` | `0x800F2B20`-`0x800F56A0` |
| `src/game/model_view_queries.c` | `.bss` | `0x800F56A0`-`0x800F5750` |
| `src/game/movie_pipeline.c` | `.bss` | `0x800F5750`-`0x800F5768` |
| `src/game/model_keyframe_update.c` | `.bss` | `0x800F5768`-`0x800F5918` |
| `src/game/model_handler_registry.c` | `.bss` | `0x800F5918`-`0x800F5B98` |
| `src/game/ai_script.c` | `.bss` | `0x800F5B98`-`0x800F5CBC` |

Bytes inside a unit's range that no code names are written as `static`
address-named objects (`static u32 D_8009B448;`): they keep the retail
layout without exporting a name another region's data could collide with.
The duel block shows the rule for data shared by many split files: the
original duel unit spans many repository sources, so its zero-initialized block
belongs to the first of them in text order that references it,
`duel_field_card_render.c`. Its consumers declare the block through
`duel_field_card_render_data.h`. Struct pointers are spelled with their tags
(`struct DuelSideState *`) so the definitions can precede the headers that
complete the types. Adjacent words that code reaches by name are separate
definitions (`gDuel_pSide0SaveData` and `gDuel_pSide1SaveData`), never a
linker alias into a neighbour.

Alignment fixes a block's start. An object's `.sbss` is aligned to its widest
member, so a block whose pointers sit at retail offsets only when counted from
a lower 4-byte boundary started there: `duel_effect_dialog_state.c` owns
`0x8009B244`-`0x8009B258` as one block, and the debug-menu block starts at
`0x8009B2B0`, not at its first named byte.

Where a unit's headers declare its globals in a different order than retail,
the unit is not converted yet; the declaration order has to be reconstructed
first.

Two small ranges remain raw: `0x8009B268`-`0x8009B27C` and
`0x8009B318`-`0x8009B37C`. Every unit between their neighbours that names
them reaches them through absolute or oversized views (`gCampaignSceneIndex`,
`gDialog_bChoice[9]`, `gDuel_bOpponentID[9]`), which an owner compiled at `-G8`
would not use for its own small data, so their owners are units that do not
name them and need further evidence. A `-G0` unit cannot own small data at all;
`sound_output_state.c` is one, which is why the sound block belongs to
`sound_driver.c` (formerly `sound_output_transition.c`).

## Initialized data

The game's initialized `.data` (`0x800906E0`-`0x80091958`) follows the same
rule: each table is defined, with its initializer, by the text unit that reads
it, after that unit's includes, and `split.yaml` lists the unit's `.data` at
its address. `generate_build_config.py` lets a text unit own `.data` inside
the merged data layout, and `--finalize-linker-script` keeps that placement.
Owners keep text order. A `-G8` owner cannot hold an initialized object of
eight bytes or less in `.data`, because that compiler puts it in `.sdata`.

Data-only units remain where no owner is settled yet: `duel_field_layout.c`
(its grid selector changes `duel_card_and_status_draw.c`'s code, and no other
reader falls in its window), `duel_effect_command_table.c` (no reader falls
between its neighbours in text order), and `model_image_copy_frames.c` and
`data_80091510.c` (an eight-byte object only a `-G0` owner could hold).
`duel_screen_tables.c` brought its typed-positions selector into
`duel_ground_projection.c`, which builds the same with it. The rest of the
region from `0x80091958` is Psy-Q data.

Regional builds discard compiled text units' `.data` as they do `.bss`, and
make those definitions weak, since each region supplies the same names from
its own data image or link file.

Small initialized data (`.sdata`) works the same way. An owner compiled at
`-G8` places an initialized object of eight bytes or less in `.sdata` without
any attribute, so folded tables drop their `section(".sdata")` forcing, and a
table is defined after the owner's existing top-level definitions, keeping
their order. Units stay data-only where the owner does not fit yet:
- `options_layout_position_data.c`, `data_8009af6c.c` and
  `model_graphics_state.c`: their readers reach them through absolute or
  forced views;
- `model_handler_diagnostics.c`: the European builds place
  `model_debug_controller.c`'s own `.sdata`;
- `ai_script_source_line_format.c`: its only reader is a `-G0` unit, which
  cannot hold small data.

## Forced `.data` views

A forced `section(".data")` extern view is what still makes a `-G8` unit reach
some small objects absolutely while it reaches others gp-relative. The
mechanism is visible in the compiler output. GCC 2.8.1 writes such a load
symbolically (`lb $2,x`) and leaves it to the assembler, but omits the
`.extern x, size` line it writes for a plain small extern, so MASPSX, having
no size, expands the load absolutely. With the size line it would use `$gp`.

Every attribute-free declaration tried so far changes the code instead:
- a plain, `volatile`, `const`, weak, aligned, asm-labelled or block-scope
  extern keeps the `.extern` line, so the reference becomes gp-relative;
- an incomplete array (`extern T x[]`), an incomplete or oversized struct, or
  a field of a larger record makes GCC split and share the address itself
  (`lui %hi`/`%lo`).

Twenty views convert byte-identically to incomplete arrays and have. The rest
need either the original declarations or a toolchain explanation for a
symbolic small-data load without size information, such as a difference in how
the original compiler emitted `.extern`. That is the open question for the
remaining views and for the two small ranges whose readers depend on them.

The pattern points at translation-unit boundaries. Retail reaches a small
variable gp-relative only from the translation unit that defines it, and
absolutely from every unit that declares it `extern`. The current units
split those original translation units apart. Measured over every North
American object, for each unit that owns small data, take the span of text
from the owner to its furthest gp-relative reader:
- 21 of the 23 owners have no absolute reader inside that span. Every unit
  in the span reaches the owner's variables gp-relative, and the absolute
  (view) readers lie outside it.
- The two exceptions are initialized `.sdata` strings (`gDebugMenu_szLineBreak`,
  `gModel_szWhyTrace`), char arrays that are reachable through incomplete-array
  externs.

The spans outline the original units:
- the duel code from `duel_field_card_render`'s block through about 55 current
  units, with `duel_ground_projection` inside it;
- the script commands (`script_command_busy` and `script_op_load_image_scene`);
- the debug menu (`debug_menu` and `debug_menu_editor_entries`);
- the model controllers (`model_background`, `model_debug_controller` and
  `model_scene_controllers`);
- smaller groups, such as `graphics_frame` and its first three readers.

Removing the views therefore starts with merging the current units back into
those translation units, so that each gp-relative reader defines what it
reads.

## Regional layouts

The regional executables use the same units in the same order, so a region
can take ownership range by range in its own `split.yaml`. Japan's
zero-initialized data is the North American layout moved down by `0x110` in
`.sbss` and `0x120` in `.bss`. Its split therefore folds `initialized_data`
and `bss_image` into `main` and lists the same owners at the shifted addresses:
- `.sbss`: every owned block from `graphics_frame` (`0x8009AF80`) through the
  sound block (`0x8009B348`-`0x8009B358`);
- `.bss`: from the frame buffers (`0x8009B388`) through `text_control_commands`.

Where a Japanese block differs from the North American one, the Japanese
wrapper gives that build its own storage. The wrapper defines
`<UNIT>_REGIONAL_STORAGE`, which the shared source checks around its North
American block (declaring the same names `extern` instead), and defines the
region's storage before including the shared source:
- a large block is declared as its typed variables in that build's address
  order (`<UNIT>_STORAGE_ORDER`), or the shared definition takes the
  build's record type and count (`DUEL_EFFECT_ENTRY_STORAGE_TYPE`);
- a small block is one scalar per slot (`D_8009B368_slpm_86398`).

Japan's text channels (four 0x60-byte records), duel-effect entries, input
repeat timers and model small block use this; every other Japanese zero-
initialized block is the shared one at the shifted address, so all of Japan's
game zero-initialized data now comes from units. Japan keeps the North
American initialized tables 0x150 bytes lower, with relocated pointers and a
few regional values, from `duel_field_layout` through `mem_card_dialog`. Its
twenty-four owners include the data-only `duel_field_layout`, which the
regional builder compiles from `data_c.json` like any data-only unit the
region's split places. Two tables take a Japanese arm in the shared source:
- the duel-effect entry ranges (a 300-entry pool split 55/100/100/45);
- the memory-card step table's fourth entry,
  `MemCardDialog_StepAlternateSave`.

From the display-object stream tables onward (`0x80090E9C`), Japan keeps the
North American tables but in its own order:
- the stream command table (Japan's wrapper defines its own copy, #8729);
- `movie_stream_requests`;
- `model_effect_state`;
- `model_background_records`;
- `model_image_copy_frames`;
- `data_80091510`.

#8729 places these. Japan's game `.data` is now fully owned up to
`ai_script`'s hand-search records.

Europe's initialized tables start `0x1004` above the North American ones and
drift as European tables grow. Its split places seventeen owners:
- from `file_set_position_table` through `campaign_scene` (`+0x1004`);
- `debug_menu_editor_entries` through `build_deck_draw_card_list_page`
  (`+0x1020`);
- `model_subdivided_effect` and `ai_script`
  (`+0x100C`).

Two of them take a European arm in the shared source:
- the debug menu's alternate page adds `DebugMenu_EnterCredits` and
  `DebugMenu_UpdateLanguageEntry`;
- the burst image records use 0x280 in the field that is 0x200 in the
  North American build.

The middle range (`0x80091E20`-`0x800925F4`) groups the same tables
differently from the North American build, but ten of its units now own
their tables there:
- `build_deck_input`: its European table ends with the divisor ladder
  (`BUILD_DECK_DIVISOR_LADDER_ONLY`).
- `duel_effect_update_object_layout`: an 800-entry pool, split
  280/220/220/80.
- `text_box_build_step`.
- `european/duel_effect_command_table`, a data-only unit compiled through
  `config/sles_03947/data_c.json`: two more secondary text commands and six
  more glyph-fade entry handlers.
- `display_effect_process_menu_records`, `mem_card_dialog` and
  `display_object_runtime`.
- `model_effect_state`.
- `european/movie_stream_requests`: six movies share one stream range.
- `model_image_copy_frames`.

The 0x1C bytes after `campaign_scene` are the European debug menu's
`"LANGUAGE = %X"` editor format, `gDebugMenu_szLanguageEditorFormat`.
`debug_menu_editor_entries` defines it under `VERSION_EUROPE` ahead of its
other formats, and owns its data from `0x80091CB8` (and `0x800920CC` in the
language builds).

The display-object stream command table is now owned in Europe and the
language builds. The background records and shadow fan steps that follow it
in the North American image are their own data-only unit,
`model_background_records.c`, because the European builds place other
units' tables between the two. In every PAL build, byte 0xAE of records 1
and 3 is 0x80 (`MODEL_BACKGROUND_RECORD_PAL_FLAG`). The five PAL builds
compile the unit through `european/model_background_records.c` and place
it at `0x800920EC` (Europe) and `0x800924CC` (language builds).

The four language builds keep Europe's middle range `0x414` higher through
`model_effect_state`. After it they place the background-record run, then
`model_image_copy_frames`, then `movie_stream_requests` just before
`model_subdivided_effect`. Their movie ranges are Europe's, so they now
compile `european/movie_stream_requests` too. They place the same ten owners,
each with a `data_c.json` for the European command table.

The four language builds keep Europe's tables:
- `duel_field_layout` at `+0x204`;
- every European owner from `duel_scene_update` on at `+0x414`.

Their `duel_ground_projection` data ends with the localized duel-result
sprite tables instead of the two North American ones. That is one
opponent/no-opponent pair per language, each [2][10]
(`duel_result_localized_sprites.h`): 0x280 bytes from `0x80091B30` up to
`duel_scene_update`. The four builds compile the unit through
`spanish/duel_ground_projection.c`, which defines
`DUEL_RESULT_LANGUAGE_SPRITE_TABLES`, and own its `.data` at `0x80091AA8`.
German, Italian and Spanish now split their data image as plain data rather
than one `.sdata`-ordered bin.

Europe's small initialized data starts `0xF7C` above the North American
`.sdata` and follows Europe's own unit order. As in its text, the camera
orbit and movie units come before `model_graphics_state`. Its split places
eighteen owners from `main_services` (`0x8009BE84`) to
`main_menu_load_package_stage`. That includes `european/data_8009af6c`,
whose PAL strings lack the North American directory-entry format
(`gMemCard_szDirectoryEntryFormat`, guarded by `VERSION_EUROPE`); all five
PAL builds compile it at the US unit's `gcc_2_8_1_g8` profile. Europe has no
small data
for `duel_field_effects`, `options_layout_position_data` or
`ai_script_source_line_format`.

Europe now defines `runtime_gp`, `gMain_bSoundVSyncBusy` and
`gDebugEffect_PreviewState` itself instead of placing them through its
symbol file. The shared sources also gained European arms:
- `debug_menu` drops two unreferenced halfwords;
- `gMainMenu_PackageStageRect` stages the package at x 0x280, y 0xD0.

The four language builds keep Europe's small data 0x414 to 0x41C higher,
except that, as in their text, they place the movie unit after the model
units: the model units are 0x408 higher and `movie_frame_pipeline` 0x4EC
higher. Each split places the same eighteen owners. Only six zero bytes
after `duel_battle_stats` (`0x8009C2BA`) stay extracted, since nothing
identifies their owner; the two after `duel_card_effects` are alignment, as
in the North American split.

Their `debug_effect_screen` small data is seven bytes. Placing the owners
removed 74 more linker assignments per build.

Japan's small initialized data starts `0xC0` below the North American
`.sdata`. Like Europe, it places the movie and camera-orbit units before the
model units. Its split places nineteen owners from `main_services`
(`0x8009AE48`) to `ai_script_source_line_format`. Its layout differs from the
North American one in three places:
- `duel_field_effects` adds a Japanese-only `"Ex %d\n"` format (`gDuel_szSelectorTraceFormat`,
  read by Japan's `duel_field_card_objects`);
- `model_graphics_state` has no selected-digit byte and keeps the frame-step
  control as unaligned single bytes from `0x8009AF0B`, given as Japanese
  storage (`D_8009AF0B_slpm_86398` onward) through
  `japanese/model_graphics_state.c`;
- `model_scene_controllers` has no `WHY?` trace.

Only `model_debug_controller`'s four bytes (`0x8009AF64`), which Japan does
not compile, stay extracted.

With every build now owning `main_services`, `main_frame` and
`debug_effect_screen`, the shared sources define `runtime_gp`,
`gMain_bSoundVSyncBusy` and `gDebugEffect_PreviewState` unconditionally.

Europe (SLES-03947) has the same unit order but its own order of names inside
many units, so most of its blocks differ. Every compiler emits tentative
definitions in declaration order, so this is a source difference, not a
compiler variant. Europe's split folds its zero-initialized data into `main`
like Japan's, and lists 50 owners from `graphics_frame` (`0x8009BFE8`) to
`0x800F6BF0`. Three small ranges have no owning unit yet, so they stay
`bss_image` bins: `0x8009C1EA`, `0x8009C280`-`0x8009C2E8`, and the tail from
`0x800F6BF0`.

The European wrappers (now including `graphics_frame`, `file_transfer_runtime`,
`duel_effect_object_pool`, `script_command_busy`, `sorted_entry_relink`,
`sound_frontend` and `sound_output_transition`, which all five PAL builds
compile) give each differing block European storage:
- `.bss` blocks are one `u8` array, as in Japan. The exception is a block
  holding one object whose real type fills it, such as
  `struct FadeTransitionState gFade_State` (32 bands).
- `.sbss` blocks reuse the shared source's typed definitions in European
  address order, with the slots between them as `D_<ADDR>_sles_03947` scalars.
- A slot whose shared type has the slot's own size is defined under that
  shared name and type, with no link alias. A struct pointer the wrapper
  needs before any header is spelled through its tag, as in
  `struct SortedEntry *`.
- A name stays untyped, aliased to its slot, when its type would move it.
  This is a measured exception: maspsx aligns `.sbss` purely by size, so a
  byte or halfword name in a European word slot can't be given its real type.
  The cases are:
  - when its natural alignment (8 for arrays of 8 bytes or more, which also
    needs the block itself 8-aligned; 4 for words, pointers and arrays of 4 or
    more) doesn't hold at the European address;
  - when a scalar would run into the next name;
  - when a header the block needs already declares it, because GCC then emits
    it in the header's order.
- Typed definitions keep the shared source's defines and includes from ahead
  of its block, so the declarations that control addressing (such as the
  `.data` views) still apply.
- A new `.sbss` guard uses `<UNIT>_REGIONAL_SMALL_STORAGE`, so it stays
  independent of a `.bss` block another region already guards. A block that
  is already guarded keeps its existing macro.

The French, German, Italian and Spanish builds (SLES-03948 to SLES-03951)
share these wrappers and one layout: Europe's owners and storage, `0x420`
higher, with three exceptions. These builds link the movie units after the
model units, in their `.bss` as in their text:
- `model_scene_controllers` keeps the North American layout of its small
  block (`0x8009C7E8`-`0x8009C818`); its generated `model_commons`
  subsegment places the globals.
- `movie_frame_pipeline`'s small block follows at `0x8009C818`-`0x8009C838`,
  with these builds' own storage (`D_<ADDR>_sles_03948`) in
  `spanish/movie_pipeline.c`.
- In `.bss`, `movie_stream_requests` sits between `model_handler_dispatch`
  and `ai_script` (`0x800F6F08`), so the model blocks are `0x408` above
  Europe's.

`spanish/mem_card_dialog.c`, which the four builds also share, repeats the
European memory-card storage. Their zero-initialized data comes from units
from `graphics_frame` (`0x8009C408`) to `0x800F7010`. German, Italian and
Spanish fold their `initialized_image` segment into `main` the way Europe
folds `initialized_data`.

A linker assignment must not target a name that an owned section defines;
the C definition is the root. Ownership therefore also removed those
assignments: two in Japan and about 240 in each of the four language builds.

`build_regional_baseline.py` reads each region's `split.yaml`:
- a compiled unit listed there with a dotted data section keeps that section
  strong and placed, with `.bss` aligned to 8 as in the North American build;
- every other compiled section is discarded and its definitions are made weak
  (checked per section, from `objdump -t`);
- names inside owned blocks are `link_symbols.ld` aliases, as in the North
  American build.
