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

## Getting the archives

Both archives the modules come from are tracked disc files, so a retail dump is
the only input needed. With the disc at `game/rpg-yfm.cue` and
`game/rpg-yfm.bin`:

```sh
make disc-files FILES="WA_MRG.MRG SU.MRG"
MAKEFLAGS=-j"$(nproc)" make match-overlays
```

`make disc-files` reads the LBA and SHA-256 recorded for each file in
`config/slus_01411/disc_layout.json`, so the archives are derived from the disc
rather than obtained separately. That closes one failure mode by construction:
`WA_MRG.MRG` has a widely mirrored variant with the anti-piracy branch patched
out, which both `make verify-inputs` and `.github/workflows/overlay-build.yml`
reject by hash. An archive carved out of an image that already matches the
tracked `bin_sha256` cannot be that variant. See the section below for why the
patched dump does not affect these five modules anyway.

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

## A local array initialiser is a layout change, not a conversion

Most unmatched overlay functions can be converted one at a time: add a `c`
subsegment, write the source, build. A function whose source declares a
**non-static local array with an initialiser** cannot, because the initialiser
is data that the C file has to emit, and that data already exists somewhere in
the module as a tracked blob.

The tell is a straight-line block copy at the top of the function — sixteen
byte chunks through `$t4`–`$t7` with a `bne` back-edge, plus a short tail —
whose source is a symbol in the module's own data:

```
addiu $v1, $sp, 0x28              # destination in the frame
addiu $v0, $v0, %lo(D_80168004)   # source in the module's data
addiu $a0, $v0, 0x70              # end of the sixteen-byte loop
```

That is GCC copying an initialised local aggregate into the frame. A
`static const` array would be indexed in place instead, with no copy, so the
copy is what distinguishes the two.

`func_80168CDC` in the password module is the worked example. Its 0x78 bytes
sit at module offset `0x4`, immediately after the header word, inside the
`module_header` data subsegment that spans `0x0`–`0xB4` — and note that the
function itself is at `0xCDC`, so the data is nowhere near its code. Converting
it means carving `module_header` and letting the C file place the table at
exactly `0x80168004` as well as matching the code.

Two consequences worth stating plainly:

- **Do not pick these in a smallest-first sweep.** The inventory sorts by code
  size and says nothing about data, so such a function looks like an ordinary
  next candidate right up to the point where the link has to be re-carved.
  Check for the block copy before starting.
- **Verify the table against the built module before trusting a carve**, word
  for word. The bytes are the acceptance criterion for the data half of the
  change, and they are cheap to read out of `tmp/overlays/<module>/module.bin`.

### A jump table is the same blocker with a different tell

A `switch` dense enough for GCC to build a jump table emits that table as data
too, and the block-copy check above will not find it. `func_8016A37C` in the
password module is the second member of this class:

```
lui   $v0, %hi(jtbl_8016807C)     # the tell: a jtbl_ symbol
addiu $v0, $v0, %lo(jtbl_8016807C)
sll   $v1, $v1, 2
addu  $v1, $v1, $v0
lw    $v0, 0x0($v1)
jr    $v0
```

Its five words sit at module offset `0x7C`–`0x90`, inside the same
`module_header` blob — and **immediately after** `func_80168CDC`'s `0x78`-byte
initialiser at `0x4`–`0x7C`. The two functions' emitted data is contiguous, so
whoever carves `module_header` should do both at once rather than twice.

### Carving the blob is necessary but not sufficient

Both blocked functions are in the **password** overlay, and they need
compiler-emitted data at module offsets `0x4` through `0x90`. In password that
range is inside `.module_header` and therefore **before all of the module's
text**. The generated linker script cannot put it there:

- `section_order` is `.text`, `.rodata`, `.data`, `.sdata`, `.sbss`, `.bss`,
  and it applies **within each segment**. So in `.module` every C object's
  `.rodata` is placed after every object's `.text` — the script literally opens
  the run with `module_RODATA_START = .` immediately after
  `module_TEXT_END = .`.
- `.module_header` lists only the tracked `module_header.data.o(.data)`. It has
  no `.rodata` line for any object at all.

So splitting the data blob is only half the change. The segment model also has
to place that one source file's `.rodata` inside `.module_header`, which is a
change to how the layout is generated rather than to the yaml alone.

**This constraint is specific to the password overlay**, and the four
`0x80168000`-based overlays are indistinguishable by address alone, so it is
worth checking rather than assuming. The header extents differ:

| overlay | header | module text starts |
|---|---|---|
| `free_duel` | `0x0`–`0x4` | `0x4` |
| `overworld_before_coup` | `0x0`–`0x4` | `0x4` |
| `overworld_after_coup` | `0x0`–`0x4` | `0x4` |
| `main_menu` | `0x0`–`0x1C` | `0x1C` |
| `password` | `0x0`–`0xB4` | `0xB4` |

