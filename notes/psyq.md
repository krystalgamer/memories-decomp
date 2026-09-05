# Psy-Q Runtime and SDK Integration

## Scope

The resident executable contains 598 functions classified as Psy-Q CRT or SDK
code:

| Region | Address range | Functions | Bytes |
|---|---:|---:|---:|
| CRT startup | `0x800129D8-0x80012B50` | 3 | `0x178` |
| SDK and runtime | `0x80073704-0x800906D4` | 595 | `0x1C8DC` |

These functions remain exact assembly and do not count toward game-code
decompilation progress. The goal is to identify their original interfaces and
reuse compatible declarations in game C, not to rewrite Sony library objects.

The Sony Computer Entertainment *Run-Time Library Reference* is the primary
API reference:

<https://archive.org/details/SCE-RunTimeLibRef-Sep1999/>

The manual documents interface names and layouts, but it is not evidence that
this executable used the manual's September 1999 library revisions. Embedded
RCS strings, binary signatures, call sites, and data layout remain the local
version authority.

## Evidence and version policy

Use SDK identities only when supported by local evidence. In descending order:

1. An exact object or function signature from a verified Psy-Q library.
2. A documented BIOS vector or syscall wrapper with matching service values.
3. An embedded library string plus matching implementation and call contract.
4. A manual signature corroborated by all local callers and data accesses.
5. An external symbol catalogue used only as a candidate for local review.

The executable contains these library anchors:

| Evidence | Address |
|---|---:|
| `intr.c` revision 1.75, 1997-02-07 | `0x800119B8` |
| `bios.c` revision 1.86, 1997-03-28 | `0x80011D70` |
| `sys.c` revision 1.140, 1998-01-12 | `0x80012148` |
| Sony library copyright 1993-1997 | `0x800919A8` |

They establish member provenance and minimum dates, not a complete SDK release
number. The project independently identifies the toolchain as Psy-Q 4.6.
External Psy-Q 4.7 signature catalogues are useful research evidence, but must
not be imported in bulk or used to override a conflicting local signature.

## Confirmed interface anchors

The following resident addresses have especially strong API-level evidence.
Project symbols remain address-based until each identity passes the normal
symbol review.

| Address | Candidate SDK identity | Local evidence |
|---|---|---|
| `0x80073860` | `OpenEvent` | Memory-card setup passes documented event classes, specifications, mode, and callbacks. |
| `0x80073870` | `CloseEvent` | Called on each stored event descriptor during memory-card teardown. |
| `0x80073890` | `EnableEvent` | Called on all eight descriptors immediately after creation. |
| `0x800738B0` | `EnterCriticalSection` | Brackets event creation and teardown with `0x800738C0`. |
| `0x800738C0` | `ExitCriticalSection` | Paired critical-section exit. |
| `0x80073920` | `nextfile` | Advances a caller-owned directory record and returns that same pointer on success. |
| `0x80073AC0` | `firstfile` | Receives a formatted device path and caller-owned directory record, returning that record on success. |
| `0x80073E1C` | `InitPAD` | `Input_InitPads` passes two adjacent 34-byte receive buffers and their exact lengths. |
| `0x80073EAC` | `StartPAD` | Called immediately after `InitPAD` to start the controller service before local input state is reset. |
| `0x80074170` | `VSync` | Applied Psy-Q 4.6 identity; matching callers query frame timing for AI yielding and time-varying screen effects. |
| `0x8007A860`, `0x8007E8A0` | `CdDataCallback` copies | Byte-identical wrappers that install a callback on DMA channel `3`. |
| `0x8007D3F0` | `DsSearchFile` | Receives a 24-byte file record and a path, then supplies disc-position data. |
| `0x8007E350` | `CdFlush` | No-argument wrapper around the CD library's internal state-reset routine. |
| `0x8007E3D0` | `CdGetSector` | Identified CD-sector transfer interface in the resident CD library. |
| `0x8007E4F0` | `CdGetSector2` | Parallel two-argument sector-transfer wrapper using the library's second transfer path. |
| `0x8007A710` | `CdIntToPos` | Applied Psy-Q 4.6 LIBCD identity; canonical copy of the sector-to-packed-BCD position conversion. |
| `0x8007E600` | `CdIntToPos_8007E600` | Applied address-qualified identity for the second byte-identical resident copy used by matching game C. |
| `0x800781F0` | `CdPosToInt` | Applied Psy-Q 4.6 LIBCD identity; canonical copy of the packed-BCD position-to-sector conversion. |
| `0x8007E710` | `CdPosToInt_8007E710` | Applied address-qualified identity for the second byte-identical resident copy used by matching game C. |
| `0x8007E7F0` | `CdControlB` | Submits the three-argument CD command and blocks until the internal completion code is `2`. |
| `0x8007E860` | `CdReadyCallback` | Replaces and returns the callback invoked with a ready-event status and result pointer. |
| `0x8007E880` | `CdSyncCallback` | Replaces and returns the callback invoked from the command-completion path. |
| `0x8007F350` | `ResetGraph` | Anchored by GPU `sys.c` evidence and the documented graph-reset contract. |
| `0x8007F6CC` | `DrawSync` | Applied Psy-Q 4.6 identity; `model_handler_registry.c` waits for queued GPU drawing after dispatching a model primitive handler. |
| `0x8007F978` | `LoadImage` | Applied Psy-Q identity; `func_800249E0` uses the tracked `RECT *` / `u32 *` prototype for two image transfers. |
| `0x8007FA38` | `MoveImage` | Applied Psy-Q 4.6 identity; matching callers copy rectangular VRAM regions for screen transitions and palette processing. |
| `0x80081DE8` | `LoadImage2` | Applied Psy-Q identity; streamed package callbacks pass rectangle-shaped records and staged image data. |
| `0x80081ED4` | `StoreImage2` | Applied Psy-Q 4.6 identity; `func_800582C0` reads a VRAM rectangle into a local pixel buffer before transforming and re-uploading it. |
| `0x80081FC0` | `MoveImage2` | Applied Psy-Q identity; `file_cd_helpers.c` passes the current display rectangle plus destination coordinates. |
| `0x800866A0` | `rsin` | Applied Psy-Q 4.6 identity; matching callers use its 4096-unit fixed-point sine output for model and display motion. |
| `0x80086770` | `rcos` | Applied Psy-Q 4.6 identity; matching callers use its 4096-unit fixed-point cosine output alongside `rsin`. |
| `0x800878B0` | `SetGeomOffset` | Applied Psy-Q 4.6 identity; matching projection paths set the GTE screen-center coordinates. |
| `0x800878D0` | `SetGeomScreen` | Applied Psy-Q 4.6 identity; matching projection paths set the GTE projection-plane distance. |
| `0x8007FAF0` | `ClearOTag` | Ordering-table initialization behavior. |
| `0x8007FC64` | `DrawPrim` | Direct GPU primitive submission behavior. |
| `0x8007FCC0` | `DrawOTag` | Ordering-table submission behavior. |
| `0x8007FD30` | `PutDrawEnv` | Draw-environment submission contract. |
| `0x8007FEFC` | `PutDispEnv` | Display-environment submission contract. |
| `0x800803F4` | `GetDispEnv` | Applied Psy-Q identity; `file_cd_helpers.c` passes the tracked `DISPENV` record and reuses its leading `disp` rectangle. |

