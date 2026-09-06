# Unchiga Pure-C Match Audit

The collaborator sources were treated as hypotheses and compiled with the local Psy-Q 4.6/GCC 2.8.1 plus MASPSX 2.81 pipeline. All work was performed sequentially. Acceptance required exact linked function bytes, equivalent relocation targets, and no allocated non-text sections.

## Results

- Initial address overlap: 94
- Rejected for actual inline assembly: 15
- Independently tested pure-C candidates: 79
- Exact pure-C matches: 41
- Pure-C nonmatches: 38
- Tool failures: 0

## Exact matches

| Address | Project symbol | Mode | Profile |
| --- | --- | --- | --- |
| `0x8001306C` | `func_8001306C` | `inline_refinement` | `gcc_2_8_1_g8_split` |
| `0x800134B4` | `func_800134B4` | `inline_refinement` | `gcc_2_8_1_g8_split` |
| `0x800154E4` | `Fade_DrawOverlay` | `collaborator_match` | `gcc_2_8_1_g8_split` |
| `0x80017DB4` | `func_80017DB4` | `inline_refinement` | `gcc_2_8_1_g8_split` |
| `0x80017E3C` | `func_80017E3C` | `inline_refinement` | `gcc_2_8_1_g8_split` |
| `0x80020D4C` | `func_80020D4C` | `inline_refinement` | `gcc_2_8_1_g8_no_split` |
| `0x80021810` | `Duel_SelectCardDrop` | `inline_refinement` | `gcc_2_8_1_g8_split` |
| `0x80022618` | `func_80022618` | `inline_refinement` | `gcc_2_8_1_g8_split` |
| `0x800234E4` | `func_800234E4` | `collaborator_match` | `gcc_2_8_1_g8_split` |
| `0x80023FBC` | `func_80023FBC` | `collaborator_match` | `gcc_2_8_1_cc_g0_as_g8_no_split` |
| `0x800245EC` | `func_800245EC` | `collaborator_match` | `gcc_2_8_1_cc_g0_as_g8_no_split` |
| `0x80024C1C` | `func_80024C1C` | `collaborator_match` | `gcc_2_8_1_g8_split` |
| `0x800250C8` | `func_800250C8` | `collaborator_match` | `gcc_2_8_1_g8_split` |
| `0x800278A0` | `func_800278A0` | `collaborator_match` | `gcc_2_8_1_g8_split` |
| `0x8002C518` | `func_8002C518` | `inline_refinement` | `gcc_2_8_1_g8_split` |
| `0x8002C6C8` | `func_8002C6C8` | `collaborator_match` | `gcc_2_8_1_g8_split` |
| `0x8002CB80` | `Duel_CalcGuardianStarMatchup` | `inline_refinement` | `gcc_2_8_1_g8` |
| `0x80036BCC` | `func_80036BCC` | `inline_refinement` | `gcc_2_8_1_g8_split` |
| `0x800373C8` | `func_800373C8` | `inline_refinement` | `gcc_2_8_1_g8_split` |
| `0x8003787C` | `func_8003787C` | `inline_refinement` | `gcc_2_8_1_g8_split` |
| `0x8003CEB8` | `func_8003CEB8` | `inline_refinement` | `gcc_2_8_1_g8` |
| `0x8003D334` | `func_8003D334` | `collaborator_match` | `gcc_2_8_1_g8` |
| `0x80041D60` | `func_80041D60` | `collaborator_match` | `gcc_2_8_1_cc_g0_as_g8_no_split` |
| `0x80044DC0` | `func_80044DC0` | `inline_refinement` | `gcc_2_8_1_cc_g0_as_g8_no_split` |
| `0x80044FFC` | `func_80044FFC` | `inline_refinement` | `gcc_2_8_1_g8` |
| `0x80045484` | `func_80045484` | `collaborator_match` | `gcc_2_8_1_cc_g0_as_g8_no_split` |
| `0x8004763C` | `func_8004763C` | `inline_refinement` | `gcc_2_8_1_cc_g0_as_g8_no_split` |
| `0x80047FAC` | `func_80047FAC` | `collaborator_match` | `gcc_2_8_1_cc_g0_as_g8_no_split` |
| `0x80049010` | `func_80049010` | `inline_refinement` | `gcc_2_8_1_cc_g0_as_g8_no_split` |
| `0x80049600` | `func_80049600` | `inline_refinement` | `gcc_2_8_1_cc_g0_as_g8_no_split` |
| `0x800497E0` | `func_800497E0` | `inline_refinement` | `gcc_2_8_1_cc_g0_as_g8_no_split` |
| `0x8004C84C` | `func_8004C84C` | `inline_refinement` | `gcc_2_8_1_cc_g0_as_g8_no_split` |
| `0x80057E20` | `func_80057E20` | `collaborator_match` | `gcc_2_8_1_g8` |
| `0x80058EC0` | `func_80058EC0` | `inline_refinement` | `gcc_2_8_1_g8_split` |
| `0x800592AC` | `func_800592AC` | `collaborator_match` | `gcc_2_8_1_g8_split` |
| `0x80059A50` | `func_80059A50` | `inline_refinement` | `gcc_2_8_1_g8` |
| `0x8005F5C8` | `func_8005F5C8` | `collaborator_match` | `gcc_2_8_1_g8` |
| `0x8005F828` | `func_8005F828` | `inline_refinement` | `gcc_2_8_1_g8` |
| `0x80070D00` | `AiScript_Call` | `inline_refinement` | `gcc_2_8_1_g8_split` |
| `0x80070E20` | `AiScript_SetRandom` | `inline_refinement` | `gcc_2_8_1_g8_split` |
| `0x80071320` | `AiScript_SetFaceDown` | `collaborator_match` | `gcc_2_8_1_g8_split` |

