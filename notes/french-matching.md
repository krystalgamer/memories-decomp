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

## Inventory and layout caveats

The initial inventory contains **1,821 discovered resident boundaries**.
Only the 53 linked C functions are marked `matching_c`; the three verified
startup functions are classified as CRT. Other generated boundaries retain
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
Runtime overlay C matching and overlay inventories are subsequent work; this
resident batch does not claim that any French overlay has been decompiled.
Existing regional progress snapshots are left unchanged until French ownership
and overlay reporting can be added with evidence.
