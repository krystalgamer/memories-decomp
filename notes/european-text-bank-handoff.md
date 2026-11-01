# PAL boot-tail and text-decoder views

This component completes the PAL-only views left outside the earlier
[logical string-bank](text-bank-views.md) component. It connects the boot
text-tail transfers, alternate local-string address anchor and encoded
decimal-digit source without equating their different C types or claiming
one uniform regional bank layout.

## Boot destination words are not table data

The European `Main_LoadBootPackageStage` has eight cases. After the image
staging/upload cases, case 5 reads `gFile_pBootTextStage5Destination` (`0x800101E0`)
separately into both descriptor destinations for a 30-sector phase, case 6
does the same with `gFile_pBootTextStage6Destination` (`0x800101E4`) for
32 sectors, and case 7 uses `gFile_pBootTextStage7Destination`
(`0x800101E8`) for seven. The European link scripts bind these names
directly; the former `D_800101E0/4/8` labels are gone.

Each remains a stored `u8 *G32`, not a byte array at the word's address,
an integer address, const object or inferred homogeneous pointer table.
The existing repeated loads, `(s32)` casts, destination-store order,
phase sizes, transfer-state masks, done values and all earlier image
steps are unchanged. The declarations live in
`european_text_bank_views.h`.

These names identify the proven per-stage destination role. No local
PAL retail executable is available to independently inspect the words'
contents, so this change does not assert pointer values or associate each
word with a particular complete bank allocation. A transfer byte count is
not an allocation extent or proof that it contains the digit string.
The same numeric addresses in NA/Japan are not assigned these PAL roles.

## Alternate anchor is separate from the offset table

`gText_AlternateStringAddressAnchor` retains native `D_801C0004` and its
mutable, incomplete `u16[]` declaration. PAL local IDs from `0x500` use
this provider's masked address while still reading halfword offsets from
the existing `gText_PrimaryStringOffsets` provider at `D_801B0004`.
It is not renamed to the primary offset table merely because the address
begins with `801C`.

All four uses remain in their original branches in `Text_LookupString`,
`TextBox_BuildStep` and the secondary text-command unit. The standalone
lookup retains OR composition; build/command paths retain their original
addition expressions. Comparisons, signed index handling, the existing
`0x100` subtraction, global-ID split, masks, card-info operation offsets,
stream pushes and return order are untouched. No common decoder helper,
new bound or assumed four-byte bank-header format is introduced.

## Digit string is not the Shift-JIS key block

`gText_DecimalDigitGlyphString` retains native `D_801BF870` as a mutable,
incomplete `u8[]`. The PAL decimal-map initializer reads its bytes directly:
an initial or subsequent leading `0xFF` stops initialization; codes below
`0xF0` use one byte; otherwise the low nibble becomes the high byte and
the next source byte completes the glyph code.

It emits at most ten map entries and preserves the original pointer
advance, second-byte handling and partial/empty initialization. An early
terminator does not clear later map entries. The source is not recast to
the NA/Japanese twenty-byte Shift-JIS key block or made const. The unused
PAL key-search helper beside the initializer is unchanged.

## Guard, profiles and evidence scope

`european_text_bank_views.h` exposes all five explicit extern/native-asm
bindings only under `VERSION_EUROPE`. The five complete common C files
change only those identifiers and one include each, plus the existing
digit-string comment. All original native declarations, primary/global
logical views, wrappers, symbol maps, data providers, masks and named
profiles remain unchanged.

The active routes comprise 15 actual source/profile units: five NA, five
Japanese and five shared PAL units used by all five European manifests.
Their complete extents are 19 functions / 4,132 instruction bytes in NA,
19 / 3,644 in Japan, and 21 / 4,324 in each PAL manifest. Profiles retain
the real -G0/-G8, common-symbol and split-addressing differences, including
the existing named `gcc_2_8_1_cc_g8_as_g0_split` PAL boot profile; no
compiler/assembler flags are invented or normalized.

Fresh untouched baselines and candidates compare complete actual compiler
output, MASPSX assembly, symbol tables and disassembly/relocations across
all 15 units. NA/Japan bodies do not consume the guarded aliases; PAL
retains every original relocation identity and qualifier. This is actual
native-source conservation, not execution of complete PAL/Japanese images
or local cloud/regional/witness test suites.

The clean complete NA resident retains retail SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
Applicable full regional image validation remains a requirement of
ordinary exact-head PR CI, not something inferred from that NA hash.
All existing tests and controls remain unchanged; no synthetic/generated-C
or source-string harness, new testcase or manual CI rerun is added.
