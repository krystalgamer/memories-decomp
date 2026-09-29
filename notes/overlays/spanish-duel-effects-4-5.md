# Spanish reuse of complete duel effects 4 and 5

This independent Spanish batch reuses the shared project implementations
offered in #6578 and #6579 without changing their source or headers:

| Effect | Source commit | Address | Bytes |
| --- | --- | --- | ---: |
| 4 | `4e18dd596dda645705d33eb81d96520baaae654e` | `80159AAC` | 1848 |
| 5 | `e49b78417a5967fedd2f6641d32067b944564f7c` | `80157E10` | 2504 |

No pending branch history or French registration is imported. Starting from
accepted `fbb7e6b6e`, all 65 Spanish C entries remain unchanged. The two
complete routines add 4,352 bytes, reaching 67/85 bank functions / 31,500
bytes. Eighteen boundaries remain explicit assembly. Configured Spanish
images contain 191/209 C instances / 87,352 bytes, not an exhaustive
runtime inventory. Other pending lifecycle batches are excluded.

## Independent regional proof

Private candidates differed from the shared source only in include paths
needed under `tmp/`. Both compiled with `gcc_2_8_1_g0_split` and reproduced
the complete 90,112-byte Spanish bank before promotion, alongside every
accepted C group and the original assembly fallback. There were no rejected
source/compiler variants in this reuse experiment. Promotion restored the
four original source/header files byte-for-byte.

The production Spanish bank retains SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
The actual Spanish `game/spain/DATA/WA_MRG.MRG` supplies both the independent
proof and all seven terrain-copy checks. A hash-equal French bank was not
substituted for Spanish archive verification.

Every registered C object's text extent, final address, function size and
executable section are checked. The two new objects call 21 and 19 distinct
address-named overlay routines respectively; each has a real inventoried
executable owner. The projected-number helper `func_801566D4` stays assembly
and is not credited as C. Existing resident/SDK bindings, shared contracts
and dispatcher/texture ownership are unchanged.

Four data blocks have unique real generated-object and final-ELF owners,
correct types/extents and exact Spanish retail bytes; none is an absolute
data alias or a replacement allocation:

| Owner | Bytes | Interpretation |
| --- | ---: | --- |
| `D_80146248` | 16 | Effect-4 initial scale |
| `D_8015B704` | 56 | Two 28-byte effect-4 configurations |
| `D_80146218` | 16 | Effect-5 initial scale |
| `D_8015B450` | 360 | Ten 36-byte effect-5 configurations |

Both initial vectors are `{4096,4096,4096,0}`. Target GCC independently
confirms all named work/configuration offsets: work sizes are `0x55C` and
`0xC38`, and configuration sizes are 28 and 36.

## Retail bounds and preserved behavior

Effect 4 uses three 32-point rings, 16 rotations, 24 positions/velocities
and two four-point card rings. Its two configurations select texture pairs
5/6 within the canonical 21-pair owner. Both records specify sprite size 10,
radii 74/80/84, strip widths 1/3, strip height 24, card half-extents 64/32,
particle speed 12 and duration 48. The packet pointer is initialized before
the scale copy to preserve the original instruction ordering.

The two card rings, transparency test, request marker, timed card-to-particle
transition, two scale-matrix updates and separate screen flash remain intact.
Completion depends only on the main color, not all three colors.

Effect 5 has 64-element rising-position, position, velocity, rotation and
rising-color arrays, plus three 32-point rings. All ten complete Spanish
records were decoded: records 5..9 repeat 0..4; strip/rising counts are
16/24/32/40/48, particle counts are 16/24/32/48/64, and numbers are
200/500/1000/2000/5000. Rise speeds are 2..6, with particle and bounce speeds
16. Every variable array count fits, including the `strip_count - 1` access.

Despite repeated configurations, variants 0..4 and 5..9 have distinct
completion paths. The former fade three colors; the latter freeze movement
and rotation once the grounded-number hold counter starts and finish only
after the last rising color clears and the counter reaches 24. Four random
calls per initial rising position, signed rebound division, two complete
matrix copies, capped spawn count and independent frame/tick counters remain.
The existing number-renderer declaration is reused through `effect_6.h`.

The repository-wide CI input-download/hash incident is not bypassed.
Maintainer repair and passing exact-head workflows remain acceptance gates.
Boot ownership, MODEL/SU loads and overworld-tail coverage remain open.
