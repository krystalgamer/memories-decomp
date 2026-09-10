# Options screen contract

## Scope and evidence

This note describes the North American Options screen from exact-matching
resident C, a complete instruction scan of `game/SLUS_014.11`, the verified
WA archive, and the existing live stereo/mono observations. It does not assign
labels or visible effects to rows that a human run has not reached.

The relevant resident path is:

```text
Main_RunOptionsMenu
  -> func_8003C2B4
       -> WA sectors 0x2115..0x2146 through func_8003C120
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

`func_8003C2B4` requests 50 WA sectors beginning at `0x2115`.
`func_8003C120` accounts for all four phases:

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
