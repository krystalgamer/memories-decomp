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

The game/engine ownership total is 1,195 functions and 396,196 (`0x60BA4`)
function bytes. Implementation status and ownership answer different
questions:

| Status | Meaning |
|---|---|
| `unmatched_asm` | Assembly fallback without an accepted C replacement or independent evidence justifying a handwritten exemption. This does not assert a known original source language. |
| `handwritten_asm` | Game assembly with positive whole-function evidence of hand-managed implementation, recorded in the inventory notes. |
| `sdk_asm` | Identified CRT/SDK-owned code, preserved outside the game decompilation queue. |
| `matching_c` | Accepted C whose complete executable matches retail. |

The split between matching C and unresolved assembly changes whenever a
function is integrated, so it is not duplicated here.
`config/slus_01411/functions.csv` is the source of truth; the generated
progress table in the root [`README.md`](../README.md) is the current readable
snapshot.

GTE instructions, hardware-facing behavior, or a disassembler's heuristic
comment alone do not justify excluding game-owned code from C decompilation.
Existing reviewed classifications are preserved by inventory refresh; newly
discovered assembly defaults to `unmatched_asm`, and SDK ownership remains a
separate region-classification step.

### GTE classification correction (2026-09-08)

The #2390 audit reviewed all 63 formerly heuristic-only classifications,
46,236 bytes (`0xB49C`) in total. Their inventory notes were empty because the
old importer promoted spimdisasm's `/* Handwritten function */` comment
directly to `handwritten_asm`. That comment is triggered by an
instruction-level `isLikelyHandwritten` predicate, not recovered source
provenance.

The audit decoded all 11,559 words with the same Rabbitizer `R3000GTE`
category selected by Splat's PSX backend. Every heuristic trigger was a
COP2 register transfer: 82 `cfc2`, 111 `mfc2`, and 171 `mtc2`, or 364 total.
There were no non-GTE triggers and no unimplemented words. The cohort also
contains 529 `lwc2`, 344 `swc2`, and these 236 recognized GTE commands:

| Operation | Retail command word | Occurrences |
|---|---|---:|
| `rtps` | `0x4A180001` | 42 |
| `rtpt` | `0x4A280030` | 40 |
| `ncds` | `0x4AE80413` | 21 |
| `nccs` | `0x4B08041B` | 25 |
| `ncct` | `0x4B18043F` | 16 |
| `nclip` | `0x4B400006` | 32 |
| `avsz3` | `0x4B58002D` | 30 |
| `avsz4` | `0x4B68002E` | 30 |

That negative result removes the generated-comment rationale, but it is not
the whole-function decision. A second pass found positive non-GTE evidence in
60 routines: they save arbitrary incoming callee-saved GPR values into fixed
fields of the object passed in `a0`, reuse those registers, and restore the
incoming values before returning, without stack frames. Their inventory notes
record the exact register sets, object slots, and save/restore addresses.
This is a hand-managed implementation pattern, while remaining externally
O32-compatible. It does not prove that an original `.s` file has been
recovered or that no custom C/assembly wrapper could reproduce the function.

| Disposition | Functions | Bytes | Evidence |
|---|---:|---:|---|
| Reopened `unmatched_asm` | `func_80033DB0`, `func_80034830` | `0x17E8` | Conventional `0x50`/`0x58` stack frames, ordinary O32 saves and shared epilogues; fixed-register FLAG/depth reads match SDK C-macro shapes. |
| Reopened `unmatched_asm`, origin unresolved | `func_80067220` | `0x134` | No custom incoming-register preservation; software-pipelined GTE loops are not exact stock macro sequences. |
| Retained `handwritten_asm` | 60 functions | `0x9B80` | Custom incoming-register preservation in caller-owned object fields, with no stack frames. |

The retained functions form four end-exclusive cohorts:

| Span | Count | Object save slots |
|---|---:|---|
| `0x800612C0-0x80067220` | 32 | `+0x20..+0x34` or `+0x20..+0x3C` |
| `0x80067354-0x80069E44` | 16 | `+0x28..+0x34` or `+0x28..+0x3C` |
| `0x80069E44-0x8006A99C` | 8 | `+0x20..+0x28` |
| `0x8006A99C-0x8006AF74` | 4 | `+0x28..+0x30` |

For example, `func_800612C0` saves incoming `s0-s3/s6-s7` at
`0x800612D8-0x800612F0` and reloads them at
`0x800614EC-0x80061504`. Stock GTE macros do not request this object-backed
prologue/epilogue.

The [per-function audit](gte-classification-audit.csv) records the GTE
families, heuristic triggers, and final disposition at this checkpoint.
The authoritative inventory carries the positive per-function notes. The
target was the verified North American executable with SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.

Psy-Q's C inline interfaces cover the GTE operation families, but imported
command macros contain DMPSX marker words rather than the native words above;
see the [header caveat](psyq.md#gte-heuristics-and-command-header-caveat).
The [measured build pipeline](research/matching-evidence.md#no-gte-command-instruction-can-currently-be-emitted-from-c)
currently passes those markers unchanged through GCC, MASPSX, and GNU as.
All three reopened functions require at least one affected GTE command, so
they are presently **toolchain-blocked** as C candidates even though their
classification remains `unmatched_asm`. That capability statement is
separate from source origin: it neither proves the functions handwritten nor
reverses the decision to remove an unsupported exemption.

No complete C replacement, game-code edit, function-boundary change, compiler
profile change, SDK reclassification, or marker translation is claimed here.
