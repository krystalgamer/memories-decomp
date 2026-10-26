# MODEL overlay inventory

This inventory separates **card/model IDs**, **physical executable images**,
**distinct loaded payloads**, and **code bodies**. The number of configured
matching overlays is not the total number of images in the game.

All seven snapshots use accepted metadata at
`205d5280799559933d06a706604a0287086b4be4`, including MODEL167.
No candidate is promoted, no build manifest is expanded, and no C match is
claimed by this inventory.

## Splat registration follow-up

`tools/project/register_model_images.py` registers this fixed image domain in
each regional `overlays.json`, with ordinary tracked Splat layouts and companion
function inventories. It verifies the snapshot CSVs, retail archives, executable
load pointers, complete payloads, and every duplicate physical copy before
writing a region. Existing modules and their C/assembly layouts are preserved.
Only unconfigured images are added. Identical whole payloads within the same
archive, size, and load address share a layout through
`duplicate_sector_offsets`; entry-body similarity is never sufficient.

New baseline layouts retain the **entire image as unclassified raw bytes** in a
single Splat `bin` subsegment (linked through `objcopy -I binary`), so nothing
is disassembled or labelled as data.
Their function CSVs are header-only and matching-C manifests are empty.
This is Splat/image coverage, not function discovery, matching-C progress,
or proof that raw bytes are data. A whole-image baseline rebuild must still
match its retail hash. Future decompilation replaces proven spans with
independently verified C or assembly while leaving other bytes unclassified.
The 1,249 supporting data-load instances per release remain separate and are
not registered as executable overlays.

The follow-up adds the following registrations relative to accepted registration
baseline `5e461a523085cc047df2c228782bb460d35981a2`, including the thirty Spanish
MODEL411/436/437/456/469/470/477/478 images and six French MODEL456 images accepted after the
inventory cutoff. Each release then has all
**3,729** physical MODEL code images covered:

| Release | Newly covered physical images | New baseline Splat layouts |
| --- | ---: | ---: |
| North America | 3,456 | 3,307 |
| Japan | 3,729 | 3,574 |
| Europe | 3,729 | 3,574 |
| Spain | 3,332 | 3,187 |
| France | 3,198 | 3,051 |
| Germany | 3,729 | 3,574 |
| Italy | 3,729 | 3,574 |
| **Total** | **24,902** | **23,841** |

README progress tables summarize uninventoried layouts in a count row rather
than adding thousands of empty rows; inventoried layouts without matching C
share a single summary row with their function and byte totals. The manifests
and generated progress JSON retain the individual modules.

Run from the repository root (requires the corresponding legal retail inputs):

```sh
tools/environments/python/bin/python tools/project/register_model_images.py \
  --region france --region usa --region japan --region europe \
  --region spain --region germany --region italy

# Read-only coverage and retail-identity check; fails if any image is missing.
tools/environments/python/bin/python tools/project/register_model_images.py \
  --check --region france --region usa --region japan --region europe \
  --region spain --region germany --region italy
```

The normal regional `*-match-overlays` targets consume these registrations;
North America uses `make match-overlays`.

## Function inventory follow-up

`tools/project/inventory_model_images.py` turns every still-raw baseline
(the exact `layout_text`, a header-only function CSV and an empty matching-C
manifest) into a function inventory. Matched or hand-maintained layouts are
never touched. Its census walk (`resolve_flow`) adds three rules to the
published census rules, which stay the default:

- a GCC switch `lw reg, off(table + index); jr reg` follows its table only
  when the whole chain is proven: the jump register's last write is that load
  (with no branch, jump or clobber in between), the load address's last write
  is an `addu` of an in-image table constant and an index, the index is
  `sll x, 2`, and a `sltiu t, x, N` guarded by `beq/bne t, zero` bounds `x`
  with no intervening write to `x`. The table must hold exactly `N` in-span
  targets. Anything else, including strength-reduced loop indexes, stays an
  unresolved indirect jump;
