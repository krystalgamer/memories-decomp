# Build-integrated candidates

Each `func_XXXXXXXX.c` is the current best source attempt for one unmatched
resident function. Keep concise refinement notes in the source comment rather
than a separate candidate note.

`config/slus_01411/candidates.json` records the named compiler profile,
path-independent object fingerprint, target-byte hash, and canonical contract
fingerprints. Text-only candidates retain the original text/relocation
fingerprint; candidates with compiler-generated `.rodata` or `.rdata` also
fingerprint those bytes and relocations. Writable data, BSS, and literal-pool
sections remain rejected. The corresponding retail assembly lives in
`src/candidates_target/`.

Normal full and incremental builds compile every candidate after linking the
game. The validator checks the build hash, requires every undefined symbol to
exist in that linked target, and fingerprints the canonical header declarations
for every candidate-local `extern`. A renamed, added, removed, or retyped
canonical declaration therefore stops the build even when the private candidate
declaration still compiles to the old object. Candidate objects are not mapped
into the game.

Run `make candidate-contract-hashes` to print the current aggregate and
per-symbol contract hashes when intentionally reviewing a metadata refresh.
The command reports only; it never rewrites tracked metadata. When a contract
changes, first try replacing the private `extern` with its canonical header;
retain a private spelling only when the candidate fingerprint proves it is a
measured part of the current near miss.

This metadata is a tree snapshot. Re-run it after rebasing across canonical
header changes even when `candidates.json` itself has no merge conflict.
Resident candidates scan root, game, and Psy-Q headers, but not
`src/overlays/`: an overlay-only declaration is not a contract a resident
candidate can consume.

## Restart archives

The `french_model166_414c/` and `model450_takeover7003/` subdirectories retain
only the best available partial source for each of two unresolved functions:
`func_8013F14C` and `func_8013E0A8`. Neither is exact or eligible for
integration. Alternative source forms are omitted; their measured results
remain in the local scratch history.

The files are deliberately named `candidate8.c` and `streamer02.c`, not
`func_XXXXXXXX.c`, and are not registered in
`config/slus_01411/candidates.json`. They are not compiled or accepted by the
candidate build. `func_8017B004` is summarized in the MODEL166 README but has
no C candidate yet. No target binaries, disassemblies, or generated objects
are archived here.
