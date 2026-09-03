# Overlays

What is known about the code the game loads outside `SLUS_014.11`, measured
against the NTSC-U disc. This is the "document" half of issue #4; the "split
into a sub-directory" half is a change to the asm tree and is not in this
note.

## The shape of it

The executable is one image, 0x1D0000 bytes from `0x80010000`, and its
`.text` ends at `0x80073840` (the PsyQ library follows). The ranges below are
inside that image but **zero in the file**: they are filled at run time from
`/DATA/WA_MRG.MRG` (LBA 10102) by one request function, `func_80014E1C(mode, table, sector, count, callback, …)`, with a
per-screen callback that takes the read stream chunk by chunk and routes each
chunk to RAM or VRAM. Two RAM ranges receive **code** that way:

| slot | address | size | loaded by | when |
|---|---|---|---|---|
| duel overlay | `0x80146000` | 0x16000 (90,112 B) | `func_8001798C` → `func_800171A8` | every duel |
| screen overlay | `0x80168000` | up to 0x7800 | each menu screen's loader | per screen |

The first slot holds one image for every duel. The second is shared: each
menu screen's blob carries its own code for that address, so `0x80168000`
means different functions at different times, and a call target in that
range only resolves against the overlay resident when the caller runs.
[`config/undefined_funcs_auto.txt` in the ygofm-decomp tree lists the 24
targets the executable `jal`s into these ranges.]

## The duel overlay — `0x80146000`

`func_8001798C` issues **one read of 235 sectors at sector `0x16C6 + 235 ×
terrain`** (`D_8009B364`, 0–6): seven copies of the same blob, one per
terrain, differing only in the field picture. The callback `func_800171A8`
is a 13-case state machine (jump table `jtbl_800100C0`) whose chunk sizes, in
case order, sum to exactly 235 × 2048 — which fixes every chunk's offset:

| blob offset | size | destination |
|---|---|---|
| `+0x00000` | 0x20000 | VRAM (768, 256) |
| `+0x20000` | 0x2000 | staging, then `LoadImage2` rect (256, 240) 256×16 |
| `+0x22000` | 0x2800 | `0x8017A1D8` — equip table |
| `+0x24800` | 0x10000 | `0x8017C2D8` — fusion table |
| `+0x34800` | 0x800 | `0x801799D8` — ritual table |
| `+0x35000` | 0x1000 | staging, then `LoadImage2` rect (0, 240) 256×8 |
| `+0x36000` | 0x10000 | VRAM (x = 512) |
| **`+0x46000`** | **0x16000** | **`0x80146000` — the overlay** |
| `+0x5C000` | 0x1800 | `0x801A8000` |
| `+0x5D800` | 0x1800 | `0x801A9800` |
| `+0x5F000` | 0x4000 | VRAM (832, 0) |
| `+0x63000` | 0x2800 | `0x80100000` |
| `+0x65800` | 0x10000 | VRAM (640, 256) — the field picture, the one chunk that differs per terrain |

So the overlay image is at `WA_MRG.MRG` sector `0x16C6 + 140` = 5970 (LBA
16072) and again every 235 sectors. Unchiga's extractor, written
independently from a live capture, lands on the same LBA; the image it
produces has sha1 `77b6430db58439e9577533d4f4fa2762996cf6fc`. The three rule
tables in the same blob decode with every card id in range (25,131 fusion
recipes, 4,041 equip pairs, 24 rituals), which is the check that the layout
is right. `0x801462B0`, one of the executable's undefined call targets, is
inside this image.

## The screen overlay slot — `0x80168000`

Two of its images are located.

**Password shop.** The blob at sector `0x1EDF` (main menu, 80 sectors,
callback `func_8003BA14`) carries a 0x7800-byte chunk at `+0x20800` for
`0x80168000`; the blob at `0x1F2F` (password screen, 86 sectors, callback
`func_8003BD14`) carries the identical chunk at `+0x23800`. Six sectors of
it are code. Three GameShark patch codes verify against it instruction for
instruction: `0x8016A87C` `subu $v1, $v1, $s0` (the star-cost subtraction),
`0x8016A880` `bnez $a0, +3`, `0x8016A6E0` `beqz $v0, +5` — the "no stars",
"cards arrive immediately" and "buy again" cheats. The shop's once-only rule
is a flag per card in the save's flag array (`0x400 + card`, tested at
`0x8016A6D8` and set on purchase).

**Free Duel.** The blob at sector `0x1E88` (87 sectors, loader
`func_8003B9BC`, callback `func_8003B808`) ends with a 0x2800-byte chunk at
`+0x29000` for `0x80168000`; 8 KB of it is code, one copy on the disc. Its
entry `0x80168FB4` is one of the executable's call targets, and four of the
five function addresses Unchiga read from a live disassembly of that screen
land on `addiu $sp` prologues in the image (the fifth, `0x80168004`, is a
frameless leaf). The unlock loop is at `0x801683C0`–`0x801683EC`: it marks
40 grid entries available and, for ids 1–38, clears the ones whose save flag
`0x6E0 + id` is off. The Free Duel unlock GameShark patch (`801683D4 A0F8` /
`801683D6 0805`, turning that loop's `bnez` into a jump) verifies there.

**Not located.** The two "enable" GameShark codes guard `0x80168100` and
`0x80168188` with values that match neither image, and match nothing raw in
`WA_MRG.MRG`, `SU.MRG` or `MODEL.MRG` (searched as a relative byte pattern at
both alignments, with the same scanner finding the executable's own guards).
Either another screen's overlay is unpacked or generated at run time, or
those two codes are for a different build.

## Other menu blobs

Every other screen loads the same way, with a fixed sector constant:
`0x1690` (54 sectors), `0x1E88` (87), `0x1EDF` (80), `0x1F2F` (86), `0x1F85`
(34), `0x1FA7` (50, the campaign's scene pictures), `0x1FD9` / `0x2077` (158,
the Egypt map before and after Heishin's coup, chosen by save flag `0x47`),
`0x2115` (50), `0x2147` (16, straight into `0x80140000`), `0x2157` (50),
`0x2189` (76), `0x1DCD` (138, the Library), and the campaign's `0x1E57` (49,
whose 4 KB third chunk at `0x801A8000` is the event script). Whether any of
these carries code for `0x80168000` besides the two above has not been
checked chunk by chunk; the way to check is the one used here — read the
callback's case order and sizes, then look for `jr $ra` density in the chunk.

## Reproducing it

`tools_src/extract_mrg_tables.py` in `MaChInEgUn3/ygofm-decomp` reads the
duel blob's tables out of a raw copy of `WA_MRG.MRG` using the loader's own
arithmetic; `docs/DISC.txt` there carries the measurements above with dates
and the two retractions they replaced.
