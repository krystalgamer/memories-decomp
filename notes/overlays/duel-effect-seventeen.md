# French duel-effect 17

`func_80148BA4`, at `80148BA4..80149F90`, is a complete **5,100-byte**
matching C lifecycle. The dispatcher selects it for effect 17. It gathers
field-card display objects, moves and rotates them toward a vortex, hides
completed cards, and draws inward particles, layered quadrants, bursts and
optional curved paths. Address-based names are retained.

This is an independent addition to the accepted 63-function French bank
baseline, not an import of another pending lifecycle branch. The branch has
**64/85 bank functions / 28,044 C bytes** and **188 configured French overlay
C functions / 83,896 bytes**. The 836-byte curve generator at `8014FABC`
remains generated assembly with a real function owner.

## Recovered configuration and storage

Two 16-byte records at `8015A60C` contain:

| Field | Offset | Phase 0 | Phase 1 |
|---|---|---|---|
| Primary RGB | `0` | `96,128,96` | `128,64,64` |
| Burst/path RGB | `3` | `192,255,192` | `255,160,160` |
| Radius | `6` | 160 | 80 |
| Particle spin | `8` | 64 | 96 |
| Height | `A` | 64 | 96 |
| Fan spin | `C` | 8 | 16 |
| Mode | `E` | 0 | 1 |

The initial vector at `80146034` is `(4096,4096,4096,0)`.
Nonnegative phases at least two select the existing cross fallback.
Negative phases update previously initialized work.

The work object is **8,224 bytes / `0x2020`**, not an assumed 8 KiB buffer:

| Region | Offset | Capacity |
|---|---|---|
| Config pointer, center | `000`, `004` | One each |
| Card velocities, rotation increments | `00C`, `0AC` | 20 vectors each |
| Per-card burst vectors | `14C` | 20 pools of 16 vectors |
| Particles, velocities | `B4C`, `D4C` | 64 vectors each |
| Curved paths | `F4C` | 48 paths of eight vectors |
| Path endpoints, saved origin | `1B4C`, `1CCC` | 48 vectors, one vector |
| Path angles, states | `1CD4`, `1D34` | 48 halfwords each |
| Counters and card state | `1D94..1DF8` | Includes 20 states and ages |
| Path ages | `1DF8` | 48 halfwords |
| Swirl, burst colors | `1E58`, `1E5C` | One each |
| Particle, path colors | `1E60`, `1F60` | 64 and 48 colors |

The target compiler verifies every field offset, the complete extent, the
16-byte configuration, canonical 112-byte `DisplayObject`, 52-byte `POLY_GT4`,
and array capacities: **60 layout constants**.

The initial vector, configurations, 21-word collector output at `8015B7A0`,
84-byte texture table, OT pointer, offset vector and depth halfword are
**seven real data owners**. Their complete sizes and bytes are checked in
both input data objects and the final ELF, not supplied by absolute aliases.

## Collector and lifecycle invariants

The canonical resident `Duel_CollectMatchingFieldCardObjects`, French
`8002CB88`, supplies a zero-terminated output:

- Mode zero passes selector -1, collecting at most 20 occupied objects from
  both sides' ten field zones. The center is `(0,-height,0)`.
- Mode one passes selector zero, collecting at most five selected-row objects
  whose `field_68` is zero. Its center includes the dispatcher's saved offset.

The 21-word output includes the terminator; no guessed five-card bound
truncates the first mode. Coordinates at object offsets `30/32` require
explicit signed-halfword reads despite the canonical union's unsigned view.
The Z coordinate already has a signed canonical view.

The original last-card and completed-card indexes are retained, not covered
with invented fallback slots. Their guards are indirect but sufficient:

- Active particles increase only for a nonempty collection. Therefore their
  respawn test can read `card_states[card_count-1]`.
- Burst color starts at zero and becomes nonzero only when a real card
  completes. Its vector address `work+0xCC+(completed<<7)` is exactly
  `card_particles[completed-1]`, inside the 20-by-16 pool.
- Active cards are capped by the collector count. Each card advances 17
  times before its visibility flag `0x40` is cleared, then never advances
  again. Halfword scale and byte rotation arithmetic preserve target wrapping.
- An empty collection skips particles, bursts and paths; after tick 60 the
  swirl's ready stage moves to the restoration stage.

All 27 possible configured collection counts were modeled independently.
Both radii and all 8,191 signed random remainders give **16,382 component
cases**. A component outside the central +/-32 range always has a nonzero
inward velocity and reaches that range within **32 updates**. The model also
checks the 48-by-eight path bounds and final color extent. This is a
target-derived bounds model, not execution or emulation of the lifecycle.

