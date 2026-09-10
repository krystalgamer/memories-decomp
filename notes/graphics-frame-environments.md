# Graphics frame environment contracts

`graphics_frame.h` owns the game-facing declarations for the SDK drawing and
display environments and the three background-tint bytes. These are declarations
of existing addresses, not C data ownership or new SDK symbol names.

## Address and layout evidence

The resident `GsSetDrawBuffClip` body at `0x80085440` loads `D_800FE048` into
`a0` at `0x8008547C`/`0x80085480`, writes the clip rectangle at offsets
`0, 2, 4, 6`, and passes that same address to `PutDrawEnv` at `0x800854A0`.
The existing `src/psyq/libgpu.h` declaration takes `DRAWENV *`. This establishes
the SDK type independently of the game's field writes.

`GsSwapDispBuff` at `0x80085500` loads `D_800FE0A8` at
`0x80085520`/`0x80085524`, updates its first two halfwords and passes it to
`PutDispEnv` at `0x80085538`. The SDK signature takes `DISPENV *`.
Startup (`func_80013154`) already copies this value into the typed
`gGraphics_DispEnv`, later passed to `PutDispEnv` by `Graphics_BeginFrame`.

| DRAWENV offset | Existing SDK member | Game producer |
| --- | --- | --- |
| `0x16` | `dtd` | `D_8009B0AD` in `Graphics_BeginFrame`; 1 at startup |
| `0x18` | `isbg` | `D_8009B0D0` in `Graphics_BeginFrame`; 1 at startup |
| `0x19` | `r0` | `D_8009B144`; 1 at startup |
| `0x1A` | `g0` | `D_8009B143`; 1 at startup |
| `0x1B` | `b0` | `D_8009B142`; 1 at startup |

The existing movie stop path (`func_8005BB7C`) independently confirms the
red/green/blue order through `ClearImage`. It now takes its existing absolute
tint declarations from `graphics_frame.h` instead of repeating them privately.
Five offset assertions and the SDK `DRAWENV`/`DISPENV` size assertions protect
the views. No inferred aggregate joins the two separately relocated symbols;
the four bytes after the `0x5C` drawing environment are not claimed.

## Code generation and integration

`D_800FE048` remains an unsized extern array, with only element zero accessed.
Startup retains its volatile view; per-frame updates retain their nonvolatile
view. The five byte stores in each function keep their original source order.
`D_800FE0A8` remains a nonvolatile scalar `DISPENV`. The tint bytes retain
startup's volatile `.data`, the movie/frame paths' nonvolatile `.data`, and
all existing scalar/array and tentative-definition forms in other consumers.
No linker identities, storage definitions, compiler profiles, SDK bodies,
register constraints or grouped translation-unit boundaries change.

The resident candidates currently contain no direct environment/tint consumer;
all 19 candidate fingerprints must still pass because this shared header has
transitive users. Pending source-move PR #3904 must carry the startup's two
selector macros and five typed stores to `src/candidates/func_80013154.c`,
with `graphics_frame.h` visible before use. The shortened
`movie_frame_pipeline.c` still owns `func_8005BB7C` and must retain
`D_8009B142_IN_DATA` before the first header inclusion and use the shared
tint declarations. Do not restore the old private byte-array
declaration or erase the volatile startup view while moving these bodies.
