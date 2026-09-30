# French overlay matches

Run `MAKEFLAGS=-j4 make french-match-overlays` from the repository root.
Supply the legally obtained French `SU.MRG`, `WA_MRG.MRG` and `MODEL.MRG`
archives in `game/france/DATA/`. The extraction gate verifies all archive
hashes and each module hash from `../overlays.json` before building. CI stages
MODEL from the expanded regional bundle; SU and WA also have the configured
`YGOFM_FRA_SU_MRG_URL` and `YGOFM_FRA_WA_MRG_URL` inputs.

The original six modules reuse the existing European C sources and named
GCC 2.8.1/MASPSX 2.81 profiles unchanged. No French C copies, inline assembly,
or source-local external declarations are added for those six modules.
The additional duel-effect bank uses shared `src/overlays/duel_effects/`
C utilities and the existing `gcc_2_8_1_g0_split` profile.
The MODEL/SU intro reuses the accepted `spanish_model_intro/runtime.c`
translation unit unchanged, with independently verified French bindings.
Twenty MODEL primary images reuse the accepted `spanish_model_primary/`
copy, effect and particle bodies and slot wrappers unchanged.
Two model-54 secondary-variant images reuse the accepted
`spanish_model_variant/` entry, mesh and ring bodies unchanged.
Two model-401 secondary-variant images reuse the accepted
`spanish_model_variant/variant432_{draw,layers,bands}.c` helpers and slot
wrappers unchanged; the other four function instances retain generated assembly.
Twenty-six header-435/585 images reuse the accepted North American
`model_variant/variant418_{sheet,bands,spokes,rings,quad}.c` bodies through ten French
symbol-renaming wrappers, using the existing GCC 2.8.1 profile. Sheet is
entry-reachable; the other four are retained module-local helpers, not
proven direct-entry execution paths.
Twenty-four header-414/564 images similarly reuse
`model_variant/variant397_{sheets,spokes,rings,quad}.c` through eight French
renaming wrappers under GCC 2.8.1. The sheets helper is direct-entry reachable;
the other three are retained module-local code.
Twelve header-421/571 images reuse
`model_variant/variant404_{sheets,spokes,rings,quad}.c` through eight French
renaming wrappers under the same GCC 2.8.1 profile. Sheets is entry-reachable;
spokes, rings and quad are retained module-local code.
Six header-337/487 images directly reuse the accepted
`spanish_model_variant/variant337_rings.c` body and existing slot-one wrapper,
without new French source files. The ring helper is entry-reachable and uses
the existing GCC 2.8.1 profile.
Twelve header-445/595 images reuse
`model_variant/variant428_{sheets,spokes,rings,quad}.c` through eight French
renaming wrappers under GCC 2.8.1. Sheets is entry-reachable; the other three
helpers are retained module-local code.
Two special Exodia/SU images contain four independently recovered helpers
under `model_exodia/`, with three other functions retained as generated assembly.
Two representative return-two primary images reuse the accepted
`spanish_model_primary/return_two.c` body and slot wrapper unchanged.

