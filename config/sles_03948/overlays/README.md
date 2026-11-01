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

Two images under French headers 688 and 755 are the North American
header-671 entry (`model_variant/variant671_entry.c`) without its `E_DATA`
work-size printf, whose format string stays in North American data because of
its nonzero pad byte. They build through
`src/overlays/french_model_variant/variant688_entry{,_slot1}.c`, with the image
and command tables 0x28 lower.

Eight images under French headers 126 and 256 are the North American
header-109 entry (`model_variant/variant109_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant126_linker_symbols.txt` binds to the French
addresses.

Four images under French headers 137 and 267 are the North American
header-120 entry (`model_variant/variant120_entry.c`) with its full-screen
flash covering the 256 PAL lines instead of 240, built through
`src/overlays/french_model_variant/variant137_entry{,_slot1}.c`. The entry has
no work-size printf, so the data labels do not move.

Twelve images under French headers 649 and 716 are the North American
header-632 entry (`model_variant/variant632_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant649_entry{,_slot1}.c` with the data
labels 0x28 lower.

Two images under French headers 428 and 578 build their loader entry from the
North American header-411 entry (`model_variant/variant411us_entry.c`, which
the US images build with the CDK GCC 2.7.2 profile) through
`src/overlays/french_model_variant/variant428_entry{,_slot1}.c`, compiled with
`gcc_2_8_1_g0_split`. The entry is 4 bytes shorter, its two helpers keep
their French C (`variant428_swarm.c` and `variant428_quads.c`), the data
starts at 0x1D94 and the effect palette loads from PAL x = 640. The wrappers
bind `model_variant428_entry_linker_symbols.txt` and
`model_variant578_linker_symbols.txt`.

Three images under French header 429 are the North American header-412
entry (`model_variant/variant412_entry.c`, which the US images build with
the CDK GCC 2.7.2 profile) rebuilt with GCC 2.8.1. The entry is 4 bytes
shorter, its two helpers keep their French C
(`variant429_swarm.c` and `variant429_quads.c`) and the data starts at
0x1D90, and the effect palette loads from PAL x = 640. The
`src/overlays/french_model_variant/variant429_entry.c` wrapper selects those
addresses and the palette, binding
`model_variant429_entry_linker_symbols.txt`.

Two images under French headers 430 and 580 build their loader entry from the
North American header-413 entry (`model_variant/variant413_entry.c`) through
`src/overlays/french_model_variant/variant430_entry{,_slot1}.c`, compiled with
`gcc_2_8_1_g0_split` where the North American images need
`gcc_2_7_2_cdk_g0`. `VERSION_FRENCH` moves the CLUT column from 512 to 640,
leaves out the swarm helper the French images already build from
`image132118_swarm{,_slot0}.c`, and the data label sits 8 bytes lower.

Four images under French headers 481 and 631 build their loader entry from
the North American header-464 entry (`model_variant/variant464_entry.c`,
which the US images build with the CDK GCC 2.7.2 profile) through
`src/overlays/french_model_variant/variant481_entry{,_slot1}.c`, compiled with
`gcc_2_8_1_g0_split`. `VERSION_FRENCH` moves the CLUT column from 512 to 640;
the rings helper keeps its French C (`variant481_rings.c`) and the funnel
helper at +0xE38 remains generated assembly.

Two images under French headers 462 and 612 build their loader entry and
ribbons helper from the North American header-445 C
(`model_variant/variant445us_entry.c` and `variant445us_ribbons.c`, which the
US images build with the CDK GCC 2.7.2 profile) through
`src/overlays/french_model_variant/variant462_entry{,_slot1}.c` and
`variant462_ribbons{,_slot1}.c`, compiled with `gcc_2_8_1_g0_split`. The
sheets and rings helpers keep their French C (`variant462_sheets.c` and
`variant462_rings.c`), the helper calls and data label move with the longer
helpers, and the effect palette loads from PAL x = 640.

French MODEL headers 447 (stage 9 slot 0) and 597 (stage 10 slot 1) now also build their entry (`func_8013B004`, 0x10F4 bytes) from the US header-430 source `src/overlays/model_variant/variant430_entry.c` through `src/overlays/french_model_variant/variant447_entry{,_slot1}.c`. `VERSION_FRENCH` selects the palette at VRAM x 640, and the wrappers map the swarm call to 0x8013C0F8 and the GsIMAGE/command tables to their French addresses 8 bytes lower.

Six images under French headers 795 and 892 are the North American
header-778 entry (`model_variant/variant778_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant795_linker_symbols.txt` binds to the French
addresses.

French MODEL headers 797 (stage 9 slot 0) and 894 (stage 10 slot 1) build the header-780 entry (`func_8013B004`) from `src/overlays/model_variant/variant780_entry.c` through `src/overlays/french_model_variant/variant797_entry{,_slot1}.c`. `VERSION_FRENCH` selects the disc CLUT word 0x3D28; there is no work-size `printf`, so code and data keep the US layout.

Four images under French headers 809 and 906 are the North American
header-792 entry (`model_variant/variant792_entry.c`) unchanged; only the
resident `jal` targets differ. The French modules build `variant792_entry.c`
and its `_slot1.c` wrapper directly through
`model_variant809_linker_symbols.txt`.

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

Two images under French headers 855 and 952 are the North American
header-838 entry (`model_variant/variant838_entry.c`) with the glow palette
from the PAL CLUT row 0x3D28 instead of 0x3D20 and its full-screen flash
covering the 256 PAL lines. The
`src/overlays/french_model_variant/variant855_entry{,_slot1}.c` wrappers set
`VERSION_FRENCH` and the height and bind
`model_variant855_linker_symbols.txt` and
`model_variant952_linker_symbols.txt`.

Four images under French headers 192 and 322 are the North American
header-175 entry (`model_variant/variant175_entry.c` and its slot-1 wrapper)
unchanged: the code differs only in its resident call targets, which
`model_variant192_linker_symbols.txt` and `model_variant322_linker_symbols.txt`
bind to the French addresses.

Two images under French headers 166 and 296 are the North American
header-149 entry (`model_variant/variant149_entry.c`) without its work-size
printf or that call's `.rodata` format string, and with a 256-line screen
flash. Both slots build through
`src/overlays/french_model_variant/variant166_entry.c`. The scale vector sits
0x14 lower and the image and command tables 0x24 lower; each image's symbols
file binds the shared data names to its own addresses.

Two images under French headers 176 and 306 are the North American
header-159 entry (`model_variant/variant159_entry.c`) without its work-size
printf or that call's `.rodata` format string. Both slots build through
`src/overlays/french_model_variant/variant176_entry.c`. The scale vector sits
0x14 lower and the image and command tables 0x24 lower; each image's symbols
file binds the shared data names to its own addresses.

Two images under French headers 172 and 302 are the North American
header-155 entry (`model_variant/variant155_entry.c`) without its work-size
printf or that call's `.rodata` format string, and with a 256-line screen
flash, built through
`src/overlays/french_model_variant/variant172_entry{,_slot1}.c`. The scale
vector sits 0x14 lower and the image and command tables 0x24 lower.

Two images under French headers 180 and 310 are the North American
header-163 entry (`model_variant/variant163_entry.c`) with its full-screen
flash covering the 256 PAL lines instead of 240, built through
`src/overlays/french_model_variant/variant180_entry{,_slot1}.c`. The entry has
no work-size printf, so the data labels do not move.

Six images under French headers 812 and 909 are the North American
header-795 entry (`model_variant/variant795_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant812_linker_symbols.txt` binds to the French
addresses.

Four images under French headers 781 and 878 are the North American
header-764 entry (`model_variant/variant764_entry.c`) unchanged; only the
resident `jal` targets differ. The French modules build `variant764_entry.c`
and its `_slot1.c` wrapper directly through
`model_variant781_linker_symbols.txt`.

French MODEL headers 788 (stage 7 slot 0) and 885 (stage 8 slot 1) build the header-771 entry (`func_8013B004`) from `src/overlays/model_variant/variant771_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant788_entry{,_slot1}.c`; the French code and data are identical to the US images.

French MODEL headers 790 (stage 7 slot 0) and 887 (stage 8 slot 1) build the header-773 entry (`func_8013B004`) from `src/overlays/model_variant/variant773_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant790_entry{,_slot1}.c`; the French code and data are identical to the US images.

French MODEL headers 785 (stage 7 slot 0) and 882 (stage 8 slot 1) build the header-768 entry (`func_8013B004`) from `src/overlays/model_variant/variant768_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant785_entry{,_slot1}.c`; the French code and data are identical to the US images.

French MODEL headers 186 (stage 7 slot 0, models 197 and 600) and 316 (stage 8 slot 1) build the North American header-169 entry (`func_8013B004`) from `src/overlays/model_variant/variant169_entry.c` through `src/overlays/french_model_variant/variant186_entry{,_slot1}.c`. The wrappers select `VERSION_FRENCH`, which drops the work-size `printf`, and the PAL screen height. The shorter text moves the scale vector down by 0x14, to `D_8013D0B0`, and the image table and the configuration rows down by 0x24, to `D_8013D0C0` and `D_8013D114`.

French MODEL headers 174 (stage 7/9 slot 0, models 51 and 212) and 304 (stage 8/10 slot 1) build the North American header-157 entry (`func_8013B004`) from `src/overlays/model_variant/variant157_entry.c` through `src/overlays/french_model_variant/variant174_entry{,_slot1}.c`. `VERSION_FRENCH` drops the work-size `printf`, so the scale vector, the image table and the configuration rows sit lower, at `D_8013C448`, `D_8013C458` and `D_8013C51C`.

French MODEL headers 170 (stage 7 slot 0) and 300 (stage 8 slot 1) build the North American header-153 entry (`func_8013B004`) from `src/overlays/model_variant/variant153us_entry.c` through `src/overlays/french_model_variant/variant170_entry{,_slot1}.c`. The wrappers select `VERSION_FRENCH`, which drops the work-size `printf`, and the PAL screen height. The shorter text moves the scale vector down by 0x14, to `D_8013C428`, and the image view and the configuration rows down by 0x24, to `D_8013C438` and `D_8013C454`.

French MODEL headers 168 (stage 7 slot 0, models 105 and 143) and 298 (stage 8 slot 1) build the header-151 entry (`func_8013B004`) from `src/overlays/model_variant/variant151_entry.c` through `src/overlays/french_model_variant/variant168_entry{,_slot1}.c`. The wrappers select `VERSION_FRENCH`, which drops the work-size `printf`, and the PAL screen height. The shorter text moves the scale vector to 0xF5C, so the wrapper renames it through the source's `MODEL151_SCALE` selector and moves the image table and configuration rows down by 0x24 to `D_8013BF6C` and `D_8013BFA4`.

French MODEL headers 786 (stage 9 slot 0) and 883 (stage 10 slot 1) build the header-769 entry (`func_8013B004`) from `src/overlays/model_variant/variant769_entry.c` through `src/overlays/french_model_variant/variant786_entry{,_slot1}.c`, which set `GRAPHICS_DEFAULT_HEIGHT` to 256 for the PAL flash; there is no work-size `printf`, so code and data keep the US layout.

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

French MODEL headers 864 (stage 7 slot 0) and 961 (stage 8 slot 1) build the header-847 entry (`func_8013B004`) from `src/overlays/model_variant/variant847_entry.c` through `src/overlays/french_model_variant/variant864_entry{,_slot1}.c`. `VERSION_FRENCH` selects the burst CLUT word 0x3D28 and `GRAPHICS_DEFAULT_HEIGHT` 256 the PAL flash; there is no work-size `printf`, so code and data keep the US layout.

Six images under French headers 851 and 948 are the North American header-834
entry (`model_variant/variant834_entry.c` and its `_slot1.c` wrapper) with its
ring palette taken from x = 640 instead of 512. `VERSION_FRENCH` selects the
CLUT through `src/overlays/french_model_variant/variant851_entry{,_slot1}.c`;
the entry has no work-size printf, so the data labels do not move.

Two images under French headers 856 and 953 are the North American
header-839 entry (`model_variant/variant839_entry.c`) with its unused CLUT word
set to 0x3D28 instead of 0x3D20, as in the French header-772 entry;
`VERSION_FRENCH` selects it through
`src/overlays/french_model_variant/variant856_entry{,_slot1}.c`. The entry has
no work-size printf, so the data labels do not move.

Nine images under French headers 646 and 713 are the North American
header-629 entry (`model_variant/variant629_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant646_entry{,_slot1}.c` with the data
labels 0x28 lower.

French MODEL headers 852 (stage 7 slot 0) and 949 (stage 8 slot 1) build the header-835 entry (`func_8013B004`) from `src/overlays/model_variant/variant835_entry.c` through `src/overlays/french_model_variant/variant852_entry{,_slot1}.c`. `VERSION_FRENCH` selects the burst CLUT word 0x3D28; there is no work-size `printf`, so code and data keep the US layout.

French MODEL headers 853 (stage 7 slot 0) and 950 (stage 8 slot 1) build the header-836 entry (`func_8013B004`) from `src/overlays/model_variant/variant836_entry.c` through `src/overlays/french_model_variant/variant853_entry{,_slot1}.c`. `VERSION_FRENCH` selects the burst CLUT word 0x3D28; there is no work-size `printf`, so code and data keep the US layout.

Eight images under French headers 689 and 756 are the North American
header-672 entry (`model_variant/variant672_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant689_entry{,_slot1}.c` with the data
labels 0x28 lower.

Four images under French headers 645 and 712 are the North American
header-628 entry (`model_variant/variant628_entry.c`) without its `E_DATA`
work-size printf. Its format string was the whole read-only section, so it
goes too, and the
`src/overlays/french_model_variant/variant645_entry{,_slot1}.c` wrappers move
the data labels 0x28 lower.

Ten images under French headers 690 and 757 are the North American
header-673 entry (`model_variant/variant673_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant690_entry{,_slot1}.c` with the data
labels 0x28 lower.

Four images under French headers 682 and 749 are the North American
header-665 entry (`model_variant/variant665_entry.c`) without its `E_DATA`
work-size printf. Its format string was the whole read-only section, so it
goes too, and the
`src/overlays/french_model_variant/variant682_entry{,_slot1}.c` wrappers move
the data labels 0x28 lower.

Twelve images under French headers 813 and 910 are the North American
header-796 entry (`model_variant/variant796_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant813_linker_symbols.txt` binds to the French
addresses.

Four images under French headers 774 and 871 are the North American
header-757 entry (`model_variant/variant757_entry.c`) with the palette from
the PAL CLUT row 0x3D28 instead of 0x3D20. The
`src/overlays/french_model_variant/variant774_entry{,_slot1}.c` wrappers set
`VERSION_FRENCH` and bind `model_variant774_linker_symbols.txt`.

French MODEL headers 777 (stage 9 slot 0) and 874 (stage 10 slot 1) build the header-760 entry (`func_8013B004`) from `src/overlays/model_variant/variant760_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant777_entry{,_slot1}.c`; the French code and data are identical to the US images.

Two images under French headers 814 and 911 are the North American
header-797 entry (`model_variant/variant797_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf or screen-height constant, and
only the resident call targets differ, which
`model_variant814_linker_symbols.txt` binds to the French addresses. They
build through `src/overlays/french_model_variant/variant814_entry{,_slot1}.c`.

French MODEL headers 815 (stage 7 slot 0) and 912 (stage 8 slot 1) build the header-798 entry (`func_8013B004`) from `src/overlays/model_variant/variant798_entry.c` through `src/overlays/french_model_variant/variant815_entry{,_slot1}.c`, which define `VERSION_FRENCH` for the fan CLUT (0x3D28 instead of 0x3D20); there is no work-size `printf`, so code and data keep the US layout.

French MODEL headers 779 (stage 9 slot 0) and 876 (stage 10 slot 1) build the header-762 entry (`func_8013B004`) from `src/overlays/model_variant/variant762_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant762_entry_fr{,_slot1}.c`; the French code and data are identical to the US images. They link through their own `model_variant779_entry_linker_symbols.txt`, so the shared `model_variant779_linker_symbols.txt` is unchanged.

French MODEL headers 821 (stage 9 slot 0) and 918 (stage 10 slot 1) build the header-804 entry (`func_8013B004`) from `src/overlays/model_variant/variant804_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant821_entry{,_slot1}.c`; the French code and data are identical to the US images.

Six images under French headers 830 and 927 are the North American
header-813 entry (`model_variant/variant813_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant830_linker_symbols.txt` binds to the French
addresses.

French MODEL headers 832 (stage 9 slot 0) and 929 (stage 10 slot 1) build the header-815 entry (`func_8013B004`) from `src/overlays/model_variant/variant815_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant832_entry{,_slot1}.c`; the French code and data are identical to the US images.

Eight images under French headers 389 and 539 build their entry from the
North American header-372 entry (`model_variant/variant372_entry.c`) through
`src/overlays/french_model_variant/variant389_entry{,_slot1}.c`, with
`VERSION_FRENCH` moving the CLUT column from 512 to 640, the sheets helper
0x3C later and the data labels 0x3C later. The French entry compiles under
`gcc_2_8_1_g0_split`; the North American one needs `gcc_2_7_2_cdk_g0`.

French MODEL headers 403 (stage 9 slot 0) and 553 (stage 10 slot 1) now also build their entry (`func_8013B004`, 0xF34 bytes) from the US header-386 source `src/overlays/model_variant/variant386_entry.c` through `src/overlays/french_model_variant/variant403_entry{,_slot1}.c`. `VERSION_FRENCH` selects the palette at VRAM x 640, and the wrappers map the helper calls and the GsIMAGE/command tables to the French addresses (helpers 0x8013BF38/0x8013CBB0/0x8013D22C, tables 0x44 higher). The images link through their own `model_variant403_linker_symbols.txt`, a copy of the shared `model_variant472` map plus the entry's bindings.

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

French MODEL headers 825 (stage 9 slot 0) and 922 (stage 10 slot 1) build the header-808 entry (`func_8013B004`) from `src/overlays/model_variant/variant808_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant825_entry{,_slot1}.c`; the French code and data are identical to the US images.

French MODEL headers 827 (stage 7 slot 0) and 924 (stage 8 slot 1) build the header-810 entry (`func_8013B004`) from `src/overlays/model_variant/variant810_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant827_entry{,_slot1}.c`; the French code and data are identical to the US images. They link through their own `model_variant827_entry_linker_symbols.txt`, so the shared `model_variant827_linker_symbols.txt` is unchanged.

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

Two images under French headers 850 and 947 are the North American
header-833 entry (`model_variant/variant833_entry.c`) with the palette from
the PAL CLUT row 0x3D28 instead of 0x3D20. The
`src/overlays/french_model_variant/variant850_entry{,_slot1}.c` wrappers set
`VERSION_FRENCH` and bind `model_variant850_linker_symbols.txt` and
`model_variant947_linker_symbols.txt`.

Four images under French headers 187 and 317 are the North American
header-170 entry (`model_variant/variant170_entry.c`) without its work-size
printf, with its three full-screen flashes covering the 256-line
PAL screen. The `src/overlays/french_model_variant/variant187_entry{,_slot1}.c`
wrappers set the height and move the two data labels after the shorter text
and read-only section 0x24 lower.

French MODEL headers 861 (stage 9 slot 0) and 958 (stage 10 slot 1) build the header-844 entry (`func_8013B004`) from `src/overlays/model_variant/variant844_entry.c` through `src/overlays/french_model_variant/variant861_entry{,_slot1}.c`. `VERSION_FRENCH` selects the core CLUT word 0x3D28; there is no work-size `printf`, so code and data keep the US layout.

Fifteen images under French headers 662 and 729 are the North American
header-645 entry (`model_variant/variant645_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant662_entry{,_slot1}.c` with the data
labels 0x28 lower.

Two images under French headers 169 and 299 are the North American
header-152 entry (`model_variant/variant152_entry.c`) without its work-size
printf. Without the call the French setup branch returns on its own, which
`VERSION_FRENCH` reproduces with an inverted completion test, and the
`src/overlays/french_model_variant/variant169_entry{,_slot1}.c` wrappers move
the two data labels after the shorter text and read-only section 0x24 lower
and bind `model_variant169_linker_symbols.txt` and
`model_variant299_linker_symbols.txt`.

Four images under French headers 156 and 286 are the North American
header-139 entry (`model_variant/variant139_entry.c`) without its work-size
printf. The `src/overlays/french_model_variant/variant156_entry{,_slot1}.c`
wrappers move the three data labels after the shorter text and read-only
section 0x18 lower and bind `model_variant156_linker_symbols.txt` and
`model_variant286_linker_symbols.txt`.

Two images under French headers 158 (stage 9 slot 0) and 288 (stage 10 slot 1) are the North American header-141 entry (`model_variant/variant141_entry.c`) without its work-size printf or that call's `.rodata` format string and sized for 256 PAL lines, built through `src/overlays/french_model_variant/variant158_entry{,_slot1}.c`. `GRAPHICS_DEFAULT_HEIGHT` sets the per-line wave arrays, their loops and the wave period, so the work offsets after those arrays and the text differ from the US build; the image and command tables sit 0x48 lower (`D_8013EBB8`, `D_8013EC98`).

Two images under French headers 863 and 960 are the North American
header-846 entry (`model_variant/variant846_entry.c` and its slot-1 wrapper)
unchanged: the code differs only in its resident call targets, which
`model_variant863_entry_linker_symbols.txt` and
`model_variant960_linker_symbols.txt` bind to the French addresses.

Four images under French headers 822 and 919 are the North American
header-805 entry (`model_variant/variant805_entry.c`) unchanged; only the
resident `jal` targets differ. The French modules build `variant805_entry.c`
and its `_slot1.c` wrapper directly through
`model_variant822_linker_symbols.txt`.

French MODEL headers 823 (stage 7 slot 0) and 920 (stage 8 slot 1) build the header-806 entry (`func_8013B004`) from `src/overlays/model_variant/variant806_entry.c` through `src/overlays/french_model_variant/variant823_entry{,_slot1}.c`, which set `GRAPHICS_DEFAULT_HEIGHT` to 256 for the PAL flash; there is no work-size `printf`, so code and data keep the US layout.

Six images under French headers 780 and 877 are the North American
header-763 entry (`model_variant/variant763_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant780_linker_symbols.txt` binds to the French
addresses.

Two images under French headers 784 and 881 are the North American
header-767 entry (`model_variant/variant767_entry.c`) with its burst CLUT word
set to 0x3D28 instead of 0x3D20 and its full-screen flash covering the 256 PAL
lines, both selected by `VERSION_FRENCH` through
`src/overlays/french_model_variant/variant784_entry{,_slot1}.c`. The entry has
no work-size printf, so the data labels do not move.

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

Two images under French headers 816 and 913 are the North American
header-799 entry (`model_variant/variant799_entry.c`) with the ring palette
from the PAL CLUT row 0x3D28 instead of 0x3D20. The
`src/overlays/french_model_variant/variant816_entry{,_slot1}.c` wrappers set
`VERSION_FRENCH` and bind `model_variant816_linker_symbols.txt` and
`model_variant913_linker_symbols.txt`.

Eight images under French headers 810 and 907 are the North American
header-793 entry (`model_variant/variant793_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant810_linker_symbols.txt` binds to the French
addresses.

French MODEL headers 811 (stage 9 slot 0) and 908 (stage 10 slot 1) build the header-794 entry (`func_8013B004`) from `src/overlays/model_variant/variant794_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant811_entry{,_slot1}.c`; the French code and data are identical to the US images.

Four images under French headers 838 and 935 are the North American
header-821 entry (`model_variant/variant821_entry.c`) unchanged; only the
resident `jal` targets differ. The French modules build `variant821_entry.c`
and its `_slot1.c` wrapper directly through
`model_variant838_linker_symbols.txt`.

French MODEL headers 839 (stage 9 slot 0) and 936 (stage 10 slot 1) build the header-822 entry (`func_8013B004`) from `src/overlays/model_variant/variant822_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant839_entry{,_slot1}.c`; the French code and data are identical to the US images.

French MODEL headers 840 (stage 7 slot 0) and 937 (stage 8 slot 1) build the header-823 entry (`func_8013B004`) from `src/overlays/model_variant/variant823_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant840_entry{,_slot1}.c`; the French code and data are identical to the US images.

French MODEL headers 841 (stage 9 slot 0) and 938 (stage 10 slot 1) build the header-824 entry (`func_8013B004`) from `src/overlays/model_variant/variant824_entry.c` through `src/overlays/french_model_variant/variant841_entry{,_slot1}.c`. `VERSION_FRENCH` selects the shock-ring CLUT word 0x3D28; there is no work-size `printf`, so code and data keep the US layout.

French MODEL headers 842 (stage 9 slot 0) and 939 (stage 10 slot 1) build the header-825 entry (`func_8013B004`) from `src/overlays/model_variant/variant825_entry.c` through `src/overlays/french_model_variant/variant842_entry{,_slot1}.c`. `VERSION_FRENCH` selects the streak sprites' bottom texel row 0xD6 instead of 0xD7; there is no work-size `printf`, so code and data keep the US layout.

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

French MODEL headers 673 (stages 7/9 slot 0) and 740 (stages 8/10 slot 1) build the header-656 entry (`func_8013B004`) from `src/overlays/model_variant/variant656_entry.c` through `src/overlays/french_model_variant/variant673_entry{,_slot1}.c`. `VERSION_FRENCH` drops the `E_DATA` work-size `printf` and its string, so the entry ends 0x10 bytes earlier and the GsIMAGE and command tables move 0x28 lower (`D_8013D3E0`/`D_8013D418` become `D_8013D3B8`/`D_8013D3F0`).

Two images under French header 449 are the North American header-432 entry
(`model_variant/variant432_entry.c`, which the US images build with the CDK
GCC 2.7.2 profile) rebuilt with GCC 2.8.1 at the same size. Its four helpers
keep their French C (`variant449_sheet.c`, `_ribbons.c`, `_streamers.c` and
`_bands.c`), the data starts at 0x2BB4 and the effect palette loads from PAL
x = 640. The `src/overlays/french_model_variant/variant449_entry.c` wrapper
selects those addresses and the palette, binding
`model_variant449_entry_linker_symbols.txt`.

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

Two images under French header 455 are the North American header-438 entry
(`model_variant/variant438_entry.c`, which the US images build with the CDK
GCC 2.7.2 profile) rebuilt with GCC 2.8.1, 4 bytes shorter. The swarm helper
keeps its French C (`variant455_swarm.c`) at 0x13B0, the data starts at
0x1978 and the effect palette loads from PAL x = 640. The
`src/overlays/french_model_variant/variant455_entry.c` wrapper selects those
addresses and the palette, binding
`model_variant455_entry_linker_symbols.txt`.

Eight images under French headers 806 and 903 are the North American
header-789 entry (`model_variant/variant789_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf, and only the resident call
targets differ, which `model_variant806_linker_symbols.txt` binds to the French
addresses.

French MODEL headers 807 (stage 9 slot 0) and 904 (stage 10 slot 1) build the header-790 entry (`func_8013B004`) from `src/overlays/model_variant/variant790_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant807_entry{,_slot1}.c`; the French code and data are identical to the US images.

Four images under French headers 798 and 895 are the North American
header-781 entry (`model_variant/variant781_entry.c`) unchanged; only the
resident `jal` targets differ. The French modules build `variant781_entry.c`
and its `_slot1.c` wrapper directly through
`model_variant798_linker_symbols.txt`.

French MODEL headers 799 (stage 9 slot 0) and 896 (stage 10 slot 1) build the header-782 entry (`func_8013B004`) from `src/overlays/model_variant/variant782_entry.c` through `src/overlays/french_model_variant/variant799_entry{,_slot1}.c`; the entry has no work-size `printf` or region-specific constant, so code and data keep the US layout.

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

French MODEL headers 697 (stage 7 slot 0) and 764 (stage 8 slot 1) build the North American header-680 entry (`func_8013B004`) from `src/overlays/model_variant/variant680us_entry.c` through `src/overlays/french_model_variant/variant697_entry{,_slot1}.c`. `VERSION_FRENCH` drops the work-size `printf` and its format string. The text is 0x10 shorter, so the image table and command rows sit 0x28 lower, at `D_8013DA9C` and `D_8013DBD0`.

French MODEL headers 696 (stage 9 slot 0) and 763 (stage 10 slot 1) build the North American header-679 entry (`func_8013B004`) from `src/overlays/model_variant/variant679us_entry.c` through `src/overlays/french_model_variant/variant696_entry{,_slot1}.c`. `VERSION_FRENCH` drops the work-size `printf` and its format string. The text is 0x10 shorter, so the image table and command rows sit 0x28 lower, at `D_8013D110` and `D_8013D244`.

French MODEL headers 695 (stage 7 slot 0) and 762 (stage 8 slot 1) build the North American header-678 entry (`func_8013B004`) from `src/overlays/model_variant/variant678us_entry.c` through `src/overlays/french_model_variant/variant695_entry{,_slot1}.c`. `VERSION_FRENCH` drops the work-size `printf` and its format string, which was the whole read-only section. The text is 0x10 shorter, so the image table and command rows sit 0x28 lower, at `D_8013D218` and `D_8013D34C`.

French MODEL headers 650 (stage 7/9 slot 0: models 48, 52 and 475) and 717 (stage 8/10 slot 1) build the North American header-633 entry (`func_8013B004`) from `src/overlays/model_variant/variant633_entry.c` through `src/overlays/french_model_variant/variant650_entry{,_slot1}.c`. `VERSION_FRENCH` drops the work-size `printf` and its format string. The text is 0x10 shorter and the remaining rodata starts at 0x31D0, so the image view and command rows sit 0x28 lower, at `D_8013E274` and `D_8013E290`.

French MODEL headers 694 (stage 7 slot 0) and 761 (stage 8 slot 1), four images of models 420 and 543, are the North American header-677 entry (`model_variant/variant677_entry.c`) without its work-size printf, built through `src/overlays/french_model_variant/variant694_entry{,_slot1}.c`. The text is 0x10 shorter and the E_DATA format string is gone, so the image and command tables sit 0x28 lower (`D_8013D73C`, `D_8013D870`).

French MODEL headers 684 (stage 9 slot 0: models 109 and 468) and 751 (stage 10 slot 1) build the North American header-667 entry (`func_8013B004`) from `src/overlays/model_variant/variant667_entry.c` through `src/overlays/french_model_variant/variant684_entry{,_slot1}.c`. `VERSION_FRENCH` drops the work-size `printf` and its format string. The text is 0x10 shorter, so the image and command tables sit 0x28 lower, at `D_8013DDEC` and `D_8013DE40`.

French MODEL headers 693 (stage 9 slot 0: models 285, 502 and 644) and 760 (stage 10 slot 1) build the North American header-676 entry (`func_8013B004`) from `src/overlays/model_variant/variant676us_entry.c` through `src/overlays/french_model_variant/variant693_entry{,_slot1}.c`. `VERSION_FRENCH` drops the work-size `printf` and its format string. The text is 0x10 shorter, so the image and command tables sit 0x28 lower, at `D_8013E468` and `D_8013E510`.

French MODEL headers 666 (stage 7 slot 0) and 733 (stage 8 slot 1) build the North American header-649 entry (`func_8013B004`) from `src/overlays/model_variant/variant649us_entry.c` through `src/overlays/french_model_variant/variant666_entry{,_slot1}.c`. `VERSION_FRENCH` drops the work-size `printf` and its format string. The text is 0x10 shorter, so the image table and command rows sit 0x28 lower, at `D_8013DA84` and `D_8013DAD8`.

French MODEL headers 692 (stage 7/9 slot 0: models 446, 523 and 529) and 759 (stage 8/10 slot 1) build the North American header-675 entry (`func_8013B004`) from `src/overlays/model_variant/variant675us_entry.c` through `src/overlays/french_model_variant/variant692_entry{,_slot1}.c`. `VERSION_FRENCH` drops the work-size `printf` and its format string. The text is 0x10 shorter, so the image table and command rows sit 0x28 lower, at `D_8013DAD0` and `D_8013DB5C`.

Two images under French headers 171 and 301 are the North American
header-154 entry (`model_variant/variant154_entry.c`) without its work-size
printf and with the PAL screen height. Both slots build through
`src/overlays/french_model_variant/variant171_entry.c`, which sets
`VERSION_FRENCH` and `GRAPHICS_DEFAULT_HEIGHT` 256. The two data labels after
the shorter text and the scale vector sit 0x24 lower, and each image's symbols
file binds the shared data names to its own addresses. The images bind
`model_variant171_linker_symbols.txt` and
`model_variant301_linker_symbols.txt`.

Two images under French headers 699 and 766 are the North American
header-682 entry (`model_variant/variant682us_entry.c`) without its `E_DATA`
work-size printf. The
`src/overlays/french_model_variant/variant699_entry{,_slot1}.c` wrappers move
the two data labels after the shorter text and read-only section 0x28 lower
and bind `model_variant699_linker_symbols.txt` and
`model_variant766_entry_linker_symbols.txt`.

Two images under French headers 683 and 750 are the North American
header-666 entry (`model_variant/variant666_entry.c`) without its `E_DATA`
work-size printf. The format string lived in the data tail, so the
`src/overlays/french_model_variant/variant683_entry{,_slot1}.c` wrappers move
the two remaining data labels 0x28 lower and bind
`model_variant683_linker_symbols.txt` and
`model_variant750_linker_symbols.txt`.

Two images under French headers 124 and 254 are the North American
header-107 entry (`model_variant/variant107us_entry.c` and its slot-1
wrapper) unchanged: the code differs only in its resident call targets, which
`model_variant124_linker_symbols.txt` and
`model_variant254_linker_symbols.txt` bind to the French addresses.

Two images under French headers 847 and 944 are the North American
header-830 entry (`model_variant/variant830_entry.c` and its slot-1 wrapper)
unchanged: the code differs only in its resident call targets, which
`model_variant847_linker_symbols.txt` and
`model_variant944_linker_symbols.txt` bind to the French addresses.

Four images under French headers 130 and 260 are the North American
header-113 entry (`model_variant/variant113_entry.c` and its slot-1 wrapper)
unchanged: the code differs only in its resident call targets, which
`model_variant130_entry_linker_symbols.txt` and
`model_variant260_linker_symbols.txt` bind to the French addresses.

Four images under French headers 647 and 714 are the North American
header-630 entry (`model_variant/variant630_entry.c`) without its `E_DATA`
work-size printf. Its format string was the whole read-only section, so it
goes too, and the
`src/overlays/french_model_variant/variant647_entry{,_slot1}.c` wrappers move
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

Two images under French headers 677 and 744 are the North American
header-660 entry (`model_variant/variant660us_entry.c`) without its `E_DATA`
work-size printf. The
`src/overlays/french_model_variant/variant677_entry{,_slot1}.c` wrappers move
the two data labels after the shorter text and read-only section 0x28 lower
and bind `model_variant677_linker_symbols.txt` and
`model_variant744_linker_symbols.txt`.

Two images under French headers 849 and 946 are the North American
header-832 entry (`model_variant/variant832_entry.c`) with the palette from
the PAL CLUT row 0x3D28 instead of 0x3D20. The
`src/overlays/french_model_variant/variant849_entry{,_slot1}.c` wrappers set
`VERSION_FRENCH` and bind `model_variant849_entry_linker_symbols.txt` and
`model_variant946_linker_symbols.txt`.

Two images under French headers 843 and 940 are the North American
header-826 entry (`model_variant/variant826_entry.c`) with its full-screen
tint grid covering the 256-line PAL screen, so the tile heights become
shifts and the text is 0x20 bytes shorter. The
`src/overlays/french_model_variant/variant843_entry{,_slot1}.c` wrappers set
the height, move the three data labels 0x20 lower and bind
`model_variant843_linker_symbols.txt` and
`model_variant940_linker_symbols.txt`.

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

Two images under French headers 179 and 309 are the North American
header-162 entry (`model_variant/variant162us_entry.c`) without its
work-size printf. The
`src/overlays/french_model_variant/variant179_entry{,_slot1}.c` wrappers move
the two data labels after the shorter text and read-only section 0x1C lower
and bind `model_variant179_linker_symbols.txt` and
`model_variant309_linker_symbols.txt`.

Two images under French headers 160 and 290 are the North American
header-143 entry (`model_variant/variant143us_entry.c`) without its
work-size printf and padded format string. As in the header-152 family the
French setup branch returns on its own, which `VERSION_FRENCH` reproduces,
and the `src/overlays/french_model_variant/variant160_entry{,_slot1}.c`
wrappers move the two data labels after the shorter text and read-only
section 0x28 lower and bind `model_variant160_linker_symbols.txt` and
`model_variant290_linker_symbols.txt`.

Four images under French headers 195 and 325 are the North American
header-178 entry (`model_variant/variant178_entry.c`) without its `E_DATA`
work-size printf, with its per-line sway tables sized for the 256-line PAL
screen instead of 240. The French build seeds each line's phase with
`i << 5`, which `VERSION_FRENCH` selects, and the
`src/overlays/french_model_variant/variant195_entry{,_slot1}.c` wrappers
move the two data labels 0x58 lower.

Twenty-six images under French headers 794 and 891 are the North American
header-777 entry (`model_variant/variant777_entry.c`) with its full-screen
flash covering the 256 PAL lines instead of 240. `VERSION_FRENCH` selects the
height, through
`src/overlays/french_model_variant/variant794_entry{,_slot1}.c`; the North
American images have no slot-one copy, so the slot-one wrapper renames the
labels itself.

Two images under French headers 119 and 249 are the North American
header-102 entry (`model_variant/variant102_entry.c` and its slot-1 wrapper)
unchanged: the code differs only in its resident call targets, which
`model_variant119_entry_linker_symbols.txt` and
`model_variant249_entry_linker_symbols.txt` bind to the French addresses.

Four images under French headers 138 and 268 are the North American
header-121 entry (`model_variant/variant121_entry.c`) with its full-screen
flash covering the 256 PAL lines instead of 240. The
`src/overlays/french_model_variant/variant138_entry{,_slot1}.c` wrappers set
the height and bind `model_variant138_linker_symbols.txt` and
`model_variant268_linker_symbols.txt`.

Two images under French headers 854 and 951 are the North American
header-837 entry (`model_variant/variant837_entry.c`) with the ring palette
from the PAL CLUT row 0x3D28 instead of 0x3D20 and its full-screen flash
covering the 256 PAL lines. The
`src/overlays/french_model_variant/variant854_entry{,_slot1}.c` wrappers set
`VERSION_FRENCH` and the height and bind
`model_variant854_linker_symbols.txt` and
`model_variant951_linker_symbols.txt`.

Four images under French headers 833 and 930 are the North American
header-816 entry (`model_variant/variant816_entry.c`) unchanged; only the
resident `jal` targets differ. The French modules build `variant816_entry.c`
and its `_slot1.c` wrapper directly through
`model_variant833_linker_symbols.txt`.

French MODEL headers 836 (stage 9 slot 0) and 933 (stage 10 slot 1) build the header-819 entry (`func_8013B004`) from `src/overlays/model_variant/variant819_entry.c` through `src/overlays/french_model_variant/variant836_entry{,_slot1}.c`, which define `VERSION_FRENCH` for the ring CLUT (0x3D28 instead of 0x3D20); there is no work-size `printf`, so code and data keep the US layout.

French MODEL headers 837 (stage 7 slot 0) and 934 (stage 8 slot 1) build the header-820 entry (`func_8013B004`) from `src/overlays/model_variant/variant820_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant837_entry{,_slot1}.c`; the French code and data are identical to the US images.

French MODEL headers 834 (stage 7 slot 0) and 931 (stage 8 slot 1) build the header-817 entry (`func_8013B004`) from `src/overlays/model_variant/variant817_entry.c` through `src/overlays/french_model_variant/variant834_entry{,_slot1}.c`, which set `GRAPHICS_DEFAULT_HEIGHT` to 256 for the PAL flash; there is no work-size `printf`, so code and data keep the US layout. They link through their own `model_variant834_entry_linker_symbols.txt`, so the shared `model_variant834_linker_symbols.txt` is unchanged.

French MODEL headers 835 (stage 9 slot 0) and 932 (stage 10 slot 1) build the header-818 entry (`func_8013B004`) from `src/overlays/model_variant/variant818_entry.c` through `src/overlays/french_model_variant/variant835_entry{,_slot1}.c`. `VERSION_FRENCH` selects the fan CLUT word 0x3D28; there is no work-size `printf`, so code and data keep the US layout.

Ten images under French headers 800 and 897 are the North American
header-783 entry (`model_variant/variant783_entry.c`) with its full-screen
flash tile covering the 256 PAL lines instead of 240. `VERSION_FRENCH` selects
the height, through
`src/overlays/french_model_variant/variant800_entry{,_slot1}.c`.

French MODEL headers 801 (stage 9 slot 0) and 898 (stage 10 slot 1) build the header-784 entry (`func_8013B004`) from `src/overlays/model_variant/variant784_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant801_entry{,_slot1}.c`; the French code and data are identical to the US images.

French MODEL headers 805 (eight slot-0 images at stages 7 and 9) and 902 (their eight slot-1 twins) build the header-788 entry (`func_8013B004`) from `src/overlays/model_variant/variant788_entry.c` through `src/overlays/french_model_variant/variant805_entry{,_slot1}.c`, which set `GRAPHICS_DEFAULT_HEIGHT` to 256 for the PAL flash; there is no work-size `printf`, so code and data keep the US layout.

French MODEL headers 803 (stage 9 slot 0) and 900 (stage 10 slot 1) build the header-786 entry (`func_8013B004`) from `src/overlays/model_variant/variant786_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant803_entry{,_slot1}.c`; the French code and data are identical to the US images.

French MODEL headers 802 (stage 7 slot 0) and 899 (stage 8 slot 1) build the header-785 entry (`func_8013B004`) from `src/overlays/model_variant/variant785_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant802_entry{,_slot1}.c`; the French code and data are identical to the US images.

Four images under French headers 808 and 905 are the North American
header-791 entry (`model_variant/variant791_entry.c`) unchanged; only the
resident `jal` targets differ. The French modules build `variant791_entry.c`
and its `_slot1.c` wrapper directly through
`model_variant808_linker_symbols.txt`.

Four images under French header 139, and their four slot-one counterparts
under header 269, are the North American header-122 entry
(`model_variant/variant122_entry.c`, wired to US headers 122 and 252) with its
full-screen flash tile covering the 256 PAL lines instead of 240.
`VERSION_FRENCH` selects the height, through
`src/overlays/french_model_variant/variant139_entry{,_slot1}.c`.

Two images under French headers 149 and 279 are the North American
header-132 entry (`model_variant/variant132_entry.c`) with its full-screen
flash covering the 256 PAL lines instead of 240, built through
`src/overlays/french_model_variant/variant149_entry.c` in both slots. The entry has
no work-size printf, so the data labels do not move.

Two images under French headers 150 and 280 are the North American
header-133 entry (`model_variant/variant133_entry.c`) with its full-screen
flash covering the 256 PAL lines instead of 240. Both slots build through
`src/overlays/french_model_variant/variant150_entry.c`. The entry has
no work-size printf, so the data labels do not move.

Two images under French headers 155 and 285 are the North American
header-138 entry (`model_variant/variant138_entry.c`) without its work-size
printf or that call's format string (kept in the North American image's
data because of its nonzero padding), built through
`src/overlays/french_model_variant/variant155_entry{,_slot1}.c`. The scale
vector sits 0x14 lower and the image and command tables 0x28 lower.

Two images under French headers 152 and 282 are the North American
header-135 entry (`model_variant/variant135_entry.c`) without its work-size
printf or that call's `.rodata` format string. Both slots build through
`src/overlays/french_model_variant/variant152_entry.c`. The scale vector sits
0x14 lower and the image and command tables 0x24 lower; each image's symbols
file binds the shared data names to its own addresses.

Two images under French headers 154 and 284 are the North American
header-137 entry (`model_variant/variant137_entry.c`) without its work-size
printf or that call's `.rodata` format string and with its flashes sized for
256 PAL lines. Both slots build through
`src/overlays/french_model_variant/variant154_entry.c`. The scale vector sits
0xC lower and the image and command tables 0x1C lower; each image's symbols
file binds the shared data names to its own addresses.

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

Two images under French headers 106 (stage 9 slot 0) and 236 (stage 10 slot 1) build the header-89 entry (`func_8013B004`) from `src/overlays/model_variant/variant89_entry.c` through `src/overlays/french_model_variant/variant106_entry{,_slot1}.c`, which set `GRAPHICS_DEFAULT_HEIGHT` to 256 for the PAL flash; there is no work-size `printf`, so code and data keep the US layout.

French MODEL headers 110 (stage 7 slot 0) and 240 (stage 8 slot 1) build the header-93 entry (`func_8013B004`) from `src/overlays/model_variant/variant93_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant110_entry{,_slot1}.c`; the French code and data are identical to the US images. They link through their own `model_variant110_entry_linker_symbols.txt`, so the shared `model_variant110_linker_symbols.txt` is unchanged.

Two images under French headers 114 and 244 are the North American header-97
entry (`model_variant/variant97_entry.c`) with its full-screen flash covering
the 256 PAL lines instead of 240, built through
`src/overlays/french_model_variant/variant114_entry{,_slot1}.c`. The entry has
no work-size printf, so the data labels do not move.

Two images under French headers 112 and 242 are the North American header-95
entry (`model_variant/variant95_entry.c`) with its full-screen flash covering
the 256 PAL lines instead of 240, built through
`src/overlays/french_model_variant/variant112_entry{,_slot1}.c`. The entry has
no work-size printf, so the data labels do not move.

Four images under French headers 120 and 250 are the North American
header-103 entry (`model_variant/variant103_entry.c`) with its full-screen
flash covering the 256 PAL lines instead of 240, built through
`src/overlays/french_model_variant/variant120_entry{,_slot1}.c`. The entry has
no work-size printf, so the data labels do not move.

Four images under French headers 125 and 255 are the North American
header-108 entry (`model_variant/variant108_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf or screen-height constant, and
only the resident call targets differ, which
`model_variant125_linker_symbols.txt` binds to the French addresses. They
build through `src/overlays/french_model_variant/variant125_entry{,_slot1}.c`.

Four images under French headers 118 and 248 are the North American
header-101 entry (`model_variant/variant101_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf or screen-height constant, and
only the resident call targets differ, which
`model_variant118_linker_symbols.txt` binds to the French addresses. They
build through `src/overlays/french_model_variant/variant118_entry{,_slot1}.c`.

Two images under French headers 113 and 243 are the North American header-96
entry (`model_variant/variant96_entry.c` and its `_slot1.c` wrapper)
unchanged: it has no work-size printf or screen-height constant, and only the
resident call targets differ, which `model_variant113_linker_symbols.txt`
binds to the French addresses. They build through
`src/overlays/french_model_variant/variant113_entry{,_slot1}.c`.

Two images under French headers 123 and 253 are the North American
header-106 entry (`model_variant/variant106_entry.c`) unchanged: it has no
work-size printf or screen-height constant, and only the resident call targets
differ, which `model_variant123_linker_symbols.txt` binds to the French
addresses. Both slots build that source directly.

Two images under French headers 115 and 245 are the North American header-98
entry (`model_variant/variant98_entry.c` and its `_slot1.c` wrapper)
unchanged: it has no work-size printf or screen-height constant, and only the
resident call targets differ, which `model_variant115_linker_symbols.txt`
binds to the French addresses. They build through
`src/overlays/french_model_variant/variant115_entry{,_slot1}.c`.

Two images under French headers 122 and 252 are the North American
header-105 entry (`model_variant/variant105_entry.c`) unchanged: it has no
work-size printf or screen-height constant, and only the resident call targets
differ, which `model_variant122_linker_symbols.txt` binds to the French
addresses. Both slots build that source directly.

Twenty images under French headers 674 and 741 are the North American
header-657 entry (`model_variant/variant657_entry.c`) without its `E_DATA`
work-size printf, built through
`src/overlays/french_model_variant/variant674_entry{,_slot1}.c` with the data
labels 0x28 lower.

Two images under French header 397 are the North American header-380 entry
(`model_variant/variant380_entry.c`, which the US images build with the CDK
GCC 2.7.2 profile) rebuilt with GCC 2.8.1, 4 bytes shorter. Its helpers start
at 0xDE4, 0x1A9C (keeping the French `variant397_sheets.c` C) and 0x206C, the
data starts at 0x2950 and the effect palette loads from PAL x = 640. The
`src/overlays/french_model_variant/variant397_entry.c` wrapper selects those
addresses and the palette, binding
`model_variant397_entry_linker_symbols.txt`.

Four images under French headers 846 and 943 are the North American
header-829 entry (`model_variant/variant829_entry.c`) unchanged; only the
resident `jal` targets differ. The French modules build `variant829_entry.c`
and its `_slot1.c` wrapper directly through
`model_variant846_linker_symbols.txt`.

French MODEL headers 848 (stage 7 slot 0) and 945 (stage 8 slot 1) build the header-831 entry (`func_8013B004`) from `src/overlays/model_variant/variant831_entry.c` through `src/overlays/french_model_variant/variant848_entry{,_slot1}.c`, which define `VERSION_FRENCH` for the core CLUT (0x3D28 instead of 0x3D20) and set `GRAPHICS_DEFAULT_HEIGHT` to 256 for the PAL flash; there is no work-size `printf`, so code and data keep the US layout.

Nine images under French headers 698 and 765 are the North American
header-681 entry (`model_variant/variant681_entry.c`) without its `E_DATA`
work-size printf. Its format string was the whole read-only section, so it
goes too, and the
`src/overlays/french_model_variant/variant698_entry{,_slot1}.c` wrappers move
the data labels 0x28 lower.

French MODEL headers 700 (stage 7 slot 0) and 767 (stage 8 slot 1) build the header-683 entry (`func_8013B004`) from `src/overlays/model_variant/variant683_entry.c` through `src/overlays/french_model_variant/variant700_entry{,_slot1}.c`. `VERSION_FRENCH` drops the `E_DATA` work-size `printf` and its string, so the entry ends 0x10 bytes earlier and the image and command tables move 0x28 lower (`D_8013D52C`/`D_8013D5D4` become `D_8013D504`/`D_8013D5AC`); `GRAPHICS_DEFAULT_HEIGHT` 256 selects the PAL screen fill.

Four images under French headers 670 and 737 are the North American
header-653 entry (`model_variant/variant653_entry.c`) without its `E_DATA`
work-size printf. Its format string was the whole read-only section, so it
goes too, and the
`src/overlays/french_model_variant/variant670_entry{,_slot1}.c` wrappers move
the data labels 0x28 lower.

Two images under French headers 778 and 875 are the North American
header-761 entry (`model_variant/variant761_entry.c` and its `_slot1.c`
wrapper) unchanged: it has no work-size printf or screen-height constant, and
only the resident call targets differ, which
`model_variant778_linker_symbols.txt` binds to the French addresses. They
build through `src/overlays/french_model_variant/variant778_entry{,_slot1}.c`.

French MODEL headers 768 (stage 7 slot 0) and 865 (stage 8 slot 1) build the header-751 entry (`func_8013B004`) from `src/overlays/model_variant/variant751_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant768_entry{,_slot1}.c`; the French code and data are identical to the US images.

French MODEL headers 776 (stage 7 slot 0) and 873 (stage 8 slot 1) build the header-759 entry (`func_8013B004`) from `src/overlays/model_variant/variant759_entry.c` through plain wrappers in `src/overlays/french_model_variant/variant776_entry{,_slot1}.c`; the French code and data are identical to the US images.

Six images under French headers 159 and 289 are the North American
header-142 entry (`model_variant/variant142_entry.c`) without its work-size
printf or that call's `.rodata` format string, built through
`src/overlays/french_model_variant/variant159_entry{,_slot1}.c`. The scale
vector sits 0x14 lower and the image and command tables 0x24 lower.

Two images under French headers 161 and 291 are the North American
header-144 entry (`model_variant/variant144_entry.c`) without its work-size
printf or that call's format string (kept in the North American image's
data because of its nonzero padding) and with a 256-line screen flash, built
through `src/overlays/french_model_variant/variant161_entry{,_slot1}.c`. The
scale vector sits 0x14 lower and the image and command tables 0x28 lower.

Six images under French headers 182 and 312 build their entry from French C,
`src/overlays/french_model_variant/variant182_entry{,_slot1}.c`. It is the
effect of the North American header-165 entry without its work-size printf
or that call's `.rodata` format string, and with a 256-line screen flash.
The scale vector sits 0x14 lower than in the North American image and the
image and command tables 0x24 lower.

French MODEL headers 183 (stage 9 slot 0) and 313 (stage 10 slot 1) build the US header-166 entry from `src/overlays/model_variant/variant166_entry.c` through `src/overlays/french_model_variant/variant183_entry{,_slot1}.c`. `VERSION_FRENCH` drops the work-size `printf` and its string (entry -0x14; tables `D_8013CCDC`/`D_8013CD68` become `D_8013CCB8`/`D_8013CD44`), and `GRAPHICS_DEFAULT_HEIGHT` 256 selects the PAL flash height.

Two images under French headers 190 and 320 are the North American
header-173 entry (`model_variant/variant173_entry.c`) without its work-size
printf or that call's `.rodata` format string, built through
`src/overlays/french_model_variant/variant190_entry{,_slot1}.c`. The scale
vector sits 0x14 lower and the image and command tables 0x24 lower.

French MODEL headers 191 (stage 9 slot 0) and 321 (stage 10 slot 1) build the US header-174 entry from `src/overlays/model_variant/variant174_entry.c` through `src/overlays/french_model_variant/variant191_entry{,_slot1}.c`. `VERSION_FRENCH` drops the work-size `printf` and its string (entry -0x14; tables `D_8013C7CC`/`D_8013C83C` become `D_8013C7A8`/`D_8013C818`), and `GRAPHICS_DEFAULT_HEIGHT` 256 selects the PAL flash height.

French MODEL headers 383 (stage 9 slot 0) and 533 (stage 10 slot 1) now also build their entry (`func_8013B004`, 0xCB8 bytes) from the US header-366 source `src/overlays/model_variant/variant366_entry.c` through `src/overlays/french_model_variant/variant383_entry{,_slot1}.c`. `VERSION_FRENCH` selects the palette at VRAM x 640, and the wrappers map the helper calls and the GsIMAGE/command tables to the French addresses (helpers 0x8013BCBC/0x8013C91C/0x8013D8FC, tables 0xC higher).

Six images under French headers 452 and 602 build their loader entry from
French C, `src/overlays/french_model_variant/variant452_entry.c` (slot 1
through `variant452_entry_slot1.c`), alongside the already-matched sheets,
streamers and lines helpers. The entry has no work-size printf; its command
rows and texture images are offsets into the image's tail data
(`D_8013D3E4`).

Six images under French headers 456 and 606 build their loader entry from
`src/overlays/spanish_model_variant/variant456_entry{,_slot1}.c`, next to the
already-matched ribbons, sheets, lines and streamers helpers. The entry
follows the North American header-433 entry's shape; its command rows
(0x2C bytes) and texture images are offsets into the tail label
`D_8013ED00`. The helper at image offset 0x101C builds from
`french_model_variant/variant456_twin_ribbons{,_slot1}.c` and the sprays
helper at 0x1F58 from
`src/overlays/spanish_model_variant/variant456_sprays{,_slot1}.c`.

Two images under French headers 458 and 608 (model 202, stage 7 slot 0 and stage 8 slot 1) build their loader entry from `src/overlays/french_model_variant/variant458_entry.c` in both slots, next to the already-matched sheets, strands and streamers helpers. The entry has no work-size printf; its command rows and texture images are offsets into the tail label `gFrenchModelVariant458_D230C`, which each image's symbols file binds at its own address. A dead `double` conversion in the strand loop and a `do { } while (0)` around its body reproduce the retail loop scheduling and are commented in the source.

Two images under French headers 454 and 604 build their loader entry from the
North American header-437 entry (`model_variant/variant437_entry.c`) through
`src/overlays/french_model_variant/variant454_entry{,_slot1}.c`, compiled with
`gcc_2_8_1_g0_split` where the North American images need
`gcc_2_7_2_cdk_g0`. `VERSION_FRENCH` moves the CLUT column from 512 to 640,
leaves out the blooms helper the French images already build from
`image22822_blooms{,_slot0}.c`, and the data labels sit 4 bytes lower.

Two images under French headers 468 and 618 build their loader entry from the
North American header-451 entry (`model_variant/variant451_entry.c`) through
`src/overlays/french_model_variant/variant468_entry{,_slot1}.c`, compiled with
`gcc_2_8_1_g0_split` where the North American images need
`gcc_2_7_2_cdk_g0`. `VERSION_FRENCH` moves the CLUT column from 512 to 640,
leaves out the swarm helper the French images already build from
`image134612_swarm{,_slot1}.c`, and the data label sits 8 bytes lower. The two
images bind resident calls through their own
`model_variant468_linker_symbols.txt`.

French MODEL headers 471 (stage 9 slot 0) and 621 (stage 10 slot 1) now also build their entry (`func_8013B004`, 0x152C bytes) from the US header-454 source `src/overlays/model_variant/variant454_entry.c` through `src/overlays/french_model_variant/variant471_entry{,_slot1}.c`. `VERSION_FRENCH` selects the palette at VRAM x 640, and the wrappers map the helper calls to 0x8013C530/0x8013CA34 and the GsIMAGE/command base `D_8013CEFC` to `D_8013CF10`.

French MODEL headers 472 (stage 7 slot 0) and 622 (stage 8 slot 1) now also build their entry (`func_8013B004`, 0x110C bytes) from the US header-455 source `src/overlays/model_variant/variant455_entry.c` through `src/overlays/french_model_variant/variant472_entry{,_slot1}.c`. `VERSION_FRENCH` selects the palette at VRAM x 640, and the wrappers map the helper calls (0x8013C734/0x8013CD30/0x8013D8B0) and the GsIMAGE/command tables (0x24 higher) to the French addresses. The images link through their own `model_variant622_linker_symbols.txt`, a copy of the shared `model_variant472` map plus the entry's bindings.

French overlays are included in the report generator's separate French
section and `french.overlays` JSON object. This overlay-only reporting does
not make claims about French resident completion. Report snapshots are
refreshed separately under #443.
