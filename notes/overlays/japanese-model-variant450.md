# Japanese MODEL450 quad, line, and streamer helpers

Japanese model 174 stages 9 and 10 contain two distinct 20,480-byte images
at sectors 48224 and 48234. Their seven closed function extents are
`4..DDC`, `DDC..1770`, `1770..1E7C`, `1E7C..2754`, `2754..2D20`,
`2D20..30A8`, and `30A8..38DC`. The four-byte header and complete
`38DC..5000` suffix retain raw owners; the suffix remains unclassified.

The accepted Spanish quad and line bodies and the independently recovered
NTSC streamer body compile to the Japanese instruction streams when selected
with the existing `gcc_2_7_2_cdk_g0` profile. Slot-specific wrappers only
rename functions where the load address changes:

| Offset | Bytes | Matching instances | Selected body |
| --- | ---: | ---: | --- |
| `0x2754..0x2D20` | 1,484 | 2 | `spanish_model_variant/variant450_quads.c` |
| `0x2D20..0x30A8` | 904 | 2 | `spanish_model_variant/variant450_lines.c` |
| `0x30A8..0x38DC` | 2,100 | 2 | `model_variant/variant450_ntsc_streamers.c` |

The six resulting objects and linked symbol extents match independently.
Both complete images then reproduce their retail SHA-256 values. This adds
six matching C instances / 8,976 instruction bytes while retaining eight
functions / 20,128 bytes as generated assembly and 11,856 bytes as raw data.
The [attempt ledger](japanese-model-variant450-attempts.csv) records the
object and production checks; the
[instance table](japanese-model-variant450-instances.csv) records the loader
slices and complete-image hashes.

The three helpers import fifteen distinct resident functions. Every Japanese
binding was recovered independently by pairing a C-object relocation with
the retail JAL and checking the destination against `functions.csv`.
Three canonical SDK names map to address-based resident inventory names:
`GsGetLs` to `func_80084EC0`, `ReadRotMatrix` to `func_80085F10`, and the
projection helpers to `func_800864D0`, `func_80086500`, and `func_800865C0`.
These are exact function starts rather than inferred cross-region offsets.

This is exact ownership evidence, not a global compiler claim. The four
remaining Japanese MODEL450 functions still require independent recovery.
Reusing accepted bodies does not extend the bounded structure, allocation,
lifetime, geometry, or reachability claims documented by the Spanish work.
