# Resident Function Map

## Ownership regions

The exact assembly split currently contains 1,795 resident functions covering
513,544 bytes.

| Region | Address range | Functions | Function bytes | Classification |
|---|---:|---:|---:|---|
| Startup | `0x800129D8-0x80012B50` | 3 | 376 (`0x178`) | PsyQ/GCC CRT startup |
| Game and engine, first span | `0x80012B50-0x80058F10` | 917 | 287,680 (`0x463C0`) | Game-owned working region |
| Embedded LIBGS getter | `0x80058F10-0x80058F20` | 1 | 16 (`0x10`) | PsyQ `GsGetWorkBase` |
| Game and engine, second span | `0x80058F20-0x80073704` | 278 | 108,516 (`0x1A7E4`) | Game-owned working region |
| SDK and runtime | `0x80073704-0x800906D4` | 596 | 116,956 (`0x1C8DC`) | PsyQ/SN libraries and handwritten runtime |

The boundaries are stored in
`config/slus_01411/function_regions.json`. `make classify-functions` applies
them to `config/slus_01411/functions.csv` without overriding a future
`matching_c` status.

## Source classification

Ownership and implementation status answer different questions:

| Status | Meaning |
|---|---|
| `unmatched_asm` | Assembly fallback without an accepted C replacement or independent evidence justifying intentional handwritten game assembly. This does not assert that its original source language is known. |
| `handwritten_asm` | Game assembly with positive evidence of intentional hand-authored implementation, recorded in the inventory notes. A disassembler hint or a hardware instruction alone is insufficient. |
| `sdk_asm` | Identified CRT/SDK-owned code, preserved outside the game decompilation queue whether it uses GTE instructions or not. |
| `matching_c` | An accepted C implementation whose complete executable matches the retail target. Register bindings and authentic SDK macro use are not a license for arbitrary inline-assembly transcription. |

The original inventory importer in commit `a5af7052` promoted Splat's
`/* Handwritten function */` annotation directly to `handwritten_asm`.
The underlying spimdisasm `SymbolFunction` sets `isLikelyHandwritten` when
an instruction's `isLikelyHandwritten()` predicate fires, then prints that
annotation. It is a disassembly heuristic, not a recovered source-file
declaration or a per-function provenance review.

Newly discovered functions therefore start as `unmatched_asm`. Inventory
regeneration preserves existing reviewed status, ownership, and notes;
the region classifier still applies the independent CRT/SDK ownership
boundaries. A proven handwritten classification remains supported and must
be made deliberately with evidence, rather than inferred from a generated
comment. Direct GTE operations can come from the authentic Psy-Q C macro
interfaces described in [the SDK header guide](psyq.md), but macro availability
alone does not prove the surrounding routine was originally compiled from C
or that a proposed replacement will reproduce its instruction scheduling.

## Boundary evidence

### Startup to game

The startup range:

- Clears `0x8009B090-0x800FE728`.
- Establishes the runtime stack and `_gp = 0x8009AF08`.
- Runs constructor/destructor tables.
- Calls `0x80012B50`, the first game/engine function.

### Game to SDK

The canonical four-instruction `GsGetWorkBase` at `0x80058F10` is an embedded
PsyQ exception inside the broader game-code address span. Game-owned code
resumes at `0x80058F20`, so `function_regions.json` records the getter as its
own region rather than assigning the complete `0x80012B50-0x80073704` range
to the game.

`0x800736C4` is a conventional game debug function that calls the
`check_point` diagnostic. At `0x80073704`, the instruction style immediately
changes to SN/PsyQ host-I/O wrappers using `break` service calls, for example
service `0x103`.

All 234 inter-function gaps, totaling `0x6F4` bytes, occur in the SDK region.
Most are archive-member alignment rather than unidentified game code.

## Source-path anchors

| String | Virtual address | Known references |
|---|---:|---|
| `S3000000.C` | `0x80011814` | Assertion sites including code near `0x8005C978` |
| `src/hirata/H_mctrl1.c` | `0x800118CC` | Error paths near `0x80070D68` and `0x80070DF8` |

These strings support source-module hypotheses but do not reveal complete
translation-unit boundaries.

## SDK anchors

The SDK region is linked to embedded library data by direct references:

- Interrupt/library state around `0x80092A68` references the `intr.c` RCS
  string and callbacks in the `0x80074524-0x800749B4` range.
- CD-ROM handlers reference the `bios.c` RCS string; `CD_init`-like code begins
  around `0x8007A2E0`.
- GPU vectors reference the `sys.c` RCS string; `ResetGraph`-like code begins
  around `0x8007F350`.
- Memory-card diagnostics are used near `0x8008D0FC`.
- GTE push/pop matrix diagnostics attach to handwritten routines around
  `0x800871D0` and `0x80087274`.
