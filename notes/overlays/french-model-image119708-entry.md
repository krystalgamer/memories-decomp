# French MODEL119708 rising model and falling sparks

The raw French MODEL.MRG images at sectors 119708 and 119718 have headers 819
and 916. Their 4,808-byte entries are independently recovered in
`image119708_entry.c`, with the slot-one wrapper relocating the three local
data identities. Both use the authoritative `gcc_2_8_1_g0_split` profile.
Neither header number was treated as evidence of a USA implementation.

## Measured layout

The command table at slot-zero `0x8013C2F8` has 40-byte rows:

| Offset | Measured role |
| --- | --- |
| `0x00` | Unsigned half-size, particle count, fall speed, unclassified halfword, spread, rise-frame divisor |
| `0x0C` | Scale increment |
| `0x10` | Rise end |
| `0x14` | Pause end |
| `0x18` | Growth end |
| `0x1C` | Hold end |
| `0x20` | Return end |
| `0x24` | Unclassified word |

The 1,592-byte state contains:

| Offset | Measured role |
| --- | --- |
| `0x000` | Stored configuration pointer |
| `0x004` | Root-motion offset VECTOR |
| `0x014` | Root-motion velocity VECTOR |
| `0x024` | 128 particle SVECTORs |
| `0x424` | Unsigned model scale |
| `0x428` | Unsigned elapsed time |
| `0x42C` | Unsigned halfword phase and live count |
| `0x430` | 128 unsigned halfword fall speeds |
| `0x530` | 128 unsigned halfword atlas ages |
| `0x630` | Ambient value |
| `0x634` | Packed texture page/CLUT |

The scale VECTOR at `0x8013C2CC` and GsIMAGE at `0x8013C2DC` remain original
retail data; no replacement allocation or initializer was introduced.

## Behavior retained

Initialization copies both slot positions, derives the rise/return velocity,
retains the opposing-dimensions query, disperses particles around the opponent,
and uploads the texture. Random differences and remainders stay signed.
The original divisors, narrowing stores, and possible division traps are not
replaced with guards or alternative distributions.

Updates preserve root rise, pause, scale growth, the one-time shrink-stage
return, hold, and root return. The slot scale is published before its next
phase update. Live particles advance one atlas age and fall once per update,
not by the frame step; elapsed time and ambient changes retain their separate
step handling. The four repeated drawing blocks expand from a C macro, with
no added helper call.

Particle corners, zeroed local vectors, lighting transforms, unsigned atlas
division/narrowing, depth-only submission, and unsigned 16-bit ordering depth
remain in native order. Camera presets 13 and 9/11, their slot selection and
durations, ambient phases, all return codes, and the original frame override
are retained.

## Recovery evidence

The ledger records four frozen compiler outputs and the initial compiler
failure against canonical coordinate-member names. The first valid body was
4,812 bytes with a 264-byte frame. Explicit stage returns restored the native
tail placement. Pointer/index spelling controls did not recover initialization
address commoning; the SDK `setVector` comma expression did, producing every
native instruction without extra declarations, fabricated accesses, fixed
registers, assembly, or one-off compiler flags.

Both relocated 4,808-byte bodies and their original-sized C symbols were
verified, followed by complete production builds:

| Image | Bytes | SHA-256 |
| --- | --- | --- |
| MODEL119708 | 20,480 | `fe41b960c56a5e4ee53db51a3b2ce903d841b37a9d0b58daf781a2dd5fb01dea` |
| MODEL119718 | 20,480 | `36bc117eb9a8490ef27f281e951d84471884e4e438aa701a609e61a19fcf219b` |

The original headers and unclassified suffixes are unchanged. Existing retail
identities and unrelated module bindings are unchanged.