## Post-terminal resolutions

Three results originally failed this audit's strict symbolic-relocation gate
but were later resolved through full executable matching:

| Address | Semantic name | Resolution |
|---|---|---|
| `0x8003D0F4` | `SaveData_ApplyRuntimeState` | Three isolated write-only addresses use verified C integer literals to preserve retail `$at` materialization. |
| `0x8003D46C` | `DuelEffect_CreateChannel` | One isolated write-only address uses the same verified literal exception. |
| `0x8004545C` | `SD_ArmBusyCallback` | Two C absolute-address expressions replace the prior register-pinned matching source. |

All three complete linked executables match the retail SHA-256. Their original
audit rows remain historical evidence; the accepted resolutions are recorded
as `post_terminal_resolution`.

## Nonmatches

| Address | Project symbol | Mode | First difference |
| --- | --- | --- | --- |
| `0x80017034` | `func_80017034` | `inline_refinement` | `+0x0: 21308000 != ac02838f` |
| `0x80018080` | `func_80018080` | `inline_refinement` | `+0x1A: 6a000492 != 6a000392` |
| `0x800240B0` | `func_800240B0` | `collaborator_match` | `+0x64: 36004014 != 38004014` |
| `0x8002892C` | `func_8002892C` | `collaborator_match` | `+0xC: 18006010 != 19006010` |
| `0x8002C938` | `func_8002C938` | `inline_refinement` | `+0x1D: c0180600 != c0100600` |
| `0x8002E370` | `func_8002E370` | `inline_refinement` | `+0x6: 0f80023c != 0f80033c` |
| `0x80031354` | `func_80031354` | `collaborator_match` | `+0x38: 0c004014 != 0d004014` |
| `0x80033CC4` | `func_80033CC4` | `inline_refinement` | `+0x2: 0980023c != 0980033c` |
| `0x80035CE4` | `func_80035CE4` | `inline_refinement` | `+0x2: 0f80023c != 0f80033c` |
| `0x80035DB8` | `func_80035DB8` | `inline_refinement` | `+0xA: 0f80023c != 0f80033c` |
| `0x80035DF4` | `func_80035DF4` | `inline_refinement` | `+0x8: 88b24224 != a0b24224` |
| `0x8003735C` | `func_8003735C` | `inline_refinement` | `+0x2: 5c008294 != 5c008394` |
| `0x800400AC` | `func_800400AC` | `collaborator_match` | `+0x9: 2148a000 != 2140a000` |
| `0x8004020C` | `func_8004020C` | `collaborator_match` | `+0x6: 00008384 != 00008584` |
| `0x800428EC` | `func_800428EC` | `collaborator_match` | `+0x4: 6c008327 != 160085a0` |
| `0x80044CFC` | `func_80044CFC` | `inline_refinement` | `+0x2: 0a80023c != 0a80013c` |
| `0x80044D0C` | `func_80044D0C` | `inline_refinement` | `+0x0: 0a80033c != 01000224` |
| `0x80044D20` | `func_80044D20` | `inline_refinement` | `+0x0: 0a80033c != 02000224` |
| `0x80044D34` | `func_80044D34` | `inline_refinement` | `+0x0: 0a80033c != 03000224` |
| `0x80058FB0` | `func_80058FB0` | `inline_refinement` | `+0x1: c0180400 != c0100400` |
| `0x80059134` | `func_80059134` | `inline_refinement` | `+0x1: c0180400 != c0100400` |
| `0x80060170` | `func_80060170` | `inline_refinement` | `+0x2: 0f80023c != 0f80063c` |
| `0x800601D0` | `func_800601D0` | `inline_refinement` | `+0x2: 0f80023c != 0f80033c` |
| `0x800705AC` | `AiScript_ReadShort` | `inline_refinement` | `+0x2: 0f80023c != 0f80043c` |
| `0x80070DA8` | `AiScript_Return` | `inline_refinement` | `+0x6: 0f80023c != 0f80033c` |
| `0x80070F1C` | `AiScript_PlayFieldCard` | `collaborator_match` | `+0x0: d8ffbd27 != e0ffbd27` |
| `0x80071510` | `func_80071510` | `inline_refinement` | `+0x12: 0f80043c != 0f80033c` |
| `0x8007164C` | `AiScript_LoadBestDifference` | `inline_refinement` | `+0x12: 0f80043c != 0f80033c` |
| `0x80071688` | `AiScript_LoadBestAttacker` | `inline_refinement` | `+0x12: 0f80043c != 0f80033c` |
| `0x800716C4` | `AiScript_LoadBestTarget` | `inline_refinement` | `+0x12: 0f80043c != 0f80033c` |
| `0x800732A0` | `AiScript_PushComboEmpty` | `collaborator_match` | `+0x6: 0f80023c != 0f80033c` |
| `0x80073380` | `AiScript_ClearCards` | `inline_refinement` | `+0x8: e85b4224 != 265c4224` |
| `0x800733A8` | `AiScript_AddType` | `collaborator_match` | `+0x6: 1000b0af != 1000bfaf` |
| `0x80073420` | `AiScript_ClearTypes` | `inline_refinement` | `+0x8: e85b4224 != 005c4224` |
| `0x800735DC` | `AiScript_SetRegister` | `inline_refinement` | `+0x18: 0f80033c != 80801000` |

