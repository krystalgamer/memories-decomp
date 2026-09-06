# NAMING.md — where every Unchiga_Symbols name comes from, and how much to trust it

> **External reference — mirror of Unchiga/ygofm-decomp.** This file is copied verbatim from that repository's `notes/research/` (the matching decomp the research was done in). Paths, tools and rules it names (`config/symbol_addrs.txt`, `tools/gen_symbols.py`, `tools/setup.sh`, `config/modules/`, `tools/gen_research_notes.py`, ...) are that project's, not this one's. The addresses and evidence are the claim; the names follow this repo's `notes/naming-conventions.md` (`Subsystem_VerbObject` / `gSubsystem_Role`) since 2026-09-03 and are ready for `config/slus_01411/symbols.txt` and `notes/semantic-symbol-map.csv` after the usual per-address review.

Names in the Unchiga_Symbols corpus are **semantic claims layered on top of a byte-perfect
build**. The oracle (byte-identical rebuild of `SLUS_014.11`, MD5
`dab1b3c9a6b8a56558b5ca8f807339c3`, SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88` — the hash this repo's `make match` gates on) is name-blind: renaming a symbol can never
make a wrong build pass or a right build fail, and every naming change here
was still verified by a full re-split + rebuild + SHA1 (`pass 2:
byte-identical. OK`). What a name CAN do is mislead a reader, so every name
carries an evidence tier, and the tier tells you how hard you may lean on it.

## Where names physically live

`config/symbol_addrs.txt` is **generated — never hand-edit it**. The sources,
in `tools/gen_symbols.py`, resolve per address as
`KNOWN.get(addr) or libsyms.get(addr) or func_ADDR`:

| store | holds | wins over |
|---|---|---|
| `KNOWN` dict (gen_symbols.py) | hand-assigned function names | libsyms |
| `DATA_KNOWN` list (gen_symbols.py) | all named data symbols | — |
| `libsyms/libsyms_470.txt` | PsyQ signature identifications | the `func_` default |
| default | `func_ADDR` / `D_ADDR` | nothing |

`reference/idb_raymond_2018/rename_plan.json` is the authoritative old→new
record of the 2026-08-31 import (173 entries); `tools/gen_vars_peer.py`
replays it whenever the peer header is regenerated. Unit **file** names are a
separate namespace (yaml + `gen_build.py`): `src/func_80021598.c` still
defines `Duel_CalcRankScore` — a file rename pass ("stage B") is pending and
purely cosmetic.

## Evidence tiers, strongest first

### Tier 1 — real SDK symbols (PsyQ library + BIOS)
*Style:* whatever Sony wrote — `SpuSetReverbModeParam`, `_spu_FsetRXX`,
`CdIntToPos`, `EnterCriticalSection`.
*Evidence:* byte-signature matches against the shipped PsyQ 4.7 libraries
(`libsyms/libsyms_470.txt`, each row citing its origin, e.g. `LIBCD.LIB/SYS.OBJ
len=128`), and for `bios_a0_*/bios_b0_*` trampolines the documented PSX BIOS
call tables. These are the **actual original names** — the only tier that is
not an interpretation.
*Trust:* highest, but not infallible — FLIRT-style signature collisions exist
(see Conflicts below).

### Tier 2 — idb_raymond_2018 import (camelCase)
*Style:* `Duel_CalcRankScore`, `AiScript_FindBestCombo`, `Main_RunDuel`, `gDuel_aFusionTable`,
`gAiScript_aMemory`.
*Evidence:* extracted from a third-party IDA 6.8 database (author "Raymond",
2018) whose **recorded input MD5 is byte-identical to our target** — every
address maps 1:1, no cross-game inference. The names are a deep community
reverser's semantic reads (they align with FM wiki / AI-scripting lore), NOT
Konami debug symbols. Adopted **verbatim** — including the camelCase and the
author's own hedges (`Duel_MaybeEndOfDuel`, `Model_LoadMonsterMerge`) — so that
every one of these names is traceable to `reference/idb_raymond_2018/` rather
than silently laundered into house style. 145 renames + 28 new data symbols
applied (commits `0a58f74`, `fd158f6`, and the NEW_ADDR batch), each batch
byte-verified.
*Trust:* high for game-logic meaning; zero authority on what Konami called it.

### Tier 3 — behavior-derived names (snake_case)
*Style:* `upload_tpage_and_clut`, `mask_test_call_800eb26c`,
`backward_word_copy_with_tail_dispatch`, `weighted_random_pick`.
*Evidence:* the matched C body itself — the name describes verified mechanics,
nothing more. Written by this fleet during matching. Deliberately mechanical:
a Tier 3 name never claims game meaning it can't see (which is why Tier 2
displaced 145 of them or their `func_` neighbors — `spawn_fade_teardown_dispatch`
was correct mechanics, `tradeLoop` is what it's *for*).
*Trust:* reliable about WHAT the code does, silent about WHY.

### Tier 4 — placeholders
*Style:* `func_80012B50`, `D_8009B338`.
*Evidence:* none; the address is the name. Any `func_`/`D_` symbol is an open
invitation — check `reference/idb_raymond_2018/idb_crossref.txt` first before
inventing a Tier 3 name for one.

## Modifiers and disambiguators

- **`Name_ADDR` suffix** = a *second linked copy* of a library routine.
  `CdIntToPos_8007E600` / `CdPosToInt_8007E710`: libsyms owns the bare names
  at `0x8007A710`/`0x800781F0`; the binary genuinely links libcd twice.
  (`gen_symbols.py` also auto-suffixes any accidental duplicate this way.)
- **`_a/_b/_c`, `_arr`, `_raw`, `_b` alias families** + `// allow_duplicated:true`
  = multiple typed views of one address (scalar vs array vs raw byte view).
  Splat requires BOTH sides of a shared address to carry the attribute.
- **`u_` / `maybe_` prefixes** = uncertainty markers inherited from the IDB
  author; kept, not cleaned up, because deleting a hedge manufactures
  confidence no one earned.
- **`// absolute:True`** = symbol past the image end (file spans to vram
  `0x801E0800`; `gSD_MusicTrack = 0x801EA800` is runtime arena), so splat cannot
  place it in a segment.
- **`// type:func`** on data-like addresses is itself a claim to audit:
  `func_80100000`/`func_80140000` are marked func in our config but the IDB
  says they are the player/enemy monster **model buffers** — left unrenamed
  pending review.

## Conflicts: 31 rows where two names claim one address

`reference/idb_raymond_2018/idb_crossref.txt` section `CONFLICT`. Policy
(operator ruling 2026-08-31): **leave them; the build stays 1:1 and neither
side gets bulk-applied.** They split into recognizable classes:

- *IDA FLIRT noise (ours wins):* `GsGetWorkBase_0/_1/_2` — one tiny signature
  matching three different functions; the `_ExitCard`/`_ExitCard_0` pair.
- *Probably ours wrong (IDB wins, pending byte-check):* `FntLoad` at
  `0x8007E910` vs our `upload_tpage_and_clut` — uploading the debug-font
  tpage+clut is literally FntLoad's job, and the IDB agrees with us on the
  adjacent `SetDumpFnt`/`FntOpen`/`FntFlush`/`FntPrint` cluster.
- *Genuinely undecided (needs bytes vs real PsyQ libs):* `SpuInit` vs
  `SsUtReverbOff` at `0x80074E60`; `SpuWrite` vs `SpuRead` at `0x80077150`;
  `DecDCTin` vs `DecDCTBufSize` at `0x8008FD38`.
- *Semantic vs mechanical (both true, one address):* `calcStats` vs
  `calc_clamped_pair`, `checkFusion` vs `fusion_table_lookup`,
  `randInterval` vs `call_8e590_mod` — Tier 2 meaning vs Tier 3 mechanics.
  Adjudicating these is a style decision, not an evidence question.

The standard for flipping any conflict row: byte-level comparison against the
real PsyQ library object, or behavior proof from the matched C — never
"the other database says so."

## Rules of engagement (in the source repo)

1. Never hand-edit `config/symbol_addrs.txt`; edit `KNOWN`/`DATA_KNOWN` in
   `tools/gen_symbols.py` and re-run the pipeline.
2. Before naming ANY `func_`/`D_` symbol, grep
   `reference/idb_raymond_2018/idb_crossref.txt` for its address.
3. Every rename batch goes through commit → `tools/setup.sh` → byte-identical
   check. `prebuild_gate` will (correctly) refuse a large uncommitted sweep —
   commit first, never `PREBUILD_GATE=0`.
4. In `INCLUDE_ASM("asm/nonmatchings/<dir>", sym)` the dir is the **unit**
   name and only the `.s` file is the **symbol** name — rename only the
   symbol half, and purge stale old-name `.s` files after a re-split.
5. New names must not collide with any existing symbol, declaration, or
   common identifier; suffix duplicates with `_ADDR`.
6. Preserve inherited hedges; add your own (`maybe_`) when the evidence is
   partial.


## Convention: memories-decomp style (2026-09-03)

Project-assigned names (Tier 2, live-traced, module) follow the convention of
the upstream research repo, krystalgamer/memories-decomp
(`notes/naming-conventions.md` there), so that our symbols can enter its
`config/slus_01411/symbols.txt` and `notes/semantic-symbol-map.csv` unchanged:

- **functions**: `<Subsystem>_<Verb><Object>` — `Duel_SelectCardDrop`,
  `Main_RunFreeDuelMenu`, `TextBox_Create`, `AiScript_JumpRandom`,
  `NameEntry_UpdateScreen`. Preferred verbs: Init/Term/Reset/Clear, Get/Set,
  Calc, Is/Has/Can/Check, Load/Read/Free/Transfer, Play/Stop/Start/Update/Draw,
  Req/Inq/Do, CB (callbacks).
- **globals**: `g<Subsystem>_<type><Role>` — `gDuel_wCardDropID`,
  `gLibrary_abCardChest`, `gFile_apszName`, `gNameEntry_pCursorWidget`. The
  compact type prefix (`b` byte/bool, `w` 16-bit, `dw` 32-bit, `n` count/index,
  `p` pointer, `a` array, combined as `ab`/`aw`/`apfn`...) is kept only when the
  width is established; otherwise the role stands alone (`gCampaignMap_Location`).
- **shared addresses**: where memories-decomp already names an address in its
  semantic map, we use its spelling verbatim (150 of the 215 renames of
  2026-09-03), so the two repos agree symbol-for-symbol.
- **subsystems in use**: Main, File, Duel, Library, BuildDeck, Fade, Dialog,
  TextBox (the 0x64-byte text records at 0x800EB0F8 and their typewriter),
  Text (string banks, glyph codes), Script (the story/dialog stream engine),
  Widget, Campaign, CampaignMap, FreeDuel, NameEntry, Password, CardGrid,
  Model, Rand, Util, Debug, Ai / AiScript, SD.

Exempt, because they are not our names: Tier 1 SDK symbols (`FntPrint`,
`CdIntToPos`, `_spu_FsetRXX`) and the `SD_*` / `g_SDValue` sound-driver
vocabulary shared with Duelists of the Roses
(`notes/research/dotr_symbol_vocabulary.md`). Typed-alias families
(`_a/_b/_arr`, `Name_ADDR` second copies) keep their suffix on the new stem
(`gDialog_bChoice` / `gDialog_bChoice_b`). Uncertainty is carried as a word
in the name and a note in descriptions.md (`Duel_MaybeEndOfDuel`,
`AiScript_SetUnknownFlag`), never manufactured away.

Mechanical Tier 3 names (`poll_call_393b0_until_bit13`,
`compare_rec_two_level_std`, `mask_test_800eb26c_*`) are NOT in this
convention: they describe verified mechanics of a function whose game role is
still unknown, which memories-decomp's confidence policy keeps address-only.
They stay snake_case here as behaviour labels and are listed apart in the
rosters ("Mechanical names"); each is a candidate for a subsystem name once
its role is proven.

History: the 2026-08-31 import used the IDB author's camelCase; 2026-09-03
(morning) unified everything as lowerCamelCase; the same day, after
memories-decomp requested changes on our research PR, the 215 non-mechanical
names were restyled to the convention above (rename_plan.json rows keep the
address's original evidence kind, or `SUBSYSTEM_STYLE` for fleet names).
Every step was byte-verified.

## Module symbols (2026-09-02)

Screen modules load into one shared range (0x80160000.. for free duel, name
entry, password; 0x80180000.. for the main menu), so a module address means
something different per screen. `config/symbol_addrs.txt` names an address
once and feeds the split, so it carries ONLY the EXE-referenced entry calls
(`FreeDuel_Entry`, `NameEntry_Init`, `func_8016A080`, ...). Every
other module function or variable goes in `config/modules/<module>.txt`
(same line syntax); `tools/module_symbols.py --check` refuses a duplicate.
Tiers and rules above apply unchanged; the rosters list them under
"Module symbols".
\n