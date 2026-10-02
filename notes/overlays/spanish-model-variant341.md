# Spanish MODEL headers 341 and 491

Four independently checked Spanish secondary images for models 7 and 552,
stages 7/8, reuse the accepted French wrappers for the shared header-324
webs, fan, draw, spokes and rings bodies, plus the accepted French entry
and its slot-one wrapper. Shared C bodies and declarations are unchanged.
Compilation uses the named
`gcc_2_8_1_g0_split` profile: GCC 2.8.1 and MASPSX 2.81, regardless of
historical compiler comments in the source.

## Images and retained code

The [instance ledger](spanish-model-variant341-instances.csv) records each
independent legal Spanish slice and hash. Compact records 7 and 502 load ten
2,048-byte sectors at `record * 276 + 180/190` into
`0x8013B000/0x8017B000`. Headers are 341/491 and entry is at `+4`.

| Offset range | Bytes | Owner | Direct entry-call path |
|---|---:|---|---|
| `0x4..0xF38` | 3,892 | entry C | loader entry |
| `0xF38..0x1354` | 1,052 | webs C | yes |
| `0x1354..0x17C0` | 1,132 | fan C | yes |
| `0x17C0..0x22F8` | 2,872 | generated assembly | yes |
| `0x22F8..0x26C4` | 972 | draw C | yes |
| `0x26C4..0x29D8` | 788 | spokes C | no |
| `0x29D8..0x2D58` | 896 | rings C | no |

All four complete 20,480-byte images match without masks or instruction
patches. Twenty-four selected, sized compiler-owned functions contribute
34,928 instruction bytes, including all twenty earlier owners /19,360 bytes.
Four companion instances remain explicit generated assembly, totaling
11,488 bytes. Every four-byte header and
8,872-byte suffix has a real raw owner. The suffix beginning at `0x2D58`
remains unclassified; neither it nor the retained helpers is excluded from
the outstanding runtime scope.

## Layout and runtime ownership

The following helper evidence predates the additive entry proof below.

The 123 target-compiled layout constants, 135 retained/web instruction
anchors and 189 retained/fan anchors per image verify the local declarations
and entry accesses. All three arrays have actual read-only storage in a
492-byte section at offsets0/184/328; zero-sized NOTYPE labels are not
treated as storage extents.
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

The single 112-byte fan occupies `0xD04..0xD74`, with eleven eight-byte
points, inner color at88, outer color at92 and size at96. Fourth color
bytes and bytes100..112 remain opaque. Its fixed 36-byte `POLY_G4` at
`0xD90..0xDB4` is initialized by the real `SetPolyG4` at `0x80082EC8`.
Projection coordinate outputs are at packet offsets8/16/24/32; direct
color writes end at byte30. The 256-byte frame contains rotation40..48,
scale48..64, matrix64..96, local-screen matrix96..128, coordinate128..208,
projection result208..212, flag212..216 and saved registers216..256.
The ordering table stays in `s8`. Twelve static calls resolve to nine
distinct resident callees.

The negative-command update preserves context in `s2` and calls the fan
at `0xD54`. Both paths establish `a0 = s2` at `0xD14` or `0xD24`;
the call's delay slot at `0xD58` is a velocity store, not context setup.
One outer record and two pairs of projections draw four quads through the
same packet pointer. Both signed depth and signed projection flag must be
nonnegative, unlike the web's strictly positive gates. Sorting truncates
depth to unsigned16 and retains fourth argument1. Odd frames double scale.

Fan scale changes only when signed substep halfword `0xF40` plus one equals
the actual unsigned descriptor halfword2. In phase0, the original unsigned
`(time << 12) / 50` quotient retains its zero-divisor trap, then uses a
signed clamp4096 and advances to phase1. Other phases subtract `step << 5`
from positive scale and clamp at0. Rotation at `0xF4C` advances
`step << 5` on every call, outside the scale gate; `rot.vz` reads its low
unsigned halfword. Direct fan accesses end at `0xF54`; the larger entry
minimum remains `0xF64`.

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

### Historical helper registration

The [terminal ledger](spanish-model-variant341-attempts.csv) identifies the
ten unchanged wrappers and named profile. Initial independent compilation
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
The accepted web addition contributed four C instances / 4,208 bytes, bringing its
154 configured Spanish images to 870/1,082 C instances / 941,652 bytes.
Accepted MODEL435 spiral coverage was subsequently preserved by a
non-rewriting merge and fresh production/resident validation.

