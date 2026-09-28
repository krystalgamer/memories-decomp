# Spanish overlay build configuration

Run `make verify-spanish-inputs` and `make spanish-match-overlays` from the
repository root. The inputs are the unmodified Spanish `SLES_039.51`,
`DATA/SU.MRG` and `DATA/WA_MRG.MRG`, placed under `game/spain/`; their sizes and
hashes are pinned in `../target.yaml` and `../files.sha256`. The Spanish CI
workflow uses `YGOFM_SLES_03951_URL`, `YGOFM_ESP_SU_MRG_URL` and
`YGOFM_ESP_WA_MRG_URL`.

These six independently linked runtime modules reuse the existing European C
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
| Total | | | 63 | 124 | 55852 |

Sector sizes are 2048 bytes. Counts are per module instance: the overworld and
password variants share code but have different archive slices and full-image
hashes. These are not 124 distinct new C implementations. Main-menu code loads
at `0x80180000`; the remaining modules load at `0x80168000`.

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

The progress generator includes these inventories in a separate Spanish
overlay section and JSON object. Resident Spanish matching is not yet
configured or counted; a later resident change must establish its own complete
executable match. Progress snapshots are refreshed separately under #443.
