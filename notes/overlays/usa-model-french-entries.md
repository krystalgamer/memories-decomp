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
