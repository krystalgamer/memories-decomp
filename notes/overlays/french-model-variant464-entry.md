# French MODEL464 loader entry

`FrenchModelVariant464_HandleRequest` (`0x8013B004` / `0x8017B004`, MODEL464
family, image offset `0x4`, 0x1364 bytes; one source for both slots, bound per
image) is the loader entry of the six MODEL175/182/244 images
described in [the family note](spanish-model-variant464.md). **No release had
matching C for this body before.**

The body follows the French MODEL439 entry (`variant439_entry.c`) with a
different work layout, kept local in `variant464_entry.h`:
- three 0x1A0-byte webs at `+0x0`, five 0x84-byte primary records at
  `+0x4E0`, six 0x9C-byte sheets at `+0x774`, six rings at `+0xB1C`, four
  spoke rings at `+0xE7C` and one fan at `+0x10BC`;
- three 0x1E4-byte screen rings at `+0x114C` (seventeen points per row, the
  rows at radius 256/384/512 and depth 0/-128/-256) with one RGB byte set per
  point, cycling red, orange, yellow, green, cyan, azure, blue and magenta;
- three 0x118-byte curtains at `+0x16F8`; the third has scale 0 and the
  others start at 0 and -2048;
- the packets at `+0x1A40..+0x1BA0`, the matrix at `+0x1BC4`, the target at
  `+0x1BE4`, the descriptor pointer at `+0x1C2C`, the part handles at
  `+0x1C34`, the timing index at `+0x1C40`, the phase at `+0x1C60` and the
  slot at `+0x1C70`.

The 72-byte descriptor keeps the MODEL439 colours and part bytes, growth
start at `+0x1C`, a timing array at `+0x30` and a signed count at `+0x40`.
Each command initialises primary progress to `-(i * 2048) - j * 512`.
After the timing word selected by the index is reached (and while the index
is below the count), the run path resets primary progress to
`-(i * 1024) - j * 512`, clears the first five sheets' shown flags and
advances the index. It calls the sheets and rings helpers from the growth
start and the advancing helper from the first timing word.

Matching needed:
- the flat CLUT as a `u16` and the packed word declared before the curtain
  page, which fix the spill slots;
- the shared `outer` counter for the sheet loops, `i` for the run-path
  primary reset and the spoke-style angle step in the ring loop;
- binding `primary` before `i` in the run-path reset.

`GetClut`, `GsGetLwUnit`, `RotTransPers`, `SetPolyFT4`, `SetPolyG3`,
`SetPolyG4`, `Square0` and `SquareRoot0` were added to the French MODEL464
binding file at their resident addresses.

## Spanish reuse

The six Spanish SLES-03951 images with the same names are byte-identical to
the French ones (same MODEL.MRG hash and the same image hashes). They now
use the French entry, [rings](french-model-variant464-rings.md) and
[webs](french-model-variant464-webs.md) sources and share the French binding
file contents. All twelve images rebuild exactly; the
[attempt ledger](french-model-variant464-entry-attempts.csv) records the
materially distinct probes.
