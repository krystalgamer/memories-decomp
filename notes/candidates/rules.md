# Resident near-miss candidates

Candidate sources for unmatched resident functions live in `tmp/`, which is not
tracked, so they are lost when a session ends. `notes/overlays/candidates/`
already solves this for the overlays and records that the same near miss had
been rebuilt at least six times before it existed. The resident side had no
equivalent, so the same loss was happening here: a candidate would reach a
measured state, the state would be described in prose, and the next run would
rebuild the code from that prose.

This file is the durable copy for functions in `src/game/`. The rules are the
overlay file's rules, unchanged, because the problem is the same one.

Rules:

- Store the exact source that produces the recorded state, not a tidied
  version. These are measurements, and reformatting them can change the output.
- Record the profile and the measured result in the heading, and re-verify
  before trusting a stored candidate. If it no longer reproduces, say so in the
  function's inventory row rather than silently editing it here.
- Delete an entry once the function matches and its source is promoted into
  `src/game/`. `make check-candidate-index` enforces this: an entry whose
  function is already `matching_c` in the inventory fails the check. A stale
  entry reads exactly like an open near miss, so it will be picked up and
  worked on before anyone notices the function is done.
- The inventory row and `notes/research/matching-evidence.md` remain the place
  for findings, negatives and levers. This file holds only code.
- Check every `func_XXXXXXXX` a stored candidate names against `functions.csv`
  before integrating. Semantic renaming moves on while candidates sit here, and
  one naming a since-renamed callee still compiles and still diffs byte-exact
  against the target — the symbol only has to exist at link time. It then fails
  the whole build with `undefined reference`, well after the per-function check
  has gone green. This has cost two integrations so far: `func_8003FF08` became
  `SD_BGMPlay` and `func_8003FF34` became `SD_BGMFadeOut`.

Opcode distance below is the encoding-based multiset distance described in
`notes/research/matching-evidence.md`, not a positional diff. A large
positional count on top of a small opcode distance is an allocation or
scheduling problem, not a structural one, and the stored candidate is then the
right base to continue from rather than something to rewrite.

Each stored candidate is one file in this directory, named for the
function's address. `README.md` is a generated index of them and is not
tracked, so it is not visible when browsing this repository on the web: from
there the entries appear only as their filenames, which are addresses. Run
`make candidate-index` in a clone to produce the browsable table.

## Human-facing bundles

Every remaining note-based resident candidate has a generated directory under
`for_humans/`. It carries the exact self-contained C
block, the target spimdisasm assembly, and the complete named compiler-profile
options. Run `make candidate-bundles` after changing a candidate and commit the
corresponding bundle changes. In particular, any PR that edits a candidate's
self-contained C fence must regenerate its bundle in the same change; otherwise
the hard `make check-metadata` gate reports stale source and build metadata.
That check also rejects missing or extra bundles.

Candidates migrated into the build live under `src/candidates/`, with their
retail assembly under `src/candidates_target/` and build metadata in
`config/slus_01411/candidates.json`. A migrated candidate no longer retains a
note or `for_humans` bundle. Normal full and incremental builds compile these
sources, compare path-independent hashes of their text and relocations, and
reject undefined symbols that are absent from the linked target.

The metadata also fingerprints every canonical header declaration named by a
candidate-local `extern`. This is separate from the object hash: a candidate
may keep compiling to identical bytes after a game type or prototype changes,
precisely because its private declaration bypasses the canonical header. The
contract fingerprint makes that change fail with the candidate and dependency
names before the stale source can be trusted. A dependency with no canonical
declaration is fingerprinted explicitly too, so later centralizing it is also
detected.

`make candidate-contract-hashes` prints the current aggregate and per-symbol
hashes for an intentional metadata review. It does not modify
`candidates.json`; update that file only after checking why each reported
dependency changed. Prefer consuming the canonical header and removing the
private `extern` when that preserves the candidate fingerprint. Refresh a hash
with the private declaration still present only when the mismatch is measured
and intentionally part of the current candidate.

The hashes are a snapshot of the header tree and therefore have the same
merge-order rule as `notes/global-usage.csv`: after rebasing across any
canonical-header change, regenerate and review them even when Git reports no
textual conflict. Resident candidates intentionally exclude `src/overlays/`
from their declaration index because they cannot include overlay headers.
An overlay candidate (an entry with a `"module"`, in `src/candidates/<module>/`)
indexes its own module's headers as well and no other module's; see
`notes/overlays/candidates/rules.md`.

## Reclassified from matching C

Some build-integrated candidates were never near misses. #3859 moved every
resident function whose byte-exact source depended on a pinned register
(`register T x asm("$N")`) or on inline assembly out of `matching_c`, because
that source does not stand as a decompilation. Each of those candidates is
the exact source that used to match, devices included, with a header comment
naming the device and the file it came from; its `functions.csv` row starts
"Build-integrated candidate ... Reclassified from matching_c (#3859)". The
open problem for such a candidate is removing the device, not closing a
distance, so the stored source is a byte-exact starting point rather than a
measurement to improve on.

They keep their declarations in the header of the unit they came from.
`make check-unmatched-contracts` accepts that for a candidate, because
`src/candidates/` still gives it a defining C translation unit; see
`notes/build.md`.

Five functions whose "C" was only the target's words in a top-level `asm`
block (`func_800291E0`, `func_8002A4A8`, `func_8002A788`, `func_80030998` and
`Main_RunCredits`) had no C to keep and went straight back to generated
assembly.
