# Build and Match Workflow

## Workspace contract

Run every command from the repository root. Project entry points validate the
current directory and reject paths that resolve outside the workspace.

Project-managed dependencies are installed beneath `tools/`. Generated files,
package caches, compiler scratch files, split output, linker maps, and rebuilt
executables stay beneath `tmp/`.

## Inputs

Supply the North American `SLUS-01411` files beneath `game/`. They are ignored
by Git and validated against `config/slus_01411/files.sha256`.

```sh
make verify-inputs
```

The executable must have SHA-256:

```text
84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88
```

## Local tools

```sh
make tools
make check-tools
```

The bootstrap installs:

- A copied Python 3.10 environment with hash-locked Python packages.
- Splat, spimdisasm, Rabbitizer, and `m2c`.
- Pinned checkouts of asm-differ, maspsx, and decomp-permuter.
- GNU binutils 2.42 configured for `mipsel-none-elf`.
- A pinned `mips-sony-psx` GCC 2.8.1 probe compiler built from the public
  decompals/old-gcc recipe. This is the default selected by `make tools`.

Downloaded archives, installed packages, source checkouts, and toolchains remain
under `tools/`. Temporary build directories remain under `tmp/`.

To use the pinned prebuilt GCC 2.8.1 release instead of compiling that probe,
run the component targets rather than `make tools`:

```sh
make python-tools toolchain compiler-281-prebuilt compiler-272
make check-build-tools
tools/environments/python/bin/python tools/bootstrap/old_gcc_272.py --check
```

The prebuilt installer verifies release `0.17` and its archive/member hashes,
then installs the compiler at the same project path used by the profiles. Its
wrapper is named `mips-sony-psx-gcc`, while the packaged driver reports
`mips-linux-gnu`; the explicit profile flags and PSX macro definitions provide
the required target behavior. `make check-tools` validates the source-built
installation, whereas `make check-build-tools` validates the prebuilt one.

### Host requirements, and what breaks on a newer distribution

The bootstrap pins its dependencies exactly, which is the right default and
also means a host newer than the pins needs three specific accommodations.
Everything below was reproduced on **Ubuntu 26.04 LTS with GCC 15.2.0**; the
pins themselves are correct and none of this asks for them to be loosened.

**The Python environment is pinned to CPython 3.10 exactly.**
`tools/bootstrap/tools.json` declares `major_minor: [3, 10]` and
`bootstrap.py` enforces it with `sys.version_info[:2] != expected`. Ubuntu
26.04 ships only 3.14, and 3.10 is not packaged for it. A standalone
interpreter satisfies the check without touching the system:

```sh
uv python install 3.10
make python-tools BOOTSTRAP_PYTHON="$(uv python find 3.10)"
```

**`make toolchain-system` is unusable there.** It requires
`binutils-mips-linux-gnu` at exactly `2.38-1ubuntu1cross2`; 26.04 ships
`2.45.90.20260125-1ubuntu1cross1`, so the check fails and
`USE_SYSTEM_MIPS_BINUTILS=1` is not an option. The from-source `make
toolchain` is the only route, which leads to the next point.

**binutils 2.42 does not compile under GCC 15.** GCC 15 defaults to C23, where
`static_assert` is a keyword, and the build stops at:

```text
opcodes/mips-formats.h:86:7: error: expected identifier or '(' before 'static_assert'
```

Selecting the older dialect builds it unmodified:

```sh
CFLAGS=-std=gnu17 make toolchain
```

`make compiler-281-prebuilt` avoids compiling GCC and needs no multilib
development files on x86 hosts. Its real driver is a statically linked 32-bit
i386 executable, so non-x86 hosts must use the source-build path instead.

