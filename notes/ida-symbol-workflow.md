# IDA 9.x Symbol Workflow

The IDAPython scripts under `tools/ida/` exchange names with the tracked
project formats. They require IDA 9.x with Python 3.

## Import

Run `tools/ida/import_project_symbols.py` with **File -> Script file**.

The script reads:

- `config/slus_01411/functions.csv` for all resident function boundaries and
  current names;
- `config/slus_01411/symbols.txt` for accepted global names and explicit
  function aliases.

When the script is run from this checkout it locates the repository
automatically. Otherwise it asks for `config/slus_01411/functions.csv`.

Unmapped addresses and overlapping function ranges are skipped and counted.
By default, existing non-automatic analyst names are preserved. The prompt can
explicitly allow project names to replace them. Exact name collisions at a
different address are never overwritten.

The database should map the PS-X EXE at its runtime addresses, beginning at
`0x80010000`; otherwise most imports will be reported as unmapped.

## Export

Run `tools/ida/export_project_symbols.py` and choose an output
`functions.csv`. The script writes three review artifacts in that directory:

| File | Purpose |
|---|---|
| selected `functions.csv` | Project inventory schema with IDA function names substituted while preserving sizes, status, ownership, and notes |
| `symbols.txt` | User-defined, non-placeholder IDA names in Splat-compatible assignment syntax |
| `semantic-symbol-map.csv` | Only function-name changes and candidate globals, marked `confidence=review`; accepted globals are compared against the main map plus `notes/semantic-symbol-map.d/*.csv`, and the output stays one file |

Export never overwrites the tracked repository files automatically. Review
function boundaries, game/SDK ownership, evidence, duplicate names, overlay
addresses, and global semantics before copying rows into the project.

`semantic-symbol-map.csv` deliberately marks every export as unverified.
Passing IDA analysis or an inherited database name through this script does
not make it an accepted project name.

## Excluded linker aliases

`config/slus_01411/c_symbols.ld` is not imported or exported. It contains
compiler/linker compatibility aliases, including multiple typed views of the
same address, rather than semantic reverse-engineering claims.
