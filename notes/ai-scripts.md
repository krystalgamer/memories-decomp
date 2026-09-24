# Opponent AI scripts

The computer's decisions come from two bytecode programs on the disc, run by
the VM in [`src/game/ai_script_vm.c`](../src/game/ai_script_vm.c). This note
covers where the programs live, the full instruction set, and what the programs
do for each duelist. Use it together with [ai-structures.md](ai-structures.md),
which describes the VM state and the slot map.

`tools/project/ai_script_disasm.py` disassembles the programs from your own
disc image. The tool uses only the Python standard library:

```text
python tools/project/ai_script_disasm.py    # reads game/DATA/WA_MRG.MRG
# writes tmp/ai-scripts/: hand_full.txt, field_full.txt, NN_<duelist>.txt,
# index.md, verify.txt
```

The output is game data. It is written to the ignored `tmp/` directory and
must not be committed.

Each claim below carries one of these tags:

* **[code]**: read from the matching C in `src/game`, with the retail address
  where one is known;
* **[script]**: read from the disassembled bytecode, which the tool
  reproduces;
* **[inferred]**: an interpretation that no code or listing states directly.
  Each one says why it is believed.

## 1. Where the scripts are and how they are selected

* **[code]** `Duel_LoadTerrainPackage` loads a 235-sector record from
  `WA_MRG.MRG` sector `0x16C6 + gDuel_bTerrain * 0xEB`
  ([runtime-loader.md](overlays/runtime-loader.md#seven-235-sector-wa-records)).
  Phase 8 of `Duel_LoadPackageStage`
  ([duel_load_package_stage.c](../src/game/duel_load_package_stage.c))
  copies relative sectors 184-186 (`0x1800` bytes) to `0x801A8000`. Phase 9
  copies sectors 187-189 to `0x801A9800`.
* **[code]** The hand-phase state `DuelScene_UpdateHandActions`
  ([duel_scene_hand_actions.c](../src/game/duel_scene_hand_actions.c)) calls
  `func_80028220()` and then `AiScript_Init(D_801A8000)`. The field-phase
  state ([duel_scene_field_actions.c](../src/game/duel_scene_field_actions.c))
  calls `AiScript_Init(D_801A9800)`. `AiScript_Init` (`0x800705D8`) sets the
  cursor to the base address, so both programs always start at offset 0. The
  program is **not** chosen per duelist: there is one **hand script** and one
  **field script**, and every duelist runs them.
* **[script]** Each program starts by running `LoadOpponentID ->r15` (the
  1-based `gDuel_bOpponentID`). It then branches on r15 by comparing it with
  constants loaded by `Store` (`JumpEqual` / `JumpNotEqual`). It also reads
  per-duelist numbers from `gDuel_aOpponentData` (`0x800917F0`,
  [ai_opponent_data.c](../src/game/ai_opponent_data.c)) through
  `LoadOpponentData`. A duelist's "personality" is therefore three things:
  (a) the r15 branches that name it, (b) its nine-byte row in
  `gDuel_aOpponentData`, and (c) the deck it draws.
* **[script]** All seven terrain records hold byte-identical copies of both
  chunks. The SHA-256 prefixes are hand `0cda7f9ce737e35c` and field
  `d8bf4e5b8d135721`. The terrain does not change the AI program.
* **[script]** Layout inside each 6 KB chunk:

  | Chunk | Code | Rest of the chunk |
  |---|---|---|
  | hand, `0x801A8000` | `0x0000-0x1498`: 1314 instructions, 5273 bytes. The last instruction is `EndHand` at `0x1498`. | 871 bytes (`0x1499-0x17FF`) that do not form an instruction stream. They start with `0xFF`, and nothing jumps into them. |
  | field, `0x801A9800` | `0x0000-0x0D07`: 795 instructions, 3336 bytes. The last instruction is `EndField` at `0x0D07`. | 2808 bytes (`0x0D08-0x17FF`) that are byte-identical to the hand chunk at the same offsets. |

  **[inferred]** The field chunk was probably written into a buffer that
  still held the hand chunk. Its tail is the end of the hand script plus the
  same 871 residue bytes. The game never executes these bytes.
* **[script]** Code and data are not mixed. A linear sweep from offset 0
  finds exactly the instruction starts that recursive descent finds.

## 2. The VM, briefly

Everything in this section is **[code]** unless marked otherwise. The details
are in [ai-structures.md](ai-structures.md#interpreter-dispatch-and-yielding).

* **Registers:** `gAiScript_aMemory` (`0x800F5B98`) holds 20 signed words,
  r0-r19. `AiScript_Init` zeroes them. With two exceptions, an operand byte
  is a *register index*, not a value. The exceptions are `Store`, which takes
  a 16-bit immediate, and `SetRandom`, which takes two 16-bit bounds.
  Constants reach the other instructions by first being stored in a register.
  **[script]** Every register operand in reachable code is below 20.
* **Jumps:** targets are 16-bit little-endian offsets from the start of the
  chunk (`script_base`). `Call` pushes the cursor onto an 8-entry stack.
  Overflowing that stack, or returning with an empty stack, prints an assert
  from `src/hirata/H_mctrl1.c` and hangs. **[script]** In both scripts the
  deepest nesting is one call, and no return runs with an empty stack.
* **Chance:** `JumpRandom rN, L` jumps when `rand() % 100 < rN`. The PsyQ
  `rand` is described in [rng.md](rng.md). A chain such as
  `JumpRandom 40% A; JumpRandom 50% B; Jump C` therefore gives A 40%,
  B 30% and C 30%. The per-duelist listings print this product for every
  chain. `SetRandom lo, hi, rD` sets `rD = rand() % (hi - lo + 1) + lo`.
* **Ending a run:** `AiScript_Run` (`0x80070650`) returns 1 after `EndHand`,
  3 after `EndField` and 2 after `PlayFieldCard`. After any other
  instruction it returns 0 if `VSync(1) >= 240` (a time slice). The caller
  runs it again on the next frame, and the cursor continues from where it
  stopped. The scripts contain no explicit yield instruction.
* **Hand phase:** a single run ends at the one `EndHand` that is reached. The
  outcome is the selection record at `D_800EAE88` (`AiSelection`):
  * bytes 0-4: the hand slots to play, a list of up to five, which is the
    fusion material list;
  * byte 6: the destination slot, set by `SetPosition`;
  * byte 7: the guardian-star choice;
  * byte 8: a flag (`D_800EAE90`).
* **Field phase:** each `PlayFieldCard slot, a, b, c` ends the run with one
  field action. The meaning of the operands is **[code]**
  ([ai_script_end.c](../src/game/ai_script_end.c)):

  | Operand | Stored in | Meaning |
  |---|---|---|
  | `slot` | `[9]` | the acting AI card |
  | `c` (category) | `[10]` | a target slot. The value 255 is stored as `0x3A`. |
  | `b` | `[11]` | the third value of the record |

  When `c = 0`, `[10]` becomes `slot` only if `slot >= 6` and `a == 1`, which
  means activating the magic or trap card itself. Otherwise `[10]` becomes 0.
  **[code]** When the caller in
  [duel_scene_field_actions.c](../src/game/duel_scene_field_actions.c) finds
  `[10] == [11] == 0`, it sets the used-this-turn flag (`0x4000`) on that
  card. It then returns to state 2 with the `0x80` "initialised" bit clear.
  **[inferred]** That makes the next pass call `func_80028220` and
  `AiScript_Init` again. So the field script restarts from offset 0 after
  every action, with a fresh board snapshot, and each card is visited at
  most once per turn because acted-on cards carry the used flag.
* **Board snapshot:** `func_80028220` / `func_80027DF8` export
  `gDuel_aActiveCards` in this layout:

  | Slots | Contents |
  |---|---|
  | 1-5 | AI monsters |
  | 6-10 | AI magic/trap row |
  | 11 onward | AI hand (compacted), then **the AI's remaining deck in draw order** |
  | 56-60 | player monsters |
  | 61-65 | player magic/trap row |
  | 66-70 | player hand |

  **[code]** The "AI hand" range in every search is
  `11 .. 11 + gDuel_aOpponentData[id][0] - 1` (`Ai_GetHandSize`). For
  duelists whose value is above 5, that window therefore covers the next
  cards of their deck. See §5.

### Selection-record flag (byte 8)

The opcodes that set and clear it are named `SetSelectionRandom` and
`ClearSelectionRandom`; the names are mechanical.

* **[script]** The hand script sets the flag whenever a card goes to a free
  zone slot: a monster to a monster slot, a magic card to an empty
  magic/trap slot, or a trap card to a random magic/trap slot. It clears the
  flag when a card is used on something already there: a burn or field card,
  a fusion onto a field monster, or an equip.
* **[inferred]** So the flag most likely means "place in the slot given by
  `SetPosition`". The opposite meaning would be "use on that slot or
  activate".

## 3. Instruction set

Operand notation:

| Notation | Meaning |
|---|---|
| `rX` | a register read |
| `->rD` | the register written |
| `L` | a 16-bit jump target |
| `imm` | a 16-bit immediate |
| `b` | a raw byte |

The zone codes of `CountCards`, `FindFirstCard`, `FindCard` and `MatchType`
use `Ai_GetCardRange`:

| Code | Zone |
|---|---|
| 0 | AI monsters |
| 1 | AI monsters not used this turn |
| 2 | AI magic/trap row |
| 3 | AI magic/trap row, not used |
| 4 | AI hand window |
| 5 | player monsters |
| 6 | player monsters, not used |
| 7 | player magic/trap row |
| 8 | player magic/trap row, not used |
| 9 | player hand |

`FindStrongest` and `FindWeakest` use `Ai_GetWinningCardRange`, which is a
**different** encoding:

| Code | Zone |
|---|---|
| 0 | AI monsters |
| 1 | AI monsters not used this turn |
| 2 | AI hand window |
| 3 | player monsters |
| 4 | player attack-position monsters, not used |
| 5 | player defence-position monsters, not used |
| 6 | player hand |

"sets" operands take these values:

| Value | Meaning |
|---|---|
| 1 | skip cards in the card set (`AddCard`) |
| 2 | skip cards in the type set (`AddType`) |
| other | no exclusion |

"hide" operands take these values:

| Value | Meaning |
|---|---|
| 1 | a face-down player card is skipped or not matched |
| 0 | a face-down player card is seen with its real stats |

All lengths and semantics in the table are **[code]**. Every operand length
was read from the handler; none was inferred.

| Op | Name (handler) | Len | Operands | Effect | Source, address |
|---:|---|---:|---|---|---|
| 00 | (null) | - | - | The table slot is null, so executing it crashes. It never occurs in reachable code. | `ai_script_commands.c` |
| 01 | Jump | 3 | L | goto L | `ai_script_control_flow.c` `0x80070988` |
| 02 | JumpGreaterEqual | 5 | rA rB L | if rA >= rB goto L | same, `0x800709C0` |
| 03 | JumpGreater | 5 | rA rB L | if rA > rB goto L | `0x80070A40` |
| 04 | JumpEqual | 5 | rA rB L | if rA == rB goto L | `0x80070AC0` |
| 05 | JumpNotEqual | 5 | rA rB L | if rA != rB goto L | `0x80070B3C` |
| 06 | JumpBetween | 6 | rV rHi rLo L | if rLo <= rV <= rHi goto L. Unused. | `0x80070BB8` |
| 07 | JumpRandom | 4 | rP L | if rand()%100 < rP goto L | `0x80070C60` |
| 08 | Call | 3 | L | push return address, goto L | `0x80070D00` |
| 09 | Return | 1 | - | pop | `0x80070DA8` |
| 0A | SetRandom | 6 | imm lo, imm hi, ->rD | rD = lo + rand()%(hi-lo+1) | `0x80070E20` |
| 0B | Subtract | 4 | rA rB ->rD | rD = rA - rB | `0x80070EB4` |
| 0C | PlayFieldCard | 5 | rSlot rA rB rCat | Emit a field action and end the run (returns 2). See §2. | `ai_script_end.c` `0x80070F1C` |
| 0D | EndHand | 1 | - | end the run (returns 1) | `0x80070FF8` |
| 0E | EndField | 1 | - | end the run (returns 3) | `0x80071000` |
| 0F | CalcCardPower | 4 | rCard rMode ->rD | Base ATK (mode 0), DEF (1) or the higher of the two. A non-monster gives 0, except in mode 0 for Sparks..Tremendous Fire (50/100/200/500/1000). | `ai_script_calc_card_power.c` `0x80071008` |
| 10 | TestHighStat | 3 | rCard ->rD | 1 if a monster's DEF >= ATK. Unused. | `ai_script_query_commands.c` `0x80071194` |
| 11 | LoadCardType | 3 | rCard ->rD | card type (0 Dragon .. 23 Equip) | `0x80071248` |
| 12 | LoadCardID | 3 | rSlot ->rD | card id in a snapshot slot | `0x800712B4` |
| 13 | SetFaceDown | 3 | rSlot ->rD | 0 empty, 1 face-up, 2 face-down. Unused; the name is mechanical. | `0x80071320` |
| 14 | LoadLifePoint | 3 | rSide ->rD | life points of record `1 - side` | `0x800713B8` |
| 15 | LoadOpponentID | 2 | ->rD | `gDuel_bOpponentID` | `ai_script_load_duel_globals.c` `0x80071424` |
| 16 | LoadTerrain | 2 | ->rD | `gDuel_bTerrain` (0 Normal .. 6 Dark) | `0x80071460` |
| 17 | LoadDeckSize | 2 | ->rD | Non-empty slots 11..55 minus 5, which is the AI's cards left in its deck. | `ai_script_combo.c` `0x8007149C` |
| 18 | LoadSelectionRandom | 2 | ->rD | Reads byte 8 of the selection record. Unused. | `0x80071510` |
| 19 | TestPinned | 3 | rSide ->rD | Swords of Revealing Light turns remaining != 0. Side 0 is the player, side 1 is the AI itself **[inferred from use]**. | `0x8007154C` |
| 1A | StartCombo | 2 | ->rD | first field slot (1..10) of the selected combo | `0x800715C4` |
| 1B-1D | LoadBestDifference / LoadBestAttacker / LoadBestTarget | 2 | ->rD | Read the results of FindBestAttack. Unused. | `0x8007164C` / `0x80071688` / `0x800716C4` |
| 1E | FindStrongest | 6 | rStat rZone rHide rSets ->rD | slot with the highest ATK/DEF/either, or 0 | `0x80071700` |
| 1F | FindWeakest | 6 | rStat rZone rHide rSets ->rD | slot with the lowest stat, or 0 | `0x80071924` |
| 20 | FindKiller | 4 | rTarget rMode ->rD | The unused AI monster with the **lowest ATK** that still beats the target's ATK (mode 0) or DEF (mode 1), guardian-star bonus included. | `ai_script_find_killer.c` `0x80071B64` |
| 21 | FindDefenseStopper | 3 | rHide ->rD | 1 if the AI's monsters, paired strongest against strongest, beat every player monster | `ai_script_find_card.c` `0x80071CB0` |
| 22 | CountCards | 3 | rZone ->rD | number of cards in the zone | `0x80071EB8` |
| 23 | FindFirstCard | 4 | rState rZone ->rD | first slot that is empty (0), face-up (1) or face-down (2) | `0x80071FC8` |
| 24 | FindCard | 5 | rCard rZone rHide ->rD | first slot holding that card id. With hide=1, a face-down card in a player zone does not count. | `0x8007214C` |
| 25 | MatchType | 6 | rType rZone rHide rSets ->rD | first slot of that card type | `ai_script_match_type.c` `0x800722CC` |
| 26 | FindEquipTarget | 5 | rCard rMode rSets ->rD | first AI monster (1..5) that the equip card can equip (`Duel_CheckEquip`); mode 1 skips monsters used this turn | `ai_fusion.c` `0x8007249C` |
| 27 | CheckRitual | 3 | rCard ->rD | 1 if `Duel_CheckRitual(0, card)` | `0x800725C4` |
| 28 | FindFirstMonster | 2 | ->rD | Unused. | `0x80072640` |
| 29 | FindFirstType | 3 | rType ->rD | Unused. | `0x800726F4` |
| 2A | FindBestCombo | 4 | rDepth rSets ->rD | Search AI field monsters and the hand window, up to depth+1 cards, for the highest ATK-or-DEF result (`Ai_CompleteFusion`, `0x800727C0`). Result: 0 = the combo starts in the hand, 1 = it starts on the field and uses more cards, 2 = the field card alone is best, 3 = nothing. | `0x80072A48` |
| 2B | EvaluateFusion | 5 | rCard rDepth rSets ->rD | Same search, seeded with one given card. 0 = something better was found. | `0x80072DC0` |
| 2C / 2D | SkipHand / SkipField | 5 | b b b b | Read and ignore four bytes. Unused. | `ai_script_skip.c` `0x80072F1C` / `0x80072F54` |
| 2E | PlayFaceUp | 6 | r0..r4 | Write selection bytes 0-4, the hand slots to play. The name is mechanical. | `ai_script_actions.c` `0x80072F8C` |
| 2F | SetPosition | 2 | rSlot | selection byte 6 (destination slot) | `0x80073050` |
| 30 | FindBestAttack | 3 | rHide ->rD | Unused. The decompiled outer loop has no increment; do not rely on it. | `ai_script_find_best_attack.c` `0x8007308C` |
| 31 | PushComboCard | 1 | - | Clear selection bytes 0-5, then copy the combo's hand slots (>= 11) into them. | `ai_script_state_ops.c` `0x80073220` |
| 32 | PushComboEmpty | 1 | - | the same copy without the clear | `0x800732A0` |
| 33 / 34 | HandNop / FieldNop | 1 | - | Do nothing. Unused. | `0x80073300` / `0x80073308` |
| 35 | AddCard | 2 | rCard | add to the 32-entry card set | `0x80073310` |
| 36 | ClearCards | 1 | - | Unused. | `0x80073380` |
| 37 / 38 | AddType / ClearTypes | 2 / 1 | rType / - | Edit the 25-entry type set. Unused. | `0x800733A8` / `0x80073420` |
| 39 / 3A | SetSelectionRandom / ClearSelectionRandom | 1 | - | set / clear selection byte 8 | `0x80073448` / `0x80073458` |
| 3B / 3C | func_80073464 / func_80073474 | 1 | - | Set / clear `gAiScript_State.field_97`. Unused, and nothing reads the field. | `0x80073464` / `0x80073474` |
| 3D | MoveCard | 2 | rSlot | Set the used-this-turn flag on a snapshot slot. The scripts use it as a "done with this target" marker. The name is mechanical. | `0x80073480` |
| 3E | LoadOpponentData | 4 | rId rField ->rD | field 0 gives `values[1]*100`; field f > 0 gives `values[f+1]` | `0x800734DC` |
| 3F | Store | 4 | imm ->rD | rD = imm | `0x800735A0` |
| 40 | SetRegister | 3 | rS ->rD | copy | `0x800735DC` |
| 41 | Add | 4 | rA rB ->rD | rD = rA + rB | `0x80073624` |
| 42 | LoadGuardianStarChoice | 2 | rX | selection byte 7 = low byte of rX | `ai_script_state_ops.c` |
| 43 | Print | 2 | b | Print a "check_point" debug line. Unused. | `0x800736C4` |

**[script]** The two scripts use 47 of the 67 opcodes; the hand script uses
40 and the field script 33. None of the 20 that neither uses is needed to
understand the retail AI: `JumpBetween`, `TestHighStat`, `SetFaceDown`,
`LoadSelectionRandom`, `LoadBest*`, `FindFirstMonster`, `FindFirstType`,
`SkipHand`/`SkipField`, `FindBestAttack`, the two nops, `ClearCards`,
`AddType`/`ClearTypes`, the `field_97` pair and `Print`. Because
`FindBestAttack` is unused, the matchup search described in older notes is
dead code in retail.

## 4. What the hand script does

Every step here is **[script]**, from `hand_full.txt`. Offsets are from
`0x801A8000`. The script runs once per AI hand phase and stops at the first
decision it reaches.

1. **Prelude, `0x0000`.** It sets r15 to the duelist id.
   * `Call 0x001E` sets r14 to **0 for duelists 8, 15, 35, 36, 37 and 38**
     (Heishin, Pegasus, Heishin 2nd, Seto 3rd, DarkNite, Nitemare) and to 1
     for everyone else. r14 is the "hide" operand of almost every later
     search of the player's side.
   * r6 = `LoadLifePoint 0` and r7 = `LoadLifePoint 1`. **[inferred]** r6 is
     the AI's life points and r7 the player's, because r7 is compared with
     the damage of each burn card before the burn is used as a finisher.
   * r16 = duelist `values[7]`, a percentage.
2. **Lethal burn, `0x005E`.** Only if the player has **no** card in the
   magic/trap row, the script looks for Sparks, Hinotama, Final Flame,
   Ookazi and Tremendous Fire in the hand window. It plays the first one
   whose damage (50/100/200/500/1000) is at least the player's LP.
3. **Home terrain, `0x0130`.** The script plays the duelist's field card when
   it holds that card and the terrain differs:

   | Duelist | Id | Field card |
   |---|---:|---|
   | Ocean Mage | 21 | Umi |
   | Forest Mage | 23 | Forest |
   | Mountain Mage | 25 | Mountain |
   | Desert Mage | 27 | Wasteland |
   | Meadow Mage | 29 | Sogen |
   | Guardian Neku | 34 | Yami |

   These are the five shrine-gate Mages and Neku. **No High Mage has this
   rule.** The claim in the-game.md §5.11 that the High Mages restore their
   terrain is not what the script does. High Mages do play field cards, but
   only through strategy A in step 6 (the `FindBestCombo` result-2 path at
   `0x0BFE`).
4. **Who is winning, `0x024C`.** The script compares the strongest AI field
   monster with the strongest player field monster, both by the higher of
   ATK and DEF, with r14 deciding whether face-down player cards count.
   * If **both** sides have no monster (the usual opening turn), it
     continues with step 5, as if ahead (`0x0270` tests the sum first).
   * Otherwise, if the AI has no monster, or its best is not stronger, it
     goes to **"behind"** (step 8).
   * Otherwise it continues with step 5.
   * **[code]** The two field monsters are *chosen* by the snapshot's
     `attack`/`defense`, which `func_80027DF8` fills from
     `Duel_CalcCardStats` (terrain and equips included). They are then
     *compared* by `CalcCardPower`, which uses base stats
     (`Duel_GetBaseCardStat`). The same split applies to the field script's
     lethal check (field step 4.1), while `FindKiller` and
     `FindDefenseStopper` use only the boosted snapshot values.
5. **Harpie's Feather Duster, `0x0299`.** If the player has a magic/trap
   card and the AI holds Harpie's Feather Duster, the AI plays it with
   probability r16 (`values[7]`).
6. **Strategy roll, `0x02DE`.**
   * If the AI's LP is below `values[1] * 100`, the script always takes
     strategy B.
   * Otherwise a per-duelist roll picks strategy A, B or C (the table in §6).
     There are two tables: one is used when the AI's weakest field monster is
     at least as strong as the player's best (or the AI has none), and the
     other when it is weaker.

   | Strategy | Offset | What it does |
   |---|---|---|
   | A | `0x0BA5` | Best fusion. `FindBestCombo` with depth `values[3]`, reduced to 1 when the AI's deck has `values[2]` cards or fewer. It plays the whole chain from the hand to an empty monster slot (or over its weakest monster), or fuses onto the field monster where the chain starts. If the best "combo" is a single field card, it plays that monster type's field card when it has one and the terrain differs, for example Mountain for a Dragon. Otherwise it plays its strongest hand monster, or, if that is not a monster, a random card from slots 11-15 (`SetRandom 11, 15`). If nothing is found (result 3), it falls through to C. |
   | B | `0x0FDB` | Power up the weakest field monster. `EvaluateFusion` from the AI's weakest field monster with depth `values[4] - 1` (0 when the deck is low). Cards that fail are added to the card set and the next weakest is tried. |
   | C | `0x1042` | Play a card that is not a monster: the first trap, else equip, else magic, else ritual card in the hand window. It goes to a free magic/trap slot. An equip without a free slot goes straight onto a monster (`FindEquipTarget`, else the weakest). A trap without a free slot overwrites a random slot 6-10. With none of these in the window, it plays the strongest monster. |

7. **Fusion placement, `0x13EF`/`0x1420`/`0x1493`.** The destination is:
   * a monster: the first empty AI monster slot, else the slot of the AI's
     weakest monster;
   * a magic card: the first empty magic/trap slot.

   `LoadGuardianStarChoice` is always given 0, so **the AI always takes the
   first guardian star**.
8. **Behind, `0x10E0`.**
   * If the strongest monster in the hand window is at least as strong as
     the player's best, the AI plays it.
   * Else, while the AI's deck still has 33 or more cards (roughly the first
     two turns), it goes to strategy A.
   * Else it tries removal against the player's strongest monster in this
     order, playing the first card it holds:
     1. the type-specific card for that monster's type: Dragon Capture Jar,
        Warrior Elimination, Eternal Rest, Stain Storm, Eradicating Aerosol,
        Breath of Light, Eternal Draught;
     2. Spellbinding Circle;
     3. Shadow Spell;
     4. Crush Card, when the target has ATK >= 1500;
     5. Raigeki;
     6. a trap: Widespread Ruin, Acid Trap Hole, Invisible Wire, Bear Trap,
        Eatgaboon, House of Adhesive Tape or Fake Trap;
     7. Swords of Revealing Light, unless the player is already pinned;
     8. Dark Hole, when the AI has fewer monsters than the player.
   * If it holds none of these, it goes to strategy A.

## 5. What the field script does

Every step here is **[script]**, from `field_full.txt`. Each run yields one
action. The caller then restarts the script (§2), so the order below is a
priority list that the script walks again after every action.

1. **Prelude.** The same as in the hand script: r14, r6/r7, and
   r16 = `values[7]`.
2. **Magic and trap cards already on the AI's row**, for each card not used
   this turn:
   * **Harpie's Feather Duster:** activated with probability r16 if the
     player has magic/trap cards. If the player has none, it is kept.
   * **Burn cards and heal cards** (Mooyan Curry, Red Medicine, Goblin's
     Secret Remedy, Soul of the Pure, Dian Keto). All of these use the same
     rule:
     1. kept if the AI's LP >= the player's;
     2. otherwise activated if the AI's LP > `values[1] * 100`;
     3. otherwise activated if the player's row is empty;
     4. otherwise kept with probability r16 and activated the rest of the
        time.
   * **Equip:** activated onto the first monster it fits
     (`FindEquipTarget`), else kept.
   * **Ritual:** activated if `CheckRitual` passes.
   * **Dark-piercing Light:** activated only if every player monster is
     face-down and the player has more monsters than the AI.
   * **Swords of Revealing Light:** activated if the player is not pinned and
     the AI has no monster or the player's best is stronger.
   * **Stop Defense:** activated if the AI is not pinned and either (a) its
     strongest ATK minus the player's weakest ATK is at least the player's
     LP, or (b) the AI does not have fewer monsters and `FindDefenseStopper`
     says its monsters beat all of the player's.
   * **Type-removal cards** (as in hand step 8): activated only if the
     player's strongest monster is at least as strong as the AI's strongest
     **and** is of the
     matching type. Spellbinding Circle and Shadow Spell need only the
     first condition. Crush Card also needs ATK >= 1500. Raigeki also needs
     the player to have at least as many monsters as the AI. Dark Hole also
     needs the player to have more monsters.
   * **Field cards:** activated if the player's strongest is at least as strong
     as the AI's, the two are of different types, the terrain is not already that field,
     and the field boosts the AI's type but not the player's. On Umi the
     rule also counts Machine and Pyro as losers.
3. **Wall monsters, `0x08F4`.** Any unused AI monster from a fixed list of
   20 high-DEF cards is switched to defence:
   Cocoon of Evolution, Millennium Shield, Labyrinth Wall, Mystical Elf,
   Dragon Piper, Castle of Dark Illusions, Metal Guardian, Sleeping Lion,
   Hard Armor, Spirit of the Harp, Prevent Rat, Green Phantom King,
   Gorgon Egg, Wall Shadow, Blocker, Golgoil, 30,000-Year White Turtle,
   Queen Bird, Yado Karu, Boulder Tortoise.
   **[inferred]** "Switched to defence" is the reading of
   `PlayFieldCard slot, 0, 1, 0`, because `[11] = 1` routes the caller to its
   position branch.
4. **Attacks, `0x0AD0`.** These are skipped if the AI is pinned by Swords.
   1. **Lethal check.** The AI's strongest unused ATK minus the player's
      weakest attack-position ATK is compared with the player's LP. If it is
      enough, the AI attacks that monster.
   2. **Attack-position targets.** It takes the player's strongest unused
      attack-position monster. `FindKiller` finds the AI's **weakest**
      monster that still beats it, guardian-star bonus included. If one
      exists, it attacks; if not, the target is marked done with `MoveCard`
      and the next strongest is tried.
   3. **Defence-position targets**, the same way against DEF.
   4. **Face-down monsters.** For each one, the AI attacks with its strongest
      monster with probability `values[8]`. This value is 0 for Pegasus,
      Heishin 2nd, Seto 3rd, DarkNite and Nitemare, so they never attack
      blind. They do not need to: with r14 = 0, face-down cards were already
      included, with their real stats, in the searches above.
   5. **Direct attack** with the strongest monster when the player has no
      monsters (category 255).
5. **Leftover monsters, `0x0C7F`.** Every other unused AI monster is
   switched to defence, except these eight, which stay as they are:
   Blue-Eyes Ultimate Dragon, Gate Guardian, Perfectly Ultimate Great Moth,
   Meteor B. Dragon, B. Skull Dragon, Black Luster Soldier, Blue-Eyes White
   Dragon, Metalzoa.
6. **End.** `EndField` runs when no unused AI monster is left.

## 6. Per-duelist behaviour

**[script]** Only three things in the scripts depend on the duelist id:

* r14: whether the duelist sees face-down cards;
* the home-terrain rule (hand step 3);
* the two strategy tables of hand step 6.

Everything else comes from the duelist's row of `gDuel_aOpponentData`
(**[code]** layout, **[script]** use):

| Byte | Read by | Meaning |
|---:|---|---|
| 0 | `Ai_GetHandSize` | size of the "hand window": the hand plus the next deck cards |
| 1 | LoadOpponentData field 0 | ×100: the LP threshold (hand step 6, field burn and heal rule) |
| 2 | field 1 | deck-size threshold under which fusion depth drops |
| 3 | field 2 | `FindBestCombo` depth operand; the search takes up to this + 1 cards |
| 4 | field 3 | `EvaluateFusion` limit; the script passes this − 1 as the depth operand |
| 5, 6 | - | not read by either script |
| 7 | field 6 | Harpie's Feather Duster chance and the burn/heal hold chance |
| 8 | field 7 | chance to attack a face-down monster blind |

The strategy splits below are the combined results of the random rolls. The
tool prints them per duelist. A = best fusion, B = power up the weakest
field monster, C = magic/trap/equip first.

| Id | Duelist | Sees face-down | Row of `gDuel_aOpponentData` | A/B/C when ahead | A/B/C with a weak monster out |
|---:|---|:---:|---|---|---|
| 1 | Simon Muran | no | 5,20,10,1,1,0,0,25,50 | 40/30/30 | 30/30/40 |
| 2 | Teana | no | 5,30,20,1,2,0,0,25,25 | 30/60/10 | 20/60/20 |
| 3 | Jono | no | 5,10,5,2,1,0,0,25,75 | 70/15/15 | 70/15/15 |
| 4 | Villager 1 | no | 5,20,10,2,2,1,1,25,50 | 15/15/70 | 20/20/60 |
| 5 | Villager 2 | no | 5,30,20,3,1,1,1,25,75 | 70/15/15 | 30/10/60 |
| 6 | Villager 3 | no | 5,10,5,1,3,1,1,25,25 | 30/60/10 | 40/30/30 |
| 7 | Seto | no | 10,20,10,3,3,2,2,50,50 | 70/15/15 | 30/40/30 |
| 8 | Heishin | **yes** | 20,20,10,2,2,3,3,75,50 | 30/40/30 | 20/60/20 |
| 9 | Rex Raptor | no | 8,10,5,3,2,2,2,25,75 | 70/15/15 | 70/15/15 |
| 10 | Weevil Underwood | no | 8,10,5,2,3,2,2,25,50 | 30/60/10 | 30/30/40 |
| 11 | Mai Valentine | no | 10,20,10,3,3,2,2,75,25 | 15/15/70 | 20/60/20 |
| 12 | Bandit Keith | no | 12,20,10,2,2,2,2,50,50 | 70/15/15 | 40/30/30 |
| 13 | Shadi | no | 12,30,20,2,2,2,2,50,25 | 30/30/40 | 20/60/20 |
| 14 | Yami Bakura | no | 14,20,10,3,3,3,3,75,25 | 15/15/70 | 30/40/30 |
| 15 | Pegasus | **yes** | 16,20,10,3,3,3,3,75,0 | 30/30/40 | 20/20/60 |
| 16 | Isis | no | 16,20,10,3,3,3,3,75,25 | 30/40/30 | 10/60/30 |
| 17 | Kaiba | no | 16,20,10,3,3,3,3,75,50 | 70/15/15 | 30/60/10 |
| 18 | Mage Soldier | no | 12,10,5,1,1,1,1,25,75 | 40/30/30 | 30/30/40 |
| 19 | Jono 2nd | no | 10,10,5,1,2,1,1,25,75 | 30/60/10 | 20/60/20 |
| 20 | Teana 2nd | no | 10,30,20,2,1,1,1,25,25 | 70/15/15 | 70/15/15 |
| 21 | Ocean Mage | no | 14,10,5,2,2,2,2,50,50 | 30/40/30 | 40/30/30 |
| 22 | High Mage Secmeton | no | 16,20,10,2,3,3,3,50,25 | 30/60/10 | 20/60/20 |
| 23 | Forest Mage | no | 14,10,5,2,2,2,2,50,50 | 15/15/70 | 30/40/30 |
| 24 | High Mage Anubisius | no | 16,20,10,3,3,3,3,50,25 | 30/30/40 | 20/60/20 |
| 25 | Mountain Mage | no | 14,10,5,3,2,2,2,50,50 | 70/15/15 | 30/10/60 |
| 26 | High Mage Atenza | no | 16,20,10,3,3,3,3,50,25 | 15/15/70 | 10/60/30 |
| 27 | Desert Mage | no | 14,10,5,2,2,2,2,50,50 | 15/15/70 | 30/30/40 |
| 28 | High Mage Martis | no | 16,20,10,2,3,3,3,50,25 | 30/60/10 | 20/60/20 |
| 29 | Meadow Mage | no | 14,10,5,3,2,2,2,50,50 | 70/15/15 | 40/30/30 |
| 30 | High Mage Kepura | no | 16,20,10,3,3,3,3,50,25 | 40/30/30 | 20/60/20 |
| 31 | Labyrinth Mage | no | 16,20,10,3,3,3,3,75,75 | 70/15/15 | 30/10/60 |
| 32 | Seto 2nd | no | 18,10,5,1,2,3,3,50,25 | 30/60/10 | 20/60/20 |
| 33 | Guardian Sebek | no | 20,10,5,2,2,3,3,50,50 | 15/15/70 | 20/20/60 |
| 34 | Guardian Neku | no | 20,30,20,2,1,3,3,50,75 | 70/15/15 | 70/15/15 |
| 35 | Heishin 2nd | **yes** | 20,20,10,2,2,3,3,75,0 | 30/40/30 | 30/40/30 |
| 36 | Seto 3rd | **yes** | 20,20,10,2,2,4,4,75,0 | 30/30/40 | 20/60/20 |
| 37 | DarkNite | **yes** | 20,20,10,2,2,5,5,50,0 | 15/15/70 | 20/20/60 |
| 38 | Nitemare | **yes** | 20,10,5,2,2,5,5,75,0 | 70/15/15 | 30/60/10 |
| 39 | Duel Master K | no | 15,20,10,3,3,5,5,50,25 | 30/40/30 | 20/60/20 |

Notable points:

* **Seeing face-down cards: [script].** Pegasus is not alone. Heishin,
  Heishin 2nd, Seto 3rd, DarkNite and Nitemare also run with r14 = 0. For
  these six:
  * `FindStrongest`/`FindWeakest` over the player's monsters include
    face-down cards with their real ATK/DEF;
  * `FindCard` in player zones matches face-down cards;
  * the attack logic picks targets and killers against the real stats.

  The other 33 duelists ignore face-down player cards in those searches.
  They see a face-down monster only through the blind-attack roll
  (`values[8]`).
* **Terrain restoring: [script].** The Mages at the shrine gates (21, 23,
  25, 27, 29) and Guardian Neku restore their home terrain. The High Mages
  do not have that rule (hand step 3).
* **Fusion strength scales with the table, not the script: [code] +
  [script].**
  * `values[3]` ranges from 1 to 3 across the duelists (`FindBestCombo`
    searches up to 2-4 materials).
  * The hand window is 5 cards for the first six duelists and up to 20 for
    the late game.
  * **[code]** The window starts with the real hand and continues into the
    AI's own deck in draw order (`func_80027DF8`). A window of 20 therefore
    lets `FindBestCombo`, `FindStrongest` and the other hand searches look
    at the next 15 cards of the deck.
  * **[inferred], not verified:** whether a card chosen from beyond the real
    hand can then actually be played. `DuelScene_UpdateHandActions` moves
    its hand cursor to `slot - 11` without a bounds check. The effect of an
    index of 5 or more in that path has not been traced.
* **The AI never picks its second guardian star: [script].** Every
  `LoadGuardianStarChoice` operand is the constant 0.
* **Attacks are economical: [code] + [script].** `FindKiller` returns the
  *weakest* monster that still wins, so the AI keeps its big monster for the
  next target. The attack order is: lethal, then the strongest
  attack-position target, then the strongest defence-position target, then
  blind attacks, then a direct attack.
* **The AI's burn cards in hand are finishers only: [script].** It plays
  them from the hand only when the burn is lethal and the player's
  magic/trap row is empty.

## 7. Verification

These numbers are produced by `tools/project/ai_script_disasm.py`, which writes
them to `tmp/ai-scripts/verify.txt`.

| Check | hand | field |
|---|---|---|
| Distinct chunks over the 7 terrain records | 1 | 1 |
| Instructions (recursive descent from 0) | 1314 | 795 |
| Unknown opcodes, overlaps, out-of-chunk targets | 0 | 0 |
| Branch/call operands (distinct targets) | 547 (168) | 239 (57) |
| Targets not on an instruction start | 0 | 0 |
| Linear sweep agrees with the descent | yes | yes |
| Code bytes + unreached bytes | 5273 + 871 = 6144 | 3336 + 2808 = 6144 |
| Last instruction | `EndHand` @ `0x1498` | `EndField` @ `0x0D07` |
| Register operand >= 20, stack overflow or underflow | 0 | 0 |
| Instructions no duelist 1-39 can reach | 0 | 0 |

The tool's opcode table covers all 67 handlers. Every length was read from
the handler's `AiScript_ReadByte`/`AiScript_ReadShort` calls; none was
inferred. The constant-propagation pass resolves a branch only when all of
its operands are known constants. A per-duelist listing is therefore a
superset of what that duelist can execute, never a subset.

**Not verified dynamically.** No trace of the VM's program counter against a
running game was taken for this note. That check is optional: log
`gAiScript_State.previous_cursor - script_base` in `AiScript_Run` under a
trace switch in any build that runs the retail code, then confirm that
every logged offset is an instruction start in the listing. It was not done
here. The claims marked [inferred] above are the ones that such a trace, or
a closer reading of the two scene-state callers, would settle.
