# Build-integrated candidates

Each `func_XXXXXXXX.c` is a reproducible source attempt for one unmatched
resident or overlay function. Overlay candidates live in a subdirectory named
for their configured module, since different modules reuse function addresses.
Keep concise refinement notes in the source comment rather than a separate
candidate note.

`config/slus_01411/candidates.json` records the named compiler profile,
path-independent object fingerprint, target-byte hash, and canonical contract
fingerprints. Text-only candidates retain the original text/relocation
fingerprint; candidates with compiler-generated `.rodata` or `.rdata` also
fingerprint those bytes and relocations. Writable data, BSS, and literal-pool
sections remain rejected. The corresponding retail assembly lives in
`src/candidates_target/`.

Normal full and incremental resident builds compile their candidates after
linking the game; `make build-overlays` compiles the overlay candidates after
linking their modules. The validator checks the build hash, requires every
undefined symbol to exist in the linked target (the module or resident ELF for
an overlay), and fingerprints the canonical header declarations
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
candidate can consume. Overlay candidates also scan their module's headers.
