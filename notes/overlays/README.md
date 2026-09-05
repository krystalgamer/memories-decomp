# Overlay Research

This directory holds project-owned documentation for runtime-loaded code and
its resident load banks.

- [`../../src/overlays/README.md`](../../src/overlays/README.md) defines the
  module-scoped source layout. Runtime overlay C belongs under
  `src/overlays/<module>/`, never in resident `src/game/`.
- [`runtime-loader.md`](runtime-loader.md) records the asynchronous loader,
  archive attribution, load-bank layout, and recovered WA sector packages.
- [`matching-patterns.md`](matching-patterns.md) records the source-shape
  rules recovered while matching overlay functions, and the one residual
  difference that no available compiler profile reproduces.
- [`recorded-blockers.md`](recorded-blockers.md) explains how to read the
  per-function notes that say why something will not match, and why the
  explanation half of them should be retested rather than trusted.
- [`module-crosswalk.md`](module-crosswalk.md) correlates verified archive
  slices and load ranges with the external per-screen symbol files.
- `tools/project/overlay_diff.py` compares one candidate against the retail
  module bytes without building the module; see below.
- [`../mrg-files.md`](../mrg-files.md) documents the MRG container evidence and
  development-path strings.
- [`../research/Unchiga_Symbols/modules.md`](../research/Unchiga_Symbols/modules.md)
  is an external reference for screen-module identities and per-module symbol
  files.

Generated resident slot images remain under `tmp/splat/assets/overlays/`; they
are build artifacts rather than tracked overlay sources.

| Content | Repository location | Tracked |
|---|---|---|
| Resident executable C | `src/game/` | Yes |
| Verified runtime module C | `src/overlays/<module>/` | Yes |
| Resident load-slot snapshots | `tmp/splat/assets/overlays/` | No |
| Extracted archive payloads and probes | `tmp/` | No |

## Module build status

Every configured module is extracted from its archive, split by a
module-specific Splat layout, rebuilt, and compared byte-for-byte against the
verified slice by `make match-overlays`.

| Module | Archive | Sectors | Load address | Function inventory |
|---|---|---:|---:|---|
| `free_duel` | `WA_MRG.MRG` | `7898` x5 | `0x80168000` | Yes |
| `password` | `WA_MRG.MRG` | `8054` x15 | `0x80168000` | Yes |
| `overworld_before_coup` | `WA_MRG.MRG` | `8153` x6 | `0x80168000` | Not yet |
| `overworld_after_coup` | `WA_MRG.MRG` | `8311` x6 | `0x80168000` | Not yet |
| `main_menu` | `SU.MRG` | `98` x16 | `0x80180000` | Not yet |

A module joins the build gate once `config/slus_01411/overlays.json` gives it a
`layout`. A `config/slus_01411/overlays/<module>_matching_c.json` then lets
matching C replace part of its generated assembly.

Nothing generates the `matching_c.json` manifests, so their layout is a
convention. `make verify-overlays` pins it: every one of them, resident and
per-module, must read back byte for byte as

```python
json.dumps(data, indent=2, sort_keys=True) + "\n"
```

Adding an entry by hand is fine. Rewriting a whole file with a different
`json.dumps` call is what to avoid, because it buries the one line that
changed under a reindent of everything else.

Every tracked CSV under `config/` and `notes/` is checked by the same target,
found by walking those trees rather than from a list, so a new one is guarded
the day it is added. Every row must have the column count of its header. The
last column of these tables is prose, so an unquoted comma in it splits the
row and the text after the comma is **silently discarded** — there is no
column for it to land in, the file still parses, and every other check still
passes. Quote the field instead, which is what
`config/slus_01411/functions.csv` already does:

```
0x80168090,0x124,FreeDuel_PlaceCursor,unmatched_asm,overlay/free_duel,"Name from ..., and ..."
```

A byte round-trip through `csv.writer` does **not** catch this, which is worth
knowing before reaching for one: a row that has grown an extra field
round-trips to itself exactly. The column count is the invariant that matters.

Both guards live in `overlay_extract.py verify`, which is `make
verify-overlays`. That target is **not** reached by `make match-overlays` —
the dependency chain is `match-overlays → build-overlays → overlays`, and
`overlays` runs the *extract* command, not `verify`. The overlay workflow runs
the two as separate steps for that reason. If you add a metadata check here,
check that the step still runs it; a job named after a target is not evidence
that the target runs.

`verify-overlays` compares the extracted `tmp/overlays/<module>/module.bin`
against the archives, so it needs `make overlays` to have run first. It does
not depend on it, deliberately — the comparison is only meaningful against an
extraction it did not just perform. On a clean tree, or a CI runner, run them
as **two invocations**:

```sh
make overlays
make verify-overlays
```

Not `make overlays verify-overlays`. With no dependency between the two goals,
a parallel `make` starts them together and the verify loses the race.

The CI step took two attempts to get here, and the two failures had different
causes — worth separating, because only the second is the race:

| attempt | step body | why it failed |
|---|---|---|
| 1 | `make verify-overlays` | nothing had extracted; no `make overlays` in the job at all |
| 2 | `make overlays verify-overlays` | genuine race under the runner's `MAKEFLAGS=-j4` |

The second is reproducible locally: `rm -f tmp/overlays/*/module.bin`, then
`MAKEFLAGS=-j4 make overlays verify-overlays` fails while the same two goals
as separate invocations pass. The CI log for that attempt shows the ordering
directly — `make: *** Waiting for unfinished jobs....` and the extraction
output arriving *after* the error.

The metadata half of that target is also reachable on its own, as `make
check-metadata`. It reads `config/` and `notes/`, touches no overlay image and
needs no retail data, no toolchain and no bootstrapped Python, so
`.github/workflows/metadata.yml` runs it as a separate job that is a checkout
and one command, with **no path filter** — which is what covers the three tracked
CSV tables that live under `notes/` and that the overlay build deliberately
ignores. `make verify-overlays` still runs everything, so nothing is lost
locally.

Name entry has no module of its own: its package's executable phase is the
same image as the password screen, entered at different functions. See
[`../../src/overlays/name_entry/README.md`](../../src/overlays/name_entry/README.md).

Per-module decompilation counts are generated into the README progress
section by `tools/project/progress.py`; they are deliberately not duplicated
here.

## The anti-piracy-patched dump does not affect these modules

`notes/setup.md` records that a commonly circulated North American image has
one anti-piracy branch patched out inside `DATA/WA_MRG.MRG`, and
`make verify-inputs` now rejects it by hash. Since four of the five overlay
modules are extracted from that archive, the obvious worry is whether work
done against the patched dump has to be redone. It does not, and this is the
measurement rather than an assurance.

The two archives differ in **exactly one byte**, at offset `0xB61902`:

| | word at `0xB61900` | disassembles as |
|---|---|---|
| patched | `0x1000000A` | `b +0x2C` — always taken |
| clean | `0x1062000A` | `beq v1,v0,+0x2C` — the check |

A conditional branch turned unconditional, which is what "the anti-piracy
branch patched out" means literally.

That offset lies **below every module's extent** in the archive:

| module | archive bytes | contains the patch |
|---|---|---|
| `free_duel` | `0x00F6D000..0x00F6F800` | no |
| `password` | `0x00FBB000..0x00FC2800` | no |
| `overworld_before_coup` | `0x00FEC800..0x00FEF800` | no |
| `overworld_after_coup` | `0x0103B800..0x0103E800` | no |

The patch sits `0x40B6FE` bytes before the lowest of them. So every extracted
`module.bin` is byte-identical between the two dumps, and `make match-overlays`
produces the same five hashes on either — confirmed by re-extracting from a
clean image and rebuilding from scratch.

The executable is unaffected too: `game/SLUS_014.11` keeps the same
`84a54ed7…` hash in #798, because the patched instruction is in overlay
archive data, not in the resident binary.

To redo this check on any future disputed dump, diff the two archives and test
each differing offset against the extents in `config/slus_01411/overlays.json`
— `sector_offset * sector_size` to `(sector_offset + sector_count) *
sector_size`.

## Diffing one candidate function

`make match-overlays` is the acceptance gate, but it rebuilds and rehashes
every module, which is far too slow to compare source shapes against each
other. `tools/project/overlay_diff.py` compiles a single file and compares
just that function against the bytes already extracted from the retail
module:

```sh
tools/environments/python/bin/python tools/project/overlay_diff.py \
    main_menu 0x801840F8 tmp/candidate.c
```

It reports `MATCH` or prints a numbered side-by-side disassembly with `>>` on
each differing instruction, and exits non-zero on a difference. The source
argument is optional; without it the tool re-checks whatever
`<module>_matching_c.json` already configures — **including that entry's
compiler profile**, since a function built with a non-default profile would
otherwise be spot-checked with the wrong one. `--profile` overrides, and a
candidate passed explicitly defaults to `gcc_2_8_1_g0_split`.

Two details make the comparison trustworthy:

- **Relocated fields are excluded, and only those.** An unlinked object leaves
  every relocated field zero where the module holds the resolved value, so the
  tool reads the object's own relocation table and masks exactly the affected
  bits — the low 16 of a `%hi`/`%lo` pair, the low 26 of a jump. Registers and
  opcodes are still compared in those words.
- **The function is sliced out by symbol.** A source may define a group of
  contiguous functions, so the tool locates the requested one in the object's
  symbol table rather than assuming it starts at offset zero. Slicing by the
  symbol is what guarantees the right function is compared, so a candidate
  whose length does not yet agree with the inventory is still diffed — that is
  the normal state of a function being worked on, and the diff is what shows
  why. The length difference is reported on its own line.

This does not replace `make match-overlays`, which is still what proves a
module reassembles and links. It is what makes it practical to measure several
candidate shapes instead of guessing between them: a probe costs about a fifth
of a second rather than minutes.
