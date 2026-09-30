# Spanish MODEL family 87/217

Models 140 and 584 load this family in stages 9/10, at `0x8013B000` and
`0x8017B000`. All four independent ten-sector images have one directly
reachable 2,584-byte entry at offset `4`. Their sectors, headers and hashes
are recorded in [the instance ledger](spanish-model-variant87-instances.csv).

The Spanish manifests reuse the accepted
[`variant87_entry.c`](../../src/overlays/french_model_variant/variant87_entry.c)
and its existing slot-one wrapper without modifying or copying the body.
The source directory is its historical home, not a claim that regional
portability is automatic. Both slots were independently compiled under the
named GCC 2.8.1 / MASPSX 2.81 `gcc_2_8_1_g0_split` profile and compared with
the legal Spanish archive. All four complete images match separately.
This adds four configured C instances / 10,336 instruction bytes.

## Preserved boundaries

The entry occupies `0x4..0xA1C`; every instruction in that interval is
reachable by direct control flow, with one return and no unresolved indirect
jump. The four-byte header and **all 17,892 bytes at `0xA1C..0x5000`** retain
real generated storage. The suffix remains unclassified. A single known
entry does not prove that no other runtime entry points exist.

The selected input object and final executable symbol both own the complete
2,584-byte function. The header and suffix have their own real input/final
data owners; an absolute configuration alias alone is not storage ownership.
All 21 distinct resident callees are verified against the selected Spanish
input objects, sized function symbols and retail instruction bytes.

## Layout and runtime evidence

Thirty-five target-compiled size/offset values establish the local and
canonical SDK layouts. The configuration stride is 22 bytes, with two
three-byte colors, part/count bytes, and seven signed halfwords. The normal
loader metadata supplies request 17001, selecting configuration one at
image offset `0xA32` through the resident's modulo-1000 dispatch.
This is a verified load path, not a general array-capacity declaration.

The entry initializes 26 canonical `SVECTOR`s and randomized destination
positions. It projects the 26 points into local coordinate/depth arrays,
draws a fan of `POLY_G3` triangles during growth/hold, then draws fading
`GsBOXF` points. The destination span `0xD4..0x1D4` ends at the frame field;
normal descriptors fit their writes within that span. The partial state
size of 476 bytes is not caller allocation proof.

This is **not family 136**, despite its coincidentally equal entry size.
That family has different geometry, configuration stride and lifecycle.
Its unfinished candidate is not counted here.

The [Spanish attempt ledger](spanish-model-variant87-attempts.csv) records
each independently compiled terminal slot result; fingerprints identify the
selected source file. The source's earlier recovery history remains in the
[French ledger](french-model-variant87-attempts.csv), rather than being
duplicated or presented as new Spanish experiments.
