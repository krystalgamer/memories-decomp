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

#### What is left, and why each piece resists ownership

Forty of the forty-five extracted ranges in `initialized_data` are owned by a C
source. The five that are not are not simply the ones nobody has got to yet;
four of them have a specific structural obstacle, and it is worth writing down
which, so the next attempt starts from the obstacle rather than rediscovering
it.

| Range | Section | Bytes | Labels | What blocks it |
| --- | --- | --- | --- | --- |
| `initialized_data_800906e0` | `.data` | 36 | 3 | leads with `initialized_data_start`, a layout boundary |
| `initialized_data_80091958` | `.data` | 38320 | 274 | bulk; no single owner, split it first |
| `initialized_data_8009af08` | `.sdata` | 28 | 6 | holds `_gp` itself, and a pointer; the pointer's window is now C-owned |
| `initialized_data_8009af2a` | `.sdata` | 4 | 3 | overlapping symbols |
| `initialized_data_8009af6c` | `.sdata` | 184 | 45 | scattered head and tail; two coherent interior runs are C-owned |

The four-byte range is the one worth reading closely, because it is the
overlapping-symbol case in its smallest form. Splat emits three labels there:
`D_8009AF2A` covering two bytes, then `D_8009AF2C` and `D_8009AF2D` covering
one byte each. `debug_effect_screen.c` declares the middle one
`extern u8 D_8009AF2C[2]`, so its array view deliberately spans the byte that
`D_8009AF2D` names in its own right, and the same file also declares
`extern u8 D_8009AF2D` and writes it as a scalar. One line uses both views at
once:

```c
FntPrint(D_80010074, D_8009AF2C[0], D_8009AF2D);
```

That is the dual-name rule at a four-byte scale, and it is what stops a C
definition from taking the range: a definition has to emit the labels, and
these labels overlap. `D_8009AF2C[2]` and `D_8009AF2D` are both faithful to
retail, one indexes the pair the up/down repeat adjusts while the other names
the second element on its own, and neither is drift to be collapsed into the
other.

`initialized_data_8009af08` is blocked for a different reason: it opens with
`runtime_gp`, which `symbols.txt` fixes at `0x8009AF08` and which the
`%gp_rel` arithmetic elsewhere in this file uses as the `_gp` base. A C
definition of that word would have to reproduce a symbol the addressing model
is expressed in terms of. It also stores a pointer to
`gFile_PrimaryTransferDescriptor`, so the range carries a relocation as well as
plain bytes.

The name deserves a note, because it looks wrong and is not. `runtime_gp` is
also read and written as an ordinary counter: `main_services.c` does
`cnt = runtime_gp - 1; runtime_gp = cnt;` and resets it to `0x3C` when it goes
negative, matching the `0x0000003C` the range is initialized with, and the
function's own comment calls it the watchdog counter. A GP base is never
decremented, so the two readings genuinely conflict -- but the answer is that
the address carries two independent roles rather than that either reading is
mistaken. `_gp` points at `0x8009AF08` because that is where the small-data
region is centred; the word stored there is a separate variable that happens
to live at the base.

The naming follows the first role, and that is load bearing outside the game
sources: `candidate_pin_audit.py` and `audit_unchiga_candidates.py` both look
the symbol up by name to recover the base, `global_usage.py` special-cases it
beside the `_start`/`_end` boundary markers, and the GPREL16 reach test in
`notes/research/matching-evidence.md` is expressed as ±32 KB around it. So
`runtime_gp` should not be renamed to describe the counter, and the counter is
not drift to be cleaned up either.

That sharpens what blocks the range rather than removing the block. A source
file taking `0x8009AF08` would have to define a watchdog counter under a name
three tools resolve as the GP base, which is a heavier commitment than the
twenty-eight bytes suggest.

#### Who actually owns the remaining bytes

The triage above asks what resists ownership. The prior question is who owns
the bytes at all, and answering it for all five ranges at once changes the
shape of the remaining work considerably.

Every label in each range can be attributed to the functions that reference
it, and `functions.csv` records an owner for each of those functions --
`game`, `psyq/sdk` or `psyq/crt`. Doing that across the whole set gives:

