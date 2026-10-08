# Italian overlay build configuration

Run `make verify-italian-inputs`, then `make italian-match-overlays`, then
`make italian-verify-overlays` from the repository root. Inputs are immutable
`SLES_039.50`, `DATA/SU.MRG` and `DATA/WA_MRG.MRG` under `game/italy/`.
Their sizes and hashes are pinned in `../target.yaml` and `../files.sha256`.

The six original non-MODEL payloads independently extracted from the Italian archives are
byte-identical to the corresponding Spanish payloads, at the same sector
offsets and load addresses. The complete modules are rebuilt and compared
individually, not accepted solely from that cross-region resemblance.
They reuse the existing European C sources and named GCC 2.8.1/MASPSX 2.81
profiles, with the verified Spanish resident bindings.

| Module | Archive | First sector | Sectors | Matching functions | C bytes |
|---|---|---:|---:|---:|---:|
| `free_duel` | `WA_MRG.MRG` | 9304 | 5 | 9 | 4252 |
| `main_menu` | `SU.MRG` | 98 | 16 | 31 | 18280 |
| `overworld_after_coup` | `WA_MRG.MRG` | 9920 | 6 | 15 | 6184 |
| `overworld_before_coup` | `WA_MRG.MRG` | 9762 | 6 | 15 | 6184 |
| `password_a` | `WA_MRG.MRG` | 9374 | 15 | 27 | 10476 |
| `password_b` | `WA_MRG.MRG` | 9460 | 15 | 27 | 10476 |
| Original six subtotal | | | 63 | 124 | 55852 |

Sector size is 2048 bytes. Main-menu code loads at `0x80180000`; all other
modules load at `0x80168000`. Counts describe module instances, not 124 new
function implementations. Raw data and jump-table ownership remain intact.
The legacy overworld generated-assembly window at offsets `0x1618..0x17D0`
is preserved, not claimed as C or newly classified as handwritten assembly.
It begins within an instruction sequence rather than at a function prologue;
the accepted shared inventories do not list it as a function.
Password language accesses retain the measured binding
`D_8009C02B = 0x8009C44B`.

The combined Italian CI workflow uses `YGOFM_SLES_03950_URL`,
`YGOFM_ITA_SU_MRG_URL` and `YGOFM_ITA_WA_MRG_URL`. The progress generator
validates each inventory against its matching-C manifest; #443 snapshots
remain separate from ordinary matching work.

## MODEL450 stale-PR maintenance

The two existing raw-image registrations at sectors `48224`/`48234` now select
the four verified MODEL450 ribbon, band, quad and line C helpers in each slot.
This refreshes #6996 against the current raw-image inventory rather than
creating duplicate canonical modules. Physical module count remains 3,584.
It adds eight matching-C instances and 13,440 bytes; the three other functions
per image remain generated assembly and the suffix from `0x3940` stays raw.
The same four selections are already accepted in French, so no French source
or registration changes are needed.

See [maintenance evidence](../../../notes/overlays/italian-model-variant450-takeover.md)
for complete-image ownership checks and the preserved original PR.
