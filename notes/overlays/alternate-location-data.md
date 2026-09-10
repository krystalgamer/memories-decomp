# Alternate overworld table storage

`src/overlays/overworld/alternate_location.c` defines the sixteen
`AlternateLocation` records at `D_80169E54`. The existing
`alternate_location.h` is both its owning header and the interface consumed
by `CampaignMap_PickAlternateExit` and `CampaignMap_UpdateAlternateLocation`.
It supplies the bounded extern and `ALTERNATE_LOCATION_COUNT`.

The record types are unchanged: 66 bytes per location, including four
12-byte exits at `+0x12`. The new data-only unit compiles separately into
both overworld images under `gcc_2_8_1_g0`. Neither alternate function body
nor its existing `-O0` profile changes.

## Exact extent and placement

| File range | Runtime range | Owner |
|---|---|---|
| `0x1E54-0x2274` | `0x80169E54-0x8016A274` | Sixteen C records, `16 * 66 = 1056 = 0x420` bytes |
| `0x2274-0x3000` | `0x8016A274-0x8016B000` | Remaining generated data blob |

The split does not claim the following four bytes or the object-pointer
state beginning at `D_8016A278`. All following state, arrays and padding
remain in the raw blob. The new table segment selects only the C object's
`.data`; it does not rely on an absolute assignment overriding the symbol.

Both verified module slices have SHA-256
`0259a2516466fd98262c69a1f4da03d22fec71b9f4e125adea7925fe35220c12`.
The standalone compiled data section and each final module reproduce every
byte of that slice. A clean rebuild distinguishes the selected C object from
stale generated assembly.

## Equal bytes do not identify one object

These alternate tables also equal the live tables at `0x801691A8` byte for
byte. That is four equal slices across the two module images, but **two
different storage addresses in each module**. The live and alternate
functions use different globals and have different unresolved-call
constraints. No table is aliased to the other or combined into one object.

The initializer therefore follows `AlternateLocation` and `AlternateExit`,
not the live `MapLocation` interpretation. The alternate readers constrain
the gate, input mask, destination, move value, confirm byte and unsigned
coordinate copies. Bytes they do not interpret remain explicit byte arrays
or scalar padding fields. They are not replaced with the live family's
camera or sprite names merely because the stored bits agree.

In particular, `pad17` is `0x80` in records 10-15, and the other padded
ranges include nonzero bytes. Every byte is initialized explicitly rather
than treating unobserved storage as zero or free space. High-bit flag
values and the unused-exit destination sentinel `16` are preserved.

## Symbols and unchanged raw calls

The old generated blob had interior labels `D_80169E60` and `D_80169E62`,
at the first record's `a` and `b` fields. No tracked C/header names either
label directly. The C consumers continue to address those fields through
`D_80169E54[index]`.

After mapping, `D_80169E54` is section-defined data at `0x80169E54`.
Splat's generated absolute alias for `D_80169E62` still has its original
address; the unreferenced generated `D_80169E60` storage label is no longer
emitted. There is no linker assignment overriding the C table definition.
No interior function symbol was found in this table's baseline ELF range.

This does **not** resolve the alternate controller's separate call into the
live table at `func_80169230`. That target and the other raw callees remain
unchanged, as documented in
[`campaign-map-records.md`](campaign-map-records.md). Moving table storage
into C is not evidence for relabeling those calls as live entrypoints.