| range | section | bytes | psyq | game |
| --- | --- | ---: | ---: | ---: |
| `initialized_data_800906e0` | `.data` | 12 | 8 | 0 |
| `initialized_data_80091958` | `.data` | 34724 | 34692 | 0 |
| `initialized_data_8009af08` | `.sdata` | 20 | 8 | 8 |
| `initialized_data_8009af2a` | `.sdata` | 7 | 0 | 7 |
| `initialized_data_8009af6c` | `.sdata` | 184 | 0 | 184 |

The large `.data` range is not a game data blob at all. Its 274 labels are
referenced by 246 distinct functions and **every one of them is
`psyq/sdk`** -- `_spu_init`, `_spu_setReverbAttr`, `SpuSetReverbModeParam`,
`StCdInterrupt`, `CD_cw`, `FntOpen` and their neighbours. The text range that
reaches into it opens with `PCopen`, `InitHeap`, `_bu_init`, `OpenEvent`,
`EnterCriticalSection` and the `open`/`read`/`write`/`close` wrappers. This is
the Psy-Q library's own initialized data: SPU voice and reverb state, the CD
streaming machinery, the font system, the heap and event tables.

That is a negative worth stating precisely rather than by implication. No file
under `src/` mentions any of those 274 names -- not the game sources, not the
overlays, not the candidate sources. The single unreferenced label,
`D_80092A68`, appears in neither `c_symbols.ld` nor `symbols.txt`.

The consequence is that the biggest item on the remaining list is not
"bulk remainder, must be split before any ownership". It is vendor data, and
the project already holds that Psy-Q CRT and SDK code is never a
decompilation candidate. The same reasoning applies to its data: there is no
game translation unit that could honestly define it, and inventing one would
assert authorship the image does not support.

Netting the vendor bytes out, the genuine game-owned remainder across all
five ranges is about **199 bytes, all of it `.sdata`** -- the seven bytes at
`0x8009AF2A`, the remaining 184 at `0x8009AF6C`, and eight of the twenty at
`0x8009AF08`. That is a very different target from thirty-four kilobytes, and
it lands entirely in the section the `.data`-before-`.sdata` rule calls the
harder one: a text unit can own `.sdata` in place, but the byte layout then
has to survive the section's four-byte alignment, which is exactly what the
measured failure at `0x8009AF2A` ran into.

Two caveats on the method. Sizes are measured label-to-label, so the final
label in each range is assumed four bytes and the totals are approximate at
the margin; and attribution is by symbol reference, so a byte reached only
through pointer arithmetic from a neighbouring symbol would be credited to
that neighbour. Neither affects the conclusion, which rests on a 246-to-0
split rather than on a close count.

#### The largest game-owned range, and who does not own it

The original 276 bytes at `0x8009AF6C` were previously written off here as a
scattered grab-bag with no coherent translation unit. Three interior ranges
are now C-owned: the 36-byte model/graphics state block at `0x8009AF88`, the
56-byte primitive-template block at `0x8009AFAC`, and the 116-byte handler
state/diagnostic block at `0x8009AFE4`; the 28-byte head at `0x8009AF6C` has
since been converted as well. The remaining 40 bytes are the tail described
below.

Mapping the original 77 labels in address order now gives five pieces:

| range | state |
| --- | --- |
| `0x8009AF6C`-`0x8009AF87` | C-owned mixed-subsystem window |
| `0x8009AF88`-`0x8009AFAB` | C-owned model/graphics state |
| `0x8009AFAC`-`0x8009AFE3` | C-owned model primitive templates |
| `0x8009AFE4`-`0x8009B057` | C-owned model handler state and diagnostics |
| `0x8009B058`-`0x8009B07F` | scattered 40-byte tail |

The four owned ranges prove that placement was never the obstacle. All are
data-only units inserted between `save_data_mask_state` and
`ai_script_source_line_format`, so their position comes directly from the
split template. `model_graphics_state` also proves that mixed byte, halfword
and word fields are viable when each unnamed continuation byte is represented
explicitly: its `.sdata` is exactly 36 bytes and retains the pointer relocation
to `D_80091008`.

`model_handler_state` applies the same measured-layout approach to a more
varied block: five leading state labels, a private halfword continuation, two
word pairs, two mutable words, and eleven fixed 4- or 8-byte strings. That
spelling produces an exact 116-byte section without relocations. The split at
`D_8009B058` leaves the unrelated 40-byte tail extracted.