| Module | Archive | First sector | Sectors | Matching functions | C bytes |
|---|---|---:|---:|---:|---:|
| `free_duel` | `WA_MRG.MRG` | 9304 | 5 | 9 | 4252 |
| `main_menu` | `SU.MRG` | 98 | 16 | 31 | 18280 |
| `overworld_after_coup` | `WA_MRG.MRG` | 9920 | 6 | 15 | 6184 |
| `overworld_before_coup` | `WA_MRG.MRG` | 9762 | 6 | 15 | 6184 |
| `password_a` | `WA_MRG.MRG` | 9374 | 15 | 27 | 10476 |
| `password_b` | `WA_MRG.MRG` | 9460 | 15 | 27 | 10476 |
| `duel_effects` | `WA_MRG.MRG` | 7193 | 44 | 85 | 81804 |
| `model_intro` | `SU.MRG` | 1767 | 16 | 5 | 1484 |
| `model_primary_*` (20 images) | `MODEL.MRG` | `record * 276 + 220/222` | 40 | 20 | 16432 |
| `model_variant_54_stage9/10_slot0/1` (2 images) | `MODEL.MRG` | `15104/15114` | 20 | 6 | 10400 |
| `model_variant_401_stage9/10_slot0/1` (2 images) | `MODEL.MRG` | `97076/97086` | 20 | 6 | 5928 |
| `exodia_slot0` | `SU.MRG` | 1686 | 10 | 2 | 3916 |
| `exodia_slot1` | `SU.MRG` | 1696 | 10 | 2 | 2588 |
| `model_return_two_slot0/1` (2 images) | `MODEL.MRG` | `220/222` | 4 | 2 | 16 |
| Header-435/585 variants (26 images) | `MODEL.MRG` | `record * 276 + 180/190/200/210` | 260 | 130 | 137176 |
| Header-414/564 variants (24 images) | `MODEL.MRG` | `record * 276 + 180/190/200/210` | 240 | 96 | 90912 |
| Header-421/571 variants (12 images) | `MODEL.MRG` | `record * 276 + 180/190/200/210` | 120 | 48 | 45552 |
| Header-337/487 variants (6 images) | `MODEL.MRG` | `record * 276 + 180/190/200/210` | 60 | 6 | 7296 |
| Header-445/595 variants (12 images) | `MODEL.MRG` | `record * 276 + 180/190/200/210` | 120 | 48 | 45456 |
| Configured images | | | 1027 | 580 | 504812 |

Sector sizes are 2048 bytes. The duel-effect bank loads at `0x80146000` and
has seven identical copies at sectors `7193 + terrain * 240`. Its manifest
lists the other six locations in `duplicate_sector_offsets`; extraction and
verification compare every complete copy with the primary before accepting
the input. The table counts this shared image once, not seven times.
Main-menu and MODEL/SU intro code load separately at `0x80180000`; the original
five WA images load at `0x80168000`. MODEL primary slots load at `0x8013A000`
and `0x8017A000`. Each complete module, including its untranslated raw
data and preserved assembly, reproduces its French retail input exactly.

