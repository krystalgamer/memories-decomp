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
| `overworld_before_coup` | `WA_MRG.MRG` | `8153` x6 | `0x80168000` | Yes |
| `overworld_after_coup` | `WA_MRG.MRG` | `8311` x6 | `0x80168000` | Yes |
| `main_menu` | `SU.MRG` | `98` x16 | `0x80180000` | Yes |

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

### What the "module header" actually is

The blob called `module_header` is not a header. Reading the bytes out of
`tmp/overlays/<module>/module.bin` shows every module has the same shape:

```
[ one word: function count ][ the module's .rodata ][ the module's .text ]
```

| overlay | count word | pre-text `.rodata` | text starts |
|---|---|---|---|
| `free_duel` | `0x13` = 19 | none | `0x4` |
| `overworld_before_coup` | `0x14` = 20 | none | `0x4` |
| `overworld_after_coup` | `0x14` = 20 | none | `0x4` |
| `main_menu` | `0x0F` = 15 | `0x4`–`0x1C` | `0x1C` |
| `password` | `0x15` = 21 | `0x4`–`0xB4` | `0xB4` |

Password's region is three separate `.rodata` items, in the same order as the
functions that own them:

- `0x4`–`0x7C`, thirty-one words of Shift-JIS codes ending in `ffffffff` —
  `func_80168CDC`'s initialiser, at module offset `0xCDC`
- `0x7C`–`0x90`, five words `8016a3fc 8016a5c0 8016a68c 8016a794 8016a8a0`,
  every one an address inside `func_8016A37C` at `0x237C` — its jump table
- `0x90`–`0xB4`, the ASCII string `SaveLoad Buf add = 0x%x size = 0x%x\n`

**The constraint is not specific to password.** `main_menu` has the same
structure: `0x4`–`0x1C` is six words — `8018416c 80183514 801836f4 80183884
80183a14 80184254` — all main_menu code addresses, so a table of function
pointers, and two of them are the comparators `func_80183514` and
`func_801836F4`. Any module whose C files emit read-only data will hit this,
and three modules only avoid it because they have none.

### The placement is a yaml change, not a build-system change

An earlier version of this note concluded that the generated linker script
could not place a C object's `.rodata` before the module text, because
`section_order` is `.text, .rodata, ...` and `.module` therefore opens its
rodata run immediately after `module_TEXT_END`.

The conclusion was wrong, but so was the first correction to it. That first
correction said `section_order` cannot help because it only orders sections
*within* one segment, and therefore the rodata needs a segment of its own
declared ahead of the text. Both halves of that are true, and it does work —
but it treats a symptom. `section_order` is not a constraint to work around;
it is a **description of the module's layout**, and ours described the wrong
one. The modules are `.rodata` then `.text`, so the option should say so:

```yaml
  section_order:
    - ".rodata"
    - ".text"
    - ".data"
    - ".sdata"
    - ".sbss"
    - ".bss"
```

With that, the whole pre-text region belongs to the ordinary `module` segment,
which simply starts at `0x4` instead of after the rodata, and no extra segment
is needed:

```yaml
  - name: module_header
    type: code
    start: 0x0
    vram: 0x80168000
    subsegments:
      - [0x0, data, overlays/password/module_header]

  - name: module
    type: code
    start: 0x4
    vram: 0x80168004
    subsegments:
      - [0x4, rodata, overlays/password/module_rodata]
      - [0x90, .rodata, overlays/password/name_entry_main]
      - [0xB4, c, overlays/password/func_801680B4]
      ...
      - [0x5400, data, overlays/password/data]
```

The linker script then opens `.module` at `0x80168004` with
`module_RODATA_START`, runs the rodata, and only then reaches the text — which
is what the retail module is. `.data` still follows the text, so the `0x5400`
blob is unaffected. Both password and main_menu are configured this way and
all five modules stay byte-exact.

`configs/JAP10/main.yaml` in `khasinski/rage-racer-decomp` is the same
arrangement on a whole executable: `section_order` with `.rodata` first, one
`main` segment, and dozens of `.rodata` and `rodata` subsegments interleaved
in address order. It is worth reading because it shows the steady state this
grows into rather than the first step.

The two subsegment spellings — `.rodata` with a dot and a C file name against
`rodata` with no dot — are described in `notes/build.md`, along with the same
convention for `.data` and `.bss` and the three ways a layout mistake here
fails silently. They are not overlay-specific and are not repeated here.

Only the `.rodata` half of that convention is verified in this repository, by
the worked example below. The `.data` half is the same mechanism but has not
been exercised here yet, and there is a specific reason to expect it to be
harder, which is worth knowing before starting.

### The overlay data blobs are mostly not `.data` at all

Password's blob runs `0x5400`–`0x7800`, 9216 bytes, and only **1499 of them
are non-zero**, the first at `0x5590`. Everything before that — the whole
`D_8016D400` block of flags, pointers and counters this module works through —
is zeros, and the non-zero part is sparse rather than tabular: `04` at
`0x5590`, `10` at `0x55CA`, and so on. That is a struct with a few non-default
fields, not a lookup table.

The trap follows directly. A zero-valued global written in C as `u8 x;` or
`u8 x = 0;` is not `.data`; the compiler puts it in `.bss`. Declaring a
`.data` subsegment for a file whose globals are all zero therefore yields an
empty section, the blob loses those bytes, and everything after it moves. It
fails the same silent way everything else in this area fails — a whole-module
hash mismatch with nothing naming the file responsible.

So for these modules the `.data` conversion is really two questions, and only
the second is about `.data`: which globals are genuinely initialised, and
where the zero ones belong. Read the blob before assuming, the same way the
rodata region was read before it was carved.

That is the same spelling other PS1 projects use for jump tables; the
references collected under `tmp/references/jtbl/` show `[0x1148, .rodata,
map3_s03]`, `[0x988, .rodata, thread]` and `[0x40A30, .rodata, C82B8]` in
three separate repositories.

**Verified, not inferred.** Configuring password as above and running splat
produces exactly the required line, at the required address:

```
.module 0x80168004 : AT(module_ROM_START) SUBALIGN(2)
{
    module_RODATA_START = .;
    .../asm/data/overlays/password/module_rodata.rodata.o(.rodata);
    .../src/overlays/password/name_entry_main.o(.rodata);
    ...
}
```

so a C object's `.rodata` can be placed ahead of the module's text today, with
no tooling change. What remains for the two blocked functions is the ordinary
work of writing source whose emitted `.rodata` is byte-correct.

### A worked example that is in the tree

`NameEntry_Main` now calls the resident `printf` with the literal
`SaveLoadBuf add = 0x%x size = 0x%x\n`. Before the `.rodata` integration,
matching source expressed the same call as
the callee at `0x8008E870` with `D_80168090` as an extern into the blob.
Replacing that address-based callee and extern format pointer with `printf`
and the literal makes the compiler emit all 36 bytes, and the yaml hands that
section its address:

