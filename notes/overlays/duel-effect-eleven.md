# French effect eleven: 28-piece breakup

`func_80146760`, `80146760..80147B18`, is a complete **5,048-byte** matching C
lifecycle under `gcc_2_8_1_g0_split` (GCC 2.8.1 / MASPSX 2.81). It initializes
a four-by-seven textured grid, throws and rotates its pieces, then replaces
each with fading fragments. Central rays, dust, a tapered gradient and two
scaled ring passes accompany the breakup.

## Shared mode/image storage: a real overlap

The first whole-bank candidate matched every byte but **failed the data-owner
gate**. The dispatcher reads 21 mode halfwords from `8015A430`, ending at
`8015A45A`. Effect eleven reads five 28-byte images beginning at `8015A458`.
Those ranges overlap by two bytes: mode 20 is the low halfword of the first
image's `pmode`, whose full word is nine.

Declaring both independent owners caused Splat to give `D_8015A458` only two
bytes and put the remaining image bytes under an unreferenced pad label.
Whole-image identity alone did not detect this false ownership.

The canonical `image_inputs.h` now declares a **180-byte union at
`D_8015A430`**: `modes[21]` at zero and five images at offset 40, after the
20 non-overlapping halfwords. The dispatcher uses `.modes`; this lifecycle
takes a pointer into `.variant.images`. There is no standalone image alias
or invented allocation. French and Spanish symbol maps both describe the
real owner as `0xB4` bytes. Both input `.data` objects and linked ELFs have
the complete owner, with all bytes unchanged.

The previously accepted dispatcher's complete object text, function extent
and relocations remain identical. Its 21 iterations are not shortened to
hide the overlap.

## Configuration and work layout

The single 48-byte configuration at `8015A4E4` supplies initial fragment
size 8, radial speed 4, lift 16, initial random denominator 8 and spread 16.
The unused halfword at `+0xA` contains 360; this function does not consume
it, so no duration semantics are asserted. The glow is `(128,96,32)`.
There are 32 rays and 32 dust particles, within their 32/64-vector capacities.
The gradient narrows from 64 to 12 by eights and grows from 16 to 128 by
sixteens; three rings use radii 8/16/32 and height spacing eight.

The target-compiled work extent is **7,976 bytes / `0x1F28`**:

| Offset | Storage |
| --- | --- |
| `000` | Configuration pointer |
| `004`, `0E4`, `1C4`, `2A4` | Four-by-seven position, velocity, rotation and rotation-step vectors |
| `384` | Four-by-seven-by-sixteen fragment vectors |
| `1184` | 32 rays |
| `1284`, `1484` | Separate 64-vector dust position/velocity pools |
| `1684`, `1984`, `198C` | Three 32-vector rings, origin, selected UV pair |
| `1990`, `19C8`, `1A00` | Four-by-seven states/frames and 448 fragment sizes |
| `1D80`, `1DB8`, `1DF0`, `1DF2` | Random denominators, angle sums, completed count, per-piece modes |
| `1E2C`, `1E30`, `1E34` | Ring scale, tick, fallback counter |
| `1E36`, `1E38`, `1E3A`, `1E3E` | Gradient dimensions and two texture-page/CLUT pairs |
| `1E42`, `1EB2`, `1F22` | 28 piece colours, 28 fragment colours, glow |

Nonnegative phase selects its low four bits. Values 0..4 select the five
images; other values enter the established 180-update cross fallback.
Initial per-piece mode is one only when `phase >> 7` equals one. No guessed
mode normalization changes that test.

## Motion and lifecycle details

The grid starts at `x = column*12-18`, `z = row*8-24`, then receives the
dispatcher-provided origin. Independent random components perturb its
velocities. Signed `ratan2` calls derive rotation steps; rotations retain
signed remainders and the angle sum narrows to a halfword before its mode
toggle test. X/Z advance only on even ticks, while Y advances each update.

The separate negative/positive vertical-velocity branches, each with its
own gravity increment of three, are intentional matching structure.
Factoring gravity out collapses the branches and changes loop-counter
register allocation. Dust's zero gravity update is also retained: GCC
keeps the original halfword read even though no value changes.

