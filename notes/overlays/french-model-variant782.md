# French MODEL782 entry

The instance ledger identifies twelve distinct complete 20,480-byte images,
with headers 782/879. Models 58, 217, 453, 563 and 612 use stages 7/8;
model 78 uses stages 9/10. Both slots contain one closed 5,292-byte entry,
a 584-byte frame and one 16-byte source-owned unit-scale vector.
All 1,323 instructions were examined. The entry has 74 direct calls to
26 resident destinations, with no local or indirect calls.

All twelve complete images were independently linked from the exact C
objects. Twenty-four C entry/literal contributions and thirty-six disjoint
raw owners reproduce every byte. All 26 actual resident callee bodies
agree with the clean exact French resident ELF. No resident implementation,
SDK declaration or compiler profile changes.

## Local views and behavior

The 40-byte descriptor begins at image offset `0x154C`, indexed by the
command argument. Commands are `314000 + argument`; the observed arguments
are 0, 2, 3 and 4. Each selected row was decoded from its own image.
Signed halfword dimensions occupy `0..0xE`, with unidentified halfwords
at 2, 6 and `0xE`. Growth rate is signed32 at `0x10`. Debris count,
half-size, spread and speed occupy `0x14..0x1A`; dot count and speed
occupy `0x1C/0x1E`. Delay and burst-end are signed16 at `0x20/0x22`;
final duration is signed32 at `0x24`.

The conservative `0x9B0` state access view contains a guest-width descriptor
pointer, an eight-byte opposing-slot origin at 4, sixteen debris positions
at `0xC` and sixteen velocities at `0x20C`. The following `0x180` bytes
after each debris array remain unidentified. The observed 64 dot positions
and velocities begin at `0x40C/0x60C`; their screens begin at `0x80C`.
Five texture handles occupy `0x90C..0x91F`, lighting level is at `0x920`,
and 64 halfword depths begin at `0x924`. Signed16 animation, elapsed,
phase and growth age occupy `0x9A4..0x9AA`; byte shade is at `0x9AC`.
This is an access view, not allocator ownership. Twenty-nine target-compiled
constants verify its offsets and the unchanged SDK sizes.

Initialization copies the opposing slot's four halfwords into the origin.
Debris uses five random calls per particle: differences for X/Z and a
negative random Y. Position Y uses half the spread, while velocity Y uses
the full speed. Only origin X/Z are added. Dot initialization uses two
random angles; the Y and Z velocity formulas deliberately repeat the same
sin/cos expression. It clears screens/depths, initializes lighting and
counters, and uploads five images.

Updates capture frame step and set override one, without a restore call
in this entry. Phase zero lowers lighting; any positive phase raises it.
Before the initial delay, elapsed advances and the entry returns zero.
The dot passes draw previous screens/depths with attribute `0x40000000`,
project fresh screens, then draw with attribute zero. Both require Y below
350 and nonzero depth, using depth divided by four as the priority.
Interpolation and the native Y-velocity adjustment follow.

The debris pass calls the SDK `SetPolyFT4` function, not its macro.
While signed animation is below eight, particles advance by velocity
without a frame-step multiplier. Projection receives the state particle
directly, not a stack copy. Four unrolled panel draws follow. Front panels
use texture three and omit `MulMatrix2`; side panels use texture four
and include it. The first front panel writes vertices after matrix calls;
the third draw writes its front vertices before those calls. Repeated
descriptor reads across escaped stores are retained. All quad gates use
nonnegative depth only, with sorter flags three.

Growth age increments once. Before burst-end, the effect callback `(32,8)`
runs every update, elapsed advances, and the entry returns one. Afterwards,
animation increments and shade follows the original byte subtraction
rule, not a general saturation clamp. Phase becomes one. Elapsed below
the signed final duration advances and returns zero; otherwise return two.

## Matching evidence and remaining coverage

Seven paired experiments are preserved. Correct panel/cursor ordering
recovered the structure. The accepted local MODEL770 division form
recovered the two dot-priority argument moves that shifts omitted.
Reusing the debris X sample carrier for the first dot angle and the
debris Y carrier for the second resolved the remaining register permutation.
Declaration reordering alone made no difference.

The profile remains `gcc_2_8_1_g0_split`, GCC 2.8.1/MASPSX 2.81.
There are no forced registers, artificial stores, fake dependencies,
inline assembly or source-local externs. Reference types were not imported.

Each image retains a four-byte raw header, five 28-byte image records at
`0x14C0..0x154C`, and an unclassified 15,028-byte suffix at
`0x154C..0x5000`. Descriptor views do not own that suffix.
The combined 180,336 suffix bytes remain unresolved. This adds twelve
C instances, 63,504 instruction bytes and 192 literal bytes, not exhaustive
French runtime completion.
