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
Twenty-eight header-460/610 images reuse
`model_variant/variant443_{sheets,strand}.c` through four French renaming
wrappers under GCC 2.8.1. Sheets is entry-reachable; strand is retained
module-local code with no demonstrated entry execution path.
Four header-422/572 and four header-442/592 images reuse
`model_variant/variant405_{bands,spokes,rings,quad}.c` and
`model_variant/variant425_{bands,spokes,rings,quad}.c`, respectively, through
sixteen French renaming wrappers under GCC 2.8.1. All four helpers in each
family are retained module-local code, not demonstrated entry execution paths.
Two header-418/568 and four header-433/583 images similarly reuse
`model_variant/variant401_{spokes,rings,quad}.c` and
`model_variant/variant416_{spokes,rings,quad}.c` through twelve French
wrappers. All three C helpers per image are retained, not entry-reachable.
Two header-431/581, four header-440/590 and four header-465/615 images reuse
`model_variant/variant414_{spokes,rings}.c`,
`model_variant/variant423_{spokes,rings,quad}.c` and
`model_variant/variant448_{spokes,rings,quad}.c` through sixteen French
renaming wrappers. All helpers are retained, not entry-reachable. Current
G32/PSXLONG declarations and GCC 2.8.1/MASPSX 2.81 profiles are preserved.
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
| Header-460/610 variants (28 images) | `MODEL.MRG` | `record * 276 + 180/190/200/210` | 280 | 56 | 62944 |
| Header-422/572 variants (4 images) | `MODEL.MRG` | `record * 276 + 180/190` | 40 | 16 | 17392 |
| Header-442/592 variants (4 images) | `MODEL.MRG` | `record * 276 + 180/190` | 40 | 16 | 17392 |
| Header-418/568 variants (2 images) | `MODEL.MRG` | `record * 276 + 200/210` | 20 | 6 | 5088 |
| Header-433/583 variants (4 images) | `MODEL.MRG` | `record * 276 + 180/190` | 40 | 12 | 10176 |
| Header-431/581 variants (2 images) | `MODEL.MRG` | `record * 276 + 180/190` | 20 | 4 | 3336 |
| Header-440/590 variants (4 images) | `MODEL.MRG` | `record * 276 + 200/210` | 40 | 12 | 10176 |
| Header-465/615 variants (4 images) | `MODEL.MRG` | `record * 276 + 200/210` | 40 | 12 | 10144 |
| Configured images | | | 1547 | 714 | 641460 |

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
MODEL command words. The configured inventory is now 714/1231 matching instances,
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
The [French header-460/610 proof](../../../notes/overlays/french-model-variant460.md)
adds 28 distinct complete images, each with two matching C helpers and three
assembly functions. Seventeen entry/configuration anchors, 49 target layouts
and all 28 actual command-selected configuration views support the accessed
record bounds; every 10,036-byte suffix remains explicitly unclassified.
The [French header-422/572 proof](../../../notes/overlays/french-model-variant422.md)
and [header-442/592 proof](../../../notes/overlays/french-model-variant442.md)
add eight distinct images with 32 C helper instances. Each family has
95 independently compiled layout constants and 44 entry anchors. Both use
456-byte bands, not header435's 492-byte padded records. Five/six functions
per image remain assembly, and every 6,092/3,024-byte suffix stays unclassified.
The [French header-418/568 proof](../../../notes/overlays/french-model-variant418.md)
and [header-433/583 proof](../../../notes/overlays/french-model-variant433.md)
add six distinct images with 18 C helper instances. Each family has
71 independently compiled layout constants and 43 entry anchors. Five
entry-reachable functions per image remain assembly; all three C helpers
are retained, and every 7,908/8,008-byte suffix stays unclassified.
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