Duplicate library copies require address-qualified symbols rather than aliases.
For example, CD conversion helpers appear more than once in the executable,
and the two 0x24-byte routines at `0x8007A860` and `0x8007E8A0` are
instruction-for-instruction copies. Each forwards its callback argument to the
resident DMA callback installer with channel `3`, matching `CdDataCallback`.
CD teardown selects one of these wrappers according to the active library
state, so both linked addresses are live members rather than redundant padding.
One original name cannot be assigned to multiple resident addresses.

The callback invocation paths distinguish the two adjacent setter routines:

- `0x8007E860` swaps the pointer at `D_800F8394`. Initialization registers
  dispatcher `0x8007BB74` through `0x8007BED4`, whose destination is
  `D_800F5F88`. That dispatcher explicitly recognizes event code `1`
  (`CdlDataReady`) and forwards its one-byte event code and result pointer to
  `D_800F8394`.
- `0x8007E880` swaps the distinct pointer at `D_800F8398`. Initialization
  registers dispatcher `0x8007BC48` through `0x8007BEE0`, whose destination is
  `D_800F5F8C`. The command state machine at `0x8007C7D4` calls
  `D_800F5F8C` with literal event code `2` (`CdlComplete`) at `0x8007C940`;
  `0x8007BC48` then forwards the status and unchanged second argument to
  `D_800F8398`.

This pins `CdReadyCallback` and `CdSyncCallback`, respectively, rather than
relying on the adjacent function order. Both setters return the previous
pointer and use the common status-and-result callback shape.

## Shared declaration policy

The repository owner has chosen to track the real Psy-Q 4.6 headers under
`src/psyq/` so reverse-engineering effort remains focused on the game rather
than reconstructing SDK declarations. Game C should include those headers
directly when it uses a Psy-Q interface.

The imported headers retain Sony's interfaces, scalar spelling, and
documentation. Narrow project adaptations are made only where a fixed-width
record or local include boundary requires them: affected headers include
`src/types.h` and use `u8`, `u16`, or `u32` for those adapted widths. Native
SDK prototypes that use `long`, `unsigned long`, or `unsigned char` remain
unchanged when no project alias is needed. Do not maintain parallel clean-room
declarations for interfaces already provided by these headers.

The four original Japanese root-counter comments in `kernel.h` are stored as
UTF-8 rather than Shift-JIS so repository tools can follow the transitive
`libapi.h` include chain. The declarations and comment text are unchanged.

Recommended header boundaries are:

| Header | Interfaces and records |
|---|---|
| `src/psyq/r3000.h` | MIPS memory segments, exception/status bits, CP0 registers, and context indices |
| `src/psyq/asm.h` | Numeric and ABI register aliases for assembly-facing headers |
| `src/psyq/kernel.h` | BIOS event descriptors, task contexts, executable headers, and directory entries |
| `src/psyq/libapi.h` | Events, critical sections, counters, and low-level memory-card/BIOS wrappers |
| `src/psyq/malloc.h` | Heap initialization and the three allocator families |
| `src/psyq/stdlib.h` | C runtime umbrella for allocation, conversion, sorting, random numbers, search, and exit |
| `src/psyq/abs.h` | Integer absolute-value function and macro |
| `src/psyq/convert.h` | Integer text conversion plus `labs` |
| `src/psyq/qsort.h` | In-place generic sorting with the original unprototyped comparator |
| `src/psyq/rand.h` | Fifteen-bit `rand` result contract and `srand` seed entry point |
| `src/psyq/ctype.h` | Table-driven character classification plus case-conversion functions and macros |
| `src/psyq/limits.h` | Integral-width limits for the Psy-Q MIPS compiler target |
| `src/psyq/stddef.h` | Target definitions of `size_t`, `wchar_t`, `NULL`, and `WEOF` |
| `src/psyq/stdarg.h` | Integer-slot-aligned variadic argument traversal macros |
| `src/psyq/setjmp.h` | Single-task non-local jumps with an explicit saved MIPS register layout |
| `src/psyq/assert.h` | Debug assertion macro using `printf` and `exit`, disabled by `NDEBUG` |
| `src/psyq/stdio.h` | Minimal formatted and character console I/O declarations |
| `src/psyq/libmath.h` | Software floating-point math, conversion helpers, and math error globals |
| `src/psyq/memory.h` | Byte-memory operations plus the BSD `bcopy`/`bzero`/`bcmp` aliases |
| `src/psyq/strings.h` | String operations, including search/token helpers, layered over `memory.h` |
| `src/psyq/string.h` | Compatibility wrapper that includes `strings.h` |
| `src/psyq/libsn.h` | Debugger-host PC file service and `pollhost`/`PSYQpause` break traps |
| `src/psyq/fs.h` | Low-level filesystem device-table, character-buffer, and I/O-block records |
| `src/psyq/sys/types.h` | Target ABI typedefs plus major/minor device-number helpers |
| `src/psyq/sys/errno.h` | Runtime error numbers 1-37 and the external `errno` object |
| `src/psyq/sys/fcntl.h` | Internal `F*` file-mode and device-I/O flags |
| `src/psyq/sys/file.h` | Public `O_*` aliases and seek-origin constants layered over `sys/fcntl.h` |
| `src/psyq/sys/ioctl.h` | Encoded file, terminal/SIO, and disk control request constants |
| `src/psyq/romio.h` | ROM-monitor compatibility include for the system file interface |
| `src/psyq/libsio.h` | Base SIO status, mode, control, lifecycle, and callback interface |
| `src/psyq/libcomb.h` | COMB packet, transfer, control-line, and asynchronous request interface |
| `src/psyq/libcd.h` | CD commands, locations, file records, callbacks, and sector transfers |
| `src/psyq/libds.h` | Ds packet/streaming commands, locations, file records, and callbacks |
| `src/psyq/libetc.h` | Simple pad polling, vertical sync, callbacks, and video mode |
| `src/psyq/libpad.h` | Direct pad communication, Pad-driver multitap/gun setup, modes, and actuator services |
| `src/psyq/libgun.h` | Legacy `InitGUN`/`SelectGUN` light-gun lifecycle interface |
| `src/psyq/libtap.h` | Legacy `InitTAP` multitap lifecycle and enable/disable interface |
| `src/psyq/libgpu.h` | Rectangles, draw/display environments, images, primitives, and ordering tables |
| `src/psyq/libgte.h` | Fixed-point geometry records, assembler-side transfer macros, and out-of-line GTE helpers |
| `src/psyq/inline_c.h` | GCC extended-assembly loads, stores, commands, and state helpers for direct GTE use from C |
| `src/psyq/inline_o.h` | Alternate GCC GTE macros that route operands through fixed temporary registers |
| `src/psyq/gtemac.h` | High-level geometry macros composed from the lower-level `gte_*` operations |
| `src/psyq/inline_s.h` | Preprocessed `aspsx` GTE command macros, with padded and unpadded forms |
| `src/psyq/gtereg_s.h` | Preprocessed `aspsx` names for GTE data and control registers |
| `src/psyq/inline_a.h` | `macro`/`endm`-style assembler GTE command definitions |
| `src/psyq/gtereg.h` | `equs`-style assembler names for GTE data and control registers |
| `src/psyq/gtenom.h` | `macro`/`endm`-style assembler GTE read and store helpers |
| `src/psyq/libgs.h` | `Gs` work areas, objects, lights, cameras, and sorting helpers |
| `src/psyq/libhmd.h` | Hierarchical-model units, primitive handlers, animation, and MIMe records |
| `src/psyq/libsnd.h` | VAB banks, sequences, tones, and high-level playback |
| `src/psyq/libspu.h` | SPU voices, transfer state, reverb, and callbacks |
| `src/psyq/libmcrd.h` | High-level `MemCard` status, directory, and I/O interfaces |
| `src/psyq/libmcx.h` | MCX application, clock, memory, serial, LED, device, and UIFS services |
| `src/psyq/mcgui.h` | Configurable memory-card save/load UI environment and entry points |
| `src/psyq/libpress.h` | MDEC environment, callbacks, input, output, and VLC helpers |