- `j` leaving the span is a tail call to user RAM and counts as an exit;
- any direct call or jump outside user RAM (`0x80010000`-`0x80200000`), such as
  `j 0x8053D830`, rejects the body. Such bodies were linked for another address
  and are stale image residue.

Every closed-CFG body becomes an `unmatched_asm` row and its own
`unmatched_<offset>` generated-assembly segment. The 4-byte header stays
`data`; every other byte (gaps, truncated bodies at the image end, unresolved
flow, residue, tables and assets) stays raw `bin`. A preamble whose closed span
ends exactly with the next closed body is split and noted as a fall-through.

Row notes record reachability from rooted edges only. The loader entry is
traversed first, then bodies whose addresses resident code references. Edges
come from the reached body itself: local calls, in-image tail jumps, its own
`lui`/`addiu` address references, and the fall-through successor of a split
preamble. A reference made by an unreached body proves nothing, so the other
closed bodies keep an explicit unresolved-reachability note rather than being
excluded.

| Release | Layouts | Functions | Function bytes | Entry | From entry | Resident-rooted | Unresolved reachability | Left raw |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| North America | 2,516 | 3,497 | 13,400,312 | 2,516 | 728 | 0 | 253 | 6 |
| Japan | 3,568 | 5,708 | 20,596,428 | 3,567 | 1,404 | 5 | 732 | 6 |
| Europe | 3,566 | 5,695 | 20,552,704 | 3,565 | 1,396 | 6 | 728 | 6 |
| France | 1,680 | 1,694 | 14,384,280 | 1,680 | 12 | 0 | 2 | 6 |
| Germany | 3,566 | 5,695 | 20,552,704 | 3,565 | 1,396 | 6 | 728 | 6 |
| Italy | 3,566 | 5,695 | 20,552,704 | 3,565 | 1,396 | 6 | 728 | 6 |
| Spain | 3,179 | 3,484 | 16,229,108 | 3,179 | 268 | 0 | 37 | 6 |
| **Total** | **21,641** | **31,468** | **126,268,240** | | | | | **42** |

The six images per release left raw share one loader whose entry switches index
their tables through a strength-reduced loop register (`$s4 = 4 * $fp`), which
the straight-line chain cannot prove. With no closed body proven, each remains a
single raw baseline and still appears in the progress tables' uninventoried count.
The Japanese and the shared English/German/Italian SU intro/credits image has
no closed loader entry; its other closed bodies are still inventoried.
Identical payloads are repeated across slots and releases, so these totals are
work-distribution denominators, not counts of distinct algorithms.
Each regional `*-match-overlays` target rebuilds every image byte-for-byte.

```sh
tools/environments/python/bin/python tools/project/inventory_model_images.py \
  --region usa --region japan --region europe --region france \
  --region germany --region italy --region spain
# Read-only; fails if a raw baseline or generated inventory is stale.
tools/environments/python/bin/python tools/project/inventory_model_images.py \
  --check --region usa
``` The historical tables below retain
their original cutoff rather than silently changing the inventory's evidence.

## Why the denominator is not 722

The regular loader accepts IDs 0 through 721 but excludes 300-349, 650-699,
and 720. That leaves **621 records** in `MODEL.MRG`, each 276 sectors long.
Every record contains six executable slices:

| Role | Stages | Record-relative sectors | Sectors per image | Physical images |
| --- | --- | --- | ---: | ---: |
| Effect variants, first selection | 7/8 | 180/190 | 10 | 1,242 |
| Effect variants, second selection | 9/10 | 200/210 | 10 | 1,242 |
| Primary handlers | 11/12 | 220/222 | 2 | 1,242 |
| Special-battle handlers in SU | 3/4 | SU 1686/1696 in France | 10 | 2 |
| Intro/credits in SU | Separate load | SU 1767 in France | 16 | 1 |
| **Total executable image instances** | | | | **3,729** |

Slot pairs have different load addresses; they are not counted as a single
physical image. Payload identity is SHA-256 **plus load address**. Two payloads
can contain identical code but different textures, descriptors, or padding.
Conversely, equivalent code relocated to another slot can have different
instruction bytes.

