# Spanish duel effect 23

The complete `func_80152048` interval, `0x80152048..0x80152EC4`, is
3,708 bytes / 927 instructions. Spanish registration reuses the unchanged
accepted French `src/overlays/duel_effects/effect_23.c` and header with
`gcc_2_8_1_g0_split`, the existing GCC 2.8.1 / MASPSX 2.81 profile.
No second implementation or private replacement SDK types are introduced.

## Independent Spanish acceptance

The independent base is `e9295667f83d3cb83a9b58856dd4f458125b1692`.
Before registration, the shared source plus all 76 accepted Spanish C
entries reproduced the entire 90,112-byte bank with SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Spanish inputs and Spanish resident bindings were used; equal French bank
hashes were not treated as a substitute for this compilation and link.

The additive result is 77/85 bank C functions / 54,280 bytes, or
201/209 configured Spanish C instances / 110,132 bytes. Eight bank
boundaries remain generated assembly. Other pending lifecycle and renderer
PRs are not included in these counts.

An independently recovered local candidate also matched all 927 instructions.
Its signed `DisplayObject` coordinate views, branch directions and distinct
positive origin-step lifetime restored the target's 240-byte stack frame
and final origin branch sequence. The initial unsigned-coordinate candidate
incorrectly eliminated negative-coordinate comparisons; the corrected
candidate was 3,796 bytes. Subsequent conditional forms reached 3,712 bytes,
with one extra instruction at the origin calculation. A separate positive
step local reached exact bytes; reusing the inner loop variable instead
changed allocation to a 232-byte frame and failed.

The latest accepted base already contained the French implementation, which
independently reproduced the same Spanish instructions and full bank.
It was therefore reused unchanged instead of promoting the local candidate.
Its source/header fingerprint is
`2ce67889f65fdaa643ed9590572e4fa20b79bc7be962e06b0bb642663ff8ea00`.
Local immutable experiments and raw comparisons remain under
`tmp/effect23-proof/`; terminal pre-registration proof is under
`tmp/effect-23-full/`. These ignored files are not required build inputs.

## Layout and ownership

Thirty-two target-GCC constants verify the work record, particle stride,
canonical display-object fields and actual table extents.

| Region | Offset | Extent |
|---|---:|---:|
| Position and rotation | `0x000`, `0x008` | Two `SVECTOR`s |
| Five slot origins | `0x010` | 40 bytes |
| Card velocities and rotations | `0x038`, `0x060` | 40 bytes each |
| Particle positions and velocities | `0x088`, `0x100` | Five rows of three vectors each |
| Speed and unobserved halfword | `0x178`, `0x17A` | Two halfwords |
| Stage and swing direction | `0x17C`, `0x17E` | Two halfwords |
| Slot states and particle ages | `0x180`, `0x18A` | Five halfwords each |
| Active, hidden and collected counts | `0x194`, `0x196`, `0x198` | Three halfwords |
| Sweep color | `0x19A` | Four bytes |

The declared work record is `0x19E` bytes with halfword alignment, not
`0x1A0`: it has no pointer or word-aligned member requiring four-byte tail
padding. This measures the accessed record, not an independently inferred
allocator size. The halfword at `0x17A` remains unnamed beyond its offset.

Four unique non-executable generated-data owners retain their original bytes:
the 16-byte scale vector `D_801461A8`, the 84-byte texture union `D_8015B748`,
the 84-byte collected-object array `D_8015B7A0`, and the 8-byte global
offset `D_8015B7F8`. The scale vector ends exactly at the separately owned
effect-2 vector at `0x801461B8`; no overlapping coarse extent is claimed.
Texture pairs 19 and 20 use offsets `0x4C/0x4E` and `0x50/0x52`.

The eight overlay callees are real executable functions with inventoried
extents, not absolute function aliases standing in for code. Resident
model helpers, card collection and sound entry points retain their actual
resident owners. SDK calls resolve through existing public compatibility
names to actual executable SDK inventory entries.
The existing completion-byte alias `D_8009B261` remains at `0x8009C600`,
backed by non-executable resident image storage. It is not newly allocated
overlay data and is distinct from the four section-backed overlay objects.

## Preserved lifecycle

Initialization collects the back-row card objects, classifies signed card
coordinates into five slots, and constructs three particles per slot.
The sweep rises from y=-184, then advances by signed steps of four while
swinging between rotations of -320 and 320. Both turning points retain
sound 36. Activation thresholds remain `70*i-44` and `236-70*i`; the
asymmetric constants are not replaced by a guessed symmetric sweep.

Activated card objects retain additive blending, movement, saturating
15-step color fades and the hidden-flag transition. The low byte of
`card_rotations[i].vx` is deliberately added to all three display rotation
bytes. Initialization still generates all three vector components; using
vy/vz for the latter two updates would change retail behavior.

Particle animation lasts 24 updates, advances the 32-pixel texture frames
every three ticks, and preserves canonical vector accumulation. Completion
with cards requires both the sweep and final collected card to be black;
the empty-row path requires only the sweep color. Neither path is removed.

Production acceptance includes all configured Spanish images and all seven
actual terrain copies, exact C object/ELF extents, accepted-entry preservation,
real data and callee ownership, focused regressions and resident matching.
Boot, MODEL/SU loads and opaque overworld fragments remain open. This match
does not complete the exhaustive runtime or seven-region campaign.
