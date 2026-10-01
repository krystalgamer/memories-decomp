# Spanish MODEL headers 341 and 491

Four independently checked Spanish secondary images for models 7 and 552,
stages 7/8, reuse the accepted French wrappers for the shared header-324
webs, draw, spokes and rings bodies. The eight wrappers, shared C bodies and
declarations are unchanged. Compilation uses the named
`gcc_2_8_1_g0_split` profile: GCC 2.8.1 and MASPSX 2.81, regardless of
historical compiler comments in the source.

## Images and retained code

The [instance ledger](spanish-model-variant341-instances.csv) records each
independent legal Spanish slice and hash. Compact records 7 and 502 load ten
2,048-byte sectors at `record * 276 + 180/190` into
`0x8013B000/0x8017B000`. Headers are 341/491 and entry is at `+4`.

| Offset range | Bytes | Owner | Direct entry-call path |
|---|---:|---|---|
| `0x4..0xF38` | 3,892 | generated assembly | yes |
| `0xF38..0x1354` | 1,052 | webs C | yes |
| `0x1354..0x17C0` | 1,132 | generated assembly | yes |
| `0x17C0..0x22F8` | 2,872 | generated assembly | yes |
| `0x22F8..0x26C4` | 972 | draw C | yes |
| `0x26C4..0x29D8` | 788 | spokes C | no |
| `0x29D8..0x2D58` | 896 | rings C | no |

All four complete 20,480-byte images match without masks or instruction
patches. Sixteen selected, sized compiler-owned functions contribute
14,832 instruction bytes. Twelve function instances remain explicit
generated assembly, totaling 31,584 bytes. Every four-byte header and
8,872-byte suffix has a real raw owner. The suffix beginning at `0x2D58`
remains unclassified; neither it nor the retained helpers is excluded from
the outstanding runtime scope.

## Layout and runtime ownership

Eighty-two target-compiled layout constants and 135 Spanish instruction
anchors per image verify the local declarations and entry accesses.
The entry captures `a0 -> s2 -> s6`, forms records at `0x6CC`, `0x764`
and `0xAC4`, and passes the original context to the draw helper.
One 152-byte quad-ring ends at `0x764`; six 144-byte rings end at `0xAC4`;
four 144-byte spoke records end at `0xD04`.
The line-ring color is at 128, while the spoke color is at 132.
Spokes read scale at `0x6CC + 0x88 = 0x754`; an inherited source comment
is not evidence for a different address.

The draw quad is at `0xDE8`, the shared `GsGLINE` at `0xEB0`, base at
`0xED8`, velocity at `0xEEC`, frame at `0xF18`, step at `0xF24`, time at
`0xF48` and state at `0xF50`. `GsGLINE` is 20 bytes with colors at
12 and 15. The partial draw view ends at `0xF54`; the entry's halfword
store at `0xF62` establishes a minimum context extent of `0xF64`.
These are accessed extents, not allocation-capacity declarations.

Three 416-byte narrow web records occupy `0..0x4E0`. Each contains
two four-by-six grids of eight-byte vectors at offsets 0 and 192,
color at 384, scale at 404 and done at 408; ranges 388..404 and 412..416
remain opaque. The web helper reuses the fixed line at `0xEB0`, writing
through byte 17 of its 20-byte storage. Its 296-byte frame contains
the coordinate at 128..208, projection depth at 208..212, flag at
212..216 and ordering-table pointer at 216..220, below saved registers.

The negative-command entry update calls webs at `0xDD0` when phase is
at least two, passing the unchanged context. The helper's retained
phase-below-two path still reads word translation at `0xED8/EDC/EE0`;
the other path reads signed halfwords at `0xEE4/EE6/EE8`. Four discarded
`ratan2` results remain in the exact body. Nonpositive scale produces
a zero matrix scale, not an early traversal exit. Above 6,144, color
fade uses signed division by 2,048 and byte truncation, without a new clamp.
Sorting requires both signed depth and signed projection flag to be
strictly positive; the sort depth is then truncated to unsigned 16 bits.

Growth adds `step << 8`. Reaching 8,192 in state five clamps scale and
sets done; other states subtract 8,192. The third record can set state
six using the local constant `done = 1`: this is not an aggregation of
all three records' completion flags.

Actual metadata requests are 507000 for both models and slots. The
matching controller passes command modulo 1,000, selecting the first
20-byte descriptor at module `0x2E54`, entirely within the single suffix
owner. Updates use command `-1`. No duplicate descriptor object is added.

All 36 distinct resident callees, three matching initializer/controller/
loader owners and both context-pointer storage owners were checked against
selected input objects, linked symbols/sections and the exact Spanish
resident. Its SHA-256 is
`b0fefd88b6510f49af4f01e6180e40371652b7ceaa5f31dcb938c942316fc790`.
The independently verified 372-byte SDK owner at `0x80089928` is named
`ratan2` in this family's bindings and four symbol maps; its resident
address and SDK classification are unchanged.
The selected `spanish_raw_80010000.o` data subsection supplies
`0x80010024/28`, holding `0x80136000/0x80176000`. It lies within a mixed
executable `.main` output section; absolute labels or output flags alone
were not treated as data ownership.
The minimum context view does not overlap the selected 96-sector model,
two-sector primary or ten-sector secondary loads. Whole-game lifetime
isolation and every primary-context write remain outside this evidence.

## Experiment and integration record

The [terminal ledger](spanish-model-variant341-attempts.csv) identifies the
eight unchanged wrappers and named profile. Initial independent compilation
matched every selected Spanish helper. Accepted upstream additions of
unrelated ribbon and curtain types changed a shared header fingerprint.
The dependency guard stopped the initial integration; all candidates and
complete-image owner proofs were rerun against each updated dependency,
including after reconciliation with accepted family-402 coverage.

One layout-proof harness initially required a sized constant symbol.
The real compiler emits a zero-sized NOTYPE label in a complete 184-byte
read-only section. The corrected proof checks that actual storage and all
46 values; no SDK type, source body or padding was changed.
Rejected and successful scratch proofs remain local.

Spanish regressions reuse the existing family fixture while reading the
Spanish archive, checksums, inventory and ledgers. They retain strict
function-boundary walks, source selection, fallback bindings, descriptor
bounds and context-access checks. French defaults remain unchanged.

Family-402 coverage was accepted while the initial three-helper change was being validated.
A non-rewriting merge preserves all 64 accepted modules and adds only these
four images, bringing configured Spanish totals to 68 images, 310/408 C
instances and 242,876 C instruction bytes. These totals are not exhaustive
Spanish runtime or seven-release completion claims.

The later web promotion independently rebuilt both web wrappers and all
six retained compiler objects, checked all four complete images and real
resident/caller/context owners, and compiled 82 layout constants into
328 bytes of read-only storage. The original 46 values remain unchanged.
Accepted MODEL435 ribbons and unrelated North American additions were
preserved before integration; none changed these proof dependencies.
The provisional branch adds four C instances / 4,208 bytes, bringing its
154 configured Spanish images to 844/1,082 C instances / 873,012 bytes.
Pending MODEL435 spiral work is not counted.
