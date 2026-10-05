# Yu-Gi-Oh! Forbidden Memories Decompilation

[![Matching build](https://github.com/krystalgamer/memories-decomp/actions/workflows/matching-build.yml/badge.svg)](https://github.com/krystalgamer/memories-decomp/actions/workflows/matching-build.yml)

This repository is a byte-matching decompilation of the North American
(`SLUS-01411`), Japanese (`SLPM-86398`), European (`SLES-03947`), French
(`SLES-03948`), German (`SLES-03949`), Italian (`SLES-03950`), and Spanish
(`SLES-03951`) PlayStation releases of
**Yu-Gi-Oh! Forbidden Memories**. Accepted changes must continue to rebuild
each supported PS-X executable exactly.

The French target's shared-C matches, input requirements, and clean
build command are documented in [French matching](notes/french-matching.md).
The generated snapshot includes its configured overlay inventories but does
not yet report French resident metrics.

> [!IMPORTANT]
> The repository does not contain game data or proprietary Psy-Q tools. Supply
> legally obtained copies of the required files beneath `game/`; they remain
> ignored by Git.

## Project status

The generated report covers configured resident and overlay inventories, not
an exhaustive census of runtime code. A 100% resident row does not establish
complete overlay coverage. The newly identified
[duel-effect and boot banks](notes/overlays/duel-effect-bank.md) also exist in
Spanish, Italian, and German; they are not yet fully represented in those
regions' overlay tables, and their decompilation remains in progress.

<!-- BEGIN GENERATED PROGRESS -->

### North American (`SLUS-01411`)

Target SHA-256: `84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`

| Metric | Current |
|---|---:|
| Game C-decompilation targets matched | **1,134 / 1,134 (100.00%)** |
| Game C-decompilation target bytes matched | **356,080 (`0x56EF0`) / 356,080 (`0x56EF0`) (100.00%)** |
| Remaining game C-decompilation targets | 0 functions, 0 (`0x0`) |
| Evidence-backed handwritten game assembly | 61 functions, 40,116 (`0x9CB4`) |
| Total game-owned functions | 1,195 |
| Preserved Psy-Q CRT/SDK assembly | 591 functions, 117,348 (`0x1CA64`) |
| Total discovered functions | 1,786 |
| Embedded/unassigned resident text | 1,780 (`0x6F4`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `credits` | 6 / 6 (100.00%) | 5,512 (`0x1588`) / 5,512 (`0x1588`) (100.00%) |
| `duel_effects` | 85 / 85 (100.00%) | 81,856 (`0x13FC0`) / 81,856 (`0x13FC0`) (100.00%) |
| `free_duel` | 9 / 9 (100.00%) | 4,140 (`0x102C`) / 4,140 (`0x102C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 17,724 (`0x453C`) / 17,724 (`0x453C`) (100.00%) |
| `model_primary_116_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_116_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_141_slot0` | 1 / 1 (100.00%) | 2,448 (`0x990`) / 2,448 (`0x990`) (100.00%) |
| `model_primary_141_slot1` | 1 / 1 (100.00%) | 2,448 (`0x990`) / 2,448 (`0x990`) (100.00%) |
| `model_primary_150_slot0` | 1 / 1 (100.00%) | 276 (`0x114`) / 276 (`0x114`) (100.00%) |
| `model_primary_150_slot1` | 1 / 1 (100.00%) | 276 (`0x114`) / 276 (`0x114`) (100.00%) |
| `model_primary_167_slot0` | 1 / 1 (100.00%) | 276 (`0x114`) / 276 (`0x114`) (100.00%) |
| `model_primary_167_slot1` | 1 / 1 (100.00%) | 276 (`0x114`) / 276 (`0x114`) (100.00%) |
| `model_primary_370_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_370_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_394_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_394_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_416_slot0` | 1 / 1 (100.00%) | 2,564 (`0xA04`) / 2,564 (`0xA04`) (100.00%) |
| `model_primary_416_slot1` | 1 / 1 (100.00%) | 2,564 (`0xA04`) / 2,564 (`0xA04`) (100.00%) |
| `model_primary_707_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_707_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_715_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_715_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_8_slot0` | 1 / 1 (100.00%) | 1,572 (`0x624`) / 1,572 (`0x624`) (100.00%) |
| `model_primary_8_slot1` | 1 / 1 (100.00%) | 1,572 (`0x624`) / 1,572 (`0x624`) (100.00%) |
| `model_variant_102_stage10_slot1` | 5 / 6 (83.33%) | 10,728 (`0x29E8`) / 12,252 (`0x2FDC`) (87.56%) |
| `model_variant_102_stage9_slot0` | 5 / 6 (83.33%) | 10,728 (`0x29E8`) / 12,252 (`0x2FDC`) (87.56%) |
| `model_variant_108_pos0_slot0` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_108_stage10_slot1` | 7 / 9 (77.78%) | 8,656 (`0x21D0`) / 17,376 (`0x43E0`) (49.82%) |
| `model_variant_108_stage8_slot1` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_108_stage9_slot0` | 7 / 9 (77.78%) | 8,656 (`0x21D0`) / 17,376 (`0x43E0`) (49.82%) |
| `model_variant_110_stage10_slot1` | 4 / 4 (100.00%) | 8,000 (`0x1F40`) / 8,000 (`0x1F40`) (100.00%) |
| `model_variant_110_stage9_slot0` | 4 / 4 (100.00%) | 8,000 (`0x1F40`) / 8,000 (`0x1F40`) (100.00%) |
| `model_variant_114_stage10_slot1` | 8 / 9 (88.89%) | 13,412 (`0x3464`) / 16,632 (`0x40F8`) (80.64%) |
| `model_variant_114_stage9_slot0` | 8 / 9 (88.89%) | 13,412 (`0x3464`) / 16,632 (`0x40F8`) (80.64%) |
| `model_variant_116_pos0_slot0` | 6 / 6 (100.00%) | 11,472 (`0x2CD0`) / 11,472 (`0x2CD0`) (100.00%) |
| `model_variant_116_stage8_slot1` | 6 / 6 (100.00%) | 11,472 (`0x2CD0`) / 11,472 (`0x2CD0`) (100.00%) |
| `model_variant_124_pos0_slot0` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_124_stage8_slot1` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_125_pos0_slot0` | 3 / 6 (50.00%) | 5,064 (`0x13C8`) / 19,720 (`0x4D08`) (25.68%) |
| `model_variant_125_stage8_slot1` | 3 / 6 (50.00%) | 5,064 (`0x13C8`) / 19,720 (`0x4D08`) (25.68%) |
| `model_variant_138_pos0_slot0` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_138_stage8_slot1` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_152_stage10_slot1` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_152_stage9_slot0` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_159_stage10_slot1` | 4 / 4 (100.00%) | 8,000 (`0x1F40`) / 8,000 (`0x1F40`) (100.00%) |
| `model_variant_159_stage9_slot0` | 4 / 4 (100.00%) | 8,000 (`0x1F40`) / 8,000 (`0x1F40`) (100.00%) |
| `model_variant_161_stage10_slot1` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_161_stage9_slot0` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_162_pos0_slot0` | 8 / 9 (88.89%) | 13,412 (`0x3464`) / 16,632 (`0x40F8`) (80.64%) |
| `model_variant_162_stage8_slot1` | 8 / 9 (88.89%) | 13,412 (`0x3464`) / 16,632 (`0x40F8`) (80.64%) |
| `model_variant_164_pos0_slot0` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_164_stage8_slot1` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_165_pos0_slot0` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_165_stage10_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_165_stage8_slot1` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_165_stage9_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_166_stage10_slot1` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_166_stage9_slot0` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_168_stage10_slot1` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_168_stage9_slot0` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_170_stage10_slot1` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_170_stage9_slot0` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_174_stage10_slot1` | 2 / 7 (28.57%) | 2,388 (`0x954`) / 14,552 (`0x38D8`) (16.41%) |
| `model_variant_174_stage9_slot0` | 2 / 7 (28.57%) | 2,388 (`0x954`) / 14,552 (`0x38D8`) (16.41%) |
| `model_variant_180_pos0_slot0` | 8 / 8 (100.00%) | 12,492 (`0x30CC`) / 12,492 (`0x30CC`) (100.00%) |
| `model_variant_180_stage8_slot1` | 8 / 8 (100.00%) | 12,492 (`0x30CC`) / 12,492 (`0x30CC`) (100.00%) |
| `model_variant_182_pos0_slot0` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_182_stage8_slot1` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_184_stage10_slot1` | 8 / 9 (88.89%) | 13,412 (`0x3464`) / 16,632 (`0x40F8`) (80.64%) |
| `model_variant_184_stage9_slot0` | 8 / 9 (88.89%) | 13,412 (`0x3464`) / 16,632 (`0x40F8`) (80.64%) |
| `model_variant_185_pos0_slot0` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_185_stage8_slot1` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_186_pos0_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_186_stage8_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_187_pos0_slot0` | 8 / 8 (100.00%) | 13,720 (`0x3598`) / 13,720 (`0x3598`) (100.00%) |
| `model_variant_187_stage8_slot1` | 8 / 8 (100.00%) | 13,720 (`0x3598`) / 13,720 (`0x3598`) (100.00%) |
| `model_variant_190_stage10_slot1` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_190_stage9_slot0` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_193_pos0_slot0` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_193_stage8_slot1` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_1_pos0_slot0` | 9 / 9 (100.00%) | 14,412 (`0x384C`) / 14,412 (`0x384C`) (100.00%) |
| `model_variant_1_stage8_slot1` | 9 / 9 (100.00%) | 14,412 (`0x384C`) / 14,412 (`0x384C`) (100.00%) |
| `model_variant_20_pos0_slot0` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_20_stage8_slot1` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_210_pos0_slot0` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_210_stage8_slot1` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_217_stage10_slot1` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_217_stage9_slot0` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_221_stage10_slot1` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_221_stage9_slot0` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_239_stage10_slot1` | 8 / 8 (100.00%) | 13,720 (`0x3598`) / 13,720 (`0x3598`) (100.00%) |
| `model_variant_239_stage9_slot0` | 8 / 8 (100.00%) | 13,720 (`0x3598`) / 13,720 (`0x3598`) (100.00%) |
| `model_variant_242_stage10_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_242_stage9_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_259_pos0_slot0` | 9 / 10 (90.00%) | 12,932 (`0x3284`) / 17,400 (`0x43F8`) (74.32%) |
| `model_variant_259_stage8_slot1` | 9 / 10 (90.00%) | 12,932 (`0x3284`) / 17,400 (`0x43F8`) (74.32%) |
| `model_variant_262_stage10_slot1` | 6 / 7 (85.71%) | 7,200 (`0x1C20`) / 11,060 (`0x2B34`) (65.10%) |
| `model_variant_262_stage9_slot0` | 6 / 7 (85.71%) | 7,200 (`0x1C20`) / 11,060 (`0x2B34`) (65.10%) |
| `model_variant_275_stage10_slot1` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_275_stage9_slot0` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_279_pos0_slot0` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_279_stage8_slot1` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_282_stage10_slot1` | 5 / 6 (83.33%) | 10,728 (`0x29E8`) / 12,252 (`0x2FDC`) (87.56%) |
| `model_variant_282_stage9_slot0` | 5 / 6 (83.33%) | 10,728 (`0x29E8`) / 12,252 (`0x2FDC`) (87.56%) |
| `model_variant_288_stage10_slot1` | 5 / 6 (83.33%) | 10,728 (`0x29E8`) / 12,252 (`0x2FDC`) (87.56%) |
| `model_variant_288_stage9_slot0` | 5 / 6 (83.33%) | 10,728 (`0x29E8`) / 12,252 (`0x2FDC`) (87.56%) |
| `model_variant_290_stage7_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_290_stage8_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_294_stage10_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_294_stage9_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_295_stage7_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_295_stage8_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_296_stage10_slot1` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_296_stage9_slot0` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_297_pos0_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_297_stage8_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_2_pos0_slot0` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_2_stage8_slot1` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_31_stage10_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_31_stage9_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_34_pos0_slot0` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_34_stage10_slot1` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_34_stage8_slot1` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_34_stage9_slot0` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_352_stage10_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_352_stage9_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_358_stage10_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_358_stage9_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_361_pos0_slot0` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_361_stage10_slot1` | 8 / 8 (100.00%) | 13,720 (`0x3598`) / 13,720 (`0x3598`) (100.00%) |
| `model_variant_361_stage8_slot1` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_361_stage9_slot0` | 8 / 8 (100.00%) | 13,720 (`0x3598`) / 13,720 (`0x3598`) (100.00%) |
| `model_variant_367_stage10_slot1` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_367_stage9_slot0` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_368_stage10_slot1` | 8 / 8 (100.00%) | 13,720 (`0x3598`) / 13,720 (`0x3598`) (100.00%) |
| `model_variant_368_stage9_slot0` | 8 / 8 (100.00%) | 13,720 (`0x3598`) / 13,720 (`0x3598`) (100.00%) |
| `model_variant_369_stage10_slot1` | 8 / 9 (88.89%) | 13,412 (`0x3464`) / 16,632 (`0x40F8`) (80.64%) |
| `model_variant_369_stage9_slot0` | 8 / 9 (88.89%) | 13,412 (`0x3464`) / 16,632 (`0x40F8`) (80.64%) |
| `model_variant_370_stage10_slot1` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_370_stage9_slot0` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_376_pos0_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_376_stage8_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_388_stage10_slot1` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_388_stage9_slot0` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_391_pos0_slot0` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_391_stage8_slot1` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_395_stage10_slot1` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_395_stage9_slot0` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_399_stage10_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_399_stage9_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_400_stage10_slot1` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_400_stage9_slot0` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_401_pos0_slot0` | 6 / 7 (85.71%) | 8,936 (`0x22E8`) / 12,116 (`0x2F54`) (73.75%) |
| `model_variant_401_stage10_slot1` | 3 / 5 (60.00%) | 3,120 (`0xC30`) / 8,056 (`0x1F78`) (38.73%) |
| `model_variant_401_stage8_slot1` | 6 / 7 (85.71%) | 8,936 (`0x22E8`) / 12,116 (`0x2F54`) (73.75%) |
| `model_variant_401_stage9_slot0` | 3 / 5 (60.00%) | 3,120 (`0xC30`) / 8,056 (`0x1F78`) (38.73%) |
| `model_variant_408_stage10_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_408_stage9_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_410_stage10_slot1` | 6 / 8 (75.00%) | 6,592 (`0x19C0`) / 12,576 (`0x3120`) (52.42%) |
| `model_variant_410_stage7_slot0` | 4 / 4 (100.00%) | 8,000 (`0x1F40`) / 8,000 (`0x1F40`) (100.00%) |
| `model_variant_410_stage8_slot1` | 4 / 4 (100.00%) | 8,000 (`0x1F40`) / 8,000 (`0x1F40`) (100.00%) |
| `model_variant_410_stage9_slot0` | 6 / 8 (75.00%) | 6,592 (`0x19C0`) / 12,576 (`0x3120`) (52.42%) |
| `model_variant_424_pos0_slot0` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_424_stage8_slot1` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_427_pos0_slot0` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_427_stage10_slot1` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_427_stage8_slot1` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_427_stage9_slot0` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_436_pos0_slot0` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_436_stage8_slot1` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_440_pos0_slot0` | 8 / 8 (100.00%) | 12,492 (`0x30CC`) / 12,492 (`0x30CC`) (100.00%) |
| `model_variant_440_stage8_slot1` | 8 / 8 (100.00%) | 12,492 (`0x30CC`) / 12,492 (`0x30CC`) (100.00%) |
| `model_variant_443_stage10_slot1` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_443_stage9_slot0` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_44_stage10_slot1` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_44_stage9_slot0` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_457_stage10_slot1` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_457_stage9_slot0` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_458_pos0_slot0` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_458_stage10_slot1` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_458_stage8_slot1` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_458_stage9_slot0` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_459_pos0_slot0` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_459_stage10_slot1` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_459_stage8_slot1` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_459_stage9_slot0` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_460_pos0_slot0` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_460_stage8_slot1` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_462_stage10_slot1` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_462_stage9_slot0` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_465_stage10_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_465_stage9_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_469_pos0_slot0` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_469_stage10_slot1` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_469_stage8_slot1` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_469_stage9_slot0` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_478_stage10_slot1` | 8 / 8 (100.00%) | 13,720 (`0x3598`) / 13,720 (`0x3598`) (100.00%) |
| `model_variant_478_stage9_slot0` | 8 / 8 (100.00%) | 13,720 (`0x3598`) / 13,720 (`0x3598`) (100.00%) |
| `model_variant_491_pos0_slot0` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_491_stage8_slot1` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_501_stage7_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_501_stage8_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_504_pos0_slot0` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_504_stage8_slot1` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_518_stage7_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_518_stage8_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_520_stage10_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_520_stage9_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_531_stage10_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_531_stage9_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_54_stage10_slot1` | 3 / 3 (100.00%) | 5,224 (`0x1468`) / 5,224 (`0x1468`) (100.00%) |
| `model_variant_54_stage9_slot0` | 3 / 3 (100.00%) | 5,224 (`0x1468`) / 5,224 (`0x1468`) (100.00%) |
| `model_variant_550_pos0_slot0` | 9 / 9 (100.00%) | 14,412 (`0x384C`) / 14,412 (`0x384C`) (100.00%) |
| `model_variant_550_stage8_slot1` | 9 / 9 (100.00%) | 14,412 (`0x384C`) / 14,412 (`0x384C`) (100.00%) |
| `model_variant_552_pos0_slot0` | 7 / 7 (100.00%) | 11,616 (`0x2D60`) / 11,616 (`0x2D60`) (100.00%) |
| `model_variant_552_stage8_slot1` | 7 / 7 (100.00%) | 11,616 (`0x2D60`) / 11,616 (`0x2D60`) (100.00%) |
| `model_variant_558_stage10_slot1` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_558_stage9_slot0` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_573_pos0_slot0` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_573_stage10_slot1` | 7 / 9 (77.78%) | 8,656 (`0x21D0`) / 17,376 (`0x43E0`) (49.82%) |
| `model_variant_573_stage8_slot1` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_573_stage9_slot0` | 7 / 9 (77.78%) | 8,656 (`0x21D0`) / 17,376 (`0x43E0`) (49.82%) |
| `model_variant_576_pos0_slot0` | 6 / 6 (100.00%) | 11,472 (`0x2CD0`) / 11,472 (`0x2CD0`) (100.00%) |
| `model_variant_576_stage8_slot1` | 6 / 6 (100.00%) | 11,472 (`0x2CD0`) / 11,472 (`0x2CD0`) (100.00%) |
| `model_variant_580_pos0_slot0` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_580_stage8_slot1` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_590_stage10_slot1` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_590_stage9_slot0` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_594_pos0_slot0` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_594_stage8_slot1` | 5 / 6 (83.33%) | 10,188 (`0x27CC`) / 11,428 (`0x2CA4`) (89.15%) |
| `model_variant_595_pos0_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_595_stage8_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_596_pos0_slot0` | 8 / 8 (100.00%) | 13,720 (`0x3598`) / 13,720 (`0x3598`) (100.00%) |
| `model_variant_596_stage8_slot1` | 8 / 8 (100.00%) | 13,720 (`0x3598`) / 13,720 (`0x3598`) (100.00%) |
| `model_variant_598_stage10_slot1` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_598_stage9_slot0` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_609_pos0_slot0` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_609_stage8_slot1` | 5 / 5 (100.00%) | 9,868 (`0x268C`) / 9,868 (`0x268C`) (100.00%) |
| `model_variant_612_stage10_slot1` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_612_stage9_slot0` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_621_stage10_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_621_stage9_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_630_pos0_slot0` | 9 / 10 (90.00%) | 12,932 (`0x3284`) / 17,400 (`0x43F8`) (74.32%) |
| `model_variant_630_stage8_slot1` | 9 / 10 (90.00%) | 12,932 (`0x3284`) / 17,400 (`0x43F8`) (74.32%) |
| `model_variant_631_stage10_slot1` | 6 / 7 (85.71%) | 7,200 (`0x1C20`) / 11,060 (`0x2B34`) (65.10%) |
| `model_variant_631_stage9_slot0` | 6 / 7 (85.71%) | 7,200 (`0x1C20`) / 11,060 (`0x2B34`) (65.10%) |
| `model_variant_640_pos0_slot0` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_640_stage8_slot1` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_642_stage10_slot1` | 5 / 6 (83.33%) | 10,728 (`0x29E8`) / 12,252 (`0x2FDC`) (87.56%) |
| `model_variant_642_stage9_slot0` | 5 / 6 (83.33%) | 10,728 (`0x29E8`) / 12,252 (`0x2FDC`) (87.56%) |
| `model_variant_645_stage10_slot1` | 5 / 6 (83.33%) | 10,728 (`0x29E8`) / 12,252 (`0x2FDC`) (87.56%) |
| `model_variant_645_stage9_slot0` | 5 / 6 (83.33%) | 10,728 (`0x29E8`) / 12,252 (`0x2FDC`) (87.56%) |
| `model_variant_647_stage10_slot1` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_647_stage9_slot0` | 4 / 4 (100.00%) | 9,012 (`0x2334`) / 9,012 (`0x2334`) (100.00%) |
| `model_variant_68_pos0_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_68_stage8_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_704_pos0_slot0` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_704_stage8_slot1` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_70_pos0_slot0` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_70_stage8_slot1` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_712_stage10_slot1` | 6 / 7 (85.71%) | 8,152 (`0x1FD8`) / 13,108 (`0x3334`) (62.19%) |
| `model_variant_712_stage9_slot0` | 6 / 7 (85.71%) | 8,152 (`0x1FD8`) / 13,108 (`0x3334`) (62.19%) |
| `model_variant_71_pos0_slot0` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_71_stage8_slot1` | 9 / 9 (100.00%) | 14,696 (`0x3968`) / 14,696 (`0x3968`) (100.00%) |
| `model_variant_7_pos0_slot0` | 7 / 7 (100.00%) | 11,616 (`0x2D60`) / 11,616 (`0x2D60`) (100.00%) |
| `model_variant_7_stage8_slot1` | 7 / 7 (100.00%) | 11,616 (`0x2D60`) / 11,616 (`0x2D60`) (100.00%) |
| `model_variant_84_pos0_slot0` | 8 / 9 (88.89%) | 13,412 (`0x3464`) / 16,632 (`0x40F8`) (80.64%) |
| `model_variant_84_stage8_slot1` | 8 / 9 (88.89%) | 13,412 (`0x3464`) / 16,632 (`0x40F8`) (80.64%) |
| `model_variant_87_pos0_slot0` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_87_stage8_slot1` | 7 / 7 (100.00%) | 11,744 (`0x2DE0`) / 11,744 (`0x2DE0`) (100.00%) |
| `model_variant_88_stage10_slot1` | 8 / 9 (88.89%) | 13,412 (`0x3464`) / 16,632 (`0x40F8`) (80.64%) |
| `model_variant_88_stage9_slot0` | 8 / 9 (88.89%) | 13,412 (`0x3464`) / 16,632 (`0x40F8`) (80.64%) |
| `model_variant_96_pos0_slot0` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_96_stage8_slot1` | 2 / 2 (100.00%) | 6,340 (`0x18C4`) / 6,340 (`0x18C4`) (100.00%) |
| `model_variant_98_stage10_slot1` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `model_variant_98_stage9_slot0` | 5 / 5 (100.00%) | 10,372 (`0x2884`) / 10,372 (`0x2884`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password` | 27 / 27 (100.00%) | 10,884 (`0x2A84`) / 10,884 (`0x2A84`) (100.00%) |

_Generated from `config/slus_01411/functions.csv` and `config/slus_01411/overlays/*_functions.csv` by `tools/project/progress.py`._

### Japanese (`SLPM-86398`)

Target SHA-256: `ee3f45584fb747fd33c9560f0fc68ced03b399fbd9a2e9d6a71eb0f5daa89585`

| Metric | Current |
|---|---:|
| Game C-decompilation targets matched | **1,135 / 1,135 (100.00%)** |
| Game C-decompilation target bytes matched | **351,252 (`0x55C14`) / 351,252 (`0x55C14`) (100.00%)** |
| Remaining game C-decompilation targets | 0 functions, 0 (`0x0`) |
| Evidence-backed handwritten game assembly | 61 functions, 40,116 (`0x9CB4`) |
| Total game-owned functions | 1,196 |
| Preserved Psy-Q CRT/SDK assembly | 630 functions, 122,064 (`0x1DCD0`) |
| Total discovered functions | 1,826 |
| Embedded/unassigned resident text | 1,832 (`0x728`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `duel_effects` | 85 / 85 (100.00%) | 81,856 (`0x13FC0`) / 81,856 (`0x13FC0`) (100.00%) |
| `free_duel` | 9 / 9 (100.00%) | 4,140 (`0x102C`) / 4,140 (`0x102C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 17,724 (`0x453C`) / 17,724 (`0x453C`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password` | 27 / 27 (100.00%) | 10,280 (`0x2828`) / 10,280 (`0x2828`) (100.00%) |

_Generated from `config/slpm_86398/functions.csv` and `config/slpm_86398/overlays/*_functions.csv`, validated against their matching-C manifests by `tools/project/progress.py`._

### European (`SLES-03947`)

Target SHA-256: `49544302dbe341489ae0eaf7888cac002a6160c57454970c434cdcb171d2a40e`

| Metric | Current |
|---|---:|
| Game C-decompilation targets matched | **1,140 / 1,140 (100.00%)** |
| Game C-decompilation target bytes matched | **357,184 (`0x57340`) / 357,184 (`0x57340`) (100.00%)** |
| Remaining game C-decompilation targets | 0 functions, 0 (`0x0`) |
| Evidence-backed handwritten game assembly | 61 functions, 40,116 (`0x9CB4`) |
| Total game-owned functions | 1,201 |
| Preserved Psy-Q CRT/SDK assembly | 623 functions, 120,584 (`0x1D708`) |
| Total discovered functions | 1,824 |
| Embedded/unassigned resident text | 1,808 (`0x710`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `duel_effects` | 85 / 85 (100.00%) | 81,804 (`0x13F8C`) / 81,804 (`0x13F8C`) (100.00%) |
| `free_duel` | 9 / 9 (100.00%) | 4,252 (`0x109C`) / 4,252 (`0x109C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password_a` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |
| `password_b` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |

_Generated from `config/sles_03947/functions.csv` and `config/sles_03947/overlays/*_functions.csv`, validated against their matching-C manifests by `tools/project/progress.py`._

### Spanish (`SLES-03951`)

Target SHA-256: `b0fefd88b6510f49af4f01e6180e40371652b7ceaa5f31dcb938c942316fc790`

| Metric | Current |
|---|---:|
| Game C-decompilation targets matched | **1,140 / 1,140 (100.00%)** |
| Game C-decompilation target bytes matched | **357,700 (`0x57544`) / 357,700 (`0x57544`) (100.00%)** |
| Remaining game C-decompilation targets | 0 functions, 0 (`0x0`) |
| Evidence-backed handwritten game assembly | 61 functions, 40,116 (`0x9CB4`) |
| Total game-owned functions | 1,201 |
| Preserved Psy-Q CRT/SDK assembly | 623 functions, 120,584 (`0x1D708`) |
| Total discovered functions | 1,824 |
| Embedded/unassigned resident text | 1,808 (`0x710`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `duel_effects` | 85 / 85 (100.00%) | 81,804 (`0x13F8C`) / 81,804 (`0x13F8C`) (100.00%) |
| `exodia_slot0` | 3 / 3 (100.00%) | 6,056 (`0x17A8`) / 6,056 (`0x17A8`) (100.00%) |
| `exodia_slot1` | 4 / 4 (100.00%) | 7,480 (`0x1D38`) / 7,480 (`0x1D38`) (100.00%) |
| `free_duel` | 9 / 9 (100.00%) | 4,252 (`0x109C`) / 4,252 (`0x109C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_1` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_2` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_3` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_4` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `model_intro` | 5 / 5 (100.00%) | 1,484 (`0x5CC`) / 1,484 (`0x5CC`) (100.00%) |
| `model_primary_116_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_116_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_141_slot0` | 1 / 1 (100.00%) | 2,448 (`0x990`) / 2,448 (`0x990`) (100.00%) |
| `model_primary_141_slot1` | 1 / 1 (100.00%) | 2,448 (`0x990`) / 2,448 (`0x990`) (100.00%) |
| `model_primary_150_slot0` | 1 / 1 (100.00%) | 276 (`0x114`) / 276 (`0x114`) (100.00%) |
| `model_primary_150_slot1` | 1 / 1 (100.00%) | 276 (`0x114`) / 276 (`0x114`) (100.00%) |
| `model_primary_167_slot0` | 1 / 1 (100.00%) | 276 (`0x114`) / 276 (`0x114`) (100.00%) |
| `model_primary_167_slot1` | 1 / 1 (100.00%) | 276 (`0x114`) / 276 (`0x114`) (100.00%) |
| `model_primary_370_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_370_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_394_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_394_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_416_slot0` | 1 / 1 (100.00%) | 2,564 (`0xA04`) / 2,564 (`0xA04`) (100.00%) |
| `model_primary_416_slot1` | 1 / 1 (100.00%) | 2,564 (`0xA04`) / 2,564 (`0xA04`) (100.00%) |
| `model_primary_707_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_707_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_715_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_715_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_8_slot0` | 1 / 1 (100.00%) | 1,572 (`0x624`) / 1,572 (`0x624`) (100.00%) |
| `model_primary_8_slot1` | 1 / 1 (100.00%) | 1,572 (`0x624`) / 1,572 (`0x624`) (100.00%) |
| `model_return_two_slot0` | 1 / 1 (100.00%) | 8 (`0x8`) / 8 (`0x8`) (100.00%) |
| `model_return_two_slot1` | 1 / 1 (100.00%) | 8 (`0x8`) / 8 (`0x8`) (100.00%) |
| `model_variant_0_stage10_slot1` | 2 / 7 (28.57%) | 2,424 (`0x978`) / 14,596 (`0x3904`) (16.61%) |
| `model_variant_0_stage7_slot0` | 1 / 7 (14.29%) | 1,164 (`0x48C`) / 14,448 (`0x3870`) (8.06%) |
| `model_variant_0_stage8_slot1` | 1 / 7 (14.29%) | 1,164 (`0x48C`) / 14,448 (`0x3870`) (8.06%) |
| `model_variant_0_stage9_slot0` | 2 / 7 (28.57%) | 2,424 (`0x978`) / 14,596 (`0x3904`) (16.61%) |
| `model_variant_102_stage10_slot1` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_102_stage9_slot0` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_108_stage10_slot1` | 7 / 9 (77.78%) | 8,712 (`0x2208`) / 17,232 (`0x4350`) (50.56%) |
| `model_variant_108_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_108_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_108_stage9_slot0` | 7 / 9 (77.78%) | 8,712 (`0x2208`) / 17,232 (`0x4350`) (50.56%) |
| `model_variant_110_stage10_slot1` | 3 / 4 (75.00%) | 5,940 (`0x1734`) / 8,024 (`0x1F58`) (74.03%) |
| `model_variant_110_stage9_slot0` | 3 / 4 (75.00%) | 5,940 (`0x1734`) / 8,024 (`0x1F58`) (74.03%) |
| `model_variant_114_stage10_slot1` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_114_stage9_slot0` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_116_stage7_slot0` | 6 / 6 (100.00%) | 11,572 (`0x2D34`) / 11,572 (`0x2D34`) (100.00%) |
| `model_variant_116_stage8_slot1` | 6 / 6 (100.00%) | 11,572 (`0x2D34`) / 11,572 (`0x2D34`) (100.00%) |
| `model_variant_121_stage7_slot0` | 2 / 7 (28.57%) | 2,424 (`0x978`) / 14,520 (`0x38B8`) (16.69%) |
| `model_variant_121_stage8_slot1` | 2 / 7 (28.57%) | 2,424 (`0x978`) / 14,520 (`0x38B8`) (16.69%) |
| `model_variant_124_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_124_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_125_stage7_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_125_stage8_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_138_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_138_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_140_stage10_slot1` | 1 / 1 (100.00%) | 2,584 (`0xA18`) / 2,584 (`0xA18`) (100.00%) |
| `model_variant_140_stage9_slot0` | 1 / 1 (100.00%) | 2,584 (`0xA18`) / 2,584 (`0xA18`) (100.00%) |
| `model_variant_147_stage7_slot0` | 1 / 8 (12.50%) | 1,304 (`0x518`) / 18,532 (`0x4864`) (7.04%) |
| `model_variant_147_stage8_slot1` | 1 / 8 (12.50%) | 1,304 (`0x518`) / 18,532 (`0x4864`) (7.04%) |
| `model_variant_149_stage7_slot0` | 2 / 5 (40.00%) | 2,120 (`0x848`) / 8,120 (`0x1FB8`) (26.11%) |
| `model_variant_149_stage8_slot1` | 2 / 5 (40.00%) | 2,120 (`0x848`) / 8,120 (`0x1FB8`) (26.11%) |
| `model_variant_152_stage10_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_152_stage9_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_159_stage10_slot1` | 3 / 4 (75.00%) | 5,940 (`0x1734`) / 8,024 (`0x1F58`) (74.03%) |
| `model_variant_159_stage9_slot0` | 3 / 4 (75.00%) | 5,940 (`0x1734`) / 8,024 (`0x1F58`) (74.03%) |
| `model_variant_161_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_161_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_162_stage7_slot0` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_162_stage8_slot1` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_164_stage7_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_164_stage8_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_165_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_165_stage7_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_165_stage8_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_165_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_166_stage10_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_166_stage9_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_168_stage10_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_168_stage7_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_168_stage8_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_168_stage9_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_170_stage10_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_170_stage9_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_174_stage10_slot1` | 4 / 7 (57.14%) | 6,720 (`0x1A40`) / 14,652 (`0x393C`) (45.86%) |
| `model_variant_174_stage9_slot0` | 4 / 7 (57.14%) | 6,720 (`0x1A40`) / 14,652 (`0x393C`) (45.86%) |
| `model_variant_175_stage7_slot0` | 1 / 6 (16.67%) | 1,164 (`0x48C`) / 11,688 (`0x2DA8`) (9.96%) |
| `model_variant_175_stage8_slot1` | 1 / 6 (16.67%) | 1,164 (`0x48C`) / 11,688 (`0x2DA8`) (9.96%) |
| `model_variant_180_stage7_slot0` | 7 / 8 (87.50%) | 8,296 (`0x2068`) / 12,468 (`0x30B4`) (66.54%) |
| `model_variant_180_stage8_slot1` | 7 / 8 (87.50%) | 8,296 (`0x2068`) / 12,468 (`0x30B4`) (66.54%) |
| `model_variant_182_stage10_slot1` | 1 / 6 (16.67%) | 1,164 (`0x48C`) / 11,688 (`0x2DA8`) (9.96%) |
| `model_variant_182_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_182_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_182_stage9_slot0` | 1 / 6 (16.67%) | 1,164 (`0x48C`) / 11,688 (`0x2DA8`) (9.96%) |
| `model_variant_184_stage10_slot1` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_184_stage9_slot0` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_185_stage7_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_185_stage8_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_186_stage7_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_186_stage8_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_187_stage7_slot0` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_187_stage8_slot1` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_193_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_193_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_1_stage7_slot0` | 9 / 9 (100.00%) | 14,384 (`0x3830`) / 14,384 (`0x3830`) (100.00%) |
| `model_variant_1_stage8_slot1` | 9 / 9 (100.00%) | 14,384 (`0x3830`) / 14,384 (`0x3830`) (100.00%) |
| `model_variant_20_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_20_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_210_stage7_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_210_stage8_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_211_stage7_slot0` | 1 / 8 (12.50%) | 1,304 (`0x518`) / 18,532 (`0x4864`) (7.04%) |
| `model_variant_211_stage8_slot1` | 1 / 8 (12.50%) | 1,304 (`0x518`) / 18,532 (`0x4864`) (7.04%) |
| `model_variant_239_stage10_slot1` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_239_stage9_slot0` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_242_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_242_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_244_stage7_slot0` | 1 / 6 (16.67%) | 1,164 (`0x48C`) / 11,688 (`0x2DA8`) (9.96%) |
| `model_variant_244_stage8_slot1` | 1 / 6 (16.67%) | 1,164 (`0x48C`) / 11,688 (`0x2DA8`) (9.96%) |
| `model_variant_259_stage7_slot0` | 9 / 10 (90.00%) | 12,988 (`0x32BC`) / 17,452 (`0x442C`) (74.42%) |
| `model_variant_259_stage8_slot1` | 9 / 10 (90.00%) | 12,988 (`0x32BC`) / 17,452 (`0x442C`) (74.42%) |
| `model_variant_262_stage10_slot1` | 5 / 7 (71.43%) | 5,184 (`0x1440`) / 11,036 (`0x2B1C`) (46.97%) |
| `model_variant_262_stage9_slot0` | 5 / 7 (71.43%) | 5,184 (`0x1440`) / 11,036 (`0x2B1C`) (46.97%) |
| `model_variant_263_stage10_slot1` | 1 / 8 (12.50%) | 1,304 (`0x518`) / 18,532 (`0x4864`) (7.04%) |
| `model_variant_263_stage9_slot0` | 1 / 8 (12.50%) | 1,304 (`0x518`) / 18,532 (`0x4864`) (7.04%) |
| `model_variant_275_stage10_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_275_stage9_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_279_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_279_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_282_stage10_slot1` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_282_stage9_slot0` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_288_stage10_slot1` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_288_stage9_slot0` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_290_stage7_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_290_stage8_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_294_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_294_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_295_stage7_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_295_stage8_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_297_stage7_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_297_stage8_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_2_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_2_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_31_stage10_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_31_stage9_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_34_stage10_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_34_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_34_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_34_stage9_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_352_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_352_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_358_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_358_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_361_stage10_slot1` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_361_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_361_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_361_stage9_slot0` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_367_stage10_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_367_stage9_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_368_stage10_slot1` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_368_stage9_slot0` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_369_stage10_slot1` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_369_stage9_slot0` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_370_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_370_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_376_stage7_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_376_stage8_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_379_stage10_slot1` | 3 / 7 (42.86%) | 4,144 (`0x1030`) / 15,048 (`0x3AC8`) (27.54%) |
| `model_variant_379_stage7_slot0` | 2 / 7 (28.57%) | 2,508 (`0x9CC`) / 15,644 (`0x3D1C`) (16.03%) |
| `model_variant_379_stage8_slot1` | 2 / 7 (28.57%) | 2,508 (`0x9CC`) / 15,644 (`0x3D1C`) (16.03%) |
| `model_variant_379_stage9_slot0` | 3 / 7 (42.86%) | 4,144 (`0x1030`) / 15,048 (`0x3AC8`) (27.54%) |
| `model_variant_385_stage10_slot1` | 1 / 7 (14.29%) | 1,216 (`0x4C0`) / 15,584 (`0x3CE0`) (7.80%) |
| `model_variant_385_stage7_slot0` | 2 / 7 (28.57%) | 2,428 (`0x97C`) / 14,288 (`0x37D0`) (16.99%) |
| `model_variant_385_stage8_slot1` | 2 / 7 (28.57%) | 2,428 (`0x97C`) / 14,288 (`0x37D0`) (16.99%) |
| `model_variant_385_stage9_slot0` | 1 / 7 (14.29%) | 1,216 (`0x4C0`) / 15,584 (`0x3CE0`) (7.80%) |
| `model_variant_388_stage10_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_388_stage9_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_391_stage7_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_391_stage8_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_395_stage10_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_395_stage9_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_399_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_399_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_400_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_400_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_401_stage10_slot1` | 3 / 5 (60.00%) | 2,964 (`0xB94`) / 7,900 (`0x1EDC`) (37.52%) |
| `model_variant_401_stage7_slot0` | 7 / 7 (100.00%) | 12,100 (`0x2F44`) / 12,100 (`0x2F44`) (100.00%) |
| `model_variant_401_stage8_slot1` | 7 / 7 (100.00%) | 12,100 (`0x2F44`) / 12,100 (`0x2F44`) (100.00%) |
| `model_variant_401_stage9_slot0` | 3 / 5 (60.00%) | 2,964 (`0xB94`) / 7,900 (`0x1EDC`) (37.52%) |
| `model_variant_408_stage10_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_408_stage9_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_410_stage10_slot1` | 6 / 8 (75.00%) | 6,588 (`0x19BC`) / 12,568 (`0x3118`) (52.42%) |
| `model_variant_410_stage7_slot0` | 3 / 4 (75.00%) | 5,940 (`0x1734`) / 8,024 (`0x1F58`) (74.03%) |
| `model_variant_410_stage8_slot1` | 3 / 4 (75.00%) | 5,940 (`0x1734`) / 8,024 (`0x1F58`) (74.03%) |
| `model_variant_410_stage9_slot0` | 6 / 8 (75.00%) | 6,588 (`0x19BC`) / 12,568 (`0x3118`) (52.42%) |
| `model_variant_419_stage7_slot0` | 2 / 5 (40.00%) | 2,120 (`0x848`) / 8,120 (`0x1FB8`) (26.11%) |
| `model_variant_419_stage8_slot1` | 2 / 5 (40.00%) | 2,120 (`0x848`) / 8,120 (`0x1FB8`) (26.11%) |
| `model_variant_424_stage7_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_424_stage8_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_427_stage10_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_427_stage9_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_431_stage10_slot1` | 2 / 7 (28.57%) | 2,424 (`0x978`) / 14,520 (`0x38B8`) (16.69%) |
| `model_variant_431_stage9_slot0` | 2 / 7 (28.57%) | 2,424 (`0x978`) / 14,520 (`0x38B8`) (16.69%) |
| `model_variant_436_stage7_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_436_stage8_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_440_stage7_slot0` | 7 / 8 (87.50%) | 8,296 (`0x2068`) / 12,468 (`0x30B4`) (66.54%) |
| `model_variant_440_stage8_slot1` | 7 / 8 (87.50%) | 8,296 (`0x2068`) / 12,468 (`0x30B4`) (66.54%) |
| `model_variant_443_stage10_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_443_stage9_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_44_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_44_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_458_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_458_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_459_stage10_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_459_stage9_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_460_stage7_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_460_stage8_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_462_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_462_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_465_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_465_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_469_stage10_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_469_stage7_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_469_stage8_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_469_stage9_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_478_stage10_slot1` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_478_stage9_slot0` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_491_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_491_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_501_stage7_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_501_stage8_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_504_stage7_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_504_stage8_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_518_stage7_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_518_stage8_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_520_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_520_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_525_stage10_slot1` | 1 / 8 (12.50%) | 1,304 (`0x518`) / 18,532 (`0x4864`) (7.04%) |
| `model_variant_525_stage9_slot0` | 1 / 8 (12.50%) | 1,304 (`0x518`) / 18,532 (`0x4864`) (7.04%) |
| `model_variant_531_stage10_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_531_stage9_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_54_stage10_slot1` | 3 / 3 (100.00%) | 5,200 (`0x1450`) / 5,200 (`0x1450`) (100.00%) |
| `model_variant_54_stage9_slot0` | 3 / 3 (100.00%) | 5,200 (`0x1450`) / 5,200 (`0x1450`) (100.00%) |
| `model_variant_550_stage7_slot0` | 9 / 9 (100.00%) | 14,384 (`0x3830`) / 14,384 (`0x3830`) (100.00%) |
| `model_variant_550_stage8_slot1` | 9 / 9 (100.00%) | 14,384 (`0x3830`) / 14,384 (`0x3830`) (100.00%) |
| `model_variant_551_stage7_slot0` | 5 / 5 (100.00%) | 8,284 (`0x205C`) / 8,284 (`0x205C`) (100.00%) |
| `model_variant_551_stage8_slot1` | 5 / 5 (100.00%) | 8,284 (`0x205C`) / 8,284 (`0x205C`) (100.00%) |
| `model_variant_552_stage7_slot0` | 7 / 7 (100.00%) | 11,604 (`0x2D54`) / 11,604 (`0x2D54`) (100.00%) |
| `model_variant_552_stage8_slot1` | 7 / 7 (100.00%) | 11,604 (`0x2D54`) / 11,604 (`0x2D54`) (100.00%) |
| `model_variant_558_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_558_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_562_stage10_slot1` | 2 / 5 (40.00%) | 2,120 (`0x848`) / 8,120 (`0x1FB8`) (26.11%) |
| `model_variant_562_stage9_slot0` | 2 / 5 (40.00%) | 2,120 (`0x848`) / 8,120 (`0x1FB8`) (26.11%) |
| `model_variant_573_stage10_slot1` | 7 / 9 (77.78%) | 8,712 (`0x2208`) / 17,232 (`0x4350`) (50.56%) |
| `model_variant_573_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_573_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_573_stage9_slot0` | 7 / 9 (77.78%) | 8,712 (`0x2208`) / 17,232 (`0x4350`) (50.56%) |
| `model_variant_576_stage7_slot0` | 6 / 6 (100.00%) | 11,572 (`0x2D34`) / 11,572 (`0x2D34`) (100.00%) |
| `model_variant_576_stage8_slot1` | 6 / 6 (100.00%) | 11,572 (`0x2D34`) / 11,572 (`0x2D34`) (100.00%) |
| `model_variant_57_stage10_slot1` | 2 / 5 (40.00%) | 2,120 (`0x848`) / 8,120 (`0x1FB8`) (26.11%) |
| `model_variant_57_stage9_slot0` | 2 / 5 (40.00%) | 2,120 (`0x848`) / 8,120 (`0x1FB8`) (26.11%) |
| `model_variant_580_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_580_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_584_stage10_slot1` | 1 / 1 (100.00%) | 2,584 (`0xA18`) / 2,584 (`0xA18`) (100.00%) |
| `model_variant_584_stage9_slot0` | 1 / 1 (100.00%) | 2,584 (`0xA18`) / 2,584 (`0xA18`) (100.00%) |
| `model_variant_590_stage10_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_590_stage9_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_594_stage7_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_594_stage8_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_595_stage7_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_595_stage8_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_596_stage7_slot0` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_596_stage8_slot1` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_609_stage7_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_609_stage8_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_610_stage7_slot0` | 1 / 8 (12.50%) | 1,304 (`0x518`) / 18,532 (`0x4864`) (7.04%) |
| `model_variant_610_stage8_slot1` | 1 / 8 (12.50%) | 1,304 (`0x518`) / 18,532 (`0x4864`) (7.04%) |
| `model_variant_621_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_621_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_630_stage7_slot0` | 9 / 10 (90.00%) | 12,988 (`0x32BC`) / 17,452 (`0x442C`) (74.42%) |
| `model_variant_630_stage8_slot1` | 9 / 10 (90.00%) | 12,988 (`0x32BC`) / 17,452 (`0x442C`) (74.42%) |
| `model_variant_631_stage10_slot1` | 5 / 7 (71.43%) | 5,184 (`0x1440`) / 11,036 (`0x2B1C`) (46.97%) |
| `model_variant_631_stage9_slot0` | 5 / 7 (71.43%) | 5,184 (`0x1440`) / 11,036 (`0x2B1C`) (46.97%) |
| `model_variant_632_stage10_slot1` | 1 / 8 (12.50%) | 1,304 (`0x518`) / 18,532 (`0x4864`) (7.04%) |
| `model_variant_632_stage9_slot0` | 1 / 8 (12.50%) | 1,304 (`0x518`) / 18,532 (`0x4864`) (7.04%) |
| `model_variant_640_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_640_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_642_stage10_slot1` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_642_stage9_slot0` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_645_stage10_slot1` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_645_stage9_slot0` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_68_stage7_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_68_stage8_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_6_stage7_slot0` | 5 / 5 (100.00%) | 8,284 (`0x205C`) / 8,284 (`0x205C`) (100.00%) |
| `model_variant_6_stage8_slot1` | 5 / 5 (100.00%) | 8,284 (`0x205C`) / 8,284 (`0x205C`) (100.00%) |
| `model_variant_704_stage7_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_704_stage8_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_705_stage10_slot1` | 1 / 4 (25.00%) | 1,348 (`0x544`) / 10,448 (`0x28D0`) (12.90%) |
| `model_variant_705_stage9_slot0` | 1 / 4 (25.00%) | 1,348 (`0x544`) / 10,448 (`0x28D0`) (12.90%) |
| `model_variant_707_stage7_slot0` | 5 / 8 (62.50%) | 7,296 (`0x1C80`) / 16,108 (`0x3EEC`) (45.29%) |
| `model_variant_707_stage8_slot1` | 5 / 8 (62.50%) | 7,296 (`0x1C80`) / 16,108 (`0x3EEC`) (45.29%) |
| `model_variant_70_stage7_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_70_stage8_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_712_stage10_slot1` | 6 / 7 (85.71%) | 8,216 (`0x2018`) / 13,168 (`0x3370`) (62.39%) |
| `model_variant_712_stage9_slot0` | 6 / 7 (85.71%) | 8,216 (`0x2018`) / 13,168 (`0x3370`) (62.39%) |
| `model_variant_719_stage7_slot0` | 1 / 5 (20.00%) | 936 (`0x3A8`) / 11,348 (`0x2C54`) (8.25%) |
| `model_variant_719_stage8_slot1` | 1 / 5 (20.00%) | 936 (`0x3A8`) / 11,348 (`0x2C54`) (8.25%) |
| `model_variant_71_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_71_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_7_stage7_slot0` | 7 / 7 (100.00%) | 11,604 (`0x2D54`) / 11,604 (`0x2D54`) (100.00%) |
| `model_variant_7_stage8_slot1` | 7 / 7 (100.00%) | 11,604 (`0x2D54`) / 11,604 (`0x2D54`) (100.00%) |
| `model_variant_84_stage7_slot0` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_84_stage8_slot1` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_87_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_87_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_88_stage10_slot1` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_88_stage9_slot0` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_96_stage7_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_96_stage8_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_98_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_98_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `options` | 14 / 14 (100.00%) | 4,156 (`0x103C`) / 4,156 (`0x103C`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password_a` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |
| `password_b` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |

_Generated from `config/sles_03951/functions.csv` and `config/sles_03951/overlays/*_functions.csv`, validated against their matching-C manifests by `tools/project/progress.py`._

### French (`SLES-03948`)

Resident progress is not included here; the following counts cover only the inventoried runtime overlays.

Source: `config/sles_03948/overlays/*_functions.csv`, validated against their matching-C manifests.

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `duel_effects` | 85 / 85 (100.00%) | 81,804 (`0x13F8C`) / 81,804 (`0x13F8C`) (100.00%) |
| `exodia_slot0` | 3 / 3 (100.00%) | 6,056 (`0x17A8`) / 6,056 (`0x17A8`) (100.00%) |
| `exodia_slot1` | 4 / 4 (100.00%) | 7,480 (`0x1D38`) / 7,480 (`0x1D38`) (100.00%) |
| `free_duel` | 9 / 9 (100.00%) | 4,252 (`0x109C`) / 4,252 (`0x109C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_1` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_2` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_3` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_4` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `model_intro` | 5 / 5 (100.00%) | 1,484 (`0x5CC`) / 1,484 (`0x5CC`) (100.00%) |
| `model_primary_116_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_116_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_141_slot0` | 1 / 1 (100.00%) | 2,448 (`0x990`) / 2,448 (`0x990`) (100.00%) |
| `model_primary_141_slot1` | 1 / 1 (100.00%) | 2,448 (`0x990`) / 2,448 (`0x990`) (100.00%) |
| `model_primary_150_slot0` | 1 / 1 (100.00%) | 276 (`0x114`) / 276 (`0x114`) (100.00%) |
| `model_primary_150_slot1` | 1 / 1 (100.00%) | 276 (`0x114`) / 276 (`0x114`) (100.00%) |
| `model_primary_167_slot0` | 1 / 1 (100.00%) | 276 (`0x114`) / 276 (`0x114`) (100.00%) |
| `model_primary_167_slot1` | 1 / 1 (100.00%) | 276 (`0x114`) / 276 (`0x114`) (100.00%) |
| `model_primary_370_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_370_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_394_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_394_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_416_slot0` | 1 / 1 (100.00%) | 2,564 (`0xA04`) / 2,564 (`0xA04`) (100.00%) |
| `model_primary_416_slot1` | 1 / 1 (100.00%) | 2,564 (`0xA04`) / 2,564 (`0xA04`) (100.00%) |
| `model_primary_707_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_707_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_715_slot0` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_715_slot1` | 1 / 1 (100.00%) | 216 (`0xD8`) / 216 (`0xD8`) (100.00%) |
| `model_primary_8_slot0` | 1 / 1 (100.00%) | 1,572 (`0x624`) / 1,572 (`0x624`) (100.00%) |
| `model_primary_8_slot1` | 1 / 1 (100.00%) | 1,572 (`0x624`) / 1,572 (`0x624`) (100.00%) |
| `model_return_two_slot0` | 1 / 1 (100.00%) | 8 (`0x8`) / 8 (`0x8`) (100.00%) |
| `model_return_two_slot1` | 1 / 1 (100.00%) | 8 (`0x8`) / 8 (`0x8`) (100.00%) |
| `model_variant_102_stage10_slot1` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_102_stage9_slot0` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_103_stage7_slot0` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_103_stage8_slot1` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_108_stage10_slot1` | 7 / 9 (77.78%) | 8,712 (`0x2208`) / 17,232 (`0x4350`) (50.56%) |
| `model_variant_108_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_108_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_108_stage9_slot0` | 7 / 9 (77.78%) | 8,712 (`0x2208`) / 17,232 (`0x4350`) (50.56%) |
| `model_variant_10_stage7_slot0` | 5 / 6 (83.33%) | 7,184 (`0x1C10`) / 10,148 (`0x27A4`) (70.79%) |
| `model_variant_10_stage8_slot1` | 5 / 6 (83.33%) | 7,184 (`0x1C10`) / 10,148 (`0x27A4`) (70.79%) |
| `model_variant_110_stage10_slot1` | 3 / 4 (75.00%) | 5,940 (`0x1734`) / 8,024 (`0x1F58`) (74.03%) |
| `model_variant_110_stage9_slot0` | 3 / 4 (75.00%) | 5,940 (`0x1734`) / 8,024 (`0x1F58`) (74.03%) |
| `model_variant_114_stage10_slot1` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_114_stage9_slot0` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_116_stage7_slot0` | 6 / 6 (100.00%) | 11,572 (`0x2D34`) / 11,572 (`0x2D34`) (100.00%) |
| `model_variant_116_stage8_slot1` | 6 / 6 (100.00%) | 11,572 (`0x2D34`) / 11,572 (`0x2D34`) (100.00%) |
| `model_variant_124_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_124_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_125_stage7_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_125_stage8_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_127_stage10_slot1` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_127_stage9_slot0` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_128_stage10_slot1` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_128_stage9_slot0` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_130_stage7_slot0` | 1 / 1 (100.00%) | 2,784 (`0xAE0`) / 2,784 (`0xAE0`) (100.00%) |
| `model_variant_130_stage8_slot1` | 1 / 1 (100.00%) | 2,784 (`0xAE0`) / 2,784 (`0xAE0`) (100.00%) |
| `model_variant_138_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_138_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_140_stage10_slot1` | 1 / 1 (100.00%) | 2,584 (`0xA18`) / 2,584 (`0xA18`) (100.00%) |
| `model_variant_140_stage9_slot0` | 1 / 1 (100.00%) | 2,584 (`0xA18`) / 2,584 (`0xA18`) (100.00%) |
| `model_variant_141_stage10_slot1` | 1 / 1 (100.00%) | 3,384 (`0xD38`) / 3,384 (`0xD38`) (100.00%) |
| `model_variant_141_stage9_slot0` | 1 / 1 (100.00%) | 3,384 (`0xD38`) / 3,384 (`0xD38`) (100.00%) |
| `model_variant_152_stage10_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_152_stage9_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_159_stage10_slot1` | 3 / 4 (75.00%) | 5,940 (`0x1734`) / 8,024 (`0x1F58`) (74.03%) |
| `model_variant_159_stage9_slot0` | 3 / 4 (75.00%) | 5,940 (`0x1734`) / 8,024 (`0x1F58`) (74.03%) |
| `model_variant_161_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_161_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_162_stage7_slot0` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_162_stage8_slot1` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_164_stage7_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_164_stage8_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_165_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_165_stage7_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_165_stage8_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_165_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_166_stage10_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_166_stage9_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_168_stage10_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_168_stage7_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_168_stage8_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_168_stage9_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_170_stage10_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_170_stage9_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_174_stage10_slot1` | 5 / 7 (71.43%) | 8,820 (`0x2274`) / 14,652 (`0x393C`) (60.20%) |
| `model_variant_174_stage9_slot0` | 5 / 7 (71.43%) | 8,820 (`0x2274`) / 14,652 (`0x393C`) (60.20%) |
| `model_variant_180_stage7_slot0` | 7 / 8 (87.50%) | 8,296 (`0x2068`) / 12,468 (`0x30B4`) (66.54%) |
| `model_variant_180_stage8_slot1` | 7 / 8 (87.50%) | 8,296 (`0x2068`) / 12,468 (`0x30B4`) (66.54%) |
| `model_variant_182_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_182_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_184_stage10_slot1` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_184_stage9_slot0` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_185_stage7_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_185_stage8_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_186_stage7_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_186_stage8_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_187_stage10_slot1` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_187_stage7_slot0` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_187_stage8_slot1` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_187_stage9_slot0` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_189_stage7_slot0` | 2 / 4 (50.00%) | 4,912 (`0x1330`) / 10,052 (`0x2744`) (48.87%) |
| `model_variant_189_stage8_slot1` | 2 / 4 (50.00%) | 4,912 (`0x1330`) / 10,052 (`0x2744`) (48.87%) |
| `model_variant_190_stage10_slot1` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_190_stage9_slot0` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_192_stage10_slot1` | 1 / 1 (100.00%) | 3,144 (`0xC48`) / 3,144 (`0xC48`) (100.00%) |
| `model_variant_192_stage9_slot0` | 1 / 1 (100.00%) | 3,144 (`0xC48`) / 3,144 (`0xC48`) (100.00%) |
| `model_variant_193_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_193_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_196_stage10_slot1` | 1 / 1 (100.00%) | 3,144 (`0xC48`) / 3,144 (`0xC48`) (100.00%) |
| `model_variant_196_stage9_slot0` | 1 / 1 (100.00%) | 3,144 (`0xC48`) / 3,144 (`0xC48`) (100.00%) |
| `model_variant_1_stage10_slot1` | 3 / 4 (75.00%) | 4,868 (`0x1304`) / 9,184 (`0x23E0`) (53.01%) |
| `model_variant_1_stage7_slot0` | 9 / 9 (100.00%) | 14,384 (`0x3830`) / 14,384 (`0x3830`) (100.00%) |
| `model_variant_1_stage8_slot1` | 9 / 9 (100.00%) | 14,384 (`0x3830`) / 14,384 (`0x3830`) (100.00%) |
| `model_variant_1_stage9_slot0` | 3 / 4 (75.00%) | 4,868 (`0x1304`) / 9,184 (`0x23E0`) (53.01%) |
| `model_variant_202_stage7_slot0` | 2 / 4 (50.00%) | 3,680 (`0xE60`) / 8,968 (`0x2308`) (41.03%) |
| `model_variant_202_stage8_slot1` | 2 / 4 (50.00%) | 3,680 (`0xE60`) / 8,968 (`0x2308`) (41.03%) |
| `model_variant_20_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_20_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_210_stage7_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_210_stage8_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_217_stage10_slot1` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_217_stage9_slot0` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_221_stage10_slot1` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_221_stage9_slot0` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_227_stage10_slot1` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_227_stage9_slot0` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_22_stage10_slot1` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_22_stage9_slot0` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_239_stage10_slot1` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_239_stage7_slot0` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_239_stage8_slot1` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_239_stage9_slot0` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_23_stage7_slot0` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_23_stage8_slot1` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_242_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_242_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_243_stage10_slot1` | 1 / 1 (100.00%) | 5,520 (`0x1590`) / 5,520 (`0x1590`) (100.00%) |
| `model_variant_243_stage9_slot0` | 1 / 1 (100.00%) | 5,520 (`0x1590`) / 5,520 (`0x1590`) (100.00%) |
| `model_variant_258_stage7_slot0` | 2 / 4 (50.00%) | 4,912 (`0x1330`) / 10,052 (`0x2744`) (48.87%) |
| `model_variant_258_stage8_slot1` | 2 / 4 (50.00%) | 4,912 (`0x1330`) / 10,052 (`0x2744`) (48.87%) |
| `model_variant_259_stage7_slot0` | 9 / 10 (90.00%) | 12,988 (`0x32BC`) / 17,452 (`0x442C`) (74.42%) |
| `model_variant_259_stage8_slot1` | 9 / 10 (90.00%) | 12,988 (`0x32BC`) / 17,452 (`0x442C`) (74.42%) |
| `model_variant_262_stage10_slot1` | 5 / 7 (71.43%) | 5,184 (`0x1440`) / 11,036 (`0x2B1C`) (46.97%) |
| `model_variant_262_stage9_slot0` | 5 / 7 (71.43%) | 5,184 (`0x1440`) / 11,036 (`0x2B1C`) (46.97%) |
| `model_variant_275_stage10_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_275_stage9_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_279_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_279_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_27_stage7_slot0` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_27_stage8_slot1` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_281_stage7_slot0` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_281_stage8_slot1` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_282_stage10_slot1` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_282_stage9_slot0` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_288_stage10_slot1` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_288_stage9_slot0` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_290_stage7_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_290_stage8_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_294_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_294_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_295_stage7_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_295_stage8_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_296_stage10_slot1` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_296_stage9_slot0` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_297_stage7_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_297_stage8_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_2_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_2_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_31_stage10_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_31_stage9_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_34_stage10_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_34_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_34_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_34_stage9_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_352_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_352_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_358_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_358_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_360_stage10_slot1` | 3 / 4 (75.00%) | 4,868 (`0x1304`) / 9,184 (`0x23E0`) (53.01%) |
| `model_variant_360_stage9_slot0` | 3 / 4 (75.00%) | 4,868 (`0x1304`) / 9,184 (`0x23E0`) (53.01%) |
| `model_variant_361_stage10_slot1` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_361_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_361_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_361_stage9_slot0` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_367_stage10_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_367_stage9_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_368_stage10_slot1` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_368_stage9_slot0` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_369_stage10_slot1` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_369_stage9_slot0` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_370_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_370_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_376_stage7_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_376_stage8_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_37_stage7_slot0` | 1 / 1 (100.00%) | 3,496 (`0xDA8`) / 3,496 (`0xDA8`) (100.00%) |
| `model_variant_37_stage8_slot1` | 1 / 1 (100.00%) | 3,496 (`0xDA8`) / 3,496 (`0xDA8`) (100.00%) |
| `model_variant_386_stage7_slot0` | 1 / 1 (100.00%) | 3,092 (`0xC14`) / 3,092 (`0xC14`) (100.00%) |
| `model_variant_386_stage8_slot1` | 1 / 1 (100.00%) | 3,092 (`0xC14`) / 3,092 (`0xC14`) (100.00%) |
| `model_variant_388_stage10_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_388_stage9_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_391_stage7_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_391_stage8_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_395_stage10_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_395_stage9_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_399_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_399_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_39_stage10_slot1` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_39_stage9_slot0` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_400_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_400_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_401_stage10_slot1` | 3 / 5 (60.00%) | 2,964 (`0xB94`) / 7,900 (`0x1EDC`) (37.52%) |
| `model_variant_401_stage7_slot0` | 7 / 7 (100.00%) | 12,100 (`0x2F44`) / 12,100 (`0x2F44`) (100.00%) |
| `model_variant_401_stage8_slot1` | 7 / 7 (100.00%) | 12,100 (`0x2F44`) / 12,100 (`0x2F44`) (100.00%) |
| `model_variant_401_stage9_slot0` | 3 / 5 (60.00%) | 2,964 (`0xB94`) / 7,900 (`0x1EDC`) (37.52%) |
| `model_variant_408_stage10_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_408_stage9_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_410_stage10_slot1` | 6 / 8 (75.00%) | 6,588 (`0x19BC`) / 12,568 (`0x3118`) (52.42%) |
| `model_variant_410_stage7_slot0` | 3 / 4 (75.00%) | 5,940 (`0x1734`) / 8,024 (`0x1F58`) (74.03%) |
| `model_variant_410_stage8_slot1` | 3 / 4 (75.00%) | 5,940 (`0x1734`) / 8,024 (`0x1F58`) (74.03%) |
| `model_variant_410_stage9_slot0` | 6 / 8 (75.00%) | 6,588 (`0x19BC`) / 12,568 (`0x3118`) (52.42%) |
| `model_variant_422_stage7_slot0` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_422_stage8_slot1` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_424_stage7_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_424_stage8_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_427_stage10_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_427_stage7_slot0` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_427_stage8_slot1` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_427_stage9_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_436_stage7_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_436_stage8_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_440_stage7_slot0` | 7 / 8 (87.50%) | 8,296 (`0x2068`) / 12,468 (`0x30B4`) (66.54%) |
| `model_variant_440_stage8_slot1` | 7 / 8 (87.50%) | 8,296 (`0x2068`) / 12,468 (`0x30B4`) (66.54%) |
| `model_variant_443_stage10_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_443_stage9_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_44_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_44_stage7_slot0` | 5 / 5 (100.00%) | 10,472 (`0x28E8`) / 10,472 (`0x28E8`) (100.00%) |
| `model_variant_44_stage8_slot1` | 5 / 5 (100.00%) | 10,472 (`0x28E8`) / 10,472 (`0x28E8`) (100.00%) |
| `model_variant_44_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_454_stage10_slot1` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_454_stage9_slot0` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_457_stage10_slot1` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_457_stage9_slot0` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_458_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_458_stage7_slot0` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_458_stage8_slot1` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_458_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_459_stage10_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_459_stage7_slot0` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_459_stage8_slot1` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_459_stage9_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_460_stage7_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_460_stage8_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_462_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_462_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_465_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_465_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_469_stage10_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_469_stage7_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_469_stage8_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_469_stage9_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_472_stage7_slot0` | 5 / 5 (100.00%) | 9,140 (`0x23B4`) / 9,140 (`0x23B4`) (100.00%) |
| `model_variant_472_stage8_slot1` | 5 / 5 (100.00%) | 9,140 (`0x23B4`) / 9,140 (`0x23B4`) (100.00%) |
| `model_variant_477_stage10_slot1` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_477_stage9_slot0` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_478_stage10_slot1` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_478_stage9_slot0` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_480_stage7_slot0` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_480_stage8_slot1` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_484_stage10_slot1` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_484_stage9_slot0` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_491_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_491_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_499_stage7_slot0` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_499_stage8_slot1` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_501_stage7_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_501_stage8_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_502_stage7_slot0` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_502_stage8_slot1` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_504_stage10_slot1` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_504_stage7_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_504_stage8_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_504_stage9_slot0` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_508_stage10_slot1` | 1 / 1 (100.00%) | 3,440 (`0xD70`) / 3,440 (`0xD70`) (100.00%) |
| `model_variant_508_stage9_slot0` | 1 / 1 (100.00%) | 3,440 (`0xD70`) / 3,440 (`0xD70`) (100.00%) |
| `model_variant_509_stage7_slot0` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_509_stage8_slot1` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_514_stage10_slot1` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_514_stage7_slot0` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_514_stage8_slot1` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_514_stage9_slot0` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_518_stage7_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_518_stage8_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_520_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_520_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_531_stage10_slot1` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_531_stage9_slot0` | 1 / 1 (100.00%) | 2,384 (`0x950`) / 2,384 (`0x950`) (100.00%) |
| `model_variant_53_stage10_slot1` | 1 / 1 (100.00%) | 3,092 (`0xC14`) / 3,092 (`0xC14`) (100.00%) |
| `model_variant_53_stage9_slot0` | 1 / 1 (100.00%) | 3,092 (`0xC14`) / 3,092 (`0xC14`) (100.00%) |
| `model_variant_547_stage10_slot1` | 1 / 1 (100.00%) | 3,092 (`0xC14`) / 3,092 (`0xC14`) (100.00%) |
| `model_variant_547_stage9_slot0` | 1 / 1 (100.00%) | 3,092 (`0xC14`) / 3,092 (`0xC14`) (100.00%) |
| `model_variant_54_stage10_slot1` | 3 / 3 (100.00%) | 5,200 (`0x1450`) / 5,200 (`0x1450`) (100.00%) |
| `model_variant_54_stage7_slot0` | 3 / 4 (75.00%) | 4,696 (`0x1258`) / 7,424 (`0x1D00`) (63.25%) |
| `model_variant_54_stage8_slot1` | 3 / 4 (75.00%) | 4,696 (`0x1258`) / 7,424 (`0x1D00`) (63.25%) |
| `model_variant_54_stage9_slot0` | 3 / 3 (100.00%) | 5,200 (`0x1450`) / 5,200 (`0x1450`) (100.00%) |
| `model_variant_550_stage10_slot1` | 3 / 4 (75.00%) | 4,868 (`0x1304`) / 9,184 (`0x23E0`) (53.01%) |
| `model_variant_550_stage7_slot0` | 9 / 9 (100.00%) | 14,384 (`0x3830`) / 14,384 (`0x3830`) (100.00%) |
| `model_variant_550_stage8_slot1` | 9 / 9 (100.00%) | 14,384 (`0x3830`) / 14,384 (`0x3830`) (100.00%) |
| `model_variant_550_stage9_slot0` | 3 / 4 (75.00%) | 4,868 (`0x1304`) / 9,184 (`0x23E0`) (53.01%) |
| `model_variant_551_stage7_slot0` | 5 / 5 (100.00%) | 8,284 (`0x205C`) / 8,284 (`0x205C`) (100.00%) |
| `model_variant_551_stage8_slot1` | 5 / 5 (100.00%) | 8,284 (`0x205C`) / 8,284 (`0x205C`) (100.00%) |
| `model_variant_552_stage7_slot0` | 7 / 7 (100.00%) | 11,604 (`0x2D54`) / 11,604 (`0x2D54`) (100.00%) |
| `model_variant_552_stage8_slot1` | 7 / 7 (100.00%) | 11,604 (`0x2D54`) / 11,604 (`0x2D54`) (100.00%) |
| `model_variant_555_stage7_slot0` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_555_stage8_slot1` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_556_stage7_slot0` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_556_stage8_slot1` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_557_stage10_slot1` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_557_stage9_slot0` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_558_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_558_stage7_slot0` | 5 / 5 (100.00%) | 10,472 (`0x28E8`) / 10,472 (`0x28E8`) (100.00%) |
| `model_variant_558_stage8_slot1` | 5 / 5 (100.00%) | 10,472 (`0x28E8`) / 10,472 (`0x28E8`) (100.00%) |
| `model_variant_558_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_573_stage10_slot1` | 7 / 9 (77.78%) | 8,712 (`0x2208`) / 17,232 (`0x4350`) (50.56%) |
| `model_variant_573_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_573_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_573_stage9_slot0` | 7 / 9 (77.78%) | 8,712 (`0x2208`) / 17,232 (`0x4350`) (50.56%) |
| `model_variant_576_stage7_slot0` | 6 / 6 (100.00%) | 11,572 (`0x2D34`) / 11,572 (`0x2D34`) (100.00%) |
| `model_variant_576_stage8_slot1` | 6 / 6 (100.00%) | 11,572 (`0x2D34`) / 11,572 (`0x2D34`) (100.00%) |
| `model_variant_580_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_580_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_581_stage10_slot1` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_581_stage9_slot0` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_584_stage10_slot1` | 1 / 1 (100.00%) | 2,584 (`0xA18`) / 2,584 (`0xA18`) (100.00%) |
| `model_variant_584_stage9_slot0` | 1 / 1 (100.00%) | 2,584 (`0xA18`) / 2,584 (`0xA18`) (100.00%) |
| `model_variant_590_stage10_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_590_stage9_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_594_stage7_slot0` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_594_stage8_slot1` | 5 / 6 (83.33%) | 10,168 (`0x27B8`) / 11,404 (`0x2C8C`) (89.16%) |
| `model_variant_595_stage7_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_595_stage8_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_596_stage10_slot1` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_596_stage7_slot0` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_596_stage8_slot1` | 7 / 8 (87.50%) | 9,568 (`0x2560`) / 13,712 (`0x3590`) (69.78%) |
| `model_variant_596_stage9_slot0` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_598_stage10_slot1` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_598_stage9_slot0` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_609_stage7_slot0` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_609_stage8_slot1` | 5 / 5 (100.00%) | 9,992 (`0x2708`) / 9,992 (`0x2708`) (100.00%) |
| `model_variant_612_stage10_slot1` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_612_stage9_slot0` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_621_stage10_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_621_stage9_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_622_stage10_slot1` | 1 / 1 (100.00%) | 5,520 (`0x1590`) / 5,520 (`0x1590`) (100.00%) |
| `model_variant_622_stage9_slot0` | 1 / 1 (100.00%) | 5,520 (`0x1590`) / 5,520 (`0x1590`) (100.00%) |
| `model_variant_630_stage7_slot0` | 9 / 10 (90.00%) | 12,988 (`0x32BC`) / 17,452 (`0x442C`) (74.42%) |
| `model_variant_630_stage8_slot1` | 9 / 10 (90.00%) | 12,988 (`0x32BC`) / 17,452 (`0x442C`) (74.42%) |
| `model_variant_631_stage10_slot1` | 5 / 7 (71.43%) | 5,184 (`0x1440`) / 11,036 (`0x2B1C`) (46.97%) |
| `model_variant_631_stage9_slot0` | 5 / 7 (71.43%) | 5,184 (`0x1440`) / 11,036 (`0x2B1C`) (46.97%) |
| `model_variant_640_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_640_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_641_stage7_slot0` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_641_stage8_slot1` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_642_stage10_slot1` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_642_stage9_slot0` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_645_stage10_slot1` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_645_stage9_slot0` | 4 / 6 (66.67%) | 5,908 (`0x1714`) / 12,212 (`0x2FB4`) (48.38%) |
| `model_variant_647_stage10_slot1` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_647_stage9_slot0` | 4 / 4 (100.00%) | 9,132 (`0x23AC`) / 9,132 (`0x23AC`) (100.00%) |
| `model_variant_68_stage7_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_68_stage8_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_6_stage7_slot0` | 5 / 5 (100.00%) | 8,284 (`0x205C`) / 8,284 (`0x205C`) (100.00%) |
| `model_variant_6_stage8_slot1` | 5 / 5 (100.00%) | 8,284 (`0x205C`) / 8,284 (`0x205C`) (100.00%) |
| `model_variant_701_stage10_slot1` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_701_stage9_slot0` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_704_stage7_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_704_stage8_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_707_stage7_slot0` | 6 / 8 (75.00%) | 10,116 (`0x2784`) / 16,108 (`0x3EEC`) (62.80%) |
| `model_variant_707_stage8_slot1` | 6 / 8 (75.00%) | 10,116 (`0x2784`) / 16,108 (`0x3EEC`) (62.80%) |
| `model_variant_708_stage10_slot1` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_708_stage9_slot0` | 1 / 1 (100.00%) | 4,260 (`0x10A4`) / 4,260 (`0x10A4`) (100.00%) |
| `model_variant_70_stage7_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_70_stage8_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_712_stage10_slot1` | 6 / 7 (85.71%) | 8,216 (`0x2018`) / 13,168 (`0x3370`) (62.39%) |
| `model_variant_712_stage9_slot0` | 6 / 7 (85.71%) | 8,216 (`0x2018`) / 13,168 (`0x3370`) (62.39%) |
| `model_variant_71_stage7_slot0` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_71_stage8_slot1` | 8 / 9 (88.89%) | 10,516 (`0x2914`) / 14,740 (`0x3994`) (71.34%) |
| `model_variant_73_stage10_slot1` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_73_stage9_slot0` | 1 / 1 (100.00%) | 4,440 (`0x1158`) / 4,440 (`0x1158`) (100.00%) |
| `model_variant_7_stage7_slot0` | 7 / 7 (100.00%) | 11,604 (`0x2D54`) / 11,604 (`0x2D54`) (100.00%) |
| `model_variant_7_stage8_slot1` | 7 / 7 (100.00%) | 11,604 (`0x2D54`) / 11,604 (`0x2D54`) (100.00%) |
| `model_variant_84_stage10_slot1` | 1 / 1 (100.00%) | 3,092 (`0xC14`) / 3,092 (`0xC14`) (100.00%) |
| `model_variant_84_stage7_slot0` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_84_stage8_slot1` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_84_stage9_slot0` | 1 / 1 (100.00%) | 3,092 (`0xC14`) / 3,092 (`0xC14`) (100.00%) |
| `model_variant_85_stage7_slot0` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_85_stage8_slot1` | 1 / 1 (100.00%) | 5,860 (`0x16E4`) / 5,860 (`0x16E4`) (100.00%) |
| `model_variant_87_stage7_slot0` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_87_stage8_slot1` | 6 / 7 (85.71%) | 7,076 (`0x1BA4`) / 11,720 (`0x2DC8`) (60.38%) |
| `model_variant_88_stage10_slot1` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_88_stage9_slot0` | 7 / 9 (77.78%) | 8,832 (`0x2280`) / 16,488 (`0x4068`) (53.57%) |
| `model_variant_96_stage7_slot0` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_96_stage8_slot1` | 1 / 2 (50.00%) | 1,500 (`0x5DC`) / 6,320 (`0x18B0`) (23.73%) |
| `model_variant_98_stage10_slot1` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `model_variant_98_stage9_slot0` | 5 / 5 (100.00%) | 10,440 (`0x28C8`) / 10,440 (`0x28C8`) (100.00%) |
| `options` | 16 / 16 (100.00%) | 4,664 (`0x1238`) / 4,664 (`0x1238`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password_a` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |
| `password_b` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |


### Italian (`SLES-03950`)

Target SHA-256: `a01cc55d48df37c6bf9c6e2dfcbb4f429b4f8f2b111e35b44930d56bb12f9b1d`

| Metric | Current |
|---|---:|
| Game C-decompilation targets matched | **1,140 / 1,140 (100.00%)** |
| Game C-decompilation target bytes matched | **357,700 (`0x57544`) / 357,700 (`0x57544`) (100.00%)** |
| Remaining game C-decompilation targets | 0 functions, 0 (`0x0`) |
| Evidence-backed handwritten game assembly | 61 functions, 40,116 (`0x9CB4`) |
| Total game-owned functions | 1,201 |
| Preserved Psy-Q CRT/SDK assembly | 623 functions, 120,584 (`0x1D708`) |
| Total discovered functions | 1,824 |
| Embedded/unassigned resident text | 1,808 (`0x710`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `free_duel` | 9 / 9 (100.00%) | 4,252 (`0x109C`) / 4,252 (`0x109C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_1` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_2` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_3` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_4` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password_a` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |
| `password_b` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |

_Generated from `config/sles_03950/functions.csv` and `config/sles_03950/overlays/*_functions.csv`, validated against their matching-C manifests by `tools/project/progress.py`._

### German (`SLES-03949`)

Target SHA-256: `d9ba940664c6f2c908b3f10d9a8232b0ea830155150a3ff234a2ac12ea3f07cc`

| Metric | Current |
|---|---:|
| Game C-decompilation targets matched | **1,140 / 1,140 (100.00%)** |
| Game C-decompilation target bytes matched | **357,700 (`0x57544`) / 357,700 (`0x57544`) (100.00%)** |
| Remaining game C-decompilation targets | 0 functions, 0 (`0x0`) |
| Evidence-backed handwritten game assembly | 61 functions, 40,116 (`0x9CB4`) |
| Total game-owned functions | 1,201 |
| Preserved Psy-Q CRT/SDK assembly | 623 functions, 120,584 (`0x1D708`) |
| Total discovered functions | 1,824 |
| Embedded/unassigned resident text | 1,808 (`0x710`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `free_duel` | 9 / 9 (100.00%) | 4,252 (`0x109C`) / 4,252 (`0x109C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_1` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_2` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_3` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `main_menu_language_4` | 31 / 31 (100.00%) | 18,280 (`0x4768`) / 18,280 (`0x4768`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password_a` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |
| `password_b` | 27 / 27 (100.00%) | 10,476 (`0x28EC`) / 10,476 (`0x28EC`) (100.00%) |

_Generated from `config/sles_03949/functions.csv` and `config/sles_03949/overlays/*_functions.csv`, validated against their matching-C manifests by `tools/project/progress.py`._

<!-- END GENERATED PROGRESS -->

The resident progress tables separate game-owned C-decompilation targets from
evidence-backed handwritten assembly and preserved Psy-Q CRT/SDK routines.
Each version has its own authoritative resident function inventory, validated
against its matching manifest. Runtime-overlay inventories are reported
separately for all supported regions, including both password variants where
present.
Overlay percentages describe identified function boundaries, not all executable
bytes: the Japanese, European, French, and Spanish overworld modules include
preserved code outside their inventoried function boundaries, excluded from their
denominators. See
[function-inventories.md](notes/function-inventories.md) for the reusable
version-neutral inventory scaffold.

Run `make progress` when intentionally refreshing the project-wide snapshot.
It updates the generated table above and writes detailed machine-readable
metrics to `tmp/reports/progress.json`. Routine decompilation changes do not
need to update or commit the snapshot; `make check-progress` remains available
as an opt-in consistency check.

## Quick start

Place the original executables at `game/SLUS_014.11` and
`game/japanese/SLPM_863.98`, then run:

```sh
make verify-target
make verify-japanese-target
make tools
MAKEFLAGS=-j"$(nproc)" make match
MAKEFLAGS=-j"$(nproc)" make japanese-match
```

The match targets succeed only when each rebuilt executable is byte-identical
to its retail target. Between clean acceptance builds, seed the object cache
immediately after an unchanged matching build, then use the incremental edit
loop:

```sh
tools/environments/python/bin/python tools/project/build_incremental.py --seed-existing
MAKEFLAGS=-j"$(nproc)" make match-incremental
```

Warm incremental builds reuse content-validated split output and unchanged
objects, but still relink and compare the entire executable. The first split
cache population regenerates once. Finish accepted changes with `make match`;
see [incremental build details](notes/build.md#incremental-edit-builds).

The full repository audit additionally requires the DATA files and BIN/CUE
listed in `config/slus_01411/files.sha256`:

```sh
MAKEFLAGS=-j"$(nproc)" make audit
```

Those DATA files do not have to be sourced separately. With the retail disc at
`game/rpg-yfm.cue` and `game/rpg-yfm.bin`, `make disc-files` extracts every one
of them straight out of the image at the LBAs recorded in
`config/slus_01411/disc_layout.json`:

```sh
make disc-files                          # every tracked DATA file
make disc-files FILES="WA_MRG.MRG SU.MRG"  # only the overlay archives
```

Extraction refuses to run unless the image matches the tracked `bin_sha256`,
and each extracted file is checked against its tracked SHA-256 before it
replaces anything on disk, so a wrong dump fails immediately instead of
surfacing later as a build mismatch. Files already present and correct are left
alone, so the target is safe to re-run.

The `nproc` examples allow Make to schedule independent prerequisites on all
logical CPUs. The incremental Python driver also uses a numeric `MAKEFLAGS`
job count to build invalidated components concurrently. Clean baseline object
construction and linking remain sequential. Set a smaller `-j` value
explicitly on memory-constrained systems.

## Decompilation workflow

1. Select a game-owned assembly function from
   `config/slus_01411/functions.csv`.
2. Explore materially distinct source structures, declarations, and compiler
   profiles as deeply as the function requires. Preserve precise mismatch
   evidence and candidates under `tmp/`; the historical six-row ledgers do
   not cap further research.
3. Accept C only when `make match` reproduces the entire executable exactly.
4. Commit the matching source and authoritative metadata. Refresh the README
   separately with `make progress` when a project-wide snapshot is desired.

Unmatched functions remain exact assembly fallbacks. Handwritten and Psy-Q
assembly are tracked separately from compiler-generated game code.

## Repository layout

| Path | Purpose |
|---|---|
| `src/game/` | Matching C for the resident executable |
| `src/overlays/` | Module-scoped runtime overlay sources and layout policy |
| `src/types.h` | Shared fixed-width primitive aliases |
| `asm/` | Exact assembly fallbacks and data assembly |
| `config/slus_01411/` | Function inventory, compiler profiles, symbols, and target metadata |
| `tools/` | Project scripts, pinned tools, and local toolchains |
| `notes/` | Research, plans, evidence, and detailed documentation |
| `tmp/` | Generated builds, reports, caches, and scratch work |
| `game/` | Ignored, immutable user-supplied retail inputs |

All commands must run from the repository root, and project work must remain
inside this working directory.

## Documentation

- [Setup and required inputs](notes/setup.md)
- [Build and exact-match workflow](notes/build.md)
- [Compiler and toolchain fingerprint](notes/toolchain.md)
- [Psy-Q runtime and SDK integration](notes/psyq.md)
- [Random-number generation](notes/rng.md)
- [Runtime overlay research and source layout](notes/overlays/README.md)
- [Decompilation plan](notes/decompilation-plan.md)
- [Completed remaining-function campaign](notes/remaining-decompilation-pass.md)
- [Semantic naming pass](notes/semantic-naming-pass.md)
- [Global usage data](notes/global-usage.csv)
