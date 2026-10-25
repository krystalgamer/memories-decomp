# Options screen contract

## Scope and evidence

This note describes the North American Options screen from exact-matching
resident C, a complete instruction scan of `game/SLUS_014.11`, the verified
WA archive, and the existing live stereo/mono observations. It does not assign
labels or visible effects to rows that a human run has not reached.

The relevant resident path is:

```text
Main_RunOptionsMenu
  -> File_RequestOptionsPackage
       -> WA sectors 0x2115..0x2146 through Options_LoadPackageStage
       -> WA sectors 0x2147..0x2156 directly to 0x80140000
  -> Options_Init
  -> Options_Update
       -> Options_HandleInput
```

## Reachable input behavior

`Options_Init` sets `gOptions_bSelection` to zero and
`gOptions_bState` to one. A complete scan of the retail instruction stream
finds exactly six direct accesses to the selection byte:

| Address | Access | Role |
|---:|---|---|
| `0x8003C6C0` | `sb zero, 0x47C(gp)` | initialization |
| `0x8003C770` | `lb` | initial layout |
| `0x8003C7A0` | `lb` | row-zero test |
| `0x8003C83C` | `lb` | layout after an audio change |
| `0x8003C850` | `lb` | nonzero-row test |
| `0x8003C880` | `lbu` | derive state from a nonzero row |

There is no later store, no `$gp` address formation for the byte, and no raw
pointer to `0x8009B384` in the executable. The loaded Options destinations are
high-memory asset banks and do not overlap it. Therefore the natural NTSC-U
path leaves the selection at row zero.

On row zero, `Options_HandleInput` tests only horizontal pressed input:

| Input | Condition | Effect |
|---|---|---|
| Right | working output type is `0` | writes `1` to both Options and stored sound state, calls `SD_SetOutputType(1)`, plays sound `0x2F`, updates layout |
| Left | working output type is `1` | writes `0`, calls `SD_SetOutputType(0)`, plays sound `0x2F`, updates layout |
| Same-side horizontal input | already at that endpoint | returns without another sound or layout update |
| Circle | no earlier horizontal branch consumed the tick | sets state zero and plays sound `8`; state zero waits for fade-out and returns to the previous main mode |
| Cross or Square | selection remains zero | no effect |
| Up or Down | not tested | no resident effect |

The established live polarity is `0 = stereo`, `1 = mono`. The setter updates
the sound driver's output mode immediately and rebuilds the CD-audio mix when
the value changes.

## Dormant row and state machinery

The source retains a three-position layout and branches for nonzero selection
values, but no natural writer reaches them:

- layout positions are `0x90` and `0xA0` for selection values 1 and 2, with
  the cursor placed eight pixels below those values;
- confirming selection 1 writes state 2;
- confirming selection 2 writes state 3;
- state 2 is an empty update arm and no longer processes input;
- state 3 changes back to state 1 on the next update.

This establishes vestigial or disabled machinery, not two usable NTSC-U menu
entries. In particular, forcing selection 1 would enter an idle state with no
resident exit handling, while forcing selection 2 would produce a one-update
detour. The pending human trace can establish what is visible and confirm
whether directional input is inert, but static evidence does not justify
naming the dormant rows.

## Stored setting and save behavior

`Options_Init` copies `gSD_bOutputType` into the screen's working byte. A
negative stored value is displayed as working value zero, but initialization
does not itself replace the stored negative sentinel. A successful horizontal
toggle writes the stored byte immediately.

`SaveData_BuildPayload` normalizes a negative stored value to zero and writes
the result at save-state offset `+0x5DE`. `SaveData_ApplyRuntimeState` reads
that byte and calls `SD_SetOutputType` only while the runtime stored value is
still negative. A later load therefore does not overwrite a sound choice that
has already been established in the current process.

## WA package and PocketStation payloads

`File_RequestOptionsPackage` requests 50 WA sectors beginning at `0x2115`.
`Options_LoadPackageStage` accounts for all four phases:

