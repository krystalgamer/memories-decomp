# Duel-effect packet helpers

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
French sorting uses the same X/Y fields and a local G4 view of the generic
argument. No source group, manifest entry, allocation or instruction changes:
all four French sorting functions retain their complete 828 bytes, and the
full French and Spanish overlay images still match. Japanese overlay matching
also preserves the newly accepted regional additions.
