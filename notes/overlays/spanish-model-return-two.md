# Spanish MODEL return-two primaries

The remaining primary-entry family is a game-owned eight-byte handler that
returns 2 without accessing its context, command, data, or other functions.
It uses the existing two-argument MODEL callback ABI, not an SDK exclusion.
The named compiler profile is `gcc_2_8_1_g0_split` (GCC 2.8.1/MASPSX 2.81).

Two model-zero images are registered:

| Slot | Stage | Header | Archive sector | Load address | C interval |
|---|---:|---:|---:|---|---|
| 0 | 11 | 54 | 220 | `0x8013A000` | `0x8013A004..0x8013A00C` |
| 1 | 12 | 57 | 222 | `0x8017A000` | `0x8017A004..0x8017A00C` |

Each complete image is 4,096 bytes. The four-byte header and all 4,084 bytes
after the entry have real generated-data owners. **The entire suffix remains
unclassified and is not C coverage.** Finding a returning entry does not prove
that its suffix lacks other game code.

The primary loader mapping is described in the
[primary-handler evidence](spanish-model-primary.md). Across 621 compact
records, 611 records contain this entry at both primary slots: 1,222 instances.
The other ten records contain the 20 already registered nontrivial primaries.
The [instance ledger](spanish-model-return-two-instances.csv) records every
compact-record index, model ID, sector offset, and complete slot-image SHA-256.
It is evidence, not a second build manifest.

All 1,242 primary slices were re-extracted and checked against the legal
Spanish `MODEL.MRG`. Both slot-specific C objects were independently compiled.
All 1,222 return-two images were then individually linked with their own raw
header and suffix. Verification checked the actual selected compiler object,
eight-byte executable function extent, real input/final raw-data owners, and
the complete image hash. There are no direct callees or structure accesses
requiring extra callee/layout bindings.

The [attempt ledger](spanish-model-return-two-attempts.csv) records the terminal
exact candidates. Slot one renames only the function before including the
same C body. No reference types, instruction arrays, inline assembly, register
pinning, or one-off compiler flags were used.

Configured progress adds **two function instances / 16 instruction bytes**,
not 1,222 configured modules or 9,776 bytes. The exhaustive instance evidence
does not make unequal full images eligible for `duplicate_sector_offsets`.
Other MODEL variants, secondary loads, unknown suffixes, and the remaining
duel-bank curve are still open.

## Independent French verification

The French archive is verified against its own checksum manifest and the
earlier legal-disc comparison. All 1,242 French primary slices are re-read,
and both unchanged accepted slot-specific C objects are independently
compiled. All 1,222 return-two instances are individually linked with real
four-byte header and 4,084-byte suffix owners; the selected C object and
section-defined eight-byte function extent are checked in every image.
The other 20 primary slices retain their accepted nontrivial registrations.

There are 1,189 distinct full images among these 1,222 instances. The
independently generated French record/model/sector/hash rows agree exactly
with the existing instance ledger, which is shared rather than duplicated.
Identical archive hashes alone are not substituted for this verification.
The shared European tests exercise both regional manifests and, when their
legal inputs are present, every listed image in each archive.

French registration adds only the two model-zero representatives above and
16 C instruction bytes. All 32 accepted French images, including the partial
Exodia registration and its four assembly functions, remain unchanged.
Configured French coverage becomes **34 images, 245/249 matching C instances
and 169,516 instruction bytes**. All suffixes remain unclassified; other
runtime families and an exhaustive executable-code census remain open.

## French in-place promotion

After the all-release registration (#7120), each French return-two image has
its own physical registration. The 1,220 French instances other than the two
model-zero representatives belong to 1,188 raw-byte layouts. Images in the same
layout have identical archive, size, load address and complete payload. Each
of those layouts now uses the representative structure: a
four-byte raw header, the unchanged accepted slot-specific C entry, and a
4,084-byte unclassified tail. Module names, outputs and physical keys are kept,
and the shared linker-symbol and slot symbol files are reused.

Every one of the 3,594 configured French overlay images was rebuilt and matched,
including all 1,188 promoted layouts. For each promoted layout the regression
checks the complete image hash, that the selected C object is the only definer
of the eight-byte entry, and the raw header/tail owners. The
[module ledger](french-model-return-two-modules.csv) maps every
instance to its layout, model, record and sector.

Configured French progress gains **1,188 C instances / 9,504 instruction
bytes**, one per configured layout rather than per duplicate physical
instance. Every suffix remains unclassified and is not counted as C.
