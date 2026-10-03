# French MODEL headers 400 and 550

Four independent ten-sector images in French `MODEL.MRG` contain matched
five-ribbon and seven-origin sheet helpers. Models 189 and 258 use stages 7/8 and load slots
`8013B000` / `8017B000`. The actual headers are 400/550; the
[instance inventory](french-model-variant400-instances.csv) records their
physical sectors, complete hashes, commands and descriptor selectors.

| Image offset | Bytes per image | Owner |
| --- | ---: | --- |
| `0` | 4 | Raw header |
| `4` | 2,692 | Entry assembly |
| `A88` | 3,504 | Ribbon C |
| `1838` | 1,408 | Sheet C |
| `1DB8` | 2,448 | Column helper assembly |
| `2748` | 10,424 | Unclassified raw suffix |

Current source ownership accounts for eight C owners / 19,648 bytes, eight
assembly owners / 20,560 bytes, and eight raw header/suffix owners / 41,712
bytes, covering all 81,920 bytes.
The C definitions have their real sized `.text` input sections; no absolute
function alias, masked comparison or favorable alternative offset establishes
ownership. Every remaining function and suffix byte is preserved.

The original sheet-only production rebuild matched the complete French executable and all
287 configured French overlays. The 283 previous registrations are unchanged.
Each new production image was independently relinked with a map; mapped ELFs
equal the production ELFs, and their selected input definitions establish all
24 owners at that checkpoint: four C, twelve assembly, and eight raw.
The 33 resident owners were reverified after rebuilding.
The original promotion passed repository metadata/type/attempt checks and
57 generic overlay tests; those were not a dedicated MODEL400 fixture.

## Independent target evidence

All sixteen measured function spans have closed internal branches and jumps,
one final return, and resolved direct calls. The entry directly calls the three
local helpers. Thirty-three resident callees were checked against the complete
French retail executable, final ELF definitions and actual selected input
objects. The existing French header-475 linker bindings cover these same
addresses and are reused without modification. The sheet helper uses nine
of those imports.

The metadata commands are `566000` for model 189 and `566001` for model 258.
They select records zero and one of the 32-byte descriptor table at image
`2844`. The helper consumes timing words at descriptor offsets `C`, `10` and
`14`; the first two are 76/240 and 116/320 respectively. This is an observed
selector set, not a general command-domain or allocation guarantee.

Seventy-two entry-context layout constants were independently compiled before
candidate work. Five 880-byte node views start at zero; seven 152-byte sheet
views start at `1130`; two 820-byte column views start at `1558`. The minimum
observed context extends through `20A4`, which is not an allocation-size claim.
Eighteen further target-compiled assertions support canonical sheet reuse:
`ModelVariantSheet` has stride 152, four vector arrays at `0/20/40/60`,
outer/inner RGB at `80/84`, and size at `88`. Node positions are at `2C8`.
The helper's GT4 is the second packet, at context `1F4C`.

## Source and behavior

The accepted NA458 sheet helper supplies structural comparison evidence, not
a matching body or compiler policy. The French implementation reuses the
existing `ModelVariantSheet`, SDK declarations and context-access macros.
It introduces no new types, headers or regional conditionals. Compilation
uses the authoritative `gcc_2_8_1_g0_split` profile: GCC 2.8.1 / MASPSX 2.81,
not the historical compiler comment in the common header.

The helper visits the world position, five node positions, and the destination,
drawing four textured quads at each origin. The node pointer advances only for
the five node-origin passes. Both matrix-scaling operations, the rotation
matrix read/set calls, and the depth adjustment `depth * 8 / 10` are retained.
Submission requires nonnegative adjusted depth and projection flags.

Size updates remain inside the origin loop and preserve their distinct state
transitions, signed size comparisons, unsigned descriptor arithmetic, and
unsigned timing comparison. No extra input clamp, divide guard, allocation
claim or stable-frame-step assumption is introduced. Original address-based
function names remain unchanged.

The stored descriptor pointer at context `205C` is loaded through
`*(u8 *G32 *)(work + 0x205C)` before all four timing-word reads. This preserves
four-byte storage and zero-extension on supported native 64-bit builds instead
of reconstructing the pointer from a signed `s32`. The correction leaves both
slots' C instruction bytes identical to the frozen exact objects. A clean
French resident/all-287-overlay rebuild and four independent complete-image
relinks reverified the 24 image owners and 33 resident owners.

`tools/project/tests/test_french_model_variant400.py` pins the four manifests,
matching source/profile/size, assembly and raw extents, descriptor selectors,
latest terminal fingerprints, guest-pointer loads, and sheet layout/behavior
bounds. Native pointer and target-layout checks require their respective
compilers; retail descriptor/CFG checks require the legal archive. French
aggregate expectations at the sheet-only checkpoint were 287 images, 1,575 matching C instances out of 1,849,
and 2,104,108 C instruction bytes. The header-475 fixture selects its own
instance inventory rather than treating shared SDK bindings as family identity.

## Experiments and limits

