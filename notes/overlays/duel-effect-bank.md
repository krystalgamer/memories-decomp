# French duel-effect bank

The six previously configured French overlays did not cover all runtime code.
The exact resident `Duel_LoadTerrainPackage` implementation requests WA sectors
`0x1B88 + terrain * 0xF0`. `Duel_LoadPackageStage` uses phase order
`0, 16, 1, 2, 3, 4, 5, 6, 7`: phase 0 returns 16 and the transfer callback
passes `q->result++`. The sizes before phase 7 are
`64 + 5 + 4 + 5 + 32 + 1 + 2 + 32 = 145` sectors.

Phase 7 transfers 44 sectors through `D_800101DC`. The original French
executable contains `0x80146000` at that pointer location. All seven complete
images at WA sectors `7193 + terrain * 240` are byte-identical:

| Property | Value |
|---|---|
| Image length | `0x16000` / 90,112 bytes |
| Load address | `0x80146000` |
| Module word | `0xB` |
| SHA-256 | `a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3` |
| Initial data and tables | `0x0..0x258` |
| Text | `0x258..0x141E4` |
| Trailing data and padding | `0x141E4..0x16000` |

The resident `DuelEffect_UpdateRequests` calls legacy `func_801462B0`,
independently bound to French `0x80146258`. This dispatcher selects many local
effect routines. Its texture-word helper is called at `0x8014F564`; its
`GsIMAGE` field accesses and page/CLUT expressions agree with the existing
local `model_texture_upload.c` and SDK declarations. No reference-project
types, compiler settings, or source bodies were imported.

The initial disassembly identifies 85 contiguous provisional functions,
covering all 81,804 text bytes. The first batch matched **ten functions /
1,380 bytes**:
seven at `0x8014F010..0x8014F490` and three at
`0x8014F524..0x8014F608`. They implement color assignment, randomized vector
initialization, quad setup, integer power, and texture-page/CLUT packing.
Address-based names remain until broader semantic naming review.
That batch left 75 boundaries as provisional generated assembly, not exclusions.
Neither raw data interval is claimed as C-owned storage.

## Experiments and acceptance

| Experiment | Result |
|---|---|
| Texture helper pair under existing `g0_split`, `g8_split`, and `g0` profiles | All 164 bytes match; not accepted until the entire bank was rebuilt. |
| Entire bank from generated assembly, then the helper pair in C | Both complete images match and both linked C symbols have exact sizes and section ownership. |
| Eleven adjacent utilities using array indexing and conditional-value expressions | Four quad functions differ; combined object is 36 bytes too large. |
| Explicit vertex-pointer traversal and sign-factor multiplication | Ten functions match; `func_8014F490` retains eight differing bytes from exchanged `s3`/`s4` allocation. |
| Narrow the remaining quad width to `s16` | Adds four bytes; rejected. |
| Keep 32-bit width with a `register` qualifier | Same eight-byte allocation mismatch; rejected. |
| Retain that 148-byte quad as generated assembly; build two complete C groups | Entire 90,112-byte image matches; all ten functions have exact addresses, sizes, and section-defined ELF ownership. |

An initial broad SDK `stdlib.h` include needed unrelated include search paths;
the existing narrow `rand.h` declaration avoids that dependency. `rand` is
bound to the verified French resident address `0x8008F708` in both the
disassembler symbols and the linker aliases. No aliases override C functions.
Compiler flags and shared resident implementations are unchanged.

The production `french-match-overlays` gate builds this image along with the
original six. `duplicate_sector_offsets` verifies the six additional complete
archive copies before the representative is built, avoiding seven redundant
identical layouts and inventories. Input hashing and duplicate-copy failures
are fatal; existing single-instance manifests retain their previous behavior.

## Remaining runtime scope

French boot code also exists outside the old six-overlay inventory:
`Main_RunBootSequence` requests WA `0x2503` for `0x25` sectors, and
`Main_LoadBootImageStage` consumes `32 + 1 + 1` sectors before its final
three-sector transfer. Those sectors start at 9509 and load at `0x80168000`
through `D_800101D8`. The complete 6,144-byte module hashes to
`22b33785288bd88f989d56741d6a9aa3621dc5b26d089fa2b5e09f6416454c05`;
resident code calls `0x801680F4` and `0x80168160`. Game versus SDK ownership
requires further evidence before promotion or exclusion.

The overworld `0x1618..0x17D0` fragment overlaps mutable live state, lacks a
prologue, and contains fixed-point code and an epilogue. Absence of external
direct jumps in a module-only scan does not establish unreachability.
MODEL/SU dynamic loads also require an exhaustive entry-point and ownership
audit. Neither resident completion nor this bank's inventory closes #6460.

## Vector and indexed-quad initialization

A second complete source group adds **five functions / 1,204 bytes** at
`0x8014F608..0x8014FABC`, bringing the bank to **15 matching functions /
2,584 bytes**, with 70 provisional functions still in generated assembly.
All ten previous C entries and all 85 original boundaries are preserved.

The first three functions initialize arrays of the existing SDK `SVECTOR`
type. The instruction stride is eight bytes, and writes target the three
halfwords at offsets zero, two, and four; padding is not modified.
Three-dimensional signed random spread, negative-only vertical spread, and
two-dimensional spread preserve the original signed remainder and division
operations. The scale and count arguments are independently constrained by
the observed halfword loads and masks. `rand` uses the already verified
resident binding and existing SDK header, not a local declaration.

The next two functions initialize four vertices from caller-supplied
halfword width and height tables. Both mirror widths across the vertical
axis; one indexes both row heights and the other zeros the lower row.
All Z coordinates are zero, with the original field-store and table-load
ordering retained. The pointers are arguments, not guessed module storage.

| Experiment | Result |
|---|---|
| Compile the three random-vector initializers with `gcc_2_8_1_g0_split` | First candidate matches all 920 bytes in a complete bank link. |
| Extend the same unit with both indexed-quad initializers | First candidate matches all 1,204 bytes; the complete bank remains byte-identical and all 15 C functions have exact linked ownership. |
| Promote with the owning header and unchanged named profile | Production overlay builds retain the complete image identity and all original module copies. |

No source from a different region or compiler variant was introduced.
`func_8014F490` remains the previously documented register-allocation
mismatch; no partial body, relocation-masked candidate, or new exclusion is
counted as a match.
