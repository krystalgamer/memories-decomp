# Duel-effect packet helpers

## Scoped packet/projection C views

The complete four-function packet object and two-function projection object
now expose six typed C names through `packet_semantic_views.h`; the symbol
files bind them in place of the former function labels. Profiles and
grouping remain unchanged. The new header follows each affected source's complete original
SDK/header block.

| C name | Former label | NA address | JP address | UK / FR / ES address |
|---|---|---|---|---|
| `DuelEffect_SubmitOffsetQuad` | `func_80152EC4` | `0x80153A9C` | `0x80160F58` | `0x80152EC4` |
| `DuelEffect_SubmitOffsetFt4` | `func_80152F9C` | `0x80153B74` | `0x80161030` | `0x80152F9C` |
| `DuelEffect_SubmitOffsetGt4` | `func_801530B0` | `0x80153C88` | `0x80161144` | `0x801530B0` |
| `DuelEffect_InitBaseMatrix` | `func_801531C4` | `0x80153D9C` | `0x80161258` | `0x801531C4` |
| `DuelEffect_ProjectSubmitFt4` | `func_80151218` | `0x80151DF0` | `0x8015F2AC` | `0x80151218` |
| `DuelEffect_ProjectSubmitGt4` | `func_8015131C` | `0x80151EF4` | `0x8015F3B0` | `0x8015131C` |

Six registry records use `overlay/duel_effects/function` at the actual NA
inventory addresses. Thirty name cells change across the five configured
bank inventories, not their non-name fields, extents, order or profiles.
Numeric label suffixes do not establish the NA VMA. No resident records,
new native exports, linker aliases or German/Italian bank coverage are added.

### Offset submission

All three offset helpers keep four X updates followed by four Y updates,
with their repeated halfword reads and narrowing. They retain these
different submission paths:

| View / condition | Submission |
|---|---|
| Generic quad, all flags | Original resident call with wrapped base priority plus one; flags forwarded |
| FT4, mode 1 | Resident call at base priority with flags 1 |
| FT4, every other `u16` mode | Clear semitransparency, SDK sort at base priority |
| GT4, mode 1 | Resident call at base priority with flags 1 |
| GT4, every other `u16` mode | Clear semitransparency, SDK sort at wrapped base priority plus one |

The generic view remains `void *`: actual G4 packets and FT4-layout
coordinates share the accessed XY offsets, as the original caller-backed
contract below explains. It is not narrowed to FT4 or generalized to
arbitrary incompatible packet layouts. No offset caching, reordered
stores, mode-as-boolean normalization or priority widening is introduced.

### Projection and base matrix

Both projection helpers keep `RotAverage4` and its packed coordinate
`PSXLONG *` output casts. Coordinates can be written even when the later
signed flag test suppresses submission; a rejected flag does not make the
whole function a no-op.

With nonnegative flags and zero bias, each helper delegates to its typed
offset path, retaining its fixed module priority and offset behavior.
With nonzero bias, both retain `depth - (bias - 1)`, the negative-to-zero
clamp and unsigned-halfword conversion after arithmetic shift by two.
The FT4 mode-zero branch clears code mask `0x02` and SDK-sorts; other FT4
modes use the resident call. The GT4 biased path always uses the resident
call, including mode zero. Keep these differences, locals, comparison
domains, casts and SDK/call order rather than unifying the routines.

`DuelEffect_InitBaseMatrix` writes identity rotation components and
translation `(0, 0, 300)`, not a wholly identity affine matrix. It retains
the explicit diagonal and paired off-diagonal stores and leaves the SDK
matrix pad untouched. Actual effect callers use it before layered transform
setup. No new matrix type, storage, padding initialization or axis unit is
inferred.

### Conservation

All actual bank callers use the six C names, whose typed declarations
preserve the original ABI. Existing owners, tests, fixtures, source selectors, negative controls and
historical curve/paired-ABI guards remain unchanged. Module data stays
native; this pass does not invent data namespaces or new global meanings.

Accepted-parent CPP/native/whole-image comparisons account only for those
declarations, one include per affected C file and the six identifiers.
Raw compiler/MASPSX output, native symbols, ordinary/fallback assembly and
the complete NA executable/ELF must match. Full five-bank and seven-copy
acceptance uses the existing PR gates, not local cloud tests. No
C/source-string/native harness is added.

## Original matching evidence and contracts

