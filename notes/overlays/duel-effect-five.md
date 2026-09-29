# Complete French duel effect 5

`func_80157E10` occupies `80157E10..801587D8`, a complete 2,504-byte
lifecycle selected by effect 5 in the accepted dispatcher. It was recovered
directly from French instructions using existing project declarations and
the `gcc_2_8_1_g0_split` profile. The number renderer's existing declaration
is reused through `effect_6.h`; no duplicate or source-local prototype,
reference-project type or compiler flag is introduced.

All 63 accepted French bank entries remain unchanged. This independent
batch reaches 64/85 C functions / 25,448 bytes, with 21 assembly boundaries;
configured French images contain 188 C instances / 81,300 bytes. Other
pending batches are excluded, and the called number renderer remains
genuine assembly rather than being credited as C.

## Experiments and exact acceptance

The first candidate had exactly 2,504 bytes but 14 differing instruction
words in initialization: individual vector-field assignments did not retain
the target's shared vector address and phase/color-offset register allocation.
Using the existing `setVector` comma-expression macro restored all target
bytes with unchanged declarations and compiler settings.

The corrected candidate then reproduced the complete 90,112-byte bank
before promotion. Production SHA-256 remains
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
All seven French images, all seven bank copies and 188 registered C owners
retain exact acceptance. All 30 bank C objects have their complete extents;
20 named overlay routines, including this entry, have real executable owners.

`D_80146218` is the real 16-byte initial vector. `D_8015B450` is the real
360-byte configuration table: ten 36-byte records, not a replacement
allocation or an absolute data alias. Generated-input and final-ELF types,
addresses, extents and retail bytes are checked independently.

## Target layouts and bounds

Target-GCC fixes the work size at `C38` hex:

| Offset | Field |
| --- | --- |
| `000` | Configuration pointer |
| `004` | 64 rising positions |
| `204` / `404` | 64 secondary positions / velocities |
| `604` | Three rings of 32 vectors |
| `904` | 64 strip rotations |
| `B04` / `B0C` | Number position / velocity |
| `B14` / `B18` / `B1C` | Unsigned scale / frame / tick |
| `B20` / `B22` / `B24` | Number mode / spawned count / bounce divisor |
| `B26` / `B28` / `B2A` | Hold counter / variant / cross-effect counter |
| `B2C` | 64 rising colors |
| `C2C` / `C30` / `C34` | Background / secondary / number colors |

Configuration offsets are RGB triplets 0/3, rise speed 6, particle speed 8,
strip count A, two widths C, height 10, three radii 12, sprite size 18,
bounce speed 1A, signed number 1C, particle count 20 and an uninterpreted
halfword 22. All offsets above are hexadecimal.

Records 5..9 duplicate 0..4 byte-for-byte. Strip/rising counts are
16/24/32/40/48; secondary particle counts are 16/24/32/48/64. Thus every
count fits its 64-element array and every `strip_count - 1` access is valid.
Numbers are 200/500/1000/2000/5000, rise speeds 2..6, and every record uses
particle speed 16 and bounce speed 16.

## Preserved lifecycle distinctions

Initialization consumes four random calls for each of 64 rising positions.
Signed remainder and division place X/Z around zero; Y starts at -106.
Active rising particles move and transition color independently. Their count
accumulates the captured frame step and is capped at the configured strip
count; the frame also accumulates that step while tick increments once.

Only after the last selected rising particle crosses above zero does the
number/secondary stage run. The number uses the existing renderer, adds its
velocity, applies gravity 4 and rebounds using the incremented divisor.
Two explicit complete matrix copies precede the scale/light-matrix calls;
they are not replaced by the different component-copy helper.

Variants 0..4 fade the background, secondary and number colors and finish
only when all three are zero. Variants 5..9 instead retain those colors,
freeze secondary movement and strip rotation once the hold counter becomes
nonzero, and finish after the last rising color is zero and the hold counter
reaches 24. The hold counter advances at grounded-number updates, not
unconditionally per frame. These are distinct paths despite identical
configuration records.

No existing executable source, shared contract or resident binding changes.
Boot ownership, MODEL/SU loads and overworld-tail coverage remain unresolved;
these totals are not an exhaustive runtime-completion claim.
