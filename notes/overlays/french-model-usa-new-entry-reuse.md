# French MODEL images reusing the newer North American entry C

Twelve North American MODEL entry sources match the French MODEL images below
once relocations are masked. Most of these sources were written for headers
72–828 in the USA MODEL image PRs. Under the project's GCC 2.8.1 split profile
they rebuild the French loader entries byte for byte. Only two regional
constants differ:

- The burst palette of headers 752, 754, 756, 766 and 779 is loaded from VRAM
  x = 640 rather than 512. Each North American source names it as
  `MODEL_VARIANT<header>_CLUT_X`, defaulting to 512.
- The full-screen flash of headers 119 and 754 covers 256 lines rather than
  240. Those sources use `GRAPHICS_DEFAULT_HEIGHT` from
  `src/game/graphics_constants.h`, which defaults to 240.

The North American objects are unchanged.

| French family | Images | North American body | Regional define |
|---|---:|---|---|
| 89 / 219 | 26 | `model_variant/variant72_entry.c` | — |
| 127 / 257 | 24 | `model_variant/variant110_entry.c` | — |
| 128 / 258 | 10 | `model_variant/variant111_entry.c` | — |
| 135 / 265 | 25 | `model_variant/variant118_entry.c` | — |
| 136 / 266 | 20 | `model_variant/variant119_entry.c` | height 256 |
| 769 / 866 | 30 | `model_variant/variant752_entry.c` | CLUT x 640 |
| 771 / 868 | 49 | `model_variant/variant754_entry.c` | CLUT x 640, height 256 |
| 773 / 870 | 51 | `model_variant/variant756_entry.c` | CLUT x 640 |
| 783 / 880 | 59 | `model_variant/variant766_entry.c` | CLUT x 640 |
| 796 / 893 | 72 | `model_variant/variant779_entry.c` | CLUT x 640 |
| 844 / 941 | 32 | `model_variant/variant827_entry.c` | — |
| 845 / 942 | 35 | `model_variant/variant828_entry.c` | — |

- The French wrappers are `french_model_variant/variant<family>_entry{,_slot1}.c`,
  named after the slot-0 French header. Each one sets only the regional
  defines. The slot-1 wrapper also repeats the North American slot-1 renames,
  because the image layout is the same at `0x8017B000`.
- Each family's binding file, `model_variant<family>_linker_symbols.txt`,
  binds the same names as the North American image to their French resident
  addresses. Every bound address is a French resident function start.
- The images keep their `french_model_image_*` module names. Only the entry
  and its `.rodata` become C; the rest of each image stays raw.

**433 French C instances** were added, all rebuilt and verified with
`overlay_build.py build/verify --region france --module`. The
[ledger](french-model-usa-new-entry-reuse.csv) lists every image with both
source fingerprints. The earlier
[header 397–428 reuse](french-model-usa-entry-reuse.md) follows the same
pattern.