The [French header-431/581 proof](../../../notes/overlays/french-model-variant431.md),
[header-440/590 proof](../../../notes/overlays/french-model-variant440.md) and
[header-465/615 proof](../../../notes/overlays/french-model-variant465.md)
add ten distinct complete images and 28 retained C instances. Each family
has 71 freshly compiled local layout constants; 57/69/77 entry anchors
establish the respective accessed views. Header 465 has one 296-byte
sheet-prefix record, not two standard sheets, and a retained 2,392-byte
assembly function after the quad. Suffixes remain unclassified:
8,376/9,440/3,244 bytes per respective image.

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

Another 510 MODEL variant images are single-entry images whose text is the
North American entry for the US header 17 below their own (French headers 71, 81, 89, 98, 127, 128, 134, 135, 136, 140, 145, 147, 769, 771, 772, 773, 783, 787, 796, 844, 845 and their slot-one
counterparts 201, 211, 219, 228, 257, 258, 264, 265, 266, 270, 275, 277, 866, 868, 869, 870, 880, 884, 893, 941, 942). They
build from the accepted `model_variant/variant{64,72,81,110,111,117,118,130,827,828}_entry`
sources and their `_slot1` copies unchanged. The
`variant{54,119,123,128,752,754,755,756,766,779}_entry` families compile through
`src/overlays/french_model_variant/` wrappers that define `VERSION_FRENCH`
(the US `variant770_entry` wrapper is `variant787_entry`, because
`variant770_entry` already names the French header-770 source). That
selects the French burst CLUT (`0x3D28` instead of `0x3D20`), and in
`variant{54,119,123,128,754}` the 256-line `GRAPHICS_DEFAULT_HEIGHT` sizes the full-screen
flash quad. Each source family has one `model_variant<header>_linker_symbols.txt`
file with the French resident addresses (`variant117_entry` uses
`model_variant134_linker_symbols.txt`, because `model_variant117` already names
the French header-117 family).

A further 182 single-entry images (French headers 181, 188, 189, 193, 644,
651, 664, 671 and the slot-one counterparts 311, 318, 319, 323, 711, 718, 731
and 738) are the North American
`model_variant/variant{164,171,172,176,627,634,647,654}_entry` bodies without
the US-only work-size `printf`. Their `french_model_variant/` wrappers define
`VERSION_FRENCH`, which drops that call (and, in `variant{171,176}`, its
`.rodata` format string); `variant164` also takes the 256-line `GRAPHICS_DEFAULT_HEIGHT`
for its full-screen flash. The removed call shortens the entry, so the wrappers
rename the entry's data references to their earlier French addresses.

Sixty-eight images under French headers 665 and 732 are the North American
header-648 entry (`model_variant/variant648_entry.c`) without its `E_DATA`
work-size printf. They use the same recipe, through
`src/overlays/french_model_variant/variant665_entry{,_slot1}.c`, with the data
labels 0x28 lower.

Four images under French headers 858 and 955 are the North American
header-841 entry (`model_variant/variant841_entry.c`) with the burst palette
from the PAL CLUT row 0x3D28 instead of 0x3D20. The
`src/overlays/french_model_variant/variant858_entry{,_slot1}.c` wrappers set
`VERSION_FRENCH` and bind `model_variant858_linker_symbols.txt`.

