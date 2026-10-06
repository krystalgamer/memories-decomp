# Spanish MODEL443 twisted mesh

The helper at `+0x298C..+0x2F2C` is independently reconstructed matching C:
1,440 bytes in each of MODEL62's stage-9/10 MODEL443/593 loads. These are two
C instances and 2,880 instruction bytes, representing one unique routine.
This is not a port of accepted French or other regional C. Initial screening
checked 6,121 regional C entries and four same-sized bodies without an accepted
normalized match. The accepted-base refresh after MODEL417's merge checked
6,129 entries, again without a match.

## Physical loads and ownership

An exhaustive physical loader scan finds exactly record 62, sectors
17312/17322, with distinct 20KiB image hashes. Command 609000 selects the
88-byte descriptor at `+0x4BF8` in both images. The instances ledger preserves
these identities and descriptor hashes.

Seven closed, contiguous functions begin at `+4`, `+0x1ADC`, `+0x23AC`,
`+0x298C`, `+0x2F2C`, `+0x3668`, and `+0x3EF0`. Code ends at `+0x45A8`.
All six helpers are called by the entry. Only the selected mesh becomes C;
the other twelve function instances remain generated ASM and both raw tails
remain data. Thirty-five resident addresses retain their original ownership.

## Geometry and timing

Entry `+0x1958` passes the context captured in `s3` at `+0xC` under its
phase-at-least-four gate. One `0x54C`-byte group at context `+0xC80` contains
the observed nine-by-nine `SVECTOR` grid and nine colors at group `+0x510`.
The unobserved interval between the grid and colors remains a byte view,
not a guessed second grid or auxiliary structure.

Radius is the signed size at context `+0x2A68 + 0x88`, divided by 64 and
narrowed to a signed halfword. Tilt is `ratan2(path.z, path.y) + 3072`.
The other three initial `ratan2` return values are unused, but calls remain.
Each row advances along the signed path according to progress, while its nine
points use 512-unit angle increments. The row's base angle also advances by
512. Signed division, multiplication, shift, and halfword-store order remain
unchanged.

The origin supplies the translation with zero rotation. Retail writes three
4096 values to the scale vector at stack `+0x30/+0x34/+0x38` but does not
call `ScaleMatrix` here. These are observed stores, not invented stack padding
or an unused allocation control. They remain in C and have explicit regressions.

The draw pass projects 64 neighboring quads using the `POLY_G4` at
`+0x33AC..+0x33D0`. Vertex colors alternate between the current and next
column's colors, reused on both rows. Nonnegative depth and flag gate the
resident polygon submission helper; depth narrows to 16 bits and flags are one.

During phase four, progress below 1024 is recomputed from the unsigned time
ratio using descriptor `+0x48/+0x4C`. Reaching 1024 clamps progress and changes
phase to five. The angle decreases by `step * 128` on every invocation.

## Why the row address is an integer expression

The last remaining mismatch was the operand order of a single commutative
`addu`. Typed row-pointer expressions emitted the opposite order. The exact
source adds the typed row stride to the 32-bit guest base address, with a
shared signed row index. This is confined to the game's fixed context domain,
not a claim that arbitrary native host pointers can be truncated.

The complete chain is checked against matched Spanish resident instructions:
`func_8004CB0C` at `0x8004FC2C` initializes slot `+0xDEC` from `D_80010024`
or `D_80010028`, whose values are `0x80136000` and `0x80176000`.
`func_800559D4` at `0x80058B4C` loads that unchanged secondary context and
passes it to both the initialization and frame callbacks at the selected
overlay base plus four. The overlay entry preserves it for this helper.

All nine row bases and their nine points remain below 2^32 and inside the
observed `0x288`-byte grid. The complete private state view through `+0x369C`
also ends before the next overlay allocation. The stored timing pointer uses
`*G32`; transient local pointers remain plain.

## Reproduction evidence

The authoritative `gcc_2_8_1_g0_split` profile uses GCC 2.8.1 and MASPSX 2.81.
Fourteen source probes recover the row-index lifetime, spill order, draw-phase
reset scheduling, and final guest-address addition. A separate second-slot
compilation is exact. The seventeen-row ledger retains all fifteen source/slot
records and both terminal full-image matches.

Regressions cover 36 target layouts, repeated header inclusion, every physical
load and descriptor, original-context reaching definitions, the fixed guest
context chain, all row bounds, observed scale stores, packet bounds, all
function/data owners, every selected relocation, and resident ownership.
Terminal dependency fingerprints hash the body, private header, shared
model-variant header, and `gpu_packets.h`, in that order.
