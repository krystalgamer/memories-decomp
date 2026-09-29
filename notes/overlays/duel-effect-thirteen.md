# French duel-bank effect thirteen

`func_801503F8`, at `0x801503F8..0x80150E00`, is one complete 2,568-byte
matching C object. This independent French recovery preserves all 63
accepted bank entries and does not incorporate pending matching batches.
The dispatcher already declares the third argument as `s16`; recovery
retains that contract rather than widening it or changing the caller.

## Experiments and complete-image acceptance

All candidates used the existing `gcc_2_8_1_g0_split` profile (GCC 2.8.1,
MASPSX 2.81). Candidate sources, objects, logs and instruction differences
remain under `tmp/coverage-probes/`.

| Candidate | Result |
|---|---|
| 01 | 2,544 bytes: repeated shifts collapse; position/scale registers differ; third argument incorrectly widened |
| 02 | Repeated byte divisions restore 2,568 bytes; 11 words differ in pointer allocation and prologue scheduling |
| 03 | Changing scale/rotation assignment order leaves the same 11 differences |
| 04 | Explicit scale-before-position pointer setup leaves six prologue scheduling differences |
| 05 | Restoring the existing caller-backed `s16` third argument reproduces every byte |

Before promotion, the accepted bank plus candidate reproduced all 90,112
retail bytes with SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Production verification then covered all seven configured French images
and all seven identical terrain bank copies.

The ownership proof checks all 188 configured C symbols (81,364 bytes),
all 30 complete bank C objects, all 14 named overlay routines used by this
source and five real data owners in both their input objects and final ELF.
The additional resident binding is the accepted 12-byte
`Model_GetLightSourceMatrix` at `0x8005C328`.

## Layout, data and retained behavior

Forty-three target-compiled constants verify the 20-byte configuration,
`0xFA0`-byte work structure and all used field/array extents. The work
contains 48 independent eight-point paths, 48 endpoints, per-path angles,
states, ages and colors. Activation advances by two and caps at 48;
path ages cap at seven. All four configurations have height 80; curve
generation always receives the nonzero count eight.

The real owners are the initial scale vector at `0x80146178` (16 bytes),
a separate 21-record image table at `0x8015AC48` (588 bytes), four
configurations at `0x8015AE94` (80 bytes), the canonical texture-word table
at `0x8015B748` (84 bytes), and the canonical offset at `0x8015B7F8`
(8 bytes). The image table equals the primary table's retail bytes but
remains distinct storage. Existing texture/offset symbols are not replaced
with private storage or absolute aliases.

Rendering preserves the evolving curves, endpoint sprites, rotating glow,
optional background, texture blend changes and bouncing negative number.
The displayed number comes from the caller, not the trailing configuration
halfword. Promotion to `s32` before absolute value safely covers every
`s16` input, including -32768. Bounce divisors advance from one through five.

Two unusual behaviors are intentional: the secondary glow divides its red
byte three times, leaving green and blue unchanged; completion checks the
last path color, background and number, but not the glow. Explicit pointer
setup and repeated byte divisions preserve old-GCC allocation and truncation.

The curve generator and number renderer remain assembly. This branch
reaches 64/85 bank C functions and 25,512 matching bytes, with 21 assembly
boundaries remaining. Boot ownership, MODEL/SU loads and the overworld tail
still need investigation; these configured images do not establish
exhaustive runtime completion. Progress snapshots under #443 are separate.

## Accepted baseline integration

Integration through master `c125c88b` preserves all 73 accepted French
entries, including effects 4/5, and the accepted Spanish lifecycle metadata.
Adding only the unchanged effect-13 source/header gives **74/85 bank C
functions / 43,612 bytes**, 11 assembly boundaries, and **198 configured
French C instances / 99,464 bytes**.

Effects 15, 13 and 12 now have contiguous, separately complete C objects;
no object is partially replaced or extended. Both sides' bindings, real
owners, evidence and tests remain present, with the common matrix getter
deduplicated. The accepted authenticated CI input repair is included
without altering input hashes. No pending branch is stacked or counted.

Integration through accepted master `b73bc0b5` additionally preserves
effects 10/7 and all 75 accepted matching entries and inventory rows verbatim.
Only the unchanged reviewed effect-13 source/header is added: **76/85 bank C
functions / 49,320 bytes**, nine assembly boundaries, and **200 configured
French C instances / 105,172 bytes**.

All seven complete French images and bank copies match. The combined proof
checks 200 linked C owners, all 42 complete bank C objects, 14 routine owners,
five real input/final data owners and 43 target layout constants. All 64
focused regressions pass. Accepted shared sources, other regional registrations
and repaired CI remain unchanged; fresh hosted checks are required on this head.