`r3000.h` defines the hardware-facing MIPS vocabulary: cached and uncached
segment conversions, exception vectors, status/cause bits, coprocessor
register names, and saved-context indices. `asm.h` supplies the corresponding
numeric and assembler register aliases. `kernel.h` layers the BIOS ABI records
and constants over both headers, including `TCB`, `EvCB`, `EXEC`, `XF_HDR`,
and the 40-byte `DIRENTRY`. These are low-level ABI definitions; they are not
game-owned scheduler or filesystem structures merely because their fields
have similar roles.

`libapi.h` is the callable BIOS-wrapper layer over `kernel.h`. It declares
root-counter control, events, threads, low-level `open`/`read`/`write` and
directory iteration, executable loading, pad lifecycle, critical sections,
register access, and raw memory-card services. The records passed to
`firstfile`, `nextfile`, `Load`, and `Exec` come from `kernel.h`, while the
`O_*` mode aliases for `open` come from the separate `sys/file.h` stack.
`libmcrd.h` is a higher-level card API and must not be substituted for the
`_card_*` block operations merely because both address memory cards.

`input_init_pads.c` directly includes `libapi.h` for the confirmed resident
`InitPAD` and `StartPAD` calls. The `OpenEvent`, critical-section, `firstfile`,
and `nextfile` call sites still use local declarations pending exact header
migration. No current game C directly includes `kernel.h` or `libmcrd.h`.

The graphics headers also form distinct layers. `libgpu.h` owns the GPU packet
ABI: `RECT`, `DRAWENV`, `DISPENV`, primitive records, packet-construction
macros, image transfers, and direct primitive or ordering-table submission.
`libgte.h` supplies the fixed-point vector and matrix types used for geometry.
`libgs.h` then builds scene coordinates, cameras, lights, object records, and
sorting helpers on top of both lower layers.

The imported `libgpu.h` also refers to `SVECTOR` in its model-primitive
records without including `libgte.h`. Game C that uses `libgpu.h` therefore
includes `libgte.h` first, even when its own direct use is limited to a GPU
environment or rectangle type.

The resident block at `0x800F56F0` now has field-level evidence matching the
32-byte `GsRVIEW2` record: viewpoint and reference-point triplets, roll, and a
parent-coordinate pointer. Game code initializes it before `GsSetRefView2`
and derives a distance plus two 4096-unit angles from the two points. Matching
sources still use local views until a shared-type migration is proven
byte-identical.

The imported `libgs.h` includes only `src/types.h` even though it refers to
`MATRIX`, `VECTOR`, `SVECTOR`, `CVECTOR`, `RECT`, `DRAWENV`, `DISPENV`, and
`PACKET`. A translation unit using it must therefore expose `libgte.h` and
`libgpu.h` first, rather than treating `libgs.h` as a self-contained umbrella.
`libhmd.h` extends that stack again: its `GsCOORDUNIT`, `GsUNIT`, and
`GsARGUNIT*` records describe hierarchical-model primitive processing and
depend on GTE, GPU, and `libgs` declarations without including those headers.

Current game C includes `libgpu.h` in `func_800249E0.c`, where `RECT` backs
two image transfers, and `file_cd_helpers.c`, where `DISPENV` receives the
current display environment and its leading `disp` rectangle is passed to
`MoveImage2`. No current game C includes `libgs.h` or `libhmd.h`, so a local
render or model record should not be migrated to one of their types from a
matching size or similar role alone; field-level and resident-call evidence
are still required.

The tracked `libgpu.h` declares both `LoadImage` and `LoadImage2` with the
same `RECT *` / `u32 *` argument shape. The `LoadImage` migration is complete
in `func_800249E0.c`; current `LoadImage2` callers retain local declarations
and rectangle-compatible views, so applying the shared prototype to them still
requires an exact code-generation check.

The GTE headers are a layered toolchain interface rather than interchangeable
umbrellas. `libgte.h` owns the geometry records and callable library
prototypes. Its `ASSEMBLER` branch also provides a small set of FIFO, matrix,
and register-transfer macros, but the header does not include any of the
standalone inline files. C that emits GTE instructions directly must include
`libgte.h` for types and then select one low-level macro implementation:

- `inline_c.h` lets GCC choose operand registers and supplies both the normal
  command macros, which emit two leading `nop` instructions, and `_b` variants
  that emit the command word without those stalls.
