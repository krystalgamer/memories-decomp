# Ring-effect ownership and frame reconstruction

`func_8006C37C` keeps its address-based name. `model_ring_effect.h` owns its
caller contract and the three source types; no type definition remains in
the final C translation unit.

## Borrowed persistent prefix and preset

`RingWork` describes only the consumed `0x230`-byte prefix of caller-owned
module-data storage. Its settings pointer is followed by 67 real eight-byte
SDK `SVECTOR` values at `+4`, a colour at `+0x21C`, an unclassified word at
`+0x220`, fade/growth words at `+0x224/+0x228`, and phase at `+0x22C`.
The last three bytes are alignment tail, not newly initialized state.

The resident callback table `D_800114E8` contains
`{0x8006CD78, 0x8006AF74, 0x8006C37C, 0x8006F1B4}`. `func_8004EB00`
copies the four entries and selects index 2 when the card's type bits equal
1. At `0x8004F1C0` it calls the selected entry with model slot 1's `+0xDEC`
pointer and command 398, after selecting model 1 through `D_8009AFA0`.
Later calls pass `-1`; the fade-out call at `0x8004FC0C` passes `-2` and
advances the caller's state when the effect returns 2.

`func_8004CB0C` installs the `+0xDEC` data argument from `D_80010024` for
slot 0 and `D_80010028` for slot 1, with retail values `0x80136000` and
`0x80176000`. These are borrowed data arguments, not entry points. The trace
establishes this effect's required prefix and lifetime, not the total size or
exclusive ownership of either module allocation. An initialized, aligned,
live `RingWork` prefix is required for negative updates.

`RingSettings` is the twelve-byte reading of the existing six-halfword
`D_80091604` definition in `model_geometry_tables.c`:
`{0x2020, 0x0020, 300, 20, 10, 0}`. No duplicate settings allocation or
new assembler alias is introduced. Colour bytes are 32/32/32/0; the signed
radius/divisor fields are at `+4/+6/+8`. The last halfword remains unnamed.

Initialization sets fade to 20 and growth to zero. The selected frame-step
getter clamps its unsigned-byte input to 6. Normal updates clamp growth to
`0..10`; fade updates subtract the step, with the caller stopping the fade
path when return value 2 reports `fade < 1`. These domains and nonzero
divisors make the measured products, shifts, and divisions meaningful.
Arbitrary corrupted presets, updates before initialization, or indefinitely
continuing a completed fade are not supported-input claims.

## What the 32-byte local reservation does and does not mean

The retail frame is `0x3C0` bytes. It places the dimension query at `sp+C0`
and projected screen points at `sp+E0`, leaving a 32-byte interval. The
matching `func_80057E20` writes **only eight bytes**:
`ModelEffectAdjustment {x, y, z, max}`. No source or instruction reads the
remaining 24 bytes of the interval.

The original purpose of those 24 bytes is unresolved. The final source uses
`RingQueryFrameReservation`, a union of the actual eight-byte query and a
32-byte byte array, to record the observed **local frame reservation**.
It is not described as a recovered 32-byte semantic object, a callee output
requirement, four adjustment records, or a matrix. Only `&dimensions.value`
is passed to the real helper. Native ownership tests prefill the interval
and prove that helper leaves bytes 8 through 31 untouched.

The existing query-prefix-only control retained 2,556 text bytes but changed
the frame to `0x3A8` and produced 43 stack-address differences. Replacing
the provisional padded struct with the explicit frame-reservation union
retains the `0x3C0` frame and is exact. Thus the reservation is grounded in
frame offsets and code generation, while its historical semantic purpose
remains unknown.

## SDK/import boundaries and legacy rendering facts

`func_80058F10` remains a 16-byte SDK-assembly import. Its four instructions
load the pointer at `D_800E9D98` and return it. The caller uses the result
as `GsOT *`, through the preserved `void *` declaration in
`ordering_tables.h`. This does not reinstate the withdrawn `GsGetWorkBase`
identification or provide a game-owned C implementation.

`func_80058DCC` returns the unsigned model-selector byte from
`model_graphics_state.h`. `func_80059220` retains its existing `void *`
interface returning the real `MATRIX D_800FE148`; its prototype belongs to
`screen_projection.h`, already included by its implementation.

The two explicit eight-byte clears precede the nested automatic
`VECTOR {4096,4096,4096,0}` initializer. Separate primary cursors traverse
the two 33-point rings, with the second cursor initialized from the first
after it advances. The quad is initialized with the SDK **function**
`SetPolyG4` but never submitted. All 67 vertices are projected, while only
the outer ring's 32 fan triangles are submitted. Every triangle repeats
the `flags[0..2]` test; it does not test its own vertex indices.

`RotTrans` receives the twelve-byte `MATRIX.t` subobject through `VECTOR *`.
Only its XYZ stores belong to that output contract; there is no separate
16-byte vector allocation and no valid reason to inspect a fourth word.
Projection outputs retain SDK `DVECTOR` and `u16` arrays. Packet submission
retains the deliberate `u16` depth conversion before the full-word helper
argument. Native tests validate ownership and ABI boundaries, not GTE
emulation.
