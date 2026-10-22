# Paired card-grid coordinate helpers

`CardGrid_GetColumnX` and `CardGrid_GetRowY` name former `func_8002C400`
and `func_8002C484`. Their role is established by independent Library
coordinate writers, not a newly discovered call to either function.

## Independent coordinate evidence

`Library_InitializeScreen` reads `gCardGrid_bCursorColumn`, derives X,
and writes the result to both `motion.rest_x` and `motion.x`. It reads
`gCardGrid_bCursorRow`, derives Y, and writes `rest_y` and `y`.
`Library_StartCursorMotion` independently repeats both calculations for
explicit column/row arguments before writing those target coordinates
and setting up cursor motion.

The mappings exactly match the two helpers:

| Helper | Preserved calculation |
| --- | --- |
| `CardGrid_GetColumnX(value)` | For `value >= 10`, `(value % 10) * 14 + 174`; otherwise `(value % 10) * 14 + 14` |
| `CardGrid_GetRowY(value)` | `q = value / 10`, then `q * 178 + (value - q * 10) * 16 + 14` |

`card_grid.h` independently identifies two side-by-side ten-by-ten
sections. Columns use fourteen-pixel spacing and section origins
`0x0E/0xAE`; rows use sixteen-pixel local spacing, fourteen-pixel origin
and 178-pixel section-row pitch. `Library_DrawCardGrid` uses that same
178-pixel pitch.

The old source happens to spell ten as `DUEL_FIELD_SIDE_ZONE_COUNT`;
`duel_grid.h` confirms that value, while the Library writers use
`CARD_GRID_SECTION_SIDE_LENGTH` or literal ten. The reused constant name
alone was not evidence of a duel-zone purpose. Its token and existing
include remain unchanged in this namespace-only migration.

These are grid/rest target coordinates, not a promise of final
viewport-adjusted screen coordinates. The old static and bounded runtime
no-observed-call evidence stays verbatim in `duel_grid_offsets.c`.
That evidence concerns calls, not whether independent code computes
the same mapping. No new Library caller, original gameplay purpose,
unreachability or unused-code claim is asserted.

## Arithmetic and source contracts

Both APIs retain `s32(s32)`. The definitions keep their original parameter
name, expressions, signed division/remainder and operation order. In
particular, the row helper keeps `value - quotient * 10` rather than a
rewritten remainder expression. No unsigned cast, bounds check, clamp,
lookup table or invalid-coordinate behavior is introduced.

The X branch chooses only two bases. It is not generalized to an
arbitrary number of horizontal sections for arguments at or above 20.
The Y quotient-based section pitch is also unchanged.

The source filename, first-X/second-Y definition order and contiguous
`0x84/0x58` extents stay intact under the named `gcc_2_8_1_g0` profile.
All fourteen native records retain game ownership, matching-C status
and prior notes. The manifests and compiler profiles are unchanged.

## Native entries and compatibility

| Image | Column X | Row Y |
| --- | --- | --- |
| North America | `0x8002C400` | `0x8002C484` |
| Japan | `0x8002C1CC` | `0x8002C250` |
| SLES_039.47 | `0x8002C490` | `0x8002C514` |
| Other PAL images | `0x8002C5D4` | `0x8002C658` |

Canonical exports and numeric compatibility aliases preserve these
entries without moving or regrouping code. The isolated declaration
header does not alter existing grid, duel, SDK or overlay headers.
Existing overlay C/imports and test modules remain unchanged.
No harness, C fixture, probe or case is added; full regional images and
cloud/witness execution remain PR-side validation.