```yaml
      - [0x4, rodata, overlays/password/module_rodata]
      - [0x90, .rodata, overlays/password/name_entry_main]
```

All five modules still hash byte-exactly, and the evidence that the bytes
really moved is on both sides of the change:

- `objdump -h` on `name_entry_main.o` reports `.rodata` of `0x24` — the exact
  string length. Before this, *every* overlay C object had an empty `.rodata`,
  which is why the path had never been exercised.
- the extracted blob `module_rodata.rodata.s` now ends at `0x8C`, the last word
  of the jump table, instead of running to `0xB4`.

So the pre-text region is no longer a wall. It is an ordinary boundary that
moves down as functions are converted, one `.rodata` subsegment at a time.

Two practical points from doing it:

- **Move the region into the text segment before converting anything.** The
  count word has to stay in its own `module_header` segment, because
  `section_order` puts `.rodata` first and would otherwise place the rodata
  ahead of it. Extending `module` down to `0x4` with the region still a plain
  blob is a separate, independently verifiable change: password and main_menu
  were both moved that way first, and all five modules stayed byte-exact
  before any C was touched.
- **The order of `.rodata` subsegments is the order of the owning functions.**
  Password's three items sit at `0x4`, `0x7C` and `0x90` for functions at
  `0xCDC`, `0x237C` and `0x2A6C`. That is ordinary linker behaviour — rodata
  contributions follow object order — and it means the addresses are a
  prediction, not a free choice: if a conversion puts a section at the wrong
  offset, the owning function is wrong.

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

Worth knowing before starting: **every currently tracked overlay C object has
an empty `.rodata`**, measured with `objdump -h`. Treat this as a pre-flight
property to recheck as C coverage grows rather than a fixed object count.
Nothing in the overlays has ever exercised this path, so there is no working
example to copy from and no reason to assume the rodata lines in the generated
script are correct.

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

### A delay slot belongs to the block before the transfer

The last false positive was subtler than the branch-target one. An instruction
in a delay slot executes **before** control transfers, so a store written in a
call's delay slot is followed by the call, even though the listing prints the
`jal` first. Attributing the delay slot to the block after the transfer makes
the store and the next load look adjacent when a whole function ran between
them.

`func_8016A080` was flagged that way, and recorded here as a confirmed
`volatile`. It is not. Its reload has an ordinary explanation: the store is
followed by a call, and a call invalidates memory.

**Current state of both tells across all remaining unmatched entries: nothing
is flagged.** The second tell has no hits at all once delay slots are attributed
correctly. The first remains specific to `func_80183B2C`, which is still caught
when queried directly.

That is worth saying plainly rather than quietly deleting a row: the scan
reported eleven functions when it was written loosely, three after the width
and destination-register conditions, one after blocks were split at branch
targets, and none once delay slots were placed correctly. Every one of those
reductions removed a false positive, and one of them had already been written
into an inventory row as a fact. This is the same kind of result as the `-O0` scan above,
and worth the same trust: it is a cheap check that closes off a whole class of
guesses rather than opening one.

## Read the opcode distance before the position count

`overlay_diff.py` prints an opcode distance beside every DIFF. It is the L1
distance between the two instruction multisets, classified from the encoding
rather than the printed mnemonic, so `li` is whatever it actually assembled to
and no alias table sits between the measurement and the thing measured.

Distance zero means the candidate already has the target's exact instruction
mix and differs only in register choice or scheduling. That is worth knowing
before spending a pass on source shape, because no rearrangement of statements
will change a multiset that is already right.

The position count does not tell you this. It is order-sensitive, so one
misplaced block or one unfilled delay slot shifts everything after it and
inflates the count far beyond the real damage. On `func_8016913C` the two read
321 and 26 for the same candidate. Read the distance first, then the jump
count for block placement, and only then the positions.

`nop` is classified separately from `sll` even though it encodes as
`sll $0,$0,0`. Bucketing them together is wrong in a way that misdirects the
next pass: a shift difference is a shape error in the source, while a `nop`
difference is the scheduler failing to fill a delay slot, and those call for
opposite responses. `func_80168CDC` showed this. Its residual read as four
extra shifts, which sent a pass looking for a shift the source did not have;
both sides in fact emit exactly eight real `sll`, and the four extra were
unfilled delay slots.

Once they are separated the residual usually splits into a cause and a
symptom. `func_80168CDC` reads as four extra `nop` against three missing
`addiu` and two missing `addu`, which is not five faults but one: the target
recomputes an address at each use where the candidate hoists it, and the five
instructions that go missing are the ones the scheduler would have used to
fill those four slots. Fix the arithmetic and the `nop`s close on their own.
Chasing the `nop`s directly cannot work, because there is no way to spell an
unfilled delay slot in C.

## Pass a small constant as a literal, not through a local

When a constant is both passed as an argument and used elsewhere in the
function, whether the source names it decides where the compiler materialises
it, and that is visible in the output.

A local holding the constant is one value with one live range, so the compiler
materialises it once, early, and keeps it in a register across the whole block.
Passing the literal at the call site instead lets it be rematerialised at each
point of use, which on this compiler means inside each branch arm, typically in
a delay slot that would otherwise be empty.

`func_80168CDC` is the worked example, and the effect is not marginal. Its
draw call takes 16 as argument 4, and the same 16 is the shift amount for an
s16 truncation. Written with a local for both, the compiler emits one early
`li` and leaves four delay slots in the arms empty. Written with the literal
at the call, it emits `li a3,16` inside each arm instead. That one line took
the function from 239 of 240 instructions at opcode distance 9 to 240 of 240
at distance 2, and the differing positions from 123 to 44.

This is also why the earlier reading of that residual was wrong in a useful
way. It looked like three missing `addiu` and four extra `nop`, four separate
faults plus some padding. It was one fault: `li` assembles to `addiu`, the
three missing ones were the rematerialisations, and the four `nop`s were the
slots those instructions would have filled. Whenever missing `addiu` and extra
`nop` appear together, check where a constant is being named.

The reverse is not automatic. Spelling *every* use as a literal can be worse:
on the same function, making the shift amount a literal too collapsed the
variable `sllv`/`srav` pair into shifts by immediate and lost two instructions.
Change one use at a time and measure.

## A matching count under `no_sched1` can be a false positive

Turning off the first scheduling pass changes how many instructions the
compiler emits, so sweeping it against a near-miss candidate will sometimes
land exactly on the target's length. That is not the same as being closer, and
the position count cannot tell the two apart.

`func_80168CDC` is the worked example. Under `gcc_2_8_1_g0_split` it builds
239 of 240 at opcode distance 9; under `gcc_2_8_1_g0_no_sched1_split` it builds
240 of 240 at distance 16. The exact count is reached by way of a worse
instruction mix. Its inventory row had already noticed the trade in prose --
the profile "closes the count by changing the null path instead" -- but judged
it genuine, because with only a count there was nothing to weigh the trade
against. The distance prices it, and reverses the conclusion.

