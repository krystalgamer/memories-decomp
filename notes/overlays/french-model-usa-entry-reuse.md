# French MODEL entries reusing accepted North American C

The entry functions of eight French MODEL variant families are the accepted
North American entry bodies. Under the project's GCC 2.8.1 split profile they
produce byte-identical code, except that the PAL images load the effect palette
from VRAM x = 640 (`GetClut(640, 244)`) instead of 512.

| French family | Images | North American body | Entry size |
|---|---:|---|---|
| 414 | 24 | `model_variant/variant397_entry.c` | `0x1224` |
| 415 | 10 | `model_variant/variant398_entry.c` | `0x12B0` |
| 421 | 12 | `model_variant/variant404_entry.c` | `0x11CC` |
| 424 | 30 | `model_variant/variant407_entry.c` | `0x12D4` |
| 433 | 4 | `model_variant/variant416_entry.c` | `0x104C` |
| 435 | 26 | `model_variant/variant418_entry.c` | `0x1080` |
| 445 | 12 | `model_variant/variant428_entry.c` | `0x1030` |
| 796 | 72 | `model_variant/variant779_entry.c` | `0x20DC` |

- Each North American source now names its palette x as
  `MODEL_VARIANT<header>_CLUT_X`. It defaults to 512, so the North American
  objects are unchanged.
- The French wrappers `french_model_variant/variant<family>_entry{,_slot1}.c`
  set it to 640. They also rename only the in-image helpers and the data table
  that follows the last function to their French addresses. The French helpers
  are a few bytes shorter, so these addresses move earlier.
- Header 779 stores its burst palette as a constant rather than calling
  `GetClut`, so `variant779_entry.c` writes it as
  `getClut(MODEL_VARIANT779_CLUT_X, 244)`: `0x3D20` in North America and
  `0x3D28` in the PAL images. Its North American images already use
  `gcc_2_8_1_g0_split`.
- The other North American images use the `gcc_2_7_2_cdk_g0` profile. That profile
  leaves one extra epilogue delay-slot `nop` and allocates different
  temporaries. The French images match only `gcc_2_8_1_g0_split`.
- Each family binding file gains only the missing SDK bindings these bodies
  call (for example `GetTPage`, `GetClut` and `SetPolyGT4`). Every bound
  address is a French resident function start.
- The 72 header-796/893 images were raw-byte layouts with one inventoried
  assembly entry each. They are now `model_variant_<model>_<stage>_slot<n>`
  layouts that copy the North American header-779 segments (entry, `.rodata`
  scale vector, raw tail). Their new `model_variant796_linker_symbols.txt` was
  read from the French `jal` targets at the North American call sites; all 22
  bindings agree with the existing French binding files.

**190 French C instances** were added. All French and North American overlay
images rebuild exactly; no other release uses these sources. The
[ledger](french-model-usa-entry-reuse.csv) lists every image, its North
American body, the French wrapper and both fingerprints.
