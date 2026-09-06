# Decompilation Progress

The root [`README.md`](../README.md) contains the current high-level matching
totals. Its marked progress section is generated from the authoritative
function inventory rather than maintained by hand.

Regenerate the split, update the README, and write machine-readable metrics to
`tmp/reports/progress.json` with:

```sh
make progress
```

Optionally verify that the committed README snapshot is current without
modifying it:

```sh
make check-progress
```

This check is intentionally opt-in so routine decompilation PRs do not need to
rewrite the shared snapshot. Matching C is measured only against game-owned
code; exact fallback assembly and identified Psy-Q CRT/SDK functions do not
count as decompiled C.

The matching build reports drift as a non-blocking warning, in the same way it
reports stale global usage data, so the snapshot cannot rot unnoticed while the
check itself stays opt-in for routine PRs.

Detailed campaign outcomes and historical milestones are recorded in:

- [`remaining-decompilation-pass.md`](remaining-decompilation-pass.md)
- [`semantic-naming-pass.md`](semantic-naming-pass.md)
- [`grouped-translation-units.md`](grouped-translation-units.md)

## Boundary symbols must be ignored, not merely unused

Both commands validate the tracked inventory against the split before they
measure anything, so a split that defines a function the inventory does not
list stops the report outright. The way that happens in practice is a symbol in
`config/slus_01411/symbols.txt` whose address falls inside the text range but
outside any function. Splat has no way to know a name like `text_end` marks a
boundary rather than code, so it emits a `glabel` over the alignment padding and
the inventory then disagrees with the split by exactly one phantom function.

Declaring such a symbol `// ignore:True` is what keeps splat from placing it.
Do not resolve the disagreement with `make inventory` instead: that writes the
phantom into `config/slus_01411/functions.csv`, which makes the totals wrong
rather than absent. The mismatch message names the offending addresses and
which side they came from, so the two cases are distinguishable.
