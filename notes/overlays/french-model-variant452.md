# French MODEL452 gradient-line grids and sheets

## Scope and ownership

The six independently hashed 20,480-byte images are MODEL1, MODEL360 and
MODEL550, stages 9/10 in both runtime slots. Their headers are 452/602 and their
load addresses are `0x8013B000`/`0x8017B000`. The instance ledger records the
compact archive record, loader command, sector and complete image SHA-256.
The archive SHA-256 is
`0c3f90cf4a3b4776d1188054a6cd5d89c5bc364215dac15c4730ef74edbc8da3`.

| Image range | Bytes | Ownership |
| --- | ---: | --- |
| `0x0000..0x0004` | 4 | Raw module header |
| `0x0004..0x10E0` | 4316 | Entry, generated assembly |
| `0x10E0..0x151C` | 1084 | Matching four-quad sheet C helper |
| `0x151C..0x2010` | 2804 | Entry-called helper, generated assembly |
| `0x2010..0x23E4` | 980 | Matching gradient-line C helper |
| `0x23E4..0x5000` | 11292 | Raw, unclassified suffix |

All 24 active function instances have closed instruction-level control-flow
graphs, including delay slots, without holes or indirect calls. Entry calls
the three helpers at image offsets `0xF58`, `0xF60` and `0xF78`, passing its
context in each call's delay slot. The 34 external direct-call destinations
are resident inventory starts and are declared in the family linker file.

The suffix contains additional code-like material but is outside these entry
CFGs. Preserving it as raw storage is not an SDK, dead-code or other ownership
exclusion. This work does not finish MODEL452 or the French overlay scope.

## Independently measured view

Each of three grid records has stride `0x260`: two `SVECTOR[6][6]` endpoint
arrays at `0` and `0x120`, RGB bytes at `0x240`, and signed count at `0x254`.
The minimum context view has `GsGLINE` at `0x32CC`, translation at `0x3308`,
two delta integers at `0x331C`/`0x3320`, a screen delta at `0x3338`, a vector
delta at `0x333C`, step at `0x3360`, and phase at `0x33A8`.
Its `0x33AC` extent is only the minimum accessed by this helper, not evidence
of allocation capacity or a complete description of the entry's state.
Unknown bytes remain uninterpreted.

The SDK declarations come from the repository's native Psy-Q headers.
Target-compiler layout assertions cover the structures and measured offsets.
The source uses three signed 16-bit loop indices, matching the explicit
sign-extension sequences, and retains four retail `ratan2` calls whose
results are unused.

For each record the helper clamps the scale to zero below zero, keeps the
original color through 2048, and otherwise multiplies it by
`(4096 - scale) / 2048` with the observed signed-division rounding.
It projects 36 endpoint pairs per grid, repeating the pair for
`RotTransPers4`, and sorts a gradient line only when depth and flags are
nonnegative. The second endpoint's RGB is zero.

Counts below 4096 advance by `step << 7`. On crossing the threshold they
wrap if phase is below four, otherwise clamp to 4096. A count that remains
below the threshold clears the local completion flag; an already complete
count does not clear it. After the third record, phase four advances to five
only if the flag is still set. The two branch destinations at helper-relative
`0x2DC` and `0x300` distinguish this behavior from the initial incorrect
outer-`else` reading and have explicit regression assertions.

## Matching evidence

GCC 2.8.1 and MASPSX 2.81 use the existing `gcc_2_8_1_g0_split` profile.
The attempt ledger retains four paired experiments:

| Experiment | Bytes | Frame | Differing words per slot |
| --- | ---: | ---: | ---: |
| Single clamped scale value | 968 | 296 | 205 |
| Separate scale and fade lifetimes | 972 | 296 | 204 |
| Branch-local assignments | 980 | 296 | 2 |
| Correct completion branch placement | 980 | 296 | 0 |

Both slot bodies are compared at their actual link addresses, including
absolute local jumps, rather than with relocation bytes masked. All three
complete images per slot are independently hashed and have identical helper
bodies within that slot. Slot-zero helper SHA-256:
`1855be463b1e995524d5d5c871d67322e6b9b8379a881f4cc74e367fbf805dd4`;
slot-one:
`b3f9c892287204f7f77620addcb12fc8bdd90c59a0830ab0375dc0c1697264cc`.

The shared C body and symbol-only slot wrapper are selected separately in
each image manifest. Full production-image equality and sized linked C
ownership, not isolated text matching alone, are required for terminal
`matched` ledger records.

For the initial gradient-line integration, clean production builds matched the complete French resident and all 323
configured French overlays. Independent map-producing relinks equal the six
production ELFs. Their 36 unique input owners account for 122,880 bytes:
six C functions / 5,880 bytes, eighteen assembly functions / 49,224 bytes,
and twelve raw regions / 67,776 bytes. The selected C objects have the same
text as the frozen exact candidates. All 34 resident callees were also
checked against their sized linked functions, input objects and retail
bodies. The prior 317 registrations are unchanged.

## Sheet helper

The first independently measured sheet candidate matches all 1,084 bytes
with a 256-byte frame in each of the six linked instances. The slot wrapper
only renames `func_8013C0E0` to `func_8017C0E0`. The accepted
`ModelVariantSheet` record is reused unchanged: four corner arrays at
`0`, `0x20`, `0x40` and `0x60`, outer/inner colors at `0x80`/`0x84`, scale at
`0x88`, and stride `0x98`.

