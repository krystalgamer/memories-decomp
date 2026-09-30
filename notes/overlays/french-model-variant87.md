# French MODEL headers 87 and 217

The header-87 entry has an exact C implementation in
`src/overlays/french_model_variant/variant87_entry.c`. Header 217 uses the
same body through a slot-1 symbol-renaming wrapper. The existing named
`gcc_2_8_1_g0_split` profile uses GCC 2.8.1 and MASPSX 2.81.

Production integration reproduces all four complete 20,480-byte images
for models 140 and 584, alternate 1, both slots. The
[instance ledger](french-model-variant87-instances.csv) records their
independent archive slices, commands, actual header words and hashes.
The measured slot-1 header is 217, not the 237 that a higher-family
header-number convention would suggest.

## Loader and image ownership

The compact archive records are 140 and 534 respectively. Stages 9 and 10
select ten 2,048-byte sectors beginning at `record * 276 + 200` and
`record * 276 + 210`. The four starting sectors are 38,840, 38,850,
147,584 and 147,594. Each slice was read directly from the checksum-verified
French `MODEL.MRG`, independently of the extracted scratch payload.

Loads are `0x8013B000` and `0x8017B000`; entry is at `+4`.
The complete entry occupies `0x4..0xA1C`: 2,584 bytes, 646 instruction
words and a 496-byte stack frame. A direct control-flow walk covers every
entry word, with 29 direct resident calls, no module-local calls and one
return. Each calibration ELF has three nonoverlapping sized owners:
the four-byte header, the C function and the 17,892-byte raw suffix.
The descriptor symbol aliases the start of that suffix, not a duplicate
storage allocation. The suffix is unclassified, not proven wholly data.

The slot wrapper renames the entry and descriptor symbol. Real compiler
and linker relocations reproduce the slot-1 address differences; no
instruction masking or binary patching is used for acceptance.

## Descriptors and context views

The matched resident initializer `func_8004CB0C` copies `D_80010024/28`
into each slot's `field_DEC`. The French retail values are
`0x80136000` and `0x80176000`. The matched controller `func_800559D4`
passes this field to the secondary entry at module `+4`, initializes with
the request modulo 1,000 in state 7, then transitions to state 8.
Subsequent updates pass `-1`. The primary handler receives the distinct
`field_DE8` context.

All four selected archive records have command 17,001 and therefore use
descriptor 1 at module offset `0xA32`. A descriptor is 22 bytes:
outer RGB at 0, inner RGB at 3, part at 6, count at 7, then signed
halfwords for radius, spread, growth, hold, fade, interval and delay at
8 through `0x14`. The selected values are part 11, count 16, radius 70,
spread 300, growth 10, hold 30, fade 10, interval 2 and delay 40.

The context view has the stored configuration pointer at 0, 26
`SVECTOR` point records at 4, destination storage beginning at `0xD4`,
the frame word at `0x1D4` and the completion byte at `0x1D8`.
Initialization establishes the configuration, all 26 points, all 16
selected destinations, frame and completion before subsequent updates.
The last accessed point component ends at `0xD2`; the last accessed
selected destination component ends at `0x152`. These do not overlap the
following fields. Target-compiled layout constants independently verify
these offsets and SDK argument layouts.

The final completion-byte access establishes a 473-byte minimum view.
Its C size of 476 includes trailing alignment. The opaque `0x100`-byte
destination span represents the measured separation from `0xD4` to
`0x1D4`; it does not assert 32 initialized vectors or an allocation size.
No backing object is introduced by this pointer view. The three selected
load ranges—96-sector model payload, two-sector primary module and
ten-sector secondary module—do not overlap these context accesses.

These are fixed-RAM pointers, not allocator returns. Total reservation
capacity, primary-context write extents and whole-game interference from
other scene controllers are not inferred. Other scene controllers also
use `field_DEC`; this evidence is specific to the selected loader and
callback path, not a global lifetime audit.

## Exactness evidence

The canonical body and its slot wrapper reproduce all 81,920 image bytes.
A fresh complete French resident build matches
`57ecdfb9a9e1faf8b342fe7c7304c23723810861f3ab2fa3bef9eb27b5146b44`.
All 21 distinct resident callee destinations and all 29 object call
relocations were checked against that build. The three resident functions
owning initialization, callback dispatch and transfer phases have matching
sized C symbols and byte-identical retail bodies.

The local pinned Psy-Q 4.6 signature evidence independently identifies
`ccos`, `csin`, `RotTransPersN`, `AverageZ3`, `SetPolyG3`, `GsSortBoxFill`
and `rand`. The random generator matches `LIBC2.LIB/RAND.OBJ`; the shorter
`LIBC.LIB/C47.OBJ` candidate does not match. These signature checks establish
SDK identities, not masked game-code acceptance or a compiler change.

The final source keeps the observed arithmetic widths and scalar lifetimes:
unsigned grouping of the Y offset preserves the halfword result while
retaining the target instruction order; Z reuses the spread across the
modulo and offset adjustment; the completion result has a separate scalar.
Simplifying those expressions changes instruction scheduling or registers.
Unusual phase-dependent position/scale behavior is preserved, not repaired.

This family does not establish exhaustive runtime overlay coverage.

## Production acceptance

The full French overlay gate reproduces all 172 configured images while
preserving the previous 168 module records. The clean French resident
gate passes independently. Production ELFs have four sized C owners and
eight sized raw owners for the new images, with the same complete-image
hashes as calibration. All 24 layout constants were recompiled using the
final canonical header; all 21 resident callees were checked again.

The [attempt ledger](french-model-variant87-attempts.csv) preserves one
missing-binding failure, 36 source/compiler mismatches, the exact-text
candidate and two terminal canonical-source records. A separate initial
fixture failure exposed the incorrect slot-header delta assumption before
publication; all four retail header words were then checked directly.
The corrected fixture retains the shared default for other families and
specifies this family's measured delta of 130.

All 82 French MODEL variant regressions pass, including seven for this
family covering archive slices, source fingerprints, raw owners, complete
entry control flow, selected descriptor/context bounds and resident calls.
Metadata, basic-type and G32/PSXLONG checks pass.

The addition is four matching C instances and 10,336 instruction bytes.
Configured French totals are 718/1,235 matching C instances and 651,796
C instruction bytes. These counts do not classify the raw suffixes or
prove complete runtime coverage. Project-wide report refreshes remain
separate snapshots.
