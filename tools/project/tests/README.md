# Build-tool and policy regressions

This suite tests the project's tooling and contracts that an exact PSX image
hash cannot establish. Game behavior is verified by the clean resident and
overlay matching jobs, not by a second implementation of each game function.

Keep tests for:

- Input verification, safe staging/extraction, encrypted CI caches and build
  selection.
- Build/split cache invalidation, compiler assembly filters, source grouping,
  inventory/report algorithms and metadata validation.
- Declaration visibility, SDK signatures, source ownership, compiler-profile
  policy and rejection of prohibited matching techniques.
- Symbol extents and native guest-pointer width/zero extension, which are not
  established by a byte-identical PSX executable.

Do not add per-function gameplay oracles, extracted/recompiled game-body
harnesses, game-source spelling assertions, or per-overlay copies of layout,
manifest and instruction expectations already checked by the matching jobs.
Do not freeze live progress totals in tests: independently valid matching
PRs advance those totals and must not require snapshot-assertion rewrites.
Add reusable tooling coverage only when the failure is outside those hashes.
The existing LP64 guest-pointer witness was moved from the French MODEL400
suite without changing its test method or C fragment.

Retired game witnesses remain available in Git history. Research notes link
to their historical versions; those links are evidence of earlier work, not
an active test requirement. Their removal does not relax any resident/overlay
hash, matching-source policy, declaration check or required CI job.

CI discovers the remaining suite with:

```sh
tools/environments/python/bin/python -m unittest discover \
  -s tools/project/tests -p 'test_*.py' -v
```
