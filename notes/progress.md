# Decompilation Progress

The root [`README.md`](../README.md) contains the current high-level matching
totals. Its marked progress section is generated from the authoritative
function inventory rather than maintained by hand.

There are two reporting families, not just the README table:

| Generator | Outputs | Contents |
|---|---|---|
| `make progress` | Marked section of `README.md`; ignored `tmp/reports/progress.json` | Resident and configured-overlay function/byte metrics |
| `make global-usage` | `notes/global-usage.csv` | Global names, function identity/status, evidence paths, access widths, and source contexts |

For a scheduled report refresh, synchronize with master and run both generators
from the repository root, then run the checks in a separate Make invocation:

```sh
MAKEFLAGS=-j"$(nproc)" make progress global-usage && \
MAKEFLAGS=-j"$(nproc)" make check-progress check-global-usage check-metadata
```

Do not combine generators and their checks as unordered parallel targets: a
check could run before its report is written. The explicit check targets fail
when their tracked snapshot is stale; they do not repair it. `check-progress`
still regenerates the ignored JSON before checking the README, so that file's
timestamp alone does not prove the tracked reports were refreshed.

These checks remain opt-in for routine decompilation PRs, which do not need to
rewrite shared snapshots. The
[matching CI workflow](../.github/workflows/matching-build.yml) wraps the report
checks in `continue-on-error` steps and emits warnings. That CI policy does not
make a direct `make check-progress` or `make check-global-usage` invocation
non-failing. Read the actual error rather than assuming every failure is
ordinary snapshot drift; split/inventory disagreements need investigation.

Matching C is measured only against game-owned code; exact fallback assembly
and identified Psy-Q CRT/SDK functions do not count as decompiled C.

## Unchanged matching counts do not imply fresh reports

[`progress.py`](../tools/project/progress.py) derives metrics from function
status, size, ownership, the image map, and overlay inventories. Converting a
function to matching C is a common reason for totals to change, not the only
input to the report.

[`global_usage.py`](../tools/project/global_usage.py) also renders names,
`evidence_path`, and normalized source-line `context`. A symbol or constant
rename, declaration change, or translation-unit grouping can therefore alter
the CSV while every README count and byte total stays the same. For example,
replacing `D_8009B220 = 0x8000` with the equal-valued named flag in
[`duel_magic_effect_dispatch.c`](../src/game/duel_magic_effect_dispatch.c)
changes its recorded context,
not its matching status. Some naming-only changes leave both reports unchanged;
classifying a commit as "naming-only" is not itself a freshness check.

When multiple workers handle report updates, inspect the latest report PRs
and their final regeneration/head updates, not only one worker's saved timer
or a PR's original creation time. Check for an existing report PR again before
publishing. Update an existing owned PR or coordinate with its author rather
than opening a competing snapshot. Record the master basis and any numerical
delta, but assess the global-usage output separately even when that delta is
zero.

Detailed campaign outcomes and historical milestones are recorded in:

- [`remaining-decompilation-pass.md`](remaining-decompilation-pass.md)
- [`semantic-naming-pass.md`](semantic-naming-pass.md)
- [`grouped-translation-units.md`](grouped-translation-units.md)

Do not rewrite dated historical counts as part of a current report refresh.

## Boundary symbols must be ignored, not merely unused

The `progress` and `check-progress` targets validate the tracked inventory
against the split before measuring anything. A split that defines a function
the inventory does not list stops the report outright. This can happen when
a symbol is declared in `config/slus_01411/symbols.txt` at an address inside
the text range but outside any function. Splat has no way to know a name like
`text_end` marks a boundary rather than code, so it emits a `glabel` over the
alignment padding and the inventory then disagrees with the split by exactly
one phantom function.

Declaring such a symbol `// ignore:True` is what keeps splat from placing it.
Do not resolve the disagreement with `make inventory` instead: that writes the
phantom into `config/slus_01411/functions.csv`, which makes the totals wrong
rather than absent. The mismatch message names the offending addresses and
which side they came from, so the two cases are distinguishable.

A missing inventory function can also be a split-boundary problem. The
startup routine at `0x80012A78` can otherwise appear only as an `alabel`
inside an oversized `entrypoint`, even though matching `Main_Init` calls
it and the inventory separately records its `0x70` bytes. The explicit
`entrypoint` (`0xA0`) and `__main` (`0x70`) declarations in
`symbols.txt` preserve both established function boundaries. Inspect such
missing entries before regenerating the inventory; merging them into a
neighbor would hide the reporting problem rather than resolve it.
