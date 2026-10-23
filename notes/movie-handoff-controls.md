# Movie display and SDK handoff state

## Ownership is part of the contract

| Original NA identity | Canonical view | Existing owner and view |
|---|---|---|
| `D_8009B061` | `gMovie_bCopyDisplayOnStop` | Movie-owned initialized `u8`, plain caller declaration |
| `D_800FE0CC` | `gLibGs_sActiveBufferIndex` | LIBGS-owned `s16`, `.data` caller view |
| `D_800F5D44` | `gLibCd_nDeferredStreamInterrupt` | LIBCD-owned `s32`, `.data` caller view |

The library prefixes describe SDK-owned data, not new game-owned storage
or claims of official Psy-Q symbol spellings. No SDK function is renamed,
reimplemented or entered in an attempt ledger. The existing SDK assembly
and all SDK headers remain unchanged.

## Copy the current display on the existing stop path

`Movie_OpenStream` stores its sixth argument into the copy-control byte.
`Movie_StopStream` calls `Movie_MoveDisplayImage(0, 0x100)` when that byte
or its own argument is nonzero. The helper reads the current `DISPENV`,
waits for GPU idle and retries the existing `MoveImage2` call using the
display rectangle.

This is a copy-current-display-on-stop control, not a guarantee of
restoring a pre-playback screenshot. The raw byte assignment keeps the
existing truncation of the signed argument; neither it nor the separate
stop argument is normalized to a Boolean. Call order, waits, volume,
ring teardown and return behavior remain unchanged.

NA stores this byte at offset one of the existing twenty-byte `.sdata`
movie state block. All eleven objects, explicit zero initializers,
section attributes, widths and definition order remain. The supported
compiler/MASPSX object does not report ELF sizes for these initialized
symbols: the widths come from the unchanged `u8`/`u32` definitions,
actual offsets and complete twenty-byte section, not invented metadata.

## LIBGS active-buffer state, not a reset latch

The accepted NA `GsGetActiveBuff` implementation returns a signed
halfword load from the original index address. `GsInitGraph2` clears it;
`GsSortClear`, `GsSetDrawBuffClip` and `GsSetDrawBuffOffset` read it;
`GsSwapDispBuff` updates it and reinstalls the environments. This agrees
with the established SDK identities in [psyq.md](psyq.md).

Movie open/stop retain their writes of one before the existing swap
and graph-setup sequence. The new name does not widen the halfword,
turn it into a Boolean reset flag or change any SDK behavior. The
`GraphicsDimension` union, width/height globals, RGB24 paths and all
clearing rectangles remain unchanged.

The two-argument call view of `GsGetActiveBuff` in `Movie_OpenStream`
also remains exactly as written; naming data is not permission to
remove its deliberately retained argument setup.

## LIBCD deferred stream interrupt handoff

Local SDK inventory identifies `StClearRing` and `StCdInterrupt` through
Psy-Q 4.6 `LIBCD.LIB/C_002.OBJ` and `C_011.OBJ`; `libcd.h` declares
these APIs. This state belongs to LIBCD, not LIBPRESS.

The accepted NA ring-clear routine clears the word. The stream
interrupt handler writes one in its existing early deferred branch
after testing its SDK gate and hardware-status bit. In RGB24 mode,
`Movie_OnDecodeOutput` tests the same word, calls `StCdInterrupt` and
then clears it.

The signed word and absolute `.data` addressing remain unchanged.
This is not an interrupt count, callback pointer, new concurrency
protocol or proof that service has completed. In particular, the
original call-before-clear order is retained even if the SDK call
could set the state again; no retry, race or barrier repair is made.

## Native providers and declaration boundaries

The narrow canonical header follows the original movie header in the
two caller roots and the initialized data owner. The legacy header's
declarations, prototypes and dimension types stay unchanged; only its
old reset-latch prose is corrected. All regional wrappers, compiler
profiles, source groups, data-unit registration and existing tests
remain unchanged.

Native imports retain the original Japanese and PAL providers,
including PAL's `gEuropean_D_8009B061`, `gEuropean_D_800FE0CC` and
`D_800F6CD4` assembler identities. The NA initialized byte keeps its
C-defined canonical primary and receives `legacy = ABSOLUTE(canonical)`; the
other regional byte providers and both SDK words keep
`canonical = original`. Automatic SDK providers use conditional
`PROVIDE`, retaining production resolution without adding unused
symbol-expression dependencies to unrelated import links. No numeric
address is guessed or pinned.

The accepted NA legacy name has absolute ELF class `A`, even though its
real initialized data-object definition has class `D`. `ABSOLUTE`
preserves that legacy class/address while the canonical primary retains
its real C-owned `D` definition. It assigns no address over the primary
and changes no section, storage, compiler flag or data byte.

Actual compiler/CPP evidence covers seven contexts: six caller
contexts and the explicit NA `data_c.json` owner, with forty-two
function registrations and one data-unit registration. Twenty-one
declarations retain their types and attributes: fifteen imports,
five unused views and one initialized C definition. No bound,
qualifier, struct layout, new storage, harness, fixture, probe or
testcase is introduced.