- `inline_o.h` instead moves operands through fixed registers `$12`-`$15` and
  declares those registers plus memory as clobbered. Its command macros include
  the two leading `nop` instructions and it has no `_b` command family.
- `gtemac.h` is a higher-level composition layer. Macros such as
  `gte_RotTransPers` call the selected low-level `gte_ld*`, command, and
  `gte_st*` macros; it provides no types, includes, or include guard itself.

These three macro headers also have no include guards and overlap in the
`gte_*` namespace, so including both low-level implementations is a
redefinition rather than a harmless compatibility choice. Selecting
`inline_c.h` versus `inline_o.h`, or a padded command versus its `_b` form, can
change register allocation and the emitted instruction schedule and therefore
requires an exact-match check. The current game sources use `libgte.h` in
`func_800249E0.c`, `func_80041E7C.c`, and `func_80041F90.c`; the latter two
also include `inline_c.h` for `gte_stopz`. No current game C includes
`inline_o.h` or `gtemac.h`.

The remaining files target assembly sources. `inline_s.h` and `gtereg_s.h`
use C-preprocessor definitions; `inline_s.h` explicitly identifies `aspsx` as
its target and emits command words with `.word`. `inline_a.h`, `gtereg.h`, and
`gtenom.h` use the alternate `macro`/`endm`, `equs`, and `dw` syntax. The two
families encode the same coprocessor commands and register roles in different
source dialects; neither belongs in a C translation unit.

`libsnd.h` and `libspu.h` expose different sound layers. `libsnd.h` is the
high-level `Ss*` sequencer and VAB interface: it owns `VabHdr`, `ProgAtr`,
`VagAtr`, sequence/SEP playback, tick modes, bank transfer, and utility voice
allocation. `libspu.h` is the lower-level `Spu*` interface for the 24 hardware
voices, SPU RAM transfer, key state, IRQs, reverb, common mixer attributes,
the SPU allocator, and the `SpuSt*` streaming state machine.

The headers deliberately keep parallel API types and constants.
`SndVolume` and `SpuVolume` are both two-short stereo records, and the
`SS_REV_TYPE_*` and `SPU_REV_MODE_*` values describe the same named effect
families, but they belong to different call surfaces. Matching layout or
numeric values alone is not sufficient reason to substitute one family for
the other.

No current game C includes either sound header. The existing `SpuRead` and
`SpuGetVoiceEnvelope` identifications in sound-driver comments describe the
resident callees reached by the matching code; they do not yet establish that
the surrounding game-owned state is an SDK `Spu*` or `Ss*` structure.

`libpress.h` is the codec side of the media stack, not the disc-stream
controller. It defines the `DECDCTENV` quantization/IDCT record, the
`DECDCTTAB` VLC table, `DecDCT*` reset/decode/input/output interfaces, and the
separate `EncSPU*` PCM-to-SPU-waveform encoders. The `StSetRing`,
`StSetStream`, `StGetNext`, and `StFreeRing` functions are instead declared by
both `libcd.h` and `libds.h`; the `SpuSt*` family in `libspu.h` is a third,
SPU-audio streaming interface.

The resident movie setup path combines these layers: `func_8005B8A0` reaches
the CD `St*` ring/stream calls and `DecDCTvlcBuild`. That call chain is evidence
for cooperating APIs, not evidence that their similarly named stream
interfaces are interchangeable. No current game C directly includes
`libpress.h`.

`libsn.h` is a development-host interface, not a retail storage API.
`PCinit`, `PCopen`, `PCcreat`, `PClseek`, `PCread`, `PCwrite`, and `PCclose`
communicate with the Psy-Q host file server, while `pollhost` and `PSYQpause`
emit debugger break instructions `1024` and `1031`. These declarations are
not interchangeable with `libcd`, `libds`, or memory-card calls. No current
game C includes `libsn.h`, so its presence in the imported SDK set does not
establish a resident dependency on the development host.

`fs.h` describes the implementation-facing filesystem switch rather than an
application file API. Its `device_table`, `device_buf`, and `iob` records carry
device callbacks, buffering state, transfer counts, and file offsets.
`romio.h` adds no independent records or functions; it only includes the
system file declarations used by the ROM-monitor environment. Neither header
is included by current game C. In particular, these records must not replace
`CdlFILE`, `DslFILE`, or `DIRENTRY` merely because all of them participate in
file operations.

