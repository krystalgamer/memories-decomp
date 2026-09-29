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
