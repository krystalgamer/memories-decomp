# Resident Function Map

## Ownership regions

The exact assembly split currently contains 1,794 resident functions covering
513,544 bytes.

| Region | Address range | Functions | Function bytes | Classification |
|---|---:|---:|---:|---|
| Startup | `0x800129D8-0x80012B50` | 3 | 376 (`0x178`) | PsyQ/GCC CRT startup |
| Game and engine | `0x80012B50-0x80073704` | 1,196 | 396,212 (`0x60BB4`) | Game-owned working region |
| SDK and runtime | `0x80073704-0x800906D4` | 595 | 116,956 (`0x1C8DC`) | PsyQ/SN libraries and handwritten runtime |

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

The game/engine region currently contains:

| Status | Functions | Bytes |
|---|---:|---:|
| Unmatched assembly | 1,131 | 349,976 (`0x55718`) |
| Heuristically handwritten assembly | 63 | 46,236 (`0xB49C`) |
| Matching C | 0 | 0 |

The 63 handwritten classifications are provisional disassembler heuristics.
They require instruction-level review before being treated as final original
source classifications.
