# German overlay build configuration

Run `make german-match-overlays`, then `make german-verify-overlays` from
the repository root. Immutable archive inputs are `DATA/SU.MRG`,
`DATA/WA_MRG.MRG`, and `DATA/MODEL.MRG` under `game/germany/`. Their actual archive hashes and
module slice boundaries are pinned in `../overlays.json`. The overlay gate
is independent of the resident executable build.

The German WA archive SHA-256 is
`fbe294274a0c88fd70f1ea94a85ef6b2a6b5e4e9b5fd0c98eabdf687614d6fc6`,
not the Spanish/Italian whole-archive hash. All eight German module slices are
nevertheless byte-identical to their Spanish counterparts. Each complete
module is independently rebuilt and compared with its German archive slice,
reusing the accepted European/shared C and unchanged compiler profiles.

| Module | Archive | First sector | Sectors | Matching functions | C bytes |
|---|---|---:|---:|---:|---:|
| `free_duel` | `WA_MRG.MRG` | 9304 | 5 | 9 | 4252 |
| `main_menu` | `SU.MRG` | 98 | 16 | 31 | 18280 |
| `model_variant_174_stage9_slot0` | `MODEL.MRG` | 48224 | 10 | 4 | 6720 |
| `model_variant_174_stage10_slot1` | `MODEL.MRG` | 48234 | 10 | 4 | 6720 |
| `overworld_after_coup` | `WA_MRG.MRG` | 9920 | 6 | 15 | 6184 |
| `overworld_before_coup` | `WA_MRG.MRG` | 9762 | 6 | 15 | 6184 |
| `password_a` | `WA_MRG.MRG` | 9374 | 15 | 27 | 10476 |
| `password_b` | `WA_MRG.MRG` | 9460 | 15 | 27 | 10476 |
| Total | | | 83 | 132 | 69292 |

Sector size is 2048 bytes. Main menu loads at `0x80180000`; the MODEL450
images load at `0x8013B000` and `0x8017B000`; the remaining modules load at
`0x80168000`. Counts describe module instances rather than 132 newly written
functions. Raw data and jump-table ownership remain intact.

The legacy overworld generated-assembly window at `0x1618..0x17D0` is
preserved, not claimed as C or newly classified as handwritten assembly.
It starts within an instruction sequence rather than at a function prologue;
the accepted shared inventories do not list it as a function. Password
language accesses retain `D_8009C02B = 0x8009C44B`.

The two MODEL450 images select exact shared primary-ribbon, three-band, quad,
and line helpers. Their entry, secondary helper, final ribbon helper, and
complete `0x3940..0x5000` suffix remain assembly/raw owners. The complete
German `MODEL.MRG` is byte-identical to the Spanish and Italian archives, but
the German images are still independently extracted, compiled, linked, and
hashed; archive identity is not treated as C ownership by itself.
[The instance table](../../../notes/overlays/german-model-variant450-instances.csv)
records the exact archive slices, while the
[attempt ledger](../../../notes/overlays/german-model-variant450-attempts.csv)
records all eight terminal target selections.

Overlay CI uses `YGOFM_GER_SU_MRG_URL` and `YGOFM_GER_WA_MRG_URL`.
The separate resident pipeline uses `YGOFM_SLES_03949_URL`.
Reporting validates every inventory against its
matching-C manifest; #443 snapshots remain separate from matching work.
