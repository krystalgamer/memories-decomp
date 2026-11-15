# safechk 2.0.0.3 boot protection module (`safecheck2003`)

The boot sequence loads WA_MRG module `0x16` at `0x80168000` before the
title screen. It is Sony's anti-modchip check, the runtime library safechk
2.0.0.3, and its modules are named `safecheck2003` after that version (they
were formerly called `apv2`; the two pinned compiler patches under
`tools/bootstrap/patches/` still cite this note as `apv2.md`).
`Main_RunBootSequence` calls `SafetyInit` once and then calls `SafetySync(1)`
every frame until it returns zero.

| Region | Module | WA sectors | Header word |
|---|---|---|---|
| USA | `safecheck2003` | `5827` x3 | `0x16` |
| Japan | `japanese_safecheck2003` | `5810` x3 | `0x16` |
| Europe | `european_safecheck2003` | `9509` x3 | `0x09` |
| France, Germany, Italy, Spain | `french_safecheck2003`, `german_safecheck2003`, `italian_safecheck2003`, `spanish_safecheck2003` | `9509` x3 | `0x09` |

There are four distinct images: USA, Japan, Europe, and one image shared by the
four other PAL regions. Their code is identical apart from the `jal` targets
of the twelve resident Psy-Q calls. Each region's `safecheck2003_linker_symbols.txt`
binds those names to the region's addresses.

## Layout

After the module word, the image is the sections of one object, each
rounded up to sixteen bytes, in the order `.rdata`, `.text`, `.data`,
`.bss`:

| Offset | Section | Contents |
|---|---|---|
| `0x0` | | module word (WA_MRG packaging, kept raw) |
| `0x4` | `.rdata` | 20-entry state jump table of `SafetySync` |
| `0x54` | | English warning text |
| `0xA8` | | Shift-JIS warning text |
| `0xE4` | | `safechk_offset`, the `s8 dir[8]` initializer of `SafetyCheck5`, and 8 padding bytes |
| `0xF4` | `.text` | nine functions, to `0x140C` |
| `0x140C` | | 8 padding bytes |
| `0x1414` | `.data` | `__ps_libinfo__`, `safechk_commands`, `safechk_cdregs`, `safechk_params`, `safechk_results`, `safechk_state` (`= 1`), `safechk_intsum` |
| `0x147C` | | message table `safechk_messages`, `safechk_coords` (34 bytes) and 14 padding bytes |
| `0x14C4` | `.bss` | `safechk_vsync`, `safechk_last_track`, `safechk_minute`, `safechk_second`, `safechk_message_set` |
| `0x14D4-` | | leftover memory, different in each region; kept raw |

The 2.0.0.2 object shows the same rounding: its `.text` (`0x10B0` bytes)
ends in two zero words after the last `jr ra`, its `.rdata` (`0xA0`) in four
zero bytes after the offsets, and its `.data` (`0x90`) in six after the
coordinates. Its `.bss` is the same `0x10` bytes; its `.data` lacks only the
message table.

## Behaviour

| Function | USA size | Role |
|---|---|---|
| `SafetyInit` | `0x6C` | Reads BIOS region byte `0xBFC7FF52`. `'A'` selects state 1 and message set 1; `'E'` selects state 0 and message set 2, so the check is skipped; anything else selects state 1 and message set 0. |
| `SafetySync` | `0x938` | 20-state controller (see Controller states): issues the CD commands, checks the results, and shows the warning when the drive answers like a modified console. |
| `SafetyPollResult` | `0x1D4` | Polls the CD controller for a command's responses; returns 0 pending, 1 done, -1 error, -2 shell open. |
| `SafetySendCommand` | `0xF0` | Sends one CD command, described by `safechk_commands` entries of `{cmd, nparams, nresults, intsum}` (14 commands). |
| `SafetyCheck2` | `0x16C` | Draws the warning screen, then calls `exit()`. |
| `SafetyCheck3` | `0x174` | Draws a text line. |
| `SafetyCheck4` | `0x1B4` | Draws the glyph rows of one character through `Krom2RawAdd`. |
| `SafetyGetGlyphRange` | `0x94` | Returns the lowest and highest bit set in a BIOS-font ASCII glyph; the caller derives the width. |
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
| `0x8016941C` | `safechk_commands` | `D_8016941C` | Same `{cmd, nparams, nresults, intsum}` words; 14 entries against v1's 10, eight of them shared. |
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
| `SafetyPollResult` | `.text+0x948` | 0 of 117 |
| `SafetySendCommand` | `.text+0xB1C` | 0 of 60 |
| `SafetyCheck5` | `.text+0xF20` | 0 of 98 |
| `SafetyInit` | `.text+0x0` | different: 2.0.0.3 adds the region select |
| `SafetyCheck2`..`4`, `SafetyGetGlyphRange` | | changed or new for the English/ASCII text |

