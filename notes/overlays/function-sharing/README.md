# Cross-release overlay function sharing

**France, Spain, Germany and Italy share every catalogued code-load image,
byte-for-byte at the same load addresses.** Each has 3,752 physical code-load
slots and the same 3,587 distinct complete images. This comparison includes
the archive basename, sector, length and destination, not just an unordered
bag of hashes.

These findings derive from the [seven-release census](../function-inventory/README.md),
whose metadata cutoff is `288c4f22830f58d99eb61abe0805f30276e0a055`.
They do not incorporate later C promotions. The reports are a research and
reuse aid, **not a new matching denominator or proof of semantic equivalence**.

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
