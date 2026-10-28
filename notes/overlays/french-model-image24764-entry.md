# French MODEL24764-family fixed-height rings and beams

For #6460, raw French MODEL.MRG sectors 24764, 24774, 143996 and 144006
now own their loader entries in C. Each entry occupies image `+0x4..0xC4C`
(3,144 bytes), for 12,576 instruction bytes across four instances.
The two same-slot native bodies are byte-identical. An opposite-slot
wrapper changes only the entry and three image-local data symbols.
Module headers and the unclassified suffixes remain unchanged.

The existing named `gcc_2_8_1_g0_split` profile uses GCC 2.8.1/MASPSX 2.81.
All four complete 20,480-byte production images equal the original retail
bytes and hashes, with original-sized C symbols:

| Sector | Entry | Complete-image SHA-256 |
|---|---|---|
| 24764 | `func_8013B004` | `224f56d1088ff8aea9f6f2cef72d9b51a577760150eff6f02fda82031b38811b` |
| 24774 | `func_8017B004` | `b73e8deb470a54a672a462713498f84ccf3c035490210b0f224187f8edb7a3d8` |
| 143996 | `func_8013B004` | `92244ca494605c76c51749aa56f8466f3e4fc03d64d64f3041f373ad9286ac49` |
| 144006 | `func_8017B004` | `72eff8ab58cd1371db0169e27016a0056692ee3eccb3097d7bcb492da624ce67` |

## Native recovery

The independently recovered MODEL16740 algorithm supplied the common
structure, with every native structural difference examined before the
first candidate. This family fixes both ring and beam translation Y to
`-350`, rather than the earlier configurable ring Y and zero beam Y.
Beam corners span negative through positive configured height, rather
than ending at zero. Image-local VECTOR, two GsIMAGE records and the
38-byte configuration table move to `+0xC4C`, `+0xC5C` and `+0xC94`.
Configuration `+0x12` is not read by this entry and remains unnamed.

The shared native state layout holds 32 beam origins, 66 ring points,
the opposite-slot origin, current-slot translation, two packed textures,
elapsed time and a one-time completion byte. The real frame remains
992 bytes, including all 66-element screen/depth/interpolation/flag arrays.
Expanding ring segments use `RotTransPersN`/`AverageZ4`; sixteen staggered
mirrored beam pairs use `RotAverage4`. The PAL flash, matrix stack,
frame-step timing and 0/4/1/2 completion protocol are preserved.

The [attempt ledger](french-model-image24764-entry-attempts.csv) records
the first byte-exact candidate and all four production image results.
The candidate retains native shared scalar lifetimes, phase-local RGB
and pair-counter variables, the common inner-row index and staged flash
factor. Local views follow the measured accesses; canonical game/SDK
headers supply declarations. This delivery is self-contained and does
not include or depend on the pending MODEL16740 source.

No reference types or compiler claims, inline assembly, fixed registers,
volatile accesses, artificial dependencies or compiler exception are used.