The two functions that no tested compiler reproduces are therefore the
same compiled code in both releases.

Caveats:

- v2's `SafetySync` is not v1's `SafetyCheck`. v1's entry is
  `SafetyCheck(minute, second, sector)` and runs to completion; v2's takes one
  `s32` (the boot loop passes 1) and advances the state in `safechk_state` once
  per call.
- v1's `SafetyCheck1` sends a command and waits for its result. v2 splits it
  into `SafetySendCommand` (send) and `SafetyPollResult` (wait), and adds
  `SafetyGetGlyphRange` and the state words; their names are descriptive
  (see Descriptive names).
- `D_80169458`, `D_8016945C` and `D_80169460` were `safechk_cdregs[1..3]`,
  `D_8016946D` and `D_8016946E` `safechk_results[1..2]`, and `D_8016941D` the
  `nparams` byte of `safechk_commands[0]`. The source indexes the arrays;
  the linked bytes are the same.
- The eight zero bytes at `0x8016940C-0x80169413`, between the end of `.text`
  and `__ps_libinfo__`, are the `.text` section's rounding to sixteen bytes
  (see Layout). In v1, `.data` also begins with `__ps_libinfo__`.

The `mgs_reversing` file opens with a note that `safechk.obj` was probably
written in C but cannot be matched with "the PSSN compiler toolchain". It says
that SCEI first built the runtime libraries both with GCC 2.x hosted on Unix
(COFF) and with Psy-Q for MS-DOS, and that from Release 4.1 (September 1997)
Sony compiled each release only with the Unix GCC 2.x and post-converted the
COFF objects to Psy-Q format; the few `safechk.obj` releases most likely came
from that environment. That fits the findings below: no SN `CC1PSX` build
reproduces the module, while a Cygnus GCC 2.7.2 source snapshot does.

## CD-ROM commands

`safechk_commands` holds `{cmd, nparams, nresults, intsum}` per command.
`SafetySendCommand(n)` acknowledges the controller interrupts (index 1,
`0x1F801803 = 7`), sets the interrupt enable register to `0x18`, writes
`nparams` bytes of `safechk_params` to the parameter FIFO `0x1F801802` and
the command byte to `0x1F801801` (index 0). `SafetyPollResult(n)` reads the
response number (`0x1F801803 & 7`), adds it to `safechk_intsum` and
acknowledges; once the sum reaches `intsum` it reads `nresults` bytes from the
response FIFO `0x1F801801` into `safechk_results` and re-enables the
interrupts (`0x1F`). `intsum` is 5 for exactly the commands that answer INT3
then INT2 (3 + 2) and 3 for those that answer INT3 alone; v1's `SafetyCheck1`
waits on the same byte with the sum in a register. An INT5 (error) response
returns -1, or -2 when status bit 4 (ShellOpen) is set; a normal response
with ShellOpen also returns -2, except for command 10, whose first byte is
not a status. The decode follows the psx-spx "CDROM Drive" chapter
(<https://psx-spx.consoledev.net/cdromdrive/>: command summary, status
code, Setmode and test commands):