#### Scatter is not what blocks a carve; gaps are

The 28-byte head above was written off twice here, first as part of a
grab-bag and then as "scattered", on the grounds that its contents have no
common subject. That reasoning was wrong, and the range converted without
difficulty once the right question was asked.

Its contents really are unrelated: a `"%s\n"` format used only by the
still-unmatched function at `0x8002E41C`, two separate one-character strings
read by `mem_card_create_state.c` and `func_80044608.c`, the display-object
`ot_index` array, and the `"MTrk"` MIDI track tag compared by
`sound_sequence_marker_scan.c`. Four subsystems, no shared subject, and no
name for the unit better than its address.

None of that matters. What decides whether a range can be carved is whether
its objects **tile it exactly**, leaving no byte unaccounted for. Here they
do: 4 + 4 + 8 + 4 + 8 is 28, which is exactly the distance to
`model_graphics_state`. Because every byte is claimed, the unit reproduces
the window whatever its contents mean. Coherence is a naming problem, not a
matching problem, and `data_80091510.c` already established that an
address-named unit is an acceptable answer to it.

The corollary is the useful one: a range should be screened by measuring
whether its objects sum to its length, not by reading its contents and
judging whether they belong together. Screening by subject rejects ranges
that would have converted, which is what happened here.

#### The tiling test applied to the rest of the small ranges

Re-screening the remaining small `.sdata` ranges with that test rather than
by subject sorts them immediately.

| range | length | named objects | tiles? |
| --- | ---: | --- | --- |
| `0x8009AF10` | 16 | 4 + 4 + 4, then 4 unnamed | yes, with explicit padding |
| `0x8009AF2A` | 6 | 2 + 1 + 1 | no: two bytes unnamed and unreachable |

`0x8009AF10` converted. It holds two boot-time sizing constants read together
by the code at `0x800129FC`, where they are subtracted from `bss_end`
(`0x00200000` is the console's main RAM size and `0x00002000` the reserve
held back from it), plus a file-transfer pointer that shares nothing with
them. Another address-named unit, for the same reason as before.

Its trailing word is unnamed, carries no reference anywhere in the image and
is zero. Representing it as explicit padding is what `model_graphics_state.c`
already does for its own unnamed continuation bytes, and the section
attribute matters more on padding than on anything else in these files: a
zero-valued object without one is placed in `.sbss`, and the window then
comes up short by exactly that much.

This also settles a doubt recorded earlier, that a pointer relocation in
`.sdata` might obstruct a carve. It does not. `D_8009AF18` initialises to the
address of `gFile_PrimaryTransferDescriptor` and links unchanged, as
`D_8009AF88` already did.

`0x8009AF2A` stays extracted, and now for a stated reason rather than a
vague one: its three labels account for four of six bytes, so two bytes
belong to no object and no consumer names them. That is a genuine failure of
the tiling test rather than a judgement about coherence.

#### A splat label is not always an object

This range names seven labels but holds five objects. `D_8009AF74` is one
eight-byte `volatile u16[4]`, and because code addresses its middle elements
directly, spimdisasm also emitted names at `0x8009AF76` and `0x8009AF7A`.

Defining those two as separate objects would have produced byte-identical
output and still been wrong, since they are positions inside an array rather
than things in their own right. The bytes cannot detect this error, so the
check has to come from the consumers: `display_object_helpers.h` had already
worked out the array shape and recorded that the two inner labels were once
spelled privately.

`0x8009AF10` shows the same hazard in its other form. Splat's third label
there spans eight bytes, but `D_8009AF18` is a four-byte pointer: its readers
load it and immediately dereference it at `+0x08`, `+0x10` and `+0x46`. The
label runs long only because the word after it has no name of its own. One
label covered part of an object in the first case and more than an object in
the second, so neither direction can be assumed.

This is the resident-side counterpart to the overlay obstacle noted earlier,
where a label's extent is the gap to the next name rather than an object's
size. Both say the same thing: label boundaries are evidence about where code
points, not about where objects begin and end. Read the consumers before
trusting them.

The remaining coherent handler range is harder only because its layout mixes
single bytes, packed sub-word state, words and aligned strings. Its identity is
still clear: the model handler family and `func_800540B4` are its readers, and
the strings and state words are renderer diagnostics/working state. Any next
conversion should therefore start from byte-layout probes, not from uncertainty
about placement or ownership.

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