The nested `sys/` headers are another compatibility stack. `sys/types.h`
defines target ABI names such as 32-bit `size_t`, `time_t`, and `off_t`,
16-bit `dev_t`, `uid_t`, and `gid_t`, plus the `major`, `minor`, and `makedev`
macros. `sys/fcntl.h` contains only the internal `FREAD`, `FWRITE`, creation,
buffering, and asynchronous-I/O flags. `sys/file.h` includes that header and
maps the public `O_*` names onto those flags while defining `SEEK_SET`,
`SEEK_CUR`, and `SEEK_END`; it declares no file functions itself. `sys/errno.h`
fixes the runtime's error-number ABI from `EPERM` 1 through `EALREADY` 37 and
declares the shared `errno` integer. `sys/ioctl.h` similarly contains request
numbers rather than an `ioctl` prototype: `FIOC*` controls file input,
`TIOC*` controls terminal/SIO behavior, and `DIOFORMAT` requests disk format.
`romio.h` is only an include wrapper around `sys/file.h`, while `libsnd.h`
uses `sys/types.h` for its target scalar types. No current game C includes the
`sys/` headers directly.

`stdlib.h` is an umbrella over the imported `abs.h`, `convert.h`, `malloc.h`,
`qsort.h`, and `rand.h` declarations, with `bsearch` and `exit` added directly.
`libmath.h` is separate: it exposes the double-precision transcendental
functions, `math_errno`/`math_err_point`, and the `printf2`/`sprintf2`
variants. Neither family is interchangeable with the fixed-point vector and
matrix operations in `libgte.h`. No current game C includes these runtime
headers, so existing hand-written declarations should only be migrated after
their exact ABI and compiler behavior are checked.

`ctype.h` classifies an unsigned-byte index through the external `_ctype_`
table. Its `is*` macros return the matching bit mask rather than a normalized
Boolean, while `toupper` and `tolower` are callable functions and the
underscore-prefixed forms are unchecked arithmetic macros. `limits.h`
records the compiler target's eight-bit characters, 16-bit shorts, and
32-bit `int` and `long` ranges; these implementation limits describe scalar
ABI widths, not the signedness or ownership of an unknown game field. No
current game C includes either header, so adopting a classification macro or
limit still requires exact code-generation evidence.

`stddef.h` defines `size_t` as `unsigned int` and `wchar_t` as
`unsigned long`; both are 32-bit types under this target's `limits.h`. It also
defines `NULL` as integer zero and `WEOF` as `0xFFFFFFFF`. `stdarg.h` uses a
`void *` `va_list` and advances it through arguments rounded up to
`sizeof(int)`, encoding the old compiler's stack and alignment assumptions.
These are target/compiler support declarations, not portable host-build
substitutes. No current game C includes either header directly, although
`stdlib.h` includes `stddef.h`.

`setjmp.h` defines `jmp_buf` as twelve 32-bit words for the saved PC, stack
pointer, frame pointer, registers `s0`-`s7`, and global pointer. It is the
single-task form and carries no signal mask or host-thread context.
`assert.h` expands a failed assertion to a formatted `printf` followed by
`exit(1)`; when `NDEBUG` is defined, both `assert` and the underlying
`_assert` macro expand to nothing. Neither header includes the declarations
for those output/termination functions itself, and no current game C includes
either header.

`stdio.h` is not a complete hosted C stream interface. It defines
`BUFSIZ`, `EOF`, the three seek-origin constants, and a local `size_t`, then
declares only `printf`, `sprintf`, and basic character/string input and output.
There is no `FILE` type or `fopen`/`fread` family, and the `getc`/`putc`
signatures use integer handles rather than stream pointers. It must not be
substituted for the debugger-host file service in `libsn.h` or the retail
disc and memory-card APIs. No current game C includes this header.

`malloc.h` exposes three parallel allocator families:
`InitHeap`/`malloc`/`calloc`/`realloc`/`free`, then identically shaped `*2`
and `*3` variants. Each initializer receives an explicit memory base and
length, so allocations and releases must stay within the same numbered
family. These heaps are separate from game-owned fixed-slot allocators such
as the display-object pool. No current game C includes `malloc.h` directly;
it is also part of the `stdlib.h` umbrella.

The other `stdlib.h` component headers remain deliberately small. `abs.h`
provides both `abs(int)` and an `ABS` macro whose argument can be evaluated
more than once. `convert.h` declares decimal and base-selectable integer
parsers plus `labs`. `qsort.h` retains the original `int (*)()` comparator
prototype; changing a matching caller to a modern fully prototyped callback
can change argument setup. `rand.h` fixes `RAND_MAX` at 32767 and exposes the
`rand`/`srand` pair whose resident implementation is documented separately in
[`rng.md`](rng.md). Current game C uses local declarations rather than
including these component headers.

The imported string headers form a compatibility stack rather than three
independent libraries. `string.h` only includes `strings.h`; `strings.h`
declares the string routines and includes `memory.h`; `memory.h` declares the
memory routines and BSD-compatible aliases. Several prototypes intentionally
omit parameter types to avoid conflicts with old compiler built-ins. No
current game C includes this stack, and replacing an exact hand-written copy
loop with `memcpy` or `bcopy` still requires a full executable match because
GCC may choose different load/store sequences.

Do not add `src/types.h` to an imported header solely for uniformity. Headers
that expose project-adapted fixed-width records must include it directly and
use the project aliases; self-contained prototype-only headers may retain the
SDK's native scalar spelling.