- MDEC diagnostics cover the final resident routines through `0x800906D4`.

Library function names remain address-based until matched against a verified
library object or another high-confidence signature.

## Game-region status

The game/engine ownership totals are stable:

| Classification | Functions | Bytes |
|---|---:|---:|
| Game code without a handwritten exemption | 1,135 | 356,388 (`0x57024`) |
| Evidence-backed handwritten assembly | 60 | 39,808 (`0x9B80`) |
| Total game/engine region | 1,195 | 396,196 (`0x60BA4`) |

The first row's split between matching C and assembly fallback changes
whenever a function is integrated, so it is not duplicated here.
`config/slus_01411/functions.csv` is the source of truth; the generated
progress table in the root [`README.md`](../README.md) is the latest generated
snapshot, refreshed separately.

Only independently justified `handwritten_asm` functions are excluded from
the ordinary C candidate queue. An `unmatched_asm` classification does not
promise that an exact C replacement is already known.

## Handwritten/GTE audit

The September 8, 2026 audit for issue #2390 reviewed all 63 previously
handwritten-classified game functions, totaling 46,236 bytes (`0xB49C`).
All 11,559 instruction records, file offsets, and complete function extents
were compared with the verified North American executable. The two spans
are `0x80033DB0-0x80035598` and `0x800612C0-0x8006AF74`; all ranges here
are end-exclusive. With the `0x800`-byte executable header and load address
`0x80010000`, the corresponding file spans are `0x245B0-0x25D98` and
`0x51AC0-0x5B774`.

The result is **three unmatched functions, including one explicitly unresolved
leaf, and 60 retained handwritten implementations with independent evidence**.
This is a source-classification correction, not three new C matches or proof
of the original source-file language.

| Disposition | Function or address span | Functions | Bytes | Decisive evidence |
|---|---|---:|---:|---|
| `unmatched_asm` | `func_80033DB0` | 1 | `0xA80` | Conventional `0x50` stack frame and O32 saves; exact SDK C-macro instruction shapes around GTE operations |
| `unmatched_asm` | `func_80034830` | 1 | `0xD68` | Conventional `0x58` stack frame and O32 saves; the same SDK C-macro evidence |
| `unmatched_asm`, source origin unresolved | `func_80067220` | 1 | `0x134` | No custom incoming-register preservation; software-pipelined GTE loops require the separate caveat below |
| Retained `handwritten_asm` | `0x800612C0-0x80067220` | 32 | `0x5F60` | Live-transform textured variants save incoming `s0-s3/s6-s7` or `s0-s7` in argument-object word slots `+0x20..+0x34` or `+0x20..+0x3C` |
| Retained `handwritten_asm` | `0x80067354-0x80069E44` | 16 | `0x2AF0` | Cached-geometry textured variants save incoming `s0-s3` or `s0-s5` in argument-object word slots `+0x28..+0x34` or `+0x28..+0x3C` |
| Retained `handwritten_asm` | `0x80069E44-0x8006A99C` | 8 | `0xB58` | Live-projection closed-line variants save incoming `s0-s2` in argument-object word slots `+0x20..+0x28` |
| Retained `handwritten_asm` | `0x8006A99C-0x8006AF74` | 4 | `0x5D8` | Cached-projection closed-line variants save incoming `s0-s2` in argument-object word slots `+0x28..+0x30` |

### Two clear GTE-macro cases

`func_80033DB0` and `func_80034830` preserve their callee registers on the
stack and restore them through conventional shared epilogues at
`0x80034804-0x80034830` and `0x80035568-0x80035598`. Each makes two ordinary
calls. Signed divide-by-three lowering in the first and signed four-vertex
averaging in the second are compiler-like, not source-language proof.
Their scratchpad addresses beginning at `0x1F800380` are data buffers, not
the incoming-register save slots found in the retained cohort.

The first function has nine `RTPS`, nine `NCDS`, and two `NCLIP` sites;
the second has twelve, twelve, and two. Every command has the two preceding
`nop` instructions used by the normal SDK C-macro forms. In particular,
all 21 fixed-`t4` FLAG reads and all 21 fixed-`t4` shifted-depth reads have
the exact shapes of `gte_stflg` and `gte_stszotz` in
[`inline_c.h`](../src/psyq/inline_c.h), lines 1024-1030 and 1082-1089.
Those macros explicitly clobber register `$12` and memory; a fixed scratch
register is therefore not evidence that the whole routine was hand-written.
Examples occur at `0x80033FF4-0x80034000` and
`0x8003403C-0x8003404C`, with corresponding quad examples at
`0x80034AB0-0x80034ABC` and `0x80034AF8-0x80034B08`.

