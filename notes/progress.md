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

Detailed campaign outcomes and historical milestones are recorded in:

- [`remaining-decompilation-pass.md`](remaining-decompilation-pass.md)
- [`semantic-naming-pass.md`](semantic-naming-pass.md)
- [`grouped-translation-units.md`](grouped-translation-units.md)