So when a profile sweep closes a count, check the distance before adopting the
profile. If the distance rose, the profile is fitting the length rather than
the function, and building on it puts every later measurement on a worse base.

## Sweep the cross product, not each axis in turn

Levers are not independent, and the habit of testing them one at a time is
how a reachable match gets recorded as closed.

`FreeDuel_PlaceCursor` is the worked example. Its row said the residual was
closed against source order over several passes, and that was honest: every
axis really is inert on its own. The match needs three things at once --
reading the column into a local before forming the panel address, *not*
naming the row as a local, and keeping the panel as a local. Change any one
or two and the five differing positions do not move. Change all three and
they go to zero. No amount of further one-at-a-time measurement would have
found it.

Before writing that an axis is closed, exhausted or inert, ask which cross
product has not been run. "I measured six things and they were inert" is not
a stopping condition.

`CampaignMap_UpdateLocationTransition` makes the same point with numbers that
are easy to check. Its camera block ends in five accumulator adds followed by
five stores, so there are two obvious axes of 120 permutations each. Swept one
at a time both looked finished: the add order alone left the best cell at seven
differing positions, which is where it already was, and the store order alone
was flat at seven across all 120. Either result on its own reads as a closed
axis. The full 14,400-cell product dropped it to five, and the winning cell
pairs an add order that had not appeared anywhere in the single-axis top ten
with a store order that was tied with everything else.

That is the mechanism behind the rule, not a coincidence. A one-at-a-time sweep
holds the other axis at its current value, so it can only find a lever that
improves things *given the rest of the current shape*. When the true
configuration needs two simultaneous changes, the intermediate states are no
better than where you started -- and frequently worse -- so every single-axis
probe reports inert. The axes were never independent; measuring them
independently is what hid the answer.

But a cross product is still only as wide as the axes you thought of, and
`func_80168AB4` is the cautionary example. Roughly 10,800 cells were measured
against its one remaining rotation, across statement order, read-modify-write
splitting, `volatile`, aliased pointers, local naming, declaration order and
all 29 profiles. Every cell returned the same ten differing positions, and its
row concluded the residual was unreachable from source and the function should
be left alone. That was wrong. The match needed an axis none of those cells
touched -- moving a store out of statement position and into an argument -- and
it is described in the next section.

The size of a negative result is not evidence about its completeness. Ten
thousand cells across seven axes say exactly as much about the eighth axis as
one cell would.

## A side effect in an argument is emitted later than the same statement

When two instructions are both ready at the same point in a block and neither
has a longer chain to the end, GCC 2.8.1 breaks the tie on the order they were
emitted into RTL. Statement order does not decide that, because every statement
in a block is emitted before any of the call's argument setup. So a store
written as the statement before a call is always emitted ahead of the argument
moves, no matter where among its neighbouring statements it is placed.

Written inside an argument instead, the same store is emitted with that
argument -- after the values of the earlier arguments have been computed. That
is the only way source has of moving a side effect past argument setup, and it
is a real lever whenever a residual is a rotation of a block that ends in a
call.

`func_80168AB4` is the worked example. Its last block was already exact in
content: 138 of 138 instructions, opcode distance 0, the same instruction
multiset, and the seven middle instructions in the same relative order. The
target opened the block with the argument move

```
move a2,zero
```

and closed it with the timer rearm

```
li v0,2
sh v0,96(s1)
```

while every build did precisely the reverse. Both are ready as soon as the call
is, and both sit one step from it, so the tie falls to emission order. As a
statement the rearm is emitted first and takes the head of the block, pushing
the argument move to the end:

```c
    *(s16 *)(w + 0x60) = 2;
    node = TextBox_GetGlyphAt(3, D_8016D42C << 4, 0);
```

Carried in the third argument it is emitted after `a1` is computed, falls to
the end of the block, and lets the argument move take the head, which matches:

```c
    node = TextBox_GetGlyphAt(3, D_8016D42C << 4, (*(s16 *)(w + 0x60) = 2, 0));
```

Which argument matters. The same store folded into the first or the second
argument is emitted too early and measures exactly the same as the statement
form; only the last argument puts it after the rest of the setup.

Two things make this axis easy to miss. The first is that it looks like a
scheduling problem, and scheduling is the standard example of something source
cannot reach -- but the scheduler's *input order* is source-reachable even when
its algorithm is not. The second is that every cheap experiment lands on the
wrong side of it: reordering statements, splitting the store, pinning it with
`volatile`, aliasing the pointer and renaming locals are all statement-level
changes, and statement-level changes cannot move a store past argument setup by
construction. A sweep over thousands of those cells returns one number and
reads like proof.

The tell to look for: a residual that is a **rotation** rather than a
substitution -- distance 0, the two columns holding the same instructions, and
a block that ends in a call, with the pieces that swapped being one side effect
and one argument move.


The reason cross products were not being run is that they cost too much.
`overlay_diff.py` takes about thirty seconds per candidate, so a few hundred
cells is a few hours and nobody tries it.

`tools/project/overlay_sweep.py` compiles the whole product in parallel and
sorts by distance, then positions. The 3360-cell sweep that matched
`FreeDuel_PlaceCursor` took under two minutes. As a library:

    from overlay_sweep import sweep, show
    show(sweep("free_duel", 0x80168090, "FreeDuel_PlaceCursor",
               {"label": source_text, ...}, profiles=[...]))

Generate the variants programmatically from the axes rather than writing them
out; the point is to cover combinations a person would not think to try. Keep
spellings that are known to be worse alone in the set, because a lever that
loses on its own can still be part of the winning combination.

From the command line it sweeps one source across every profile:

    tools/project/overlay_sweep.py free_duel 0x80168090 cand.c --all-profiles

Always confirm a hit with `overlay_diff.py` before integrating it.

## Score a lever on the axis it targets, not on the total distance

The total opcode distance is the right thing to compare two *candidates* by.
It is the wrong thing to judge a single *lever* by, because a lever that fixes
the axis it aims at often disturbs another at the same time, and the total then
reports no change.

Five signedness flips on `func_8016913C` all measured at distance 26, exactly
the baseline, which read as five inert changes. Scoring them on the byte loads
alone showed something quite different: four were genuinely inert, changing
nothing whatever, while the fifth halved that axis, took `lbu` from one too
many to exactly right, and moved the length two instructions closer to the
target. Its cost was a sign-extension pair elsewhere, which is why the total
did not move.

So when testing a lever, decide beforehand which opcode classes it should
change, and read those counts. A lever that improves its own axis and pays for
it elsewhere is usually a real finding with a second fault still in front of
it, and it is worth recording as evidence even when you do not adopt it. A
lever that leaves its own axis untouched is inert, and that is worth recording
too, because it removes a hypothesis permanently.

## Keep a near-miss candidate instead of rebuilding it