| Index | Bytes | Command | psx-spx responses | Use |
|---|---|---|---|---|
| 0 | `01 00 01 03` | Getstat (psx-spx: Nop) | INT3(stat) | after shell open (17) and before the warning (16); reading stat clears ShellOpen once the lid is closed |
| 1 | `13 00 03 03` | GetTN | INT3(stat, first, last), BCD | last track to `safechk_last_track` |
| 2 | `14 01 03 03` | GetTD track | INT3(stat, mm, ss), BCD | start of track 2, or of the lead-out (track 0) on a one-track disc |
| 3 | `02 03 01 03` | Setloc amm, ass, asect | INT3(stat) | `safechk_minute`, `safechk_second`, 0 |
| 4 | `16 00 01 05` | SeekP | INT3, INT2 | audio seek to that position |
| 5 | `0E 01 01 03` | Setmode | INT3(stat) | mode 1: bit 0, CDDA |
| 6 | `0A 00 01 05` | Init | INT3, INT2 | after GetTN |
| 7 | `0B 00 01 03` | Mute | INT3(stat) | silence the audio about to play |
| 8 | `03 00 01 03` | Play | INT3(stat) | play from the Setloc position |
| 9 | `19 01 01 03` | Test 04h | INT3(stat) | read SCEx string (and force motor on); resets the counters |
| 10 | `19 01 02 03` | Test 05h | INT3(total, success) | get SCEx counters |
| 11 | `09 00 01 05` | Pause | INT3, INT2 | stop the audio |
| 12 | `1E 00 01 05` | ReadTOC | INT3, INT2 | needs BIOS vC1+; vC0 answers error `40h` (invalid command) |
| 13 | `1A 00 01 05` | GetID | INT3, INT2 or INT5 | an INT5 (for example ID denied) shows the warning |

v1's ten entries are Getstat, MotorOn (`07`), Setloc, SeekP, Setmode, a
four-result Test (sent as `20h`, the controller version), Mute, Play, a
two-result Test (sent as `04h`, then as `05h`) and Pause. 2.0.0.3 keeps eight
of those words, drops MotorOn and the four-result Test, gives Test `04h` its
own one-result entry and adds GetTN, GetTD, Init, ReadTOC and GetID; the
2.0.0.2 table is identical to 2.0.0.3's.

The check is the one psx-spx describes under Test `04h`: it "resets the
total/success counters to zero, and does then try to read the SCEx string
from the current location (the SCEx is stored only in the Lead-In area, so,
if the drive head is elsewhere, it will usually not find any strings, unless
a modchip is permanently simulating SCEx strings)". The controller plays
muted CD-DA from halfway to track 2 (or to the lead-out), sends Test `04h`,
waits 200 vertical blanks and reads the counters with Test `05h`. A nonzero
success count leads to the warning. v1 runs the same Test `04h`/`05h` pair at
the position its caller passes.

## Controller states

`safechk_state` selects the case of `SafetySync`; `SafetySync(1)` runs one
state per call, `SafetySync(0)` runs states until one returns 0. "Send n"
is `SafetySendCommand(n)`, and each polling state calls
`SafetyPollResult` with the command its predecessor sent; a 0 result stays
in the state. Unless noted, -1 restarts at state 1, and -2 sends Getstat and
goes to state 17.

| State | Polls | On success | Exceptions |
|---|---|---|---|
| 0 | | finished: returns 0 | `'E'` BIOS starts here |
| 1 | | send 1 (GetTN), go to 2 | start state |
| 2 | GetTN | `safechk_last_track = safechk_results[2]`; send 6 (Init), go to 3 | -1 resends GetTN |
| 3 | Init | `safechk_params[0]` = 2 if the last track is at least 2, else 0; send 2 (GetTD), go to 4 | |
| 4 | GetTD | half of the returned mm:ss to `safechk_minute`/`safechk_second` (BCD); send 12 (ReadTOC), go to 5 | |
| 5 | ReadTOC | send 13 (GetID), go to 6 | -1: if stat bit 0 is clear, state 1; if the error byte has `40h` (no ReadTOC), Setloc as in state 6 and go to 7; otherwise stay |
| 6 | GetID | Setloc(`safechk_minute`, `safechk_second`, 0), go to 7 | -1: warning (`SafetyCheck2`) |
| 7 | Setloc | `safechk_params[0] = 1`; send 5 (Setmode CDDA), go to 8 | |
| 8 | Setmode | `safechk_vsync = VSync(-1)`, go to 9 | |
| 9 | | after more than 3 VSyncs: send 4 (SeekP), go to 10 | |
| 10 | SeekP | send 7 (Mute), go to 11 | |
| 11 | Mute | send 8 (Play), go to 12 | |
| 12 | Play | `safechk_params[0] = 4`; send 9 (Test 04h), go to 13 | |
| 13 | Test 04h | `safechk_vsync = VSync(-1)`, go to 14 | |
| 14 | | after more than 200 VSyncs: `safechk_params[0] = 5`; send 10 (Test 05h), go to 15 | |
| 15 | Test 05h | success count nonzero: send 0 (Getstat), go to 16; zero: `safechk_vsync = VSync(-1)`, go to 19 | |
| 16 | Getstat | warning (`SafetyCheck2`) | |
| 17 | Getstat | state 1 (restart) | -2 resends Getstat and stays |
| 18 | Pause | state 0 | |
| 19 | | after more than 10 VSyncs: send 11 (Pause), go to 18 | |