- `DisplayObject_ResetVelocity` reads only `$a0`, and its two `void (void)` consumers are
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

The array length is a threshold, not a claim about storage. Two symbols have
carried the pattern, and both would overrun their neighbours if read literally:

- `gDuel_bTerrain` (0x8009B364) is one byte -- `gFreeDuel_bReturnFlags` sits at
  0x8009B365 -- yet is declared `[8]` and `[]` as well as a plain and a
  `section(".data")` scalar, across eight files spanning all three rows above.
- `gSD_bOutputType` (0x8009B408) is read only at index 0 yet was declared `[16]`
  and `[9]`. `options_init.c` stated the reason inline: it "needs an oversized
  array extern to force absolute (lui+lbu)". Its consumers now take the
  `section(".data")` arm `sound.h` guards instead.

The last row is why the forms are not interchangeable, and it is worth
measuring rather than assuming. Relaxing `func_80024E58.c`'s `[8]` to an
incomplete `[]` costs four bytes of text, because that file assembles at `-G4`;
the identical relaxation in `func_8001798C.c`, which assembles at `-G8`, is
exact. `options_init.c` used to sit on the `-G4` assembler arm with a sized
array for the same reason. Once it took the `.data` arm, the assembler
threshold had nothing left to decide in it, and `Options_Init` now builds at
`gcc_2_8_1_g8_split` inside `options_screen.c`.

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
  `MainMenu_LoadPackageStage` the note records that `D_8009B058`, `D_801DD000` and
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

`make check-unmatched-contracts` enforces both sides of that boundary. For
functions it cross-references `functions.csv`, the matching-C source manifest,
and `unmatched.h`: an unmatched function referenced by matching C must have
one central declaration, and a file-local declaration is rejected unless its
exact source and spelling appear in
`unmatched_contract_exceptions.json`. The same check rejects declarations that
remain after a function becomes matching C. Unreferenced assembly functions
do not receive guessed prototypes merely to fill the header.

For data, the check cross-references `c_symbols.ld`, every top-level extern in
matching C, and all resident headers. A symbol centralized in `unmatched.h`
cannot remain locally declared, lose its linker assignment, or gain a
subsystem-header declaration without making the check fail. Distinct guarded
central views are allowed; exact duplicates are not. A measured local
addressing view can remain only when its source and normalized declaration are
recorded in `unmatched_data_contract_exceptions.json`. Both exception lists
are site-specific so a type or qualifier change cannot hide behind an approved
symbol name.

