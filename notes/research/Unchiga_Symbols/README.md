# Unchiga_Symbols — named-symbol research for SLUS_014.11

**External reference.** This folder mirrors `notes/research/` of
Unchiga/ygofm-decomp, the matching decompilation of Yu-Gi-Oh! Forbidden
Memories (SLUS-01411, USA) in which this research was done. Every file here is
a verbatim copy of that repository's; the paths, tools and rules they mention
(`config/symbol_addrs.txt`, `tools/gen_symbols.py`, `tools/setup.sh`,
`config/modules/`, `tools/gen_research_notes.py`, ...) belong to that project,
not to memories-decomp. Nothing here is house policy for this repo.

**Names.** Since 2026-09-03 every project-assigned name in the corpus follows
this repository's `notes/naming-conventions.md`: `<Subsystem>_<Verb><Object>`
for functions, `g<Subsystem>_<type><Role>` for globals, `SD_*`/`g_SDValue` and
Psy-Q names untouched. Where `notes/semantic-symbol-map.csv` already names an
address, the corpus uses that spelling verbatim (160 shared addresses agree
symbol-for-symbol). Mechanical behaviour labels (snake_case, listed apart as
"Mechanical names" in the rosters) are descriptions of verified mechanics for
functions whose game role is still unknown — by this repo's confidence policy
they stay address-only here and are not proposed as names. Names still go
through the usual per-address review before entering
`config/slus_01411/symbols.txt` or `notes/semantic-symbol-map.csv`.

**Verification.** Every name is 1:1 byte-verified: after each rename batch the
whole executable was re-split and rebuilt, and the rebuild is byte-identical
to the original — SHA-256 `84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`
(the hash this repo's `make match` gates on), SHA1 `84747e64f6da8e764206ec203e489acf8c9dcf7d`,
MD5 `dab1b3c9a6b8a56558b5ca8f807339c3`. Names marked live-traced were also verified
against the running game (function-entry tracing, RAM diffing, injected input
on a recompiled build).

## Files

| file | why it exists |
|---|---|
| `symbols_guide.md` | The headline document: 141 confirmed symbols organized by where in the game they run, in plain English. |
| `known_functions.md` | The complete roster of 1111 named functions with addresses, each tagged with its evidence source. |
| `known_variables.md` | The complete roster of 108 named variables and data symbols, same treatment. |
| `descriptions.md` | The naming gate: 180 verified one-paragraph descriptions, because a name only counts as done once its description matches the matched code or a live observation. |
| `findings.md` | The live-trace evidence ledger (F1–F128) recording what was proven, how, and the status of each claim. |
| `suspects.md` | The not-yet-100% identifications, kept apart from the confirmed set with the evidence gathered and what would graduate each. |
| `modules.md` | What the disc-loaded screen modules are and how their shared address range was traced. |
| `modules/` | Per-module symbol files (41 symbols) for free duel, name entry, password, main menu and overworld, kept separate because those screens load into the same addresses. |
| `NAMING.md` | Where each kind of name comes from in the source repo, how much to trust each evidence tier, and the convention used in this corpus. |

Source repository: Unchiga/ygofm-decomp (private); contact Unchiga for
access or questions. Corrections and counter-evidence welcome.

**2026-09-04 delta.** Four names from the recomp's Free Duel and card-viewer
work (`Main_InitFreeDuelMenu` 0x8003B9BC, module-resident `FreeDuel_Init`
0x8016824C, `gLibrary_wViewerCardID` 0x8009B246, `gLibrary_aCardArtRecord`
0x801DC000), findings F121-F128 (free-duel screen init and portrait VRAM layout,
the per-card 7-sector art record at LBA 10817+7*id with its LoadImage rects,
the card-title rasterisation facts, mode-byte values), and one medium-confidence
proposal held back (`gMain_apLoadArena`, the load-arena pointer table at
0x80010000). Rebuild after the re-split is byte-identical.