Two details are as compiled: state 4 builds the BCD second's units digit
from the already converted BCD minute (`safechk_minute % 10`), and an `'E'`
BIOS selects message set 2, outside the two-entry `safechk_messages`, but
never reaches the warning.

## Descriptive names

Sony's own names are known only for `SafetyInit` and `SafetySync`, which the
2.0.0.2 object exports; it has no local symbols. `SafetyCheck2`..`5` and the
`safechk_*` data names come from the MGS reconstruction of v1. The
functions and data that v1 lacks, or that it keeps in registers, have
descriptive names in the same two families: `Safety<Verb><Object>` like
Sony's exported entries, rather than the project's `<Subsystem>_<Verb>`
form, and lowercase `safechk_<role>` like the v1 data. They were address
names until the roles above were worked out.

| Address | Name | Former name | Confidence | Evidence |
|---|---|---|---|---|
| `0x80168A98` | `SafetyPollResult` | `func_80168A98` | high | Reads the interrupt flags and response FIFO and returns 0, 1, -1 or -2 (CD-ROM commands); the wait half of v1's `SafetyCheck1`. 2.0.0.2 `.text+0x948`. |
| `0x80168C6C` | `SafetySendCommand` | `func_80168C6C` | high | Writes the parameters and command byte; the send half of v1's `SafetyCheck1`. 2.0.0.2 `.text+0xB1C`. |
| `0x801691F0` | `SafetyGetGlyphRange` | `func_801691F0` | high | Returns `(lowest set bit << 16) \| highest set bit` over a glyph's 15 rows; `SafetyCheck3` computes the width from it and `SafetyCheck4` draws that bit range. New in 2.0.0.3. |
| `0x80169474` | `safechk_state` | `D_80169474` | high | The `SafetySync` switch value; 2.0.0.2 `.data+0x60`, set by its four-instruction `SafetyInit`. |
| `0x80169478` | `safechk_intsum` | `D_80169478` | high | The running sum compared with `intsum` (formerly called `wait`); 2.0.0.2 `.data+0x64`. |
| `0x8016947C` | `safechk_messages` | `D_8016947C` | high | Two `SafechkMessage` entries; entry 0 is the text that v1 keeps alone as `safechk_message`. New in 2.0.0.3. |
| `0x801694C4` | `safechk_vsync` | `D_801694C4` | high | `VSync(-1)` at the start of the state 9, 14 and 19 delays; 2.0.0.2 `.bss+0`. |
| `0x801694C8` | `safechk_last_track` | `D_801694C8` | high | GetTN's last track; 2.0.0.2 `.bss+4`. |
| `0x801694CC` | `safechk_minute` | `D_801694CC` | high | Setloc's minute; v1 takes it as `SafetyCheck`'s `minute`. 2.0.0.2 `.bss+8`. |
| `0x801694CD` | `safechk_second` | `D_801694CD` | high | Setloc's second (with the units-digit caveat above); v1's `second`. 2.0.0.2 `.bss+9`. |
| `0x801694D0` | `safechk_message_set` | `D_801694D0` | high | Set by `SafetyInit` from the region byte; indexes `safechk_messages` and selects kanji (0) or ASCII glyphs. New in 2.0.0.3: the 2.0.0.2 object has no reference to `.bss+0xC`. |

The 2.0.0.2 offsets are those of the object's relocations. The struct types
in `safechk.h` are `SafechkCdCommand`, `SafechkMessage`, `SafechkRect`,
`SafechkGlyphPrim` and `SafechkFramePoly`.

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

The module selects `gcc_2_7_2_cygnus_g0_mips_as_gnu_reorder`:

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
  dropped for these units. `build_baseline.py` also ends `.text`, `.rdata`
  and `.data` with `.align 4`, the sixteen-byte rounding of Sony's
  assembler, which `mipsel-none-elf` GNU as does not apply.