The data audit also reports the headerless remainder without rejecting it.
That lets centralization proceed in measured batches instead of flattening
array, section, volatile, or asm-alias forms simply to make a count reach zero.
The first enforced batch moved thirty unanimous primitive declarations from
thirty-one matching-C sites already including `unmatched.h`.

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
It has one consumer, `main_menu_load_package_stage.c`; that file declares it `extern u8
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

#### Reading the lever out of the objects

The two subsections above need the answer either to be predictable from the
table or to have been written down already in an attempt record. There is a
third source that is free, exact, and exists for every symbol: the built
objects record the addressing form as a relocation, so the lever can be read
instead of inferred.

```sh
OD=tools/toolchains/binutils-2.42/bin/mipsel-none-elf-objdump
"$OD" -dr tmp/splat/build/src/game/<file>.o | grep D_8009B16C
```

`R_MIPS_GPREL16` means the symbol resolved into small data. `R_MIPS_HI16`
paired with `R_MIPS_LO16` means it did not. When one symbol shows both forms
across the objects that use it, the differing declarations are not drift to be
collapsed: they are opposite levers, and no single spelling can serve them.

This check belongs *before* any attempt to reconcile sizes, because a size
disagreement on its own does not settle the question. #3108 settled
`D_8009B20C` by bounding the object from two directions -- the next named
address above it, and the highest index any consumer actually reaches below --
and then centralizing on the size that was correct rather than the largest one
observed. That reasoning holds only while every consumer wants the same
addressing form, and the relocations are what say whether it can hold at all.

`D_8009B16C` is the case where it cannot. The object is eight bytes:
`D_8009B174` is the next name. Five files declare it three ways, and four of
them agree.

| Translation unit | Spelling | Bytes | Relocation |
| --- | --- | --- | --- |
| `duel_scene_update.c` | `extern u16 D_8009B16C` | 2 | `R_MIPS_GPREL16` |
| `func_800179F4.c` | `extern u16 D_8009B16C` | 2 | `R_MIPS_GPREL16` |
| `debug_effect_screen.c` | `extern u8 D_8009B16C[4]` | 4 | `R_MIPS_GPREL16` |
| `main_run_duel_and_library.c` | `extern u16 D_8009B16C[9]` | 18 | `R_MIPS_HI16` + `R_MIPS_LO16` |

Both translation units in the disagreement compile at `-G8`, so the profile is
not what separates them; the declared size alone decides, by falling on one
side or the other of the eight-byte small-data threshold. The `[9]` claims
eighteen bytes for an eight-byte object, which reads like an error until the
relocation shows it is doing the same job `options_init.c` once documented
inline for `gSD_bOutputType` -- oversizing on purpose to force absolute addressing.

The `[4]` is load bearing from the other direction, and this one was written
down: `func_800222F4`, which is `debug_effect_screen.c`, records "small-data
sized arrays for `D_8009B16C` and `D_8009AF2C`" as the discriminator that
matched it under `gcc_2_8_1_g8_split`.

That leaves no size to centralize on. Eight bytes is the true extent, but
eight bytes is still small data, so adopting it would keep the four
gp-relative consumers correct and break the fifth. Anything above it would
move all five out of small data. The symbol is a genuine four-arm case, not a
cleanup target, and the reads confirm the split is meaningful rather than
accidental: the `u16` consumers only ever test bits `0x1000` and `0x2000` at
offset 0, while `debug_effect_screen.c` only ever touches byte 2.

#### Agreeing relocations do not mean interchangeable declarations

The check above rejects a symbol when its objects disagree. The tempting
converse -- that agreeing relocations clear a symbol for centralizing -- is
false, and `gAi_wBestDifference` is the counter-example. Two files declare it,
they disagree about the spelling, and yet both objects relocate it the same
way:

| Translation unit | Spelling | Profile | Relocation |
| --- | --- | --- | --- |
| `ai_script_load_best_values.c` | `extern unsigned short gAi_wBestDifference` | `gcc_2_8_1_g0` | `R_MIPS_HI16` + `R_MIPS_LO16` |
| `ai_script_find_best_attack.c` | `extern u16 gAi_wBestDifference[]` | `gcc_2_8_1_g8_split_no_strength_reduce` | `R_MIPS_HI16` + `R_MIPS_LO16` |

The object is two bytes and holds exactly one `u16`: `gAi_bBestAttacker` is the
next name, at `+2`, with `gAi_bBestTarget` at `+3`. Nothing indexes above `[0]`.
So the array brackets describe no more storage than the scalar does, and by the
size and index tests alone the two spellings look like drift worth collapsing.

They are not. Read the profiles against the table above and each spelling is
the one its own translation unit needs. `ai_script_load_best_values.c` compiles
at `-G0`, where nothing is placed in small data and a plain scalar already gets
`lui %hi` + `%lo`; it needs no lever. `ai_script_find_best_attack.c` compiles at
`-G8`, where a two-byte scalar would be placed in small data and addressed
`%gp_rel`; the brackets are what push it back out. The agreement in the last
column is the *result* of two different levers pulled correctly, not evidence
that one declaration could serve both. Unify them on the scalar and the `-G8`
consumer goes `%gp_rel`; unify them on the array and the `-G0` consumer changes
the expression it reads for no reason.

The order the checks are applied therefore matters. Disagreeing relocations
reject a symbol outright, but agreeing relocations only mean the profiles have
not yet been consulted: when the consumers compile under *different* `-G`
settings, agreement is the expected outcome of correct code and says nothing
about interchangeability. Compare profiles before reading the relocations as a
clearance. A symbol like this one belongs in a shared header only behind the
same kind of guarded arms `input.h` and `sound.h` already use -- never as one
flat declaration.

#### One address, several faithful types

Not every symbol has a type waiting to be found. Some are shared staging
areas, written by unrelated subsystems that each stage a different shape into
the same bytes. For those, the differing declarations are not drift and there
is no canonical type to adopt: each one is faithful to what its own caller
puts there.

`D_801D5608` is the clearest example in the tree. Eight sources declare it a
flat `s32 []`; `duel_rewards.c` declares it `s32 [16][DUEL_SIDE_COUNT]` and
calls it "the separate `D_801D5608[stat][side]` display table";
`password/shop.c` carries a plain `s32` same-symbol alias and assigns one
word; and two overlays in *different* segments,
`free_duel/screen_runtime.c` and `password/shop.c`, each use

```c
typedef struct { u32 lo; u32 hi; } Pair;
```

and declare `extern Pair D_801D5608;`. The password source needs both views
because its starchip renderer writes the scalar while its message setup writes
the pair.

Those two overlay definitions are textually identical, which makes them look
like the duplicate-type cleanup that `screen_projection.h` describes for
`ProjectedPair`. They are not the same case. `ProjectedPair` was one layout
that three files had each rediscovered, so naming it once lost nothing.
`Pair` is two overlays agreeing about the two words *they* stage, while other
callers stage a rank table or a single count into the same address. Hoisting
`Pair` into a shared header would present one caller's view as the symbol's
type.

`main_run_credits.c` settles it from a third direction. It hand-assembles the
access rather than declaring the symbol at all, storing a halfword at `+0` and
a word at `+4` through explicit relocation directives:

```
".word 0x3C060000\n"
".reloc .-4, R_MIPS_HI16, D_801D5608\n"
".word 0xACC20000\n"
".reloc .-4, R_MIPS_LO16, D_801D5608\n"
```

That is a fourth shape again, and it is pinned: the address form is written
into the source.

The rule this gives is narrow but useful. A type duplicated across files is
worth unifying when the files agree about *the same object* -- and a symbol
whose consumers stage different shapes into one buffer is not that, however
identical two of those consumers happen to look. State what such a symbol is
NOT, and leave the views alone.

#### Two kinds of differing spelling, and only one is predictable

A consumer whose declaration disagrees with the definition is not
automatically load bearing. Two measurements a few hours apart came out
opposite ways, and the difference between them is mechanical enough to use as
a decision procedure.

**Case one: the difference constrains optimization.** `graphics_frame.c`
defines `D_8009B0AD`, `D_8009B0D0` and `D_8009B0A8` as plain bytes.
`main_services.c` declared all three `extern volatile u8`, under a comment
saying the init block is volatile so the emitted order is the source order.
Those three are the *first three stores* of that run, so the honest
expectation was a shift. There was none: dropping the `volatile` and taking
the declarations from the owning header builds byte-identical. The comment was
true of the rest of the run and over-claimed for these three.

**Case two: the difference changes the addressing mode.** `main_run_trade.c`
defines `u8 D_8009B269;`. `script_control_commands.c` compiles at `-G8` and
declares it `extern u8 D_8009B269[]`, writing `D_8009B269[0]` at two sites.
Converting that consumer to the scalar spelling does not shift bytes -- it
*shortens the executable*:

```
error: rebuilt executable is 0x1d07f8 bytes, expected 0x1d0800
```

Eight bytes, across two write sites, four bytes each. At `-G8` a small scalar
is small-data eligible and each store becomes one `%gp_rel` instruction; the
array spelling pushes it out of small data and each store becomes a `%hi/%lo`
pair. The array is the lever the table above describes, and here it is
measured rather than inferred.

The two cases separate cleanly:

- A spelling that changes **addressing mode** -- array versus scalar under
  `-G8`, `section(".data")`, an addend neighbour -- is structural. The `-G`
  table predicts it, it changes the instruction *count*, and there is no need
  to spend a build confirming it.
- A spelling that only **constrains the optimizer** -- `volatile` is the one
  that occurs here -- is not predictable from the table at all. It costs
  nothing wherever the optimization it forbids was not available anyway, which
  is why two separate `volatile` views in this tree turned out inert. Measure
  it; do not assume either way.

The build error tells you which case you are in. A pure **length** change means
the instruction count moved, which points at addressing mode. A same-length
mismatch points at scheduling or register allocation, where `volatile` and
register pins live.

#### One symbol reached by two addressing modes at the same `-G` level

The section above says the `-G` table predicts an addressing-mode lever. It
predicts which levers are *available*; it does not, on its own, say which one a
given consumer needs. `D_8009B26C` is the case that separates those two
claims, and it is worth writing down because it looks like an obvious
centralization target and is not one.

Seven sources declare it identically as `extern u8 D_8009B26C[]` and write
`D_8009B26C[0]`: `frontend_scene_states.c`, `duel_effect_basic_commands.c`,
`duel_effect_mode_7.c`, `func_8002FA28.c`, `func_8002EB48.c`,
`script_control_commands.c` and `async_state_poll.c`. Seven identical
declarations of one symbol, with no disagreement to resolve, is exactly the
shape that has passed byte-exact elsewhere. It still cannot be centralized.

Two facts block it.

**Retail reaches the symbol both ways, and the sources pin it.** Three units
hand-assemble their accesses, and they do not agree about the relocation:

```
main_run_credits.c:   .reloc .-4, R_MIPS_GPREL16, D_8009B26C
func_80030998.c:      .reloc .-4, R_MIPS_HI16,    D_8009B26C
                      .reloc .-4, R_MIPS_LO16,    D_8009B26C
