# External Reference and Inline Refinement Pass

Date: 2026-08-31

## Scope and policy

This pass used same-address files under
`tmp/references/ygofm-decomp/src/` only as source-structure evidence.
Reference headers, declarations, types, macros, compiler claims, flags,
toolchain documentation, and assembly were not trusted. Local declarations
and profiles remained derived from this project's binary, callers,
relocations, matching sources, and tracked compiler matrix.

Only game-owned functions were processed. No Psy-Q CRT or SDK function was
targeted.

## Reference cohort integration

All 12 prepared reference cohorts were validated as one disjoint union before
integration:

| Result | Count |
|---|---:|
| Game targets | 379 |
| External attempt rows | 1,601 |
| Exact pure-C matches | 130 |
| Nonterminal partial histories | 33 |
| Deferred after six misses | 216 |
| Duplicate targets | 0 |
| Validation errors | 0 |

Every target history was imported through
`tools/project/record_external_attempt.py`. Exact matches were integrated one
function per commit, with a full executable link and target SHA-256 check.
Nonmatches and deferrals were also committed one function at a time so their
campaign measurements remain durable.

## Inline-assembly refinement

The post-integration inventory found 136 matching game sources containing a
GCC asm extension: actual inline instructions, empty scheduling barriers,
register-variable bindings, or a combination of those constructs.

Every one of the 136 functions received one new pure-C
`inline_refinement` attempt:

| Reference classification | Matched | Nonmatch | Total |
|---|---:|---:|---:|
| Pure-C same-address reference | 29 | 88 | 117 |
| Same-address reference also used GCC asm | 1 | 15 | 16 |
| No same-address reference | 1 | 2 | 3 |
| **Total** | **31** | **105** | **136** |

The 31 exact refinements replaced their tracked sources atomically:

```text
0x80013940  0x80019B2C  0x8001B7AC  0x8001D3C4
0x8002DF2C  0x8002E6B8  0x800380D4  0x80038110
0x800384E4  0x8003CFC8  0x8003FE80  0x8004365C
0x80043960  0x80043EBC  0x80047AD0  0x80049308
0x8004975C  0x8004B6E8  0x8004B70C  0x8004BAA0
0x800533D8  0x80058A7C  0x80059000  0x800591C0
0x80059AA8  0x8007058C  0x80070988  0x80070C60
0x80070EB4  0x80071194  0x80073480
```

The remaining 105 functions have one recorded nonmatching refinement in the
historical inline-refinement ledger. Their candidate hashes, profiles,
references when available, and failure summaries are in
`config/slus_01411/external_attempts.csv`; continued exploration is not capped
by the unused rows in that campaign ledger.

The current remaining asm-extension inventory is:

| Construct | Count |
|---|---:|
| Sources | 105 |
| Actual asm statements | 45 |
| Emitted instruction or word lines | 858 |
| Empty compiler barriers | 50 |
| Register variables with asm bindings | 241 |

## Acceptance

The combined external ledger contains 1,737 valid rows: 1,601
`reference_match` rows and 136 `inline_refinement` rows. The final clean
rebuild matches the retail executable:

```text
SHA-256 84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88
```