The regular record domain and six phases come from the matched
`Model_LoadMonsterMerge` and `func_80056D7C` transfer paths
(`src/game/model_slot_control.c` and `src/game/model.h`). `func_800577B0` loads the two special-battle banks;
the special model ID is **777**, outside the regular 0-721 domain.
The intro/credits controller and European wrappers establish the separate
SU image. The census reads destinations from the checksum-verified regional
executable, not assumed common addresses. See also
[runtime loading](../runtime-loader.md).

## French results

| Family | Physical images | Distinct loaded payloads | Configured physical images | Entry images with known matching-C bytes |
| --- | ---: | ---: | ---: | ---: |
| Primary | 1,242 | 1,209 | 22 | 1,242 |
| Variants | 2,484 | 2,362 | 500 | 344 |
| Special battle | 2 | 2 | 2 | 2 |
| Intro/credits | 1 | 1 | 1 | Not represented by one assumed entry |
| **Total** | **3,729** | **3,574** | **525** | |

There are **3,204 unconfigured physical images** at this cutoff. That is
registration work, **not 3,204 new functions to decompile**.

In particular, all **1,242 primary entry images have byte-identical entry
bodies to existing matching C**, represented by only **11 exact body hashes**.
Only 22 of these physical images are configured. Most primary-image growth
therefore reflects discovering/registering copies, not discovering new entry
algorithms. Byte identity is a reuse lead: each image still needs its own
complete-image rebuild, exact linked ownership, and non-code accounting before
matching registration.

## All supported releases

Each release has the same **3,729 physical code images**, **3,574 distinct
loaded MODEL payloads**, and **1,249 supporting data-load instances**.
The matching/registration classifications differ:

| Release | Configured MODEL images | Unconfigured MODEL images | Variant entry images with local known-C byte identity | Variant entry images without that identity |
| --- | ---: | ---: | ---: | ---: |
| North America | 273 | 3,456 | 234 | 2,250 |
| Japan | 0 | 3,729 | 0 | 2,484 |
| Europe | 0 | 3,729 | 0 | 2,484 |
| Spain | 367 | 3,362 | 102 | 2,382 |
| France | 525 | 3,204 | 344 | 2,140 |
| Germany | 0 | 3,729 | 0 | 2,484 |
| Italy | 0 | 3,729 | 0 | 2,484 |

These are **MODEL-only** figures, not total regional overlay progress.
Zero configured MODEL images does not mean zero decompiled code in the release,
nor that every body is novel. In particular, the cross-release comparison
retains byte-identical C donor leads without assigning foreign C status locally.
Every release still has 498 variant entry images with unresolved flow/boundaries.

The seven releases contain 26,103 physical code-image instances. Deduplicating
complete payloads together with their load addresses reduces the summed 25,018
regional unique-payload count to **13,107 cross-release unique payloads**.
There are **1,340 exact body groups shared across releases**, among 5,350
registered-or-closed-candidate groups after boundary alignment; **787 groups
have known matching C somewhere**. None of these groups is a semantic-function
count, and candidate-only groups are not a proven remaining-code denominator.

`cross-release/peer-c-leads.csv` records the exact source and destination
image/offset for every cross-release known-C lead. It retains local status
unchanged and prefers Spanish donors when available. The site counts are:
Europe 1,189; France 1,360; Germany 2,942; Italy 2,942; Japan 1,189; Spain 1,568;
North America 1,189. These include repeated payload sites and small leaf bodies,
not that many new implementations.

The comparison reuses the existing boundary-alignment machinery. A registered
span may inform another release **only when the complete loaded payload is
identical**. Its local matching status is never copied. The 6,029 displaced
overlapping candidate spans remain recorded in
`cross-release/boundary-adjustments.csv`; unresolved sites remain in their
regional reports. No relocation masking or semantic-equivalence matching is
used.

### What remains in the variant entries

The following detailed breakdown is for **France**:

| Entry classification | Physical images | Distinct exact entry hashes |
| --- | ---: | ---: |
| Known matching-C bytes | 344 | 90 |
| Registered unmatched assembly | 156 | 40 |
| Closed-flow, unregistered candidates without matching-C identity | 1,486 | 480 |
| Unresolved entry flow/boundary | 498 | Not a reliable function-body count |
| **Total** | **2,484** | |

