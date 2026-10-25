# North American MODEL images built from French entries

These North American MODEL variant images contain a single framed function, the
entry at `+0x4`, and that entry is the same code as a French entry already matched
in `src/overlays/french_model_variant/`: compiled under the French profile
(`gcc_2_8_1_g0_split`), the French object's `.text` equals the North American
bytes once `jal` targets and the `%hi`/`%lo` pairs are masked. Each image keeps
the French layout after the entry (the entry's `.rodata` and the image's data
views, then an unclassified tail), and builds from the French source, the slot-1
wrapper for images loaded at `0x8017B000`. The resident bindings in each
`model_variant_french<N>_linker_symbols.txt` are read from the North American image itself:
every `R_MIPS_26` relocation of the French object is resolved to the target of
the `jal` at the same offset of the North American entry.

Fourteen of the French sources called `Model_GetLightSourceMatrix`,
`GsGetLwUnit` or `Model_SetFrameStepOverride` through an implicit declaration,
which the North American declaration check rejects. They now include the header
that declares the function; their objects (`.text`, `.rodata`, `.data` and both
relocation sections, slot 0 and slot 1) are byte-identical to the objects built
before the change, so the French images are unaffected.

| Header | Entry size | French source | Images |
|---:|---:|---|---|
| 59 | `0xB94` | `variant76_entry.c` | `447_pos0_slot0`, `489_stage9_slot0`, `562_pos0_slot0`, `57_pos0_slot0` |
| 60 | `0xC14` | `variant77_entry.c` | `386_pos0_slot0`, `53_stage9_slot0`, `547_stage9_slot0`, `84_stage9_slot0` |
| 65 | `0xDE0` | `variant82_entry.c` | `66_pos0_slot0` |
| 70 | `0xA18` | `variant87_entry.c` | `140_stage9_slot0`, `584_stage9_slot0` |
| 80 | `0x1154` | `variant97_entry.c` | `100_pos0_slot0`, `150_stage9_slot0` |
| 84 | `0xAE0` | `variant101_entry.c` | `130_pos0_slot0` |
| 90 | `0x1184` | `variant107_entry.c` | `28_stage9_slot0`, `402_pos0_slot0`, `85_stage9_slot0` |
| 99 | `0x1158` | `variant116_entry.c` | `127_stage9_slot0`, `22_stage9_slot0`, `422_pos0_slot0`, `454_stage9_slot0`, `484_stage9_slot0`, `504_stage9_slot0`, `556_pos0_slot0`, `581_stage9_slot0`, `701_stage9_slot0`, `73_stage9_slot0` |
| 100 | `0x1088` | `variant117_entry.c` | `139_pos0_slot0`, `146_pos0_slot0`, `15_stage9_slot0` |
| 104 | `0x10A4` | `variant121_entry.c` | `227_stage9_slot0`, `281_pos0_slot0`, `39_stage9_slot0`, `477_stage9_slot0`, `502_pos0_slot0`, `509_pos0_slot0`, `514_pos0_slot0`, `514_stage9_slot0`, `557_stage9_slot0`, `641_pos0_slot0`, `708_stage9_slot0` |
| 112 | `0xFE4` | `variant129_entry.c` | `219_pos0_slot0`, `30_stage9_slot0`, `355_stage9_slot0`, `435_stage9_slot0`, `490_stage9_slot0` |
| 114 | `0x15B8` | `variant131_entry.c` | `183_pos0_slot0`, `273_pos0_slot0`, `486_pos0_slot0`, `490_pos0_slot0`, `540_pos0_slot0`, `637_pos0_slot0` |
| 115 | `0xEB4` | `variant132_entry.c` | `416_stage9_slot0` |
| 124 | `0xF5C` | `variant141_entry.c` | `20_stage9_slot0`, `428_pos0_slot0`, `445_pos0_slot0` |
| 126 | `0xFF0` | `variant143_entry.c` | `234_stage9_slot0`, `481_pos0_slot0`, `496_pos0_slot0`, `546_stage9_slot0` |
| 189 | `0xB94` | `variant76_entry_slot1.c` | `447_stage8_slot1`, `489_stage10_slot1`, `562_stage8_slot1`, `57_stage8_slot1` |
| 190 | `0xC14` | `variant77_entry_slot1.c` | `386_stage8_slot1`, `53_stage10_slot1`, `547_stage10_slot1`, `84_stage10_slot1` |
| 195 | `0xDE0` | `variant82_entry_slot1.c` | `66_stage8_slot1` |
| 200 | `0xA18` | `variant87_entry_slot1.c` | `140_stage10_slot1`, `584_stage10_slot1` |
| 210 | `0x1154` | `variant97_entry_slot1.c` | `100_stage8_slot1`, `150_stage10_slot1` |
| 214 | `0xAE0` | `variant101_entry_slot1.c` | `130_stage8_slot1` |
| 220 | `0x1184` | `variant107_entry_slot1.c` | `28_stage10_slot1`, `402_stage8_slot1`, `85_stage10_slot1` |
| 229 | `0x1158` | `variant116_entry_slot1.c` | `127_stage10_slot1`, `22_stage10_slot1`, `422_stage8_slot1`, `454_stage10_slot1`, `484_stage10_slot1`, `504_stage10_slot1`, `556_stage8_slot1`, `581_stage10_slot1`, `701_stage10_slot1`, `73_stage10_slot1` |
| 230 | `0x1088` | `variant117_entry_slot1.c` | `139_stage8_slot1`, `15_stage10_slot1` |
| 234 | `0x10A4` | `variant121_entry_slot1.c` | `227_stage10_slot1`, `281_stage8_slot1`, `39_stage10_slot1`, `477_stage10_slot1`, `502_stage8_slot1`, `509_stage8_slot1`, `514_stage10_slot1`, `514_stage8_slot1`, `557_stage10_slot1`, `641_stage8_slot1`, `708_stage10_slot1` |
| 242 | `0xFE4` | `variant129_entry_slot1.c` | `219_stage8_slot1`, `30_stage10_slot1`, `355_stage10_slot1`, `435_stage10_slot1`, `490_stage10_slot1` |
| 244 | `0x15B8` | `variant131_entry_slot1.c` | `183_stage8_slot1`, `273_stage8_slot1`, `486_stage8_slot1`, `490_stage8_slot1`, `540_stage8_slot1`, `637_stage8_slot1` |
| 245 | `0xEB4` | `variant132_entry_slot1.c` | `416_stage10_slot1` |
| 254 | `0xF5C` | `variant141_entry_slot1.c` | `20_stage10_slot1`, `428_stage8_slot1`, `445_stage8_slot1` |
| 256 | `0xFF0` | `variant143_entry_slot1.c` | `234_stage10_slot1`, `481_stage8_slot1`, `496_stage8_slot1`, `546_stage10_slot1` |
| 765 | `0x14AC` | `variant782_entry.c` | `217_pos0_slot0`, `453_pos0_slot0`, `563_pos0_slot0`, `58_pos0_slot0`, `612_pos0_slot0`, `78_stage9_slot0` |
| 812 | `0xF84` | `variant829_entry.c` | `485_pos0_slot0` |
| 862 | `0x14AC` | `variant782_entry_slot1.c` | `217_stage8_slot1`, `453_stage8_slot1`, `563_stage8_slot1`, `58_stage8_slot1`, `612_stage8_slot1`, `78_stage10_slot1` |
| 909 | `0xF84` | `variant829_entry_slot1.c` | `485_stage8_slot1` |

