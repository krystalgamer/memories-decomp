# French MODEL variant 157/287

Four complete 20,480-byte images contain the same two physical-load forms of
the closed 4,180-byte entry: models 131 and 145, stages 9/10, headers 157/287.
Commands 88000/88001 select the two independently decoded descriptors.
This adds 16,720 matching-C instruction bytes and 64 source-owned literal
bytes. It does not establish exhaustive MODEL coverage.

## Ownership and evidence

| Image offset | Bytes | Owner |
| --- | ---: | --- |
| `0x0000` | 4 | Raw module header |
| `0x0004` | 4,180 | C entry |
| `0x1058` | 16 | C unit VECTOR |
| `0x1068` | 28 | Raw GsIMAGE view |
| `0x1084` | 16,252 | Unclassified suffix |

All four full images were relinked without masks, using eight actual C
entry/literal contributions and twelve disjoint raw owners. The 27 resident
callee bodies were checked against the byte-identical French resident ELF.
The 65,008 suffix bytes remain unclassified, including descriptor data;
interpreting a descriptor does not establish ownership of the whole suffix.

Native slot entry hashes:

- `0x8013B004`: `5318a5df4a6f4c98ceacc7298c38b82bbcfcda296ef2a6ca441728cc47296f9d`
- `0x8017B004`: `9c782c320da2a7a8ee63b03a8de321d55cb38a838ae303c1b9a6fac2e50395cb`

The local GCC 2.8.1 / MASPSX 2.81 `gcc_2_8_1_g0_split` profile remains
authoritative. The companion attempt ledger preserves 21 paired experiments
and two canonical records. One experiment used the existing named
no-CSE-follow-jumps profile; it did not match and is not used by this source.
No reference types, flags, inline assembly, forced registers, artificial
stores, or fake dependencies were used.

## State and behavior

The 28-byte descriptor has ten byte fields followed by nine signed halfwords.
Native byte loads at offsets 6/7/8 establish trail RGB, not packed halfwords.
The selected radii exceed twice the sprite half-size, and the selected
movement, ring, flash, and spacing divisors are positive.

The `0x820` state contains an origin, 33 ring points, 32 starting points, and
192 history vectors, six per particle. A flat history view permits the
constructor's continuous traversal without crossing C subarray boundaries.
The signed start-vector pad stores a circular index modulo six. Constructor
copies preserve xyz only; other vector padding remains untouched.

The constructor samples radial starts, fills their histories, uploads one
texture and clears elapsed/completion/toggle state. Before the descriptor
delay, the opposing slot supplies the origin with Y forced to zero.

The ring alternates scale, ramps and fades its captured RGB, projects its
center plus 33 ring vertices, and submits 32 fan triangles twice when depth
and native flag checks permit. The first flags value depends on the current
projection flag; it is not an independently initialized per-triangle flag.
The second submission chooses its blend flag from the narrowed red byte.

The main particle pass and six-age history pass preserve signed16 position
narrowing, native Y clipping, asymmetric 63/64 UV endpoints, and projection
without `MulMatrix2`. History capture and falling movement happen even when
the main primitive is rejected. Two screen-sized flashes use the native
separation of twelve frames plus 32 particle spacings.

After elapsed advances, the routine returns zero before the delayed ring
phase, four before the spacing span completes, one once at completion and
zero on subsequent calls, then two at the final ring-duration-plus-twelve
threshold. These differing thresholds are intentional.

## Matching structure

Direct array indexing lets the compiler recover native ring traversal.
Independent constructor angle and history-index lifetimes recover the
post-random angle calculation. Resolving history before committing captured
trail colors preserves scheduling. A branch-local ring phase and separate
terminal spacing/additional-duration values preserve the final arithmetic.
The final entries have the native 712-byte frame and exact literal placement.

The initial read-only preparation sentence claiming no prior family scratch
was template wording: read-only native research and an uncompiled draft
preceded the independent worker. There was no duplicate compiled campaign.
