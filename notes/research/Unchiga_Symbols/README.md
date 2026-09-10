# Unchiga_Symbols — named-symbol research for SLUS_014.11

**External-reference provenance.** This folder began as a mirror of
`notes/research/` from Unchiga/ygofm-decomp, the matching decompilation of
Yu-Gi-Oh! Forbidden Memories (SLUS-01411, USA) in which this research was
done. Memories-decomp has since reconciled names, appended local trace
evidence and naming-gate descriptions, and corrected claims against its own
matching source. Paths, tools and rules mentioned from the source project
(`config/symbol_addrs.txt`, `tools/gen_symbols.py`, `tools/setup.sh`,
`config/modules/`, `tools/gen_research_notes.py`, ...) still belong to that
project, not to memories-decomp. Nothing here is house policy for this repo.

**Names.** Since 2026-09-03 every project-assigned name in the corpus follows
this repository's `notes/naming-conventions.md`: `<Subsystem>_<Verb><Object>`
for functions, `g<Subsystem>_<type><Role>` for globals, `SD_*`/`g_SDValue` and
Psy-Q names untouched. Where `notes/semantic-symbol-map.csv` already names an
address, the corpus uses that spelling verbatim. Mechanical behaviour labels
(snake_case, listed apart as "Mechanical names" in the rosters) are
descriptions of verified mechanics for functions whose game role is still
unknown — by this repo's confidence policy they stay address-only here and are
not proposed as names. Names still go through the usual per-address review
before entering
`config/slus_01411/symbols.txt` or `notes/semantic-symbol-map.csv`.

**Verification.** Applied resident renames are name-blind and retain the
byte-identical executable — SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`
(the hash this repo's `make match` gates on), SHA1
`84747e64f6da8e764206ec203e489acf8c9dcf7d`, MD5
`dab1b3c9a6b8a56558b5ca8f807339c3`. Names marked live-traced were also
verified against the running game. Candidate, HOLD and PENDING rows preserve
evidence but are not claims that a symbol has been applied.

## Files

| file | why it exists |
|---|---|
| `symbols_guide.md` | The headline document: confirmed symbols organized by where in the game they run, in plain English. |
| `known_functions.md` | The function roster with addresses and an evidence-source tag for each proposed or mechanical name. |
| `known_variables.md` | The variable and data-symbol roster with the same evidence treatment. |
| `descriptions.md` | The naming gate: concise descriptions grounded in matched code or live observation. |
| `findings.md` | The chronological live/static evidence ledger recording what was proven, how, and the status of each claim. |
| `suspects.md` | The not-yet-100% identifications, kept apart from the confirmed set with the evidence gathered and what would graduate each. |
| `modules.md` | What the disc-loaded screen modules are and how their shared address range was traced. |
| `modules/` | Per-module symbol files for free duel, name entry, password, main menu and overworld, kept separate because those screens load into the same addresses. |
| `NAMING.md` | Where each kind of name comes from in the source repo, how much to trust each evidence tier, and the convention used in this corpus. |

Source repository: Unchiga/ygofm-decomp (private); contact Unchiga for
access or questions. Corrections and counter-evidence welcome.

**2026-09-04 delta.** Four names from the recomp's Free Duel and card-viewer
work (`Main_InitFreeDuelMenu` 0x8003B9BC, module-resident `FreeDuel_Init`
0x8016824C, `gDuel_wViewerCardID` 0x8009B246, `gLibrary_aCardArtRecord`
0x801DC000), findings F121-F128 (free-duel screen init and portrait VRAM layout,
the per-card 7-sector art record at LBA 10817+7*id with its LoadImage rects,
the card-title rasterisation facts, mode-byte values), and one medium-confidence
proposal held back (`gMain_apLoadArena`, the load-arena pointer table at
0x80010000). Rebuild after the re-split is byte-identical.

**2026-09-04 delta (second).** Six names, all byte-verified in the source repo
(commits `a6677c8`, `4d03619`, `2e9bbf6`): `gDuel_abCardLevelAttr` 0x801D5332,
`gText_abColorSlots` 0x801D5708, `gCredits_awSecretNumbers` 0x80090B50,
`gFreeDuel_aDuelistRecords` 0x801D071C, `gFile_szSuMrgPath` 0x800117C8 and
`gFile_szModelMrgPath` 0x80011580; plus one correction, `select_sound_preset`
0x800171A8 → `Duel_LoadPackageStage`, and two module-resident names in
`modules/name_entry.txt`.

Findings F129–F157 carry the evidence. **F136 corrects F125**, which shipped in
the previous delta: the 1408 bytes at art-record +10976 are a 40x32 8bpp
thumbnail plus its own 64-entry CLUT, not a 16x88 strip. F154 originally recorded a name this corpus deliberately did not propose:
`select_reverb_preset` at `0x8005B64C` was demonstrably not a sound routine,
but the right replacement was not yet settled. Subsequent matching-source,
phase-stepper, retail-archive, and overlay-consumer analysis resolved it as
`MainMenu_LoadPackageStage`; the finding row now records that applied result.

The evidence for this delta came from cross-reading a third-party modding
toolset (basededatostea.xyz, 45 browser tools that patch SLUS_014.11 and
WA_MRG.MRG) against this decomp: where a tool pokes a byte, the game's own code
around that byte says what the byte is for. Every address, layout and count
below was then re-derived from the retail US disc and `SLUS_014.11` directly,
not taken from the tools' claims — several of the tools are demonstrably wrong
about their own offsets. Rebuild after the re-split is byte-identical.
