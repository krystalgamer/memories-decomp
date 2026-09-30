# French MODEL headers 341 and 491

Four secondary images for models 7 and 552, stages 7/8, reuse the accepted
header-324 draw, spokes and rings C bodies. Six three-line canonical
wrappers rename only the function symbols; shared bodies, headers and the
named `gcc_2_8_1_g0_split` profile are unchanged. The profile uses
GCC 2.8.1 and MASPSX 2.81.

Final canonical preflight and production links reproduce all four complete
20,480-byte images. All 188 preceding registrations are preserved, and the
full gate reproduces all 192 configured French images.

## Loader and function boundaries

The compact archive records are 7 and 502. Stages 7/8 load ten 2,048-byte
sectors at `record * 276 + 180/190`, into `0x8013B000/0x8017B000`.
Entry is at `+4`. The [instance ledger](french-model-variant341-instances.csv)
records the actual header words 341/491, selected commands, offsets and
complete hashes. Every slice and command was independently read from the
checksum-verified legal French archive.

| Offset range | Bytes | Owner | Entry-call reachable |
|---|---:|---|---|
| `0x4..0xF38` | 3,892 | generated assembly | yes |
| `0xF38..0x1354` | 1,052 | generated assembly | yes |
| `0x1354..0x17C0` | 1,132 | generated assembly | yes |
| `0x17C0..0x22F8` | 2,872 | generated assembly | yes |
| `0x22F8..0x26C4` | 972 | draw C | yes |
| `0x26C4..0x29D8` | 788 | spokes C | no |
| `0x29D8..0x2D58` | 896 | rings C | no |

Strict control-flow walks cover all 28 function spans with one terminal
return per span. Entry calls `0xF38`, `0x1354`, `0x17C0` and `0x22F8`.
Spokes and rings are retained code; no direct entry-call execution path
is claimed. Each four-byte header and 8,872-byte suffix has one sized raw
owner. The suffix at `0x2D58..0x5000` remains unclassified, not proven
wholly data or non-code.

Only the three newly accepted header-324 kernels were used for discovery,
not a repetition of the earlier broad kernel census. Actual symbol-renamed
C object links reproduce the complete images without masks, relocation
patches or instruction edits. The
[terminal attempt ledger](french-model-variant341-attempts.csv) records
all six canonical wrapper fingerprints. An initial canonical-link probe
duplicated an absolute object-path prefix during script substitution;
the corrected exact-path substitution reproduced all four images before
registration. This was a scratch harness failure, not a C mismatch.

## Accessed layouts

French entry captures `a0 -> s2 -> s6` and saves pointers to the records.
The draw helper receives `a0 = s2`. One 152-byte quad-ring record occupies
`+0x6CC..+0x764`, followed by six 144-byte rings ending at `+0xAC4` and
four 144-byte spoke records ending at `+0xD04`.
Initialization strides, loop bounds and adjacent pointer formation agree.

The quad-ring has four groups of four `SVECTOR` points at 0/32/64/96,
two colors at 128/132 and scale at 136. Both line-ring views have eight
inner vectors at 0 and eight outer vectors at 64, with angle/count at
136/140. Their color fields differ: 128 for rings, 132 for spokes.
The draw quad is at `+0xDE8`; the shared line packet is at `+0xEB0`.
Draw reads base at `+0xED8`, velocity at `+0xEEC`, frame at `+0xF18`,
step at `+0xF24`, time at `+0xF48` and state at `+0xF50`.
The spokes scale input is the quad-ring's `+0x88` field, at context
`+0x754`; an inherited source comment is not evidence for a different
address.

Twenty-six retail instruction anchors independently check capture,
pointer offsets, initialized fields, strides, counts and the draw call.
Forty-six target-compiled constants check local and SDK types. The first
proof incorrectly expected `GsGLINE.r1` at 16; the local SDK declaration,
compiled layout and retail byte stores independently establish offset
15, with `r0` at 12 and total size 20. The rejected scratch proof is
preserved; no SDK declaration was changed.

The table begins at module `+0x2E54`, with a 20-byte stride indexed by
initial command modulo 1,000. Every selected descriptor lies within the
single owning suffix; no duplicate descriptor storage is introduced.

## Context ownership and limits

The matching resident initializer supplies fixed pointers
`0x80136000/0x80176000` through `D_80010024/28` and slot `field_DEC`.
The matching controller passes that field to secondary module `+4`,
first with the selected command modulo 1,000 and then with update `-1`.
The initializer, controller and transfer-phase function all have sized
matching C owners whose bytes were checked against the fresh resident.

Direct entry accesses establish a minimum context extent of `0xF64`
(3,940 bytes); the partial draw view ends at `0xF54` (3,924 bytes).
Neither is an allocation-capacity declaration. The selected 96-sector
model, two-sector primary and ten-sector secondary load ranges do not
overlap that minimum view. Whole-game lifetime isolation and every
primary-context write remain outside this evidence. No new backing
object is allocated by these views.

## Exactness and scope

Preflight verifies all 81,920 unmasked image bytes, with 12 sized C
owners, 16 explicit fallback-function owners and eight raw owners.
The new C contribution is 10,624 instruction bytes. All 36 distinct
resident callees and three caller/loader owners agree with a fresh exact
French resident whose SHA-256 is
`57ecdfb9a9e1faf8b342fe7c7304c23723810861f3ab2fa3bef9eb27b5146b44`.
Production retains the four unmatched functions per image as generated
assembly, not as C or an opaque raw prefix.

The fixture checks archive records, canonical sources, all function
spans/calls, raw extents, instruction anchors, selected descriptors and
context/load separation. Aggregate progress assertions are updated with
registration; project-wide report regeneration remains separate.
Configured totals become 192 images, 762/1,343 C instances and 695,124
C instruction bytes. They do not establish exhaustive runtime coverage.

Final acceptance includes the clean French resident gate, production ELF
checks of every new owner, all 36 resident callees and three caller owners,
and recompilation of all 46 layout constants. All 94 French MODEL variant
regressions and 47 progress/global-usage regressions pass, together with
metadata, basic-type and G32/PSXLONG policy checks.