The subsequent fan promotion independently rebuilt both fan wrappers and
all eight retained compiler objects, including the accepted webs. Fresh
combined layout, full-image, raw-behavior and resident-owner proofs retain
both helpers' distinct packet/frame/visibility rules. The existing accepted
`ratan2` binding remains unchanged; the fan does not call it.
Integration starts from accepted master containing the MODEL442 sheets,
not a pending branch, and adds four C instances / 4,528 bytes. Provisional
Spanish totals become 878/1,082 C instances / 950,196 bytes across the
same 154 configured images. Unknown entries, the primary helper and
unclassified tails remain in scope.

## Independently recovered entry

Two freshly compiled slot objects reproduce all four 3,892-byte Spanish
entries without a source, header or profile change. All five earlier
helpers were recompiled. Four complete scratch images contain 24 genuine
C owners /34,928 bytes, four freshly assembled companion owners /11,488
bytes and eight header/suffix storage owners /35,504 bytes. All 718
annotated companion instructions per image agree with retail before
assembly; executable `incbin` does not replace the fallback. The 8,872-byte
suffixes remain unclassified.

The independent entry probe compiles 116 constants into 464 read-only bytes.
Besides canonical SDK and packet layouts, it verifies three 416-byte webs,
the 492-byte companion initialization view at `0x4E0`, one 152-byte sheet,
six 144-byte rings, four 144-byte spokes, one canonical 112-byte fan and
the minimum `0xF64` state. The companion's nine color pairs at `0x144/0x168`,
scale at `0x18C`, halfwords at `0x19C/0x19E` and word at `0x1A0` are
entry-observed; other bytes remain opaque in this view.

Each actual image supplies 203 literal anchors, seven relocated anchors,
ten complete register-write sets and 75 static entry calls. Original `a0`
is captured in `s2` at `0xC`, then in typed root `s6` at `0x14`.
Initialization reuses `s2` but jumps from `0xADC` to the common tail.
Delay-slot-aware dataflow proves only the original capture reaches all four
helper calls. Fan arguments are established at either `0xD14` or `0xD24`;
its delay slot is a direction store, not argument setup. Draw, companion
and webs pass the original pointer in their respective delay slots.
Neither spokes nor rings is claimed directly entry-call reachable.

The 232-byte frame saves incoming `a1` in the caller home at `sp+236..240`.
Projection output occupies `sp+112..128`; direct stack stores do not overlap
it. Six cursor homes advance during initialization, whereas the quad and
extra-GT4 homes remain stable. The untouched `sp+16..80` interval preserves
two unused matrix locals, not recovered live matrix state. Four directly
initialized packet footprints cover paired GT4s and FT4s; the extra GT4
has verified SDK arguments but no claimed direct UV writes.

Every actual command507000 selects the descriptor at `0x2E54`, with parts
`6/10/4`, unsigned substep count two and duration fifty. Web initialization
retains signed arithmetic shifts; fan, ring and spoke coordinates preserve
their unsigned logical shifts. Three webs, one companion, one sheet, six
rings, four spokes and one fan keep their independently measured strides.

Update reads unsigned projected X before the signed-halfword substep reset,
then computes signed Y. A zero-count guard precedes the bottom-tested
signed substep comparison. Substeps zero and one both use part zero with
opposite slot-dependent X offsets, shared Z offsets and Y decrement six;
substep two uses part two. Fan executes every substep. Positive phase gates
draw and companion; draw additionally requires phase below two or substep
zero. Webs execute after the loop when phase is at least two.
Both frame-step calls are retained. The phase-seven fade uses an unsigned
logical half-step and resets to zero, not 64, on transition to phase eight.
Phase two through four returns four, phase six returns one and becomes
seven, and phase eight returns two.

Fresh resident proof establishes all 36 bindings, three matching callers
and both context-pointer storage owners. The entry itself calls 25 distinct
resident functions. Thirteen SDK aliases are independently grounded in
accepted Spanish bindings and actual sized resident owners, then renamed
at unchanged overlay addresses. The accepted-base fast-forward preserves
45 source/declaration/profile/binding fingerprints.

This entry integration starts from accepted Spanish MODEL402 entries,
preserving all 188 image records and prior coverage. It adds four C
instances /15,568 bytes for provisional totals of 1,076/1,272 C instances /
1,369,260 bytes. The companion, unknown suffixes and expanded seven-release
runtime scope remain open. No generated progress report is included.

Local production acceptance passes for all 188 complete Spanish images,
a fresh Spanish resident and a clean North American executable. Final
production ownership retains all 24 C owners, four generated companions
and eight header/suffix owners; all 116 target constants are checked.
The 36 resident callees, three matching callers and both context-pointer
owners are freshly archived before the North American build. All six
repository policies, 62 focused regressions and 1,402 full-suite tests
pass without skips. These local gates do not replace exact-head CI or
maintainer acceptance.
