# French MODEL167 entry

Model71, physical record71, loads header167 at stage9/sector19796 and its
header297 counterpart at stage10/sector19806. Controller command98000 passes
argument0; the entry selects descriptor `argument % 100`. Both closed entries
contain949 instructions (3,796 bytes), use a272-byte frame and make37 calls
to22 resident destinations. Complete20,480-byte images match without masking.

## Ownership and state

| Image offset | Bytes | Owner |
| --- | ---: | --- |
| `0x0000` | 4 | Raw module header |
| `0x0004` | 3,796 | C entry |
| `0x0ED8` | 16 | C unit `VECTOR` |
| `0x0EE8` | 56 | Two raw `GsIMAGE` records |
| `0x0F20` | 16,608 | Unclassified suffix |

The28-byte descriptor contains two RGB triplets and ten halfword-sized fields.
Bytes6/7 (`19 00`) and the halfword at0x10 (`40`) are not accessed by the entry
and remain uninterpreted. Selected parameters include amplitude1300, particle
half-size200, scatter radius350, fade duration90, particle duration64, particle
delay90, stagger4, count64 and initial delay20.

The state contains a guest config pointer at0;64 `SVECTOR` points at4;
256 signed offsets at0x204;256 signed increments at0x404; signed phase/scroll
at0x604/0x606; two unsigned packed textures at0x608; started at0x610; and elapsed
at0x614, with natural extent0x618. Native vector pads remain untouched.
Stack storage reuses `vertices[0]` for the four opposite-slot values during
construction; no extra target vector is invented.

## Preserved behavior

Construction zeros offsets and creates increments
`amplitude + amplitude * csin(i * 32) / 4096`. The64 particle positions use
three random samples and five trigonometric calls each, preserving intermediate
signed divisions. Copied opposite-slot Y becomes-350. The two image uploads
use modes2 and1 respectively; both packed texture high halves use `lhu`.

The initial active-slot query remains even though its result is unused.
The established `func_800595C8` name and `model_slot_properties.h` signature
are retained: the helper clamps and writes three slot-property components.
The entry ramps slot2 from2048 to-4096 and later back to2048; no unsupported
color or geometry semantics are assigned to those fields.

The screen effect draws256 rows, two192-pixel quads per row, with a64x64
texture window at ordering-table index4095. Repeated per-corner scroll and
offset expressions preserve native loads and signed arithmetic. Semitransparency
is enabled during color fades and disabled during the steady interval.

Offset accumulation is intentionally two operations: addition with signed16
narrowing, then signed modulo16384. Both native stores matter; combining the
expressions would remove the narrowing. Two loops rotate the increments array
without per-element modulo. Scroll and phase advance only during the active
screen interval.

Particle billboards deliberately omit `MulMatrix2`. Their atlas starts at
byte U128; the native negative immediates have the same byte representation.
Cached frame step advances elapsed after drawing. Returns are0 before particle
delay,4 during emission,1 once as the final fade begins,0 during its remaining
calls, then2 on completion. Started increments only on its zero branch.

## Exact-match evidence

All four paired experiments use `gcc_2_8_1_g0_split`, GCC2.8.1 and MASPSX2.81.
The10-row ledger retains eight exploratory rows and two canonical matches.

| Experiment | Bytes / frame | Differing words per slot |
| --- | --- | ---: |
| Initial screen strips and particles | 3760 / 272 | 870 |
| Explicit paired wave cursors | 3772 / 272 | 554 |
| Byte-width staged RGB components | 3796 / 272 | 5 |
| Started-zero terminal arm | 3796 / 272 | 0 |

Independent pointer inductions recover constructor code and later cursor
allocation. Byte-width color components recover the native staged division
results across `SetSemiTrans`; the started-zero arm fixes the final branch
layout. No forced registers, fake dependencies, artificial stores, inline
assembly, reference types or compiler substitutions are used.

Validation covers34 target layouts,25 native anchors, descriptor/image records,
37/22 call signature, wrapper, ledger, complete images and actual C/raw linked
owners. All22 resident callee bodies agree with the exact resident ELF.
Clean production resident/overlay and policy gates remain mandatory for delivery.

The33,216 suffix bytes remain unclassified. This match does not establish
exhaustive overlay coverage or regional completion.
