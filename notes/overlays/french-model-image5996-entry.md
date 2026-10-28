# French MODEL image 5996/6006 entry

Part of #6460. The entries at `8013B004` and `8017B004` are each
`0xF40`/3,904 bytes. Both use `gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX
2.81. The source retains address-based symbols; effect descriptions below
do not establish a card or animation identity.

## Measured layout

The configuration at `8013BFA8` has a 30-byte stride: triangle RGB at
0..2, card RGB at 3..5, part index at 6, signed triangle height/width at
8/10, ring radius parameters at 14/16, card extent at 18, burst duration
at 20, triangle count at 22, initial delay at 24, playback-release time
at 26 and burst start at 28. Bytes 7 and 12..13 are not read here.

State offsets are:

| Offset | Observed storage |
|---|---|
| `000` | Stored configuration pointer |
| `004` | 32 eight-byte initial rotations |
| `104` | 32 eight-byte angular velocities |
| `204` | 66 eight-byte ring vertices, two consecutive groups of 33 |
| `414` | Three packed texture results |
| `420` | Byte update counter |
| `421` | Byte notification latch |
| `424` | Signed elapsed time |

The initialized scale vector is at `8013BF44`; three 28-byte `GsIMAGE`
records follow at `8013BF54`. Opposite-slot data addresses add `0x40000`.
Only the first two uploaded textures are read by this entry. Packed results
hold CLUT in the low halfword and TPAGE in the high halfword.

## Behavior

Initialization assigns three `rand() % 4096` rotation components and three
`rand() % 32` velocity components per configured triangle. It then replaces
velocity Z with `32 - velocity X`; the preceding random call remains real
and is preserved. There is no added count clamp.

Two closed 33-point rings use angles `i * 128`, including the repeated
endpoint. The first uses parameter 16 for XY radius and
`-side * parameter14` for Z; the second uses parameter 14 for XY and Z zero.
Elapsed time starts at the negative configured delay. Negative-time updates
only advance by the frame step and return zero.

Before playback release, both slots are held and ambient is set to -1024.
Each triangle rotates by initial angle plus velocity times elapsed time.
It is projected twice: configured size with configured third-vertex RGB,
then half size with white third-vertex RGB. The other two vertex colors
remain black. Submission preserves the native nonnegative depth/flag checks
and draw-mode argument 1.

Before burst start, a textured card follows the configured part. Its extent
is divided by four, its color is 128/128/128 and its uniform scale alternates
4096/4608 from the update counter's low bit. Its ordering-table depth is
biased by -30. Once released, both playback directions become 1.

During the burst interval, a full-extent card is placed at `(0,-350,side*450)`.
Uniform scale grows from 2048 by `elapsed_since_burst * 4096 / duration`;
configured RGB fades by the remaining duration. A second textured pass
projects all 66 ring vertices and submits 32 connecting quads. Each quad's
depth averages both adjacent endpoints on both rings; rejection uses only
the current and next first-ring flags, ORed and masked with `0x20`.
The original divisions and their nonzero-duration preconditions are retained.

The footer advances time and the byte counter. After release but before
burst start it restores ambient to 2048. It returns 2 only when time is
strictly greater than burst start plus duration. Otherwise, at/after burst
start the latch permits return 1 once; the default return is zero.

## Recovery and exactness

The recovered stack frame is 968 bytes. Native code uses stack+232 both for
the fourth card corner and, later, for the 66 projected screen points.
The local union expresses these observed, non-overlapping scratch uses.
The depth, interpolation and flag arrays remain genuine SDK batch outputs.

Real walking cursors recover the native geometry/projection lifetimes.
Cursors advance before loop counters; pulse scale is retained before RGB
writes. The native footer guards completion work and falls through to a
default return, rather than returning zero independently from every arm.
A scalar current-endpoint flag preserves native load/register ownership
without writing an intermediate value to the escaped SDK flag output.

The attempt ledger records one bootstrap declaration failure, nine native
compiler experiments with frozen source/header fingerprints and precise
measured reasons, and both production proofs. The OR-reversal experiment
changes the two load instructions but does not solve their native ordering;
it is not claimed to be byte-identical to its predecessor.

Both complete 20,480-byte production images match their original bytes:

| Sector | SHA-256 |
|---|---|
| 5996 | `07aa4396c788975e4129577a4e8f3da0d8cdfa526c93d741c55817f34ddf3237` |
| 6006 | `e438891a04af46eaa7feca3e898cb18b4a35ab27fef2df0da19bada3615ecc44` |

Each original-sized C function owns `.unmatched_4`. The four-byte loader
header and every byte from `0xF44` onward remain unchanged raw data.
No reference declarations, inline assembly, fixed registers, volatile tricks,
fabricated accesses or arithmetic dependencies are used.