## Preserved details and shared contract

Each of the three fan layers draws four quadrants three times. It performs
its own nine RGB divisions, color transition and `field_1D94` update.
That halfword's purpose is not inferred merely from its 4096 threshold.
The swirl approaches `(primary.r,primary.g,primary.r)`: using red for blue is
intentional target behavior, especially visible in phase one's unequal RGB.
Path age saturates at seven; restarting a faded path resets its color and
age but does not clear its path state.

Random doubling uses an unsigned shift and then the target's signed 32-bit
conversion. This preserves the signed result without left-shifting a negative
signed value, and prevents old GCC from moving doubling into the radius.
Fan multiplication order likewise preserves the target instruction order.

This caller loads brightness as a halfword. The shared `func_8015405C`
declaration and definition therefore accept `u16` arguments, while its body
explicitly truncates each channel to `u8` before packing. Merely widening
the parameters changes the masks and shortens the helper by four bytes.
The final form preserves **the entire accepted color-transition object**:
text, all three function extents, and relocations. All 65,536 grayscale
halfword packing cases are also checked. The declaration does not claim to
recover the original author's exact typedef.

## Experiments and exact acceptance

| Candidate | Result |
|---|---|
| Initial source | 5,088 bytes; unsigned X/Y reads, narrow brightness argument, random reassociation and reversed fan operands differ. |
| Signed coordinates, widened brightness, reordered products | 5,088 bytes; only random scaling differs structurally. Commuting multiplication does not prevent reassociation. |
| Separated random shift | All 5,100 function bytes match, but the whole bank fails because the widened shared helper loses its byte masks. Not promoted. |
| Explicit byte packing with the widened shared signature | Complete function, bank, real owners and shared objects match. Promoted only after those gates pass. |

The named profile remains `gcc_2_8_1_g0_split` with GCC 2.8.1/MASPSX 2.81.
No raw instruction arrays, new profile, imported reference types or hash
changes are used. The matrix getter and collector remain canonical resident
C; `ratan2` reuses the accepted resident SDK binding and remains SDK assembly.

Final verification covers all **26 available configured regional images**,
all seven French bank copies, 188 French linked C owners, all 30 complete
bank C objects and 18 named overlay routine owners. The 37 focused
bank/dispatch/progress regressions and staged repository policies pass.

A separate private Spanish-bank build preserves all 65 existing C entries.
It uses legally available French bytes only after independently checking
their equality to the configured Spanish module hash. Its dispatcher and
color-transition objects equal their accepted and French counterparts.
**The unavailable Spanish archive and its copies are not claimed verified.**
No Spanish C registration is added.

Twenty-one bank assembly boundaries remain on this independent branch.
Boot ownership, MODEL/SU loads and overworld-tail scope still need research;
this match does not claim exhaustive French runtime completion.

## Accepted baseline integration

The additive refresh onto accepted `3ed6b0f15` preserves all 77 accepted
French entries and inventory rows. It adds only the reviewed effect-17
registration and its measured owners; the entire source/header tree,
including widened brightness packing and later dispatcher lifecycle
contracts, is byte-identical to accepted master.

The combined bank has 78/85 C functions / 58,128 bytes, seven explicit
assembly boundaries, and 202/209 configured French C instances /
113,980 bytes. Complete French image/terrain-copy gates, all linked C
owners and whole objects, real-data owners, target layout constants,
duel/progress regressions and metadata policies are repeated on this
integration. The original recovery counts above remain historical;
neither those counts nor another head's CI substitutes for this validation.

### Accepted effect 24

The further additive integration of `0832a2be4` preserves all 78 accepted
French rows, including effect 24, all PAL-wrapper/contour bindings and the
accepted brightness contract. It adds only effect 17 for 79/85 bank C
functions / 62,084 bytes and 203 configured C instances / 117,936 bytes.
The complete source/header tree remains identical to accepted master.
Current-head French image, ownership, target-layout, duel/progress and
metadata gates are repeated; other pending matches are not stacked or counted.

### Accepted effect 11

The additive integration of accepted `4329554d9` preserves all 79 accepted
French entries and adds only reviewed effect 17. The complete shared
C/header tree, brightness contract, current wrappers and measured owners
remain unchanged. Combined totals are 80/85 bank functions / 67,132 C bytes,
204/209 configured instances / 122,984 C bytes and 47 whole bank C objects.
Complete-image, real-owner, target-layout and combined regression gates are
repeated on this head; five bank boundaries remain assembly.
