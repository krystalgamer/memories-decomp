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

Every stored resident candidate has a generated directory under
[`for_humans/`](for_humans/README.md). It carries the exact self-contained C
block, the target spimdisasm assembly, and the complete named compiler-profile
options. Run `make candidate-bundles` after changing a candidate and commit the
corresponding bundle changes. `make check-metadata` rejects missing, extra, or
stale bundles.