`libpad.h` provides `PadInitMtap`, `PadInitGun`, and the shared controller
communication state APIs. The separately imported `libtap.h` and `libgun.h`
provide older `InitTAP` and `InitGUN` lifecycle families with different
buffers and control calls. Device role alone is therefore insufficient to
substitute one family for another; use the header that matches the resident
call signature.

`libetc.h` is a separate convenience layer. `PadRead` returns both simple-pad
states in one 32-bit value, and `_PAD(port, button)` shifts a 16-bit button
mask into the selected controller half. The same header owns `VSync`, callback
reset/restart controls, NTSC/PAL selection, and the `getScratchAddr` macro for
word-indexed access to the `0x1F800000` scratchpad. Its `PadInit`/`PadStop`
pair is not interchangeable with the buffer-oriented `PadInitDirect` and
`PadStopCom` interface in `libpad.h`. No current game C includes `libetc.h`.

`libsio.h` and `libcomb.h` expose parallel serial interfaces. The SIO header
defines the controller's status/mode bits, `AddSIO`/`DelSIO`,
`_sio_control`, and `Sio1Callback`. The COMB header uses a distinct
`_comb_control` entry point for packet sizing, buffered transfer counts,
asynchronous requests, RTS/CTS, and VBlank signaling. Their overlapping
status-bit names do not establish interchangeable call contracts. No current
game C includes either header.

`libapi.h` and `libmcrd.h` retain the SDK `kernel.h` dependency for BIOS
records such as `DIRENTRY`. Both include the local `r3000.h` and `asm.h`
first, establishing the kernel guards without relying on system include paths
that the matching compiler does not provide.

These three memory-card-adjacent headers are not interchangeable.
`libmcrd.h` provides file and block operations, `libmcx.h` exposes the
separate `Mcx` device command family, and `mcgui.h` combines card metadata
with background, controller, sound, texture, and cursor resources for
`McGuiSave`/`McGuiLoad`. No current game C includes `libmcx.h` or `mcgui.h`,
so these rows document the imported header boundaries rather than claiming a
resident function identity.

The real `src/psyq/libds.h` and `src/psyq/libcd.h` provide parallel record
families. The resident file-search anchor is `DsSearchFile`, while the
position conversion used by `File_GetPosition` is the address-qualified
CD-library copy `CdPosToInt_8007E710`:

| Record | Verified ABI surface |
|---|---|
| `DslLOC` / `CdlLOC` | Layout-compatible four-byte CD locations. The resident MSF-to-LBA routine reads the BCD minute, second, and sector bytes at offsets `0`-`2`. |
| `DslFILE` | 24-byte Ds search result with `DslLOC` at `0`, size at `4`, and a 16-byte name at `8`. `DsSearchFile` copies records at a `0x18` stride. |
| `CdlFILE` | Parallel 24-byte CD-library search result. Its leading `CdlLOC` layout lets `File_GetPosition` pass `DslFILE.pos` to the resident `CdPosToInt_8007E710` copy through an explicit compatible view. |

The conversion routine `CdPosToInt_8007E710` independently verifies the
shared `DslLOC`/`CdlLOC` field order. It converts each of the first three
bytes from packed BCD, then computes:

```text
logical_sector = (minute * 60 + second) * 75 + sector - 150
```

`File_GetPosition` passes `DslFILE.pos` through the explicit
layout-compatible `CdlLOC` view and stores the result as the file position.
The fourth `track` byte is not read by this conversion and should not be
mistaken for part of the sector calculation.

The adjacent inverse routine `CdIntToPos_8007E600` takes a logical sector
number and a destination `CdlLOC *`. After adding the 150-sector lead-in, it
computes:

```text
minute = adjusted_sector / (60 * 75)
second = (adjusted_sector / 75) % 60
sector = adjusted_sector % 75
```

It converts each result to packed BCD and writes offsets `0`, `1`, and `2`.
The destination pointer is also returned. Offset `3` remains untouched, so a
caller that needs a defined `track` value must initialize it separately.
Together, the two resident routines verify both conversion directions without
requiring a copied SDK structure definition.

### Memory-card directory evidence

`func_80044470` formats a `bu%02X:%s` device path, passes it and a caller-owned
record to `0x80073AC0`, then advances subsequent records through `0x80073920`.
Both resident functions return the supplied record pointer on success, matching
the `firstfile` and `nextfile` interfaces. The caller allows five retries after
the initial attempt before stopping enumeration.

Three matching-C consumers independently establish the directory geometry:

| Property | Local evidence |
|---|---|
| Directory record size | `func_80044470` and `func_80044598` advance records by 40 bytes. |
| File-size field | `func_80044544` reads a 32-bit size at record offset `24`. |
| Allocation block size | `func_80044544` rounds each file size up to `8192` bytes. |
| Card capacity | Enumeration and free-space accounting both stop at `15` blocks. |

