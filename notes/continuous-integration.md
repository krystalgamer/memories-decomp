# Matching Build Continuous Integration

`.github/workflows/matching-build.yml` performs a clean hosted Ubuntu build
for pushes to `master`, pull requests, and manual dispatches. It installs or
restores the pinned local tools, runs `make clean`, rebuilds the complete
PS-X EXE, reports generated global-usage drift without blocking the build,
and explicitly verifies:

```text
84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88
```

## Regional retail bundle

Retail files remain ignored and must never be committed. All North American,
Japanese, European, French, German, Italian, and Spanish build workflows use
`.github/actions/retail-inputs` with these **Actions repository secrets**, not
repository variables:

| Secret | Value |
|---|---|
| `YGOFM_CI_FILES` | Private HTTPS URL for the CI ZIP bundle |
| `YGOFM_CI_FILES_USERNAME` | HTTP basic authentication username |
| `YGOFM_CI_FILES_PASSWORD` | HTTP basic authentication password |

Credentials must not be embedded in workflows or committed configuration.
The download requires HTTPS, including redirects; credentials are not forwarded
to a different host on redirect. All former per-file URL secrets, including
`YGOFM_SLUS_01411_URL`, `YGOFM_SU_MRG_URL`, and `YGOFM_WA_MRG_URL`, are no longer used.

The ZIP has the following member layout. Each directory contains its executable,
`SU.MRG`, `WA_MRG.MRG`, `MODEL.MRG`, and any additional archive referenced by
that region's `overlays.json` directly (there is no `DATA/` inside the ZIP):

| ZIP directory | Executable | Installed directory |
|---|---|---|
| `ci_files/usa/` | `SLUS_014.11` | `game/` |
| `ci_files/jap/` | `SLPM_863.98` | `game/japanese/` |
| `ci_files/eur/` | `SLES_039.47` | `game/europe/` |
| `ci_files/fra/` | `SLES_039.48` | `game/france/` |
| `ci_files/ger/` | `SLES_039.49` | `game/germany/` |
| `ci_files/ita/` | `SLES_039.50` | `game/italy/` |
| `ci_files/esp/` | `SLES_039.51` | `game/spain/` |

`tools/project/stage_ci_inputs.py` selects only the requested region, moves the
MRG files into its `DATA/` directory, and checks every selected member against
the existing regional `files.sha256` manifest **before installing any files**.
Missing or duplicate members, invalid ZIPs, and hash mismatches fail the job.
Existing retail inputs with different bytes are never overwritten.
Archive selections follow the regional overlay manifest, in addition to the
SU/WA/MODEL baseline. MODEL is required even before a region registers its first
MODEL module. Repeated references to an archive stage it only once.
Each new input must also appear in the regional `target.yaml` with the same
checksum and its exact byte size. The full input gate requires `target.yaml`
and `files.sha256` to describe identical inventories.
Each archive must be directly inside that region's `DATA/` directory, and its
overlay checksum must agree with the regional `files.sha256` entry. For example,
the Spanish job requires the privately supplied `ci_files/esp/MODEL.MRG` member.
Missing required inputs fail rather than skipping modules.
The French and German overlay-only jobs select all three MRG files.
The North American matching job selects only `SLUS_014.11`; its overlay job
selects all four files. Other entries in the North American disc manifest
(such as STR/XA and BIN/CUE) are not selected merely
because they appear in the checksum inventory.
The expanded local bundle's 28 inputs were checked against all seven local
retail disc images; the new regional MODEL checksums preserve those exact bytes.
The hosted `YGOFM_CI_FILES` URL must serve this expanded bundle before these jobs
run. Supplying `game/ci_files.zip` locally does not replace the hosted file or
change the repository secrets.
The known patched North American WA dump remains explicitly rejected.
Downloads and staging files stay under ignored `tmp/` and are removed after use;
retail files are not cached or uploaded as artifacts.

To stage a legally obtained local copy without network access, from the
repository root:

```sh
python3 tools/project/stage_ci_inputs.py \
  --archive game/ci_files.zip --region france
```

Use `--archives-only` to stage SU, WA, MODEL and additional configured overlay
archives, or `--executable-only` to stage just the resident executable without
reading the overlay manifest. These options are mutually exclusive.
Existing complete-image build and overlay-verification gates remain unchanged.

## Verification gates

Normal matching-build targets use `make verify-target`, which validates only
the executable. Disc-analysis and full repository-audit targets continue to
use `make verify-inputs`, so local LBA and extracted-data verification remains
unchanged.

A missing secret, download failure, input hash mismatch, tool failure, build
failure, or rebuilt executable hash mismatch makes the workflow fail.

`make check-global-usage` verifies `notes/global-usage.csv` strictly. The CSV
makes per-function claims, naming each function and whether its references to
a global are C or assembly, so a stale row is not merely behind, it is wrong
about the change that just landed. It is also address-ordered, so concurrent
matches append in different places and merge cleanly.

The matching workflow runs this check as an optional diagnostic. A stale CSV
adds a warning annotation and makes the step visibly fail, but
`continue-on-error` prevents that bookkeeping drift from failing the
`clean-build` job or holding a matching PR behind unrelated merges.

Regenerate the CSV with `make global-usage` in dedicated report updates or when
a change specifically targets global-usage metadata. Matching PRs do not need
to carry this report.

## Compiler installation

CI does not build GCC. It downloads the official
`gcc-2.8.1.tar.gz` asset from the `decompals/old-gcc` 0.17 release,
verifies the archive and individual `gcc`, `cc1`, and `cpp` SHA-256 hashes,
and installs a local wrapper that points the fixed-prefix release driver at
the repository-local compiler components.

The similarly named `gcc-2.8.1-psx.tar.gz` asset is not used: a complete local
comparison found 95 differing C objects and 45 text-size changes. The selected
`mips-linux-gnu` release asset reproduces the project's required GCC 2.8.1
code generation under the explicit PSX flags.

Binutils 2.42 is not built in CI.

Ubuntu 22.04 installs the pinned
`binutils-mips-linux-gnu=2.38-1ubuntu1cross2` package. The project creates
small wrappers beneath `tools/toolchains/binutils-2.42/bin/` so existing build
paths remain local and stable. The wrapper manifest records and validates the
package version plus the SHA-256 of every packaged binary. The explicit `-EL`,
MIPS I, ABI, and small-data flags remain controlled by the project.

Ubuntu's MIPS assembler marks text sections with 16-byte alignment, while the
original pinned assembler uses 4-byte alignment. After assembling a C or
generated text object, the package-backed path uses the packaged `objcopy` to
set `.text` alignment to 4. This changes section metadata only and prevents
the linker from inserting gaps between function objects.

Local development may continue using the source-built pinned binutils 2.42.
CI sets `USE_SYSTEM_MIPS_BINUTILS=1`, causing `make check-build-tools` to
validate the package-backed wrappers instead.

GitHub does not expose repository secrets to pull requests from forks. Those
runs therefore fail at the explicit private-input check rather than receiving
the retail executable.
