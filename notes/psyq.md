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
| `0x8007A860`, `0x8007E8A0` | `CdDataCallback` copies | Byte-identical wrappers that install a callback on DMA channel `3`. |
| `0x8007D3F0` | `DsSearchFile` | Receives a 24-byte file record and a path, then supplies disc-position data. |
| `0x8007E3D0` | `CdGetSector` | Identified CD-sector transfer interface in the resident CD library. |
| `0x8007E600` | `CdIntToPos` | Adds the two-second lead-in and writes packed-BCD minute, second, and sector fields to a `CdlLOC`. |
| `0x8007E710` | `CdPosToInt` | Decodes the first three BCD bytes of a `CdlLOC` and returns a zero-based logical sector number. |
| `0x8007E7F0` | `CdControlB` | Submits the three-argument CD command and blocks until the internal completion code is `2`. |
| `0x8007E860` | `CdReadyCallback` | Replaces and returns the callback invoked with a ready-event status and result pointer. |
| `0x8007E880` | `CdSyncCallback` | Replaces and returns the callback invoked from the command-completion path. |
| `0x8007F350` | `ResetGraph` | Anchored by GPU `sys.c` evidence and the documented graph-reset contract. |
| `0x8007F978` | `LoadImage` | GPU transfer call sites pass rectangle-like coordinates and source data. |
| `0x8007FAF0` | `ClearOTag` | Ordering-table initialization behavior. |
| `0x8007FC64` | `DrawPrim` | Direct GPU primitive submission behavior. |
| `0x8007FCC0` | `DrawOTag` | Ordering-table submission behavior. |
| `0x8007FD30` | `PutDrawEnv` | Draw-environment submission contract. |
| `0x8007FEFC` | `PutDispEnv` | Display-environment submission contract. |
| `0x800803F4` | `GetDispEnv` | Writes the current display environment to a caller-owned record. |

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

Add clean-room declarations under `src/psyq/` by library family when a game C
caller is ready to use them. Do not copy proprietary Psy-Q headers or manual
text into the repository. Reconstruct only the ABI surface supported by the
executable and public interface documentation.

Recommended header boundaries are:

| Header | Interfaces and records |
|---|---|
| `src/psyq/libapi.h` | Events, critical sections, counters, heap, and BIOS wrappers |
| `src/psyq/libcd.h` | CD commands, locations, file records, callbacks, and sector transfers |
| `src/psyq/libgpu.h` | Rectangles, draw/display environments, images, primitives, and ordering tables |
| `src/psyq/libgte.h` | Vectors, matrices, and GTE helper interfaces |
| `src/psyq/libgs.h` | `Gs` work areas, objects, lights, cameras, and sorting helpers |
| `src/psyq/libspu.h` | SPU voices, transfer state, reverb, and callbacks |
| `src/psyq/libcard.h` | Card events, status, directory records, and I/O |
| `src/psyq/libmdec.h` | MDEC environment, callbacks, input, and output |

Every shared SDK header must include `src/types.h` and use the project's
fixed-width aliases. Keep incomplete records opaque. A fielded structure is
appropriate only when offsets and widths are established by callers, target
instructions, or an exact documented ABI.

The first clean-room declaration is `src/psyq/libcd.h`:

| Record | Verified ABI surface |
|---|---|
| `CdlLOC` | Four-byte CD location. The resident MSF-to-LBA routine reads the BCD minute, second, and sector bytes at offsets `0`-`2`. |
| `CdlFILE` | 24-byte search result with `CdlLOC` at `0`, size at `4`, and a 16-byte name at `8`. `DsSearchFile` copies records at a `0x18` stride, and `File_GetPosition` passes the leading location to the conversion routine. |

The conversion routine at `0x8007E710` independently verifies the `CdlLOC`
field order. It converts each of the first three bytes from packed BCD, then
computes:

```text
logical_sector = (minute * 60 + second) * 75 + sector - 150
```

`File_GetPosition` passes `CdlFILE.pos` directly to this routine and stores
the result as the file position. The fourth `track` byte is not read by this
conversion and should not be mistaken for part of the sector calculation.

The adjacent inverse routine at `0x8007E600` takes a logical sector number and
a destination `CdlLOC *`. After adding the 150-sector lead-in, it computes:

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

The routine at `0x8007E7F0` matches the blocking `CdControlB` interface rather
than the asynchronous `CdControl` variant. It truncates the command argument
to one byte, submits the command and parameter pointer, then repeatedly polls
the command handle while passing through the caller's result pointer. It
returns one only when the internal completion code is `2`, and zero when
command submission fails. Game callers corroborate the command contract:
`func_8005C62C` issues command `0x02` (`CdlSetloc`) followed by `0x16`
(`CdlSeekP`), while another caller loops on command `0x09` (`CdlPause`).

### Rectangle layout evidence

`func_800249E0` builds two consecutive eight-byte records in `D_80177EA4`.
Each record receives signed halfword stores at offsets `0`, `2`, `4`, and `6`,
then is passed to the resident function at `0x8007F978`, the `LoadImage`
candidate:

| Record | `+0` | `+2` | `+4` | `+6` |
|---|---|---|---|---|
| card image | x coordinate | y coordinate | width `0x14` | height `0x20` |
| palette row | x coordinate `0x380` | y coordinate | width `0x40` | height `1` |

This verifies the Psy-Q `RECT` ABI surface as four signed 16-bit fields in
`x`, `y`, `w`, `h` order and a total size of eight bytes. The clean-room
declaration now lives in `src/psyq/libgpu.h`; `func_800249E0` uses it for both
GPU transfer rectangles while retaining the original byte-offset arithmetic
that selects each record.

Before replacing a local definition:

1. Identify the resident callee and verify its argument and return contract.
2. Compare every local field access with the documented SDK offsets and widths.
3. Check all other definitions with the same size or role; do not migrate a
   single convenient caller while leaving conflicting layouts unexplained.
4. Introduce the smallest shared declaration that covers the proven surface.
5. Rebuild the complete executable. If the shared type changes code generation,
   retain the exact local view and document the exception.

## Initial migration candidates

The existing C sources expose several useful starting points:

| Current source pattern | SDK target | Required proof |
|---|---|---|
| `CdlFILE` in `src/psyq/libcd.h` | CD file-search result | Initial migration complete in `src/game/file_stream.c`; extend only when another caller's field use agrees with the shared layout. |
| `RECT` in `src/psyq/libgpu.h` | GPU transfer rectangle | Initial migration complete in `func_800249E0`; preserve byte-offset selection when extending it to other callers. |
| Local draw/display environment buffers | `DRAWENV` and `DISPENV` | Confirm complete size, alignment, and all fields touched by resident GPU functions. |
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
- No proprietary SDK binary, library, or header is committed.
- Shared declarations use fixed-width types and preserve the observed ABI.
- `make match` still reproduces the retail executable exactly.
