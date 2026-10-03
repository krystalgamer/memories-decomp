# French MODEL headers 400 and 550

Four independent ten-sector images in French `MODEL.MRG` contain the matched
seven-origin sheet helper. Models 189 and 258 use stages 7/8 and load slots
`8013B000` / `8017B000`. The actual headers are 400/550; the
[instance inventory](french-model-variant400-instances.csv) records their
physical sectors, complete hashes, commands and descriptor selectors.

| Image offset | Bytes per image | Owner |
| --- | ---: | --- |
| `0` | 4 | Raw header |
| `4` | 2,692 | Entry assembly |
| `A88` | 3,504 | First helper assembly |
| `1838` | 1,408 | Sheet C |
| `1DB8` | 2,448 | Column helper assembly |
| `2748` | 10,424 | Unclassified raw suffix |

The four complete scratch links reproduce all 81,920 bytes. Their actual
input definitions account for four C owners / 5,632 bytes, twelve assembly
owners / 34,576 bytes, and eight raw header/suffix owners / 41,712 bytes.
The C definitions have their real sized `.text` input sections; no absolute
function alias, masked comparison or favorable alternative offset establishes
ownership. Every remaining function and suffix byte is preserved.

The production rebuild also matches the complete French executable and all
287 configured French overlays. The 283 previous registrations are unchanged.
Each new production image was independently relinked with a map; mapped ELFs
equal the production ELFs, and their selected input definitions establish all
24 owners listed above. The 33 resident owners were reverified after rebuilding.
Repository metadata/type/attempt checks and 57 focused overlay tests pass.

## Independent target evidence

All sixteen measured function spans have closed internal branches and jumps,
one final return, and resolved direct calls. The entry directly calls the three
local helpers. Thirty-three resident callees were checked against the complete
French retail executable, final ELF definitions and actual selected input
objects. The existing French header-475 linker bindings cover these same
addresses and are reused without modification. The matched helper uses nine
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

## Experiments and limits

The [attempt ledger](french-model-variant400-attempts.csv) retains four paired
entry candidates and twelve paired column candidates, all nonexact. Entry
frames remain 136 rather than 200; the apparent 64-byte gap does not justify
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
possible entry into the bank, or complete this family in C. Entry, first helper
and column helper remain assembly fallbacks.