Candidate sources live in `tmp/`, which is not tracked, so they disappear when
a session ends and the next run rebuilds them from the prose in the function's
inventory row. That has happened at least six times, three of them on
`func_80168CDC`, and it costs several minutes and can drift from the state the
row describes.

`notes/overlays/candidates/` is the durable copy, one file per function. Put a candidate there once
it is close enough that the next run would otherwise rebuild it, store the
exact source rather than a tidied version, and re-verify it with
`overlay_diff.py` before trusting it. The inventory row still holds the
findings; the file holds only code.

## Decode a divide's magic constant instead of guessing the divisor

An unsigned divide by a constant compiles to a multiply-high and a shift, and
the pair names the divisor exactly. The magic is `ceil(2^(32+s) / d)` truncated
to 32 bits, so for the same magic each extra shift doubles the divisor:

| magic | shift | divisor |
|---|---|---|
| `0xCCCCCCCD` | 3 | 10 |
| `0xCCCCCCCD` | 4 | 20 |
| `0xCCCCCCCD` | 5 | 40 |
| `0x88888889` | 3 | 15 |
| `0x88888889` | 4 | 30 |
| `0x51EB851F` | 5 | 100 |
| `0x10624DD3` | 6 | 1000 |

`func_8016A37C`'s starchip drain was reconstructed as dividing by 10, 100,
1000 and 10000 -- the obvious reading, since its four range tests are at those
values. The constants say 10, 20, 30 and 40. That is a better design than the
guess: the step grows with the magnitude of the count, so the counter drains in
roughly constant time whatever it started at, which is what an animated counter
wants.

### Neither headline metric could see the error

The **opcode distance** cannot: `lui`, `ori` and `srl` are the same classes at
any immediate, so a wrong constant is invisible to it by construction.

The **differing-position count** could not either, here. The block was
displaced by one instruction -- the target hoists a `lui` into a branch delay
slot twelve instructions earlier -- so every position in it differed whichever
divisors were used, and the count sat at 103 before and after.

What does see it is the instruction multiset with the **register fields masked
out**, which went from 308 to 292: sixteen, being the four `ori` immediates and
the four shift amounts.

```python
def strip_regs(w):
    op = w >> 26
    if op == 0:
        return w & 0x000007FF          # SPECIAL: funct and shamt
    return w if op in (2, 3) else (op << 26) | (w & 0xFFFF)
```

Worth running whenever a block is displaced and the position count has stopped
responding: it answers "is the content right yet" separately from "is it in the
right place", and those are different questions that the standard metrics
conflate.

`tools/project/overlay_blind_audit.py` runs it over every candidate stored in
`candidates.md` at once and prints the immediates each side actually chose,
with the zeroed relocations filtered out. That is worth doing periodically
rather than only on the function being worked: run across all five stored
candidates it reported `func_8016913C` writing `+0x76` and `+0x78` where the
target writes `+0x5E` and `+0x60` -- a struct whose padding was twenty-four
bytes too long, which had survived several passes because both headline
metrics were blind to it.

## A register-form shift means the count is not a constant

MIPS has two encodings for each shift: `sll`/`srl`/`sra` take the count as a
5-bit immediate, and `sllv`/`srlv`/`srav` take it from a register. When the
target uses the register form and the candidate uses the immediate form, the
temptation is to look for a register that happens to hold the right value and
assume some pass substituted it. It does not work that way, and the difference
is worth reading precisely because it says something definite about the source.

GCC picks the encoding at RTL generation, from whether the count operand is a
`CONST_INT` or a `REG`. Nothing later converts one into the other: CSE
propagates registers *into* constants, not the reverse, so a count the compiler
can see is a constant will always take the immediate form.

So a register-form shift is a statement that the count is not a compile-time
constant in the source. A four-line probe settles where one can come from:

```c
u32 g1(void *p, void *o, u16 pri, u32 w, int x) { f(p,o,pri,1); return x + (w >> 1); }
u32 g2(void *p, void *o, u16 pri, u32 w, int x) { int n = 1; f(p,o,pri,n); return x + (w >> n); }
u32 g3(void *p, void *o, u16 pri, u32 w, int x, int n) { f(p,o,pri,n); return x + (w >> n); }
u32 g4(void *p, void *o, u16 pri, u32 w, int x) { u32 r = x + (w >> 1); f(p,o,pri,1); return r; }
```

Only `g3` -- where the count is a **parameter** -- emits `srl $16,$16,$18`. The
literal, the local assigned 1, and the halving hoisted above the call all emit
`srl $16,$16,1`. Naming a local does not help however many times it is used:
on `func_801681A0` a local used ten times, as the fourth argument of all eight
calls and as both shift counts, is still folded.

There is one apparent counter-example, and it is instructive rather than
contradictory. `func_80168CDC` needed `sllv`/`srav` and got them without any
variable, by declaring the shifted value `s16`: the widening at the use is
emitted as a shift *pair*, and it is the pair, not a source-level shift, that
CSE then satisfied from the argument register. So before concluding that a
register-form shift demands a parameter, check whether the shift is really a
widening in disguise -- a `sllv` by 16 immediately followed by a `srav` by the
same register is a sign extension, not a division.

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

void NameEntry_UpdateDialog(void)
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

This settled the recorded residual of `NameEntry_UpdateDialog`
(`0x80169734`), a 309-instruction function, in a few minutes: the question was
which source shape keeps `p++` and `next = p + 1` as two separate additions
instead of folding them to `p + 2`, and twenty lines of C answered it without
rebuilding the other three hundred instructions. Use it whenever a row's open
question names a specific construct.

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

### A prefix probe verifies a prefix, and immediate scheduling is not part of it

A large function cannot be reconstructed and iterated in one sitting, but its
opening can be probed on its own. Write only the leading section, compile it
under the function's own name and profile, and read the candidate column
against the target's first instructions. The prologue differs — a partial probe
saves fewer registers, so stack slots and the save list shift — but everything
after it lines up, and what it confirms is real: operand widths, the exact form
of a pointer computation, which global the compiler parks in a callee-saved
base register, and the shape of each loop.

`FreeDuel_Init` is 468 instructions and had been decoded but never
reconstructed. A probe of its first three sections reproduced target
instructions 0 through 104 exactly, which pins the record-pointer arithmetic,
the sign-extended clamp, the base register holding `gFreeDuel_bScreenFlags`,
both table-clear loops and the unlock loop, and turns 105 instructions of the
eventual reconstruction from prose into confirmed source.

The bound is specific and worth stating, because it looks like a mismatch. Two
differences survived, and both were the *position of an immediate*: the target
materialises all four register arguments of a call up front where the probe
emits them just before the `jal`, and hoists a `li` above a base computation
where the probe emits it after. Where an immediate is materialised depends on
register pressure, and a probe that omits 363 instructions has far less of it.
So a prefix probe is authoritative about structure and widths and silent about
immediate scheduling. Re-check that class of difference against the full
reconstruction, and do not perturb the opening trying to fix it.

