# MODEL variant modules

`Model_LoadMonsterMerge` loads four more stages per model after the MODEL
primary. Each stage is 10 sectors at `record * 276 + 180`, `+ 190`, `+ 200` and
`+ 210`. Slot 0 loads at `0x8013B000` and slot 1 at `0x8017B000`. This note
calls these images the model variants. The first registered image is
`model_variant_1_pos0_slot0`: record 1, stage offset `+ 180`, slot 0, sector
456, SHA-256
`782fd3a706de83fdffa971127c2b80797bce24f2ba15130da793219cda9b1c3d`.

## Compiler

The variants were not built with the gcc 2.8.1 that builds the rest of the
game. Their functions end in `addiu $sp, $sp, N; jr $ra; nop`. gcc 2.7.2 emits
that epilogue as reorder-mode text, and the assembler fills the delay slot
with a `nop`. gcc 2.8.1 emits the epilogue as RTL, and reorg moves the stack
adjustment into the slot. Everything else in the two helpers registered here
is the same under both compilers, so the epilogue is the only thing that
separates them.

The `gcc_2_7_2_cdk_g0` profile uses the decompals/old-gcc 0.17
`gcc-2.7.2-cdk` release (`cygnus-2.7.2-970404`), installed by
`make compiler-272-prebuilt` from the pinned
`tools/bootstrap/old_gcc_272_prebuilt.json`. Apart from the compiler path it
is identical to `gcc_2_8_1_g0`: same flags, same maspsx arguments.

Four helper bodies were measured against their retail words with each 2.7.2
build:

| Helper | Instructions | `gcc-2.7.2-cdk` | `gcc-2.7.2-psx` |
|---|---:|---|---|
| quad (`quad.c`) | 218 | match | 4 words differ |
| rings (`rings.c`) | 224 | match | 24 words differ |
| third helper | 195 | match | 27 words differ |
| fourth helper | 314 | match | match |

The "third helper" and "fourth helper" rows are not registered in this change.
PsyQ 4.1's `CC1PSX.EXE` (the same `cygnus-2.7.2-970404`) also matches all four
helpers, run under wine.

Negative control: with `gcc_2_8_1_g0` selected for both registered functions,
`make match-overlays` fails with "rebuilt module does not match its input" for
this module. `make overlays` and `make verify-overlays` pass under either
profile, because neither of them compiles C.

## First image

| Range | Address | Contents |
|---|---|---|
| `0x0000-0x0004` | `0x8013B000` | Module header word |
| `0x0004-0x3168` | `0x8013B004` | Seven functions, still assembly |
| `0x3168-0x34E8` | `0x8013E168` | `func_8013E168` in `src/overlays/model_variant/rings.c` |
| `0x34E8-0x3850` | `0x8013E4E8` | `func_8013E4E8` in `src/overlays/model_variant/quad.c` |
| `0x3850-0x5000` | `0x8013E850` | Data, unclassified |

The 36 resident calls resolve to the North American addresses of the same
names. They are listed in `model_variant_linker_symbols.txt`, which later
images can share.

The two C functions are shared helpers: the same masked bodies recur in many
other variant images at other addresses. Registering those images does not
need new C for these two, only a layout and symbol file per image.
