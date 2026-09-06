# Semantic Naming Pass

Initial pass date: 2026-09-01

## Result

The initial evidence-backed naming pass applied:

| Kind | Applied names |
|---|---:|
| Functions | 136 |
| Globals | 53 |
| **Total** | **189** |

These counts are the preserved result of that dated pass, not current totals.
Later semantic work continues to extend the machine-readable
`notes/semantic-symbol-map.csv`, which is the authority for the current
accepted set. Each row records the address, accepted name, confidence,
evidence, and caveat. `tools/project/apply_semantic_names.py` applies and
checks the registry across:

- `config/slus_01411/functions.csv`;
- `config/slus_01411/matching_c.json`;
- `config/slus_01411/symbols.txt`;
- tracked resident C declarations, definitions, and call sites;
- semantic source filenames for matching C functions.

Overlay sources remain excluded because each module has an independent linker
namespace, and the current overlay build configs do not publish resident
semantic aliases.

Confirmed Psy-Q CRT/SDK function names may also use the registry. Lower
confidence non-game function mappings remain rejected.

These are semantic project names, not claims that the original Forbidden
Memories symbols have been recovered.

## Evidence coverage

All nonambiguous descriptive function labels in `tmp/references/gms.c` were
reviewed against the current inventory and translated into the DOTR-style
subsystem form documented in `notes/naming-conventions.md`.

Major function families documented by the initial pass:

| Prefix | Count | Scope |
|---|---:|---|
| `AiScript_` | 65 | AI bytecode readers, dispatcher, commands, searches, and actions |
| `Main_` | 18 | initialization, top-level loop, menus, campaign, duel, and credits states |
| `Duel_` | 15 | card statistics, fusion, ritual, guardian stars, rank, drops, and deck work |
| `Ai_` | 7 | AI range/set helpers and fusion completion |
| `File_` | 3 | disc position and existence helpers |
| `SD_` | 6 | sound state, output mode, effect playback, initialization, and termination |
| `Library_` | 2 | card-used/owned library state |
| `Util_` | 2 | behavior-established generic helpers |
| Other | 4 | `BuildDeck_`, `Text_`, `Rand_`, and `Model_` |

The global pass applies all sufficiently specific entries from
`tmp/references/ram_map.txt`, corroborated GMS data labels, and key AI/sound
state:

- exact cross-game sound root: `g_SDValue`;
- player/opponent life points, deck state, terrain, opponent ID, card drops,
  fusion data, rank data, starchips, and menu state;
- AI script state, command table, operand memory, best-action results, active
  cards, and packed card statistics.

The model loader at `0x80056504` is named `Model_LoadMonsterMerge`, using the
local `MODEL.MRG`/`SU.MRG` behavior and DOTR's `LoadMerge` terminology.

## Unchiga corroboration pass

Unchiga's live traces, static recompilation, symbol catalogue, and matching
sources were checked against local code before accepting another 19 names:

- `SD_SEPlay`, `SD_SEPlayFull`, and `SD_SetOutputType` are directly supported
  by reversible menu traces and local call/body behavior.
- `SD_Init` and `SD_Term` are the locally matched initialization and mirrored
  cleanup chains.
- `0x80046768` is named `SD_InitState`, not the proposed `SD_LoadData`.
  Its local body initializes `g_SDValue`, work buffers, callbacks, and default
  fields but does not itself read `SD_BGM.DAT`, `SD_SE.DAT`, or `MASTER.XA`.
- `gSD_bOutputType` records the signed-byte `-1` initialization sentinel and
  the live-confirmed stereo/mono values `0` and `1`.
- File-name/LBA tables, card-grid and Free Duel cursor pairs, password digits,
  terrain, ritual, rank-score, equip, and card-name sort-key data received
  subsystem names only where local data shape or live behavior corroborated
  the imported label.

The Unchiga SDK catalogue was not bulk-imported: it relies partly on a Psy-Q
4.7 identification that conflicts with this project's independently
established Psy-Q 4.6 toolchain, and Psy-Q functions remain outside the game
decompilation scope.

The recomp's independently instrumented reward path confirms
`Duel_AwardCard` at `0x80021894`: it saturates the awarded card's chest count
and updates the sixteen-entry recent-award history.

## Remaining-campaign naming

After all canonical and inline-refinement histories became terminal, thirteen
newly matched functions received mechanical, evidence-backed names:

- `Duel_UpdateCardPickCursor`;
- `Duel_ApplyCardObjectFlags`;
- six `DuelEffect_` table/entry maintenance helpers;
- `Model_CopySlotU16Values`;
- `Model_InitLightTriplet`;
- `Model_RegisterHandlerKey`;
- `Model_FindHandlerKey`;
- `Model_GetPrimitiveHandler`.

Larger render/state functions whose purpose remains ambiguous were deliberately
left address-based.

## Deliberately deferred labels

Eight GMS labels remain address-based because the available label is explicitly
uncertain, personal, or describes only an unknown flag:

| Address | GMS label | Reason |
|---|---|---|
| `0x800218F0` | `u_maybe_endOfDuel` | The label itself is uncertain and the large routine needs caller/state analysis. |
| `0x8002D7C4` | `hirataLoop` | Personal/opaque label with no stable behavior name. |
| `0x80071510` | `ai_sub_80071510` | Loads an unidentified AI flag. |
| `0x80073448` | `ai_sub_80073448` | Sets the same unidentified AI flag. |
| `0x80073458` | `ai_sub_80073458` | Clears the same unidentified AI flag. |
| `0x80073464` | `aiInstUnkOn` | Sets a second unidentified AI flag. |
| `0x80073474` | `aiInstUnkOff` | Clears a second unidentified AI flag. |
| `0x8007368C` | `ai_sub_8007368C` | Copies an operand into an unidentified AI output byte. |

These should be named only after the consuming state or flag semantics are
established.

## Validation

The project inventory regenerates without losing any accepted names:

```sh
make classify-functions
tools/environments/python/bin/python \
  tools/project/apply_semantic_names.py --check
```

The final clean executable rebuild remains byte-identical:

```text
SHA-256 84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88
```
