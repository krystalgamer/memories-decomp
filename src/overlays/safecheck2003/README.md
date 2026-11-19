# safechk 2.0.0.3 boot module

`safechk.c` is the boot protection module `safecheck2003` (WA_MRG module
`0x16`, loaded at `0x80168000`): Sony's runtime library safechk 2.0.0.3 as
one translation unit, with its `.rdata`, `.text`, `.data` and `.bss`. Every
region builds it from this source with the
`gcc_2_7_2_cygnus_g0_mips_as_gnu_reorder` profile; only the resident Psy-Q
call targets differ, bound by each region's
`config/<serial>/overlays/safecheck2003_linker_symbols.txt`.
The leading module word is the game's packaging, `module_header.c`
(`gcc_2_8_1_g0`, `VERSION_EUROPE` for PAL's `0x09`).

The module was formerly called APv2. See
[`notes/overlays/safecheck2003.md`](../../../notes/overlays/safecheck2003.md)
for its layout, behaviour and compiler.
