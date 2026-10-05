# French MODEL153 entry

The instance ledger identifies ten distinct 20,480-byte images with headers
153/283. Models 38, 271 and 635 select stages 7/8; models 167 and 356 select
stages 9/10. Each slot has one closed 3,592-byte entry, a 304-byte frame and
a 16-byte source-owned unit-scale vector. The 898 instructions contain 41
direct calls to 21 resident destinations, without local or indirect calls.
The slot wrapper renames only the entry, literal and two raw storage views.

All ten complete images were independently linked from the actual C objects
and three disjoint raw owners. Their 20 C contributions and 30 raw owners
reproduce every byte without masking. All 21 actual resident callee bodies
agree with the exact French resident ELF. No speculative SDK declaration,
resident implementation or shared type is introduced.

## Local views and native behavior

The 30-byte descriptor has two byte RGB triples, a part byte and one unknown
byte at `0..7`. Signed halfwords describe main half-size and target spread
at `8/0xA`, burst half-size, rise height and burst spread at `0xC..0x10`,
count and spacing at `0x12/0x14`, travel, fade and burst durations at
`0x16..0x1A`, and delay at `0x1C`. Loader commands are `84000 + argument`;
observed arguments are `0, 2, 3, 5`. The entry retains the native `% 100`
descriptor selection. Every selected row was checked in its own image.

The `0x450` state access view has a guest-width descriptor pointer at `0`,
64 positions at `4`, 64 targets at `0x204`, four burst offsets at `0x404`,
nine texture handles at `0x424`, a completion byte at `0x448`, and elapsed
time at `0x44C`. This extent is not an allocator ownership claim.
Twenty-six target-compiled constants verify descriptor, state, vector,
matrix, image and packet layouts.

Initialization clears all four halfwords of each position. Each target
starts at the opposing slot center, then receives signed random offsets:
X adds twice a spread remainder minus spread, Y adds a spread remainder,
and Z adds direction times a spread remainder. Target padding is cleared.
Four burst offsets use independent symmetric X/Z remainders and negative
rise height for Y; their unused padding is not initialized. Image zero is
uploaded with mode two and images one through eight with mode one. Time
and completion are cleared.

Runtime initializes waiting positions from the selected model part using
`GsGetLwUnit`. Main particles travel to their targets, grow from scale 512
toward 4096, then rise and fade while scale falls toward 2048. Travel
interpolation deliberately does not multiply by frame step; the subsequent
vertical rise does. Animation uses `(time / 2 + index) % 8`, four 64-pixel
columns, 87-pixel row spacing and 86-pixel lower-edge offsets.

Burst rendering starts independently at each particle's travel completion.
Each of the four copied offsets is multiplied by burst time in signed
16-bit vector fields, narrowed, then divided by burst duration before the
target position is added. This narrowing must not be replaced with a
single wide multiply/divide expression. The burst uses texture zero,
configured secondary RGB, unit scale and eight 32-pixel-wide frames at
V coordinates 174 through 205.

Both passes retain native signed depth and flag gates, matrix-call order
and pointer advancement. The otherwise unused POLY_F4 setter remains.
After the time update, the entry returns zero before the travel threshold,
four until the final stagger threshold, one once at that threshold, then
zero while the burst remains active, and two after its terminal duration.
No new bounds or division guards alter native behavior.

## Matching evidence and remaining coverage

The ten-row ledger preserves four paired experiments and two canonical
matches, with no blocked attempts. The first reconstruction matched
everything except eight terminal words. Direct spacing-product comparisons
removed the animation scalar's inherited register preference, fixing three
words. A nested completion threshold regressed to 22 words; preserving the
early return-four threshold while expressing the completion transition
positively fixed the remaining five.

The source uses authoritative `gcc_2_8_1_g0_split`,
GCC 2.8.1/MASPSX 2.81. No forced registers, fake dependencies, artificial
stores, inline assembly or reference-header types are used.

Each image retains a raw four-byte header, nine 28-byte GsIMAGE records at
`0xE1C..0xF18`, and an explicitly unclassified 16,616-byte suffix at
`0xF18..0x5000`. Descriptor declarations are views into that suffix, not
C-owned storage. The combined 166,160 suffix bytes remain unclassified.
This adds ten C instances, 35,920 instruction bytes and 160 literal bytes,
not exhaustive French runtime completion.
