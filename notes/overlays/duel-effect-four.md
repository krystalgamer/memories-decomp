# Complete French duel effect 4

`func_80159AAC` occupies `80159AAC..8015A1E4`: 1,848 bytes, the last
function before bank data. It is a complete lifecycle routine selected by
effect 4 in the accepted dispatcher, independently recovered from French
instructions with existing project declarations and the
`gcc_2_8_1_g0_split` profile. No reference-project types, flags or source
were imported.

All 63 accepted French bank entries remain unchanged. This independent
batch reaches 64/85 C functions / 24,792 bytes, with 21 explicit assembly
boundaries. Configured French images contain 188 C instances / 80,644 bytes.
Other pending effect batches are excluded.

## Experiments and acceptance

The first candidate had the correct 1,848-byte extent and differed at only
`+5C/+60`: the packet and scale pointer setup instructions were transposed.
Initializing the packet pointer before copying the initial scale reproduced
both original instructions. No compiler flags, forced registers, padding or
inline assembly were introduced.

The corrected candidate reproduced every function byte and then every byte
of the independent 90,112-byte bank before promotion. The production bank
has SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
All seven French images, all seven bank copies and their 188 registered
C owners retain exact acceptance. All 30 bank C objects have complete
inventoried text extents; 22 distinct named overlay routines, including this
entry point, resolve to real executable sections.

Two data blocks retain real generated-input and final-ELF storage, exact
types, extents and retail bytes. `D_80146248` is the 16-byte initial vector
`{4096,4096,4096,0}`. `D_8015B704` is two 28-byte configurations, not an
absolute alias or a replacement C allocation. No resident binding changes.

## Target layout and configuration bounds

The target-GCC work size is `55C` hex:

| Offset | Field |
| --- | --- |
| `000` | Configuration pointer |
| `004` | Three rings of 32 vectors |
| `304` | 16 rotations |
| `384` / `444` | 24 particle positions / velocities |
| `504` | Two four-vector card rings |
| `544` / `548` | Unsigned scale / frame |
| `54C` / `54E` | Stage / cross-effect halfwords |
| `550` / `554` / `558` | Main / card / screen colors |

Configuration offsets are color 0, sprite size 4, three radii 6, two strip
widths C, strip height 10, card half-height/half-width 12/14, texture 16,
particle speed 18 and duration 1A. All offsets are hexadecimal.

The retail configurations differ only in color (`192,192,255` versus
`255,192,255`) and texture index (5 versus 6). Both have sprite size 10,
radii 74/80/84, strip widths 1/3, strip height 24, card half-extents 64/32,
particle speed 12 and duration 48. Both texture indices fit the accepted
21-pair texture owner. Fixed loop counts exactly fit each recovered array.

## Preserved lifecycle details

Phases 0/1 select the configurations; larger phases enter the crossed-line
fallback, which completes after its 180-update interval. Ordinary rendering
uses the captured resident frame step and existing frame-step override.

Either nonzero main or card color enters the card-rendering path. Its two
four-vertex rings use widths/heights 24/28 and 30/34. Texture selection sets
the horizontal UV range while vertical coordinates remain 128/255.
The RGB word's low 24 bits are compared with `808080` hex to select the
original quad transparency mode.

At the configured frame threshold, the first transition clears the card
color, initializes the screen flash and sets the stage. The canonical
request's `field_1D` marker precedes the 24-particle loop. The expanding
three-ring draw and sprite use two scale-matrix updates, followed by 16
independently rotated strips. Main color fades by 15; scale grows by 4096
only while below `7000` hex, without an extra clamp.

The screen color fades separately by 31. Completion tests only the main
color, not all three colors. Existing source and contracts are unchanged;
boot ownership, MODEL/SU loads and overworld-tail coverage remain open.