MASPSX's normal processing cannot model this output: it always emits
`.set noreorder` and pads jumps with `nop`.

## Source

The module builds from C in every region: `src/overlays/safecheck2003/safechk.c`
is the whole library as one translation unit, with its types and
prototypes in `safechk.h`. Each region's `safecheck2003.yaml` places the
unit's `.rdata` at `0x4`, `.text` at `0xF4`, `.data` at `0x1414` and `.bss`
at `0x14C4`, and keeps the module word and the leftover bytes from `0x14D4`
raw; the `*_matching_c.json` manifests select the Cygnus profile, and each
region's `safecheck2003_linker_symbols.txt` binds the twelve resident calls.
The nine functions were promoted from exact per-function candidates, which
are retired.

Definition order reproduces both data sections. `.data` follows definition
order; the string literals of the message table go to `.rdata` when the
table is defined, the English text first, so the table comes after
`SafetySync`, whose jump table leads `.rdata`, and `SafetyCheck5`'s `dir`
initializer follows them. The data are `static`, as the 2.0.0.2 object
exports only `SafetyInit` and `SafetySync`; the five `.bss` words are
tentative definitions, which GCC emits as `.lcomm` in declaration order.
`safechk_params`, `safechk_results` and `safechk_intsum` have explicit zero
initializers: GCC 2.7 puts zero-initialized statics in `.data`.

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
- `SafetyPollResult` reads the controller status inside `do { } while (0)`.
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
- `color` is an `s32` from `SafetyCheck2` through `SafetyCheck4`. As a
  `u16` parameter of `SafetyCheck3`, its pass to `SafetyCheck4` gains an
  `andi` (two words).
- `SafetyCheck3` is written plainly (`rect->x += rect->w + 1`, `+= 8`,
  `+= rect->w + 2`). The retired `gcc_2_7_2_psx_g0_gnu_reorder` profile
  (decompals `gcc-2.7.2-psx`) it used before needed `c` reassigned to the
  spacing in each branch.

## Which compiler

The target-filled delay slots rule out FSF 2.7.x. Retail fills slots from the
branch target, for example `li v0,-1; beq s0,v0,L [li v0,1]` in the controller
and `li v0,10; beq a0,v0,ret [li v0,1]` in `SafetyPollResult`. GCC's reorg pass
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
digits of its address; `SafetyPollResult` (A98) is the source before the loop head.

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
  a2,a1` in `SafetySendCommand` and `SafetyCheck2`, and `move t0,zero` as `move
  t0,a1` in `SafetyPollResult`. Retail has `zero`. The pass
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
| FSF r13561 `538fe8cddb4` | 1997-01-29 Ian Lance Taylor: combine.c (try_combine), "Add a case where reg_n_refs is cleared" | `SafetyPollResult` 118 words without it |
| `bd8c4b14b51` | 1996-12-10 Kenner: "Treat ZERO_EXTEND like SIGN_EXTEND in emit_unop_insn" | without it `u8` global reads become QImode loads: the controller 457 words, as in 96q3, and `SafetyCheck3` 63 |
| FSF r13734 `38b610ed8e0` | 1997-03-18 Ian Lance Taylor: assign_parms REG_EQUIV | present: `SafetyCheck3` 35 words |
| FSF r13805 `2a9fb5489fc` | 1997-03-26 Ian Lance Taylor: reload_cse_regs | present: `SafetyPollResult` 1, `SafetySendCommand` 1, `SafetyCheck2` 2 |
| `1f2d8f510f4` | 1997-07-11 Wilson: MIPS RTL epilogue | present: `SafetyInit` 5, `SafetyCheck5` 26, `SafetyPollResult` 36 |

No other applied commit changes any of the nine functions, apart from a
1996-07 `expand_mult_highpart` change and its revert, which cancel. The
retail compiler is therefore a Cygnus snapshot whose FSF merge falls between
1997-01-29 and 1997-03-18, which the 970404 tree less those two later
changes reproduces. As a cross-check, the real Psy-Q 4.1 `CC1PSX` with
`-mmips-as` differs from retail on the current sources in exactly the words
those two changes explain: `SafetyPollResult` 1, `SafetySendCommand` 1,
`SafetyCheck2` 2 and `SafetyCheck3` 35; the 970404 source built without the
reverts gives the same.
