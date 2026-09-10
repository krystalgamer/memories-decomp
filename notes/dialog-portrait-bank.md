# Campaign dialog portrait bank

## Scope

The fixed 49-sector campaign scene package contains the event script and the
portrait bank used by text control F6. Matching
`Campaign_LoadScenePackageStage` routes the package's final `0xF000`-byte
phase to the shared arena at `0x80100000`, and matching
`Campaign_LoadScenePackage` uploads the usable records to VRAM before the
event script starts.

The arena pointer itself remains `D_80010000`. It is reused by unrelated
loaders and runtimes, so naming that pointer object
`gText_aDialogPortraits` would describe only one transient content state.

## Disc and record layout

The portrait phase is WA range `0xF35000..0xF44000`:

| Region | Size | Role |
|---:|---:|---|
| `0xF35000..0xF43D80` | `0xED80` | 25 complete portrait records |
| `0xF43D80..0xF44000` | `0x280` | sector-rounded spill; not consumed by the uploader |

Each complete record is:

```text
0x000..0x8FF  48 x 48 8-bit image (0x900 bytes)
0x900..0x97F  64-entry BGR555 CLUT (0x80 bytes)
```

The record size is `0x980`, and:

```text
25 * 0x980 = 0xED80
0xF000 - 0xED80 = 0x280
```

The 25-record bank SHA-256 is:

```text
3b3898fdf0d74a0ed8d86ca4faac871621747dfe85977db4f5697c18725dcccf
```

The final `0x280` bytes are nonzero, but they are not an unknown table. They
are byte-identical to the first `0x280` bytes of the next `0x980` portrait
record at WA offset `0xF63D80` in the Free Duel portrait bank. The campaign
phase ends on a sector boundary partway through that record, while its upload
loop deliberately stops after 25 records.

## RAM-to-VRAM upload

After the package transfer completes, `Campaign_LoadScenePackage` treats
`0x80100000` as a walking `u32` source and uploads exactly 25 pairs.

Portrait images use a `RECT` with:

```text
word width = 24
height     = 48
```

In 8-bit texture data, 24 VRAM words encode 48 pixels. The destination is a
five-column grid:

```text
x = 0x380 + (index % 5) * 24 words
y =          (index / 5) * 48 rows
```

This produces a 5 x 5 atlas over VRAM word X `0x380..0x3F7` and pixel rows
`0..239`.

CLUTs use `64 x 1` rectangles. Indices `0..15` occupy X `0x380`, Y
`0xF0..0xFF`; indices `16..24` occupy X `0x3C0`, Y `0xF0..0xF8`.
The source advances by exactly `0x980` bytes after each image/CLUT pair.

## F6 selection and rendering

`Text_HandleDisplayEffectCommand` handles F6 as:

```text
F6 effect_id flags
```

IDs below `0x41` use the first two shared display-effect records. IDs
`0x41..0x59` select the third record as a campaign portrait and install the
portrait lifecycle step.

The renderer derives:

```text
index = effect_id - 0x41
u     = (index % 5) * 48
v     = (index / 5) * 48
CLUT  = base + (index % 16)
page  = base + (index / 16) * 64
```

These expressions are the inverse of the uploader's atlas placement. The 25
legal F6 IDs therefore cover every uploaded portrait exactly once:

```text
0x41 + 0 = 0x41
...
0x41 + 24 = 0x59
```

## Naming conclusion

The two resident functions have package-wide roles:

- `Campaign_LoadScenePackageStage` routes the image, palette, event-script,
  and portrait phases of the fixed campaign package.
- `Campaign_LoadScenePackage` requests that package, initializes event state,
  uploads the portrait atlas, and creates the initial scene object.

The portrait constants belong in `campaign_scene_package.h`, where the loader
and renderer share them. No global portrait-bank symbol is applied to
`D_80010000`; the semantic object is the transient data at its pointed-to
arena, not the reused pointer constant itself.
