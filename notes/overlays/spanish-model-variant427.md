# Spanish MODEL427 nine-curtain renderer

Independently recover the 1,296-byte helper at `+0x35BC..+0x3ACC` in
MODEL379 stages 9/10, headers 427/577. The fresh pre-recovery scan covered
5,853 accepted regional C registrations and found no accepted normalized
shape. This is new decompilation, not a French or regional C port.

The first retail-derived candidate was sixteen bytes short. Evaluating
the nonnegative curtain-growth term before the primary scale restored the
1,296-byte extent, leaving four clamp-branch words. Initializing that term
to zero and assigning it only when nonnegative matched every instruction.
The second slot also compiled exactly with `gcc_2_8_1_g0_split`
(GCC 2.8.1 / MASPSX 2.81). No forced registers, padding or new flags.

## Physical inputs and complete inventory

An exhaustive record/stage scan finds exactly two physical images:

| Model / record | Stage / slot | Sector | Header | Command |
|---|---|---:|---:|---:|
| 379 / 329 | 9 / 0 | 91004 | 427 | 593000 |
| 379 / 329 | 10 / 1 | 91014 | 577 | 593000 |

Each image is 20,480 bytes, loaded at `0x8013B000` or `0x8017B000`.
The [instance ledger](spanish-model-variant427-instances.csv) records
both complete-image hashes. The fourteen function instances are:

| Span | Bytes per image | Ownership |
|---|---:|---|
| `+0x4..+0xFB4` | 4,016 | Assembly entry |
| `+0xFB4..+0x157C` | 1,480 | Assembly |
| `+0x157C..+0x1FC4` | 2,632 | Assembly |
| `+0x1FC4..+0x28A0` | 2,268 | Assembly |
| `+0x28A0..+0x2DF8` | 1,368 | Matching C; [six panels](spanish-model-variant427-panels.md) |
| `+0x2DF8..+0x35BC` | 1,988 | Assembly; no observed entry call |
| `+0x35BC..+0x3ACC` | 1,296 | New matching C |

All seven control-flow graphs close on their exact spans in both images.
Four-byte headers and 5,428-byte suffixes retain raw ownership. The
original curtain recovery added two C instances / 2,592 instruction bytes.
The subsequent panel recovery adds two more C instances / 2,736 bytes.
Both images now contain four C instances / 5,328 instruction bytes and
ten retained assembly instances, not fourteen C functions.

## Original context, records and call order

Entry calls the selected helper at `+0xE04`, with `move a0,s5` in its
`+0xE08` delay slot. A reaching-definition walk resolves `s5` solely to
entry `+0xC`, the original incoming context. Initialization reuses that
register on a path which bypasses this call.

The call is gated by shared phases 3 through 5. It occurs **before** the
primary helper called at `+0xE0C`, so it observes primary scales before
that later update.

| Private view | Offset / extent |
|---|---|
| Three 1,264-byte primary records | `+0x0000..+0x0ED0`; scale at record `+0x4EC` |
| Nine 280-byte curtains | `+0x2A78..+0x3450` |
| One reused 52-byte GT4 packet | `+0x3484..+0x34B8` |
| Three padded word-vector origins | `+0x3690..+0x36C0` |
| Step / draw gate / fade intensity | `+0x3748 / +0x3770 / +0x3780` |
| Inner / outer RGBA | `+0x37A8 / +0x37AC` |
| Shared angle / phase | `+0x37B0 / +0x37C4` |

The curtain view reuses the verified `ModelVariantCurtain`: two
seventeen-point arrays, a signed scale and a count word. Entry establishes
all nine records, initializes the first row with radius 384 and the
second with radius 512 at Y=-300, and configures the actual GT4 packet.
The first three scales are zero; the remaining six are staggered from
zero down to -6,826. Counts are initialized but not updated by this helper.

The `0x37C8` state extent is a partial view, not allocation capacity.
The verified resident context pointers `0x80136000/0x80176000` leave that
view outside both corresponding load banks. Every adjacent-point strip
stays within seventeen vertices; color and projection destinations stay
inside the single initialized packet.

## Rendering and progression

Only a positive draw-gate word enables drawing. Curtain index modulo three
selects a primary scale and origin. The first three curtains use uniform
primary scale. The remaining six add nonnegative curtain scale to X/Z,
leaving Y at primary scale. Rotation is zero.

At phase five or above, each RGB channel is multiplied by signed intensity,
divided by 1,024 with signed truncation and narrowed to a byte. Each channel
then subtracts its byte value times curtain scale divided by 8,192, with
another byte narrowing. Negative curtain scales can therefore increase and
wrap colors; they are not clamped for this calculation.

The packet's first two corners use inner RGB and the last two use outer
RGB. Sixteen strips reuse that packet. Sorting requires nonnegative depth
and flag; priority narrows to sixteen bits.

Progression is outside the draw guard. If curtain scale is below 8,192,
add `step * 128`; if the result reaches 8,192, subtract 8,192 once.
This is neither a completion clamp nor an unrestricted modulo operation.
The shared angle advances by `step * 80` after all nine records. The
original initial `rsin(1300)` call is retained although its result is unused.

Five regressions cover exhaustive physical inputs and exact CFGs,
27 target-compiled layout constants, original-context provenance and phase
gates, all input/final C/assembly/raw owners, eight external and two local
jump relocations per C object, all 34 resident bindings, actual loader and
context-storage owners, and terminal source/header fingerprints.
The [attempt ledger](spanish-model-variant427-attempts.csv) preserves four
scratch comparisons and two complete-image terminal records.