## Verify a rule by perturbing the source it was drawn from

Every rule in `matching-patterns.md` cites a function, and those functions are
matched and committed. That makes each rule falsifiable in about a minute:
apply the change the rule says is wrong to the matched source, run
`overlay_diff.py` on that one function, and see whether the match survives. If
it does, the rule is not load-bearing and says more than the evidence supports.

Six rules were checked this way. Four are load-bearing, and the perturbation
costs are large enough to be unambiguous:

| Rule | Perturbation | Result |
|---|---|---|
| volatile aggregate local | drop the qualifier | 195 of 216 |
| compound assignment for the sign flip | write it out as `x = -x` | 217, 153 positions |
| a re-read needs its own local | spell the global in the third statement | 222, 108 positions |
| `s16` local for a frame count | test the field directly | 1 position |

Two were not, and both were mine:

- The high end of the `func_80183B2C` range test can be written unsigned rather
  than as a signed-negative test, byte-identically. The rule had claimed any
  two-sided comparison changes the shape.
- The subtraction in `FreeDuel_UpdateCursorTween` does not need its
  intermediate assign-back statement; collapsing it is byte-identical. Only the
  choice of variable matters, which the live-range rule beside it already says.

Both are corrected in place. The lesson is worth the minute it costs: a rule
written from a single match tends to record the whole shape that happened to
work, including the parts that were incidental, and the only way to tell which
parts carry weight is to break them one at a time.

A second pass covered six of the older rules, the ones written before this
window. Four hold with wide margins, and one of them reproduces its recorded
number exactly: declaring `func_801812B4`'s narrow local `s32` costs six
instructions, which is what the rule says. The operand-order pair on
`func_801840F8` each cost one position, so they are real but slight.

Two needed refining rather than correcting:

- The chained assignment rule is load-bearing for its pointer form, where
  splitting costs an instruction in either statement order, but **not** for the
  plain-constant form beside it: two statements in descending order are
  byte-identical, and only the order carries weight.
- The conditional-expression rule costs an instruction at two of its four sites
  in `func_801812B4` and none at the other two, which still differ by sixteen
  positions. The tell is sound; the price is not uniform.

Neither of those is a wrong rule, but both said more than the evidence
supported, which is the same failure mode as the two corrected above.

## An argument register in the prologue tells you the call's arity

A value that the target loads into `a0`-`a3` shortly before a `jal` is an
argument to that call. That sounds obvious stated plainly, but it is easy to
miss when the residual is small, because the diff presents it as a register
allocation difference rather than as a missing argument.

`CampaignMap_UpdateLocationTransition` spent a long time at five differing
positions, all in the prologue, and every one of them looked like scheduling:

```
   19  lui  v0,0x8017              lui  s0,0x0
   20  lui  s0,0x8017              lui  v0,0x0
   21  lw   a1,-27128(v0)          lbu  a0,0(s0)
   22  lbu  a0,-27124(s0)          lw   v1,0(v0)
   ...
   25  sw   a1,-27180(v0)          sw   v1,0(v0)
```

The candidate had the same instruction mix, the same count and an opcode
distance of zero. The two `lui`s were merely in the other order, and the loaded
word sat in `v1` instead of `a1`. Read as scheduling, that is a dead end: six
prologue shapes had already measured inert, and no reordering of statements
moves a value into `a1`.

Read as arity it is immediate. Position 24 is a `jal`, so position 25 is its
delay slot, and positions 21 and 22 load `a1` and `a0` -- the first two
argument registers. The target's call takes two arguments. The candidate's
took one, so its second load had nowhere to go but a temporary. Adding the
argument matched the function outright.

The store is the part worth understanding, because it explains why the
difference showed up as a register name. In the target the same `a1` serves
twice: once as the argument and once as the source of `D_801695D4 =
gCampaignMap_MoveState`, which GCC then folds into the call's delay slot. That
is only possible because argument setup has already put the value in a
caller-saved register that is live across the delay slot but dead after the
call. When the call takes one argument, the store's operand has no reason to be
in `a1`, GCC allocates `v1`, and the two `lui`s swap because the load order
follows. One missing argument, four consequences, none of which look like a
missing argument.

The tells, in order of how much they narrow the search:

- A load into `a1`, `a2` or `a3` that is not followed by a `jal` in *your*
  build, but is in the target, means the target's call has more arguments.
- More generally, count the argument registers written between the previous
  transfer and the `jal`. That count is a lower bound on the arity, and it is
  a fact about the target, not a hypothesis about your source.
- If a store's source register in the target is an argument register, the
  stored value and an argument are the same value.

The prototype has to move with the call. `func_801688BC(a, b)` against
`extern void func_801688BC(s32);` is a hard error, so a sweep over call sites
alone reports every cell as a build failure and looks like the axis is
unavailable. Vary the declaration and the call together.

The corroboration arrived after the fact and is worth recording as a process
note. The callee at `0x801688BC` is `CampaignMap_StartCameraTween`, already
matched and already in the tree as `src/overlays/overworld/camera_tween.c`,
declared `void CampaignMap_StartCameraTween(s32 index, s32 steps)`. Its arity
was sitting in a sibling file the entire time. The candidate had been carrying
a placeholder `extern void func_801688BC(s32);` invented when the function was
still unnamed, and nothing ever forced the two to agree because the candidate
compiles standalone in `overlay_diff`. Before sweeping a call, check whether the
callee is already matched in the same module and copy its real signature.

## Prefer the callee's real symbol over a local `func_` placeholder

Following on from the above: a candidate developed under `overlay_diff` never
links against the module, so a wrong `extern` for a callee costs nothing until
integration. Two failure modes come out of that, and both cost real time here.

The first is arity, described above. The second is that placeholders hide
symbols which no longer exist. When the last assembly segment referencing a
symbol is converted to C, splat stops emitting that symbol into
`undefined_syms_auto.txt`, because the list is generated from the remaining
disassembly. Converting `text_AA8` dropped four data symbols and both callees
from the auto list, and `make build-overlays` failed with six undefined
references, none of which the per-function diff could have predicted.

The fix was not a linker-symbols file. All six already had names:
`gCampaignMap_aLocationTable`, `gCampaignMap_MoveState`, `gCampaignMap_Location`
and `gCampaignMap_LocationPrev` in
`notes/research/Unchiga_Symbols/modules/overworld.txt`, and the two callees in
sibling sources. The candidate was referring to real things by names nobody
used. Renaming to the established symbols resolved every reference and left the
match intact, which is expected -- the linker binds the same addresses either
way, so a pure rename cannot change codegen.

Reserve a linker-symbols entry, as `password` has for `gSaveData_aPlayerNameSjis`,
for a symbol that genuinely has no definition in reach. Check the module symbol
file and the already-matched siblings first.

## A struct pointer schedules differently from byte-offset casts

