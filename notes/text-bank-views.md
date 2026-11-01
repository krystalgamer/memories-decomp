# Logical string-bank lookup views

| Canonical view | NA provider | Japanese provider | PAL provider |
| --- | --- | --- | --- |
| `gText_PrimaryStringOffsets` | `D_801C0000` | `D_801C0000` | `D_801B0004` |
| `gText_GlobalStringOffsets` | `D_801D5800` | `D_801D6000` | `D_801D5804` |
| `gText_PrimaryStringAddressAnchor` | `D_801B0000` | no compiled consumer | no compiled consumer |

These names describe the existing lookup roles, not uniform physical banks
or a recovered file format. All remain mutable incomplete `u16[]` views.
No array bound, allocation, `const`, packed structure or new decoder is
introduced.

## Primary offsets and their address coupling

`Text_LookupString`, `TextBox_BuildStep` and the secondary text commands
read halfword offsets from the primary table. Ordinary indices from
`0x500` retain the existing subtraction of `0x100`.

NA uses its separate address anchor for ordinary strings. It also uses the
primary table's own masked address for the above-`0xCFFF` branch, indexing
that same table with `id - 0xD000`. The name does not eliminate this shared
table/address role or assert that every entry describes one content class.

Japan uses the primary table as its masked address anchor as well as its
offset table. PAL uses `D_801B0004` for both low-range offsets and the
below-`0x500` address anchor; for larger local IDs, its address anchor
switches to the distinct `D_801C0004` while offsets still come from the
primary table.

That PAL alternate anchor remains address-named. It is not incorrectly
substituted for NA/Japan's primary offset table merely because its address
starts with `801C`.

## Global offsets retain regional ID rules

NA selects its global offsets after the separate above-`0xCFFF` branch.
Japan selects its global bank with bit 15 and masks that bit from the
index. PAL keeps its `0x7FFF` split and `0x8000` subtraction.

All three tracked Japanese consumer wrappers select
`VERSION_JAPAN_TEXT_LOOKUP_STRING`, retaining `0x801F0000` as the address
mask. Other consumers retain `0xFFFF0000`. Each base is still derived
from the original provider's masked address, not the exact start of an
inferred allocation.

The standalone lookup keeps OR composition. Text-box and command paths
keep their existing additions or OR expressions. No common helper
replaces these instruction-sensitive forms.

Card-info operation `0x40` keeps its NA `+0xD100` versus Japanese/PAL
`+0x100` construction. Operation `0x20` keeps `+0x8000`. These branches
do not establish a uniform name/description file cohort.

## Ownership and source projection

The legacy `text_constants.h` declarations, masks and guards are unchanged.
The narrow canonical header follows that owner in each of the three
resident source files. The address-anchor declaration remains harmless
where unused; only NA receives a native linker binding for that role.

Fifteen active logical/native bindings retain their original external
providers. Thirteen addresses are mapped locally; the two original
Japanese imported providers remain symbolic rather than being assigned
guessed numeric addresses.

Several old regional identifiers project to one logical name inside
unchanged conditional branches. Conservation records each original
identifier occurrence, verifies the complete projected source and
reconstructs the complete original file using that provenance. Each
compiled source/profile independently reverses the logical names to its
own original assembler identities. Neither check filters unrelated text,
instructions, directives or ordering.

## Unchanged stream and loader contracts

The lookup's unused first argument, ID signedness, repeated loads and
stream-stack increment/store/return order remain. The staged string
callback still returns pointer-slot storage through its measured ABI.

The PAL decimal glyph byte string and the PAL boot destination words stay
outside this scope. A stored `u8 *G32` load-address word is not a bank's
`u16[]` data view, and a transfer window is not an allocation extent.
No European-only pointer-word meaning is assigned to a coincident NA
address.

Regional wrappers, source groups, named profiles, old headers, overlays
and all existing tests stay unchanged. No synthetic/generated-C or
source-string harness, fixture, probe or testcase is added. Existing
cloud/regional/overlay/witness suites are not run locally; fresh full-image
regional PR gates remain mandatory.

## Separate PAL tail handoff

The previously excluded PAL views now have their own scoped
[boot/anchor/digit-string contract](european-text-bank-handoff.md).
`gText_AlternateStringAddressAnchor` keeps the distinct `0x801C0004`
provider (formerly `D_801C0004`), and `gText_DecimalDigitGlyphString` keeps the encoded byte-string
provider rather than adopting the NA/Japanese Shift-JIS key-block type.
The three boot destination words remain stored pointers under their
individual labels. None replaces the primary/global offset views above
or asserts an identical physical bank layout across releases.
