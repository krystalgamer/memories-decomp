# APv2 boot protection module

The boot sequence loads WA_MRG module `0x16` at `0x80168000` before the
title screen. It is Sony's APv2 anti-modchip check. `Main_RunBootSequence`
calls `SafetyInit` once and then calls `SafetySync(1)` every frame until
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
| `0xE4` | `safechk_offset` (`s8 dir[8]` in the candidate) |
| `0xF4-0x140C` | code: nine functions |
| `0x140C-0x14D4` | data: eight zero bytes, `__ps_libinfo__`, CD command table, message table, state |
| `0x14D4-` | leftover memory, different in each region; kept raw |

## Behaviour

| Function | USA size | Role |
|---|---|---|
| `SafetyInit` | `0x6C` | Reads BIOS region byte `0xBFC7FF52`. `'A'` selects state 1 and mode 1; `'E'` selects state 0 and mode 2, so the check is skipped; anything else selects state 1 and mode 0. |
| `SafetySync` | `0x938` | 20-state controller: issues the CD commands, checks the results, and shows the warning when the drive answers like a modified console. |
| `func_80168A98` | `0x1D4` | Polls the CD controller for a command result. |
| `func_80168C6C` | `0xF0` | Sends one CD command, described by `safechk_commands` entries of `{cmd, nparams, nresults, wait}` (14 commands). |
| `SafetyCheck2` | `0x16C` | Draws the warning screen, then calls `exit()`. |
| `SafetyCheck3` | `0x174` | Draws a text line. |
| `SafetyCheck4` | `0x1B4` | Draws the glyph rows of one character through `Krom2RawAdd`. |
| `func_801691F0` | `0x94` | Returns a glyph's width. |
| `SafetyCheck5` | `0x188` | Draws the frame polygons. |

The USA warning reads `SOFTWARE TERMINATED` / `CONSOLE MAY HAVE BEEN MODIFIED`
/ `CALL 1-888-780-7690`.

## Identification: safechk 2.0.0.3

The module is Sony's runtime library `safechk`. Its `__ps_libinfo__` block at
`0x80169414` (module offset `0x1414`) reads `50 73 1C 00 00 00 20 03`: the
`Ps` tag, library number 28 (`safechk`), and version 2.0.0.3. The Metal Gear
Solid decompilation ships version 1.0.0.3 as named, hand-written assembly
(`asm/safechk.s` in `FoxdieTeam/mgs_reversing`), whose block reads
`... 10 03`. The symbols here use its names where the call graph or the data
bytes make the correspondence clear:

| Address | Name | Former name | Evidence |
|---|---|---|---|
| `0x801680F4` | `SafetyInit` | `SafetyInit` | Exported name in the 2.0.0.2 object (below); 2.0.0.3 adds the region select. |
| `0x80168160` | `SafetySync` | `func_80168160`, `SafetyCheck` | Exported name in the 2.0.0.2 object, whose body is identical; sends the commands and calls `SafetyCheck2`. |
| `0x80168D5C` | `SafetyCheck2` | `func_80168D5C` | Same SDK call sequence as v1, from `ResetGraph` to `exit`. |
| `0x80168EC8` | `SafetyCheck3` | `func_80168EC8` | Calls `strlen`, `Krom2RawAdd` and `SafetyCheck4`. |
| `0x8016903C` | `SafetyCheck4` | `func_8016903C` | One `DrawPrim` call site, per glyph. |
| `0x80169284` | `SafetyCheck5` | `func_80169284` | Two `DrawPrim` call sites, using `safechk_coords` and `safechk_offset`. |
| `0x801680E4` | `safechk_offset` | `D_801680E4` | Bytes equal v1's `dh 257, 511, -255, -1, 0, 0, 0, 0`. |
| `0x80169414` | `__ps_libinfo__` | (inside `D_8016940C`) | Library 28, version 2.0.0.3. |
| `0x8016941C` | `safechk_commands` | `D_8016941C` | Same `{cmd, nparams, nresults, wait}` words; 14 entries against v1's 10, seven of them shared. |
| `0x80169454` | `safechk_cdregs` | `D_80169454` | Bytes equal v1's: `0x1F801800` to `0x1F801803`. |
| `0x80169464` | `safechk_params` | `D_80169464` | Eight zero bytes, sent as command parameters. |
| `0x8016946C` | `safechk_results` | `D_8016946C` | Eight zero bytes, filled from the controller. |
| `0x80169494` | `safechk_coords` | `D_80169494` | Bytes equal v1's 48. |

