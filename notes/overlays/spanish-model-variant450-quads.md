# Spanish MODEL450 quad helper

The `27C0..2D88` function in both MODEL174 stages 9/10 is independently
matched as 1,480 bytes of C using `gcc_2_8_1_g0_split`. It adds two C
instances / 2,960 instruction bytes without adding images or function
boundaries. The accepted 900-byte line helpers are unchanged.
See the [family inventory](spanish-model-variant450.md) for all seven
functions and the remaining ten assembly instances / 24,544 bytes.
All 11,648 unclassified suffix bytes remain in scope.

## Regional comparison

At accepted cutoff `7d62f03d119b0452873c0b037f18de9a26c415db`, the
5,106 registered overlay C spans contained no exact or same-instruction-shape
donor for the six remaining PAL functions. The seven PAL spans each occur
in both slots of all five PAL releases, with four exact body variants;
their within-slot differences are only direct jump/call destinations.

The same MODEL174 stages in North America (headers 433/583) and Japan
(432/582) contain seven differently compiled closed functions. All 42
Spanish/US/Japanese function slices were independently hash-verified.
Their structural ordinal pairings are:

| PAL offset / bytes | NTSC offset / bytes |
| --- | --- |
| `4` / 3540 | `4` / 3544 |
| `DD8` / 2492 | `DDC` / 2452 |
| `1794` / 1848 | `1770` / 1804 |
| `1ECC` / 2292 | `1E7C` / 2264 |
| `27C0` / 1480 | `2754` / 1484 |
| `2D88` / 900 | `2D20` / 904 |
| `310C` / 2100 | `30A8` / 2100 |

No registered C shape donor was found for those NTSC bodies either.
Ordinal and instruction-shape similarities are leads, not semantic proofs.
This helper is not claimed to be region-exclusive; each differing release
still needs its own exact compiler and ownership verification.

## Measured views and behavior

Seven 160-byte groups begin at context `3598`. Each contains sixteen
canonical `SVECTOR`s, endpoint colors at `80/84`, signed scale at `88`,
fading at `98`, and brightness at `9C`. The shared `POLY_GT4` at `415C`
is 52 bytes. Thirty-two target-compiled constants verify the local group,
configuration and partial-context views and the SDK packet offsets.
The primary view at `440`, stride 536, is reused from the line helper.

Each group projects four quads, selecting points `j`, `j+4`, `j+8` and
`j+12`. The first three vertices use one RGB triplet and the fourth uses
the other. Fading multiplies each channel by signed brightness, divides
by 1024 with truncation toward zero, then narrows to a byte.

The central group uses translation at `4268`; the remaining six use
successive primary translations. Rotation is zero. Flag bit zero adds
scale / 8 for the central group or scale / 4 for the others.

Depth is multiplied by eight and divided by ten **before** testing the
scaled depth and projection flag for nonnegativity. Consequently raw
depth -1 rounds to zero and may queue; raw depth -2 does not. The queue
API then narrows depth to sixteen bits. Twenty-eight actual submission
slice executions cover these boundaries and flag rejection, and another
28 executions verify RGB arithmetic while preserving all other packet bytes.

The central group grows toward 4096 using unsigned configuration times,
then applies its separate fade interval. Other groups grow toward 16384
once the primary threshold reaches 1024, then reduce brightness by
`step * 64`. The last group changes phase to 3 at full scale and to 5
after brightness reaches zero. There are 158 actual isolated update-slice
cases across both slots, including four retained zero-duration `break 7`
controls. No new invalid-input handling is introduced.

## Compiler and ownership evidence

The [six-row ledger](spanish-model-variant450-quads-attempts.csv) preserves
two material experiments, two private complete-image matches and two
final production matches. The first candidate emitted 1,508 bytes with
the correct 264-byte frame: a direct primary cursor induced a second
translation pointer and an extra spill. An independent full-width primary
array index removes that bookkeeping and reproduces all 1,480 bytes.
No padding, register pinning, inline assembly or compiler override is used.

Both slot wrappers compile independently. Complete private links and final
production links verify all eighteen disjoint C/assembly/raw owners.
The new nine imports resolve to actual sized definitions in the exact
Spanish resident, not merely absolute aliases.

The actual `SetPolyGT4` constructor establishes length 12 and command `3C`.
Actual `GsSortPoly` and its complete queue helper copy three separate
52-byte packets, adjust all four viewport coordinates, advance the work
cursor and link narrowed-depth OT buckets. Reusing and overwriting the
source does not alter earlier packets. Their cursor and viewport storage
have verified containing raw resident owners. This execution uses no hooks;
capacity, OT extent and viewport values are supplied fixture state.

The parent calls this helper at image `C3C`, immediately after the line
helper at `C34`; both delay slots pass `s3`. The call slice executes with
the context established by the earlier actual retail initializer,
controller and negative-request prefix evidence. The intervening geometry
callees still assume callee-saved `s3` ABI preservation.

**Limits:** the new partial view ends at `42FC`, overlapping another loader
window at context `4000` by 764 bytes. It is not an allocation size, complete
context identity or noninterference proof. Projection results are supplied
for the isolated color/submission tests; no full GTE, GPU, CD, animation,
allocation or loader-phase-isolation claim is made.
