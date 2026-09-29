# Spanish duel effect 22

## Exact scope

The complete ritual lifecycle at `0x8014A8E4..0x8014C8FC` contributes
8,216 instruction bytes and a separate 228-byte compiler-generated switch
table at `0x80146054..0x80146138`. The named profile is
`gcc_2_8_1_g0_split`, using GCC 2.8.1 and MASPSX 2.81.

An independent pre-registration link retained all 81 accepted Spanish C
entries from `5d69f5746` and replaced only this function and its table.
The complete 90,112-byte bank matches
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Production linking uses the actual C object's `.rodata`, between two raw
header segments; no absolute jump-table alias or preserved copy substitutes
for compiler output. All seven Spanish terrain copies remain required.

The production ELF places the table in a non-executable 228-byte section.
All 57 input relocations resolve into the actual C function, and the
function's HI16/LO16 relocations reference the actual compiler table.
Relocating the compiler's raw table entries reproduces that linked section;
each complete Spanish terrain copy is byte-identical to the production bank.

The additive branch also retains effect 23 accepted in `273e05386`, giving
83/85 bank C functions / 77,012 instruction bytes and
207/209 configured C instances / 132,864 bytes. The table is not counted as
instruction bytes. The separate effect-24 and curve boundaries remain
assembly here. These counts are not exhaustive runtime
completion: boot ownership, MODEL/SU loads and opaque overworld fragments
remain unresolved.

## Recovered layout and real owners

The configuration is 26 bytes; 24 configurations occupy the actual
`D_8015A658` owner through `0x8015A8C8`. RGB triplets precede halfword point
size/spread, two inner widths and height, two outer widths and height,
duration, and the unobserved final halfword. The initial scale vector is
the separate 16-byte `D_80146044` owner.

The work allocation is `0xDB8` bytes. It contains two portal positions,
three card velocities, two four-point rings, 32 points and rotations,
64 inner/outer line endpoints and velocities, 64 particle positions and
velocities, three card rotations, two texture-frame indices, halfword
state/counters, five control colors and 64 particle colors. Forty
target-compiler size/offset constants establish this layout; no reference
project's guessed types or flags are imported.

The 84-byte `D_8015B7A0` list retains its canonical shared declaration.
`Duel_CheckRitual` uses the real Spanish resident function at `0x8002C9BC`,
size `0x150`, and its existing `DuelRitualResult` contract. Texture pairs,
ordering-table and priority data retain their accepted generated owners.

## Preserved lifecycle details

The 57-entry switch covers phases 665 through 721. It selects all 24
configurations; phase 670 selects configuration zero **without** enabling
the cross-line fallback. Other unsupported phases set `cross_frame` before
selecting configuration zero and return without normal initialization.
The fallback completes after its counter exceeds 180.

Two portals and animated flames lead into three cards moving toward
`(0, -96, 0)`, rings, rotating strips, gradient lines and a layered fan.
Signed coordinates are recovered explicitly from the canonical display
storage. Brightness uses the accepted halfword/low-byte packing contract.

Only 32 particle positions are initialized by `func_8014F608`, although
later code reads and updates all 64. The implementation does not expand
that initialization, zero the work buffer or replace the observed behavior.
Random line endpoints share three samples and use the original 328/392
scales; fan acceleration and particle activation retain their thresholds.

The flame frame update deliberately uses separate `if` and `else if`
gates. A combined `||` gate makes GCC hoist the second vertex-base address
out of the outer loop, spill it and enlarge the frame from 384 to 392
bytes. Compiler-pass diagnostics isolated that transformation; splitting
the gate reproduces every instruction without padding, inline assembly,
hard-register variables or new compiler flags.

The outer-strip path calculates an unused half-intensity local color but
passes the original particle color. Completion deliberately tests the same
flash color twice and also requires stage five. Neither is normalized into
a more plausible but different implementation.