The 2.0.0.2 object (`safechk.obj`, an SDK Psy-Q LNK object; not tracked)
confirms two real names and much of the code. Its libinfo reads
`... 20 02`, it exports `SafetyInit` (`.text+0`, four instructions) and
`SafetySync` (`.text+0x10`), and it carries no local symbols, so the
`SafetyCheck2`..`5` labels come from the MGS reconstruction, not from Sony.
Its data equals ours apart from the English warning and the per-region
message table that 2.0.0.3 adds. Comparing code with the object's
relocated words masked:

| Function | Object offset | Unpatched words that differ |
|---|---|---|
| `SafetySync` | `.text+0x10` | 0 of 590 |
| `func_80168A98` | `.text+0x948` | 0 of 117 |
| `func_80168C6C` | `.text+0xB1C` | 0 of 60 |
| `SafetyCheck5` | `.text+0xF20` | 0 of 98 |
| `SafetyInit` | `.text+0x0` | different: 2.0.0.3 adds the region select |
| `SafetyCheck2`..`4`, `func_801691F0` | | changed or new for the English/ASCII text |

The two functions that no tested compiler reproduces are therefore the
same compiled code in both releases.

Caveats:

- v2's `SafetySync` is not v1's `SafetyCheck`. v1's entry is
  `SafetyCheck(minute, second, sector)` and runs to completion; v2's takes one
  `s32` (the boot loop passes 1) and advances the state in `D_80169474` once
  per call.
- v1's `SafetyCheck1` sends a command and waits for its result. v2 splits it
  into `func_80168C6C` (send) and `func_80168A98` (wait), so both keep their
  address names. `func_801691F0` (glyph width) has no v1 counterpart. The message table `D_8016947C`, the state
  words `D_80169474`, `D_80169478`, `D_801694C4`, `D_801694C8` and
  `D_801694D0`, and the bytes `D_801694CC`/`D_801694CD` are
  v2-only and keep their address names too.
- `D_80169458`, `D_8016945C` and `D_80169460` are `safechk_cdregs[1..3]`, and
  `D_8016946D` and `D_8016946E` are `safechk_results[1..2]`. They stay separate
  symbols: indexing the arrays in the candidates links to the same code but
  moves the offset into the objects' relocation addends, so the object bytes
  would change.
- The eight zero bytes at `0x8016940C-0x80169413`, between the end of `.text`
  and `__ps_libinfo__`, are unexplained. In v1, `.data` begins with
  `__ps_libinfo__`.

The `mgs_reversing` file opens with a note that `safechk.obj` was probably
written in C but cannot be matched with "the PSSN compiler toolchain". It says
that SCEI first built the runtime libraries both with GCC 2.x hosted on Unix
(COFF) and with Psy-Q for MS-DOS, and that from Release 4.1 (September 1997)
Sony compiled each release only with the Unix GCC 2.x and post-converted the
COFF objects to Psy-Q format; the few `safechk.obj` releases most likely came
from that environment. That fits the findings below: no SN `CC1PSX` build
reproduces the module, while a Cygnus-derived GCC 2.7.2 comes closest.

## Compiler

The module was not built with the game's GCC 2.8.1 pipeline. It is Sony's
code, built with Psy-Q GCC 2.7.2: decompals/old-gcc `gcc-2.7.2-psx`, installed
by `make compiler-272-psx-prebuilt` and selected through the
`gcc_2_7_2_psx_g0_gnu_reorder` profile. The evidence:

- `SafetyInit` and `SafetyCheck5` match only with 2.7. GCC 2.8 duplicates
  the epilogue that the retail code shares.
