# French MODEL132 entry

Model416, physical record366, loads header132 at stage9/sector101216 and its
header262 counterpart at stage10/sector101226. The controller command63000
passes initial argument0, used directly as the descriptor index. Both closed
entries contain941 instructions (3,764 bytes), use a736-byte stack frame and
make43 calls to25 resident destinations. The two functions and their16-byte
unit vectors are C-owned; their complete20,480-byte images match without
masking. The slot1 wrapper only relocates verified symbols.

## Ownership and local declarations

| Image offset | Bytes | Owner |
| --- | ---: | --- |
| `0x0000` | 4 | Raw module header |
| `0x0004` | 3,764 | C entry |
| `0x0EB8` | 16 | C unit `VECTOR` |
| `0x0EC8` | 28 | Raw `GsIMAGE` |
| `0x0EE4` | 16,668 | Unclassified suffix |

The34-byte descriptor is nine color bytes, one uninterpreted byte and twelve
signed halfwords. The selected record supplies radius100, curve width400,
path height600, quad half-size150, ring height50, twelve radial copies,
three rings,32 particles, durations16/30/60 and delay282.

The state has a guest config pointer at0;128 `SVECTOR` path slots at4;
34 ring vertices at0x404; four copied `u16` values at0x514; one signed packed
texture at0x51C; elapsed at0x520; and started at0x524, with natural extent0x528.
These declarations follow native accesses and existing SDK headers, not
reference guesses. The loader context does not overlap the model, auxiliary
image or executable overlay spans.

Path capacity128 is bounded by the following ring storage and the final
sampling cursor. Only127 points are initialized:42 straight points and85
curved points. The selected32-particle configuration samples indices0,4,...124,
then advances the pointer to the one-past-capacity index128. Path slot127
and all vector pad halfwords remain untouched; no added initialization or
invented point semantics conceal this native behavior.

## Preserved behavior

The straight path keeps signed division before multiplication:
`-height * i / 128 * 3`. The curved path uses angle `i * 3072 / 85` and divides
the signed width by two before trigonometric multiplication. Two17-point
rings include their seam duplicates. The existing `Model_CopySlotU16Values`
copies four values from the opposite slot; only copied Y is cleared.

The otherwise unused initial `Model_GetActiveSlotIndex` call remains.
Before the delay expires, a second `Model_GetFrameStep` call advances elapsed
instead of the cached step. Active calls use the cached step after drawing.
The particle tail threshold narrows `particle_count / 3` to `s16`, but its
scale denominator `particle_count * 2 / 3` is not narrowed. Particle billboard
matrices deliberately omit a matrix multiplication after their world-position
transformation. Packed texture extraction uses signed `lh`.

The flash uses `GetDispEnv` and width/height minus one. Ring Y replaces copied
Y rather than adding to it. Each ring projects34 vertices and joins sixteen
quads across the two loops. `AverageZ4` determines visibility, while the
combined projected flags use mask0x20. Both flash and ring submissions use
literal depth1. Native started/complete returns are0 before activation,1
once,0 on subsequent active calls and2 when elapsed-minus-delay reaches
`particle_count * 2 + particle_duration`.

## Matching evidence

All seven paired experiments use `gcc_2_8_1_g0_split`, GCC2.8.1 and MASPSX2.81.
The ledger retains both slots of every experiment plus two canonical matches.

| Experiment | Bytes / frame | Differing words per slot |
| --- | --- | ---: |
| Initial independently recovered body | 3748 / 728 | 347 |
| Shared curve-phase scalar | 3748 / 728 | 253 |
| Shared opposite-ring vertex index | 3756 / 736 | 27 |
| Positive started-byte terminal | 3764 / 736 | 11 |
| Complete-first terminal tree | 3764 / 736 | 8 |
| Complete-first, positive started-byte arm | 3764 / 736 | 2 |
| Derived ring angle at both calls | 3764 / 736 | 0 |

The shared phase scalar recovers native allocation across curve angle, radius,
scale, atlas and opposite-ring indexing. Explicit complete-first and positive
started-byte branches recover the native state reload and return-zero block.
Deriving the ring angle directly from its point index recovers the final
saved-register scheduling. No forced registers, fake dependencies, artificial
stores, inline assembly or compiler substitutions are involved.

Validation covers38 target-compiled layouts,24 native anchors, the selected
descriptor and image record,43/25 call signature, wrapper,16-row ledger,
complete images and actual linked C/raw ownership. All25 resident callee
bodies are checked against the exact French resident ELF. Clean production
resident/overlay and policy gates remain mandatory for delivery.

The33,336 suffix bytes across these images remain unclassified. This match
does not establish exhaustive overlay coverage or regional completion.
