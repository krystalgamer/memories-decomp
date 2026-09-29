# Complete Spanish duel effect 9

`func_801593A8` at `801593A8..80159AAC` is a complete 1,796-byte C
translation unit using `gcc_2_8_1_g0_split`. Independent complete-bank and
production Spanish overlay links reproduce all 90,112 bytes and SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
All 65 previously accepted Spanish manifest entries remain unchanged.

This independently based branch reaches 66/85 C functions and 28,944 C bytes,
with 19 explicit assembly boundaries. Pending effects 2/21, 8/12 and 15/1
are not stacked into this branch or counted in its totals.

## Recovered storage and bounds

Target-GCC layout verification fixes the work view at `0x740` bytes:

| Offset | Field |
| --- | --- |
| `000` | Configuration pointer |
| `004` | Negative-number position |
| `00C` / `10C` | 32 particle positions / velocities |
| `20C` | Three rings of 32 vectors |
| `50C` | 64 rotation vectors |
| `70C` | Positive-number position |
| `714` / `716` | Signed positive / negative vertical steps |
| `718` / `71C` / `720` | Scale / frame / tick words |
| `724` / `726` | Negative / positive number modes |
| `728` / `72A` / `72C` / `72E` | Stage / bounce count / unused field / cross counter |
| `730` / `734` / `738` / `73C` | Negative / background / effects / positive colors |

The five configuration records at `8015B650` have a 36-byte stride and a real
180-byte generated data owner. The initial vector at `80146238` has a real
16-byte owner. Both retain original bytes, exact ELF object sizes and unique
input-object storage rather than absolute aliases.

The records request 16/24/32/40/48 rotations, all fitting the observed
64-element span. Every bounce speed is 16; displayed values are
200/500/1000/2000/5000, so both signed number paths stay within the recovered
value domain. Particle and circle counts are fixed at 32.
Two unread configuration fields remain explicitly offset-named to preserve the
observed stride without inventing their meaning.

## Lifecycle and shared declarations

Five ordinary positive phases initialize their record; larger phases select
the crossed-line fallback. Normal rendering retains all four color paths:
background fading, signed bouncing negative number, departing positive number,
and the ring/strip/particle effects.

The first impact copies the negative vertical step to the positive path,
initializes the rebound and marks stage one. Later rebounds increment the
counter before signed division, with divisors two through five. The positive
number keeps the signed 200-unit boundary and its separate clearing branch.
Stage one triggers canonical `SD_SEPlayFull(26)` at Spanish `80040204`,
marks the existing resident request, resets two colors and advances to stage
two. All four completion colors must finish before the resident completion
byte is raised.

The unchanged `func_801566D4` prototype moves from effect 6's private lifecycle
header to the existing shared `drawing_helpers.h`, now that a second recovered
caller uses it. Effect 6 and the shared declaration retain their original
types and exact bytes. The 1,024-byte projected-number helper itself remains
explicitly unmatched assembly; this addition makes no claim to its C ownership.

## Experiments and acceptance

The first candidate was 1,800 bytes with 256 differing words. Moving the
first-rebound assignment before the stage-one store yielded all 1,796 bytes
with zero differences, using the same named compiler profile. Candidate
snapshots, fingerprints and the independent-bank receipt remain local under
`tmp/`.

Production verification checks every current C object/final-ELF function
extent and executable type, the two data owners, all recovered field offsets,
all 19 real overlay callees and actual configuration bounds. Retained effect 6
also passes its layout/data/callee checks after the declaration move.

Spanish, French, English PAL, Japanese and North American overlay builds pass,
as do Spanish duplicate-copy verification, repository metadata policy,
accepted dispatcher/texture ownership and all 106 duel-focused tests.

## Accepted effects 8/12 integration

After additive rebase onto accepted `3d43de926`, all 67 accepted Spanish
manifest entries and the reviewed lifecycle sources remain unchanged.
The combined branch has 68/85 C functions / 31,672 bytes,
17 explicit assembly boundaries and 192/209 configured C
instances / 87,524 bytes. Accepted effect-8/12 sources, helper
contracts, bindings, configurations and tests are retained.

## Independent French registration

The French registration reuses the accepted source and header at master
`a34d452e` byte-for-byte, after Spanish recovery was accepted. An independent
French trial links the entire 90,112-byte bank before changing its inventory;
the complete image retains the SHA-256 above. The new object contains exactly
the one 1,796-byte routine at `801593A8..80159AAC`, not an interior slice or
partially covered translation unit.

All 71 previously accepted French entries remain unchanged. This independent
batch reaches **72/85 bank C functions / 38,488 bytes**, with 13 assembly
boundaries, and **196 configured French C instances / 94,340 bytes**.
Pending effect-4 and other French batches are neither stacked nor counted.
The seven bank copies are at sectors `7193 + terrain * 240`, 44 sectors each.

Production checks cover all seven configured French images, their exact-size
linked C owners and every complete bank C object. The lifecycle and its 19
overlay callees resolve to real executable sections. The initial vector and
five configuration records have exact-size generated-input and final-ELF
data owners, with unchanged retail bytes. Target-GCC layout checks cover both
complete local types, every named field, the signed-step widths and fixed
array capacities.

All five French records retain rotation counts 16/24/32/40/48, bounce speed
16 and signed number magnitudes 200/500/1000/2000/5000. The 32-particle loops
and three 32-vector rings stay within their declared spans. Divisors remain
two through five after the bounce counter increment. The unused configuration
words are preserved without assigning them new semantics.

`SD_SEPlayFull` binds to the existing French resident C owner at `80040204`
with its 40-byte extent; no resident source or ownership changes. The shared
number-renderer declaration remains `s32`, and its 1,024-byte implementation
remains unmatched assembly. This is a complete effect-9 match, not a claim
that the number renderer or broader runtime census is finished. Boot,
MODEL/SU and overworld-tail coverage remain open.

### Accepted French baseline integration

The additive refresh onto accepted `3ed6b0f15` preserves all 77 accepted
French entries and inventory rows, including effects 4, 5, 10, 7, 13 and 23.
Only the reviewed effect-9 registration and measured owners are added;
the complete source/header tree is byte-identical to accepted master.
The combined branch has 78/85 bank C functions / 54,824 bytes, seven
explicit assembly boundaries, and 202/209 configured C instances /
110,676 bytes. Other pending French matches are not counted.

Complete French images and terrain copies, linked C and generated-data
owners, whole-object coverage, target layout constants, duel/progress
regressions and metadata policies are repeated on this integration.
The prior French counts describe the original independent recovery and
do not substitute for current-head checks.

### Accepted French effect 24 integration

The additive merge of accepted `0832a2be4` preserves all 78 accepted French
entries, including effect 24 and current PAL-wrapper/contour bindings.
The complete shared C/header tree remains identical to this accepted base;
only the reviewed effect-9 registration is added. The combined branch has
79/85 bank C functions / 58,780 bytes, six assembly boundaries, and
203/209 configured instances / 114,632 bytes. Production checks repeat
complete images, all 46 bank C input objects, actual owners, target layouts
and the combined duel/progress regressions without counting pending PRs.

The subsequent integration of accepted `4329554d9` preserves all 79
accepted entries, including effect 11, with the same unchanged shared
source/header tree. The independent effect-9 branch now has 80/85 bank
functions / 63,828 C bytes and 204/209 configured instances / 119,680 bytes.
All 47 complete bank objects and five remaining assembly boundaries are
accounted for; the other pending French matches are not stacked.
