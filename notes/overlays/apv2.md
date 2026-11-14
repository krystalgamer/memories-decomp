# APv2 boot protection module

The boot sequence loads WA_MRG module `0x16` at `0x80168000` before the
title screen. It is Sony's APv2 anti-modchip check. `Main_RunBootSequence`
calls `func_801680F4` once and then calls `func_80168160(1)` every frame until
it returns zero.

| Region | Module | WA sectors | Header word |
|---|---|---|---|
| USA | `apv2` | `5827` x3 | `0x16` |
| Japan | `japanese_apv2` | `5810` x3 | `0x16` |
| Europe | `european_apv2` | `9509` x3 | `0x09` |
| France, Germany, Italy, Spain | `french_apv2`, `german_apv2`, `italian_apv2`, `spanish_apv2` | `9509` x3 | `0x09` |

There are four distinct images: USA, Japan, Europe, and one image shared by the
four other PAL regions. Their code is identical apart from the `jal` targets
of the twelve resident Psy-Q calls. Each region's `apv2_linker_symbols.txt`
binds those names to the region's addresses.

## Layout

| Offset | Contents |
|---|---|
| `0x0` | module word |
| `0x4` | 20-entry state jump table |
| `0x54` | English warning text |
| `0xA8` | Shift-JIS warning text |
| `0xE4` | `s8 dir[8]` |
| `0xF4-0x140C` | code: nine functions |
| `0x140C-0x14D4` | data: CD command table, message table, state |
| `0x14D4-` | leftover memory, different in each region; kept raw |

## Behaviour

| Function | USA size | Role |
|---|---|---|
| `func_801680F4` | `0x6C` | Reads BIOS region byte `0xBFC7FF52`. `'A'` selects state 1 and mode 1; `'E'` selects state 0 and mode 2, so the check is skipped; anything else selects state 1 and mode 0. |
| `func_80168160` | `0x938` | 20-state controller: issues the CD commands, checks the results, and shows the warning when the drive answers like a modified console. |
| `func_80168A98` | `0x1D4` | Polls the CD controller for a command result. |
| `func_80168C6C` | `0xF0` | Sends one CD command, described by `D_8016941C` entries of `{cmd, nparams, nresults, wait}` (14 commands). |
| `func_80168D5C` | `0x16C` | Draws the warning screen, then calls `exit()`. |
| `func_80168EC8` | `0x174` | Draws a text line. |
| `func_8016903C` | `0x1B4` | Draws the glyph rows of one character through `Krom2RawAdd`. |
| `func_801691F0` | `0x94` | Returns a glyph's width. |
| `func_80169284` | `0x188` | Draws the frame polygons. |

The USA warning reads `SOFTWARE TERMINATED` / `CONSOLE MAY HAVE BEEN MODIFIED`
/ `CALL 1-888-780-7690`.

## Compiler

The module was not built with the game's GCC 2.8.1 pipeline. It is Sony's
code, built with Psy-Q GCC 2.7.2: decompals/old-gcc `gcc-2.7.2-psx`, installed
by `make compiler-272-psx-prebuilt` and selected through the
`gcc_2_7_2_psx_g0_gnu_reorder` profile. The evidence:

- `func_801680F4` and `func_80169284` match only with 2.7. GCC 2.8 duplicates
  the epilogue that the retail code shares.
- The retail delay slots are filled the way an assembler in reorder mode fills
  them. Its `move` instructions are `addu d,s,$0`, the ASPSX encoding.
- `-fno-cse-follow-jumps` lets the natural `func_80168EC8` source match
  without changing the others.

MASPSX cannot model this output: it always emits `.set noreorder` and pads
jumps with `nop`. With MASPSX 2.67 only `func_801691F0` matches. The profile
therefore uses `assembler_mode: gnu_reorder`: `build_baseline.py` skips MASPSX,
rewrites `move d,s` as `addu d,s,$0`, and lets GNU as reorder the code.

## Candidates

The module still builds from assembly. All nine functions have C candidates in
`src/candidates/apv2/`, with targets in `src/candidates_target/apv2/` and
entries in `config/slus_01411/candidates.json`. Their shared types are in
`src/candidates/apv2/apv2.h`.

| Function | Status |
|---|---|
| `func_801680F4`, `func_80168C6C`, `func_80168D5C`, `func_80168EC8`, `func_8016903C`, `func_801691F0`, `func_80169284` | exact under the profile |
| `func_80168A98` | 7 words differ |
| `func_80168160` | 458 words differ |

The controller's remaining differences are mostly delay slots that retail fills
from the branch target and loads that use a different base register. GNU as
does neither, so they probably come from ASPSX's own reorder pass. Until an
assembler step models that, the module cannot move to C as a whole.

Source details that the matches depend on:

- `func_801680F4` keeps a redundant `'J'` case, which GCC needs to build the
  retail binary-search tree.
- Loops with bounds in memory are written as `if (n) do { } while (...)`. A
  `for` loop makes 2.7 emit `blez` and an unused 8-byte frame.
- `exit()` is declared without a prototype (`PSYQ_EXIT_UNPROTOTYPED` in
  `src/psyq/exit.h`), as the call passes no argument.
