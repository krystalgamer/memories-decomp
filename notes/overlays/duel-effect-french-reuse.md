# French shared effect reuse

Four complete routines contribute **7,584 bytes** against accepted master
`9b25c6617`, preserving every one of the 59 earlier French entries.

| Routine | French extent | Bytes | Shared source |
|---|---|---:|---|
| Gather | `801481A8..80148BA4` | 2,556 | `gather_effect.c` through the PAL wrapper |
| Tile | `80149F90..8014A8E4` | 2,388 | `tile_effect.c` through the PAL wrapper |
| Effect 19 | `80153ADC..80153F28` | 1,100 | Unchanged accepted Spanish `effect_19.c` |
| Effect 18 | `80154084..80154688` | 1,540 | Unchanged accepted Spanish `effect_18.c` |

Coverage reaches **63/85 bank C functions / 22,944 bytes**, with **22
assembly boundaries** and **187 configured C instances / 78,796 bytes**.
The number renderer and randomized curve remain assembly. Boot, MODEL/SU
loads and the overworld fragment still require runtime coverage work.

## Measured PAL geometry

The first complete-bank trial reused the accepted North American gather/tile
sources and Spanish effect 19 without changes. All sizes were correct, but
the full image differed in exactly nine instruction words: one in gather and
eight in tile. Effect 19 was already exact.

| Constant | North American/Japanese default | French |
|---|---:|---:|
| Gather curve-height base | 98 | 106 |
| Tile vertical row step | 28 | 30 |
| Tile vertical origin | 98 | 106 |
| Tile bottom-V addend | 126 | -120 |

Horizontal geometry does not change. The bottom-V addend is byte-wrapped:
the private `+136` expression compiles to the same `-120` immediate.
Changing only these measured expressions made the complete three-function
trial exact. No masked comparison was accepted as a match.

Production uses the existing `VERSION_EUROPE` include-wrapper pattern, as in
`src/overlays/european/free_duel/place_cursor.c`. The shared PAL changes and
wrapper paths align with the independently developed Spanish work in #6564.
The shared bodies are not copied, and default NTSC constants remain in the
original sources. The profile stays `gcc_2_8_1_g0_split`: no compiler,
optimization flag or additional profile is introduced.

A third complete-bank trial used those shared wrappers and added the
unchanged accepted effect 18. All four functions and all 63 linked C owners
match, with no partial translation-unit replacement.

## Resident bindings and real data owners

The French resident linker map independently supplies `D_8009B261 =
0x8009C600`, `D_8009B264 = 0x8009C5FC`, `PushMatrix = 0x80087158`,
`PopMatrix = 0x800871FC` and `memset = 0x8008F548`.
The authoritative resident inventory supplies the named matching C functions
`Model_GetFrameStep = 0x8005BF24` and
`Model_SetFrameStepOverride = 0x8005CBF4`; they are not new overlay C.

Seven additional symbols retain real generated header/data storage:

| Address | Bytes | Accepted view |
|---|---:|---|
| `80146024` | 16 | SDK scale vector |
| `801461C8` | 16 | SDK scale vector |
| `801461D8` | 16 | SDK scale vector |
| `8015A5F8` | 20 | Gather descriptor |
| `8015A62C` | 28 | SDK image descriptor |
| `8015A648` | 16 | Two tile descriptors |
| `8015B078` | 60 | Two effect-18 configurations |

Existing shared headers own these views. No new C storage or absolute
overlay-data alias is introduced. The private linked image and non-executable
input-data objects prove every extent and the original retail bytes.
All referenced overlay routines retain their inventory status and real
executable definitions.

## Exact-image acceptance

The full 90,112-byte French bank retains SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Each new compiled object contains exactly its one complete function and text
extent. Production acceptance checks every configured French image and all
seven terrain copies, plus the North American and Japanese full images to
preserve the shared sources' default behavior. English PAL images are also
rechecked. Temporary copies, probes and differences remain under `tmp/`.