The random fragmentation denominator starts at eight. An unsuccessful
choice decrements it; at one the remainder must be zero, so no live path
decrements it to zero. Ground contact also starts fragmentation. Up to
16 fragments may be active per piece; colours fade by 31 and stop further
visits once zero. All four initial size possibilities, 8..11, remain valid
through their live draws. The smallest first fragment reaches zero on its
last draw, then wraps on the final unsigned decrement; that wrapped size
is never read again. The source does not incorrectly clamp this behavior.

The central glow fades after both gradient dimensions reach their limits.
Its two ring passes leave their scale in the local vector; the following
piece draws retain that scale rather than resetting it. Completion requires
exactly 28 finished pieces and zero central glow, not just a frame timeout.

## Caller-backed helper contracts

The ring-height argument at `80146CE0..80146CE8` is explicitly sign-extended,
so `func_8014EE0C` takes `s16 height`. The ray width at `80146E30` is loaded
unsigned, so `func_80156E58` takes `u16 width`. Each declaration and definition
changes together. Their complete grouped object text, function extents and
relocations are identical to the accepted versions.

`Model_GetLightSourceMatrix = 8005C328` remains the accepted 12-byte resident
C function. `ratan2 = 80089928` comes from the accepted French resident
linker map, with its 372-byte inventory boundary remaining `sdk_asm`.
No SDK function is promoted to game C.

## Experiments and acceptance

1. 4,872 bytes: factored gravity collapsed the two motion branches; the
   boolean cross-call argument also differed.
2. 5,040 bytes: branch-local gravity restored motion and allocation; the
   cross selector remained eight bytes short.
3. 5,048 exact bytes and an exact bank, but the overlapping data-owner
   check failed. This candidate was **not promoted**.
4. 5,052 bytes: the canonical union fixed storage, but repeated subobject
   indexing reconstructed its base after the image helper call.
5. **5,048 exact bytes**: retaining the selected `GsIMAGE *` preserves both
   the union-backed ownership and the target's pointer reuse.

The final independent full-bank and real-owner proof preceded promotion.
All 63 accepted French bank entries are preserved. This branch contains
**64/85 bank C / 27,992 bytes**, **188 configured C / 83,844 bytes**.
All seven French images and seven terrain copies remain exact; all 30
bank C objects and 23 named overlay routine owners were checked.

Five complete generated-data owners were checked against retail bytes in
input objects and the final ELF: initial scale `80146004/16`, shared inputs
`8015A430/180`, configuration `8015A4E4/48`, output texture pairs
`8015B748/84` and origin `8015B7F8/8`. Target GCC verifies 74 layout constants,
including both views of the overlapping halfword at offset 40.

The final available-region gates cover 26 complete North American, Japanese,
English PAL and French images. A separate Spanish bank build preserves
all 65 existing C entries and matches its independently configured hash
using the legally available identical French module bytes. Its dispatcher
and both helper objects are identical to the accepted French objects,
including relocations. **Spanish archive/copy verification is not claimed**:
the Spanish archive is absent locally.

The target-derived bounds model checks all five image records, 512 phase
cases, all 28 emitters and four fragment-size possibilities, denominator
safety, terminal unread size wraps, and the central glow's 22 updates.
It is not execution of the C lifecycle or a claim about every caller's
trajectory. Full-image identity remains the acceptance gate.

Twenty-one bank assembly boundaries, boot ownership, MODEL/SU loads and
overworld-tail coverage remain unresolved. This is not an exhaustive
French completion claim.

## Accepted baseline integration

The additive refresh onto accepted `3ed6b0f15` preserves all 77 accepted
French entries and inventory rows, adding only the reviewed effect-11
registration. The full source/header tree is byte-identical to accepted
master; already accepted image-input ownership, dispatcher contracts and
later lifecycle headers are not replaced by their older reviewed versions.

The combined French bank has 78/85 C functions / 58,076 bytes, seven
explicit assembly boundaries, and 202/209 configured C instances /
113,928 bytes. Complete French image and terrain-copy matching, linked
function and real-data ownership, whole-object coverage, target layout
constants, duel/progress regressions and metadata policies are repeated
on this integration. The preceding counts and input caveats describe the
initial recovery, not a replacement for current-head validation.
