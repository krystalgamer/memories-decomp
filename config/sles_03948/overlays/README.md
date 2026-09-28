# French overlay matches

Run `MAKEFLAGS=-j4 make french-match-overlays` from the repository root.
Supply the legally obtained French `SU.MRG` and `WA_MRG.MRG` archives in
`game/france/DATA/`. The extraction gate verifies both archive hashes and each
module hash from `../overlays.json` before building. CI obtains the archives
from `YGOFM_FRA_SU_MRG_URL` and `YGOFM_FRA_WA_MRG_URL`.

These six modules reuse the existing European C sources and named
GCC 2.8.1/MASPSX 2.81 profiles unchanged. No French C copies, inline assembly,
or source-local external declarations are added.

| Module | Archive | First sector | Sectors | Matching functions | C bytes |
|---|---|---:|---:|---:|---:|
| `free_duel` | `WA_MRG.MRG` | 9304 | 5 | 9 | 4252 |
| `main_menu` | `SU.MRG` | 98 | 16 | 31 | 18280 |
| `overworld_after_coup` | `WA_MRG.MRG` | 9920 | 6 | 15 | 6184 |
| `overworld_before_coup` | `WA_MRG.MRG` | 9762 | 6 | 15 | 6184 |
| `password_a` | `WA_MRG.MRG` | 9374 | 15 | 27 | 10476 |
| `password_b` | `WA_MRG.MRG` | 9460 | 15 | 27 | 10476 |
| Total | | | 63 | 124 | 55852 |

Sector sizes are 2048 bytes. Counts are per module instance, not 124 distinct
new implementations. Main-menu code loads at `0x80180000`; the other modules
load at `0x80168000`. Each complete module, including its untranslated raw
data and preserved assembly, reproduces its French retail input exactly.

## Relocation evidence and experiments

First, all six European modules were rebuilt and verified. Their relocatable
objects and linked ELF symbols identified 3,532 relocation sites and 604
per-module external symbol/address bindings in the French slices. HI16/LO16
pairing was checked against the European linked symbol value and object
addends before deriving the French address; instruction order alone is not
sufficient because relocations and reused high halves can be out of order.
J26 and absolute 32-bit references were decoded using their original addends.
The symbol values must agree across repeated uses and shared variant files.

Unchanged opcode bits were required at instruction relocation sites.
References to object-local symbols retained their layout-derived addresses.
The first full builds matched with explicit bindings; a subsequent reduction
of redundant linker aliases exposed stale European external entries in the
Splat symbol files. Updating those entries to the recovered French addresses,
while retaining only required linker aliases, restored all six exact matches.
No C source or compiler-profile experiment was needed.

The matching inventories were checked against the French linked ELF for
exact function name, address and size. The layouts preserve jump-table
placement, raw data, and the overworld assembly block at `0x1618..0x17D0`.
That block is **not** claimed as matching C. Scratch probes, objects,
extracted modules and retail archives remain untracked.

French overlays are included in the report generator's separate French
section and `french.overlays` JSON object. This overlay-only reporting does
not make claims about French resident completion. Report snapshots are
refreshed separately under #443.
