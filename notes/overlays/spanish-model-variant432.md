# Spanish MODEL variant 432/582 drawing helper

Model 401, compact record 351, uses this second variant during loader stages
9/10. Both ten-sector images are independently checked against the legal
Spanish `MODEL.MRG`; neither is an identical-image duplicate.

| Stage | Header | Sector | Base | Full image SHA-256 |
|---|---:|---:|---|---|
| 9 | 432 | 97076 | `0x8013B000` | `46387775948e0ac86c0e3a066bb1f817f2838e6be572d5a8cc42985aaae3e117` |
| 10 | 582 | 97086 | `0x8017B000` | `f8b3890710fc32a68a9e7318dce362f8eee5f3af71a82accb199d637e86a6769` |

Only the 848-byte helper at offset `0x134C` is matching C. The slot-one wrapper
is separately compiled with the named `gcc_2_8_1_g0_split` profile (GCC 2.8.1 /
MASPSX 2.81), and its complete image is separately linked and hashed.
This adds two configured C instances and 1,696 instruction bytes.

## Boundaries and remaining scope

Direct-call control-flow traversal starts at the entry at offset `4` and
follows all internal call targets. Each function's complete contiguous
instruction interval is reachable, with one terminal return and no unresolved
indirect jump. The entry forwards its original context argument to this
helper at offset `0xBD8`, while the state is 2 or 3.

| Offset | Size | Reachable words | Status |
|---|---:|---:|---|
| `0x4` | 3496 | 874 | Game-owned unmatched assembly |
| `0xDAC` | 1440 | 360 | Game-owned unmatched assembly |
| `0x134C` | 848 | 212 | Matching C |
| `0x169C` | 1064 | 266 | Game-owned unmatched assembly |
| `0x1AC4` | 1052 | 263 | Game-owned unmatched assembly |

The four-byte loader header and the complete 12,576-byte suffix starting at
`0x1EE0` retain real generated storage. The suffix is **unclassified**, not an
exclusion or C coverage claim; the visible call graph is not proof that there
are no other runtime entry points. The other four functions per slot remain
generated assembly and count against the configured matching percentage.

## Local declaration and behavior evidence

The helper draws four projected quads for each of two 152-byte records.
Each record contains four rows of four canonical `SVECTOR`s, colors at
offsets 128/132, and a signed scale at 136. The final 12 bytes are unknown.
The first three vertices use the second color and the fourth uses the first.
The projected depth is multiplied by 8 and divided by 10 before submission;
negative depths or projection flags suppress submission.

Local state fields are established by actual halfword/word accesses:
records at `0x15C`, the canonical `POLY_GT4` at `0x2B5C`, position at `0x2F64`,
frame at `0x309C`, step at `0x30A8`, and state at `0x30E8`.
Odd frames add one eighth of the record scale. States below 3 grow scale
by `step * 512`, capped at 8192. State 3 shrinks by `step * 128`; reaching
zero clears scale and sets state 4. The partial structure size of 12,524
bytes is a compiler-layout observation, **not caller allocation proof**.

Forty target-compiled size/offset values establish these local and SDK
layouts. Canonical declarations come only from this repository's Psy-Q
headers. `ReadRotMatrix` reads the current rotation/translation controls;
`SetRotMatrix` writes rotation controls. Both bindings were checked against
their actual resident instruction bodies, not guessed from nearby names.
The nine distinct helper callees have real executable symbol owners in the
matching Spanish resident ELF. The linker bindings additionally retain the
resident calls needed by the four unmatched assembly functions.

The first source experiment already reproduced every non-stack instruction.
The target reserves 16 unused bytes between rotation and scale. An unused
local byte array preserves that observed interval without pretending to know
its original type. Removing it changes 53 stack-related words and reduces the
frame from 272 to 256 bytes. No inline assembly, pinned registers, opaque
instruction arrays, or one-off compiler flags are used.

Both experiments, including the exact terminal record, are in
[`spanish-model-variant432-attempts.csv`](spanish-model-variant432-attempts.csv).
Fingerprints identify the scratch source/header pair before include-path
promotion. The resident-only `integrate_verified_match.py` has hard-coded
North American inventories, so regional registration follows the existing
overlay manifests instead of modifying that unrelated tool.

## Verification scope

The selected production compiler object must own exactly the helper's
848-byte executable symbol. All other function symbols remain assembly
owners with their original sizes. The full header and suffix must have real
input and linked data owners, not absolute aliases. Both complete 20,480-byte
production images must retain the hashes above; this is not a claim that
their remaining functions are matching C or that all MODEL loads are covered.

Run `make spanish-match-overlays` and the focused
`tools.project.tests.test_spanish_model_variant432` tests with legal inputs
available. Target-layout, linked-owner and resident-callee receipts remain
local under `tmp/`; no retail or generated binary evidence is committed.
