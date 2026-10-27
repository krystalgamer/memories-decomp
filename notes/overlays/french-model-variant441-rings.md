# French MODEL441 framebuffer-strip rings

`func_8013F14C` / `func_8017F14C` (image offset `0x414C..0x4754`, 1,544
bytes) is the last helper before the unclassified tail in all ten French
MODEL441 images (models 166, 360, 487 and 590 at stages 7/8, model 709 at
stages 9/10). The entry calls it directly. It is the same routine as the
accepted North American `variant398_rings.c`: three rings of sixteen
`POLY_GT4` strips, textured from the frame-buffer half each strip lands on.

The French wrappers `variant441_rings.c` and `variant441_rings_slot1.c` only
define `VARIANT398_RINGS_LAYOUT_441` and rename the function before including
the shared source. The difference belongs to the header, not the release,
so it is not a `VERSION_FRENCH` arm: the North American header-424
counterpart (`func_8013F168`) is the same body and is still assembly.

## Layout and source differences

The header-441 context is the one the Spanish MODEL441 notes already
describe (step `+26DC`, phase `+271C`, private view ending at `+2720`):

| Field | Header 398 | Header 441 |
| --- | --- | --- |
| screen rings (3 x `0x1E4`) | `+0E08` | `+0000` |
| `POLY_GT4` packet | `+18BC` | `+25D4` |
| matrix (translation read) | `+1998` | `+267C` |
| direction | `+19C0` | `+26A4` |
| frame count | `+19EC`, unsigned | `+26D0`, signed |
| step | `+19F8` | `+26DC` |
| phase | `+1A30` | `+271C` |

`variant398_entry.h` declares this context as
`Variant398Header441RingsState`, and the macro selects it as the work type. It also sets growth to `step * 80` (header 398 uses
`step * 96`) and keeps wrapping the scale while `phase < 4` (header 398
stops at `phase < 3`). Without the macro, the work type stays
`Variant398EntryState` and the constants are the original ones.

The ring pointer is now assigned before the ordering-table fetch. Header 441
needs this: the context register must stay in `s6` (the ring register),
with the work copy in `s8` made after `GsGetActiveBuff`. The move does not
change the header-398 output. The other consumers of the shared
`variant398_rings.c` / `variant398_entry.h` are the ten North American
header-398 images (`gcc_2_7_2_cdk_g0`) and the ten French header-415 images
(`gcc_2_8_1_g0_split`). All of them still rebuild exactly. No Spanish
source or manifest uses these files.

## Result

The named `gcc_2_8_1_g0_split` profile matches both slots. All ten complete
20,480-byte images rebuild exactly, adding ten C instances / 15,440 bytes.
See the [attempt ledger](french-model-variant441-rings-attempts.csv). The
entry at `+0004` is now the only assembly function left in these images.