Thus **2,140 variant image entries lack established matching-C byte identity**
in this snapshot. The 480 candidate hashes are not 480 proven game functions;
candidate boundaries can be wrong, and SDK/data ownership remains to be
established. Exact hashes are not relocation-normalized, so the two slots may
still duplicate semantic work. Entry completion also does not imply that
helpers or code embedded later in the image are complete.

Across all French MODEL code images, the census records 1,796 registered function
sites, 3,509 closed candidates, and 684 unresolved candidates after identical
payload deduplication. Registered/closed sites form 425 exact-byte groups with
known matching C, 64 registered groups without matching C, and 851
candidate-only groups. These are **research categories, not a percentage of
remaining game code**; sites may overlap.

## Data and uncertainty stay visible

The report also lists **1,249 data-load instances** separately: both bulk-model
destinations for all 621 records, and seven auxiliary SU model-data slices.
Instruction-shaped bytes inside these loads do not prove execution. Auxiliary
entries are the complete seven-entry physical domain, not a claim that every
entry is selected during normal play.

The French code-image coverage retains **39,650,932 unassigned physical bytes**
(37,314,000 after full-payload deduplication), alongside known boundaries and
candidate spans. These bytes include headers, constants, descriptors, textures,
padding, and potentially undiscovered code. They are not all remaining C.
Candidate spans themselves are not proven code/data classification either.

The fixed 3,729 denominator closes the **documented MODEL loader image domain**.
It does not prove that every possible indirect execution path or every byte in
MODEL/SU has been classified. Unmapped archive regions and data-load instruction
hints remain in the parent census, rather than being silently excluded.
See [the full census methodology](../function-inventory/README.md).

## Files and reproduction

Each regional directory contains:

| File | Purpose |
| --- | --- |
| `images.csv` | Every code image's physical slice, model/stage, destination, hash, aliases, entry evidence and remaining uncertainty. |
| `data-loads.csv` | Every bulk and auxiliary data-load instance, kept outside the executable denominator. |
| `function-sites.csv` | All registered, closed-candidate and unresolved sites in MODEL code images. |
| `body-groups.csv` | Exact-byte grouping and known-C reuse leads; no relocation or semantic equivalence claims. |
| `coverage.csv` | Complete byte partition for every distinct code payload, including unassigned ranges. |
| `summary.json` | Totals, limitations, input/metadata/generator fingerprints and artifact hashes. |

`reference_status` on a site can come from an identical registered payload.
Check `configured_modules` to determine whether the particular physical image
is registered. A shared registered boundary does not register another copy.

From the repository root, using legally obtained inputs and fresh output
directories:

```sh
tools/environments/python/bin/python tools/project/overlay_function_inventory.py \
  --region france --output tmp/model-census-france
tools/environments/python/bin/python tools/project/model_overlay_inventory.py \
  --region france --census tmp/model-census-france --output tmp/model-reports/france
tools/environments/python/bin/python tools/project/overlay_function_inventory.py \
  --region usa --region japan --region europe --region spain \
  --region germany --region italy --output tmp/model-census-other
for region in usa japan europe spain germany italy; do
  tools/environments/python/bin/python tools/project/model_overlay_inventory.py \
    --region "$region" --census tmp/model-census-other \
    --output "tmp/model-reports/$region"
done
tools/environments/python/bin/python tools/project/model_overlay_inventory.py \
  --compare tmp/model-reports --output tmp/model-reports/cross-release
tools/environments/python/bin/python -m unittest \
  tools.project.tests.test_model_overlay_inventory \
  tools.project.tests.test_overlay_function_inventory \
  tools.project.tests.test_overlay_function_sharing
```

The focused generator rejects missing/duplicate regular phases, wrong physical
slices, missing special/data-load domains, changed census artifacts, and stale
source metadata. Reproducing a snapshot requires its recorded metadata cutoff;
future matches change classifications but not the loader-defined image count.
