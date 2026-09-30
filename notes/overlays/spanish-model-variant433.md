# Spanish MODEL headers 433 and 583

Four distinct Spanish images for models180/440 reuse the unchanged accepted
`variant416_{spokes,rings,quad}.c` bodies through the existing French433
wrappers. Twelve compiler-owned C instances cover 10,176 instruction bytes
using the named `gcc_2_8_1_g0_split` profile, GCC 2.8.1 and MASPSX 2.81.
Shared C, local declarations, SDK types and compiler profiles are unchanged.

## Loader and boundaries

Models180/440 map to compact records180/390. Stages7/8 load ten-sector
record slices180/190 at `0x8013B000`/`0x8017B000`. The matched resident
controller calls offset4 with the context and decoded initial command,
then update command-1. The [instance ledger](spanish-model-variant433-instances.csv)
records each actual slice, complete hash and request599001/599000.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `4..1050` | 4172 | generated assembly | yes |
| `1050..1810` | 1984 | generated assembly | yes |
| `1810..1C64` | 1108 | generated assembly | yes |
| `1C64..21CC` | 1384 | generated assembly | yes |
| `21CC..26C8` | 1276 | generated assembly | yes |
| `26C8..29D8` | 784 | spokes C | no |
| `29D8..2D54` | 892 | rings C | no |
| `2D54..30B8` | 868 | quad C | no |

Strict walks cover every instruction and terminal return in all eight
spans. The three C helpers remain retained module-local code without a
demonstrated direct entry-call path. Twenty assembly instances, 39,696
bytes, remain untranslated. Real input and final storage owners preserve
every four-byte header and 8,008-byte suffix. Those suffixes remain
unclassified, not established non-code.

## Layouts and real owners

Entry preserves the context through `a0 -> s3 -> s6`. Forty-three
Spanish instruction anchors verify three 152-byte sheets at `+0x10B0`,
six 144-byte rings at `+0x1278`, four 144-byte spokes at `+0x15D8` and
one 144-byte quad at `+0x1818`, including pointer reloads, advances,
counters and bounds. The spokes timing input is the first sheet's
size at `0x10B0 + 0x88 = 0x1138`. Ring color is offset128; spoke color
is offset132.

Seventy-three independently recompiled constants verify those four
canonical local record types and accessed fields, SDK vectors/matrices,
stored pointers in `GsCOORDINATE2`, `GsOT`, `GsGLINE`, `POLY_G4`,
`POLY_GT4` and four-byte target pointer/integer widths. Actual storage
is a complete 292-byte `.rodata` section with a size-zero NOTYPE array
label; verification uses the real section extent and all values.

Seven further Spanish anchors prove descriptor base`+0x31B4`, stride48
and the saved descriptor pointer at context`+0x1A60`. Actual requests
select commands1/0, and both 48-byte windows fit the suffix owners.
The minimum directly accessed entry context extends to `+0x1A98`.

Fresh exact Spanish resident proofs identify 36 real callee input and
final owners, three matching initializer/controller/loader owners, and
both context-pointer storage owners at `0x80010024/28`. Those pointers
are input `.data` in `spanish_raw_80010000.o`, even though their linked
output `.main` also contains executable code. The selected contexts
`0x80136000/0x80176000` and minimum views do not overlap the selected
96-sector MODEL, two-sector primary or ten-sector secondary loads.
This does not prove allocation capacity or whole-game lifetime isolation.

## Exactness and scope

The [attempt ledger](spanish-model-variant433-attempts.csv) preserves six
terminal canonical-wrapper matches covering all four images. Complete
unmasked links reproduce every image with sized compiler functions,
explicit assembly fallbacks and real header/suffix storage. Dependency
fingerprints and actual resident owners are checked separately from hashes.

Regional regressions reuse the French source and boundary fixture, adding
Spanish fallback-binding, actual descriptor and minimum-context checks.
Previously accepted module records remain unchanged; progress snapshots
stay separate. Unknown game code, further Spanish runtime discovery and
the expanded seven-release campaign remain open.