func_8002A788.c:      .reloc .-4, R_MIPS_HI16,    D_8009B26C
                      .reloc .-4, R_MIPS_LO16,    D_8009B26C
```

`func_80030998.c` settles it from inside a single block: two instructions
apart it takes `gDebug_nSceneOrSoundID` `GPREL16` and `D_8009B26C`
`HI16`/`LO16`. So the absolute form is not that unit being uniformly outside
small data; it is this symbol, at this site.

**The profile does not choose the spelling.** All seven array-spelling
consumers compile at `gcc_2_8_1_g8`. So do `main_debug.c`,
`main_run_credits.c`, `func_80030998.c` and `func_8002A788.c`, which use the
plain scalar. Same compiler, same `-G8`, opposite spellings, both matching.
`func_80024DC8.c` is the control: it is `-G0` and uses the scalar, where the
table says no lever is needed because a plain scalar already gets `%hi/%lo`.

The spelling is therefore a property of the individual access site's required
relocation, not of the symbol and not of the translation unit's profile. A
single shared declaration cannot serve both groups. Guarded arms could hold
both, but each consumer would still have to select its arm, so seven local
declarations would become seven local `#define`s and one indirection -- churn
without a reduction.

Two smaller notes for anyone who picks this symbol up. It is **not** an
unmatched symbol, so it is out of scope for the `unmatched.h` work: three
units define it -- `func_8002DC38.c`, `main_run_trade.c` and
`main_run_animated_battle.c` -- as common symbols under the `*_comm`
profiles. And nothing anywhere indexes above `[0]`, which is what the next
symbol requires: `D_8009B26D` sits one byte above it in `c_symbols.ld` and is
live in its own right, read and written by `frontend_scene_states.c` and
`func_8002EE94.c` behind a `D_8009B26D_IN_DATA` guard. `D_8009B26C` is a
single byte with a named neighbour immediately above, so its array spelling is
a lever and could never be a real array -- and `frontend_scene_states.c`
demonstrates both at once, declaring `D_8009B26C[]` while separately using
`D_8009B26D`.