Writing five fields of one object through `*(s16 *)((u8 *)base + n)` casts and
writing them through a declared `struct` pointer are the same semantics and
different RTL. On `CampaignMap_UpdateLocationTransition` the change was worth
twenty of the twenty-seven remaining differing positions, more than any other
single lever found for that function.

The reason is what the two forms present to the scheduler. Each cast is an
independent address computation over a `u8 *`, and GCC 2.8.1 keeps them
independent: it has no basis for deciding that `base + 0` and `base + 36`
address one object, so the arithmetic stays pinned near its use and the stores
cannot be reordered freely against each other. With a struct pointer there is a
single base register and the offsets are part of the store instructions, so all
five stores become interchangeable and the scheduler orders them by the
readiness of the *values*, which is what the target does.

The practical consequence is that the lever is not just worth points, it is
what makes the other axes reachable. Under casts, the store-order permutation
barely moves the diff, because the addressing pins the stores anyway. Under a
struct, the same permutation becomes live and is half of the cross product that
took the function from seven positions to five. A lever that unlocks an axis is
worth more than its own score.

It does not follow that structs are always right, and the same function shows
the boundary. Two isolated `s16` stores in the tail block, at `+48` and `+50`,
match identically whether written as casts or as named fields. Nothing else is
in flight there, so the scheduler has no freedom to exercise and the two forms
cannot diverge. The rule is about scheduling latitude: prefer a struct where
several accesses to one object are live at once, and expect no difference where
a single access stands alone.

Declaring the struct is cheap and reversible. Sizes come from the store widths
already in the target -- `sh` for `s16`, `sw` for `s32` -- and the gaps are
padding arrays. It is worth trying on any function whose residual is
concentrated in a block of stores through a shared base.

## Aligning instruction sequences, and when it stops working

Comparing per-opcode *totals* turns one fault into several unrelated-looking
ones. Aligning the two sequences of a chosen opcode class with `difflib`
instead, and printing each entry with its immediate operand attached, collapses
them back and usually names the statement.

It has found four things that totals had hidden:

- `func_8016913C`'s `sra`: the totals said two `lb` missing and one `sra`
  extra, three separate problems, which had already survived sixteen
  combinations of byte-global signedness without moving. The alignment showed a
  single `replace  target[29:31]=[lb, lb]  cand[29:30]=[sra]` at the end of the
  function, and the statement was a `<< 4` that wanted to be a `* 16`.
- `func_8016913C`'s `subu`: the alignment placed two target `addu` against one
  candidate `subu` and led straight to a hoisted loop invariant.
- `func_80180390`'s missing `slti`: one deletion in the comparison sequence,
  in the entry-visibility test.
- `func_80180390`'s missing `lhu`: four reads of two pad words where the build
  had three, which is what a missing `volatile` looks like.

**It only works when the operands survive into the candidate.** The immediate
is the discriminator, and in an unlinked candidate every relocated field is
zero. On `func_80180390` the useful entries were struct member offsets, which
are structural and print as 3, 4, 5, so the two sides lined up. On
`func_8016913C` every narrow access is to a relocated scalar global, so the
candidate side is a row of `('lb', 0)` and carries no information at all; the
alignment there is noise and the four leads above came from the *class*
sequence, not the operands.

So: attach operands when the values are structural -- struct offsets, shift
counts, comparison immediates, stack slots -- and expect nothing from them when
the class is dominated by `%lo` relocations. In the second case fall back to
aligning the bare class sequence, which still localises, or read the target's
disassembly directly.

## A sweep label is a file name, so punctuation can silently corrupt a result

`overlay_sweep` writes each cell to `tmp/overlay-sweep/<label>__<profile>.c`, and
until now every character the file system might object to became an underscore.
Two labels that differ only in punctuation therefore collapsed to the same name.
`a==1|b<2` and `a>=1|b<2` both became `a__1_b_2`, the parallel workers raced on
one file, and cells measured whichever neighbour won.

This is worse than a crash because it looks like data. An eighteen-cell product
over two comparison spellings and an arm order came back with several groups of
identical numbers and, decisively, an *identity cell that did not reproduce its
own base*: the run said 12 where the unmodified source measures 10. That is the
tell, and it is the reason to always include the unmodified source in a sweep
under its own label. If the identity cell does not return the number you already
know, stop and find out why before reading anything else in the table.

The stem now carries a digest of the untouched label, so the name stays readable
and the collision cannot happen. Re-running the same product with the fix gives
a correct identity cell and three distinct results where there had been one.

Historic sweeps are mostly unaffected, because the labels used have been
alphanumeric with hyphens and underscores, which the sanitiser preserved. The
exposure was to labels containing operators, which is exactly what a comparison
or arithmetic axis invites.

## Steer by the agreeing prefix once register allocation is what is left

Opcode distance is an L1 distance between two opcode multisets. That makes it
blind to order, which is known, but it also makes it *cancel*, which is not
obvious and is much more damaging. An error that adds an instruction and an
error that drops one sum to nothing, so a candidate can carry two real faults
and score zero for them.

The consequence is that correcting one of a cancelling pair makes the number
worse. On `func_8016913C` the transition's distance was written as
`home = 16 - w->f3C`, and the target computes `w->f3C - 16`. Fixing it raised
the reported distance from 10 to 12, because the extra `li` of the constant 16
had been paying for an instruction the candidate was missing elsewhere. Every
sign said the correction was a regression, and it was the single biggest step
forward the function has had.

The length of the leading run of instructions whose mnemonics agree cannot
cancel, because it stops at the first disagreement. `overlay_sweep` now
reports it as `pfx` and takes `sort="prefix"`. Compare mnemonics rather than
encodings: the registers are exactly what is still wrong when the shape is
right, so an encoding comparison stops at the first differing allocation and
reports nothing.

Use `sort="distance"` while the candidate is still gaining and losing whole
instructions, and `sort="prefix"` once the instruction count is right and
allocation dominates. On the run that introduced it the prefix went from 15 to
91 while the distance went from 10 to 12, and the differing-position count
independently fell from 321 to 281, so the two metrics that cannot cancel
agreed with each other and only the multiset dissented.

Two smaller consequences are worth keeping in mind. The prefix saturates on a
delay slot or a register choice long before the function is finished, so it
guides the front and says nothing about the tail; read the tail with the
alignment or the audits. And it costs two disassemblies per cell, which is why
it needed `disassemble` to stop writing one fixed file.

## `disassemble` used one fixed file, so parallel callers read each other

`overlay_diff.disassemble` wrote `target.bin` in the build directory and ran
objdump over it. Every worker in a parallel sweep wrote that one path, so a
worker could disassemble a neighbour's bytes and get a perfectly plausible
listing for the wrong function. This is the same failure as the sweep label
collision below, in a second place, and it was found the same way: a metric
computed in parallel disagreed with the same metric computed serially.

The file is now named for the process and a digest of the words, so concurrent
callers are independent. `opcode_distance` never disassembles, so no distance
ever measured is affected; only listings, and only when produced concurrently.

