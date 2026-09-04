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
  decompals/old-gcc recipe.

Downloaded archives, installed packages, source checkouts, and toolchains remain
under `tools/`. Temporary build directories remain under `tmp/`.

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

`make compiler-281-prebuilt` needs nothing.

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
- `split` deletes only the previous `tmp/splat/` output and regenerates
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

The Splat linker script under `tmp/splat/` is diagnostic. The exact build uses
the project linker script at `linker/slus_01411.ld`, which preserves the
original data/text/data ordering and file load addresses.

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
   `config/slus_01411/matching_c.json` and the named profiles in
   `config/slus_01411/compiler_profiles.json`, then normalize their assembly
   through maspsx.
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

Current matching totals are generated in the root `README.md`; run
`make progress` after accepted decompilation changes.

## Full repository audit

```sh
make audit
```

The audit performs a clean exact build, reconciles the function inventory,
reapplies ownership classifications, generates progress metrics, and checks:

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
- The root README progress section matches the authoritative function
  inventory.
- The worktree is clean after deterministic regeneration.

## Cleanup

```sh
make clean
```

Cleanup removes only these known generated paths when present:

- `tmp/extract/`
- `tmp/splat/`
- `tmp/project-build/`
- `tmp/reports/`

It does not remove downloaded tools, toolchains, user-supplied game files, or
the separate binutils bootstrap build directory.