| WA range | Size | Callback behavior |
|---:|---:|---|
| `0x108A800-0x109A800` | `0x10000` / 32 sectors | schedules the main image phase through the alternating transfer buffers |
| `0x109A800-0x109B000` | `0x800` / 1 sector | stages palette data |
| `0x109B000-0x109B800` | `0x800` / 1 sector | uploads the staged block as a `256 x 4` rectangle at VRAM `(256, 240)`, then transfers this sector to `0x801AF000` |
| `0x109B800-0x10A3800` | `0x8000` / 16 sectors | transfers a 32 KiB payload to `0x80140000` |

The fourth phase starts with a Shift-JIS header identifying
`PocketStation   Yu-Gi-Oh! Shin Duel Monsters` (without the centered dot in
the Japanese title) and has SHA-256
`25db4a58858b66ece42f550d88a7e376312bb3b5f81ce9072580fdd935602edb`.

After waiting for that request, the loader immediately transfers the next 16
WA sectors, `0x2147..0x2156`, to the same `0x80140000` address. This second
32 KiB payload carries the corresponding title with the centered dot and has
SHA-256
`93904f6009423c425353126bf1cbf773306441a588abb9811f67cac4347bdf8c`.
The payloads differ in 28,915 bytes, and the second overwrites the first before
`Options_Init` runs.

The Options lifecycle therefore preserves PocketStation package data in the
North American executable path even though resident input cannot select a
non-audio row. That association is static and exact; the purpose of loading
both variants, and any visible labels connected to the disabled machinery,
remain unresolved.

## PAL language-indexed package addendum

The NA evidence and UI claims above are unchanged. This separate
source-backed PAL contract comes from the existing regional wrapper and
complete common stage/request bodies, not a local PAL runtime session.

`Options_LoadPackageStage` retains native `func_8003C56C`: 416 bytes at
UK `0x8003C56C`, or `0x8003C76C` in France, Germany, Italy and Spain.
Its `void(FileTransferDescriptor *, s32)` binding now lives in the
existing transfer owner header under the already-existing
`VERSION_EUROPE_FILE_REQUEST_OPTIONS_PACKAGE` selector. Only the private
name macro in the actual Options wrapper is removed. The two earlier
wrappers keep their native `func_8003C224` and `func_8003C3C4` mappings;
neither is claimed as another Options provider or given a screen name.
NA/Japan declarations and native ownership remain unchanged.

The request reads the then-current language byte and queues seven
arguments: `0, 0, language * 0x29 + 0x2797, 0x29, callback, 0, 0`.
It waits once, with no second-block request in PAL. This consumes 41
sectors across the four existing callback phases:

| Phase | Sectors | Existing PAL behavior |
|---|---:|---|
| 0 | 32 | initializes image-transfer dimensions and alternating sector buffers |
| 1 | 1 | selects the shared transfer buffer for the palette stage |
| 2 | 2 | uploads a 256-by-4 rectangle at `(640, 224)`, then selects resource bank zero |
| 3 | 6 | selects the pointer currently stored in `gFile_pScreenBankBase` |

The stage keeps all descriptor stores, transfer-state masks, done values,
common-tail ordering and repeated destination-pointer reads. The image
upload still precedes phase-two destination selection; no new upload
retry, validation or wait is added. The callback itself does not select a
language or request the whole package.

`DebugMenu_UpdateLanguageEntry` only edits the shared language byte; it
does not initiate these transfers. The existing Options-screen
language-boot request and font initialization remain separate and
unchanged. See `frontend-debug-state.md` for the editor's byte-narrowing,
build-default and confirmation/cancellation behavior.

Both providers' inventories change only two name fields per PAL release;
ten scoped semantic rows record the evidence. Complete actual native
compiler/MASPSX output, symbols and disassembly/relocations remain exact.
The fresh clean NA resident retains SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
Ordinary exact-head regional image/regression checks remain mandatory;
native-unit conservation is not local full PAL-image execution. No new
testcase, synthetic/generated-C/source-string harness or local cloud
suite is introduced.
