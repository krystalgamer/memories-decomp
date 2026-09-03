# Per-module symbols

Screens that run from a disc-loaded module share one address range
(0x80160000..0x8017B000 for free duel / name entry / password, 0x80180000..
for the main menu), so an address inside a module means something different
on every screen. `config/symbol_addrs.txt` names an address ONCE and feeds the
byte-verified split, so it may only carry what the EXE itself references:
each screen loop's fixed-address entry call (`freeDuelModuleEntry`,
`func_801683EC`, ...) stays there; everything else module-resident lives in
these files, one per module, same line syntax as symbol_addrs.txt:

    name = 0xADDR;              // data
    name = 0xADDR; // type:func // code

Tooling: `python3 tools/module_symbols.py <module> [addr]` lists or resolves;
`--check` flags a module symbol whose address is also named in symbol_addrs.txt
(other than an entry). `tools/gen_research_notes.py` appends these to the
known_* rosters under "Module symbols". `captures/lib/probe.py` overlays them
on live traces when `YGOFM_MODULE=<module>` is set. Evidence rows: findings.md.