A full `make clean match` on such a host reproduces
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`, so the
pinned toolchain itself is portable; only these three bootstrap steps notice
the host.

## Analysis pipeline

```sh
make info
make extract
make map
make split
make inventory
make classify-functions
make progress
make verify-disc
```

- `info` prints the verified PS-X EXE header.
- `extract` writes the header, loaded payload, and parsed metadata to
  `tmp/extract/slus_01411/`.
- `map` validates every top-level byte range and its SHA-256.
- `split` deletes only the previous `tmp/generated/` and `tmp/splat/` output and regenerates
  disassembly, data assembly, binary regions, linker diagnostics, and automatic
  symbols there. Before Splat runs,
  `tools/project/generate_build_config.py` combines the static split template
  with `config/slus_01411/matching_c.json` and writes generated manifests under
  `tmp/generated/`.
- `inventory` reconciles generated function boundaries with the tracked
  `config/slus_01411/functions.csv`.
- `classify-functions` applies the verified game/CRT/SDK ownership ranges
  without overwriting a future `matching_c` status.
- `progress` refreshes the generated progress section in the root `README.md`
  and writes current status and ownership metrics to
  `tmp/reports/progress.json`.
- `verify-disc` verifies the original MODE2/2352 BIN/CUE, tracked ISO9660 LBAs,
  and every extracted file against its disc extent.

The exact build uses Splat's generated `tmp/splat/slus_01411.ld`. Its tracked
source of truth is `config/slus_01411/split.yaml`, combined with
`config/slus_01411/matching_c.json` by
`tools/project/generate_build_config.py` before Splat runs.
`config/slus_01411/c_symbols.ld` and
`config/slus_01411/link_symbols.ld` supplement the generated script with fixed
aliases and layout symbols; generated linker output under `tmp/` is not source
and must not be edited.

## Mapping a section to a source file

A Splat subsegment says two things: which byte range it covers, and where the
bytes are to come from. For everything that is not text there are two
spellings of the same section, and they mean opposite things:

| spelling | meaning | what Splat emits |
|---|---|---|
| `.rodata`, `.data`, `.sdata`, `.bss` — **with a dot**, plus a source file name | this section is produced by **our** C file | `build/.../<name>.o(<section>);` |
| `rodata`, `data`, `sdata`, `bss` — **no dot** | extract this range from the original image as a blob | `<name>.<section>.o(<section>);` |

So converting data is the same move as converting code: a definition leaves
the extracted blob, moves into the C file that owns it, and a dotted
subsegment names that file at the address the definition has to keep. The blob
shrinks; the boundary moves.

The resident initialized-data ranges are code groups even though they have no
text. That lets their extracted `.data` and C-owned `.sdata` contributions
share fixed-address ranges without flattening both into the same linker
section.

`section_order` is the other half, and it is easy to misread as a constraint.
**It is a description of the image's layout**, applied within each segment. If
a module is laid out rodata-then-text, the option should say so; the answer to
"the rodata is landing after the text" is to describe the image correctly, not
to build a segment to work around it. Getting this wrong looks like a tooling
limitation and is not one.

### Three ways this fails without telling you

Layout mistakes here are only caught by the final hash, so it is worth knowing
the shapes in advance.

- **Anything nobody placed is dropped.** The generated Splat script ends with
  `/DISCARD/ : { *(*); }`. A section that no line claims does not fail the
  link; it silently disappears.
- **Undeclared object sections are appended, not placed.** Splat emits a
  `(.rodata)` line for *every* C object in a segment, so a file without an
  explicit dotted subsegment still gets one — at the end of the run. That is
  harmless only while the section is empty. The first time such a file gains a
  string or a table its bytes land in the wrong place and push everything
  after them down.
- **A zero-valued global is not `.data`.** Written as `u8 x;` or `u8 x = 0;`
  it goes to `.bss`. Declaring a `.data` subsegment for a file whose globals
  are all zero yields an empty section and loses the blob's bytes.

The resident script guards its largest section explicitly —
`ASSERT(SIZEOF(.text) == 0x7DCFC, ...)` — which turns one class of this into a
link error rather than a hash mismatch. The generated overlay scripts have no
equivalent, so there the hash is the only check.

`notes/overlays/README.md` has the worked example: a printf format string
moved out of an extracted blob into the C file that uses it, placed ahead of
the module's text, with all five overlay modules still byte-exact.

### Owning resident initialized data

`src/game/file_names.c` is the worked example for the resident image: the
loader's seven disc paths and the null-terminated `gFile_apszName` table, 0xA8
bytes at 0x80090704, come from that file instead of the extracted blob.
`ai_script_commands.c` and `ai_opponent_data.c` are the other two, an opcode
table of relocations and a typed record array. Five things make one of these:

1. **Split the blob in `config/slus_01411/split.yaml`.** The owning file gets a
   dotted subsegment at its address and the remainder keeps going to a
   generated blob:

   ```yaml
   - [0x80ee0, data, initialized_data_800906e0]
   - [0x80f04, .data, game/file_names]
   - [0x80fac, data, initialized_data_800907ac]
   ```

   A blob chunk is named for the address it starts at, so taking ownership of
   one range never renames another and two people can split different parts of
   the segment without colliding.

2. **Only a unit with no text of its own can own `.data`.** A text segment's
   linker script lists `.rodata`, `.data` and `.bss` for every one of its
   objects, so those sections are claimed there first and a matched function's
   file cannot also supply them from another segment; `make split` says so
   rather than letting the bytes land in the wrong place. It lists no
   small-data section, which is why `.sdata` and `.sbss` can go back into the
   matched text unit that owns them, the way `ai_script_call_control.c` does.
3. **Name the profile in `config/slus_01411/data_c.json`.** Data units are not
   in `matching_c.json`, which describes functions; the build gets its compiler
   profile from this manifest, and `make split` rejects a file that owns a
   dotted section without one, or a manifest entry the template never maps.
   `gcc_2_8_1_g0` is the profile to use while the unit's own small-data
   placement has not been worked out: at `-G0` every definition lands in
   `.data`.
4. **Declare the symbols `extern` in a header** (`src/game/file_names.h`) and
   delete every other declaration of them, including any entry in
   `config/slus_01411/c_symbols.ld`. A file-scope definition and a linker alias
   for the same address are not interchangeable, and while both exist the alias
   silently wins.
5. **Give a definition a name of its own.** The image has more than one copy of
   several strings - the `\DATA\SU.MRG;1` the loader opens is not the
   `M:/mrgSU/SU.mrg` development path already named `gFile_szSuMrgPath` in the
   read-only region - and the link fails loudly on the duplicate, which is the
   good case.

6. **Carving `.sdata` out of the middle of a mapped blob moves the symbol.**
   Splitting an extracted range into blob, `.sdata` subsegment, blob and
   defining the symbol in its owning text unit builds and links, but the
   image does not match: the symbol lands after the other small-data
   contributions rather than at its own address. Taking one symbol in
   isolation is not enough, because the `.sdata` output section is filled in
   linker-script order and every contribution before it decides where it
   starts. This is what the issue means by figuring out the order first.

   The mismatch names the culprit precisely, which makes this cheap to
   diagnose. `D_8009AF74` sits at `_gp + 0x6C`, and the failure was a byte in
   *text* at VRAM `0x800401a4` reading `0x74` where `0x6c` was expected - the
   gp-relative displacement of the instruction that loads it, moved by exactly
   the eight bytes the definition added. When a data change breaks a byte
   inside code, read the differing value as a `%gp_rel` displacement and
   subtract `_gp` (`0x8009AF08`) to find which symbol shifted and by how much.

   The fix is to classify the whole GP-relative tail as `sdata` in image order,
   not to isolate one symbol from a range still called `data`.
   `save_data_mask_state.c` is the worked example at `0x8009AF64`: before the
   tail was classified, its two words moved from `_gp + 0x5C` to `_gp + 0x174`;
   after the extracted ranges from `_gp` onward became ordered `sdata`, splitting
   the `0x8009AF40` blob around those words preserved both their addresses and
   every existing text relocation.

**The segment holding the data has to be a `code` segment.** Only a group
segment adds each of its subsegments to the linker script; a segment declared
`type: data` emits one line for itself, so the C object is built, is never
named by the script, and is silently dropped while the blob still supplies the
original bytes - a full `make match` that proves nothing. The resident
`initialized_data` segment is therefore `type: code` with no text subsegments,
exactly like the leading read-only `main` segment.

**Assembly that still references the range keeps working.** `D_800907AC` is
reached by two functions that are still extracted assembly; when
`duel_field_layout.c` took ownership of it, those `%hi`/`%lo` references
resolved to the C definition, with no entry added to
`undefined_syms_auto.txt`. Splat only invents an absolute symbol for an
address it cannot attribute to a segment, and a dotted subsegment is an
attribution.

The build reads the generated `tmp/generated/data_sources.json` the way it
reads `text_sources.json`, so both `make match` and `make match-incremental`
compile and place these units; editing one rebuilds one object. Before
building any of them it checks that the generated linker script actually names
each object, because that is the one layout mistake the byte-exact comparison
cannot catch: an object nobody places is not loaded, the blob keeps supplying
the original bytes, and the build still matches.

### The small-data region

`.data` runs to 0x8009AF08 and `.sdata` from there to 0x8009B090, which is
where `_gp` points; the template says so, with that tail declared as `sdata`
blob chunks rather than `data` ones. The ordering is what makes ownership
possible at all: Splat emits a segment's whole `.data` list before its
`.sdata` list, so a unit owning small data in the middle of a `data` blob
would land after every byte of it. Declared as small data, blobs and owning
units interleave in address order.

Small data goes back into the **matched translation unit that owns it**, not
into a data-only unit: at `-G8` a definition of eight bytes or fewer lands in
`.sdata` by itself, and a text segment claims no small-data section.
`duel_trap_resolution.c` owns the six trap thresholds and `duel_card_effects.c`
the two life-point tables this way.

**The consumer's addressing form says whether it owns the symbol.** At `-G8`
the assembler resolves a small global `%gp_rel` - one instruction - only in the
translation unit that *defines* it; everywhere else it is `lui %hi` + `%lo`,
two. So a file that reaches a small symbol through the two-instruction form did
not define it in retail, and moving the definition there makes its text four
bytes shorter. `duel_magic_effect_dispatch.c` reads the `"%d\n"` trace format
at 0x8009AF40 that way, so that symbol belongs to some other unit and the blob
keeps it; `duel_trap_resolution.c` and `duel_card_effects.c` reach theirs
gp-relative, which is why the definitions land there and the build stays
byte-exact. The failure is loud but indirect: the shortened function shifts
every jump-table entry after it.

**A `sdata` blob chunk stops at its last non-zero symbol.** Trailing zero
bytes are padding to spimdisasm and it does not emit them, so a chunk is
shorter than the range it covers and everything after it starts too early. The
fix is a `pad` subsegment whose *address is where the emitted content actually
ends*, not the nominal boundary: `initialized_data_8009af2a` covers six bytes
but emits four, so `- [0x8b72e, pad]` before the next entry makes up the
difference. A missing pad shows up as a two-byte shift in every `%gp_rel`
reference after it, and the build's size check catches the rest.

### Which arguments a callee actually reads

Several consumers declare a function with fewer parameters than its definition
takes and call it with fewer arguments. That is not always a mistake, and it is
not always safe to correct, so the question has to be settled per call site.

The argument register is not empty at such a call. It usually holds the
*calling* function's own incoming parameter, still live because nothing
clobbered it, so the argument was being passed all along and simply was not
written down. Whether it can be written down depends on one thing:

> A no-argument call site can be given its arguments when every register the
> callee READS already holds a value the calling file can name.

Both outcomes occur, and they look identical in C:

- `func_800429D8` reads only `$a0`, and its two `void (void)` consumers are
  functions taking `u8 *object` whose prologue is `addu $16,$4,$zero` without
  reassigning `$4`. `$a0` still holds the object, so naming the argument is
  byte-exact.
- `func_80023D08` reads `$a0` and `$a1` and branches on the second. Its
  one-parameter consumer sets only `$a0`, so `dir` arrives as that function's
  own caller's leftover. There is no expression to write, and the narrow
  declaration has to stay.

To decide, find the callee's assembly and take the FIRST mention of each of
`$a0` to `$a3`, then ask whether that mention is a read or a write. A first
mention that is a write means the register is scratch, not an argument.

Two traps make this easy to get wrong:

- **Counting mentions is not analysis.** A function that sets up its own calls
  writes `$a0` to `$a3` constantly. `func_800235C0` mentions them 119 times and
  reads none of them; its first use of each is `addiu $aN, $zero, imm`.
- **The first operand is not always the destination.** For stores and branches
  it is a source, so `sw $a1, 0x54($sp)` and `bltz $a3, .L…` are READS. Reading
  them as writes makes a four-argument function look like it takes one --
  `func_8004CB0C` spills `$a1` and `$a2` to the stack in its prologue and
  branches on `$a3`, and reads all four.

### The declaration spelling is the consumer's lever

Ownership decides where a definition lands. For the far more common case of a
*consumer* that does not own a small symbol, the `extern` spelling is what
picks the addressing form, so several translation units will declare one symbol
incompatibly on purpose. Which lever a file needs follows from the `-G`
settings of its profile in `compiler_profiles.json`, so the spelling is a
property of the profile rather than of taste:

| Profile shape | What a byte- or halfword-sized global gets | Lever needed |
| --- | --- | --- |
| compile `-G0`, assemble `-G0` | already `lui %hi` + `%lo` | none; a plain scalar is correct |
| compile `-G8`, assemble `-G8` | `%gp_rel` | an array, or `section(".data")` |
| compile `-G8`, assemble **`-G4`** | `%gp_rel` | an array with a size the assembler can see is **above 4** |

The array length is a threshold, not a claim about storage. Two symbols carry
the pattern today, and both would overrun their neighbours if read literally:

- `gDuel_bTerrain` (0x8009B364) is one byte -- `gFreeDuel_bReturnFlags` sits at
  0x8009B365 -- yet is declared `[8]` and `[]` as well as a plain and a
  `section(".data")` scalar, across eight files spanning all three rows above.
- `gSD_bOutputType` (0x8009B408) is read only at index 0 yet is declared `[16]`
  and `[9]`. `options_init.c` states the reason inline: it "needs an oversized
  array extern to force absolute (lui+lbu)".

The last row is why the forms are not interchangeable, and it is worth
measuring rather than assuming. Relaxing `func_80024E58.c`'s `[8]` to an
incomplete `[]` costs four bytes of text, because that file assembles at `-G4`;
the identical relaxation in `func_8001798C.c`, which assembles at `-G8`, is
exact. `options_init.c` is the second file in the tree on the `-G4` assembler
arm, and it carries a sized array for the same reason.

So a run of incompatible declarations of one global is not automatically drift
to be collapsed. Check the profiles first: if the spellings line up with the
table, they are load bearing, and the useful work is recording which lever each
file pulls rather than unifying them.

#### Where the per-symbol answer is already written down

The table says which lever a profile needs. For a specific symbol there is
usually a better source than reasoning: `config/slus_01411/external_attempts.csv`
records, on every `matched` row, why the accepted source was spelled the way it
was. Eighty-seven of those notes explain an addressing choice, and between them
they name fifty-two globals.

They are worth reading before touching a declaration, because they cover cases
the table alone does not predict:

- **Array versus scalar decides who allocates the register.** For
  `func_8005B64C` the note records that `D_8009B058`, `D_801DD000` and
  `D_801AF800` "are arrays so `-msplit-addresses` gives them registers, while
  the scalars carry `section(.data)`" and rebuild their address per access.
- **Defining rather than declaring is itself a lever.** `func_8002BFCC`'s note
  explains that the assembler only resolves a small global gp-relative when the
  translation unit *defines* it, which is what supplies a missing load-delay
  `nop`. The same reasoning is recorded for `D_8009B142`/`143`/`144`.
- **Oversizing is deliberate.** `D_8009B488`, `D_8009B48E` and `D_8009B490`
  "needed sized array declarations to land in small data as `%gp_rel`", while
  `D_800F5678` needed the incomplete form so its `lui %hi` would hoist into a
  branch delay slot.
- **A neighbour's spelling can be the reason.** Declaring `D_8009B260` as an
  eight-byte aggregate is what keeps it non-small "while the four-byte
  `D_8009B20C` remains gp-relative".

#### Where a shared declaration belongs

Once a spelling is settled, the remaining question is which header holds it,
and the answer follows the include graph rather than the name.

`D_8009B318` is a movie playback state byte. `movie_playback_control.h`
describes exactly what its bits mean and is the obvious home by name, but
`func_80043BCC.c` does not include that header at all, while all three
consumers already include `graphics_frame.h`. Placing the declaration by what
the include graph is, rather than by which subsystem the name sounds like,
costs no new include and cannot strand a consumer.

The same test decides whether a symbol is `unmatched.h` material at all. That
header says it is for symbols that are still homeless, and the difference
between *homeless* and *merely undeclared* is whether the consumers share
anything above `types.h`:

- The three text banks are read by four files that all include
  `text_constants.h`, which already defines the constants their expressions
  use, so they went there.
- `D_800EFE18` is used by four files that all include `mem_card.h`, so that
  centralization added no include at all.
- `D_8009B363` is written by four files spanning four subsystems with no
  header between them, so it went to `unmatched.h`.

#### A neighbour can refute a size, never establish one

Both directions come up, and only one of them is sound.

Refuting works. `D_8009B0A3` is declared `[9]` by one consumer, and
`c_symbols.ld` names `D_8009B0A4` one byte later, so nine bytes would run
through that and past `gGraphics_bActiveBuffer`. The bound is therefore an
addressing lever rather than a size, the same as `gDuel_bTerrain`'s `[8]` and
`gSD_bOutputType`'s `[16]`.

Establishing does not. That the next name sits *n* bytes away shows only that
nothing is named inside those *n* bytes; the object may be shorter, and
consecutive `D_` names are often interior elements. `D_8009B23A`'s next name
is ten bytes on and its consumers read a halfword, so the gap was recorded as
an upper bound and the bytes above it left unclaimed.

A real size needs a reader. `D_800F2878` takes its length from
`DisplayObject_ResetPool`, which advances one pointer into it and one into
`D_800EFE38` together for `DISPLAY_OBJECT_LIST_COUNT` iterations; the
per-list renderer table takes its seven from the loop that walks it. Those are
sizes; a neighbour's address is not.

#### Two things that look like disagreement and are not

A scan over declaration spellings reports both of these, and neither is work:

- **Arms already in place.** Three guarded branches for one symbol inside one
  header are three spellings by text and one declaration in fact.
  `D_8009B0C0` in `graphics_frame.h` reads as a three-way conflict to a
  regular expression.
- **A semantic type against a raw view.** When some consumers have adopted a
  real type and others still reach the bytes, that is type adoption partly
  done rather than drift. Arming it would freeze the raw view in place, which
  is the opposite of finishing the job.

#### A worked rejection

`D_8009B058` looks like an ideal candidate for giving a global its real type.
It has one consumer, `func_8005B64C.c`; that file declares it `extern u8
D_8009B058[]` and immediately casts at its only use, `rect = *(RECT
*)D_8009B058`; and the next name, `D_8009B060`, is exactly eight bytes on,
which is `sizeof(RECT)`. Every cheap check agrees.

The attempt record refutes it anyway: the array spelling is what gets the
symbol a compiler-allocated register under `-msplit-addresses`, so
`extern RECT D_8009B058` would change how the address is materialised. The
same note also explains the local copy — retail copies the eight-byte `RECT`
by value into a stack slot before calling `LoadImage2`, and omitting the local
leaves the frame eight bytes short.

Both facts were free to read and would each have cost a build to rediscover.

## Exact baseline build

```sh
make build
make match
```

The build performs these steps:

1. Regenerate the validated Splat split beneath `tmp/splat/`.
2. Assemble unmatched resident MIPS text and exact data using the local GNU
   assembler.
3. Compile ordered matching-C segments using
   `config/slus_01411/matching_c.json`, and the C translation units that own
   initialized data using `config/slus_01411/data_c.json`, with the named
   profiles in `config/slus_01411/compiler_profiles.json`, then normalize their
   assembly through maspsx.
4. Convert each classified binary region into a MIPS object.
5. Link all text objects in manifest order with the original VRAM and file load
   addresses.
6. Emit `tmp/project-build/SLUS_014.11`.
7. Compare its complete size and SHA-256 with the supplied executable.

A successful run prints:

```text
MATCH  tmp/project-build/SLUS_014.11
sha256 84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88
```

This baseline does not include the original executable as one opaque blob.
Resident code is regenerated as MIPS assembly, known data ranges are
regenerated as assembly, and remaining binary ranges are separately classified
and excluded from C-decompilation progress.

Current matching totals are generated in the root `README.md`. Run
`make progress` when intentionally refreshing that project-wide snapshot;
routine decompilation changes do not need to update it.

## Incremental edit builds

The clean `split`, `build`, and `match` targets remain the acceptance path.
For repeated edits, use the separate incremental targets:

```sh
make clean
MAKEFLAGS=-j"$(nproc)" make match
# Seed immediately, before changing any source or build input.
tools/environments/python/bin/python tools/project/build_incremental.py --seed-existing