**Configured images are not exhaustive runtime coverage.** All 85 inventoried
duel-bank functions now have matching C owners, but the boot module,
other MODEL/SU dynamic loads, the intro's 31,116-byte unclassified tail, and
the overworld fragment need further coverage and ownership analysis.
The [French intro proof](../../../notes/overlays/spanish-model-intro.md#independent-french-registration)
records the loader-backed slice, real storage and five recovered routines;
it does not classify that tail as non-code.
The [French MODEL primary proof](../../../notes/overlays/spanish-model-primary.md#independent-french-registration)
records ten models at both slots, the actual descriptor-prefix owners and
the metadata-selected initial commands. Other variant phases,
secondary loads and every preserved tail remain
separate recovery work; matching entry bytes do not establish duplicate images.
The [French model-54 variant proof](../../../notes/overlays/spanish-model-variant408.md#independent-french-registration)
adds only stages 9/10 for model 54, at `0x8013B000`/`0x8017B000`.
Its ten accessed data records have real generated owners, while 15,128
bytes per image remain explicitly unclassified. Other secondary variants
and the separate Exodia/SU handler loads are not covered by this registration.
The separate [Exodia helper registration](../../../notes/overlays/exodia-helpers.md)
covers both special SU images but only four of their seven inventoried functions.
The other three functions retain 7,032 bytes of generated assembly, and the
two tails retain 27,416 explicitly unclassified bytes. These are live handlers
called directly by the dedicated resident controller despite disabled general
MODEL command words. The configured inventory is now 580/889 matching instances,
not an exhaustive runtime-code census.
The [French return-two proof](../../../notes/overlays/spanish-model-return-two.md#independent-french-verification)
independently links all 1,222 returning primary instances across 611 compact
records. Only two representative images contribute configured progress.
Their unequal complete-image suffixes are not duplicate registrations, and
all 4,084 suffix bytes per image remain unclassified.
The [French model-401 proof](../../../notes/overlays/spanish-model-variant432.md#independent-french-registration)
registers the helpers at offsets `0x134C`, `0x169C` and `0x1AC4` in each
second-variant image. All ten direct-call function boundaries are independently
recovered; four functions retain 9,872 bytes of assembly, and each 12,576-byte
suffix stays unclassified. These registrations do not establish other variants'
coverage.
The [French header-435/585 proof](../../../notes/overlays/french-model-variant435.md)
adds 26 archive instances with 23 distinct complete images. Each has nine
measured function boundaries, five matching C helpers, four assembly fallbacks,
and a 5,736-byte unclassified suffix. The direct entry call graph reaches only
the first four functions, including the newly matched one-sheet helper.
The other four C helpers remain retained code, not demonstrated additional
runtime paths. The sheet contribution adds 26 C instances and 24,336 bytes
without adding images or function boundaries.
The padded-band follow-up adds another 26 C instances and 46,904 bytes at
offset `0x28A4`. Fourteen entry anchors and 50 target layouts verify the
single initialized 492-byte record. Three existing resident bindings gain
their verified SDK names without changing any address.
The [French header-414/564 proof](../../../notes/overlays/french-model-variant414.md)
adds 24 archive instances with 22 distinct complete images. Four of seven
functions per image have matching C; three remain assembly. Twelve entry
initialization anchors and 71 target layouts support ownership, while each
8,756-byte tail remains explicitly unclassified.
The [French header-421/571 proof](../../../notes/overlays/french-model-variant421.md)
adds 12 distinct complete images. Four of nine functions per image have
matching C; five remain assembly. Fifteen entry initialization anchors and
71 target layouts support ownership. The first six functions, including
sheets, are entry-reachable, while each 3,988-byte tail remains unclassified.
The [French header-337/487 proof](../../../notes/overlays/french-model-variant337.md)
adds six distinct complete images. One of four entry-reachable functions per
image has matching C; three remain assembly. Fifty-seven target layout
constants and fourteen entry/configuration anchors verify the accessed views,
while every 12,452-byte suffix remains unclassified.
The [French header-445/595 proof](../../../notes/overlays/french-model-variant445.md)
adds twelve distinct complete images, each with four matching C helpers and
four assembly functions. The additional entry-called 2,492-byte function
after the quad helper remains assembly, not tail data. Fifteen entry anchors
and 71 target layouts support ownership; every 6,764-byte suffix remains
unclassified.
See [duel-effect bank evidence](../../../notes/overlays/duel-effect-bank.md).
The [geometry and rendering batch](../../../notes/overlays/duel-effect-geometry.md)
records the independent French proofs and the subsequently recovered height ring.
The [dispatcher evidence](../../../notes/overlays/duel-effect-dispatch.md)
describes unchanged Spanish-source reuse and six exact-size generated-data owners.
The [effect 0/6 lifecycle evidence](../../../notes/overlays/duel-effect-french-lifecycles.md)
records two complete unchanged Spanish effects and four real generated-data owners.
The [four-effect reuse batch](../../../notes/overlays/duel-effect-french-reuse.md)
records the measured PAL geometry, unchanged Spanish effects 18/19, and
preservation of the North American/Japanese defaults.
The [effect-seventeen lifecycle](../../../notes/overlays/duel-effect-seventeen.md)
records the complete vortex, canonical collector output and byte-packed brightness.
The [effect-eleven lifecycle](../../../notes/overlays/duel-effect-eleven.md)
records its 28-piece breakup, shared mode/image storage and helper contracts.
The [effect-24 lifecycle](../../../notes/overlays/duel-effect-twentyfour.md)
records the complete five-object Exodia burst, real data owners and work bounds.
The [ritual-effect recovery](../../../notes/overlays/spanish-duel-effect-22.md)
records unchanged shared C reuse, its real compiler-generated switch table,
the independently verified French registration and preserved particle extents.
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
The [number renderer](../../../notes/overlays/duel-effect-number-renderer.md)
and [effect 16/20 lifecycle](../../../notes/overlays/spanish-duel-effects-16-20.md)
also match independently in French without changing the shared Spanish C.
Their French registrations preserve the overlapping particle windows and
separate scale-vector, configuration and twelve-glyph generated-data owners.
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
