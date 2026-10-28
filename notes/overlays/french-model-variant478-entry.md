# French MODEL478 loader entries

The header-478 family used by French physical MODEL272 and MODEL636 now has
matching C for all four functions in stage 9/slot 0 and stage 10/slot 1.
All four complete 20,480-byte images reproduce their immutable retail inputs.
This completes another family in #6460.

The entry occupies `8013B004..8013C34C` in slot 0 and
`8017B004..8017C34C` in slot 1: 4,936 instruction bytes, 1,234 words and a
248-byte frame. Same-slot entry bytes are identical across the two physical
models. One implementation and the existing slot-wrapper convention use the
named `gcc_2_8_1_g0_split` profile.

## Evidence and retained ownership

All 1,234 native words were read before reconstruction. Local game/SDK headers,
the existing component views and independently measured accesses supply the
declarations. No reference-project types, headers or compiler claims are used.
The attempt ledger records twenty-three native source/header or named-profile
experiments, followed by four complete production-image terminals.

Each image retains these existing helper owners, profiles and sizes unchanged:

| Slot-0 address | Bytes (hex) | Existing C owner |
|---|---:|---|
| `8013C34C` | `54C` | French `variant478_swarm.c` |
| `8013C898` | `508` | Spanish `variant478_quads.c` |
| `8013CDA0` | `480` | Spanish `variant478_rings.c` |

Slot-1 addresses use the corresponding `8017` prefix. Production ELFs confirm
the original-sized entry and all three original-sized helpers in every image.
The entry directly calls all three helpers behind the original elapsed/phase
gates; no reachability or dead-code behavior is changed.

## Measured constructor view

The view ends at `2A38`, a minimum observed extent rather than an allocation
capacity claim. Configuration records have a `30`-byte stride at the image
suffix plus `FC`. Three part lookups are retained, and the first part's matrix
is refreshed in the runtime path.

| Context offset | Measured storage |
|---|---|
| `0000` | One `2FC`-byte ten-quad group, including positions and reset arrays |
| `02FC` | Three `264`-byte eight-point petal groups |
| `0A28` | One `694`-byte thirty-two-quad swarm |
| `10BC` | Sixteen `A8`-byte nine-point trail records |
| `1B3C` | Three `1A0`-byte four-by-six near/far grids |
| `201C` | One `A0`-byte two-color-pair record |
| `20BC` | Two existing `98`-byte sheets |
| `21EC` | Six `90`-byte eight-point rings |
| `254C` | Four `90`-byte larger rings |
| `278C` | One `90`-byte small shape |
| `281C` | Triangle, quad and textured-packet area |
| `2994` | Matrix, target, direction and projection state |
| `29E8` | Timing, configuration, part pointers and controller state |

Entry-only views expose real position and reset arrays beyond the existing
renderer prefixes. In particular, the swarm's constructor extends through
`694`; its renderer's shorter prefix is not used as a component stride.
Packet holes, unused arrays and unobserved fields remain untouched.

The nine trail colors retain their separate signed seed and byte stores.
Their actual progress accumulator is stored and then decremented by 256 in
the loop body. The target projection keeps the SDK-native packed `PSXLONG`
representation, including signed projected Y. Native unsigned trigonometric
shifts, slot-dependent matrix-Z adjustment and all timing/phase transitions
are preserved.

The final nine-word mismatch was a real producer-lifetime distinction:
`reset_value = 4096` belongs inside the final four-ring initializer, and the
subsequent `2A14` halfword store consumes it after the loop. The already
matching MODEL469 entry supplies the same local family pattern. Native
`8013BF6C` produces the value and `8013BF8C` stores it. Moving its definition
before or after that loop does not reproduce the original scheduling.
The value is a literal, not derived from the completed loop index.

No inline assembly, forced registers, source-local externs, volatile tricks,
fabricated accesses or artificial arithmetic/dependencies are used.

## Complete production image identities

| Physical model / stage / slot | SHA-256 |
|---|---|
| 272 / 9 / 0 | `3b8ea608c0cb61ad7dc71167adce3f64557d9b51da3dc02fe604c190f76e7a25` |
| 272 / 10 / 1 | `50450f7cc8ff97c0302b493c1f344164163f14791d359037eae26c30b4b1de50` |
| 636 / 9 / 0 | `0e4ff8ebc7c9eb95b77fa207c5b378bfa91129dff1205b0d83280f8df2540276` |
| 636 / 10 / 1 | `ae327442d2667e8a6f5c50018cb8c486152128dfa8069793364798412294d0e6` |
