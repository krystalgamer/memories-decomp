# Spanish MODEL456 two-group lines

Helper `+0x315C..+0x34D4` is independently reconstructed matching C: 888 bytes,
six physical instances, 5,328 instruction bytes, and one unique routine. An
independent second-slot compilation also matches. Screening 6,207 accepted
regional C entries found no same-sized body; no region has accepted C for this
routine.

## Physical loads and ownership

An exhaustive scan identifies six loads with headers 456/606 and six distinct
complete-image hashes:

| Model | Record | Stages | Sectors | Command |
|---:|---:|---:|---:|---:|
| 163 | 163 | 7/8 | 45168/45178 | 622002 |
| 460 | 410 | 9/10 | 113360/113370 | 622001 |
| 536 | 486 | 9/10 | 134336/134346 | 622000 |

The entry indexes 44-byte descriptors at `+0x3DFC`. Seven closed contiguous
functions begin at `+4`, `+0x101C`, `+0x1834`, `+0x1F58`, `+0x2ACC`, `+0x315C`
and `+0x34D4`; raw data begins at `+0x3D00`. The entry calls `+0x1F58` and
`+0x2ACC`.

The selected helper is a closed orphan: no complete image contains a call to it
or its address. Only the selected helper becomes C, leaving thirty-six ASM
instances.

## Behavior

Two 0x50-byte line groups at context `+0`, each holding six `SVECTOR` points and
a color, pair with:

- two 0xF0-byte companion records at `+0x160`, with progress at `+0` and word
  origins at `+0x10..+0x18`
- two 0xC0-byte control records at `+0x1608`, with size at `+0`, a fade flag at
  `+0x30` and intensity at `+0x34`

Four `ratan2` results are computed and discarded, as in the target.

Each group is drawn as follows:

- **Scale:** the control size, plus a quarter of that size on odd frames.
- **Color:** the group color, scaled by the control intensity while fading.
  Each fading group also advances the shared rotation angle by four steps.
- **Lines:** five spokes, from point zero to points one to five, project
  through the `GsGLINE` at `+0x22B4`. They fade from the group color to black.
- **Submission:** requires positive depth and a companion progress of at least
  1024.

## Source detail that reproduces the target

The helper uses the same construction as the accepted MODEL450 lines:

- separate companion and control base pointers with running indices;
- the same declaration order and plain assignment statements.

The first form fixes the shared induction and the 296-byte frame. The second
fixes the prologue register roles. The natural frame needs no extra locals,
forced registers or inline assembly.

## Acceptance evidence

All six complete 20KiB images match using `gcc_2_8_1_g0_split`. The
fourteen-row ledger preserves all six rejected experiments, the exact source
and second-slot compilations, and all six terminal full-image results.

Regressions cover:

- target layouts and repeated header inclusion
- all physical loads and descriptors
- the orphan no-reference proof
- every selected relocation
- all input and final function and data owners
- 34 resident bindings with loader and context ownership

The private state covers only the observed prefix through `+0x236C`.

Terminal dependency fingerprints hash the body, private header, shared
model-variant header, and `gpu_packets.h`, in that order.
