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
function bytes. `matching_c` records an exact C result; `unmatched_asm`
retains the assembly fallback while a C candidate is unresolved. Neither
status, by itself, recovers the original source language.

The split between matching C and unresolved assembly changes whenever a
function is integrated, so it is not duplicated here.
`config/slus_01411/functions.csv` is the source of truth; the generated
progress table in the root [`README.md`](../README.md) is the current readable
snapshot.

`handwritten_asm` is reserved for independently justified source-assembly
provenance. GTE instructions, hardware-facing behavior, or a disassembler's
heuristic comment alone do not justify excluding game-owned code from C
decompilation. Existing explicit inventory classifications are preserved by
inventory refresh; newly discovered assembly defaults to `unmatched_asm`,
and SDK ownership remains a separate region-classification step.

### GTE classification correction (2026-09-08)

The former 63-function, 46,236-byte game `handwritten_asm` cohort was
reclassified as `unmatched_asm` after a complete instruction audit for #2390.
All 63 inventory notes were empty. The classification came from importing
spimdisasm's `/* Handwritten function */` comment, which its
`MipsSymbolFunction` emits for the instruction-level
`isLikelyHandwritten` heuristic, not from recovered original assembly.

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

The [per-function audit](gte-classification-audit.csv) records every
address, size, command/transfer family, and trigger count at this correction
checkpoint. It is historical evidence, not a second current-status ledger.
The target was the verified North American executable with SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.

Psy-Q's C inline interfaces cover these GTE operation and transfer families.
That establishes a C/macro avenue, not a verified rewrite of any of these
whole functions. In particular, the imported command macros contain DMPSX
marker words rather than the native words above; see the
[header caveat](psyq.md#gte-heuristics-and-command-header-caveat).
Exact instruction encodings, stalls, register allocation, and the complete
executable still have to match before any candidate becomes `matching_c`.
No game code, function boundaries, compiler profiles, or SDK ownership
changed in this classification correction.
