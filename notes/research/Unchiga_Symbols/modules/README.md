# Per-module symbols

> **External reference — mirror of Unchiga/ygofm-decomp.** This file is copied verbatim from that repository's `notes/research/` (the matching decomp the research was done in). Paths, tools and rules it names (`config/symbol_addrs.txt`, `tools/gen_symbols.py`, `tools/setup.sh`, `config/modules/`, `tools/gen_research_notes.py`, ...) are that project's, not this one's. The addresses and evidence are the claim; the names follow this repo's `notes/naming-conventions.md` (`Subsystem_VerbObject` / `gSubsystem_Role`) since 2026-09-03 and are ready for `config/slus_01411/symbols.txt` and `notes/semantic-symbol-map.csv` after the usual per-address review.

Screens that run from a disc-loaded module share one address range
(0x80160000..0x8017B000 for free duel / name entry / password, 0x80180000..
for the main menu), so an address inside a module means something different
on every screen. `config/symbol_addrs.txt` names an address ONCE and feeds the
byte-verified split, so it may only carry what the EXE itself references:
each screen loop's fixed-address entry call (`FreeDuel_Entry`,
`func_801683EC`, ...) stays there; everything else module-resident lives in
these files, one per module, same line syntax as symbol_addrs.txt:

    name = 0xADDR;              // data
    name = 0xADDR; // type:func // code

Tooling: `python3 tools/module_symbols.py <module> [addr]` lists or resolves;
`--check` flags a module symbol whose address is also named in symbol_addrs.txt
(other than an entry). `tools/gen_research_notes.py` appends these to the
known_* rosters under "Module symbols". `captures/lib/probe.py` overlays them
on live traces when `YGOFM_MODULE=<module>` is set. Evidence rows: findings.md.