The measured single record is at context `0x2AC8`, and the reused `POLY_GT4`
packet at `0x3204`. The state view retains translation at `0x3308`, flags at
`0x3354`, unsigned elapsed time at `0x3358`, step at `0x3360`, a four-byte
guest timing pointer at `0x3374`, a still address-named transition field at
`0x3394`, and phase at `0x33A8`. This remains a minimum view, not proof of
allocation capacity. The unsigned timing fields used by the helper are at
descriptor offsets `0x1C`, `0x20`, `0x2C` and `0x30`.

The four quads share one packet. Their first three corners use the inner
color; their fourth uses the outer color. The helper retains the two-part
matrix pipeline (`GsGetLs`/`GsSetLsMatrix`, then `ReadRotMatrix`, rotation,
scale and `SetRotMatrix`) and signed `depth * 8 / 10` before sorting.
On odd flag parity, scale gets an additional signed eighth.
Growth/fade divisions are unsigned, matching the retail `divu` instructions.
The 4096/8192 clamps, phase transitions and `0x3394` progression are preserved.
Target-compiler assertions cover all 26 measured record/view/SDK properties.

The first production link failed because `GsSortPoly`, `ReadRotMatrix` and
`SetRotMatrix` were present only under address-based import aliases. Their
already-verified canonical SDK names now replace those aliases consistently
in the family linker file and all six Splat symbol maps; the physical
destinations remain `0x800842A8`, `0x800872A8` and `0x80087738`.
The ledger retains that integration failure without changing the exact C.

The sheet follow-up changed no physical registration, resident import address,
shared SDK declaration or accepted grid-line source. At that checkpoint the
entry and `0x151C` renderer still used generated assembly; the entire suffix
remained raw and unclassified. Nonexact renderer experiments were not part
of the sheet integration.

After correcting the import aliases, the clean French resident and all
323 configured overlay images match. Independent mapped relinks reproduce
all six production ELFs. Their 36 unique input owners now account for
twelve C functions / 12,384 bytes, twelve assembly functions / 42,720 bytes,
and twelve raw regions / 67,776 bytes. Both selected C objects in every
image equal their frozen exact candidates. All 34 resident import
destinations retain their verified sized executable functions and unique
input-object definitions, matching retail. The terminal sheet ledger rows
were recorded only after these full-image and ownership checks.

## Sixteen five-point streamers

The `0x151C` helper is recovered as 2,804 bytes with a 632-byte frame in
both slots, independently checked against all six complete image targets.
Sixteen records at context `0x720` have stride `0x130`. The five-point
spines at record offsets `0x28` and `0x78`, packed projections at `0x50`
and `0xA0`, angles at `0x64`, widths at `0xB4`, color rows at `0xC8` and
`0xDC`, depths at `0xF4`, and halfword screen offsets at `0x108`/`0x112`
are separately measured. Opaque prefixes and tails are not classified.
The renderer reuses the accepted sheet at `0x2AC8`; its own GT4 packet is
at `0x31D0`. The view ends at phase `0x33A8`, not an allocation boundary.

The recovered pipeline generates spherical five-point spines, applies the
parity-dependent sheet width, projects original and displaced endpoints,
then draws both sides of four segments using one GT4 packet. Point four
uses the fixed pair three/four. Negative depths and local projection flags
are actually cleared before each submission, as the retail stores require.
Radius and rotation advance according to phase; uncertain angle fields
remain address-named.

Fifteen paired experiments distinguish the important source structures.
Plain unused products and reused scalar assignments lose the initial
multiply pairs. The already accepted header-428/425 spiral and Exodia
petals negative-product tests retain those pairs while their assignments
are eliminated. This supplies the existing template without artificial
memory stores, volatile values, forced registers or fabricated dependencies.
The named no-CSE-follow-jumps and no-strength-reduce probes remain in the
ledger; the exact result uses the unchanged default split profile.

The terminal path also keeps a separate record-plus-16 word-column view,
as in the accepted header-423 band pattern. Only its packed coordinates,
depth, angle and width fields use that view; halfword screen offsets still
use the unshifted record's point-four indices. The two actual endpoint
pointers are established before the word-column pointer, matching the
retail invariant setup. This recovers the final three instructions without
inventing additional storage or changing the point data.

The first production link exposed three further address-only SDK aliases:
`rcos`, `rsin` and `RotTransPers`. Their canonical names replace the aliases
in the family linker file and all six symbol maps, retaining the verified
addresses `0x800866F8`, `0x80086628` and `0x80087868`. The failed link is
recorded separately from the fifteen source/profile experiments.

The follow-up preserves both accepted helpers, all physical registrations,
all 34 resident import addresses and the complete unclassified suffix. The
4,316-byte entry remains assembly. Neither this helper nor inventoried
coverage establishes French runtime-code completion.

After correcting these aliases, the clean French resident and all 323
configured overlays match. Independent mapped relinks reproduce all six
production ELFs, and all eighteen selected C object texts equal their
frozen exact candidates. The 36 unique input owners account for eighteen C
functions / 29,208 bytes, six assembly functions / 25,896 bytes, and twelve
raw regions / 67,776 bytes. All 34 resident imports retain exact sized
executable bodies and unique selected input definitions. The two terminal
renderer ledger rows were appended only after this proof, bringing the
family ledger to 48 rows. The accepted report-base change was README-only;
every clean-gate build input remained unchanged.
