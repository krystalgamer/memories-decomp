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
