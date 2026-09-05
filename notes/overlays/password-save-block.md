# What the password overlay does to the save block

Derived from matched overlay code, cross-checked against the save map in
[`../research/the-game.md`](../research/the-game.md) §4.5 and §12. Every
address below is read from a function that reproduces the retail bytes
exactly; the interpretation is flagged where it goes beyond that.

## `func_8016AA6C` rebuilds the block from a template

The function does three things in order:

1. `Util_FillMemory(D_801D0000, 0, 0x3000)` (`0x80035748`) — clears
   `0x801D0000`–`0x801D3000`.
2. `func_8008E870(D_80168090, D_801D0000, 0x3000)` — fills the same range from
   overlay data. `0x90` is below the password module's first function at
   `0x801680B4`, so the source is the module header region and not code.
3. Stamps a non-zero word at `0x801D0534`, exactly `0x334` bytes after
   `gDuel_awPlayerDeck` (`0x801D0200`), retrying until it is non-zero.

The cleared range is wider than the save block itself, which the game notes
place at `0x801D0200` onward.

## The stamped word and its source

| address | how it is formed |
|---|---|
| `0x801D0534` | `(D_8009B09C << 8) | checksum`, rewritten until non-zero |
| `0x801D060C`–`0x801D0617` | the twelve bytes XORed together to make `checksum` |

Two things line up with the documented map and are worth recording:

- The twelve XORed bytes end at `0x801D0618`, which is exactly where the flag
  array begins. The checksum covers the twelve bytes **immediately preceding**
  the flags, not a range that straddles them.
- Both addresses fall in the gap the map does not describe. The trunk ends at
  `0x801D0522` and the flag array starts at `0x801D0618`, leaving
  `0x801D0522`–`0x801D0617` unmapped; `0x801D0534` and the checksum source
  both sit inside it.

That the word is retried until non-zero says zero is reserved as a sentinel —
most likely "no save present" — but nothing traced here reads it back, so the
consumer is unidentified and the field is not named.

## Note on §4.5's phrasing

§4.5 refers to "the password-use counter in the save [`0x801D0534 + 0x164`]".
That address is `0x801D0698`, which §12's flag-array map explains as the
*used-password flags* at `0x400 + card`, and which the GameShark note
describes the same way. So the two sections agree on the address while
disagreeing on whether it is a counter or a bitfield; the flag reading is the
one supported by evidence recorded in this repository.

## `0x801D0534` is written as a structure base twice

Two separate fields in the map are expressed as offsets from `0x801D0534`,
each with a different offset:

| field | as written | resolves to |
|---|---|---|
| password use, §4.5 | `0x801D0534 + 0x164` | `0x801D0698` |
| duelist win/loss records, §12 | `0x801D0534 + 0x1EC` | `0x801D0720` |

Both are given alongside a plain absolute address elsewhere, so neither
needed to be written that way. One such expression would be incidental
phrasing; two independent offsets from the same base is a much better
argument that whoever wrote the map was reading a structure rooted there.

That is consistent with `func_8016AA6C` stamping a single word at exactly
`0x801D0534` — a header field at offset zero of that structure — but the
function alone does not establish it, since it never reads the word back.
