# Spanish duel effect 2

`func_80153200` owns the complete **2,268-byte** range
`80153200..80153ADC` with `gcc_2_8_1_g0_split`.
The dispatcher supplies a signed halfword alongside the work pointer and
phase. This implementation keeps that value, the zero-value configuration,
positive/negative number colors and all three completion-color predicates.

## Exact recovery

| Attempt | Change | Bytes | Differing words |
| --- | --- | ---: | ---: |
| 1 | Independently recovered lifecycle and local layouts | 2,268 | 3 |
| 2 | Capture the signed value before copying initial scale | 2,268 | 2 |
| 3 | Initialize the packet pointer after that capture | 2,268 | 0 |
| 4 | Express the default configuration as table record six | 2,268 | 0 |

Only the three initial register-setup instructions differed in the first
candidate. The final ordering follows the observed variable lifetimes,
without forced registers, inline assembly or new compiler flags.
Candidate snapshots remain under `tmp/effect-2-probe/`.

The independent full-bank link, recorded in `tmp/effect-2-full/verified.json`,
reproduces all 90,112 bytes, SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.

## Data views and lifecycle

The work view is `0x920` bytes: configuration pointer at zero; three
32-vector rings at `004`; 64 positions, velocities and rotations at `304`,
`504` and `704`; scale/frame at `904`/`908`; stage, number-state, signed
number and crossed-line frame at `90C`/`90E`/`910`/`912`; effect,
background and number colors at `914`/`918`/`91C`.

Each configuration is **50 bytes**. The first RGB triplet is followed by
interleaved negative/positive number-color bytes. Sprite size is at `0A`,
radii at `0C`, four signed heights at `12`, five signed widths at `1A`,
particle speed/count at `24`/`26`, variant at `28`, strip widths/height at
`2A`/`2E`, and an unrecovered final halfword at `30`.

The seven-record selector and observed stride establish **350 bytes** at
`D_8015AF18`. The zero-value target `8015B044` is exactly
`8015AF18 + 6 * 50`: it is the seventh record, not a separate guessed
allocation. Using `&D_8015AF18[6]` preserves the complete linked instruction
bytes and avoids an artificial absolute alias or out-of-bounds six-record
declaration. All seven actual particle counts (32/64/64/32/64/64/64) fit
the recovered arrays. `D_801461B8` remains a real 16-byte scale vector.

The routine preserves the distinct 180-frame crossed-line path, three-stage
textured drawing, signed number display, particles, expanding rings and
optional 64 rotated strips. It keeps the saved frame step, matrix restoration,
separate fades, unsigned scale bound and all observed caller arguments.
The projected-number renderer itself remains an honest assembly fallback.

On independent accepted base `f52efc378`, this adds one complete routine:
**63/85 bank C functions / 25,748 bytes**, **22 assembly boundaries**, and
**187/209 configured-overlay C instances / 81,600 bytes**. All 62 previously
accepted manifest entries remain unchanged. The pending effect-6/geometry
batch is not counted here, and exhaustive runtime coverage remains open.

Production Spanish overlay images, all seven bank copies, metadata and 82
targeted tests pass. Target-GCC layout probes verify both structure sizes and
every listed offset. Object/final-ELF checks verify the complete C function,
all prior C ownership, the exact-size real non-executable input data owners
and their retail bytes, and all 23 distinct overlay callees.