Four more images whose entry has the same French body hold further framed
functions after it (`model_image_model_7632_8013b000`, `_7642_8017b000`,
`_35528_8013b000` and `_35538_8017b000`); they stay uninventoried until those
functions are classified.

## Regional constants

Thirty more images have a French entry whose code differs from the North American
one in a single regional constant. Five French sources now take it from a macro
whose default is the French value, and a North American wrapper in
`src/overlays/model_variant/` sets the North American one:

- `variant770_entry`: the burst CLUT, `VARIANT770_BURST_CLUT` (French `0x3D28`,
  North American `0x3D20`);
- `variant96_entry`, `variant144_entry`, `variant198_entry` and `variant199_entry`:
  the screen height of the full-screen flash (and, in `variant144_entry`, of the
  gradient's bottom edge), `GRAPHICS_DEFAULT_HEIGHT` from
  `src/game/graphics_constants.h` (French 256, North American 240).

The same five sources also gain the headers that declare `Model_GetLightSourceMatrix`
and `GsGetLwUnit`, which they called implicitly. The French objects of all five,
both slots, are byte-identical to the objects built before these changes.

| Header | Entry size | North American source | Images |
|---:|---:|---|---|
| 79 | `0xC48` | `variant96_entry_usa.c` | `192_stage9_slot0`, `196_stage9_slot0` |
| 127 | `0xEAC` | `variant144_entry_usa.c` | `492_stage9_slot0` |
| 181 | `0xD70` | `variant198_entry_usa.c` | `508_stage9_slot0` |
| 182 | `0x1590` | `variant199_entry_usa.c` | `243_stage9_slot0`, `622_stage9_slot0` |
| 209 | `0xC48` | `variant96_entry_usa.c` | `192_stage10_slot1`, `196_stage10_slot1` |
| 257 | `0xEAC` | `variant144_entry_usa.c` | `492_stage10_slot1` |
| 311 | `0xD70` | `variant198_entry_usa.c` | `508_stage10_slot1` |
| 312 | `0x1590` | `variant199_entry_usa.c` | `243_stage10_slot1`, `622_stage10_slot1` |
| 753 | `0x16E4` | `variant770_entry_usa.c` | `103_pos0_slot0`, `187_stage9_slot0`, `239_pos0_slot0`, `23_pos0_slot0`, `480_pos0_slot0`, `499_pos0_slot0`, `555_pos0_slot0`, `596_stage9_slot0`, `85_pos0_slot0` |
| 850 | `0x16E4` | `variant770_entry_usa.c` | `103_stage8_slot1`, `187_stage10_slot1`, `239_stage8_slot1`, `23_stage8_slot1`, `480_stage8_slot1`, `499_stage8_slot1`, `555_stage8_slot1`, `596_stage10_slot1`, `85_stage8_slot1` |

## The work-size printf

Forty-seven more images have a French entry with one North American addition:
the setup command prints the size of the work area with
`printf("%d (0x%x) Byte\n", sizeof(state), sizeof(state))` (0x1244 for
`Model173State`, 0x450 for `Model153State` and `Model178State`, 0x738 for
`Model184State`, 0x5B0 for `Model177State`, 0x820 for `Model157State` and
`Model162State`, 0x464 for `Model163State`), which also moves the entry's
`.rodata` and data after it. Each of these French sources adds the call under
its own `VARIANTnnn_PRINT_WORK_SIZE` macro; the North American wrappers define
the macro and rename the image-local data symbols to their North American
addresses. The `variant177_entry`, `variant157_entry`, `variant163_entry` and
`variant162_entry` wrappers also define the North American screen height, 240,
for the full-screen flash, through the same `GRAPHICS_DEFAULT_HEIGHT` default
that header 198 uses. Under its macro, `variant178_entry` also ends with the
plain completion test (`if (work->completed == 0) { ...; return 1; } return 0;`)
that North American retail's shared return block needs. The `variant162_entry`
call prints `"%d ( %x ) Byte.\n"` instead, and retail leaves `0x18, 0x20, 0x02`
in the three bytes that pad that string to the image view, so the source spells
the format as a 20-byte array that holds them. The French sources
include the headers that declare the functions they call
(`Model_GetLightSourceMatrix`, `Model_CopySlotPosition`, `GsGetLwUnit`), so the
North American wrappers do not call any of them through an implicit
declaration. All eight French objects are byte-identical to the objects built
before.

| Header | Entry size | North American source | Images |
|---:|---:|---|---|
| 136 | `0xE14` | `variant153_entry_usa.c` | `271_pos0_slot0`, `356_stage9_slot0`, `38_pos0_slot0`, `635_pos0_slot0` |
| 156 | `0xEC0` | `variant173_entry_usa.c` | `113_stage9_slot0`, `350_stage9_slot0`, `386_stage9_slot0`, `392_pos0_slot0`, `448_stage9_slot0`, `8_stage9_slot0` |
| 266 | `0xE14` | `variant153_entry_usa.c` | `167_stage10_slot1`, `271_stage8_slot1`, `356_stage10_slot1`, `38_stage8_slot1`, `635_stage8_slot1` |
| 286 | `0xEC0` | `variant173_entry_usa.c` | `113_stage10_slot1`, `350_stage10_slot1`, `386_stage10_slot1`, `392_stage8_slot1`, `448_stage10_slot1`, `8_stage10_slot1` |
| 167 | `0xF04` | `variant184_entry_usa.c` | `169_stage9_slot0`, `524_stage9_slot0`, `591_stage9_slot0` |
| 297 | `0xF04` | `variant184_entry_usa.c` | `169_stage10_slot1`, `524_stage10_slot1`, `591_stage10_slot1` |
| 140 | `0x1068` | `variant157_entry_usa.c` | `131_stage9_slot0`, `145_stage9_slot0` |
| 270 | `0x1068` | `variant157_entry_usa.c` | `131_stage10_slot1`, `145_stage10_slot1` |
| 160 | `0x11AC` | `variant177_entry_usa.c` | `164_stage9_slot0`, `191_stage9_slot0`, `494_stage9_slot0`, `521_stage9_slot0`, `599_stage9_slot0` |
| 290 | `0x11AC` | `variant177_entry_usa.c` | `164_stage10_slot1`, `191_stage10_slot1`, `494_stage10_slot1`, `521_stage10_slot1`, `599_stage10_slot1` |
| 145 | `0xDBC` | `variant162_entry_usa.c` | `37_pos0_slot0` |
| 275 | `0xDBC` | `variant162_entry_usa.c` | `37_stage8_slot1` |
| 146 | `0xEB8` | `variant163_entry_usa.c` | `37_stage9_slot0` |
| 276 | `0xEB8` | `variant163_entry_usa.c` | `37_stage10_slot1` |
| 161 | `0xD44` | `variant178_entry_usa.c` | `141_stage9_slot0` |
| 291 | `0xD44` | `variant178_entry_usa.c` | `141_stage10_slot1` |

## What remains

Measured on 2026-10-23 over the 2,116 North American images at the variant
addresses (`0x8013B000` and `0x8017B000`) that were registered only as
baseline `model_image_model_*` layouts. Functions are framed heuristically
from `addiu sp, sp, -N` to `jr ra`, with `jal` targets and `%hi`/`%lo` pairs
masked, so the counts are orders of magnitude:

| Measure | Count |
|---|---:|
| Framed functions | about 3,300 |
| Distinct bodies (masked) | 624 |
| Instructions, one copy of each distinct body | about 709,000 |
| Images now built from French C (this note) | 210 |
| Images now built from North American C ([header 72](usa-model-variant72.md), [header 110](usa-model-variant110.md), [header 111](usa-model-variant111.md), [header 118](usa-model-variant118.md), [header 119](usa-model-variant119.md), [header 176](usa-model-variant176.md), [header 627](usa-model-variant627.md), [header 634](usa-model-variant634.md), [header 647](usa-model-variant647.md), [header 654](usa-model-variant654.md), [header 752](usa-model-variant752.md), [header 754](usa-model-variant754.md), [header 756](usa-model-variant756.md), [header 766](usa-model-variant766.md), [header 779](usa-model-variant779.md), [header 827](usa-model-variant827.md), [header 828](usa-model-variant828.md)) | 553 |

All framed functions use the gcc 2.8.1 epilogue (`jr ra` with the frame
release in its delay slot). Three single-function families had a French
entry as a close template (opcode similarity 0.59 to 0.68): 27 images like
`model_image_model_113892_8013b000` and 24 like `model_image_model_109220_8013b000`
against `variant116_entry`, and 10 like `model_image_model_114996_8013b000`
against `variant129_entry`. The first 27 and the last ten now build from
North American C, see [USA MODEL headers 176 and 306](usa-model-variant176.md)
and [USA MODEL headers 111 and 241](usa-model-variant111.md). The other bodies have no French template. Many of
them are single-function images repeated across dozens of models (the
header-647/714 entry, 917 instructions, is the only function of 41 images;
it now builds from North American C as well).
Four images whose entry is a French body hold more functions and stay
baseline layouts.

