# French MODEL variant 82/212

Model 66 stages 7/8 contain two complete 20,480-byte images with a closed
3,552-byte entry at the two physical loads. Command 12008 selects descriptor
8 directly. This adds 7,104 matching-C instruction bytes and 32 source-owned
literal bytes; it does not establish exhaustive MODEL coverage.

## Ownership and evidence

| Image offset | Bytes | Owner |
| --- | ---: | --- |
| `0x0000` | 4 | Raw module header |
| `0x0004` | 3,552 | C entry |
| `0x0DE4` | 16 | C unit VECTOR |
| `0x0DF4` | 28 | Raw GsIMAGE view |
| `0x0E10` | 16,880 | Unclassified suffix |

Both full images were relinked without masks using four actual C entry/literal
contributions and six disjoint raw owners. The 21 resident callee bodies were
checked against the byte-identical French resident ELF. The 33,760 suffix
bytes remain unclassified, including the descriptor table. Decoding the
selected descriptor does not establish ownership of the entire suffix.

Native slot entry hashes:

- `0x8013B004`: `16483f4522ca742afad71974dd594d348c0c80e714879c1948b7a75e52fe9818`
- `0x8017B004`: `6f7ef06ca68f4b3e289737af62e13e7af05c755d5d8657ff54c21a6adc56939d`

The local GCC 2.8.1 / MASPSX 2.81 `gcc_2_8_1_g0_split` profile remains
authoritative. The companion ledger preserves twenty paired experiments and
two canonical records. The named no-CSE-follow-jumps control did not match
and is not used by the source. No reference types or flags, inline assembly,
forced registers, artificial stores, or fake dependencies were used.

## State and native behavior

The 28-byte descriptor contains four bytes and twelve signed halfwords.
Its selected values are RGB 160/160/64, attachment 29, initial and target
spreads 200, half-size 150, initial Y bias 100, travel duration 100, count 200,
spark divisor 10, delay 40, rate 1 and end time 330. Byte 3 and halfword 24
remain unknown. All selected divisors are positive.

The `0x2210` state has a config pointer, positions at offset 4, velocities
at `0x1004`, remaining-flash bytes at `0x2004`, signed packed texture at
`0x2204`, elapsed time at `0x2208` and a completion byte at `0x220C`.
Only 201 position/velocity records and 200 lifetime bytes are represented as
arrays; the intervening storage remains unknown. Constructor and rendering
loops use 200 records. The native inclusive warmup loop uses 201.

Every invocation clears a rotation SVECTOR, copies the unit-scale VECTOR
and computes the direction from the active slot. Construction clears point
xyz, sets remaining lifetimes to eight, uploads one texture, then initializes
elapsed to negative delay and completion to zero.

During negative elapsed time, native code passes the uninitialized local
base MATRIX to `GsSetLsMatrix`; the light-source copy exists only in the
nonnegative rendering path. This deliberately preserves retail behavior,
like accepted MODEL117, and does not assert well-defined C. No initializer
or artificial store has been introduced.

Warmup captures the attachment position, randomizes xyz, and derives velocity
toward a randomized destination. Y uses a spread-plus-350 offset. Warmup Z
uses direction times 350, whereas rendering resets use direction times 450.
Rendering resets particles whose index/rate has not yet been reached. Active
particles advance each coordinate with its own frame-step query, not one
cached query.

Each particle projects a one-pixel box. Before directed Z reaches 450 it uses
full RGB; afterwards its RGB follows the remaining lifetime. Submission
preserves activation, random-gate, depth and projection-flag checks. Beyond
450, a nonzero lifetime also produces an eight-frame billboard atlas flash,
using four columns, two rows, 32-pixel tiles and 31-pixel UV extents.
Remaining lifetime decrements once even if the billboard is clipped.

The stack contains three adjacent SVECTOR records: zero, negative diagonal,
and positive diagonal. Projection receives records 1/1/2/2. The otherwise
unused first record has native zero stores and is retained.

After elapsed advances, return zero before rate plus travel duration, two
strictly after end time, four before rate plus travel plus count, then one
once while setting completion and zero on subsequent calls.

## Matching structure

Separate block-local random sample and biased destination values in each Y
calculation retain the native arithmetic and register lifetimes. Function-wide
temporaries change allocation; fully inline arithmetic reassociates the 350
offset and introduces an extra delay-slot NOP. A local backward `goto` to the
shared zero-return block expresses the recovered terminal control flow.
The resulting entries retain the native 280-byte frame and literal placement.
