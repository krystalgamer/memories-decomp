# Low-level memory-card work controller

`func_8003DC1C` uses the request/event API, not the separate LIBMCRD modal
driver. Its read/write completion cases remain separate structured bodies:
manually merging their tails changes GCC's later cross-jump choices. The
signed sector conversion retains its explicit negative adjustment before
the arithmetic shift. No register pin or executable assembly is introduced.

## Workspace and dispatch bounds

`mem_card_work.h` describes a `0x1C` root and `0x268`-byte slots. The root's
block byte is at `+0x10`, remaining count at `+0x12`, offset at `+0x14`,
chunk at `+0x16`, and text index at `+0x1A`. Slot cursor, free-block count
and directory-entry index are at `+0`, `+0xC` and `+0xD`.

The existing `bss_image_after_viewport` backing covers the work area at
`0x800EF6D0`. A bounded two-slot view ends at `0x800EFBBC`, four bytes
before the independently used directory buffer at `0x800EFBC0`. This is a
capacity view of existing storage, not a claim that the runtime selects both
slots. The only observed explicit resident GP store covering `D_8009B3DD`
is the byte store of zero at `0x8003D31C`, in `func_8003D300`. In particular,
the selector's `u8` type does not justify 256 slots; arbitrary selectors
remain outside the measured contract. The root's block byte and text index
retain their existing interior identities at `D_800EF6E0`/`D_800EF6EA`.

The initialized callback table at `0x80090F88` has five words, ending before
`D_80090F9C`: `func_8003DA40`, `func_8003D74C`, `func_8003D614`,
`func_8003D518`, and `func_8003DA40`. Its two-argument prototype is the
controller's measured calling view. Existing implementations take the root
as `MenuRecord *` and ignore the second argument; their prototypes and
bodies are not rewritten. A low-nibble dispatch is not proof of sixteen
allocated table entries.

## Directory and inherited return ABI

The directory header's absolute arm preserves the controller's word loads
of the count, free-block count and `struct DIRENTRY *`. Other consumers keep
their existing declarations. The free count is intentionally read through
its low byte here. `DIRENTRY` is the actual SDK type, with size `40` and
sector-head field at `+0x20`; the controller does not substitute a private
directory-record approximation.

`MemCard_FindLoadedEntry` is an unusual existing binary ABI. Its matching C
definition is **void**, calls `MemCard_FindEntry`, and returns without
overwriting the index in MIPS `$v0`. The controller consumes that index.
`MEM_CARD_FIND_LOADED_ENTRY_RESULT_VIEW` explicitly selects the returning
caller declaration; the definition retains the default void declaration.
This is not a normal ISO C return guarantee. Native behavior tests must
model the measured returning ABI with a returning stub, not call the void
implementation and assume the host leaves its return register intact.
The unchanged implementation was separately reprobed: all 40 bytes match.

## CPU-addressed scratch and checksum backing

The read, directory-frame and write staging addresses remain respectively
`0x80200000`, `0x80210000`, and `0x80300000` (body at `0x80300200`).
The constants preserve those literal request arguments. No equivalence with
other RAM aliases or assumptions about extra physical RAM are introduced.

`MEM_CARD_WORK_FRAME_BYTES` is a bounded 128-byte view at the literal
`0x80210000` address. Bytes `D_8021007A` through `D_8021007F` are existing
absolute linker identities inside that same range. They are **not** six
independent allocations. The controller modifies `+0x7A..+0x7E`, reads
`+0..+0x7E`, and writes the XOR to `+0x7F`. Native witnesses place one
128-byte allocation at that address and link the six names to its interior
bytes, with an adjacent guard outside the frame.

Reading the checksum through the named array instead of the literal-address
view generated an extra `addiu` for its zero low half, increasing text by
four bytes and shifting four table entries. The bounded physical-address
view preserves the literal materialization and is exact. The normalized
final source has 2,128 exact text bytes and 92 exact private-table bytes at
`0x800103B0`. Normalization runs before the probe, using the intended final
`src/game` include path.
