# Spanish MODEL variant 408/558

Model 54 is compact record 54 in the independently verified Spanish
`DATA/MODEL.MRG`. Its second variant occupies record-relative sectors
200..209 and 210..219, selected by loader stages 9 and 10.

| Stage | Slot | Header | First sector | Load address | Complete image SHA-256 |
|---|---:|---:|---:|---|---|
| 9 | 0 | 408 | 15104 | `0x8013B000` | `2a07c9091bdb4ee9cb0df4fea1657b6b927fe0f796a474b5ca78c2079c19678a` |
| 10 | 1 | 558 | 15114 | `0x8017B000` | `474194e9d9a86b19da1d24b771954b542ac05317f64fefc2e7cb2b12526ee82c` |

Each complete image is 20,480 bytes. The three compiler-owned functions
cover 5,200 instruction bytes per slot:

| Relative interval | Bytes | Source |
|---|---:|---|
| `0x4..0x93C` | 2360 | `variant408_entry.c` |
| `0x93C..0x102C` | 1776 | `variant408_update.c` |
| `0x102C..0x1454` | 1064 | `variant408_draw.c` |

The slot-one wrappers rename the actual function and data symbols. Each
function has its own selected compiler object, using the authoritative
`gcc_2_8_1_g0_split` profile (GCC 2.8.1 / MASPSX 2.81). The source was
independently recovered from the local retail instructions, not imported
from reference types, flags, inline assembly or instruction arrays.

## Loader and state evidence

The metadata command words at `0x110` are `576000`, `574000`, and `-2`.
`model_control.c` selects command position 1 for this variant and passes
`574000 % 1000`, hence configuration index zero. Updates pass `-1`.
The 36-byte descriptor at module offset `0x1550` selects model part 3 and
contains timing values 50, 60, and 180 at offsets 12, 16, and 20.
This is evidence for the accessed record, not the terminal table bound.

The independent state views preserve a 1,412-byte mesh followed by four
412-byte rings. The mesh has 17 rows of nine eight-byte `SVECTOR`s, nine
colors at `0x4C8`, scale at `0x4EC`, phases at `0x4FC`, and repeats at
`0x540`. A ring has two four-by-six point banks, color at 384, position
at 388, phase at 404, and repeats at 408.

The full view has a canonical 24-byte `POLY_F4` at `0xBF4`, then four
unclassified bytes before `POLY_G4` at `0xC10`. It retains two `POLY_GT4`s
at `0xC34`, two further packets at `0xC9C`/`0xCD0`, and two `POLY_FT4`s
at `0xD04`. The line, matrix, target, delta, screen delta and view delta
begin at `0xD64`, `0xD78`, `0xD98`, `0xDA0`, `0xDB0` and `0xDB4`.
Configuration/model-part pointers are at `0xDDC`/`0xDE4`; state, fade,
brightness, slot and command are at `0xE00`, `0xE04`, `0xE10`, `0xE14`
and `0xE16`. Target-compiler probes check 112 layout values across the
three views. The 3,608-byte state size is not itself proof of the caller's
allocation bound.

The entry initializes packet fields and geometry, updates camera/projection
state, invokes the mesh/ring helpers and returns lifecycle values to the
resident controller. The incoming pointer is also the ring point cursor
in the initialization branch; the separate state pointer remains stable.
All pointers use the measured 32-bit target ABI. The drawing helper emits
four rings of four-by-six gradient lines; the mesh helper updates a tubular
17-by-nine point grid and emits 16-by-eight gradient quads.

## Data ownership and honest limits

Four entry-accessed `GsIMAGE`s have real 28-byte owners at offsets `0x1470`,
`0x14E0`, `0x1518` and `0x1534`. Configuration zero has a real 36-byte owner
at `0x1550`. No absolute linker aliases replace these owners.
The image-array declaration is deliberately unsized: accessing indices
0, 4, 6 and 7 does not classify intervening records or prove a terminal bound.

`func_80059A50` copies the image descriptor and calls
`ModelTexture_PackPageClut`. It packs page/CLUT coordinates; it does **not**
upload pixels or palette data. The four accessed records have null pixel
and CLUT pointers. The other three wrapper return values are unused.

All data remains generated from the immutable retail image, not C-owned
reconstructions. Separately named unknown intervals cover
`0x1454..0x1470`, `0x148C..0x14E0`, `0x14FC..0x1518`, and
`0x1574..0x5000`: 15,128 bytes per image, explicitly unclassified.
The four-byte module header is also preserved data, not C.
No coverage is inferred for other headers, records, stages or regions.

## Recovery and acceptance evidence

The [attempt ledger](spanish-model-variant408-attempts.csv) records the
materially distinct compiler experiments, fingerprints and mismatch reasons.
Candidate sources, object files, raw image material and disassembly remain
local under `tmp/`; they are not publication artifacts.

Critical source forms must not be simplified without a complete match:

- Reusing the `packed_texture` temporary across all four coordinate-packing
  calls preserves the first result's page/CLUT lifetimes. Later assignments
  are dead but affect old-GCC register allocation. Explicit low-half masks
  introduced an extra instruction; distinct never-overwritten temporaries
  moved extraction too late.
- Index-first 32-bit point-address arithmetic reproduces the inner-loop
  operand order and avoids an additional induction pointer. Target probes
  prove point/row sizes; loop bounds stay within the recovered arrays.
- The update helper's point assignment in the first store lvalue preserves
  address-calculation scheduling after the trigonometric call.
- Branch-local target initialization, packed projection outputs and
  separate slot-copy call branches preserve the original load/store order.

Scratch acceptance covered both complete images, six function instances,
112 target layout values, 32 real resident callee owners, and the identified
data symbols. Production verification separately checks the selected
compiler objects in each generated linker script, exact input/final function
extents, all known and unknown data-owner sizes/bytes/sections, full-image
hashes and the resident callees against retail bytes. This distinguishes
compiler-owned instructions from preserved storage; a hash alone is not
the ownership proof.

The configured Spanish totals become 30 representative images,
239/240 matching C instances and 165,136 instruction bytes. The outstanding
duel-bank curve remains visible, and all other runtime loads, MODEL variants
and unknown tails remain in scope.
