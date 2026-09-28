# Spanish overlay build configuration

Run `make verify-spanish-inputs` and `make spanish-match-overlays` from the
repository root. The inputs are the unmodified Spanish `SLES_039.51`,
`DATA/SU.MRG` and `DATA/WA_MRG.MRG`, placed under `game/spain/`; their sizes and
hashes are pinned in `../target.yaml` and `../files.sha256`. The Spanish CI
workflow uses `YGOFM_SLES_03951_URL`, `YGOFM_ESP_SU_MRG_URL` and
`YGOFM_ESP_WA_MRG_URL`.

The original six independently linked runtime modules reuse the existing European C
sources and named GCC 2.8.1/MASPSX 2.81 profiles unchanged. Only the resident
symbol bindings and retail archive/module hashes differ. No Spanish C copies,
new inline assembly, or source-local external declarations are needed.

| Module | Archive | First sector | Sectors | Matching functions | C bytes |
|---|---|---:|---:|---:|---:|
| `free_duel` | `WA_MRG.MRG` | 9304 | 5 | 9 | 4252 |
| `main_menu` | `SU.MRG` | 98 | 16 | 31 | 18280 |
| `overworld_after_coup` | `WA_MRG.MRG` | 9920 | 6 | 15 | 6184 |
| `overworld_before_coup` | `WA_MRG.MRG` | 9762 | 6 | 15 | 6184 |
| `password_a` | `WA_MRG.MRG` | 9374 | 15 | 27 | 10476 |
| `password_b` | `WA_MRG.MRG` | 9460 | 15 | 27 | 10476 |
| `duel_effects` | `WA_MRG.MRG` | 7193 | 44 | 19 / 85 | 3548 |
| Total configured | | | 107 | 143 / 209 | 59400 |

Sector sizes are 2048 bytes. Counts are per module instance: the overworld and
password variants share code but have different archive slices and full-image
hashes. These are not 124 distinct new C implementations. Main-menu code loads
at `0x80180000`; the other five original modules load at `0x80168000`.

The added duel-effect bank instead loads at `0x80146000`. Its representative
image is 90,112 bytes; all seven terrain copies at `7193 + terrain * 240`
have SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
`duplicate_sector_offsets` checks every complete copy before building the
representative, without multiplying identical inventories. The table's sector
total counts representative images only.

This bank reuses the accepted shared `duel_effects/utility_helpers.c` and
`duel_effects/texture_words.c` and `duel_effects/vector_init.c` units with
`gcc_2_8_1_g0_split` unchanged. Two additional complete groups,
`number_helpers.c` and `primitive_draw.c`, add four functions / 964 bytes
using that same profile. The 19 exact C functions cover 3,548 bytes.
The complete text interval
`0x80146258..0x8015A1E4` contains 85 provisional boundaries / 81,804 bytes;
the other 66 stay generated assembly and visible in progress. External C
bindings for `rand`, `RotAverage3`, `RotAverage4`, `GsSortPoly` and the
resident packet submitter agree with the Spanish resident inventory.
The ordering-table pointer remains defined by preserved module data; no raw
data is claimed as C-owned storage.

The loader sequence and region-independent bank layout are documented in
[duel-effect bank research](../../../notes/overlays/duel-effect-bank.md).
All seven Spanish copies and the boot module were independently compared,
not inferred from French alone. The boot image at WA sector 9509 (three
sectors, load `0x80168000`) is still unregistered pending ownership analysis.
MODEL/SU loads and the preserved overworld fragment also require further
coverage work. These seven configured modules are not an exhaustive runtime
completion claim.

Spanish instruction words at 2205 relocation sites in the verified European
objects recover 332 symbol/address bindings. Consistency across all uses,
unchanged opcode bits, and final full-module SHA-256 matches validate those
bindings. The issue's `eur_esp.csv` is supplementary evidence, not an authority:
it incorrectly maps European `GsSetProjection` at `0x80085544` to `0x8004AB68`,
whereas the Spanish calls require `0x80085748`; European `DisplayObject_Reset`
at `0x80040514` similarly requires `0x80040714`, not `0x8008B768`.

Initial linkage exposed an implicit European auto-symbol, `D_8009C02B`, absent
from the password symbol files. The Spanish linker files explicitly bind that
shared C name to the recovered `0x8009C44B`; all six full modules then match.

The original layouts are preserved, including raw data, jump-table ownership,
and the overworld assembly range at offsets `0x1618-0x17D0`. That range is not
claimed as matching C. Retail archives, extracted modules, generated assembly,
object files, and relocation probes remain ignored under `game/` and `tmp/`.

The progress generator includes these inventories alongside the Spanish
resident metrics. `make spanish-match` verifies the independent resident
executable; `make spanish-match-overlays` verifies these runtime modules.
Progress snapshots are refreshed separately under #443.
