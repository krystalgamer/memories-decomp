# Spanish MODEL headers 418 and 568

Two independently verified MODEL410 images reuse the unchanged accepted
`variant401_{spokes,rings,quad}.c` implementations through the existing
French `variant418_*` wrappers. Six compiler-owned C instances cover 5,088
instruction bytes under the named `gcc_2_8_1_g0_split` profile, using GCC
2.8.1 and MASPSX 2.81. Source family401 is provenance, not Spanish identity.
No shared implementation, declaration, compiler profile or SDK type changed.

## Loader and boundaries

Model410 maps to compact record360. Stages9/10 select sectors200/210 of
the 276-sector record, loading ten sectors at `0x8013B000`/`0x8017B000`.
The [instance ledger](spanish-model-variant418-instances.csv) records the
actual slices, independent complete hashes and observed request584002.
The matched resident controller calls module offset4 with its context and
the initial command, then update command-1.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `4..FE0` | 4060 | generated assembly | yes |
| `FE0..1760` | 1920 | generated assembly | yes |
| `1760..1C9C` | 1340 | generated assembly | yes |
| `1C9C..2204` | 1384 | generated assembly | yes |
| `2204..272C` | 1320 | generated assembly | yes |
| `272C..2A3C` | 784 | spokes C | no |
| `2A3C..2DB8` | 892 | rings C | no |
| `2DB8..311C` | 868 | quad C | no |

Strict control-flow walks cover every instruction in all eight spans.
The C helpers are retained module-local code, not demonstrated entry-call
paths. Ten assembly instances (20,048 bytes) remain untranslated. Both
four-byte headers and 7,908-byte suffixes have real storage owners; the
suffixes remain unclassified, not established non-code.

## Layouts and ownership

Entry preserves the context through `a0 -> s3 -> s6`. Forty-three actual
Spanish instruction anchors establish two 152-byte sheets at `+0x11DC`,
six 144-byte rings at `+0x130C`, four 144-byte spokes at `+0x166C` and
one 144-byte quad at `+0x18AC`, including advances and initialization
bounds. The spokes timing input is the first sheet's size at
`0x11DC + 0x88 = 0x1264`. Rings use color offset128, spokes offset132.

Seventy-three target-compiled constants verify these four local record
types, their accessed fields, and canonical SDK layouts for `SVECTOR`,
`VECTOR`, `MATRIX`, `GsCOORDINATE2`, `GsOT`, `GsGLINE`, `POLY_G4` and
`POLY_GT4`, including target pointer and integer widths. The constant
array is a size-zero NOTYPE label in a complete 292-byte `.rodata`
section; its actual section extent and all values are checked.

Seven additional Spanish anchors establish descriptor base`+0x3218`,
stride48 and the saved pointer at context`+0x1AF4`. The actual request
selects command2, a 48-byte window at image`+0x3278`, within the suffix
owner. Direct entry accesses require at least `0x1B2C` context bytes.

Fresh exact Spanish resident proofs establish all 36 real callee input
and final owners, three matching initializer/controller/loader owners,
and both context-pointer storage owners at `0x80010024/28` in
`spanish_raw_80010000.o` input `.data`. That input data resides in mixed
executable output `.main`; output flags do not erase its input ownership.
The pointers select `0x80136000/0x80176000`. Their minimum accessed views
are disjoint from the selected 96-sector MODEL, two-sector primary and
ten-sector secondary loads. This is not an allocation-capacity or
whole-game lifetime-isolation claim.

## Exact matching and limits

The [attempt ledger](spanish-model-variant418-attempts.csv) records all six
unchanged canonical wrapper matches. Actual links reproduce both complete
20,480-byte images without masking, selecting sized compiler functions,
explicit assembly fallbacks and real header/suffix storage. Dependency
fingerprints, actual resident symbols and selected input sections are
checked separately from image hashes.

The regional regression fixture reuses the French boundary/source tests
and adds Spanish fallback-binding completeness, descriptor selection and
minimum-context checks. All previously accepted Spanish registrations
remain unchanged. Progress snapshots stay separate. Further game-owned
code, unknown suffixes and exhaustive runtime coverage across all seven
releases remain open.
