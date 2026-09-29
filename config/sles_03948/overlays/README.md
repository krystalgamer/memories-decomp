# French overlay matches

Run `MAKEFLAGS=-j4 make french-match-overlays` from the repository root.
Supply the legally obtained French `SU.MRG` and `WA_MRG.MRG` archives in
`game/france/DATA/`. The extraction gate verifies both archive hashes and each
module hash from `../overlays.json` before building. CI obtains the archives
from `YGOFM_FRA_SU_MRG_URL` and `YGOFM_FRA_WA_MRG_URL`.

The original six modules reuse the existing European C sources and named
GCC 2.8.1/MASPSX 2.81 profiles unchanged. No French C copies, inline assembly,
or source-local external declarations are added for those six modules.
The additional duel-effect bank uses shared `src/overlays/duel_effects/`
C utilities and the existing `gcc_2_8_1_g0_split` profile.

| Module | Archive | First sector | Sectors | Matching functions | C bytes |
|---|---|---:|---:|---:|---:|
| `free_duel` | `WA_MRG.MRG` | 9304 | 5 | 9 | 4252 |
| `main_menu` | `SU.MRG` | 98 | 16 | 31 | 18280 |
| `overworld_after_coup` | `WA_MRG.MRG` | 9920 | 6 | 15 | 6184 |
| `overworld_before_coup` | `WA_MRG.MRG` | 9762 | 6 | 15 | 6184 |
| `password_a` | `WA_MRG.MRG` | 9374 | 15 | 27 | 10476 |
| `password_b` | `WA_MRG.MRG` | 9460 | 15 | 27 | 10476 |
| `duel_effects` | `WA_MRG.MRG` | 7193 | 44 | 78 | 54824 |
| Configured images | | | 107 | 202 | 110676 |

Sector sizes are 2048 bytes. The duel-effect bank loads at `0x80146000` and
has seven identical copies at sectors `7193 + terrain * 240`. Its manifest
lists the other six locations in `duplicate_sector_offsets`; extraction and
verification compare every complete copy with the primary before accepting
the input. The table counts this shared image once, not seven times.
Main-menu code loads at `0x80180000`; the other five images
load at `0x80168000`. Each complete module, including its untranslated raw
data and preserved assembly, reproduces its French retail input exactly.

**Configured images are not exhaustive runtime coverage.** The duel bank
still has 7 provisional unmatched function boundaries. The boot module,
MODEL/SU dynamic loads, and the overworld fragment need further coverage and
ownership analysis. See [duel-effect bank evidence](../../../notes/overlays/duel-effect-bank.md).
The [geometry and rendering batch](../../../notes/overlays/duel-effect-geometry.md)
records the independent French proofs and the subsequently recovered height ring.
The [dispatcher evidence](../../../notes/overlays/duel-effect-dispatch.md)
describes unchanged Spanish-source reuse and six exact-size generated-data owners.
The [effect 0/6 lifecycle evidence](../../../notes/overlays/duel-effect-french-lifecycles.md)
records two complete unchanged Spanish effects and four real generated-data owners.
The [four-effect reuse batch](../../../notes/overlays/duel-effect-french-reuse.md)
records the measured PAL geometry, unchanged Spanish effects 18/19, and
preservation of the North American/Japanese defaults.
The [effect-twenty-three recovery](../../../notes/overlays/duel-effect-twentythree.md)
records the canonical card-row sweep and empty-row completion path.
The [effect-thirteen recovery](../../../notes/overlays/duel-effect-thirteen.md)
records the full 48-path lifecycle and caller-backed signed-number contract.
The [effect-seven recovery](../../../notes/overlays/duel-effect-seven.md)
records the complete trail, particle-burst and bouncing-number lifecycle.
The [effect-10 lifecycle](../../../notes/overlays/duel-effect-ten.md) records
six variants, the separate image table, cycling columns and phase-triggered dissolve.
The [effect-5 lifecycle](../../../notes/overlays/duel-effect-five.md) records
all ten rising-particle/number variants and their distinct completion paths.
The [effect-4 lifecycle](../../../notes/overlays/duel-effect-four.md) records
independent recovery of both card-transition variants and their particle fade.
The [effects 15/1 batch](../../../notes/overlays/duel-effect-french-fifteen-one.md)
records canonical card-object access, five real data owners and the two
independently verified complete lifecycle routines.
The [effect 8/12 batch](../../../notes/overlays/duel-effect-french-eight-twelve.md)
records the independent French image proof and caller-supported halfword contracts.
The [effect 2/21 batch](../../../notes/overlays/duel-effect-french-two-twentyone.md)
records the independent full-bank proof, seven-record configuration table,
and canonical resident ownership of the two effect slots.
The [effect-9 French proof](../../../notes/overlays/duel-effect-9.md#independent-french-registration)
records unchanged accepted-source reuse, signed number paths and two real data owners.

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
