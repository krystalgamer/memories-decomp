# French MODEL373 point groups

Model707 stages7/8 load two distinct 20KiB images from the French
`MODEL.MRG`, with headers373/523 at sectors167712/167722 and load addresses
`0x8013B000`/`0x8017B000`. The instance ledger preserves each image hash.
This change matches the directly entry-called helpers at offset`0x1EE4`,
`func_8013CEE4` and `func_8017CEE4`: 856 instruction bytes per slot.

## Ownership and retained scope

Each image retains these exact spans:

| Offset | Bytes | Owner |
| --- | ---: | --- |
| `0x0` | 4 | Raw header |
| `0x4` | 4268 | Entry assembly |
| `0x10B0` | 1724 | Assembly |
| `0x176C` | 1912 | Assembly |
| `0x1EE4` | 856 | Matching point-group C |
| `0x223C` | 1352 | Assembly |
| `0x2784` | 1764 | Assembly |
| `0x2E68` | 1412 | Assembly |
| `0x33EC` | 2820 | Assembly |
| `0x3EF0` | 4368 | Unclassified raw suffix |

Across both images, twenty actual inputs cover all40,960 bytes:
two C owners/1,712 bytes, fourteen assembly owners/30,504 bytes, and
four raw owners/8,744 bytes. The entry directly calls offsets`0x10B0`,
`0x176C`, and`0x1EE4`. The seven other function spans per image remain
assembly; the suffix is neither padding nor excluded game code.

The dedicated resident-binding file contains34 independently verified
function addresses. Accepted MODEL402 bindings supply their existing
canonical spellings except `GsGetActiveBuff`, supplied by accepted
MODEL476 bindings. Neither existing file alone covers all observed
calls. No accepted binding file or implementation changes.

## Independently measured view

The helper traverses three180-byte records at context`0xD48`.
Each accesses sixteen `SVECTOR` points, a rotation at record`0xA0`, and
a signed scale at`0xA8`. Other gaps remain opaque.

Entry instructions at image`0x7C`, `0xA8`, `0x528`, and`0x52C` establish
the `POLY_FT4` at context`0x2364` and call `SetPolyFT4` on that packet.
The helper reads signed target halfwords at`0x23D0`, view-direction words
at`0x23EC`, a signed frame step at`0x2410`, and phase at`0x2484`.
The partial`0x2488` view is not an allocation-size or capacity claim.

Retail reserves sixteen unaccessed stack bytes between the coordinate
and color locals. The source preserves this measured gap as opaque
`u8` storage, following accepted French MODEL476's declaration structure
in `variant459_curtains.c`, without guessing a semantic object type or
adding executed stores. The record and context types were independently
recovered, not imported from the structural lead.

## Preserved behavior

Positive-scale groups retain the `rsin` call even though its result is
unused. The initial `ratan2` call is likewise retained. Light color is
`(192,192,192)` below scale5120, then fades by
`(scale-5120)*192/1024`. The unused dark-color stores remain present.

After rotation, scaling, and the coordinate/light-matrix setup, each
point is projected separately. The shared FT4 packet receives a
16-by-16 screen square centered on that projection. Sorting requires
nonnegative depth and projection flag; there is no extra upper-depth
cutoff.

Scale below6144 grows by `step*192`. Crossing the limit subtracts6144
once, advances rotation X by `step*192` and Z by `step*400`, then clamps
scale to6144 when phase is at least seven. Negative scales advance
without drawing. A terminal6144 scale is still positive and follows the
zero-color drawing path on subsequent calls.

## Matching evidence

The first independently declared candidate matches both entire856-byte
functions with272-byte frames using the authoritative
`gcc_2_8_1_g0_split` profile, GCC2.8.1 and MASPSX2.81.
The append-only attempt ledger records exact text and promoted
fingerprints. Slot one changes only the function symbol.

Independent scratch proof verifies both complete retail images,
twenty actual linked input owners,22 target-compiled layout constants,
all sixteen closed function spans, and34 actual resident input-function
owners against complete retail bodies. The proof base is independently
accepted`494241c9`; integration starts from accepted`d51b71b3`, whose
intervening US-only sheet match leaves all relevant French inputs,
types, profiles, and build tools unchanged.

Before publication, the verified change was reconciled on independently
accepted`c6d22987`, preserving all291 accepted registrations, including
MODEL410 rings and their fixture fixes. The authoritative combined
inventories contain293 images,1,589 C instances out of1,883 functions,
and2,126,300 C instruction bytes. All promoted source fingerprints remain
unchanged.

Clean final production acceptance matches the complete French resident
and all293 configured overlays. Both new production ELFs
independently relink with maps; all twenty selected input owners and34
resident input owners are reverified. Both C object texts equal the
frozen exact candidates. All60 focused tests pass without skips; full
discovery passes1,643 tests with five skips, and metadata passes.

French #6460 stays open; these measured functions do not establish
exhaustive coverage.
