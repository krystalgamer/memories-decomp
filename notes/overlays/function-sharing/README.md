# Cross-release overlay function sharing

**France, Spain, Germany and Italy share every catalogued code-load image,
byte-for-byte at the same load addresses.** Each has 3,752 physical code-load
slots and the same 3,587 distinct complete images. This comparison includes
the archive basename, sector, length and destination, not just an unordered
bag of hashes.

These findings derive from the [seven-release census](../function-inventory/README.md),
whose metadata cutoff is `288c4f22830f58d99eb61abe0805f30276e0a055`.
The original tables do not incorporate later C promotions; the focused
unmatched-lead analysis below has its own explicit cutoff. The reports are a
research and reuse aid, **not a new matching denominator or proof of semantic
equivalence**.

## Distinct byte bodies

| Release or group | Distinct instruction-byte bodies |
| --- | ---: |
| France / Spain / Germany / Italy: one shared pool | 1,555 |
| English PAL | 1,551 |
| North America | 1,528 |
| Japan | 1,523 |
| Across all seven, deduplicated | 6,044 |

The aligned comparison contains 39,389 sites. Of its 6,044 distinct bodies,
1,227 occur at registered boundaries and 4,817 are candidate-only.
Excluding bodies of 16 bytes or less still leaves 31,032 sites but only
6,036 bodies: about 80.5% of those sites repeat another exact body.
This threshold excludes tiny bodies, not every possible semantic no-op.

The original census contained 39,394 eligible sites and 6,047 byte bodies.
Different releases had different amounts of registered boundary evidence.
For example, apparent localized-PAL body-set differences involved the
intro/credits and duel banks despite the complete images being identical.
The comparison therefore:

1. Shares registered spans only between images with the same **complete image
   SHA-256 and load address**.
2. Rejects contradictory or overlapping cross-release registered spans.
3. Retains the target's existing status, or records `peer_registered` with
   **blank target status** when borrowing boundary evidence.
4. Preserves displaced/overlapping candidate comparisons in
   `boundary-adjustments.csv`, rather than mistaking them for code revisions.

There are 2,984 adjusted candidate comparisons, mostly replacements by
registered evidence, **not 2,984 invalid functions**. The final distinct-body
count drops by three. Candidate-to-candidate overlaps outside registered
spans can remain.

## Existing C donors

The following sites are not marked C in the frozen target metadata but have
an exact-byte body already represented by matching C elsewhere. Bodies of
16 bytes or less are excluded.

| Target | Sites | Distinct donor bodies | Boundary evidence for those sites |
| --- | ---: | ---: | --- |
| Spain | 211 | 47 | 4 local registered, 91 peer registered, 116 candidates |
| Italy | 1,252 | 364 | 1,136 peer registered, 116 candidates |
| Germany | 1,252 | 364 | 1,136 peer registered, 116 candidates |
| France | 116 | 29 | 116 candidates |
| North America | 33 | 32 | 33 candidates |
| Japan | 25 | 25 | 25 candidates |
| English PAL | 0 | 0 | No additional nontrivial exact-byte donor at this cutoff |

Across releases these are **2,889 target sites and 421 distinct donor bodies**.
Target site counts add across releases; distinct donor-body counts do not.
Zero exact-byte leads does not mean there is no source-level reuse.
Sites refer to deduplicated images, not every physical archive alias.

`c-donor-sites.csv` supplies the target region/image/offset, boundary evidence,
unchanged local status and a concrete donor region/image/offset/name.
Filter it by the region being worked on. Join `(region, image_id)` to that
region's census `images.csv` to recover **all** physical archive instances;
an image can be selected by more than one model/stage/command.
The donor name is a locator, not a proposed semantic rename.

For the current Spanish-first campaign, the 47 Spanish donor bodies are a
useful bounded starting point, followed by the Italian and German leads.
Coordinate regional ownership before taking a group. Do not independently
recover thousands of copies without checking for an existing family donor.

## Address differences and structural search

Ignoring only the destination fields of direct MIPS `J`/`JAL` instructions
reduces the aligned body set to **2,484 comparison clusters**.
All other instruction words remain unchanged. This can hide calls to
different functions, so it does **not** establish relocation-only differences.

The existing `find_siblings.normalize_instruction` helper produces
**1,402 instruction-shape clusters**. It discards register identities and
many constants, among other details. These clusters are deliberately loose
search aids, not equivalence classes or a count of remaining functions.
The exact bodies belonging to every cluster are retained in its CSV.

Do not use regional address-based function names as equivalence keys.
They can differ for otherwise identical resident code. Nor does exact
callee-byte equality prove equivalent transitive dependencies or globals.

## Focused unmatched leads

A targeted follow-up at accepted commit
`dec60b27a4fa643abc70a09b33a7f1ee08631253` checked MODEL families 88 and 136
against all **5,062 currently registered overlay C function spans** across
the seven releases. Neither has an exact-byte or same-instruction-shape C
donor. This refreshes only these two leads, not the older census or donor
tables above. It does not rule out differently compiled semantic counterparts.

| Spanish family label | Entry bytes | Sites per release | Sites across seven releases | Main recovery obstacle |
| --- | ---: | ---: | ---: | --- |
| 88 / slot-1 header 218 | 2,384 | 14 | 98 | Terminal branch ordering and delay-slot selection; strongest private candidate differs in five instruction words. |
| 136 / slot-1 header 266 | 2,584 | 20 | 140 | Stack/local lifetimes and loop register allocation; no exact C body yet. |

