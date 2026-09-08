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

The resident `initialized_data` segment is a code group even though it has no
text. That lets its extracted `.data` and C-owned `.sdata` contributions share
one fixed-address range without flattening both into the same linker section.

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

**The segment holding the data has to be a `code` segment.** Only a group
segment adds each of its subsegments to the linker script; a segment declared
`type: data` emits one line for itself, so the C object is built, is never
named by the script, and is silently dropped while the blob still supplies the
original bytes - a full `make match` that proves nothing. The resident
`initialized_data` segment is therefore `type: code` with no text subsegments,
exactly like the leading read-only `main` segment.

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

**A `sdata` blob chunk stops at its last non-zero symbol.** Trailing zero
bytes are padding to spimdisasm and it does not emit them, so a chunk is
shorter than the range it covers and everything after it starts too early. The
fix is a `pad` subsegment whose *address is where the emitted content actually
ends*, not the nominal boundary: `initialized_data_8009af2a` covers six bytes
but emits four, so `- [0x8b72e, pad]` before the next entry makes up the
difference. A missing pad shows up as a two-byte shift in every `%gp_rel`
reference after it, and the build's size check catches the rest.

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
