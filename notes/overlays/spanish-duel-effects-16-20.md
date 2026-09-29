# Spanish duel effects 16 and 20

The complete `func_80151558` interval, `0x80151558..0x80152048`, is
2,800 bytes of C in `src/overlays/duel_effects/effect_16.c`. Its existing
GCC 2.8.1 / MASPSX 2.81 profile is `gcc_2_8_1_g0_split`. The dispatcher
uses the same lifecycle for effects 16 and 20; effect 20 maps nonnegative
phases to 1 and preserves negative update phases.

## Exact recovery

All 700 instructions match. Before registration, the candidate plus the
75 accepted Spanish C entries independently reproduced the entire
90,112-byte bank with SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
The independent base is `ab446402b432ab41e7f7592b69bf4b8b2e05660e`.
This adds one function / 2,800 bytes: 76/85 bank C functions / 48,324 bytes,
or 200/209 configured Spanish C instances / 104,176 bytes.
Nine bank functions remain generated assembly. Other pending Spanish
matches and other regional registrations are not included.

The first complete candidate produced 2,856 bytes with a 336-byte frame
instead of the retail 320-byte frame. Canonical `copyVector` usage removed
an extra live address and packet-pointer spill. Separate initialization
and update loop-variable lifetimes restore the two retail loop slots.
Parenthesized views into flat particle storage preserve 40-byte group
strides without introducing out-of-bounds inner-array indexing.

The final endpoint stores use the current level index under `k == 4`.
Replacing that index with literal 4 folds an address differently, removes
one instruction and shifts the rest of the function. A temporary endpoint
pointer restored the size but still differed in five words. The selected
current-index expression matches exactly without an extra pointer or
compiler flag. The tick bitmask and origin sign comparison preserve the
observed masking and branch direction.

Candidate sources, headers, exact compiler output, linked instruction
comparisons and mismatch reasons are retained locally under
`tmp/effect16-proof/`; the terminal whole-bank receipt is under
`tmp/effect-16-full/`. No external types, toolchain claims or source bodies
were imported.

## Layout and ownership

The private work layout is recovered from instruction offsets:

| Region | Offset | Extent or stride |
|---|---:|---:|
| Configuration pointer | `0x0000` | 4 bytes |
| Five levels, four paths, five columns | `0x0004` | 100 `SVECTOR`s; strides 160/40/8 |
| Five particle clouds | `0x0324` | 32 vectors per column |
| Three radial rings | `0x0824` | 32 vectors per ring |
| Particle positions | `0x0B24` | 160-vector storage |
| Particle velocities | `0x1024` | 160-vector storage |
| Five origins | `0x1524` | 8-byte stride |
| Activation, age and scale arrays | `0x154C`, `0x1556`, `0x1560` | Five halfwords each |
| Hits, active count, tick, cross-line stage | `0x156A..0x1570` | Four halfwords |
| Primary and secondary colors | `0x1572`, `0x1586` | Five `CVECTOR`s each |

The accessed extent ends at `0x159A`; the target's pointer alignment rounds
the declared structure to `0x159C`. Twenty-nine target-GCC constants verify
these offsets, the configuration offsets, and canonical matrix/vector
sizes. This is not a claim that every reserved particle vector is used.

Both initialization and updates use overlapping 32-vector windows with
starts five vectors apart. Ordinary disjoint `[5][32]` indexing changes
retail behavior. Flat 160-element storage and explicit five-vector starts
retain the overlap while keeping every access within its actual array.

The configuration at `D_8015AEF4` is 36 bytes, not 28: three radius
halfwords occupy `0x1C..0x20` and ring height occupies `0x22`. It ends
exactly at the separate effect-2 table at `0x8015AF18`. The initial scale
vector at `D_80146198` is 16 bytes `(4096,4096,4096,0)` and ends at
`D_801461A8`, which belongs to effect 23. An earlier coarse generated owner
spanned later scale vectors; that span is not an array owned by this effect.

The configuration, initial vector, canonical 84-byte texture union and
8-byte global offset retain section-backed generated-data ownership and
retail bytes. Texture pair 14 uses offsets `0x38/0x3A`. No C allocation,
absolute data alias or source-local extern is introduced.

## Preserved lifecycle

Phases 0 and 1 initialize the same configuration and five columns. Higher
phases activate the alternate cross-line lifecycle; updates complete that
path after its counter exceeds 180. The ordinary update activates another
column every eight ticks, capped at five.

