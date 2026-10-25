# Spanish duel-bank effect 18

`func_80154084` (`80154084..80154688`, 1,540 bytes) is complete matching C
under `gcc_2_8_1_g0_split`. The dispatcher's case 18 supplies the numeric
identity; no speculative card name is assigned.

Phases zero and one select two 30-byte configuration records, initialize
three inner rings, two four-vertex polygons, 16 rotations, 32 particles and
three outer rings. Phase two or greater starts a separate crossed-line path.
That path passes one on its first frame and zero subsequently, finishing
after its 16-bit counter exceeds 180.

Negative phase with no crossed-line counter draws the base polygons. Before
the configured delay it interpolates the color. After the delay it marks the
existing resident request, moves particles, draws/scales rings and sprites,
rotates 16 elements, fades the color and increases scale while below `0x7000`.
Scale is not unconditionally clamped. The saved frame step advances time;
matrix restoration and the final completion predicate remain outside the
color-active branch.

## Particle contour-burst family and semantic bindings

`DuelEffect_RunParticleContourBurst` names native `func_80154084` from its
complete contour/delay/expansion/fade sequence shared with effect 0, plus
32 moving particles and variant-dependent outer-ring rendering. It is not
merely a wrapper around effect 0: the two private configurations, work
layouts, geometry and complete objects remain distinct.

Both `DuelEffect_StartSwords` and `DuelEffect_StartRitual` create effect 18
with phase 1. These independent static consumers rule out a Swords-only
name; they do not establish an artwork identity or prove phase 0 unused.
The ordinary branch signals the current request at the configured delay,
not on every update or only when the entire animation is complete.

The actual NA entry is `80154B00`, JP is `80162118`, and PAL keeps
`80154084`; the complete owner is `604` bytes in each inventory.
`gDuelEffect_ParticleContourBurstConfigs[2]` binds the existing 60-byte
`D_8015B078` owner (NA `8015B144`, JP `80169104`, PAL `8015B078`).
`gDuelEffect_ParticleContourBurstInitialScale` binds the existing 16-byte
SDK VECTOR `D_801461D8` (NA `80146220`, JP `80154230`, PAL `801461D8`),
copied to local scale at entry. Preserve the vector's padding and all bytes
without assigning identity-value or const semantics.

The extern-only header retains original private types and native function
compatibility declarations. Preserve both configurations, the variant
branch, regional 106/98 ring heights, matrix copies, particle/rotation
updates, delay/request handshake and phase-at-least-2 cross fallback.
These are code-supported mechanism names, not new runtime observations.

## Human runtime observation (#418, #7930)

When the human activated Swords of Revealing Light in an NA duel, effect 18
ran with phase 1 for 31 frames and made 17 layered-helper calls. During
that time the card's borders glowed. Effect 21, the falling swords,
followed straight after. This matches the `DuelEffect_StartSwords`
requester. The ritual requester was not exercised, so the glow is one use
of the contour burst and does not justify a card-specific name.

## Layout and storage

The effect-specific work record has size `0x8D4`. Its independently recovered
offsets are:

| Offset | Member |
| --- | --- |
| `000` | Configuration pointer |
| `004` | Three rings of 32 SDK vectors |
| `304` | 16 rotation vectors |
| `384` | Two four-vertex polygons |
| `3C4` / `4C4` | 32 positions / 32 velocities |
| `5C4` | Three outer rings of 32 vectors |
| `8C4` | Crossed-line frame halfword |
| `8C8` / `8CC` | Unsigned scale / frame words |
| `8D0` | SDK color record |

Each configuration is exactly 30 bytes: color at zero, sprite size at 4,
three radii at 6, two widths at `C`, height at `10`, particle speed at `12`,
three outer radii at `14`, variant at `1A` and delay at `1C`. Phase selection
proves two records in `D_8015B078`, with total extent 60 bytes. The initial
16-byte scale vector is `D_801461D8`.

Both objects remain generated data storage with explicit extents, not C
definitions or absolute aliases. The resident request and completion globals
reuse `DuelEffectRequest` and `src/unmatched.h`; the frame-step and SDK calls
reuse their canonical declarations and Spanish bindings.

## Caller-backed helper contracts

The crossed-line helper now declares its caller-supplied `s32 mode`. Its
144-byte implementation deliberately ignores the argument; the caller still
explicitly passes zero or one. Removing those argument writes would not
match the target. The helper's instructions remain unchanged.

The gradient-quad helper's height is an unsigned halfword. Its implementation
only negates that value into 16-bit vertex fields, so the earlier signed
declaration could reproduce the callee while incorrectly generating a signed
load in this caller. The unsigned declaration preserves the callee and the
observed `lhu` argument load.

## Experiments and acceptance

The first complete candidate was 1,536 bytes with 51 differing words. Two
causes remained: the signed helper argument load and separately sequenced
rotation stores. Correcting the height declaration and using the existing
SDK `applyVector` expression produced all 1,540 bytes exactly.

Private snapshots under `tmp/effect-18-probe/` include the accepted base and
tracked source/header patch in their fingerprint. An independent complete
90,112-byte bank link reproduces SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
It includes the changed helper declarations/definitions and preserves their
instruction bytes.

Against accepted `b9dd81974`, all 58 prior Spanish manifest entries remain
unchanged: **59/85 bank C functions / 17,344 bytes**, 26 explicit assembly
boundaries, and **183/209 configured-overlay instances / 73,196 bytes**.
Production Spanish images, all seven Spanish bank copies, metadata and 72
targeted tests pass. All five registered regional overlay builds also match
after the shared helper refinements, including the 48-function North American
bank accepted in #6554. Target-GCC layout checks establish both record sizes
and every accessed field offset; input-object/final-ELF checks establish both
data owners, exact C extents and all 19 distinct overlay callees.
Runtime coverage beyond these configured inventories remains open.
