# Build-integrated candidates

Each `func_XXXXXXXX.c` is the current best source attempt for one unmatched
resident function. Keep concise refinement notes in the source comment rather
than a separate candidate note.

`config/slus_01411/candidates.json` records the named compiler profile,
path-independent text/relocation hash, and target-byte hash. The corresponding
retail assembly lives in `src/candidates_target/`.

Normal full and incremental builds compile every candidate after linking the
game. The validator checks the build hash and requires every undefined symbol
to exist in that linked target; candidate objects are not mapped into the game.
