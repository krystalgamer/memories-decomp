# Post-`mfhi` `nop` before a small external store

## Report

`func_8004E7B0` compiles to the retail instruction stream under
`gcc_2_8_1_g8` except for one `nop` after the dynamic remainder's `mfhi`.
The compiler input to the assembler ends:

```asm
rem     $2,$3,$2
sh      $2,D_8009AF8E
...
.extern D_8009AF8E, 2
```

The profile uses `-G8`. The final store is therefore one GP-relative
instruction, but maspsx 2.81 emulation treats the symbolic store as a possible
`$at` expansion and emits no post-`mfhi` `nop`.

Retail at file offset `0x3F068` contains:

```text
10 10 00 00  00 00 00 00  86 00 82 A7
mfhi $v0     nop          sh $v0, %gp_rel(D_8009AF8E)($gp)
```

Please check the generated `tmp/maspsx_bugs/post-mfhi-small-extern-store-nop/compiler.s`
with ASPSX 2.81 and `-G8`.

## Profile

`gcc_2_8_1_g8`:

- GCC 2.8.1: `-O2 -G8 -mel -mips1 -mcpu=R3000 -msoft-float -fno-builtin`
- MASPSX: `--aspsx-version=2.81 --expand-div -G8`
- assembler: `-EL -mips1 -G8`

Run:

```sh
tools/maspsx_bugs/post-mfhi-small-extern-store-nop/build.sh
```

All generated files stay under `tmp/maspsx_bugs/`.

## Ruled out

- The C reaches 124/124 instructions, exact relocations and exact section
  layout when the missing `nop` is present.
- Prebuilt and independently source-built GCC 2.8.1 produce identical
  compiler instruction streams.
- GCC 2.7.2, scheduler and CPU variants, CSE switches, volatile result/store
  forms, pointer and object wrappers, and GNU assembler division expansion do
  not reproduce the retail sequence.
- MASPSX's ASPSX 2.21 mode inserts this `nop`, but also changes unrelated
  immediate expansion and is not the selected Psy-Q 4.6 assembler.
- A local diagnostic that retains `.extern symbol,size` entries of at most
  `-G` as small-data symbols inserts the retail `nop` and changes none of 647
  cached matching C objects. That diagnostic is intentionally not included in
  the project toolchain pending the ASPSX result.