## Position counts inside fixed windows are only comparable at constant length

Counting differing positions inside the windows a residual is known to occupy
is a sharper instrument than a total, because it distinguishes a change that
repaired one region from one that moved the damage elsewhere. It has one
failure mode, and it is easy to walk into.

The windows are index ranges into the candidate. If a change adds or removes an
instruction *before* a window, everything after it shifts and the window no
longer covers the code it was chosen for. The numbers still print, they are
still plausible, and they are measuring something else.

On `func_8016A37C` a `volatile` qualifier on `D_801D0000` appeared to be the
first thing ever to move three of the four windows, from five, six and
thirty-six to four, five and thirty. It adds one instruction in the case that
precedes two of those windows, so the improvement is the shift and not a
repair; the total differing positions rose from 94 to 146 at the same time,
which is the tell.

Compare windows only between cells with the same instruction count as the base,
and read the total alongside them. Where a change does alter the length, the
windows say nothing and the mnemonic counts or the agreeing prefix should be
used instead.

## Two levers from the first distance-zero result, and where they do not reach

Taking `func_80180390` from six to zero used two shapes that had not been tried
before on any overlay function. Both are worth reaching for early, and both
were then tested on the two other near-misses and do not transfer, which bounds
them usefully.

**Qualify the store, not the object, to force a reload.** Where the target
stores a narrow global and then loads it back, GCC will normally keep the value
in a register and mask it, because it knows the truncation. Writing
`*(volatile u8 *)&g = v;` forces the store to memory and the reload; writing
`extern volatile u8 g;` also forces every other read in the function and is
much worse. The two faults this repairs, a surplus `andi` and a missing `lbu`,
are one instruction seen from opposite sides, so they move together and neither
can be fixed alone.

**Split a two-assignment `if` into two `if`s on the same condition, and get the
order right.** Where both arms assign two variables, GCC emits one conditional
branch and an unconditional jump around the else arm. The target instead
evaluates the condition once and branches on it twice, putting one assignment
in each delay slot and needing no jump. Splitting is worth something on its
own; splitting in the right order is worth several times more, because the
assignment that lands in the first delay slot has to be the one whose register
is free there. On `func_80180390` the unordered split was worth two of six and
the correct order was worth all six.

Neither reaches the other two near-misses. On `func_8016A37C`, qualifying the
stores to `D_8016D438` or `D_8016D49C` is byte-identical and qualifying the
stores to `D_8016D424` costs five, all of them `nop`. On `func_8016913C`, four
splittings of the one two-assignment arm it has are worth nothing or cost two,
because the arm's `gx` assignment is not paired with a matching one in the
other arm and the split leaves an empty branch rather than a filled slot.

The bound is the useful part: the split lever needs *both* arms to assign the
same two variables, and the volatile-store lever needs the target to actually
reload. Neither is a general-purpose spelling change.

## Give a shared local to the block that needs it, once the mix is exact

Once the opcode distance is zero the only thing left is which register holds
what, and the cheapest handle on that is which *local* holds what. A local that
several blocks reuse forces one allocation for all of them; splitting it gives
each block its own and lets the allocator choose differently in each.

On `func_80180390` this paid three times in one sitting, taking the differing
positions from 232 to 206. The three identical save-poll blocks shared one
`value` and all three held the poll result in the wrong register; one dedicated
local fixed all three at once. The fade-out block and one of the eleven
`entry = D_80184560` regions each wanted their own copy of the entry pointer.

Two things about it are easy to get wrong.

It is not additive. Of eleven `entry` regions, one is worth seven positions
alone, two are worth something alone and nothing alongside it, four are inert,
and two cost distance. Giving every region its own local is worse than giving
one region its own local, so the subsets have to be measured rather than
assumed.

It needs a local that is genuinely shared. On `func_8016A37C`, which is also at
distance zero, every candidate is already effectively per-case: `count` and
`step` appear only in the third case, `msg` only in the second, `index`,
`digit` and `card` only in the first, `widget` only in the fourth. Splitting
any of them is byte-identical, and the three `flags` reads are the only real
sharing there, of which one split is byte-identical and the other two cost
seven positions. There is nothing to separate, which is why that function's
residual has not moved for several cycles while this one's has.

## Loop form is a spill-pressure lever, with a known direction

A loop written with a label and a `goto` carries no `NOTE_INSN_LOOP_BEG`, so
`loop.c` never scans it. It gets no invariant hoisting and no strength
reduction. The same loop written as `for`, `while` or `do`/`while` does, and
those three are byte-identical to each other, so the only choice that matters
is goto against structured, not which structured form.

That makes loop form a lever on register pressure with a predictable sign:

- **Structured** enables hoisting, which creates a value that must live across
  the whole body, which costs spills.
- **Goto** suppresses hoisting, which keeps the body's pressure low and
  rematerialises addresses instead.

Both directions have now been measured on real functions. On `func_8016913C`
the walk after a negative cell was a `goto` loop, so the `%hi` of its store
target was rebuilt every iteration; making it a `while` hoisted that `%hi` once
and removed **both** of the surplus `lui` the row had been stuck on, because
every global in that range shares a high half. On `func_80180390` the opposite
holds: its entry loop is a `goto` loop and must stay one, since every
structured form hoists an address and pays two extra `sw`/`lw` for it, moving a
candidate whose instruction mix was already exact to opcode distance 5.

So the rule is not "prefer structured loops". It is:

> If the residual is a surplus `lui` inside a loop, the loop probably wants to
> be structured. If the residual is surplus spills, or the mix is already
> exact, the loop probably wants to be a `goto`.

Read the sign off the per-mnemonic counts before changing anything, because the
two cases look identical in the opcode distance alone.

## Variable identity is register allocation, in both directions

GCC 2.8.1 has no SSA. Every reference to a C local belongs to one allocno and
receives one hard register for the whole function, so the set of variables you
declare *is* the input to register allocation. When a candidate is distance
zero and only registers are left, the variable set is the remaining lever, and
it works in both directions.

**Splitting** a variable shortens live ranges and raises the
frequency-weighted priority of the part that sits in a loop. Three separate
gains on `func_80180390` came from this. A pointer reassigned both inside and
outside the entry loop was holding `a1`; giving the loop its own copy moved it
to `a0`, which the rest of that block is allocated around. A later loop that
reused the first loop's counter and walking pointer was stretching both live
ranges enough to produce a three-way rotation, in which `frame`, `slot` and `i`
held `s2`, `s0` and `s1` where the target has `s0`, `s1` and `s2`; giving the
second loop its own counter and pointer undid it. And reusing a name that was
already live in an unrelated block silently merged two ranges — using a fresh
name instead took the differing positions from 43 to 28.

