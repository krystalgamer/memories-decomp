# French resident matching

The French `SLES-03948` target is built with the existing GCC 2.8.1/MASPSX
pipeline and shared regional linker driver. Its complete executable SHA-256 is
`57ecdfb9a9e1faf8b342fe7c7304c23723810861f3ab2fa3bef9eb27b5146b44`.

Supply legally obtained inputs at `game/france/SLES_039.48`,
`game/france/DATA/SU.MRG`, and `game/france/DATA/WA_MRG.MRG`. Their sizes and
hashes are fixed in `config/sles_03948/target.yaml` and `files.sha256`. The
French CI workflow obtains these from `YGOFM_SLES_03948_URL`,
`YGOFM_FRA_SU_MRG_URL`, and `YGOFM_FRA_WA_MRG_URL`, respectively.

From the repository root:

```sh
make verify-french-inputs
MAKEFLAGS=-j4 make french-match
```

`french-match` cleans generated split/build output, compiles the configured C,
assembles remaining text, and compares the complete executable. It does not
accept function-only or relocation-normalized comparisons. All other regional
build targets and matching sources remain unchanged.

## First resident batch

The first batch matches **53 functions / 4,228 bytes** using **41 existing
European/shared translation units**, without adding or copying C. The named
compiler profiles are unchanged from the European matches. Each grouped
translation unit preserves source-definition order, contiguous function
coverage, and one profile. The inventory names and exact sizes of the C
functions were verified against the linked French ELF.

The correspondence CSV attached to issue #6460 was used only to select
candidate addresses. For each selected complete source group, French input
bytes at those addresses equal the European function bytes. Compiling and
linking every selected group then reproduces the entire French input exactly;
the CSV alone is not acceptance evidence.

The experiment sequence was:

| Candidate | Result |
|---|---|
| Same-address reuse | No complete matching C functions; French text and data relocations differ. |
| Correspondence-address reuse, unchanged European profiles | 53 functions in 41 whole translation units have identical input instruction bytes. |
| Link those C groups without named data aliases | Unresolved references to six shared data symbols; no source/compiler change required. |
| Resolve the six symbols from the identical absolute-address instruction pairs | Exact complete executable, including a clean `make french-match`. |

The six data aliases in `link_symbols.ld` retain their European addresses:
the instruction pairs that reference them are unchanged in the French image.
This is not a general assumption that French globals share European addresses.
The remaining large binary-backed tail is preserved, not declared decompiled.

## Relocation-backed resident expansion

The relocation-backed batch matches **726 functions / 214,696 bytes** from
**431 unchanged European/shared translation units**. This adds **673
functions / 210,468 bytes** while retaining every source, profile, address,
and size from the first batch. No new C, compiler flags, or inline assembly
are introduced.

The European executable was rebuilt exactly before its objects were used
as reference evidence. Candidate selection required whole-source contiguity,
unchanged non-relocation instruction bits, and no additional allocated data
sections. French addresses were then recovered from J26, absolute 32-bit,
HI16/LO16, and GP-relative relocations. Object addends and the European ELF
symbol values validate each interpretation; French GP is `0x8009C298`.
Bindings must agree across all uses and with the actual addresses of selected
C definitions.

The relocation scan examined 7,527 sites and recovered 1,085 distinct external
symbol bindings. Thirteen source groups with unsupported common-symbol
ownership were deferred, not forced into C.
Groups requiring separate rodata, small-data or BSS integration remain for a
later batch. These screening decisions do not replace the final image gate.

| Experiment | Result |
|---|---|
| 726 functions with recovered legacy C bindings | Link rejected an address-name collision between relocated C and generated French assembly. |
| Disambiguate the overlapping C function name | Link succeeded, but 120 instruction words differed: generated French assembly was using legacy numeric aliases intended only for the shared C. |
| Give conflicting generated French symbols an address-based `_French` suffix | Exact full executable, followed by a clean production `make french-match`. |