Its neighbour `D_8009B269` sits three bytes below `D_8009B26C` and behaves the
same way: converting `script_control_commands.c` to the scalar spelling
shortens the executable by eight bytes, as the section above records.

### The overlay data blobs

The issue's completion criterion names the game *and overlay* split templates,
but the ranges above are all resident. The overlay side has not been measured
before, so here it is. Every overlay carries one unowned `data` subsegment:

| overlay | labels | extent | items | non-zero |
| --- | ---: | ---: | ---: | ---: |
| `free_duel` | 5 | 6093 | 1554 | 1299 |
| `main_menu` | 41 | 15016 | 11268 | 2053 |
| `overworld_before_coup` | 11 | 4524 | 3940 | 3225 |
| `overworld_after_coup` | 11 | 4524 | 3940 | 3038 |
| `password` | 30 | 9213 | 2330 | 507 |

That is roughly thirty-nine kilobytes still resolved at link time, and unlike
the resident `.data` ranges none of it is vendor code's: overlays contain no
Psy-Q library.

Two things in the table are worth reading rather than skimming. The two
overworld blobs agree exactly on extent and label count but **not** on
content -- 3225 non-zero items against 3038 -- which is what a before/after
pair sharing one layout and differing in values should look like, and is a
reason to treat them as two jobs rather than one. And `free_duel` has only
five labels across six kilobytes, so the vast majority of it is unnamed.

