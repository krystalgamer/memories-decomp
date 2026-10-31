# Resident BSS ownership

The North American executable's zero-filled BSS image (`0x8009B090` to
`0x800FE728`, `bss_image` in `config/slus_01411/image_map.json`) used to be
three generated `bin` fragments around `graphics_frame.c`'s viewport pair, with
every game global in it resolved by a `c_symbols.ld` assignment. For #2602 the
game-owned part is now emitted by four data-only C units listed in
`config/slus_01411/data_c.json`:

| Unit | Section | Range |
|---|---|---|
| `src/game/bss_8009b090.c` | `.sbss` | `0x8009B090`-`0x8009B146` |
| `src/game/bss_8009b14a.c` | `.sbss` | `0x8009B14A`-`0x8009B14C` |
| `src/game/bss_8009b14c.c` | `.sbss` | `0x8009B14C`-`0x8009B4A8` |
| `src/game/bss_8009b4a8.c` | `.bss` | `0x8009B4A8`-`0x800F5D20` |

The Psy-Q libraries' BSS from `_spu_RQ` (`0x800F5D20`) to the end of the image
stays a generated `bin` fragment, as SDK storage is out of scope.

## How the units are written

- Every linked symbol in the range keeps its exact address. The inventory is the
  union of the configured symbols and every symbol the matching build links at
  those addresses, so labels referenced only by matching C or generated assembly
  are kept too.
- Where a header declaration is visible and its type fits the gap to the next
  symbol at that address's alignment, the unit defines the symbol with that exact
  type and includes the header. Otherwise it defines a raw `u8`/`u16`/`u32` object
  under a private C identifier with an `__asm__` label carrying the real name, so
  it cannot conflict with a consumer's view. GCC 2.8.1 word-aligns arrays and
  aggregates, so misaligned ranges are written as scalars.
- Unnamed bytes are defined as `D_<address>` objects. Each unit's section starts
  at an address aligned to its largest object, which is why the two-byte range
  at `0x8009B14A` is its own unit.
- A name that `link_symbols.ld` aliases to another (for example
  `gDuelEffect_aEntries = tent_DuelEffectEntries`) is never the definition; the
  alias target is. Secondary names sharing a defined address resolve through
  added `name = owner;` lines in `link_symbols.ld`.
- Declarations that `src/unmatched.h` held for these now-defined globals moved,
  unchanged, to `src/game/resident_bss.h`, which `unmatched.h` includes.

Tentative (common) definitions in consumers, such as the `tent_` registries, now
resolve to these strong definitions and allocate no storage, as in
[small-data-ownership.md](small-data-ownership.md).