# After each edit:
MAKEFLAGS=-j"$(nproc)" make match-incremental

# Final acceptance:
MAKEFLAGS=-j"$(nproc)" make match
```

Seeding is optional; without it, the first incremental build compiles its
objects. Seeding trusts the objects from the immediately preceding, unchanged
matching build. Do not seed after making edits. The first incremental split
also regenerates once to establish its own validated output snapshot.

`split-incremental` fingerprints the retail target, split configuration,
inventory and mappings, compiler profiles, symbols, relocations, generators,
installed Splat/MIPS dependency contents, and relevant interpreter/disassembler
settings. It repeats the matching-source ownership and grouping checks on
cache hits. It uses the pinned Splat C parser to track the function and
`INCLUDE_ASM`/`INCLUDE_RODATA` names that splitting actually observes; ordinary
C-body and header edits do not require another split. Source-shape, metadata,
tool, or target changes do.

The source-tree scan resolves each path once and supplies those canonical C
paths to both matching-source ownership validation and Splat-visible function
and include-shape parsing. Symlinks that leave the workspace are still
rejected before either consumer runs.

Generated assembly, data, assets, linker scripts, and headers must still match
their recorded contents. Missing, modified, or extra generated files force
regeneration. The scanner prunes `tmp/splat/build` and `tmp/splat/cache`, which
are not split inputs. Optional generated outputs must remain in the tracked
split tree; arbitrary Splat extensions and partial-linker layouts are not
supported by this resident cache. Failed regeneration never publishes a valid
stamp. The stamp lives at `tmp/incremental/split-cache.json`.

The object driver memoizes resolved paths, file hashes, and parsed quoted
includes only for the current invocation. Shared headers and compiler binaries
are read once rather than once per unit or profile; later invocations re-read
their contents, including when a file's timestamp has not changed.
Workspace-validated source and tool paths seed that same canonical-path cache,
so security validation and dependency hashing do not resolve each path
separately. Only active compiler profiles are fingerprinted, so an unused
profile does not require an uninstalled compiler. An already installed object
is retained only when its contents match a valid cached object. Missing or
altered installed objects are restored or rebuilt. The driver reports
`rebuilt`, `reused`, `retained`, and `materialized` counts, with
`reused = retained + materialized`. Every incremental build still relinks and
hashes the complete executable. Run builds sequentially; neither cache is a
concurrent-writer protocol.

Warm object validation reuses the cached object's file size and stats the
installed object once before comparing bytes. A fully unchanged build leaves
the cache JSON untouched; stale object keys are pruned only after a successful
relink, while rebuilt signatures remain checkpointed before linking.

The first rebuilt object is checkpointed immediately. Further signature
updates are written every 16 rebuilt objects, with any remainder flushed before
the full relink. This avoids rewriting the complete JSON cache after every
object while preserving immediate recovery for the common one-object edit and
bounding interrupted bulk builds to at most 15 signatures that must be
recomputed. Cached object files remain content-validated on the next run.

The optimization targets this local worker loop, not repeated CI benchmarking.
CI keeps its existing clean acceptance build, a small cache-regression suite,
and a warm-loop smoke check. It does not upload build/performance artifacts.

Invalidated components compile or assemble concurrently when `MAKEFLAGS`
contains a numeric `-jN` or `--jobs=N`; direct driver invocations default to one
worker and may pass `--jobs N` explicitly. Cache checkpoints are published as
workers finish, while the linker still receives objects in executable order.
A jobserver-only `-j` without a numeric count falls back to one worker rather
than guessing or oversubscribing.

Linking remains sequential. Remaining worker-loop costs include
prerequisite/tool checks, content validation of generated output and cached
objects, and the mandatory full relink/hash.

## Full repository audit

```sh
make audit
```

The audit performs a clean exact build, reconciles the function inventory,
reapplies ownership classifications, and checks:

- Every commit attributed to Copilot uses the `Copilot` name with a
  `+Copilot@users.noreply.github.com` e-mail address for both author and
  committer. Commits from other contributors are accepted as they are.
- A Copilot commit carries no `Co-authored-by` trailer other than Copilot's
  own.
- No supplied game file, generated output, downloaded dependency, installed
  environment, vendor checkout, or local toolchain is tracked.
- Tracked Markdown documentation is under `notes/`, except for per-directory
  `README.md` files and repository guidance at
  `.github/copilot-instructions.md`.
- The worktree is clean after deterministic regeneration.

## Cleanup

```sh
make clean
```

Cleanup removes only these known generated paths when present:

- `tmp/extract/`
- `tmp/generated/`
- `tmp/splat/`
- `tmp/project-build/`
- `tmp/incremental/`
- `tmp/reports/`

It does not remove downloaded tools, toolchains, user-supplied game files, or
the separate binutils bootstrap build directory.