Four paths grow to level 4; the final level keeps its calculated height
while zeroing x/z. The first terminal update increments the hit count and
writes the canonical resident request's byte `0x1D`. Ages saturate at 4.
Particle positions accumulate their overlapping velocities; two scaled
ring layers retain the scale carried between columns. Primary and
secondary colors fade by 31 and 15, with completion requiring both final
column colors to be black. Whole-color copies retain their fourth byte.

Production verification covers complete Spanish images, all seven actual
terrain copies, accepted C ownership, target layouts, real data owners,
18 executable overlay callees, 151 duel regressions and clean resident
matching. Whole-image equality alone is not the ownership proof.
The two model helpers retain their real 12-byte resident C extents;
`memset`, `rand`, `PushMatrix` and `PopMatrix` remain SDK code.
The established resident compatibility bindings at `0x8009C5FC` and
`0x8009C600` remain unchanged. Their storage is in the resident
`.image_after_viewport` input-image region, not an allocation in this
overlay; the resident linker still represents these compatibility names
as absolute aliases. This is distinct from the four section-backed
overlay data objects verified above.
Boot, MODEL/SU loads and opaque overworld fragments remain open; neither
this match nor the original configured-module list completes that scope.

## Accepted effect 11 integration

Additive rebase onto accepted `e9295667f` preserves all 76 accepted Spanish
entries, including every accepted lifecycle, real owner and test.
The previously reviewed lifecycle source/header files remain byte-identical.
The combined branch has 77/85 bank C functions / 53,372 bytes,
8 explicit assembly boundaries and 201/209 configured C
instances / 109,224 bytes. Complete production Spanish images,
linked ownership, metadata and duel regressions are checked on this head.

## Accepted effect 17 integration

Additive rebase onto accepted `99a615a8d` preserves all 77 accepted Spanish
entries, including every accepted lifecycle, real owner and test.
The previously reviewed implementation source/header files remain byte-identical.
The combined branch has 78/85 bank C functions / 58,472 bytes,
7 explicit assembly boundaries and 202/209 configured C
instances / 114,324 bytes. Complete production Spanish images,
linked ownership, metadata and duel regressions are checked on this head.

## Accepted effects 7/13 integration

Additive rebase onto accepted `c61e79ad3` preserves all 79 accepted Spanish
entries, including every accepted lifecycle, real owner and test.
The previously reviewed implementation source/header files remain byte-identical.
The combined branch has 80/85 bank C functions / 64,064 bytes,
5 explicit assembly boundaries and 204/209 configured C
instances / 119,916 bytes. Complete production Spanish images,
linked ownership, metadata and duel regressions are checked on this head.

## Independent French registration

The complete French `func_80151558` independently reproduces all 700
instructions using this same source and header without modification.
The selected profile remains `gcc_2_8_1_g0_split`. The original rejected
French allocation experiments are not promoted; the accepted shared
implementation supplies the canonical vector operations and loop lifetimes.

French `D_80146198` and `D_8015AEF4` are independently verified real
generated-data definitions of 16 and 36 bytes. Their extents stop at
`D_801461A8` and `D_8015AF18`; no neighboring effect storage is claimed.
The existing texture union and global offset retain their canonical
84-byte and eight-byte extents. The overlapping particle windows, lifecycle
phase behavior and resident helper bindings are unchanged.

Together with the [number renderer](duel-effect-number-renderer.md),
this adds two French functions / 3,824 bytes over accepted `3ed6b0f15`,
preserving all 77 accepted entries. The complete preflight bank,
79 linked C extents, 45 whole input objects and six unique generated-data
owners match independently. Production acceptance repeats all configured
French images and terrain copies, input/final ownership, focused
regressions, repository metadata policies and clean resident matching.
The resulting 79/85 bank functions do not include other pending PRs or
claim exhaustive runtime coverage.

The follow-up integration of accepted `273e05386` preserves its nine updated
French PAL-wrapper/contour bindings and shared drawing header. The new
effect-16 and renderer bodies are unchanged, while the combined bank now
has 46 complete C input objects. All seven French images and ownership
checks remain exact; 171 duel and 16 progress regressions pass.

The additive integration of accepted `0832a2be4` also retains complete
French effect 24 and the North American registrations without changing
any shared C or header. Together with the renderer, the French branch has
80/85 bank functions / 60,808 C bytes and 204/209 configured instances /
116,660 C bytes. Its 47 complete bank input objects and six unique
generated-data owners are checked against the complete French image.

The following integration of accepted French effect 11 at `4329554d9`
preserves all 79 accepted entries, unchanged shared sources and the six
pair-specific data owners. The independent pair now reaches 81/85 bank
functions / 65,856 C bytes, 205/209 configured instances / 121,708 bytes
and 48 complete bank C objects, with four bank boundaries still assembly.