- Most retail delay slots are filled the way an assembler in reorder mode
  fills them. Its `move` instructions are `addu d,s,$0`, the ASPSX encoding.
  The slots filled from a branch target come from the compiler (see
  [Which compiler](#which-compiler)).
- All nine candidates match at the stock `-O2` flags. An earlier
  `SafetyCheck3` source needed `-fno-cse-follow-jumps`; the current source
  does not (see the source details below).

MASPSX's normal processing cannot model this output: it always emits
`.set noreorder` and pads jumps with `nop`. With MASPSX 2.67 only
`func_801691F0` matches. The profile therefore runs MASPSX with
`--passthrough`, which only drops COFF debug directives, then
`build_baseline.py` rewrites `move d,s` as `addu d,s,$0` (GNU as would emit
`or`) and lets GNU as reorder the code. At the default optimization level
GNU as fills delay slots as the original assembler did; `-O1`, which CI's
system-binutils flags add, is dropped for these units.

## Candidates

The module still builds from assembly. All nine functions have C candidates in
`src/candidates/apv2/`, with targets in `src/candidates_target/apv2/` and
entries in `config/slus_01411/candidates.json`. Their shared types are in
`src/candidates/apv2/apv2.h`.

| Function | Status |
|---|---|
| `SafetyInit`, `func_80168C6C`, `SafetyCheck2`, `SafetyCheck3`, `SafetyCheck4`, `func_801691F0`, `SafetyCheck5` | exact under the profile |
| `func_80168A98` | 7 words differ |
| `SafetySync` | 457 words differ; exact under Psy-Q 4.1 `CC1PSX` with `-mmips-as` (see below) |

No available compiler builds all nine, so the module cannot move to C as a
whole; see [Which compiler](#which-compiler).

Source details that the matches depend on:

- `SafetyInit` keeps a redundant `'J'` case, which GCC needs to build the
  retail binary-search tree.
- Loops with bounds in memory are written as `if (n) do { } while (...)`. A
  `for` loop makes 2.7 emit `blez` and an unused 8-byte frame.
- The controller keeps its `goto` back to the `switch`. A `do { } while`
  loop gets loop notes, and loop.c then hoists `-1`, `-2`, `1` and `17` into
  saved registers (546 words under Psy-Q 4.1 with `-mmips-as`). Its `case 2`
  and `case 17` labels are in the order that makes reorg pick retail's copy
  of the shared `j; li s0,N` tails.
- `exit()` is declared without a prototype (`PSYQ_EXIT_UNPROTOTYPED` in
  `src/psyq/exit.h`), as the call passes no argument.
- `SafetyCheck3` reuses the character variable `c` for the spacing of the
  Krom, space and ASCII branches (`c = 1`, `8`, `2`), and adds it as
  `rect->x = rect->x + c + rect->w`. Retail reloads the character from the
  string in the Krom and ASCII branches. Without the reassignment, `-O2` CSE
  follows the newline branch into those blocks and reuses `c`, so 12 bytes
  of reloads disappear. Assigning `c` first ends that equivalence; the
  constant then propagates and flow deletes the dead assignment. The
  `rect->x += rect->w + c` form swaps the two `lhu` loads.

## Which compiler

The words that `gcc-2.7.2-psx` cannot produce are compiler output, not
assembler output:

- Retail fills branch delay slots from the branch target, for example
  `li v0,-1; beq s0,v0,L [li v0,1]` in the controller and
  `li v0,10; beq a0,v0,ret [li v0,1]` in `func_80168A98`. GCC's reorg pass
  steals such an instruction only if the register is dead on the fall-through
  path. In FSF 2.7.2 `mark_target_live_regs` stops at the next conditional
  branch and still counts `v0` as live, so these slots stay empty for any
  source. Cygnus's `find_dead_or_set_registers` follows that branch and sees
  `v0` set on both paths.
- The real Psy-Q assemblers, ASPSX 2.56, 2.67, 2.77, 2.81 and 2.86, fill none
  of these slots. They also build an indexed load's address in `$at`. Retail
  builds it in the destination register, as GNU as does.

Every SN compiler that could be found was run on the committed sources: the
`mkst/esa` Psy-Q binaries and the Runtime Library, SDevTC and Net Yaroze discs
in archive.org's `ps1_sdks` collection. The Win32 builds run under `wibo`, the
DOS builds under DOSBox. Each column is a function, named by the last three
digits of its address:

| `CC1PSX` | Found in | 0F4 | 160 | A98 | C6C | D5C | EC8 | 03C | 1F0 | 284 |
|---|---|---|---|---|---|---|---|---|---|---|
| `2.6.0` (DOS) | Psy-Q 3.3 | 0 | 490 | 8 | 0 | 0 | 42 | 70 | 21 | 21 |
| `2.7.2.SN.1` (DOS) | Psy-Q 3.5, 3.6 | 0 | 456 | 107 | 11 | 48 | 3 | 91 | 0 | 57 |
| `2.7.2.SN32.3.7 Build 0001`, `SN16` DOS build | RTL 4.0 (Japan) | 0 | 457 | 7 | 0 | 0 | 0 | 0 | 0 | 0 |
| `2.7.2.SN32.3.7.0002` | Psy-Q 4.0, RTL 4.0 (USA) | 0 | 457 | 7 | 0 | 0 | 0 | 0 | 0 | 0 |
| `2.7.2.SN16.3.7.0003` (DOS) | RTL 4.1 to 4.6 | 0 | 457 | 7 | 0 | 0 | 0 | 0 | 0 | 0 |
| `cygnus-2.7.2-970404 SN32.3.7.0004` | Psy-Q 4.1 | 15 | 522 | 107 | 53 | 84 | 83 | 100 | 0 | 93 |
| same, `-mmips-as` (also `-fpic`, and `0006` from SDevTC 2) | | 0 | **0** | 99 | 1 | 2 | 35 | 0 | 0 | 0 |
| `2.8.0 SN32 Build 4.0.0007` | Psy-Q 4.3, SDevTC 2.4 | 19 | 531 | 108 | 53 | 84 | 83 | 100 | 0 | 77 |
| `2.8.1 SN32 BUILD 4.0.0010` | Psy-Q 4.4 | 19 | 531 | 108 | 53 | 84 | 83 | 100 | 0 | 77 |
| `2.7.2` (DOS) | Net Yaroze | 0 | 457 | 7 | 0 | 0 | 0 | 0 | 0 | 0 |

The 2.8 builds do no better with `-mno-split-addresses` (5, 488, 102, 1, 2,
35, 0, 0, 79), nor does any decompals/old-gcc release from 2.5.7 to 2.95.2. All
FSF-based 2.7.x builds match decompals `gcc-2.7.2-psx`, and the Cygnus builds
match `gcc-2.7.2-cdk`.

On its own, 4.1 splits every global access into
separately scheduled `%hi`/`%lo` instructions (`mips_split_addresses`, which
requires `TARGET_GAS`). Retail has no split, and `-mmips-as` (or `-fpic`) turns
the split off. 4.1 with `-mmips-as` then reproduces the controller exactly,
including its target-filled slots and the `la`-based loads of
`D_8016946D[0..1]`. Two March 1997 Cygnus changes in 4.1 contradict retail
elsewhere:

- `reload_cse_regs` (reload1.c, Ian Lance Taylor, 1997-03-26, "reload1.c
  (reload): Call reload_cse_regs"; FSF r13805) rewrites the delay-loop address
  `move a2,zero` as `move a2,a1` in `func_80168C6C` and `SafetyCheck2`, and
  `move t0,zero` as `move t0,a1` in `func_80168A98`. Retail has `zero` in
  all three. The pass turns a word-sized constant load into a copy whenever
  another register already holds the constant and no label lies between, so
  every delay-loop form tried is rewritten.
- `assign_parms` (function.c, Ian Lance Taylor, 1997-03-18, "Add a REG_EQUIV
  note to the instruction which copies a parameter into a pseudo-register
  whenever there is an associated stack slot") makes `update_equiv_regs`
  double the live length of `rect` and `color` in `SafetyCheck3`, and global
  allocation permutes `s1`-`s4`.

Two source builds frame the compiler that retail needs:

- The `cygnus-2.7.2-970404` source (old-gcc's `b18.tar.gz`) with those two
  ChangeLog entries reverted, and nothing else changed, matches eight
  functions with the committed sources. It also matches `SafetyCheck3` with
  the natural source that does not reassign `c`. `func_80168A98` still differs.
- Cygnus `2.7-96q3 SOA-960904` (September 1996): its GPL source was
  distributed by Sega and is published in `sozud/saturn-compiler-repro`
  (`source/SOA-960904.src.tar.gz`). It has the new reorg pass and neither
  March 1997 change, and has no split addresses at all. Built for
  `mips-sony-psx` with the decompals CDK patches, it matches 0F4, C6C, D5C,
  EC8, 03C, 1F0 and 284. In the controller it fills the same slots as retail,
  but it expands a `u8` global read as a QImode load. CSE then forwards the
  value just stored to `D_801694CC` (457 words), where retail and 970404
  reload it. `func_80168A98` differs as well.

`func_80168A98` needs one reorg decision that neither source makes, and no
source shape produces it. In retail the first `bne v1,v0` takes `move a3,v1`
(the `n = t` copy) from the fall-through. Both Cygnus versions follow the next
conditional branch, `beq a2,zero`. They see `v0` set on both of its paths and
steal `move v0,zero` from the return-0 block instead. Their `reorg.c` files are
the same in this logic. The controller needs exactly this following:

- State 7's `bltz` steals `li v0,-2` in pass 1.
- That frees state 4's `bgez`, which fills its slot in pass 2.

Writing the copy before the compare lets the slot fill backward, as in retail.
CSE then computes `(u8)n` from `t`, and five words differ. A search of 448
generated sources and 16 documented options found nothing better. The
`insn_n_dups[-1]` read in Cygnus `cse.c`, whose value depends on how `cc1` was
built, makes no difference here.
