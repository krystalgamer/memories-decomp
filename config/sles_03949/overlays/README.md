# German overlay build configuration

Run `make german-match-overlays`, then `make german-verify-overlays` from
the repository root. Immutable archive inputs are `DATA/SU.MRG` and
`DATA/WA_MRG.MRG` under `game/germany/`. Their actual archive hashes and
module slice boundaries are pinned in `../overlays.json`. The overlay gate
is independent of the resident executable build.

The German WA archive SHA-256 is
`fbe294274a0c88fd70f1ea94a85ef6b2a6b5e4e9b5fd0c98eabdf687614d6fc6`,
not the Spanish/Italian whole-archive hash. The six original non-MODEL slices are
nevertheless byte-identical to their Spanish counterparts. Each complete
module is independently rebuilt and compared with its German archive slice,
reusing the accepted European/shared C and unchanged compiler profiles.

## MODEL450 stale-PR maintenance

The reviewed #6997 selections now use the existing raw-image registrations at
sectors `48224`/`48234`, not duplicate canonical MODEL174 modules. Four helpers
in each slot select unchanged accepted French/Spanish C. Physical module count
remains 3,584; eight C instances add 13,440 bytes. The three remaining functions
per image stay generated assembly and the suffix from `0x3940` remains raw.
German image notes retain the repair already made at the original PR head.
All selected helpers are already accepted in French; no new French port is needed.

See [takeover evidence](../../../notes/overlays/german-model-variant450-takeover.md).

| Module | Archive | First sector | Sectors | Matching functions | C bytes |
|---|---|---:|---:|---:|---:|
| `free_duel` | `WA_MRG.MRG` | 9304 | 5 | 9 | 4252 |
| `main_menu` | `SU.MRG` | 98 | 16 | 31 | 18280 |
| `overworld_after_coup` | `WA_MRG.MRG` | 9920 | 6 | 15 | 6184 |
| `overworld_before_coup` | `WA_MRG.MRG` | 9762 | 6 | 15 | 6184 |
| `password_a` | `WA_MRG.MRG` | 9374 | 15 | 27 | 10476 |
| `password_b` | `WA_MRG.MRG` | 9460 | 15 | 27 | 10476 |
| Total | | | 63 | 124 | 55852 |

Sector size is 2048 bytes. Main menu loads at `0x80180000`; the remaining
modules load at `0x80168000`. Counts describe module instances rather than
124 newly written functions. Raw data and jump-table ownership remain intact.

The legacy overworld generated-assembly window at `0x1618..0x17D0` is
preserved, not claimed as C or newly classified as handwritten assembly.
It starts within an instruction sequence rather than at a function prologue;
the accepted shared inventories do not list it as a function. Password
language accesses retain `D_8009C02B = 0x8009C44B`.

Overlay CI uses `YGOFM_GER_SU_MRG_URL` and `YGOFM_GER_WA_MRG_URL`.
The separate resident pipeline uses `YGOFM_SLES_03949_URL`.
Reporting validates every inventory against its
matching-C manifest; #443 snapshots remain separate from matching work.