Only password's header reaches past `0x90`. In any other overlay those same
offsets are ordinary module text and the argument above does not apply at all.

**The failure is silent.** The script ends with

```
/DISCARD/ :
{
    *(*);
}
```

so a section nobody placed is dropped rather than diagnosed. A wrong attempt
does not fail at link time; it surfaces later as a module hash mismatch from
`make match-overlays`, with nothing pointing at the discarded section.

Worth knowing before starting: **all 75 overlay C objects currently have an
empty `.rodata`**, measured with `objdump -h`. Nothing in the overlays has ever
exercised this path, so there is no working example to copy from and no reason
to assume the rodata lines in the generated script are correct.

So the pre-flight check has two halves: `grep 'jtbl_'` as well as looking for
the frame-bound block copy. Every unmatched function in all five modules was
scanned for jump table references; `func_8016A37C` is the only hit, and the
whole overlay set contains exactly one `jtbl` symbol.

Every unmatched function in all five modules was scanned for the block-copy
pattern. `func_80168CDC` is the only one affected. One other function block copies,
`func_801821DC` in `main_menu`, but its copies run **between two regions of
`D_801D1200`** rather than into the frame — a scroll within a resident buffer,
which is ordinary code and carries no data-placement constraint.

That difference is the check worth applying: look at where the destination
lives. A destination built from `$sp` is an initialised local and means data
has to be emitted; a destination that is another global is just a copy.

## One overlay function was compiled without optimisation

`func_801697D0`, which appears identically in both overworld modules, is the
only function across all five overlays that was not built with the usual
optimised profile. Trying it under `gcc_2_8_1_g0_split` cannot work, and that
is why it is the last untouched function in those two modules.

The tells are all in the generated assembly, and any one of them is enough:

```
addu  $fp, $sp, $zero            # a frame pointer, which nothing else has
sw    $v0, 0x18($fp)             # every local written to the frame ...
lw    $v0, 0x18($fp)             # ... and reloaded on the next instruction
addu  $a0, $v0, $zero            # redundant copies kept
andi  $v1, $v0, 0x20             # an already-masked value masked again
andi  $v0, $v1, 0xFFFF
lui   $at, %hi(D_8016A2B8)       # absolute addresses through the assembler
sw    $v0, %lo(D_8016A2B8)($at)  # temporary, 22 times
```

Confirmed by compiling a reduced version of the opening block directly: `-O0`
reproduces the frame pointer, both store-and-reload pairs, the redundant copy
and the `sll 5 / addu / sll 1` index chain in the target's order, while `-O1`
keeps the locals in registers, emits no frame pointer, and produces explicit
`%hi`/`%lo` pairs. The `$at` expansions say `-msplit-addresses` is absent.

**The cheap check before starting any overlay function** is therefore
`grep 'addu \$fp, \$sp'` on its generated assembly. Every unmatched function in
all five modules was scanned; this is the only hit.

Worth stating because the size is misleading in the opposite direction from
usual: at `0x684` it is one of the largest unmatched functions, but unoptimised
output has no instruction scheduling and no register allocation to reproduce,
so it should be closer to a transliteration than the optimised functions half
its size. What it needs first is a profile, since none in
`compiler_profiles.json` uses `-O0`.

## A reload only means volatile when no store separates it

Matching `func_80183B2C` turned on recognising that its two object pointers are
a `volatile` aggregate local, which is what makes the compiler re-read them
once per occurrence in the source instead of eliminating the repeat. That is a
powerful lever — it was worth 54 instructions on its own — and it is also easy
to over-apply, because repeated loads of one address are common for a reason
that has nothing to do with `volatile`.

The distinction is whether a **store** sits between the two loads. A store
through any pointer may alias the memory the load reads, so the compiler must
re-read afterwards; that says nothing about how the source was written.
`func_80180390` looks like the strongest candidate in the whole overlay set by
the naive test, with three identical loads of one global in a single call-free
block:

```
lw   v1,%lo(D_80184560)(a1)
lhu  a0,8(v1)
sb   v0,14(v1)                   # ... stores through the pointer ...
sh   a0,8(v1)
lw   v1,%lo(D_80184560)(a1)      # so this reload is forced, not chosen
sb   v0,108(v1)
lw   v0,%lo(D_80184560)(a1)      # and so is this one
sh   zero,54(v0)
```

Every reload there follows a store. Nothing about it needs a qualifier; it is
what spelling a global pointer at each use already produces.

`tools/project/overlay_scan_reloads.py` applies the correct test — identical
loads in one basic block with no intervening store — and reports the worst case
per function:

```sh
tools/environments/python/bin/python tools/project/overlay_scan_reloads.py
tools/environments/python/bin/python tools/project/overlay_scan_reloads.py \
    main_menu 0x80183B2C
```

With no arguments it scans every function still marked `unmatched_asm` in all
five modules. Both controls behave: `func_80183B2C`, the one function known to
use a volatile local, reports 3, while `func_80180390` reports 1.

### The second tell: a reload of the address a store just wrote

The check above clears its record on any store, because a store is a legitimate
reason to reload. That hides a second and equally reliable volatile signature:
re-reading the location a store *just wrote*. The store already told the
compiler what is there, so the load buys nothing unless the object is
`volatile`.

`func_8016A080` has exactly that, and a probe confirms it reproduces only with
the qualifier:

```
jal  func_80029574
sw   v0,%lo(D_8016D430)(s0)      # store, in the call's delay slot
lw   v1,%lo(D_8016D430)(s0)      # reload into a different register
li   v0,320
sh   v0,40(v1)
```

The test has to be narrow or it is useless. Written loosely — any load of an
address a store just wrote — it fires on eleven of the nineteen functions,
almost all of them ordinary: a wider store reloaded narrowly is a truncation, a
reload into the same register is a normal re-read of a field just written, and
`-O0` code stores and reloads every local by construction. Requiring the same
width **and** a different destination register cuts it to three functions.

### Both tells need blocks that start at branch targets

The first version of this scan split blocks at branch *instructions* only. That
is not enough: a load at a branch **target** may be reached without executing
the store that precedes it in the listing, so the two belong to different
blocks and the reload is not redundant at all.

That flaw produced a confident false lead. `func_80169734` clears bit `0x20` of
`D_8016D400` and then tests bit `0x20`, which reads as redundant — except the
clear is conditional and the test is at the branch target, so on the other path
the bit is whatever it was. Reported as a volatile candidate, it was neither.

Two things are needed for correct blocks, and the second is easy to miss: a
conditional branch prints an offset from the start of the function, while an
unconditional `j` prints the raw jump field, so the function's own address has
to be known before its targets can be resolved. Handling only the first kind
still leaves blocks merged across every `j`.

**Current state of both tells across all remaining unmatched entries:**

| Function | Tell |
|---|---|
| `func_8016A080` | reload after own store — confirmed `volatile` by probe |

That is the whole list. Before the block fix the same scan reported three
functions, and before the width and destination-register conditions were added
it reported eleven. The first tell remains specific to `func_80183B2C`. This is the same kind of result as the `-O0` scan above,
and worth the same trust: it is a cheap check that closes off a whole class of
guesses rather than opening one.

## Answer a shape question with a probe instead of a reconstruction

Most of the cost of a near-miss function is rebuilding its candidate from the
prose in its inventory row. When the open question is about one loop or one
expression rather than the whole function, that cost is avoidable:
`overlay_diff.py` will compile any file you hand it, so a few lines of C named
after the function under study get compiled with the exact profile and
disassembled beside the target.

```sh
cat > tmp/probe.c <<'PROBE'
#include "../src/types.h"

extern u8 D_801B125A[];

u8 *func_80169734(void)
{
    u8 *p = D_801B125A;
    u8 *last = 0;
    s32 c = *p;
    ...
}
PROBE
tools/environments/python/bin/python tools/project/overlay_diff.py \
    password 0x80169734 tmp/probe.c --profile gcc_2_8_1_g0_split
```

The instruction counts will not match and the diff will be nonsense, but the
candidate column is the compiler's answer to the question you actually asked.
The include path is `../src/types.h` because the probe sits in `tmp/` rather
than two levels down in `src/overlays/<module>/`.

This settled the recorded residual of `func_80169734`, a 309-instruction
function, in a few minutes: the question was which source shape keeps `p++` and
`next = p + 1` as two separate additions instead of folding them to `p + 2`,
and twenty lines of C answered it without rebuilding the other three hundred
instructions. Use it whenever a row's open question names a specific construct.

### Give the probe a representative consumer

A probe answers the question you asked, and what you ask includes what happens
to the values afterwards. Register allocation depends on how long a value stays
live and on what competes with it, so a probe that ends in a placeholder call
is asking a different question from the real function.

`func_80183514` showed this sharply. Its open question was which source form
keeps three values in three registers. Ending the probe with `return sink(hi,
lo)` made **six** different spellings collapse to the same shape, and that shape
was not the target's. Replacing the call with the four-comparison chain the real
function actually ends with, and changing nothing else, made the same source
produce the target's structure.

So keep the surrounding shape: return the same way, consume the values the same
way, and keep any other work that competes for registers. A probe is cheap
enough to include the consumer, and a probe without one can return a confident
wrong answer.
