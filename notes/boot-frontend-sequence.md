# Boot frontend sequence and package

## Scope

Matching `Main_RunBootSequence` (`0x80043960`) is called once by `Main_Init`
after graphics, text, sound, controller, file-transfer, and random-number
initialization. Its mode-zero path loads the persistent frontend resources,
runs a three-sector boot compliance module, loads the main-menu package, and
resets the frontend runtime before returning to `Main_Init`.

The 54-sector package callback is matching
`Main_LoadBootPackageStage` (`0x80043328`). The callback name is deliberately
package-wide: the request contains shared UI graphics and palettes as well as
executable module ID `0x16`, so the earlier proposed `Boot_LoadUiBlock` name
described only part of its work.

## Complete 54-sector package

`Main_RunBootSequence` requests WA sectors `0x1690..0x16C6`, archive byte
range `0xB48000..0xB63000`. The four stage lengths sum exactly to the request:

```text
0x18000 + 0x1000 + 0x800 + 0x1800
= 0x1B000 bytes
= 54 sectors
```

| Stage | WA range | Size | Destination / action | SHA-256 |
|---:|---:|---:|---|---|
| 0 | `0xB48000-0xB60000` | `0x18000` / 48 sectors | Uploads 48 sector-sized `64 x 16` rectangles, filling VRAM `(640,0)-(831,255)` | `422291574b4541bb7f6098f2a11ee5d78188ba6209a1b65165f728b910823f00` |
| 1 | `0xB60000-0xB61000` | `0x1000` / 2 sectors | Stages then uploads a `256 x 8` palette rectangle at VRAM `(512,248)` | `5442ea26fdc7604068636df5f893e291b8acfa9a0291967dd7958567479e7547` |
| 2 | `0xB61000-0xB61800` | `0x800` / 1 sector | Uploads its first `0x100` bytes as eight `16`-colour rows at VRAM `(640,232)`; the remaining `0x700` bytes are zero | `d9ee64f5cb45bb97a45630957fd937d0db94646ad65e47f99e4f7decc0f1f7e3` |
| 3 | `0xB61800-0xB63000` | `0x1800` / 3 sectors | Loads module ID `0x16` at `0x80168000..0x80169800` | `83d49e3fde2dca5e60961ac9bcf31fd1ce918c885f6b88fcab01496691581d3a` |

For stage 0, the resident CD callback copies the stored `(640,0)` position
into the transfer rectangle, uploads one sector as `64 x 16` BGR555 words,
and advances 16 rows per sector. Every 16 sectors it returns to row zero and
moves 64 pixels right, producing the complete `192 x 256` destination.

The first three phases contain persistent frontend resources. Tutorial
offsets identify the shared card-pointer and fusion-number images inside
stage 0, their palettes inside stage 1, and seven populated text/UI colour
ramps plus one empty row in the first `0x100` bytes of stage 2. Those visual
labels are tutorial-derived; the boundaries, upload geometry, and retail
bytes are established independently by matching loader code.

## Module ID 0x16

Stage 3 begins with:

```text
0x00000016
0x80168A70
0x801681A8
0x801681C4
...
```

The leading ID and pointer table are followed by valid MIPS code. The outer
boot sequence calls `func_801680F4` and then polls `func_80168160(1)` until
it returns zero.

The module contains the exact strings:

```text
SOFTWARE TERMINATED
CONSOLE MAY HAVE BEEN MODIFIED
CALL 1-888-780-7690
```

`func_801680F4` reads byte `0xBFC7FF52` from the PlayStation BIOS and selects
initial state values for the module. `func_80168160` dispatches through a
20-entry state table and drives the synchronous boot flow. Together with the
warning text, this establishes a boot region/compliance role rather than an
ordinary screen asset tail.

The two module entries remain address-based. The first clearly reads the BIOS
region byte and initializes state, but current static evidence does not fully
separate region selection from anti-modification behavior inside the broader
module. Naming either entry only for the warning would therefore be narrower
than its proven inputs and state effects.

## Outer startup flow

The known mode-zero flow is:

1. clear the frontend phase byte;
2. load the preceding 34-sector fixed package and wait;
3. request the 54-sector boot package;
4. initialize the fade and two persistent display objects;
5. initialize the debug font and flush the CD command state;
6. call `func_801680F4` and poll `func_80168160(1)`;
7. request the SU main-menu package;
8. finish the transition and call `Main_ResetFrontendRuntime`.

`Main_Init` is the only resident caller found and always passes zero. The
function retains a nonzero-mode branch that waits for the 54-sector package,
initializes the debug font/display state, and returns without running the
module or loading the main menu. With no known caller, that branch is
documented as retained behavior rather than assigned a user-facing purpose.

`Main_RunBootSequence` describes the sole observed startup use and its
synchronous state-machine execution. It does not imply that the function is a
per-frame top-level mode runner.
