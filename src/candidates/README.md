# Build-integrated candidates

Each `func_XXXXXXXX.c` is the current best source attempt for one unmatched
function. A resident candidate lives directly here; an overlay candidate lives
in `src/candidates/<module>/`, where `<module>` is the overlay module name used
by `config/slus_01411/overlays/<module>_functions.csv`. Keep concise refinement
notes in the source comment rather than a separate candidate note. The
function itself stays `unmatched_asm` in its inventory and keeps building from
generated assembly until a candidate matches and is promoted with
`tools/project/integrate_verified_match.py`.

`config/slus_01411/candidates.json` (schema 2) records, per candidate:

```json
{
  "address": "0x80182000",
  "module": "<module>",
  "profile": "<named profile from compiler_profiles.json>",
  "source": "src/candidates/<module>/func_80182000.c",
  "target": "src/candidates_target/<module>/func_80182000.S",
  "candidate_build_sha256": "<object fingerprint>",
  "target_bytes_sha256": "<sha256 of the target instruction words>",
  "canonical_contract_sha256": "<aggregate contract hash>",
  "canonical_contracts": {"<extern symbol>": "<per-symbol hash>"}
}
```

Omit `module` for a resident candidate. The profile is chosen by name and
compiled through the same `build_baseline.compile_c` path as matching C, so
any named profile works, and candidates of one module may use different
profiles. Text-only candidates keep the original text/relocation fingerprint;
candidates with compiler-generated `.rodata` or `.rdata` also fingerprint those
bytes and relocations. Writable data, BSS, and literal-pool sections are
rejected. The corresponding retail assembly lives in `src/candidates_target/`.

Normal full and incremental resident builds compile every resident candidate
after linking the game, and `make build-overlays` (so `make match-overlays`)
compiles every overlay candidate after building the modules. The validator
checks the build hash, requires every undefined symbol to exist in the linked
target (the module ELF, plus the resident ELF when built), and fingerprints
the canonical header declarations for every candidate-local `extern`. A
renamed, added, removed, or retyped canonical declaration therefore stops the
build even when the private candidate declaration still compiles to the old
object. Candidate objects are not mapped into any image.

Useful commands, all from the repository root:

```sh
# copy the generated retail asm of an unmatched_asm function (after the
# resident split, or the overlay build for --module) to its target path
tools/environments/python/bin/python tools/project/candidate_builds.py --write-target 0x80182000 --module <module>
make check-candidate-builds      # metadata only; part of make check-metadata
make candidate-contract-hashes   # print current contract hashes
tools/environments/python/bin/python tools/project/candidate_builds.py --print-hashes
tools/environments/python/bin/python tools/project/candidate_builds.py --overlays --print-hashes
```

The print commands report only; they never rewrite tracked metadata. The
`--print-hashes` forms need the corresponding resident or overlay build first.
When a contract changes, first try replacing the private `extern` with its
canonical header; retain a private spelling only when the candidate fingerprint
proves it is a measured part of the current near miss.

This metadata is a tree snapshot. Re-run it after rebasing across canonical
header changes even when `candidates.json` itself has no merge conflict.
Resident candidates scan root, game, and Psy-Q headers, but not
`src/overlays/`; an overlay candidate additionally sees only its own module's
headers under `src/overlays/<module>/`.