The apparently unusual SXY transfer order `12,14,13` at
`0x8003415C-0x80034168` and `0x80034C10-0x80034C1C` is also the actual
`gte_ldsxy3` macro order (`inline_c.h`, lines 181-186). The `NCDS` sites
use the existing `IR0` from projection, so they are not claimed to be full
`gte_NormalColorDpq` expansions: that higher-level macro also loads `IR0`.
These observations remove the old handwritten exemption without claiming
that a complete C implementation or its exact scheduling has been recovered.

### Why 60 functions remain handwritten

Every retained function stores arbitrary **incoming** callee-saved GPR
values in fixed fields of the object addressed by `a0`, reuses those
registers, and restores the same incoming values before returning. None
allocates a stack frame. The inventory now records each function's actual
register set, word-slot offsets, and save/restore instruction addresses.
For example, `func_800612C0` saves `s0-s3/s6-s7` at
`0x800612D8-0x800612F0` and reloads them at
`0x800614EC-0x80061504`, using `a0+0x20..+0x34` word slots.

These routines are externally O32-compatible: they restore callee-saved
registers, return through `ra`, and produce a `v0` result. The distinguishing
evidence is **hand-managed register preservation in caller-owned storage**,
not an ABI violation. Stock Psy-Q GTE macros neither implement nor request
that prologue/epilogue. A C file with a custom assembly wrapper is still
possible; the classification documents a hand-managed implementation rather
than asserting that an original `.s` file has been recovered.

The GTE operations themselves are ordinary SDK-supported geometry work:
the live textured variants use `RTPT`/`RTPS`, `AVSZ3`/`AVSZ4`,
`NCCS`/`NCCT`, and optional `NCLIP`; the cached textured variants use
depth averaging and optional clipping; the closed-line variants use
projection and/or depth averaging without lighting or clipping commands.
Even the smaller AVSZ-only functions retain the same independent save/restore
evidence. Repeated packet loops, single-`lwr` field extraction, fixed scratch
registers, and scheduling CPU work into GTE hazard slots are not separate
grounds for excluding a function.

### The unresolved leaf at `0x80067220`

This function has no stack frame, uses no callee-saved GPRs, and returns
normally with `v0 = input record + 0x1C`. It lacks the neighboring functions'
custom incoming-register preservation. However, its two loops at
`0x80067288-0x800672E8` and `0x80067318-0x80067348` are visibly
software-pipelined: buffered `v0/v1` values are transferred with `mtc2`,
the next input is prefetched before the loop bound is tested, and output
advances in a branch delay slot. The projection loop packs `IR0` and `SZ3`
into one output word. The `t4 = 0xFF` assignment at `0x80067260` appears
unused within the function.

This is not labeled a third clear stock-C-macro implementation. The standard
`gte_ldv0` uses memory-to-GTE loads rather than that buffered `mtc2` pair;
`gte_stszotz` shifts `SZ3` instead of packing `IR0`. Assembly-side SDK
register-read macros cover the operations' meanings, not an exact supported
C source sequence. The scheduling is assembly-like, but source origin is
unresolved, so the function remains eligible as `unmatched_asm` rather than
being excluded as proven handwritten code. Confidence in its original
implementation style is lower than for either of the other cohorts.

### SDK coverage and preserved history

All eight observed GTE command families have SDK definitions in
[`inline_c.h`](../src/psyq/inline_c.h): `RTPS`/`RTPT` at lines 484-492,
`NCDS`/`NCCS`/`NCCT` at lines 734-752, and `NCLIP`/`AVSZ3`/`AVSZ4`
at lines 764-777. Normal and `_b` no-padding forms must not be conflated.
The command macros contain DMPSX marker words, not the retail COP2 encodings:
for example, `gte_rtps` contains `0x0000007f`, while retail `RTPS` is
`0x4A180001`. This audit does not establish drop-in command compatibility
with the current assembler pipeline or change any SDK header. Exact
FLAG/depth-transfer macro shapes are stronger evidence than command-family
availability, but neither constitutes a complete compiled replacement.
[`gtemac.h`](../src/psyq/gtemac.h) composes these operations but warns
against mechanically replacing four-vertex sequences with a superficially
equivalent macro. Its C interfaces are distinct from the assembler-only
register-result helpers in `libgte.h` and `gtenom.h`.

The audit changes no function address, size, name, ownership, matching
manifest, or executable implementation. All 65 existing canonical nonmatch
records and 312 reference-mode records for this cohort remain intact,
including the 52 deferred reference histories. The three reclassified
functions already have canonical attempts, so they appear in the selector
with `--include-partial`, not the zero-attempt queue. Reclassification is not
a new compiler experiment and does not reset an attempt budget.
