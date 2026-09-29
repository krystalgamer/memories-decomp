# French duel effects 8 and 12

| Routine | Complete extent | Bytes | Shared source |
|---|---|---:|---|
| Effect 8 | `80147B18..801481A8` | 1680 | `src/overlays/duel_effects/effect_8.c` |
| Effect 12 | `80150E00..80151218` | 1048 | `src/overlays/duel_effects/effect_12.c` |

The new sources and paired helper-contract refinements are unchanged from
Spanish #6572 head `9cf9d893f1b422ba102ea29bc7d9716699f22c9c`.
This is an independent French proof from accepted master `fbb7e6b6e`, not
a branch stacked on that pending PR. No Spanish registrations or unrelated
changes are imported. The existing `gcc_2_8_1_g0_split` profile remains
authoritative: GCC 2.8.1 and MASPSX 2.81, without ad hoc flags.

All 63 accepted French entries remain unchanged. The two additions give
**65/85 bank C functions / 25,672 bytes**, 20 assembly boundaries, and
**189 configured French C instances / 81,524 bytes**. The separate pending
French effect 0/6 and 2/21 registrations are not counted here.

After #6575 was accepted, additive integration through master `b7e78a24`
preserves all 65 accepted entries, including effects 2/21, and this 8/12
batch: **67/85 bank C functions / 29,620 bytes**, 18 assembly boundaries,
and **191 configured French C instances / 85,472 bytes**. Both batches'
bindings, generated data owners, evidence, and regression tests remain
present. The separate pending 0/6 batch is still not counted.

After #6571 was accepted, integration through master `9a19c08c` also
preserves effects 0/6: all 67 accepted entries plus this 8/12 batch give
**69/85 bank C functions / 33,824 bytes**, 16 assembly boundaries, and
**193 configured French C instances / 89,676 bytes**. The shared sources
and helper contracts are now also accepted through the Spanish 8/12 work;
this reconciliation adds only the independent French registrations and proof.

## Independently measured contracts

The first trial used the existing helper contracts. It emitted effect 8 at
1,676 bytes and effect 12 at 1,044 bytes: each four bytes short. The complete
image was 90,104 rather than 90,112 bytes, with downstream displacement and
changed dispatcher relocations. That trial was rejected.

Both callers sign-extend the low halfword of the radial height product.
The shared `func_8014EE0C` declaration and definition therefore take `s16`
height rather than `u16`. Effect 8 passes ray width with an unsigned
halfword load; the shared `func_80156E58` declaration and definition take
`u16` width rather than `s16`. No conflicting local declarations or
function-pointer casts hide either change.

With those two paired refinements, both complete effects and the entire
bank match. The existing radial/random and primitive drawing groups retain
their function order and complete object extents; their output bytes do not
change. The refinements must preserve other regions as well as French.

## Real code and data ownership

The French archive retains SHA-256
`e00de6fac1660bcf142a20a2c0c965a020e75382d22cf62c26e0bfc153cdfe7d`.
All seven terrain copies occupy 44 sectors each at
`7193 + terrain * 240`. The complete bank retains SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.

All prior/new C functions have exact address, size and executable-section
ownership. Both new objects consist of their one complete function. The
18 distinct overlay routines named by the new bodies have real inventoried
definitions, not absolute aliases.

| Data | Bytes | Original generated owner |
|---|---:|---|
| `D_80146014` | 16 | Header, effect 8 initial VECTOR |
| `D_80146188` | 16 | Header, effect 12 initial VECTOR |
| `D_8015A514` | 228 | Data, six 38-byte configurations |
| `D_8015AEE4` | 16 | Data, single effect 12 configuration |

Every symbol retains its exact extent and retail bytes in the generated
input object and final ELF. `Model_GetLightSourceMatrix` resolves to the
existing 12-byte French resident C function at `0x8005C328`, independently
checked against the authoritative inventory. Other resident bindings and
canonical request declarations remain unchanged.

## Preserved behavior

Effect 8 keeps its six variants, crossed-line fallback, width/height clamps,
joint-boundary fading, rays, moving particles and optional two-scale rings.
Its work view is `0x820` bytes, with color deliberately unaligned at `0x81A`.
The zero acceleration expression preserves the target's otherwise unused
halfword read. Six retail configurations have 32 rays, range 64, ray width 4
and 32 particles; only the last two enable rings. The actual work arrays
retain 32 rays and 64 particle positions/velocities.

Effect 12 uses one 16-byte configuration for every nonnegative phase, with
three 32-point rings and 32 particles in its `0x518`-byte work view. Two
matrix-scale updates, later unconsumed scale writes, the resident request
marker, saved frame step and separate completion flag remain intact.

## Final acceptance

All 26 configured images across French, North American, Japanese and English
PAL reproduce their local retail inputs after the shared refinements.
All 189 French C owners, both complete new objects, both existing two-function
helper objects, 18 routine owners and four input/final data owners are
independently checked. Target GCC verifies 41 structure-size/field-offset
constants and the six measured configuration bounds.

The accepted Spanish bank's 65 C owners are also rebuilt and linked against
the independently verified French bank payload, whose hash equals the
configured Spanish bank hash. That complete compatibility image matches.
This does **not** replace Spanish archive, extraction-offset or terrain-copy
verification: those local archives remain unavailable.

The full regression suite passes 773 tests with two skips. Metadata, basic
types, source/header contracts, note links and whitespace checks pass.

No partial matches, inline assembly, register pins, guessed data allocation
or SDK/handwritten exclusions are introduced. Remaining bank code and the
boot, MODEL/SU and overworld runtime census keep the French campaign open.