`main_menu` also holds a hazard that is already on record elsewhere in these
notes: `D_80185CC8` and `D_80185CC9` both sit in its blob, and that pair is
the worked dual-name case where one file uses the array view and the scalar
neighbour both ways. Any ownership of that tail has to preserve both
spellings.

`free_duel` is the smallest by label count and looks like the obvious first
target. It is not, and the reasons generalise:

- Its five names are already semantic -- `gFreeDuel_abGridAvailable`,
  `gFreeDuel_pThumbWidget`, `gFreeDuel_apSparklePool`,
  `gFreeDuel_pCursorWidget`, `gFreeDuel_bScreenFlags` -- and three are already
  declared in `free_duel.h`. So the naming work is done and only the
  definition is missing.
- But `gFreeDuel_pThumbWidget` spans **eight bytes** under one label, while
  `screen_runtime.c` reaches it through `asm("gFreeDuel_pThumbWidget")`
  aliases typed as a four-byte pointer -- twice over, once as
  `FreeDuelWidget *` and once as `u8 *`. The label extent and the C view
  disagree about the object's size, and both alias spellings are the
  deliberate kind the small-data notes describe.
- The named symbols stop at `0x801690A8`, and the blob does not: the words
  after `gFreeDuel_bScreenFlags` are non-zero and uncharacterised. Owning the
  named prefix would still leave most of the range behind.

#### Why the overlay blobs resist carving

The resident ranges were blocked by placement and by byte layout. The overlay
blobs have a different and more basic obstacle, and it took two candidates to
see it.

`password` looks like the most tractable of the five. Its bulk is four
identical 1464-byte objects at regular stride, uniformly `.word`, mostly
zero, and referenced by nothing anywhere in the tree -- no source, no
generated assembly, no configuration. Its head holds two ranges that look
better still: `D_8016D440` is 36 words and `D_8016D4DC` is 45, both entirely
zero, and both have real consumers in `shop.c`.

Both are traps, for the same reason.

`shop.c` declares `extern u8 *D_8016D440[]` and walks it to store **four**
objects -- sixteen bytes -- and the overlay's own function notes describe
exactly that, four decoration objects one per password digit. But the label
runs 144 bytes, because that is the distance to the next *named* symbol.
`D_8016D4DC` is worse: C declares it `u16`, and the label spans 180 bytes.

So in these blobs a label's extent is the gap to the next name, not the size
of the object it names. The regions are sparsely named, so most labels look
far larger than what they actually label, and carving by label extent would
invent object sizes that contradict the declarations already in the tree.

That is the same shape as `free_duel`'s `gFreeDuel_pThumbWidget`, eight bytes
of label against a four-byte pointer in two `asm()` aliases. One instance
looked like a quirk of that symbol; three make it the rule.

The screening consequence is worth stating plainly. For resident `.sdata` a
uniformly word-sized run was sufficient evidence to carve, and it worked
first try. For overlay data it is **not** sufficient: a run can be uniformly
word-sized, entirely zero, and still unsafe, because the size the label
implies may be unrelated to the object. The extra check is to find a
consumer's declared size and require it to agree with the label extent, or
else to account for the unnamed remainder explicitly. None of the candidates
examined here passes that check.

Two method corrections, because each cost me a wrong number in this same
survey.

Measuring a blob by the span of its **labels** understates it whenever the
content continues past the last named symbol -- for `free_duel` that reported
120 bytes against a true extent of 6093, out by a factor of fifty. Measure
from the first to the last emitted datum instead.

Counting non-zero content by matching `.word` lines alone is worse, because
it fails silently in the direction that looks like good news. These blobs are
emitted mostly as `.byte` and `.short`: `main_menu` carries 9032 byte and
1480 short directives against 756 words, so a word-only count reported it as
entirely zero when 2053 of its 11268 items are non-zero. It read as the
easiest range in the table and is nothing of the kind. Count every directive
kind, and treat a suspiciously clean result as a reason to check the mix
rather than to celebrate.

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
