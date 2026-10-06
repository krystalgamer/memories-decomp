# French MODEL131 entry

The instance ledger identifies twelve distinct complete 20,480-byte images
with headers 131/261. Models 183, 273, 486, 490, 540 and 637 use stages 7/8.
Each slot contains one closed 5,560-byte entry, a 632-byte frame and a
16-byte source-owned unit-scale vector. All 1,390 native instructions were
examined. The entry makes 71 direct calls to 24 resident destinations,
with no local or indirect calls.

All twelve complete images were independently linked from the exact C
objects. Twenty-four C entry/literal contributions and thirty-six disjoint
raw owners reproduce every byte. All 24 actual resident callee bodies
agree with the clean exact French resident ELF. No resident implementation,
SDK declaration or compiler profile changes.

## Local views and behavior

The 20-byte descriptor starts at image offset `0x1604`, indexed by
`command % 100`. Commands are `62000 + argument`; selected arguments are
0, 1, 2, 3 and 5. Each selected descriptor was decoded from its own image.
Bytes 0..2 are beam RGB, 3..5 are particle RGB, 6 is the model part and
7 remains unidentified. Signed halfwords at 8..18 hold radius, particle
half-size, spin, fade, duration and delay. Spin can be negative.

The conservative `0x18C` state access view contains a guest-width descriptor
pointer, fourteen ring vectors at 4, an origin vector at `0x74`, thirty-two
particle vectors at `0x7C`, two texture handles at `0x17C`, elapsed time
at `0x184`, and command-group/completed bytes at `0x188/0x189`.
The particle vector's signed16 `pad` field is its animation counter.
The command-group byte is initialized from `command / 100` but not read
in this entry. This is an access view, not allocator ownership.
Twenty-two target-compiled constants check local offsets and SDK sizes.

Initialization captures frame step and creates seven pairs of ring vertices.
The extended radius and trigonometric calls are recomputed each iteration.
Particle positions use random radius/angle and signed Z, with animation
counter `i % 16`. Two image records are uploaded.

Updates fetch the ordering table and light-source matrix before the delay
gate. During delay, the configured model part supplies the origin, elapsed
time advances with a fresh frame-step call and the function returns zero.

The beam uses a rising/plateau/falling color envelope. Scale grows from
zero to unity, then doubles during the final fade. Rotation includes signed
spin. Position Y becomes -350, and final-fade Z approaches signed 900.
Strip length retains the native doubled-sine shift before multiplication.
Two projected planes form seven initial quads, followed by additional
strips. The endpoint can clamp while the working position still advances
by the full signed strip length. The loop includes equality and may draw
a terminal degenerate strip. Crucially, the projection-page selector toggles
inside the seven-quad loop, once per quad. Beam sorting requires nonnegative
depth and clear `0x20` projection flags.

The particle pass omits `MulMatrix2`. Its `GsSortPoly` call is unconditional
after projection: there is no depth or flag gate. Each drawn particle
increments its signed animation counter using a fresh frame-step call.
Respawn overwrites the same scalar previously used as the particle delay
adjustment with `rand() % radius`; later particles use this changed scalar
in their respawn threshold. This native behavior is preserved deliberately.

After restoring the matrix, elapsed time advances using the captured step.
The original delay adjustment is recomputed. Return precedence is below-lag
zero, at/after-duration two, before-final-lag four, then completed-positive
zero or first completion one with the completed byte set.

## Matching evidence and remaining coverage

Eleven paired experiments are preserved. Correct spill declaration order,
scale-before-rotation stores, doubled-sine expression and projection-page
initialization recovered the beam and particle sequence. A distinct ring
angle carrier recovered the constructor registers. Several terminal trees
were rejected. The existing no-CSE-follow-jumps profile also failed.
The completed-positive terminal branch with its explicit else recovered
the final two reloads and the native zero-return layout under the original
`gcc_2_8_1_g0_split` GCC 2.8.1/MASPSX 2.81 profile.

There are no forced registers, artificial stores, fake dependencies,
inline assembly, source-local externs or imported reference-header types.
Each image retains its four-byte raw header, two 28-byte image records
at `0x15CC..0x1604`, and an unclassified 14,844-byte suffix at
`0x1604..0x5000`. Descriptor views do not own that suffix.
The combined 178,128 suffix bytes remain unresolved. This adds twelve
C instances, 66,720 instruction bytes and 192 literal bytes, not exhaustive
French runtime completion.