## Rejected collaborator definitions

These files were excluded without consuming a pure-C attempt because direct source inspection found GCC inline assembly.

| Address | Reason |
| --- | --- |
| `0x80025028` | all collaborator definitions contain inline assembly |
| `0x80025BEC` | all collaborator definitions contain inline assembly |
| `0x800291E0` | all collaborator definitions contain inline assembly |
| `0x8002CE64` | all collaborator definitions contain inline assembly |
| `0x8002D370` | all collaborator definitions contain inline assembly |
| `0x8002D458` | all collaborator definitions contain inline assembly |
| `0x8002D62C` | all collaborator definitions contain inline assembly |
| `0x8002D6C8` | all collaborator definitions contain inline assembly |
| `0x80030D5C` | all collaborator definitions contain inline assembly |
| `0x80036C14` | all collaborator definitions contain inline assembly |
| `0x8003700C` | all collaborator definitions contain inline assembly |
| `0x8003CDF8` | all collaborator definitions contain inline assembly |
| `0x8003CE48` | all collaborator definitions contain inline assembly |
| `0x8003CE74` | all collaborator definitions contain inline assembly |
| `0x800472A8` | all collaborator definitions contain inline assembly |

Detailed manifests, source hashes, result JSON, command logs, and per-candidate build artifacts are under `tmp/agents/unchiga-integration/`.
