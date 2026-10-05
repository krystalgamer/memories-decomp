# French MODEL129 entry

The instance ledger identifies ten distinct 20,480-byte images with headers
129/259. Model 219 selects stages 7/8; models 30, 355, 435 and 490 select
stages 9/10. Both slots have one closed 4,068-byte entry, a 368-byte frame
and one 16-byte source-owned scale vector. There are 1,017 instructions,
48 direct calls to 21 resident destinations, and no local or indirect calls.

All ten complete images were independently linked from the actual C objects.
Twenty C contributions and thirty disjoint raw owners reproduce every byte.
All 21 actual resident callee bodies agree with the clean exact French
resident ELF. No shared SDK declaration or resident implementation changes.

## Local views and native behavior

The 32-byte descriptor has particle and flash RGB triples at `0..5`,
a part byte at `6`, one unidentified byte, a signed half-size at `8`,
five unidentified halfwords at `0xA`, then travel duration, flash duration,
spacing, one unidentified halfword, count and delay at `0x14..0x1E`.
Commands are `60000 + argument`; the five observed arguments are `0..4`.
Selected counts are 24, 28, 48, 52 and 64. Every selected descriptor was
checked in its own image.

The `0x414` state access view contains a guest-width descriptor pointer,
64 SVECTOR positions at `4`, 64 SVECTOR velocities at `0x204`, two texture
handles at `0x404`, elapsed time at `0x40C`, and completion/effect-started
bytes at `0x410/0x411`. This is an access view, not allocator ownership.
Twenty-three target-compiled constants verify these offsets and SDK sizes.
The native POLY_G4 is 36 bytes; its surrounding stack gap is not its size.

Initialization deliberately does not advance the position and velocity
cursors inside its clearing loop. The original repeated writes are retained.
It uploads two images, starts elapsed time at negative delay, and clears
the two state bytes. During that delay, all selected positions are copied
from the model-part transform.

The flash pass uses texture one, expanding scale and fading RGB. The
particle pass uses texture zero and a 3x3 vertex grid, rendered as four
quads. Before launch, the particle follows the selected model part.
Travel computes velocity toward `(0, -350, direction * 450)`, dividing
before multiplying by frame step. Position is captured before updates.
The travel scale retains the phase-entry duration across potentially
aliasing position/velocity writes; other native descriptor reloads remain.

Burst motion continues the last velocity. Its growth quotient is separate
from the eventual scale, and RGB retains the two signed divisions before
tripling. Shared signed-halfword final color carriers reproduce the native
red and green destination registers. Byte carriers left four differing
words; full-word shared carriers also changed surrounding code. This
precision refinement changes no packet storage or arithmetic ordering.

Projection retains signed depth/flag gates and the native matrix chain.
The effect-started byte increments once before the effect callback.
Elapsed time uses a fresh frame-step call at the end. Completion remains
inside the positive final-burst threshold: zero before travel, four before
the final stagger/burst threshold, then one once and two subsequently.

## Matching evidence and remaining coverage

The ledger preserves nineteen paired experiments, one blocked layout
fixture check, and two canonical records. The blocked check expected a
40-byte POLY_G4 instead of the authoritative 36-byte target type; neither
entry compiled or compared in that attempt. The fixture, not the SDK
declaration, was corrected.

Independent growth, retained travel duration and terminal nesting
recovered the near-match. The final signed-halfword color carriers resolved
the remaining four words in both slots. The profile remains
`gcc_2_8_1_g0_split`, GCC 2.8.1/MASPSX 2.81. There are no forced registers,
artificial stores, fake dependencies, inline assembly or source-local externs.

Each image retains a raw four-byte header, two 28-byte image records at
`0xFF8..0x1030`, and an explicitly unclassified 16,336-byte suffix at
`0x1030..0x5000`. Descriptor declarations are views into that suffix, not
C-owned storage. The combined 163,360 suffix bytes remain unresolved.
This adds ten C instances, 40,680 instruction bytes and 160 literal bytes,
not exhaustive French runtime completion.
