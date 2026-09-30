# Spanish duel-bank curve completion

The final inventoried Spanish duel-bank function, `func_8014FABC`, now selects
the accepted shared [`bolt_vertices.c`](../../src/overlays/duel_effects/bolt_vertices.c)
body unchanged. Its 836 bytes at image `0x9ABC..0x9E00` and 80-byte stack
frame independently match under the named GCC 2.8.1 / MASPSX 2.81
`gcc_2_8_1_g0_split` profile.

The complete 90,112-byte bank and all seven legal archive copies at sectors
7193, 7433, 7673, 7913, 8153, 8393 and 8633 preserve SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
No compiler instruction array, register pinning, function alias override,
source-body edit or wholesale image replacement is involved.

All **85 inventoried functions / 81,804 instruction bytes** now have real
matching C owners. The separate 228-byte ritual compiler table and all other
bank data retain their existing storage and layout. Completing this bank does
not complete Spanish boot, MODEL/SU, secondary loads, unknown suffixes or the
expanded seven-release campaign.

## Independent evidence

The helper takes four unsigned halfwords and a vertex pointer. It initializes
the first point, then generates the remaining jagged-column points with
`rand` and SDK `csin`, not `rsin`. Calls embedded in the multiply's conditional
expression preserve the observed early sign extension; index-first pointer
arithmetic preserves the target address calculation. The unchanged shared
source already matches the North American and French banks, but its Spanish
function bytes and complete bank copies were checked independently.

Both resident callees have actual selected input objects and sized final
function owners whose bytes match the Spanish retail executable:
`csin` at `0x80086B38` (312 bytes) and `rand` at `0x8008F708` (48 bytes).
Twelve target-compiled layout values verify the primitive widths, canonical
eight-byte `SVECTOR`, its component offsets, and the existing callers' paths.
Effects 13 and 17 each pass eight points in a 64-byte path within their
48-row arrays. Their actual bank call sites are independently located.

The [terminal reuse record](spanish-duel-curve-attempts.csv) fingerprints the
shared source and utility header together. Historical failed curve candidates
remain historical evidence; they were not replayed or relabeled as successful.
The earlier 8,216-byte North American report increase concerned effect 22,
which was already matching in Spain, not this 836-byte helper.
