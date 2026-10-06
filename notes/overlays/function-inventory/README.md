# Seven-release overlay function census

This is a derived research snapshot of the accepted metadata at
`288c4f22830f58d99eb61abe0805f30276e0a055`, after Spanish MODEL421.
It inventories loader-defined images and possible function starts across
Europe, France, Germany, Italy, Japan, Spain and North America. It also accounts
for every remaining byte of their MODEL, SU and WA archives.
It does **not** establish every function boundary, prove every candidate is
executed, or change the authoritative matching denominator.

## Snapshot results

| Release | Physical load instances | Unique loaded images | Registered boundaries | Closed-flow candidates | Unresolved candidates |
| --- | ---: | ---: | ---: | ---: | ---: |
| Europe | 5,001 | 4,832 | 209 | 5,350 | 705 |
| France | 5,001 | 4,836 | 1,538 | 4,156 | 708 |
| Germany | 5,001 | 4,836 | 124 | 5,570 | 710 |
| Italy | 5,001 | 4,836 | 124 | 5,570 | 710 |
| Japan | 4,992 | 4,831 | 182 | 5,346 | 714 |
| Spain | 5,001 | 4,836 | 1,389 | 4,305 | 708 |
| North America | 4,992 | 4,831 | 1,583 | 3,948 | 702 |
| Total | 34,989 | 33,838 | 5,149 | 34,245 | 4,957 |

Physical instances include **26,246 code loads and 8,743 data loads**.
An image identity combines its complete payload SHA-256 and actual load
address; identical images within a release share function rows. Unique-image
totals above are summed across releases, not deduplicated across them.
The three function columns concern code-load images only.

Registered metadata contains 5,276 function instances before image
deduplication: 4,439 matching C and 837 unmatched assembly. These are existing
registrations, not new matches. The combined registered/closed-candidate rows
have 6,047 distinct exact-byte body hashes across releases. Neither that
number nor the candidate counts measure distinct semantic functions or the
remaining decompilation workload: candidates can overlap, share bodies, contain
data, or belong to SDK code.

## Load coverage and newly exposed surfaces

- All 621 MODEL records, excluding the loader's reserved IDs, have six
  code-bearing phases: stages 7/8 at record sectors 180/190, stages 9/10 at
  200/210, and stages 11/12 at 220/222. Their lengths are 10/10/10/10/2/2
  sectors. Both destinations of each 96-sector bulk-data load are separate
  data-load instances.
- Configured overlays and their duplicate sectors are retained. Additional
  loader-defined surfaces include boot compliance, seven terrain copies,
  intro/credits, both special-battle banks, seven auxiliary model-data loads,
  main menus, name entry and PAL options.
- PAL menu copies begin at SU sectors 98, 234, 370, 506 and 642. PAL options
  code begins at WA sectors 10170, 10211, 10252, 10293 and 10334. North American
  and Japanese name-entry code begins at WA sectors 7968 and 7979.
  The PAL sector-9374 image retains its existing `password_a` metadata name;
  this census does not rename accepted modules.
- Destinations are read from each checksum-verified executable's actual
  pointer storage, not inferred from common RAM conventions. In particular,
  Japan's duel pointer at `0x800101DC` contains **`0x80154000`**, unlike the
  other releases' `0x80146000`.

Loader evidence is fingerprinted in `summary.json`. The archive model and
runtime assumptions are described in [MRG files](../../mrg-files.md) and
[runtime loading](../runtime-loader.md).

## Files and confidence

Each release has six CSVs; `summary.json` records counts and provenance.

| File suffix | Meaning |
| --- | --- |
| `images.csv` | Physical archive sectors, destination, payload hash, model/stage, loader evidence, configured aliases and code/data load classification. |
| `functions.csv` | Registered boundaries and speculative code-image starts, with evidence, reachable extents, exact-byte hashes, call targets and unresolved-flow reasons. |
| `data-hints.csv` | Compact instruction-shaped candidate offsets/counts inside data loads; not executable-function claims. |
| `coverage.csv` | Every unique code image partitioned into registered bytes, additional candidate-span bytes and explicitly unassigned ranges, with return-word hints in those ranges. |
| `unmapped.csv` | Complement of the union of all catalogued physical slices in each archive, with code-pattern counts. |
| `unmapped-hints.csv` | Exact archive offsets of those patterns, without invented RAM addresses or function boundaries. |

`registered_boundary` preserves the authoritative inventory's name, size and
status even when static traversal cannot reach its whole extent.
`candidate_cfg_closed` means a delay-slot-aware traversal reaches a return,
has contiguous reached words and has no recorded unsupported or escaping
flow. It is **still a candidate**, not a verified function.
`unresolved_candidate` retains incomplete, indirect, noncontiguous or
unsupported control flow rather than guessing a boundary.
All candidates have blank `size` and `reference_status`; `observed_extent`
is separate from a registered function size.

Seeds include known loader entries, aligned stack allocations, local call
targets and resident address references. A resident call address is only a
heuristic when many overlays share a bank. The decoder accepts PS1 MIPS-I/GTE
instructions, includes delay slots, explores both conditional paths and
preserves call continuation. Exact-byte hashes are not relocation-normalized.
Candidate spans and unassigned bytes are not proven code/data classifications.
Archive gaps use file-byte offsets; image-relative ranges are half-open.

## Remaining fog

MODEL bulk-data blocks contain recognizable instruction fragments, including
helpers whose addressing resembles another load bank. Their presence does
not establish execution at the bulk-data destination. They remain separate
data hints. Other MODEL suffix/texture regions remain in the unmapped reports.

PAL WA clusters at sectors 10129 and 10391 resemble the mapped options banks,
but no active loader path has been established for them. Late WA pattern hits
may instead be textures or other data. Return words and stack prologues alone
are not function evidence.

Unreferenced leaf callbacks, indirect jump tables, tail calls, dynamically
selected loads and foreign payload formats remain potential blind spots.
Game-versus-SDK ownership also needs individual investigation before any
candidate enters a matching attempt ledger. Full archive-byte accounting is
not a claim of mathematically exhaustive function recovery.

## Reproduction

From the repository root, with the seven legally obtained retail executables
and all 21 archives present:

```sh
tools/environments/python/bin/python tools/project/overlay_function_inventory.py \
  --output tmp/overlay-function-inventory-fresh
tools/environments/python/bin/python -m unittest discover \
  -s tools/project/tests -p test_overlay_function_inventory.py
```

The output directory must not already exist. Repeat `--region spain`, for
example, to restrict processing; the default processes all seven sequentially.
The summary fingerprints retail inputs, authoritative metadata, loader
sources, the generator, the installed decoder version and every CSV.
It remains `complete: false` until every requested region finishes.
There are no timestamps in generated reports. Reproduction of this snapshot
requires its fingerprinted metadata and generator, not future inventories.

These reports are derived evidence, not replacements for `functions.csv`,
overlay manifests, matching-C metadata or the project's progress reports.
No candidate is integrated or promoted by this tool.

For the fixed MODEL-only image denominator, current seven-release
registration/backlog breakdown, and exact-byte reuse leads, see the
[MODEL overlay inventory](../model-inventory/README.md).

For cross-release deduplication, boundary-aligned comparisons and concrete
existing-C donor leads, see [function sharing](../function-sharing/README.md).
