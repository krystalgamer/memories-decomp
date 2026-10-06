# French MODEL144 sphere particles and part glow

Two independently hashed images are covered: model492, physical record442,
stages9/10, headers144/274, archive sectors122192/122202. Loader command75100
passes initial argument100, selecting descriptor0 through `argument % 100`
and enabling the frame-step override flag. The instance ledger binds both
complete image hashes. No other descriptor is classified.

| Image interval | Owner | Bytes per image |
|---|---|---:|
| `0x0000..0x0004` | Raw module header | 4 |
| `0x0004..0x0EB0` | Matching C entry | 3,756 |
| `0x0EB0..0x0EC0` | C unit-scale `VECTOR` | 16 |
| `0x0EC0..0x0EDC` | Raw `GsIMAGE` record | 28 |
| `0x0EDC..0x5000` | Unclassified raw suffix | 16,676 |

Both complete20,480-byte images were independently linked with four actual
C entry/literal contributions and six disjoint, sized raw owners. They add
7,512 matching instruction bytes and32 literal bytes;33,352 suffix bytes
remain unclassified. Recognizing descriptor0 does not classify its whole
containing suffix or establish exhaustive runtime coverage.

## Native state and phases

All939 entry instructions were read. Each entry has a984-byte frame,
51 ordered direct calls to30 resident destinations, a closed control-flow
graph and no indirect/local calls. All30 actual resident callee bodies were
checked against the exact French resident image and linked ELF.
Types and signatures come from accepted local headers; compilation uses
`gcc_2_8_1_g0_split`, GCC2.8.1 and MASPSX2.81.

The90-byte descriptor starts with three RGB triples and one uninterpreted
byte, then eight signed part indices; radius, half-size, gradient duration,
particle duration, glow fade-in and glow duration; three eight-element arrays
of tint starts, active-window durations and ramp durations; main/glow delays.
Selected RGB is255,255,255 /255,64,64 /128,128,128. Parts are31,32,-1,
then negative sentinels. Radius300, half-size50, durations60/90/80/124,
tint start70, window30, ramp7, main delay164 and glow delay40 are selected.
Both sentinel traversals terminate within their eight-element arrays.

State has a config pointer at0x000,24 opaque bytes at0x004,64 `SVECTOR`
points at0x01C, a signed packed texture word at0x21C, elapsed time at0x220,
started byte at0x224 and override byte at0x225. Natural alignment gives
extent0x228. No vector/type ownership is inferred for the opaque bytes, and
the point pads are unaccessed.

Construction uses three random samples per point:
radius+`(rand()%radius)/2`, then two angles modulo4096. Coordinates are
`r*cos(a)/4096*cos(b)/4096`, `r*sin(a)/4096*cos(b)/4096`,
and `r*sin(b)/4096`, preserving every intermediate signed division.
One image is uploaded. Elapsed/started are cleared; argument>=100 selects
the override flag.

Tint windows use the slot's native animation-frame value, without inventing
a unit conversion. Active windows optionally call `Model_SetFrameStepOverride(1)`
and queue mode2, start RGB96,96,96, end RGB0,0,0, the selected ramp duration
and all eight part indices. The accepted API takes `ModelTintColor` by value.
Native code initializes only b0..b2: both fourth bytes remain uninitialized,
including the start byte that the resident tint path later uses. This native
quirk is preserved, not replaced with invented initialization.

The main-delay gradient stages unsigned16-bit RGB fading over its duration.
It projects(0,-350,direction*450) directly into `POLY_G4.x3/y3`. With
nonnegative flags, four quads retain that shared center and cover the four
screen corners(0,0),(319,0),(0,255),(319,255), including native asymmetric
minus-one edges. Corner colors use half, two-thirds, two-thirds and full RGB.
Negative flags instead produce a full320x256 uniform-color quad.

The particle phase also starts at the main delay. A1x1 `GsBOXF` with
attribute0x50000000 fades using the secondary RGB and scales the64 sphere
points by `(age<<12)/particle_duration`. All projection buffers have64
elements. Boxes use depth>>2 and the native nonnegative-depth check;
projection flags are not tested in this phase.

Part glow begins at its separate delay. The signed packed texture's upper
half uses native `lh`, unlike an unsigned handle. Four vertices lie at
plus/minus half-size and Z=-half-size/2. Atlas U cycles through three32-pixel
columns every two age units; V spans0..31. Third RGB ramps to full strength
over glow fade-in. For each selected part, translation narrows to `SVECTOR`,
rotation is(0,0,age*100), and a textured quad is submitted for nonnegative
depth/flags. This phase does not call `MulMatrix2`.

The frame step is cached at entry, before any override call. After
`PopMatrix`, elapsed advances by that cached step. Negative main age returns0;
age>=particle duration returns2. During the active interval, started is set
once and returns1; subsequent active calls return0, not2 or4.

## Refinement and acceptance

The11-row ledger preserves one compile failure, four paired probes and two
canonical matched rows. Every frozen source/header/layout hash, all41
target-compiled layout values, source-image hash, actual ELF body, literal,
51/30 call signature and complete differing-word list was independently
rechecked before promotion.

| Experiment | Entry/frame | Differing words per slot |
|---|---|---:|
| Initial gradient-corner macro usage | Compilation failed; layout passed | - |
| Accepted three-corner GPU macro | 3,756 /984 | 754 |
| Positive started-byte terminal branch | 3,756 /984 | 87 |
| Shared radius/scale phase scalar | 3,756 /984 | 4 |
| Phase-local atlas U coordinate | 3,756 /984 | 0 |

The positive terminal branch restores the common return-zero block shared
with construction. Reusing the existing phase scalar for sampled radius
and particle scale recovers native scalar lifetimes. A separate short-lived
atlas coordinate resolves the last four words without forcing registers,
adding artificial dependencies or inventing stores. Native colors, branch
semantics, random-call order, buffer bounds and opaque state remain intact.

Regression covers loader selection, image hashes, raw extents, wrapper-only
relocation, native instruction anchors,41 target layout assertions,51/30
resident-call signature and the full ledger. Clean French resident and
all-overlay matching, actual linked ownership and repository policy gates
remain mandatory production acceptance.
