# Spanish MODEL variant 432/582 drawing helpers

Model 401, compact record 351, uses this second variant during loader stages
9/10. Both ten-sector images are independently checked against the legal
Spanish `MODEL.MRG`; neither is an identical-image duplicate.

| Stage | Header | Sector | Base | Full image SHA-256 |
|---|---:|---:|---|---|
| 9 | 432 | 97076 | `0x8013B000` | `46387775948e0ac86c0e3a066bb1f817f2838e6be572d5a8cc42985aaae3e117` |
| 10 | 582 | 97086 | `0x8017B000` | `f8b3890710fc32a68a9e7318dce362f8eee5f3af71a82accb199d637e86a6769` |

The 848-byte helper at offset `0x134C` and the 1,052-byte helper at `0x1AC4`
are matching C. Each slot-one wrapper is separately compiled with the named
`gcc_2_8_1_g0_split` profile (GCC 2.8.1 / MASPSX 2.81), and its complete image
is separately linked and hashed. Together these account for four configured
C instances and 3,800 instruction bytes.

## Boundaries and remaining scope

Direct-call control-flow traversal starts at the entry at offset `4` and
follows all internal call targets. Each function's complete contiguous
instruction interval is reachable, with one terminal return and no unresolved
indirect jump. The entry forwards its original context argument to this
first helper at offset `0xBD8`, while the state is 2 or 3. The band helper
receives the same original context at offset `0xBE4`.

| Offset | Size | Reachable words | Status |
|---|---:|---:|---|
| `0x4` | 3496 | 874 | Game-owned unmatched assembly |
| `0xDAC` | 1440 | 360 | Game-owned unmatched assembly |
| `0x134C` | 848 | 212 | Matching C |
| `0x169C` | 1064 | 266 | Game-owned unmatched assembly |
| `0x1AC4` | 1052 | 263 | Matching C |

The four-byte loader header and the complete 12,576-byte suffix starting at
`0x1EE0` retain real generated storage. The suffix is **unclassified**, not an
exclusion or C coverage claim; the visible call graph is not proof that there
are no other runtime entry points. The other three functions per slot remain
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
resident calls needed by the remaining unmatched assembly functions.

The first source experiment already reproduced every non-stack instruction.
The target reserves 16 unused bytes between rotation and scale. An unused
local byte array preserves that observed interval without pretending to know
its original type. Removing it changes 53 stack-related words and reduces the
frame from 272 to 256 bytes. No inline assembly, pinned registers, opaque
instruction arrays, or one-off compiler flags are used.

The experiments, including each exact terminal record, are in
[`spanish-model-variant432-attempts.csv`](spanish-model-variant432-attempts.csv).
Fingerprints identify the scratch source/header pair before include-path
promotion. The resident-only `integrate_verified_match.py` has hard-coded
North American inventories, so regional registration follows the existing
overlay manifests instead of modifying that unrelated tool.

## Fading-band helper

The helper at `0x1AC4` uses three 288-byte records beginning at state offset
`0x5EC`. Each record has two rows of 17 canonical `SVECTOR`s, colors at
272/276, and signed phase at 280; the last four bytes remain unknown.
Sixteen canonical `POLY_GT4` packets begin at state offset `0x2B90`.
Adjacent pairs of row points supply each quad, including the closing
seventeenth point. The two row colors are copied unchanged through phase
3072, then multiplied by `(4096 - phase) / 1024` with the target's integer
evaluation order (multiply before divide).

Only positive phases draw. Each phase below 4096 advances by `step * 160`;
on reaching 4096 it wraps while state is below 4, otherwise it caps at 4096.
Completion of the third band changes state 4 to 6. Position, step and state
share the previously recovered offsets; the partial state size again does
not establish caller allocation.

Twenty-five additional target layout values and the same nine real resident
callees were independently checked. The first candidate differed only in
two scheduled initialization words. Initializing the inner index before
the quad pointer reproduces the target exactly. The band helper does not
require the first helper's unused stack interval. These two additional C
instances add 2,104 bytes without changing any remaining function boundary
or the unclassified suffix.

## Verification scope

The selected production compiler objects must own exactly the helpers'
848-byte and 1,052-byte executable symbols. All other function symbols remain assembly
owners with their original sizes. The full header and suffix must have real
input and linked data owners, not absolute aliases. Both complete 20,480-byte
production images must retain the hashes above; this is not a claim that
their remaining functions are matching C or that all MODEL loads are covered.

Run `make spanish-match-overlays` and the focused
`tools.project.tests.test_european_model_variant432` tests with legal inputs
available. Target-layout, linked-owner and resident-callee receipts remain
local under `tmp/`; no retail or generated binary evidence is committed.

## Independent French registration

French currently registers only the drawing helper at `0x134C`; the
Spanish fading-band helper at `0x1AC4` remains assembly in the French images.
The shared tests keep these regional ownership expectations separate.

Model 401 maps to compact record 351 in the independently verified French
archive. Its command words are `(597000, 598000, -2)`, and the stage-9/10
sectors, headers, load addresses and complete image hashes agree with the
table above. Both images are read from the French archive rather than
inferred from the shared archive hash.

The accepted helper source, wrapper, local declarations and compiler profile
are reused unchanged. Both complete French images are independently linked
first in scratch and then through the production pipeline. Each selected
compiler object owns exactly the 848-byte helper; all eight other functions
retain executable assembly owners, and four header/suffix storage owners
retain their exact bytes and extents.

Direct control-flow traversal independently covers every instruction in all
ten listed function intervals, with their terminal returns and internal calls.
All 34 external callee bodies are compared in hash-verified French and Spanish
residents and checked against actual section-defined executable owners in the
matching French resident ELF. The nine helper callees are checked against
actual object relocations. Forty target-compiled local/SDK layout constants
are verified against the unchanged accepted header.

Only two matching C instances / 1,696 bytes are added. All 34 accepted French
registrations remain unchanged, including the four matching Exodia helpers.
Configured French coverage becomes **36 images, 248/259 matching C instances
and 174,188 instruction bytes**. Eight newly inventoried unmatched instances
remain 14,104 assembly bytes, and both 12,576-byte tails remain unclassified.
The percentage reflects newly visible work, not removal of existing matches.
Other runtime entry points, variants and unclassified regions remain open.

Run `make french-match-overlays` for French images; the shared European test
module exercises both regional manifests and their legal inputs when present.
