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
reproduces the module, while a Cygnus GCC 2.7.2 source snapshot does.

## Compiler

The module was not built with the game's GCC 2.8.1 pipeline. It is Sony's
code, built with a Cygnus GCC 2.7.2 from early 1997: the
`cygnus-2.7.2-970404` source (decompals/old-gcc `b18.tar.gz`, the compiler
of Psy-Q 4.1 `CC1PSX` SN32.3.7.0004) without two of its March 1997 changes.
`make compiler-272-cygnus` builds it from source
(`tools/bootstrap/old_gcc_cygnus.py`, pinned by
`tools/bootstrap/old_gcc_cygnus.json`):

- the `b18.tar.gz` source and the four decompals CDK build patches
  (`Makefile.in`, `obstack.h`, little-endian `mips.h`, the `mips-sony-psx`
  configuration), at old-gcc commit `cf70ec58`;
- `tools/bootstrap/patches/cygnus-2.7.2-970404-revert-r13734.patch`, the
  reverse of FSF r13734 (gcc-mirror `38b610ed8e0`, Ian Lance Taylor,
  1997-03-18, "function.c (assign_parms): Add a REG_EQUIV note to the
  instruction which copies a parameter into a pseudo-register whenever there
  is an associated stack slot, even if the parameter actually arrived in a
  register");
- `tools/bootstrap/patches/cygnus-2.7.2-970404-revert-r13805-reload-call.patch`,
  the reverse of the "reload1.c (reload): Call reload_cse_regs" hunk of FSF
  r13805 (gcc-mirror `2a9fb5489fc`, Ian Lance Taylor, 1997-03-26). The new
  static functions of that commit stay in the file, unused;
- a Debian copy of byacc 20140715 to generate the C parser, which the
  snapshot does not ship.

The build uses the host C compiler and takes well under a minute. A zig
`x86-linux-musl` build of the same tree emits identical assembly.

The candidates select `gcc_2_7_2_cygnus_g0_mips_as_gnu_reorder`:

- `-O2` with the module's usual `-G0 -mel -mips1 -mcpu=R3000 -msoft-float
  -fno-builtin`, plus `-mmips-as`. Built for GNU as, this Cygnus snapshot
  splits every global access into separately scheduled `%hi`/`%lo`
  instructions (`mips_split_addresses`, which needs `TARGET_GAS`); retail has
  no split, and `-mmips-as` turns it off.
- MASPSX runs with `--passthrough`, which only drops COFF debug directives;
  `build_baseline.py` then rewrites `move d,s` as `addu d,s,$0` (GNU as would
  emit `or`) and lets GNU as reorder the code. Most retail delay slots are
  filled the way an assembler in reorder mode fills them, with the ASPSX
  `move` encoding; the slots filled from a branch target come from the
  compiler's reorg pass. `-O1`, which CI's system-binutils flags add, is
  dropped for these units.

MASPSX's normal processing cannot model this output: it always emits
`.set noreorder` and pads jumps with `nop`.

## Candidates

The module still builds from assembly. All nine functions have C candidates in
`src/candidates/apv2/`, with targets in `src/candidates_target/apv2/` and
entries in `config/slus_01411/candidates.json`. Their shared types are in
`src/candidates/apv2/apv2.h`.

All nine are exact under the profile: built through `candidate_builds.py`,
each object linked at its retail address, with the jump table of
`SafetySync` at `0x80168004` and `safechk_offset` at `0x801680E4`, reproduces
the retail bytes. Linked against each region's `apv2_linker_symbols.txt`,
the same objects reproduce all four regional images as well. The module can
therefore move to C as a whole.

Source details that the matches depend on:

- `SafetyInit` keeps a redundant `'J'` case, which GCC needs to build the
  retail binary-search tree.
- Loops with bounds in memory are written as `if (n) do { } while (...)`. A
  `for` loop makes 2.7 emit `blez` and an unused 8-byte frame.
- The controller has no loop. `SafetySync` returns
  `SafetySync_Run(arg, 0)`, a static inline function that runs the `switch`
  and repeats by tail call (`return SafetySync_Run(arg, ret)`). GCC compiles
  the tail call as a jump back to the `switch`, after the parameter copies,
  so `ret = 0` stays before the jump target. A `do { } while` or `for (;;)`
  loop gets loop notes: loop.c hoists `-1`, `-2`, `1` and `17` into saved
  registers (546 words), and even when loop.c skips the loop (a second entry
  into it), flow's loop-depth weighting of register references reorders
  local-alloc in the state 4 BCD block (8 words). A direct
  `return SafetySync(arg)` jumps back before `ret = 0` (9 words).
- Integration substitutes a constant array address into every access
  (`try_constants` in integrate.c), while retail loads `safechk_results`,
  `D_8016946D` and `safechk_params` once into a register (389 words).
  States 4, 5 and 6, which touch several bytes of one buffer, take its
  address from an inline accessor (`SafetySync_Results` and the like) into
  a local pointer. The integrated accessor ends in a label, and the
  substitution does not cross labels.
- The controller's `case 2` and `case 17` labels are in the order that makes
  reorg pick retail's copy of the shared `j; li s0,N` tails.
- `exit()` is declared without a prototype (`PSYQ_EXIT_UNPROTOTYPED` in
  `src/psyq/exit.h`), as the call passes no argument.
- `func_80168A98` reads the controller status inside `do { } while (0)`.
  reorg (`mostly_true_jump`) predicts a jump out of a loop as not taken, so
  the `return 0` branch fills its delay slot from the fall-through (`move
  a3,v1`) instead of taking `move v0,zero` from the return block. Without the
  loop, every Cygnus compiler steals from the target (99 words). v1's
  `SafetyCheck1` (`asm/safechk.s` in `mgs_reversing`) shows where the loop
  comes from: it re-reads the status until two reads agree (`bne v1,v0,
  @lab_12c; move a1,v1`), then tests `andi v0,a1,0xff`. v2 returns 0 instead
  of reading again, but keeps the loop around the read, the copy after the
  compare (`stat = t`) and the `u8` test (`c = stat`). The results loop
  reuses `t` as its index, which keeps the retail register assignment.
- `SafetyCheck3` is written plainly (`rect->x += rect->w + 1`, `+= 8`,
  `+= rect->w + 2`). The `gcc-2.7.2-psx` profile it used before needed `c`
  reassigned to the spacing in each branch.

## Which compiler

The target-filled delay slots rule out FSF 2.7.x. Retail fills slots from the
branch target, for example `li v0,-1; beq s0,v0,L [li v0,1]` in the controller
and `li v0,10; beq a0,v0,ret [li v0,1]` in `func_80168A98`. GCC's reorg pass
steals such an instruction only if the register is dead on the fall-through
path. In FSF 2.7.2 `mark_target_live_regs` stops at the next conditional
branch and still counts `v0` as live, so these slots stay empty for any
source. Cygnus's `find_dead_or_set_registers` follows that branch and sees
`v0` set on both paths. The real Psy-Q assemblers, ASPSX 2.56, 2.67, 2.77,
2.81 and 2.86, fill none of these slots, and they build an indexed load's
address in `$at` where retail builds it in the destination register, as GNU
as does.

Every SN compiler that could be found was run on the earlier sources: the
`mkst/esa` Psy-Q binaries and the Runtime Library, SDevTC and Net Yaroze discs
in archive.org's `ps1_sdks` collection. The Win32 builds run under `wibo`, the
DOS builds under DOSBox. Each column is a function, named by the last three
digits of its address; `func_80168A98` is the source before the loop head.

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
35, 0, 0, 79), nor does any decompals/old-gcc release from 2.5.7 to 2.95.2.
All FSF-based 2.7.x builds match decompals `gcc-2.7.2-psx` (7 of 9 exact), and
the Cygnus builds match `gcc-2.7.2-cdk`. Psy-Q 4.1 with `-mmips-as`
reproduces the controller exactly. Two March 1997 changes in it contradict
retail elsewhere, and are the two reverts above:

- `reload_cse_regs` rewrites the delay-loop address `move a2,zero` as `move
  a2,a1` in `func_80168C6C` and `SafetyCheck2`, and `move t0,zero` as `move
  t0,a1` in `func_80168A98`. Retail has `zero`. The pass
  turns a word-sized constant load into a copy whenever another register
  already holds the constant and no label lies between, so every delay-loop
  form tried is rewritten.
- The `assign_parms` change makes `update_equiv_regs` double the live length
  of `rect` and `color` in `SafetyCheck3`, and global allocation permutes
  `s1`-`s4`.

### Bisecting the history

Cygnus did not publish its own revision history, but its `reorg.c`, `cse.c`,
`sched.c`, `combine.c` and most other passes are FSF trunk files of the day
plus marked `CYGNUS LOCAL` hunks, and FSF trunk is in the GCC Git history
(gcc-mirror). The 970404 files equal FSF revisions from 1997-03-28 to
1997-04-03; the September 1996 Cygnus `2.7-96q3 SOA-960904` snapshot (Sega's
GPL source, `sozud/saturn-compiler-repro`) carries `reorg.c` as of FSF
`eed04dff2e1` (1996-05-20).

Every FSF `reorg.c` revision from the 2.7.2 release to 1997-03-28, swapped
alone into the 970404 tree with the two reverts:

| `reorg.c` | Date, author: entry | 160 | A98 (old source) | Others |
|---|---|---|---|---|
| FSF 2.7.2 release | 1995-11 | 220 | 54 | 0 |
| `126db1fa328` | 1995-12-31 Law: fill_simple_delay_slots tries the target of an unconditional branch | 497 | 54 | 0 |
| `2142b2d5d6e` | 1996-01-15 Kenner: TRAP_IF sets volatil | 497 | 54 | 0 |
| `c170c8c2082` | 1996-01-24 Wilson: find_dead_or_set_registers (Cygnus 1995-11-14 "Rewrite code to handle conditional branches") | 0 | 99 | 0 |
| `b55f96db06a` to `a6c383b0d8a` | 1996-04-15 to 1997-03-28 (spill registers at labels, update_block, EPILOGUE_USES, asm operands, moved insns, REG_BR_PROB) | 0 | 99 | 0 |

Only the conditional-branch following matters, and the controller needs it;
`MAX_REORG_PASSES` is 2 throughout. The A98 decision is not a reorg revision
but the branch prediction described in the source details, which every
version has.

The rest of the compiler was bisected by applying FSF trunk commits to the
same tree, one build per commit: the 455 commits to the cc1 sources and the
MIPS port from 1997-04-05 back to 1996-06-01, undone newest first, and the 202
up to the egcs import of 1997-08-21, applied oldest first. Commits that only
build together were applied as a group; the 102 that conflict with Cygnus
changes (the two March changes among them) were skipped. That gave 285 builds
backward and 95 forward. With the current sources:

| Commit | Date, author: entry | Effect when absent / present |
|---|---|---|
| FSF r13561 `538fe8cddb4` | 1997-01-29 Ian Lance Taylor: combine.c (try_combine), "Add a case where reg_n_refs is cleared" | `func_80168A98` 118 words without it |
| `bd8c4b14b51` | 1996-12-10 Kenner: "Treat ZERO_EXTEND like SIGN_EXTEND in emit_unop_insn" | without it `u8` global reads become QImode loads: the controller 457 words, as in 96q3, and `SafetyCheck3` 63 |
| FSF r13734 `38b610ed8e0` | 1997-03-18 Ian Lance Taylor: assign_parms REG_EQUIV | present: `SafetyCheck3` 35 words |
| FSF r13805 `2a9fb5489fc` | 1997-03-26 Ian Lance Taylor: reload_cse_regs | present: `func_80168A98` 1, `func_80168C6C` 1, `SafetyCheck2` 2 |
| `1f2d8f510f4` | 1997-07-11 Wilson: MIPS RTL epilogue | present: `SafetyInit` 5, `SafetyCheck5` 26, `func_80168A98` 36 |

No other applied commit changes any of the nine functions, apart from a
1996-07 `expand_mult_highpart` change and its revert, which cancel. The
retail compiler is therefore a Cygnus snapshot whose FSF merge falls between
1997-01-29 and 1997-03-18, which the 970404 tree less those two later
changes reproduces. As a cross-check, the real Psy-Q 4.1 `CC1PSX` with
`-mmips-as` differs from retail on the current sources in exactly the words
those two changes explain: `func_80168A98` 1, `func_80168C6C` 1,
`SafetyCheck2` 2 and `SafetyCheck3` 35; the 970404 source built without the
reverts gives the same.
