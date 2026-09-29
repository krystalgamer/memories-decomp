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
