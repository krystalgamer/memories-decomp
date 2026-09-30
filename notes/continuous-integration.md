# Matching Build Continuous Integration

`.github/workflows/build.yml` coordinates the regional build jobs for pushes
to `master`, pull requests, and manual dispatches. Its preparation job makes
the private input bundle available before any build job starts.
`.github/workflows/matching-build.yml` performs the North American clean
hosted Ubuntu build as a reusable workflow, and can still be dispatched
manually on its own. It installs or
restores the pinned local tools, runs `make clean`, rebuilds the complete
PS-X EXE, reports generated global-usage drift without blocking the build,
and explicitly verifies:

```text
84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88
```

## Regional retail bundle

Retail files remain ignored and must never be committed. The preparation
action and all North American, Japanese, European, French, German, Italian
and Spanish build workflows use these **Actions repository secrets**, not
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
Downloads retry up to three times on transfer errors, including connection
resets while fetching the expanded bundle. Exhausted retries still fail the
job before staging, and the partial ZIP is removed.
The known patched North American WA dump remains explicitly rejected.
Downloads and plaintext staging files stay under ignored `tmp/` and are
removed after use. Plaintext retail files are never cached or uploaded as
artifacts. Only the encrypted bundle described below is stored in the
Actions cache.

### One preparation job, shared encrypted cache

Automatic builds now have one `prepare-inputs` job, followed by the ten
existing regional build jobs. On a cold cache, that job downloads the ZIP
from the private server once (with the existing transfer retries), verifies
all seven regions without installing their inputs, and encrypts the ZIP
before saving the cache. On a warm cache, **no build job downloads from the
private server**.

The jobs still run on separate runners. Each restores the encrypted file
from GitHub's cache, decrypts a temporary local ZIP, and stages only its
original region/selection with the existing SHA-256 checks. Restoring from
GitHub therefore still transfers bytes to each runner; it avoids repeatedly
fetching the ZIP from the origin server, not all network transfers.
All compiler, clean-match, overlay and regression gates are unchanged.

`tools/project/ci_bundle.py` uses GnuPG AES-256 symmetric encryption with
iterated SHA-256 string-to-key derivation and integrity protection. Decryption
must report successful authenticated decryption before staging starts.
The passphrase is derived from all three existing private-bundle credentials,
passed to GnuPG through standard input rather than command-line arguments.
No additional repository secret is required. Use a strong private-bundle
password: encryption does not make a weak password safe.

The only retail cache path is `tmp/ci-retail-cache/ci-files.zip.gpg`.
An exact cache key includes a format version, the regional checksum
inventories, the set of selected overlay archives, and an iteratively derived
credential fingerprint (not a fast password hash). Changing expected inputs or rotating credentials invalidates
the cache automatically. Merely adding more modules inside the same archive
does not. There are no broad restore-key fallbacks, and `game/` and plaintext
ZIPs are excluded from caches. GitHub may make caches readable to pull
requests, which is why caching plaintext retail inputs is not permitted.

The preparation action explicitly saves a newly populated cache **before**
downstream jobs start. Consumers are cache-only: an evicted/missing cache
fails visibly rather than falling back to ten origin downloads. Rerun all
jobs to execute preparation again; for damaged ciphertext, delete that cache
in Actions before rerunning. Entries otherwise follow GitHub's normal cache
retention/eviction policy.

Regional workflow files remain individually available through
`workflow_dispatch`. Such a standalone run prepares/restores the same cache
inside its single build job. A cold preparation therefore requires the complete
seven-region bundle even for a standalone run; the files installed for that
build still follow its original regional selection. Reusable calls default to coordinated mode and
never silently prepare inputs if the parent's key is missing.
Automatic jobs are grouped under **Regional builds**; repositories with
required checks tied to the old workflow/job names should update those
rules to the corresponding reusable build checks.

README-only changes still skip the build workflow. The preparation job's
change filter preserves the previous narrower exclusions for North American,
German, Italian and Spanish jobs (`notes/**` and `tools/trace/**`); Japanese,
European and French jobs retain their README-only exclusion. Metadata remains
unfiltered. If an old commit cannot be resolved (for example after a force
push), CI logs a warning and runs all build gates rather than skipping them.

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
