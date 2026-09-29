# Spanish MODEL primary copy and effect handlers

The first registered MODEL batch covers seven models at both primary slots:
14 complete 4,096-byte images and 3,264 matching instruction bytes. Four source
units use the named `gcc_2_8_1_g0_split` GCC 2.8.1/MASPSX 2.81 pipeline. Slot-one
wrappers rename the entry and descriptor symbols before including the same C
body; there are no patched instructions or absolute descriptor aliases.

`MODEL.MRG` contains 621 compact records of 276 sectors. Transfer stages 11 and
12 read record sectors 220 and 222 respectively, two sectors each, to
`0x8013A000` and `0x8017A000`. The header is four bytes and the entry is base+4.
The record's final metadata sector is 275. Transfer phase 16 copies metadata
offset `0x100` to slot offset `0xCF8`; its command at metadata `0x118` becomes
slot `0xD10`. `model_load_step.c` phase 9 passes nonnegative `commands[2] % 1000`
to the primary entry. `model_control.c` subsequently passes `-1`.

| Models | Compact records | Header IDs, slots 0/1 | Entry bytes | Descriptor offset | Unclassified tail |
|---|---|---|---:|---:|---|
| 150, 167 | 150, 167 | 55 / 58 | 276 | `0x118` | `0x148..0x1000` |
| 116, 370, 394, 707, 715 | 116, 320, 344, 607, 615 | 56 / 59 | 216 | `0xDC` | `0x10C..0x1000` |

The copy handler selects a 16-byte descriptor, advances an unsigned tick,
computes the original delay/frame modulo, and copies the texture rectangle
through `MoveImage`. The slot contributes 256 to source/destination X and
the Y coordinates add 256. Model 150's command `999001` selects descriptor 1;
model 167's `999002` selects descriptor 2. The work structure is 16 bytes.

The effect handler selects a 16-byte descriptor using argument `% 100` and
animation `argument / 100 + 3`. It calls `func_8005D994` only when that animation
is active. Commands are `998001` (116), `998101` (370/715), `998102` (394),
and `998000` (707). They select indices 0..2 and animation 3 or 4. Its work
structure is eight bytes; the descriptor's last eight bytes are an `SVECTOR`.
The names of uncertain scalar effect parameters remain generic.

The 48-byte descriptor prefixes were reread from the legal Spanish disc and
are identical within each family. This establishes referenced storage for
indices 0..2, **not a terminal bound for every possible table**. The remaining
3,768 or 3,828 bytes per image stay unclassified generated storage, not recovered
C. Entire tails are retained independently because complete module hashes differ.

Production verification checks the real selected compiler object and linked
function's section, address and size, the generated descriptor object's 48-byte
storage and final section-defined symbol, and each complete module hash.
Target-compiled layout probes cover 27 sizes/offsets, including the canonical
metadata/slot command path. The linker binds `Model_GetActiveSlotIndex` at
`0x8005BED4`, `Model_GetSlotAnimationIndex` at `0x8005BF70`,
`func_8005D994` at `0x8004D9A4` and `MoveImage` at `0x8007FFD0`.
The MoveImage identity is supported by the identified North American SDK body
and its diagnostic string, with only regional address fields differing.

This is not exhaustive MODEL coverage. Other primary families, return-two
primaries, all four variant phases, secondary loads, and every unclassified
tail remain separate recovery/registration work.