Twenty-two images under French headers 663 and 730 are the North American
header-646 entry (`model_variant/variant646_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant663_entry{,_slot1}.c` with the data
labels 0x28 lower.

Six images under French headers 687 and 754 are the North American
header-670 entry (`model_variant/variant670_entry.c`) without its `E_DATA`
work-size printf. Its format string was the whole read-only section, so it
goes too, and the
`src/overlays/french_model_variant/variant687_entry{,_slot1}.c` wrappers move
the data labels 0x28 lower.

Eight images under French headers 126 and 256 are the North American
header-109 entry (`model_variant/variant109_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant126_linker_symbols.txt` binds to the French
addresses.

Twelve images under French headers 649 and 716 are the North American
header-632 entry (`model_variant/variant632_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant649_entry{,_slot1}.c` with the data
labels 0x28 lower.

Three images under French header 429 are the North American header-412
entry (`model_variant/variant412_entry.c`, which the US images build with
the CDK GCC 2.7.2 profile) rebuilt with GCC 2.8.1. The entry is 4 bytes
shorter, its two helpers keep their French C
(`variant429_swarm.c` and `variant429_quads.c`) and the data starts at
0x1D90, and the effect palette loads from PAL x = 640. The
`src/overlays/french_model_variant/variant429_entry.c` wrapper selects those
addresses and the palette, binding
`model_variant429_entry_linker_symbols.txt`.

Six images under French headers 795 and 892 are the North American
header-778 entry (`model_variant/variant778_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant795_linker_symbols.txt` binds to the French
addresses.

Ten images under French headers 653 and 720 are the North American
header-636 entry (`model_variant/variant636_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant653_entry{,_slot1}.c` with the data
labels 0x28 lower.

Six images under French headers 165 and 295 are the North American
header-148 entry (`model_variant/variant148_entry.c`) without its work-size
printf and format string, with the flash covering all 256 PAL lines. The
`src/overlays/french_model_variant/variant165_entry{,_slot1}.c` wrappers set
the height and move the data labels after the shorter text and read-only
section 0x24 lower.

Six images under French headers 812 and 909 are the North American
header-795 entry (`model_variant/variant795_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant812_linker_symbols.txt` binds to the French
addresses.

Sixteen images under French headers 676 and 743 are the North American
header-659 entry (`model_variant/variant659_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant676_entry{,_slot1}.c` with the data
labels 0x28 lower.

Four images under French headers 862 and 959 are the North American
header-845 entry (`model_variant/variant845_entry.c`) unchanged; only the
resident `jal` targets differ. The French modules build `variant845_entry.c`
and its `_slot1.c` wrapper directly through
`model_variant862_linker_symbols.txt`.

Six images under French headers 851 and 948 are the North American header-834
entry (`model_variant/variant834_entry.c` and its `_slot1.c` wrapper) with its
ring palette taken from x = 640 instead of 512. `VERSION_FRENCH` selects the
CLUT through `src/overlays/french_model_variant/variant851_entry{,_slot1}.c`;
the entry has no work-size printf, so the data labels do not move.

Eight images under French headers 689 and 756 are the North American
header-672 entry (`model_variant/variant672_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant689_entry{,_slot1}.c` with the data
labels 0x28 lower.

Ten images under French headers 690 and 757 are the North American
header-673 entry (`model_variant/variant673_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant690_entry{,_slot1}.c` with the data
labels 0x28 lower.

Twelve images under French headers 813 and 910 are the North American
header-796 entry (`model_variant/variant796_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant813_linker_symbols.txt` binds to the French
addresses.

Six images under French headers 830 and 927 are the North American
header-813 entry (`model_variant/variant813_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant830_linker_symbols.txt` binds to the French
addresses.

Eight images under French headers 389 and 539 build their entry from the
North American header-372 entry (`model_variant/variant372_entry.c`) through
`src/overlays/french_model_variant/variant389_entry{,_slot1}.c`, with
`VERSION_FRENCH` moving the CLUT column from 512 to 640, the sheets helper
0x3C later and the data labels 0x3C later. The French entry compiles under
`gcc_2_8_1_g0_split`; the North American one needs `gcc_2_7_2_cdk_g0`.

Twelve images under French headers 419 and 569 build their entry from the North
American header-402 entry (`model_variant/variant402_entry.c`) through
`french_model_variant/variant419_entry{,_slot1}.c` under `gcc_2_8_1_g0_split`.
The wrappers set the PAL palette x (`640` instead of `512`) and the French
addresses: the data sit four bytes higher (`CD0C`->`CD10`, `CD44`->`CD48`), the
swarm helper four bytes lower (`C1E0`->`C1DC`) and the quad helper eight bytes
lower (`C778`->`C770`) than in the North American images.
The swarm and quad helpers keep their accepted `variant419_{swarm,quads}` C, and
`model_variant419_linker_symbols.txt` now also binds the entry's resident calls.

Forty images under French headers 678 and 745 are the North American
header-661 entry (`model_variant/variant661_entry.c`) without its `E_DATA`
work-size printf, built the same way through
`src/overlays/french_model_variant/variant678_entry{,_slot1}.c` with the data
labels 0x28 lower.

Four images under French headers 824 and 921 are the North American
header-807 entry (`model_variant/variant807_entry.c`) with its full-screen
flash covering all 256 PAL lines. The
`src/overlays/french_model_variant/variant824_entry{,_slot1}.c` wrappers set
`VERSION_FRENCH` and bind `model_variant824_linker_symbols.txt`.

Eighteen images under French headers 679 and 746 are the North American
header-662 entry (`model_variant/variant662_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant679_entry{,_slot1}.c` with the data
labels 0x28 lower.

Six images under French headers 668 and 735 are the North American
header-651 entry (`model_variant/variant651_entry.c`) without its `E_DATA`
work-size printf. Its format string was the whole read-only section, so it
goes too, and the
`src/overlays/french_model_variant/variant668_entry{,_slot1}.c` wrappers move
the data labels 0x28 lower.

Two images under French header 463 are the North American header-446 entry
(`model_variant/variant446_entry.c`, which the US images build with the CDK
GCC 2.7.2 profile) rebuilt with GCC 2.8.1 at the same size. Its helpers move
to 0x2318 and 0x32E8, the uncalled rings helper keeps its French C
(`variant463_rings.c`) at 0x37D4, the data starts at 0x3C08 and the effect
palette loads from PAL x = 640. The
`src/overlays/french_model_variant/variant463_entry.c` wrapper selects those
addresses and the palette, binding
`model_variant463_entry_linker_symbols.txt`.

Eight images under French headers 660 and 727 are the North American
header-643 entry (`model_variant/variant643_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant660_entry{,_slot1}.c` with the data
labels 0x28 lower.

Ten images under French headers 661 and 728 are the North American
header-644 entry (`model_variant/variant644_entry.c`) without its `E_DATA`
work-size printf. Its format string was the whole read-only section, so it
goes too, and the
`src/overlays/french_model_variant/variant661_entry{,_slot1}.c` wrappers move
the data labels 0x28 lower.

Four images under French headers 860 and 957 are the North American
header-843 entry (`model_variant/variant843_entry.c`) with the burst palette
from the PAL CLUT row 0x3D28 instead of 0x3D20. The
`src/overlays/french_model_variant/variant860_entry{,_slot1}.c` wrappers set
`VERSION_FRENCH` and bind `model_variant860_linker_symbols.txt`.

Fifteen images under French headers 662 and 729 are the North American
header-645 entry (`model_variant/variant645_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant662_entry{,_slot1}.c` with the data
labels 0x28 lower.

Four images under French headers 822 and 919 are the North American
header-805 entry (`model_variant/variant805_entry.c`) unchanged; only the
resident `jal` targets differ. The French modules build `variant805_entry.c`
and its `_slot1.c` wrapper directly through
`model_variant822_linker_symbols.txt`.

Six images under French headers 780 and 877 are the North American
header-763 entry (`model_variant/variant763_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant780_linker_symbols.txt` binds to the French
addresses.

Ten images under French headers 667 and 734 are the North American
header-650 entry (`model_variant/variant650_entry.c`) without its `E_DATA`
work-size printf. Its format string was the whole read-only section, so it
goes too, and the
`src/overlays/french_model_variant/variant667_entry{,_slot1}.c` wrappers move
the data labels 0x28 lower.

Four images under French headers 859 and 956 are the North American
header-842 entry (`model_variant/variant842_entry.c`) with the ring palette
from the PAL CLUT row 0x3D28 instead of 0x3D20. The
`src/overlays/french_model_variant/variant859_entry{,_slot1}.c` wrappers set
`VERSION_FRENCH` and bind `model_variant859_linker_symbols.txt`.

Eight images under French headers 810 and 907 are the North American
header-793 entry (`model_variant/variant793_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant810_linker_symbols.txt` binds to the French
addresses.

Four images under French headers 838 and 935 are the North American
header-821 entry (`model_variant/variant821_entry.c`) unchanged; only the
resident `jal` targets differ. The French modules build `variant821_entry.c`
and its `_slot1.c` wrapper directly through
`model_variant838_linker_symbols.txt`.

Thirty-four images under French headers 680 and 747 are the North American
header-663 entry (`model_variant/variant663_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant680_entry{,_slot1}.c` with the data
labels 0x28 lower.

Seven images under French headers 672 and 739 are the North American
header-655 entry (`model_variant/variant655_entry.c`) without its `E_DATA`
work-size printf and format string. The
`src/overlays/french_model_variant/variant672_entry{,_slot1}.c` wrappers move
the data labels after the shorter text and read-only section 0x28 lower.

Fifteen images under French headers 686 and 753 are the North American
header-669 entry (`model_variant/variant669_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant686_entry{,_slot1}.c` with the data
labels 0x28 lower.

Six images under French headers 669 and 736 are the North American
header-652 entry (`model_variant/variant652_entry.c`) without its `E_DATA`
work-size printf, with the full-screen flash covering the 256-line PAL
screen. The format string was the whole read-only section, so it goes too,
and the `src/overlays/french_model_variant/variant669_entry{,_slot1}.c`
wrappers move the data labels 0x28 lower.

Eight images under French headers 806 and 903 are the North American
header-789 entry (`model_variant/variant789_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant806_linker_symbols.txt` binds to the French
addresses.

Twelve images under French headers 652 and 719 are the North American
header-635 entry (`model_variant/variant635_entry.c`) without its `E_DATA`
work-size printf. Its format string was the whole read-only section, so it
goes too, and the
`src/overlays/french_model_variant/variant652_entry{,_slot1}.c` wrappers move
the data labels 0x28 lower.

Six images under French headers 691 and 758 are the North American
header-674 entry (`model_variant/variant674_entry.c`) without its `E_DATA`
work-size printf. Its format string was the whole read-only section, so it
goes too, and the
`src/overlays/french_model_variant/variant691_entry{,_slot1}.c` wrappers move
the data labels 0x28 lower.

Eight images under French headers 791 and 888 are the North American
header-774 entry (`model_variant/variant774_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant791_linker_symbols.txt` binds to the French
addresses.

Eighteen images under French headers 164 and 294 are the North American
header-147 entry (`model_variant/variant147_entry.c`) without its work-size
printf and with its full-screen flash covering the 256 PAL lines instead of
240, built through
`src/overlays/french_model_variant/variant164_entry{,_slot1}.c` with the data
labels 0x1C lower.

Twenty-six images under French headers 675 and 742 are the North American
header-658 entry (`model_variant/variant658_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant675_entry{,_slot1}.c` with the data
labels 0x28 lower.

Six images under French headers 685 and 752 are the North American
header-668 entry (`model_variant/variant668_entry.c`) without its `E_DATA`
work-size printf, with the full-screen flash covering the 256-line PAL
screen. The format string was the whole read-only section, so it goes too,
and the `src/overlays/french_model_variant/variant685_entry{,_slot1}.c`
wrappers move the two data labels 0x28 lower. The uncalled function at
0x3520 and the data after it stay in place; in the two model-447 images that
helper keeps its accepted French C (`variant447_rings.c`, GCC 2.7.2 CDK)
through `variant685_rings{,_slot1}.c`, which alias the five resident names
it shares with the entry and bind them in
`model_variant685_rings_linker_symbols.txt`.

Twenty-six images under French headers 794 and 891 are the North American
header-777 entry (`model_variant/variant777_entry.c`) with its full-screen
flash covering the 256 PAL lines instead of 240. `VERSION_FRENCH` selects the
height, through
`src/overlays/french_model_variant/variant794_entry{,_slot1}.c`; the North
American images have no slot-one copy, so the slot-one wrapper renames the
labels itself.

Four images under French headers 833 and 930 are the North American
header-816 entry (`model_variant/variant816_entry.c`) unchanged; only the
resident `jal` targets differ. The French modules build `variant816_entry.c`
and its `_slot1.c` wrapper directly through
`model_variant833_linker_symbols.txt`.

Ten images under French headers 800 and 897 are the North American
header-783 entry (`model_variant/variant783_entry.c`) with its full-screen
flash tile covering the 256 PAL lines instead of 240. `VERSION_FRENCH` selects
the height, through
`src/overlays/french_model_variant/variant800_entry{,_slot1}.c`.

Four images under French header 139, and their four slot-one counterparts
under header 269, are the North American header-122 entry
(`model_variant/variant122_entry.c`, wired to US headers 122 and 252) with its
full-screen flash tile covering the 256 PAL lines instead of 240.
`VERSION_FRENCH` selects the height, through
`src/overlays/french_model_variant/variant139_entry{,_slot1}.c`.

Four images under French header 84, and their four slot-one counterparts
under header 214, are the North American header-67 entry
(`model_variant/variant67_entry.c`, wired to US headers 67 and 197) unchanged
apart from resident call targets, through
`src/overlays/french_model_variant/variant84_entry{,_slot1}.c`.

Eight images under French header 95, and their eight slot-one counterparts
under header 225, are the North American header-78 entry
(`model_variant/variant78_entry.c` and its `variant78_entry_slot1.c` copy)
unchanged: it has no work-size printf, and only the resident call targets
differ, which `model_variant95_linker_symbols.txt` binds to the French addresses.

Twenty images under French headers 674 and 741 are the North American
header-657 entry (`model_variant/variant657_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant674_entry{,_slot1}.c` with the data
labels 0x28 lower.

Four images under French headers 846 and 943 are the North American
header-829 entry (`model_variant/variant829_entry.c`) unchanged; only the
resident `jal` targets differ. The French modules build `variant829_entry.c`
and its `_slot1.c` wrapper directly through
`model_variant846_linker_symbols.txt`.

Nine images under French headers 698 and 765 are the North American
header-681 entry (`model_variant/variant681_entry.c`) without its `E_DATA`
work-size printf. Its format string was the whole read-only section, so it
goes too, and the
`src/overlays/french_model_variant/variant698_entry{,_slot1}.c` wrappers move
the data labels 0x28 lower.

Six images under French headers 159 and 289 are the North American
header-142 entry (`model_variant/variant142_entry.c`) without its work-size
printf or that call's `.rodata` format string, built through
`src/overlays/french_model_variant/variant159_entry{,_slot1}.c`. The scale
vector sits 0x14 lower and the image and command tables 0x24 lower.

Six images under French headers 182 and 312 build their entry from French C,
`src/overlays/french_model_variant/variant182_entry{,_slot1}.c`. It is the
effect of the North American header-165 entry without its work-size printf
or that call's `.rodata` format string, and with a 256-line screen flash.
The scale vector sits 0x14 lower than in the North American image and the
image and command tables 0x24 lower.

Six images under French headers 452 and 602 build their loader entry from
French C, `src/overlays/french_model_variant/variant452_entry.c` (slot 1
through `variant452_entry_slot1.c`), alongside the already-matched sheets,
streamers and lines helpers. The entry has no work-size printf; its command
rows and texture images are offsets into the image's tail data
(`D_8013D3E4`).

French overlays are included in the report generator's separate French
section and `french.overlays` JSON object. This overlay-only reporting does
not make claims about French resident completion. Report snapshots are
refreshed separately under #443.
