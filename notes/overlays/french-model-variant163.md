# French MODEL163 particle grids and expanding ring

Two independently hashed runtime images are covered: model37, stages9/10,
headers163/293, archive sectors10412/10422. The loader command is94000 and
the selected descriptor index is `command % 100`, namely0. The instance
ledger records complete image identities; no other descriptor is classified.

| Image interval | Owner | Bytes per image |
|---|---|---:|
| `0x0000..0x0004` | Raw module header | 4 |
| `0x0004..0x0EA8` | Matching C entry | 3,748 |
| `0x0EA8..0x0EB8` | C unit-scale `VECTOR` | 16 |
| `0x0EB8..0x0EF0` | Two raw `GsIMAGE` records | 56 |
| `0x0EF0..0x5000` | Unclassified raw suffix | 16,656 |

The two complete20,480-byte images were linked from actual C entry/literal
objects and disjoint, sized raw owners. Four C contributions and six raw
contributions cover the images exactly. This adds7,496 matching instruction
bytes and32 source-literal bytes. The33,312 suffix bytes remain unclassified;
recognizing the selected descriptor does not classify the rest of that tail.

## Independently recovered state and behavior

All937 entry instructions were inspected. Each entry has a960-byte frame,
53 ordered direct calls to26 resident destinations, a closed local control
flow graph, and no indirect/local function calls. All26 actual resident
callee bodies were compared with the verified French resident executable
and its linked ELF. Declarations use accepted local headers and the
authoritative `gcc_2_8_1_g0_split` profile, GCC2.8.1 with MASPSX2.81.

The24-byte descriptor consists of primary/secondary RGB, two attachment-part
bytes, then signed radius, thickness, stagger, particle duration, split time,
flash duration, ring duration and delay. Selected values are:

```text
128,48,48,192,192,96,26,21,200,50,4,120,76,40,90,160
```

The state spans0x464 bytes. A single array of130 `SVECTOR` records starts
at0x004: particles0..47, attachment endpoints/midpoints48..53,
grid54..62, inner ring63..95, outer ring96..128, and anchor129.
Sixteen quad pointers start at0x414; two unsigned packed texture handles
at0x454; completion byte at0x45C; elapsed time at0x460.
The three intervening bytes remain uninterpreted.

The constructor clears96 entire vectors, including pad, crossing logical
group boundaries within that single array. It then constructs the3x3 grid
and two33-point rings, uploads two images, and resets elapsed/completion.
The grid's four pointer groups are54,55,57,58;56,55,59,58;
62,61,59,58;60,61,57,58. Outer radius is radius+thickness and
outer Z is `-direction * thickness`. Direction is active slot times2 minus1.

Before the delay expires, two pairs of attachment transforms populate
records48..53. Each midpoint first adds coordinates with signed16-bit
narrowing, then divides the already-narrowed result by2. This is not a
widened average. Particle pad stores its X rotation; other unaccessed pads
are not assigned invented semantics.

Each particle's phase is elapsed-delay-index*stagger. Negative phases copy
the first midpoint and derive rotation from the difference between midpoints
using `ratan2`. Active particles scale by time*3584/duration+512 before
the split and `(time << 12)/duration+1536` afterwards. They draw four grid
quads, plus another four before the split, through `RotAverage4`; submission
requires nonnegative depth and flags. Their Z update applies throughout
the active phase, not only before the split.

The ring phase is elapsed-delay-split time. Negative phases capture particle0
as anchor129. During ring duration,66 vectors beginning at63 are projected
and32 quads connect indices i,i+1,i+33,i+34. Color fades with remaining
ring duration; scale is `(time << 13)/ring_duration+4096`. The projected
position/depth/interpolation/flag arrays each have66 elements, not68;
native stack padding is not evidence of extra elements.

The320x256 flash uses the same phase, bounded by flash duration, but its
brightness denominator is **split time**, not flash duration. After
`PopMatrix`, elapsed advances by the frame step cached at entry.
Completion returns1 once, then2, after elapsed-delay-stagger*48 reaches
particle duration. Otherwise the entry returns4 once elapsed-delay reaches
particle duration, or0 earlier.

## Refinement and acceptance evidence

Seven paired experiments are preserved in the attempt ledger, followed by
two canonical matched rows. Every frozen source/header/layout fingerprint,
target-compiled layout, image hash, actual ELF body, literal, call signature,
and full differing-word list was independently rechecked before integration.
Equal mismatch counts were not treated as evidence of identical artifacts.

| Experiment | Entry bytes | Frame | Differing words per slot |
|---|---:|---:|---:|
| Single-origin grid and rings | 3,740 | 952 | 594 |
| Block-local flash brightness | 3,740 | 952 | 594 |
| Flat quad pointer indexing | 3,928 | 960 | 906 |
| Explicit quad pointer traversal | 3,760 | 960 | 487 |
| Stage primary RGB loads | 3,748 | 960 | 3 |
| Index-before-stagger multiplication | 3,748 | 960 | 2 |
| Unsigned packed texture handles | 3,748 | 960 | 0 |

The explicit cursor traverses four real pointers per quad. Its canonical
`SVECTOR *G32 *` declaration marks the guest-pointer storage it traverses;
this portability annotation does not change native instructions.
Staging all three
primary RGB byte loads before any packet color store preserves native
alias-sensitive evaluation order and removes three load-delay NOPs.
The multiply operands follow the native index-before-stagger order.
Native `lhu` instructions at0x8013B3C0 and0x8013B9E4 establish unsigned
upper-half extraction from the packed texture handles. No forced registers,
volatile barriers, artificial dependencies, dummy arrays or fake stores are
used.

The regression binds image hashes, loader selection, raw extents, wrappers,
native instruction anchors, all32 target layout values,53/26 resident-call
signature and the complete16-row attempt history. Clean resident and
all-overlay exact matching remain the production acceptance gates.