The suffixes distinguish symbol namespaces; they do not assert new semantics.
The legacy C aliases in `link_symbols.ld` keep the recovered French address,
while the explicitly named French symbols retain their actual numeric
addresses. Generated Splat files were not patched. The shared source still
compiles normally, and the existing assembly fallback still reconstructs its
original bytes.

## Rodata-backed resident expansion

The resident manifest now matches **806 functions / 269,960 text bytes** in
**462 unchanged European/shared translation units**. This batch adds **80
functions / 55,264 text bytes** from **31 complete translation units**, with
**2,149 bytes of compiler-owned rodata**. Every previous matched address,
size, source, and compiler profile is retained. The added sources contain
neither inline assembly nor source-local extern declarations.

This includes transfer control, duel package loading and ritual effects,
library and menu control, memory-card handling, model packet processing, and
AI card queries. The European inventory establishes game ownership; SDK
callees remain external bindings, not newly claimed C.

The verified European objects and linked ELF were used to interpret **4,090
relocation sites**. Local rodata bases are recovered from French text
references, rather than inferred from the correspondence CSV. Object
addends and European symbol addresses disambiguate reused HI16/LO16 pairs.
Local section references, jump tables, GP-relative references, and external
bindings must agree. The original sources and named profiles are unchanged.

The split assigns each recovered rodata interval to its owning C object.
Unclaimed data remains generated assembly, with explicit original padding.
Symbols defined by linked C objects are not duplicated as absolute linker
aliases: this preserves their ELF type, size, and ownership as well as bytes.
The accepted inventory checks every C function against its exact linked
address and size.

| Experiment | Result |
|---|---|
| Link all 31 rodata-backed groups with recovered bindings | Splat rejected the old generated `func_8003C3C4_French` name at the newly matched `File_RequestEgyptOverworldPackage` address. |
| Retire that replaced fallback alias | The complete image differed starting at file offset `0x1F82`: generated raw rodata omitted two trailing zero bytes, shifting later sections. |
| Preserve the explicit two-byte pad and normal object alignment | The entire executable matched byte-for-byte. |
| Validate exact ELF C symbols | An unnecessary absolute alias shadowed an existing C definition; remove aliases for all linked C definitions, not only the new group. |
| Limit function-namespace disambiguation to actual fallback symbols | Avoid artificial disassembler boundaries while retaining the exact image and all 1,838 provisional inventory boundaries. |
| Clean production build and linked-inventory validation | 806 exact linked C functions; complete executable SHA-256 matches the fixed French target. |

No COMMON, small-data, or BSS ownership is guessed for this batch. Unsupported
groups remain assembly fallback. Full-image matching, rather than a masked
relocation comparison or function-only check, is the acceptance criterion.

## Inventory and layout caveats

The initial inventory contains **1,821 discovered resident boundaries**.
After the relocation-backed expansion and a clean split, the current
inventory contains **1,838 boundaries**, including **806 linked C functions**
marked `matching_c`; the three verified startup functions are classified as
CRT. Other generated boundaries retain
`unmatched_asm` with unknown ownership. They must not all be counted as game
targets: SDK provenance and handwritten-code classification are still needed.
Disassembler boundaries are provisional, not semantic evidence.

`make french-inventory` regenerates assembly boundaries after an exact build,
retaining the matched C records. Changes to matched ranges require updating the
matching manifest and verifying the linked ELF, not merely regenerating CSV.

The startup establishes entry `0x800128CC`, GP `0x8009C298`, and the BSS clear
range `0x8009C408..0x800FFC30`. `french-map` verifies the BSS range against
the first four startup instructions and checks every mapped region's hash.
Resident text ends after the return delay slot at `0x800918D8`. The image map
deliberately groups the remaining tail rather than borrowing unverified
European overlay/data boundaries.

The French SU/WA archives are independently extracted and hash-verified.
All six inventoried overlay instances now match unchanged European C, as
documented in `config/sles_03948/overlays/README.md`. This resident batch does
not change those manifests. French progress remains overlay-only until the
resident ownership denominator is established; report snapshots are separate
from matching changes.