The [attempt ledger](french-model-variant400-attempts.csv) retains four paired
entry candidates and twelve paired column candidates, all nonexact. Those entry
frames were 136 rather than 200; a frame-size difference alone does not justify
invented matrices or padding. The closest-length column pointer traversal is
2,460 bytes / frame 504 rather than 2,448 / 496. Indexed nodes recover frame
496 but produce 2,556 bytes. Equivalent casts, declaration ordering, alias
removal and pointer-postincrement forms did not resolve the induction mismatch.
Three alternative named profiles also remained nonexact.

The first structured sheet candidate matches both 1,408-byte functions with
272-byte frames. Replacing its temporary context declarations with the existing
canonical sheet view remains exact. Both versions were separately linked into
all four complete images with actual C and raw input owners. Only the canonical
source and its slot-one symbol wrapper are promoted.

An initial column layout reader incorrectly relied on an old-GCC constant
array's zero-sized ELF symbol; the corrected reader validates the full section.
That aborted harness run compiled no column candidate. A whole-image harness
also required the supported streaming hash API before any image build began.
Neither tool failure is presented as a source/compiler mismatch.

These results do not exclude executable material from the suffix, prove every
possible entry into the bank, or complete this family in C. Entry and column
helpers remain assembly fallbacks.

## Directly called ribbon helper

The entry calls the 3,504-byte helper at `A88`. Five 880-byte ribbon records
begin at context zero. Their seventeen-element `SVECTOR` arrays start at
`0/110`, four-byte screen arrays at `88/198`, angles at `CC`, widths at
`1DC`, RGB bytes at `220`, position at `2C8`, delta at `2D8`, depths at
`2E8`, and signed-halfword X/Y offsets at `32C/34E`. The intervening bytes
stay opaque; this accessed view is not an allocation-capacity claim.

Each screen has equivalent canonical `DVECTOR` and `PSXLONG` views.
Projection deltas and widths use signed coordinates; drawing uses signed
X halves and arithmetic high-halfword extraction for Y. This expresses
measured accesses, not an assertion about original declarations. Both
loop counters remain signed halfwords. Projection flags are the observed
five rows of seventeen words on the stack.

Entry anchors at image `30`, `2B0/2B4`, and `308/30C/310` establish the
two FT4 packets beginning at context `1F80`, including their 40-byte
stride and `SetPolyFT4` calls. The draw loop initializes its packet
cursor before the first condition and resets it in the increment clause;
odd segment indices advance to the second packet. Moving that reset
inside the body folds a different address and changes four instructions.

Geometry retains both yaw/pitch calls, the five-way angular phase,
seventeen points, endpoint bend suppression, and the 1,300-unit wave
step. The two geometry paths preserve their different coordinate
expressions. One matrix-scaling call remains. The terminal projection
uses the predecessor/current pair; other points use current/next.
Signed screen deltas supply angles and signed X differences supply width.

Rendering preserves the distinct first, last, and interior segment
endpoints, stored color, and nonnegative depth/flag checks. Begin/end
indices grow by unsigned `step * 3 / 2`, with the original clamps and
phase changes. Animation words advance by `step * 400` and `step * 384`
even when geometry is skipped. The later phase-ten arm remains after
the earlier `>= 6` arm in source and machine code. Removing that apparent
redundancy deletes 64 retail instruction bytes; it is deliberately retained
without claiming that the later arm is normally reachable.

Eight historical paired reconstructions remain in the append-only ledger.
The fresh control produces 3,484 bytes / frame 656 with 456 differing words
when the missing final words are counted. Coordinate/packed screen views
recover the exact length/frame with only the four packet-reset differences.
The loop-boundary reset recovers both complete slot texts exactly under
`gcc_2_8_1_g0_split`, GCC 2.8.1 / MASPSX 2.81. The rejected phase-arm removal
produces 3,440 bytes and 114 differences. The exact source is restored;
slot one changes only its function symbol.

Independent proof verifies all four complete 20-KiB images, 24 actual
selected input owners, sixteen closed function spans, 42 target-compiled
layout constants, packet initializer anchors, and 33 actual resident
input owners against complete retail bodies. That candidate-only proof
uses raw fallback for the other functions rather than proving production's
accepted sheet C owners. A linker-import parsing error was preserved and
corrected before successful whole-image proof. The resident was rebuilt
first so actual input objects, not merely a surviving ELF/map, were checked.

The subsequent clean production build matches the complete French executable
and all 293 configured overlays. Four independent production relinks reproduce
the complete production ELFs and identify all 24 selected owners with the
counts above. All eight C texts equal their frozen exact objects, including
the accepted sheets, whose source files remain unchanged. The 33 resident
input owners and their full retail bodies are reverified. The 83 focused
regressions finish without failures; one optional native-pointer check is
skipped because Clang is unavailable. Target-compiled layout checks run.
Metadata, basic types, external attempts, and matching-source contracts pass.

The registry remains at 293 images. Configured French totals after this
addition are 1,605 matching C instances out of 1,883 functions and
2,160,388 C instruction bytes. No progress-report surfaces are refreshed
by this matching change. Every unclassified suffix remains in scope;
French #6460 and exhaustive runtime coverage remain open.