Each family has eight exact byte-body variants in the existing `bodies.csv`:
two slots times four regional body groups. These are **two recovery leads,
not 238 independent algorithms**. Site counts refer to distinct images, not
every physical archive alias. All entries start at image offset `0x4`.
The instruction-shape fingerprints locating the complete groups are:

- Family 88: `5b28b8a586fed7a239e3a83605246f3f83b960248ac91e2e7a2c1995718fcd71`.
- Family 136: `69cbffd6d99559eb08ec669b06131e729a9afeee2a7486e3274cb75ed4cef36d`.

The corresponding Spanish model IDs are 31, 290, 295, 408, 501, 518 and 531
for family 88, and 3, 16, 17, 25, 96, 155, 369, 436, 705 and 717 for family
136. These labels and locators are not semantic renames.

### Family 88: staggered, model-part-driven textured particles

The entry initializes point/countdown storage, captures positions from model
parts at staggered intervals, and otherwise moves points toward cosine-derived
heights and side-dependent depth. It scales and projects textured quads and
selects among eight 32-pixel texture columns. Negative countdowns still take
the arithmetic-update path; they are rejected later by the packet-submission
gate, so an early dead-particle skip would not preserve the observed behavior.

After advancing elapsed time, the entry returns 0 before the configured
period, 4 until duration plus period, then 1 while setting the completion
byte on the first terminal visit and 2 on subsequent terminal visits.
The completion byte at context `+0x140C` overlaps the low byte of the second
texture-lookup result written during initialization. Keep this aliasing;
do not silently replace it with an independently allocated flag.

Fresh word comparisons of the seven releases show that, **within each slot**,
only direct `J`/`JAL` destination fields differ. Other instruction words,
including the terminal sequence, agree. This is stronger than shape equality
and makes one exact recovery a promising basis for the other releases, but
does not prove equivalence of the differently addressed resident callees.

### Family 136: rotating quad copies, screen flash and animation phases

This entry emits sixteen rotated copies for each active effect instance,
with growing scale and fading color, plus a fading screen-sized flat quad.
It also changes animation state through three phases. Its notification timing
differs from family 88: after the start threshold it reports 1 when the
notification byte is zero, sets that byte, and reports 0 on later active
visits until a phase transition resets the notification. It reports 2 at
lifetime plus interval times count. These lifecycle protocols must not be
collapsed into one helper.

Besides direct jump/call destinations, exactly one instruction differs between
PAL and North American/Japanese bodies in either slot: image `+0x3E0` loads
256 for PAL and 240 for North America/Japan. Following the stores, and executing
the isolated rectangle-setup instructions in all fourteen regional/slot bodies,
confirms a **320 x 256 PAL versus 320 x 240 NTSC flash rectangle**. This is
an actual coordinate difference, not a relocation or proof of GPU output.
The geometry, resident callees and animation reachability still need their
own promotion evidence.

The current registration scan also found eight unrelated 2,584-byte C spans
using `french_model_variant/variant87_entry.c` and its slot-1 wrapper.
Their body hashes and instruction shapes differ: **equal size is not an
existing donor**. Conversely, the lack of an exact or same-shape donor is not
proof of semantic uniqueness.

Prioritize family 88 for the nearer exact recovery and retain family 136 as
the next distinct lead. Neither analysis promotes C, excludes unknown suffixes,
proves context allocation bounds, or changes any matching denominator.

## Required proof before promotion

Even complete overlay-image equality does not establish equivalent resident
callees, global data, ABI, context allocation or runtime ownership.
Every port still needs independent named-profile compilation, selected
compiler-object and linked-symbol size/section checks, real ASM/data owners,
layout and loader/callee proof, and full-image exact matching.
Preserve SDK/CRT ownership distinctions and all unclassified suffixes.

The 4,957 unresolved census sites and instruction-shaped data-load fragments
are excluded from these counts. Undiscovered leaves, indirect control flow,
dynamic loads and unknown archive regions remain possible. There is no final
semantic unique-function count yet, and none of these reports promotes C.

## Reports and reproduction

| File | Purpose |
| --- | --- |
| `bodies.csv` | Exact-body groups, byte sizes, regions, site/provenance counts, representatives and both heuristic fingerprints. |
| `region-pairs.csv` | Pairwise exact-body, complete-image and physical-load-map comparisons. |
| `c-donor-sites.csv` | Nontrivial target sites and existing C donors; target status is never inherited. |
| `boundary-adjustments.csv` | Original candidate comparisons displaced by registered-span evidence. |
| `jump-target-clusters.csv` | Clusters masking direct jump/call destinations only. |
| `instruction-shape-clusters.csv` | Looser clusters using the existing sibling-search normalization. |
| `summary.json` | Counts, completion marker and census/tool/helper/archive/CSV fingerprints. |

With legally obtained retail archives, run from the repository root:

```sh
tools/environments/python/bin/python tools/project/overlay_function_sharing.py \
  --output tmp/overlay-function-sharing-fresh
tools/environments/python/bin/python -m unittest discover \
  -s tools/project/tests -p test_overlay_function_sharing.py
```

The output directory must be fresh. `--census` may select another complete
census with at least two supported releases; the default is the accepted
seven-release snapshot. Processing is sequential. The tool verifies every
source CSV and retail archive, then reads and verifies a full representative
image and function body for every exact group before calculating instruction
fingerprints. Reports contain derived metadata and hashes, not payload bytes.
They have no timestamps. Reproduce this snapshot using its recorded census
and tool/helper fingerprints; do not mix generations silently.