These values are centralized as `MEM_CARD_DIRECTORY_ENTRY_SIZE`,
`MEM_CARD_BLOCK_SIZE`, and `MEM_CARD_BLOCK_COUNT` in `src/game/mem_card.h`.
This proves the directory-record ABI surface used by the game.

The routine at `0x8007E7F0` matches the blocking `CdControlB` interface rather
than the asynchronous `CdControl` variant. It truncates the command argument
to one byte, submits the command and parameter pointer, then repeatedly polls
the command handle while passing through the caller's result pointer. It
returns one only when the internal completion code is `2`, and zero when
command submission fails. Game callers corroborate the command contract:
`func_8005C62C` issues command `0x02` (`CdlSetloc`) followed by `0x16`
(`CdlSeekP`), while another caller loops on command `0x09` (`CdlPause`).

The no-argument wrapper at `0x8007E350` calls `0x8007BE00`, which clears the
resident CD command-active flag and two associated state words before
calling the lower-level hook with both arguments zero. This
reset-without-reinitialization contract matches `CdFlush`, whose purpose is to
discard the current command state while leaving the CD library available for
later operations. The wrapper itself adds no arguments or result
transformation. Matching-C caller `func_80043960` declares the wrapper as
`void func_8007E350(void)` and invokes it immediately before transferring
control to an overlay and polling that module. The overlay continues using
the drive afterward, independently confirming a flush rather than full
CD-library teardown.

The sector-transfer wrappers at `0x8007E3D0` and `0x8007E4F0` preserve the
same destination-pointer and word-count arguments, call distinct low-level
transfer routines, and convert a zero low-level result into return value one.
This mirrored contract and the documented adjacent `CdGetSector` /
`CdGetSector2` interface pair identify `0x8007E4F0` as `CdGetSector2`.
The resident streaming path at `0x8007CF9C` calls `0x8007E4F0`, while its
alternate branch at `0x8007CFEC` passes the same saved arguments to
`0x8007E3D0`; the second entry is therefore live code rather than an unused
library variant.

### Rectangle layout evidence

`func_800249E0` builds two consecutive eight-byte records in `D_80177EA4`.
Each record receives signed halfword stores at offsets `0`, `2`, `4`, and `6`,
then is passed to the resident `LoadImage` function at `0x8007F978`:

| Record | `+0` | `+2` | `+4` | `+6` |
|---|---|---|---|---|
| card image | x coordinate | y coordinate | width `0x14` | height `0x20` |
| palette row | x coordinate `0x380` | y coordinate | width `0x40` | height `1` |

This verifies the Psy-Q `RECT` ABI surface as four signed 16-bit fields in
`x`, `y`, `w`, `h` order and a total size of eight bytes. The declaration
lives in the real `src/psyq/libgpu.h`; `func_800249E0` uses it for both GPU
transfer rectangles while retaining the original byte-offset arithmetic that
selects each record.

Before replacing a local definition:

1. Identify the resident callee and verify its argument and return contract.
2. Compare every local field access with the documented SDK offsets and widths.
3. Check all other definitions with the same size or role; do not migrate a
   single convenient caller while leaving conflicting layouts unexplained.
4. Use the declaration from the matching real Psy-Q header.
5. Rebuild the complete executable. If the shared type changes code generation,
   retain the exact local view and document the exception.

## Initial migration candidates

The existing C sources expose several useful starting points:

| Current source pattern | SDK target | Required proof |
|---|---|---|
| Local `InitPAD` / `StartPAD` declarations | `libapi.h` | Initial migration complete in `src/game/input_init_pads.c`; the real prototypes preserve the exact build. |
| `DslFILE` in `src/psyq/libds.h` | Ds file-search result | Initial migration complete in `src/game/file_stream.c`; extend only when another caller's field use agrees with the shared layout. |
| `RECT` in `src/psyq/libgpu.h` | GPU transfer rectangle | Initial migration complete in `func_800249E0`; preserve byte-offset selection when extending it to other callers. |
| Local draw/display environment buffers | `DRAWENV` and `DISPENV` | Initial `DISPENV` migration complete in `file_cd_helpers.c`; other buffers still require complete size, alignment, and field-use evidence. |
| Local vector and matrix records | `SVECTOR`, `VECTOR`, `MATRIX` | Separate fixed-point SDK layouts from game-specific render records. |
| Memory-card event descriptor arrays | event handles and card constants | Name the resident BIOS wrappers before centralizing prototypes and constants. |

These migrations are game-source refactors and must remain byte-identical.
Canonical SDK spelling improves call semantics, but exact code generation takes
priority over replacing every local layout.

## Review checklist

- The address belongs to `psyq/crt` or `psyq/sdk` in
  `config/slus_01411/functions.csv`.
- The proposed name follows Sony's API spelling rather than the game's semantic
  naming convention.
- Duplicate linked copies are disambiguated by address.
- Evidence and conflicts are recorded before changing tracked symbols.
- The interface comes from the tracked Psy-Q 4.6 headers.
- Imported declarations use fixed-width types and preserve the observed ABI.
- `make match` still reproduces the retail executable exactly.