The complete contiguous group `0x80152EC4..0x80153200` contains four
functions / 828 bytes, compiled with the unchanged `gcc_2_8_1_g0_split`
profile. Existing SDK packet, vector and matrix types and resident-owned
submission declarations describe every accessed field; no new record type,
inline assembly or compiler flag is needed.

| Function | Bytes | Recovered behavior |
|---|---:|---|
| `0x80152EC4` | 216 | Translate the common FT4/G4 coordinate layout; submit with caller flags and wrapped priority plus one. |
| `0x80152F9C` | 276 | Translate FT4; mode one uses flagged submission, otherwise clear semitransparency and sort at the original priority. |
| `0x801530B0` | 276 | Translate GT4; mode one uses flagged submission at the original priority, otherwise clear semitransparency and sort at wrapped priority plus one. |
| `0x801531C4` | 60 | Initialize identity rotation and translation `(0, 0, 300)`, without writing matrix padding. |

The three packet routines update all four X coordinates before the four Y
coordinates, retaining repeated halfword reads from `D_8015B7F8`. The
eight-byte FT4 and twelve-byte GT4 vertex strides follow the existing SDK
layouts. Flags/mode and priority are unsigned halfwords; the explicit
`(u16)(D_8015B800 + 1)` preserves wraparound before the resident call.
Only mode one selects flagged submission, not every nonzero mode.
The caller at `0x801558F4` also passes a SDK `POLY_G4` to `0x80152EC4`.
Its four XY pairs have the same offsets as `POLY_FT4`; the helper accesses
only those fields before forwarding the raw primitive. Its public parameter
is therefore `void *`, with an internal SDK layout view, rather than an
unjustified FT4-only interface. This caller-backed refinement retains all
216 instruction bytes unchanged.

The initializer at `0x80146258` writes the X, Y and Z halfwords at
`D_8015B7F8 + 0/2/4`; the next priority word is at `0x8015B800`. This
supports the SDK `SVECTOR` view rather than the first trial's two-component
`DVECTOR` prefix view. Neither declaration allocates storage: the vector,
priority and ordering-table pointer remain real preserved module data.
The matrix initializer uses explicit diagonal stores and paired off-diagonal
assignments; translation writes leave the SDK matrix's alignment padding
untouched.

## Matching evidence

The initial direct field-update candidate matched all four instruction
ranges at their exact sizes. Refining the global prefix declaration from
`DVECTOR` to `SVECTOR`, using the independent initialization evidence,
produced the same exact instructions. Both source/header snapshots and
compiler results were recorded under `tmp/packet-probe/`; no failed
candidate was promoted.

A private full-bank link with all nineteen accepted Spanish functions and
this group reproduces all 90,112 bytes and SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
The independent four-function proof reaches 23 real C functions / 4,376 bytes,
retaining 62 provisional assembly boundaries. Production acceptance also
checks every complete terrain copy, all seven configured Spanish images,
and exact compiled-object/linked-ELF function extents and executable-section
ownership. Combining the six accepted color/quad/matrix reuses with these
four new functions reaches 29 C functions / 4,964 bytes, leaving 56
boundaries in assembly. The final batch also includes the later color
helpers and the now-accepted textured-quad pair, reaching 37 C functions /
6,360 bytes with 48 boundaries still assembly. It does not depend on any
unmerged PR or assert full runtime coverage.

## Shared declaration reconciliation

The subsequently accepted French sorting source introduced a two-halfword
array declaration and a G4-only prototype in `drawing_helpers.h`, conflicting
with the Spanish vector and generic primitive declarations. The shared
drawing header now owns one `SVECTOR` declaration and one generic packet
interface; `packet_helpers.h` includes that owner instead of redeclaring them.
French now reuses the complete `packet_helpers.c` group instead of retaining
a second sorting implementation. The accepted shared declaration ownership
is preserved: `drawing_helpers.h` owns the vector, priority and sorting
interfaces, and `packet_helpers.h` owns the matrix initializer. No allocation
or instruction bytes change: all four French functions retain their complete
828 bytes, and the full French image still matches. The preceding declaration
reconciliation also preserved the Spanish and Japanese overlay images.

The later Japanese integration independently selected the removed sorting
source. Its four entries at `0x80160F58..0x80161294` now select the complete
canonical `packet_helpers.c` group as well. No function, address, size,
profile, shared source body or storage declaration changes. The Japanese
bank retains 43 C functions / 9,424 bytes; its full image and seven terrain
copies are reverified after this source-owner substitution.