**Merging** is just as often the answer, and it is easy to miss because it
looks like worse source. Read the target for a register that holds several
unrelated values in succession: that is the signature of one C variable reused,
not of three temporaries. On `func_80180390` the target's `v0` holds three
different structure fields one after another. While those were three separate
expressions the third load had a free register and hoisted into a load-delay
slot the target leaves empty; once they shared one variable the third load
could no longer move above the second's use and the `nop` came back. That
single change improved the agreeing prefix, the differing positions and the
mnemonic subsequence at the same time, which is rare enough to be worth
recognising as a signature of the right lever.

The corollary matters too. Where the target holds a pointer in a caller-saved
temporary for two or three instructions, the source dereferenced it inline; a
named variable forces a longer range and a different class. Declaration order,
by contrast, is inert: twenty-four random permutations of twenty-two
declarations produced byte-identical output, so it is not the tie-break it
looks like.

## `volatile` is a scheduling instruction, so measure every occurrence

A volatile memory reference is a scheduling barrier, and on these modules that
is usually what it is being used for rather than a statement about hardware. On
`func_80180390` a volatile store on an accumulation prevented the `li` of a
return value below it from reaching a branch delay slot, so it drifted up into
a load-delay slot the target leaves as `nop`; removing that one qualifier moved
the agreeing prefix from 190 to 204. Two other volatile writes in the same
function are genuinely required — removing them costs four and two instructions
— and on `func_8016A37C` all three volatile externs are required, each removal
losing an instruction.

So neither adding nor removing volatile is a rule. Enumerate the qualified
accesses and sweep the subsets; it is a small product and it distinguishes the
occurrences that carry semantics from the ones that are accidentally pinning
the scheduler.

## `return` versus `goto` only pays where the delay slot carries a value

Rewriting an early `return` as a `goto` to a shared exit changes code only when
the function returns a value. In a value-returning function the two forms are
genuinely different: `if (cond) return A; return B;` lets GCC put each return
value in its own branch or jump delay slot and send both straight to the shared
epilogue, whereas reaching a labelled block costs `{j +1, nop +1}` for the
extra jump. On `func_80180390` the choice is not even uniform across the
function — the site after the entry loop wants the inline `return` so its value
fills a `bnez` delay slot, while the very next test wants the `goto` so it
branches to the shared `li v0,-1` block. Sweeping all eleven sites one at a
time was the only reliable way to tell, and it was worth about a hundred
positions of agreeing prefix.

In a `void` function the lever does not exist. All nineteen `return;`
statements in `func_8016A37C` were swept against a shared `goto`, and every one
produced byte-identical output, because both forms are already just a jump to
the epilogue with nothing to carry. Do not spend cells on it there.

## A block boundary decides what the scheduler is allowed to fill

Signed division by a power of two expands to a rounding branch, which splits
the basic block. Anything the target schedules on the far side of that branch
cannot have come from a source statement after the division, and anything it
schedules between the multiply and the branch must come from a statement before
it. On `func_80180390` the target reloads a pointer between `mult` and the
rounding `bgez`, so the reload has to precede the divide in the source, and the
product needs its own local or the divide pays a `move` because its destination
cannot coalesce. Writing it as three statements — product, reload, divide —
moved the agreeing prefix from 244 to 283.

The same reasoning applies to hazard slots that the assembler rather than the
compiler fills. A `nop` after `mfhi` appears only if GCC emitted the consuming
instruction close behind it, so a `nop` there is evidence about GCC's
instruction order, not about scheduling freedom. Splitting a store off its
computation kept the following statement's address setup from being hoisted
into that slot and took the mnemonic subsequence from 492 to 494.

## Classify a difference before counting it, and align before classifying

A raw position count answers "how many words disagree", which is rarely the
question. Three quite different things produce a disagreement, and only one of
them is work:

- **Relocation masking.** `candidate_words` zeroes relocated fields, so every
  `%lo` displacement and every call target reads as `0` in the candidate. These
  are not differences at all.
- **Register naming.** Same mnemonic, same immediates, different registers.
  Real, but only fixable through allocation.
- **A genuine difference.** A different mnemonic, or the same mnemonic with a
  different immediate or displacement where neither side is a masked
  relocation.

Separating the three is mechanical: take the mnemonic, the register list and
the numeric-literal list of each side; equal literals with different registers
is naming, a candidate literal of `0` against a non-zero target literal in a
relocated field is masking, and anything else is genuine. On `func_8016A37C`
that splits 206 raw disagreements into 74 register renames, 51 masked
relocations and 81 genuine ones, and the 81 turn out not to be independent at
all.

The second half matters more. Before reading any of those numbers, align the
two streams with a sequence matcher keyed on mnemonic plus immediates, so that
register renaming does not block the alignment. What looked like four immovable
clusters spanning a hundred words on `func_8016A37C` is, once aligned, three
insertion points: the candidate is missing a `lui` at word 236 and another at
word 267, and materialises one it should not at word 279. Everything else in
those "clusters" is the same instructions shifted by one and renamed. That is a
three-instruction problem described as a ninety-four-word one, and the two
descriptions suggest completely different work.

The related trap is in the other direction, and it cost several runs on
`func_80180390`. Normalising a field so registers can be compared in isolation
also hides every difference in that field: two `lh` instructions were reported
as agreeing when they differed in their offsets, because the offsets had been
normalised away. Read a normalised diff against an unnormalised one before
concluding a region differs only by register.

## A shared `%hi` in the target does not mean a shared symbol

When the target reaches two nearby globals through the same register, it is
tempting to read that as one object accessed at two offsets, and to group the
declarations into a struct or an array. That inference has now been tested twice
on `func_8016913C` and is wrong both times.

The first case looked conclusive. The target stores to 0x8016D401 and loads from
0x8016D402 through `a0` at displacements -11263 and -11262, one byte apart,
which is exactly what an array at two indices produces. Declaring them as one
struct or one array, across four variants covering signed and unsigned element
types, moved the opcode distance from 2 to 22 and the agreeing prefix from 91 to
57. Reading the surrounding instructions rather than the two in isolation shows
why: the two `a0` values come from different `lui` instructions either side of a
reload, so each byte carries its own high half and the shared register name is a
coincidence of allocation.

The second case is subtler and the same answer. Three pad globals at 0x8009B394,
0x8009B398 and 0x8009B3A4 share a high half, because all three have a `%lo` with
the top bit set and therefore the same `%hi` of 0x800a. Grouping them into one
`u16` array, one `u32` array or one struct with named fields all make things
worse, by four on the distance and four on the instruction count, because GCC
emits `%hi(sym)` per symbol and a grouped declaration changes which symbol every
access names.

The rule that survives is narrower than it first appears. Two accesses sharing a
`lui` is evidence about register allocation and about the page the addresses lie
in, not about the declaration. Before grouping, check whether the shared register
is written more than once in the region; if it is, the sharing is allocation and
the symbols are separate. Grouping is worth trying when it buys aliasing, as
`MEM_IN_STRUCT_P` does, but not on the strength of a shared base register alone.

