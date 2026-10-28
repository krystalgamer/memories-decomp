# French MODEL151428 line-to-spark family

This recovers the 4,412-byte entry at offset `0x4` in four French
`MODEL.MRG` images. Images 151428 and 52620 load at `0x8013B000` and use
header 846; images 151438 and 52630 load at `0x8017B000` and use header 943.
The two slot-zero native bodies are identical, as are the two slot-one
bodies. The image headers and unclassified suffixes remain unchanged.

The shared C and relocation wrapper use the authoritative named
`gcc_2_8_1_g0_split` profile. There is no inline assembly, source-local
external declaration, forced register, volatile allocation control,
fabricated dependency, or imported reference type/toolchain claim.
Declarations were recovered from this family's measured offsets and
canonical local interfaces.

## Recovered behavior

Initialization clears 256 pairs of line endpoints, impact flags, impact
animation frames and trail ages. It obtains the opposing slot's dimensions
into the measured eight-byte position buffer, then generates 64 spark
offsets with signed `(rand() - rand()) % 4096` arithmetic, half dimensions,
and division by 4096. Each axis uses its own pair of random calls. It uploads
one texture, initializes ambient intensity to 2048, and selects camera mode 1.

Each update obtains the ordering table and frame step and sets the frame-step
override to 1. Both native camera-mode branches are retained, including the
three elapsed-time stages, slot choices, coefficients and signed camera
parameters. Phase zero dims ambient toward -512; phase two restores it toward
2048 and then enters phase three. The native pre-update threshold tests are
preserved rather than replaced with post-update clamps.

After the configured delay, the first active update derives the Y/Z velocity
from the opposing-minus-active slot position divided by six. Before the
emission boundary, while `live_count + 4 < 256`, four paired endpoints are
emitted with independent X/Z jitter and the selected part's world translation.
Each batch enables one more opposing-slot spark.

Unimpacted endpoints are projected as a colored-to-black `GsGLINE`. Its
submission depth is the signed average of the two projection returns. The
leading endpoint advances while its Y coordinate is negative; the trailing
endpoint begins advancing after the old, post-incremented trail age reaches
four. A leading endpoint reaching nonnegative Y sets the impact flag and
clamps its Y coordinate to zero.

Impacted lines draw up to 16 textured animation frames. Both the native
impact-matrix translation and endpoint addition to the corner vectors are
retained; neither is simplified away. Spark quads use the opposing-slot
matrix, per-batch offsets and their own 16-frame counters, then subtract the
configured rise from the signed-halfword Y offset. Quad submission checks
only nonnegative depth and narrows it to `u16`; no projection-flag gate is
invented.

Atlas U is `(frame / 2 % 4) * 32`, with 31 added for right corners. Top V is
`frame / 8 * 32`, and bottom V adds 31. The supported compiler emits the
original bottom-V shift-by-two/OR-31 instructions from these expressions.

After elapsed time advances, phase three returns 2 once the last live impact
has finished its 16 frames. Strictly after the emission boundary, a phase
below two becomes phase two and returns 1. At or before that boundary, a phase
below two returns 4. Other active updates return zero. The native unchecked
last-element access is preserved, not supplemented with an invented guard.

## Measured layouts and interfaces

The configuration row at slot-zero `0x8013C16C` has stride `0x24`:

| Offset | Fields |
|---|---|
| `0x00` | Three line-color bytes, three spark-color bytes, part byte, unused byte |
| `0x08` | Unsigned spread, unused halfword, quad half-size, spark rise |
| `0x10` | Unsigned delay, emission boundary, two camera-stage boundaries |
| `0x18` | Three signed camera parameters, three unclassified halfwords |

The state has measured extent `0x18A0` (6,304 bytes):

| Offset | Fields |
|---|---|
| `0x0000` | Configuration pointer |
| `0x0004` | 256 pairs of eight-byte `SVECTOR` endpoints |
| `0x1004` | Velocity vector |
| `0x100C` | 64 spark-offset vectors |
| `0x120C` | Unsigned elapsed time |
| `0x1210` | 64 unsigned spark-frame halfwords |
| `0x1290` | 256 unsigned impact-frame halfwords |
| `0x1490` | Phase halfword |
| `0x1492` | 256 unsigned impact flags |
| `0x1692` | 256 unsigned trail ages |
| `0x1892` | Live-count, batch-count and camera-mode halfwords |
| `0x1898` | Signed ambient intensity and packed texture word |

The native loops establish these accessed spans; they do not prove an original
allocation routine or source declaration. Unused vector pads are not assigned.
The packed texture's high halfword is read signed before storage into the
packet's page field, retaining the native `lh`.

The shared eight-byte position buffer is passed through the canonical
`ModelEffectAdjustment *` dimension interface and `u16 *` position-copy
interface. The matrix translations use the canonical SDK output view; native
`RotTrans` writes their three translation words. Types remain in the owning
header and stored pointers use `G32`.

Regional bindings include dimensions at `0x8005AFA4`, camera queueing at
`0x8004F5D8`, ambient at `0x8005C6D0`, frame-step override at `0x8005CBF4`,
`GsSortGLine` at `0x800840B8`, and draw-mode submission at `0x8004D5B8`.
Slot-one relocation changes the three local data identities, not these imports.

## Native experiments and complete-image proof

The adjacent attempt ledger records five frozen source/profile experiments
and four production-image records. Sources, headers, named profiles, native
bodies, object metrics and binary-equivalence claims were audited before
promotion.

The initial independently reconstructed C was 4,384 bytes with a 344-byte
frame. Native address commoning identified SDK comma-expression vector and
packet macros; those restored the original accesses and atlas arithmetic,
leaving a 4,420-byte function and the extra spill. Separating line/quad depth
declarations changed register binding but not that spill. Changing their
scalar spelling to canonical `PSXLONG` was binary-identical to that control.

The decisive expression assigns the completed average of two projection
calls, rather than storing the first return in a user-variable accumulator
that survives the second call. This restores the native temporary reuse,
impact-matrix saved register, short-lived quad depth in `v0`, 336-byte frame,
and all 1,103 instruction words.

Both relocated C bodies match every original native body. Complete production
builds also reproduce all four original 20,480-byte images and preserve each
original-sized C owner:

| Image | Complete-image SHA-256 |
|---|---|
| 151428 | `668a60330641c44376c860b727529f5b787a9424d61b3588f2b5cd4484e36e45` |
| 52620 | `878b52cbdfc657f5045048b7d86ea2b30645c1c40e50ffb5c98e8ace0d40cf34` |
| 151438 | `148e1969335b96b6097dfb1797e66fc9ee41d026cbf2176eebcd35cf78e22121` |
| 52630 | `2c1f578ff8d69e5f072aa7c0d12f7703fac87e333ff640d30a440c337cbf09d1` |

Each module retains its own archive sectors, hash, layout, inventory and
matching manifest. Unrelated root module metadata is preserved. Local work
uses the affected-image identity gate and retained repository policy checks;
cloud validation remains on the pull request.
