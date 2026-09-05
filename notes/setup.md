# Project Setup

## Required local inputs

Place the North American game files at the exact paths listed in
`config/slus_01411/files.sha256`. The primary target is:

```text
game/SLUS_014.11
```

The game files remain ignored and untracked. Validate them before any analysis:

```sh
make verify-inputs
```

The expected executable SHA-256 is:

```text
84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88
```

The original disc reference is also required:

```text
game/rpg-yfm.cue
game/rpg-yfm.bin
```

Validate its ISO9660 LBAs and extracted file contents with:

```sh
make verify-disc
```

Use an untouched North American dump. A commonly circulated image changes
one anti-piracy branch inside `DATA/WA_MRG.MRG`; input verification recognizes
that patched image and reports it explicitly instead of accepting it as the
project reference.

## Workspace rule

Run all commands from the repository root. Project scripts reject execution
from another directory and reject absolute or escaping project paths.

All project-managed software is installed beneath `tools/`. All generated
output, caches, compiler scratch files, and reports are written beneath `tmp/`.
All durable documentation is kept beneath `notes/`.

## Bootstrap

```sh
make tools
make check-tools
```

The bootstrap is pinned by:

- `tools/bootstrap/tools.json`
- `tools/bootstrap/requirements.lock`
- `tools/bootstrap/bootstrap-requirements.lock`
- `tools/bootstrap/binutils.json`
- `tools/bootstrap/old_gcc.json`
- `tools/bootstrap/old_gcc_272.json`

The installed state is ignored by Git. The bootstrap does not install global or
user-level packages.

## First verified build

```sh
make map
make match
make classify-functions
make progress
```

`make match` succeeds only when the complete rebuilt PS-X EXE is byte-identical
to the supplied target.

## Optional PsyQ compiler candidates

No proprietary compiler is downloaded by this project. A lawfully obtained
candidate may be placed beneath an ignored directory in `tools/toolchains/`.
Record its exact hash and provenance in `notes/toolchain.md` before using it.

The current evidence requires testing late ASPSX behavior and likely CCPSX
`-O2`/`-G8` output. The user independently verified Psy-Q 4.6. Its Win32 tools
use GCC 2.8.1 and its DOS tools use GCC 2.7.2. Use 2.8.1 first and keep 2.7.2
available as a fallback. The unusual `LIBDS.LIB` was an online patch
distributed before Psy-Q 4.7, so library-only identification may associate it
with 4.7. Exact compiler and library binaries must still be recorded by hash.

The bootstrapped GCC 2.8.1 PSX compiler is a reproducible diagnostic stand-in,
not Sony CCPSX. The build always supplies explicit PSX target flags as recorded
in `notes/toolchain.md`. A separately pinned GCC 2.7.2 MIPS compiler is kept
available only as the DOS-era fallback. Matching assembly is processed with
MASPSX 2.81 for GCC 2.8.1 and MASPSX 2.72 for GCC 2.7.2.
