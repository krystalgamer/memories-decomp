# Duel-effect projected number renderer

The complete Spanish `func_801566D4` interval,
`0x801566D4..0x80156AD4`, is 1,024 bytes of matching C in
`src/overlays/duel_effects/number_renderer.c`. It uses the existing
`gcc_2_8_1_g0_split` profile: GCC 2.8.1 and MASPSX 2.81. No shared header,
compiler profile, inline assembly or external reference declaration changes
are needed.

## Evidence and preserved behavior

The existing canonical declaration takes a signed word value, RGB bytes,
an `SVECTOR` offset and unsigned halfword mode, size and bias. The digit-count
callee still receives its signed-halfword argument and the digit converter
still receives the absolute value through its unsigned-halfword contract.
Do not broaden these helper contracts or introduce new input clamping.

The renderer constructs twenty local vertices, uses eight digit halfwords
and retains two complete 40-byte `POLY_FT4` packets in the original
0x160-byte frame. It projects four adjacent vertices per glyph. The first
glyph selects index 10 for a positive value and 11 otherwise; subsequent
glyphs come from the digit buffer. The otherwise unread whole-packet copy
is intentional: retail executes that copy.

With zero bias, nonnegative projection flags permit modes 1 and 0 only.
Both branches assign **both** `draw_packet` and `draw_mode` before one common
`func_80152F9C` call. The packet temporary and its declaration order are
required for the observed old-GCC branch/register allocation. Collapsing
the two assignments into a mode-only selection produced 1,016 bytes with
53 differing words; duplicating direct calls produced 1,032 bytes with
64 differing words. Assigning both arguments reproduced all 256 instructions.
Moving the vertex increment before that common call retained the correct
size but changed 13 words, and was rejected.

With nonzero bias the depth is `depth + 1 - bias`; negative depth or
projection flags suppress submission. Mode 1 submits with blend 1;
every other mode submits with blend 0. The submitted depth is the
unsigned-halfword conversion of its arithmetic right shift by two.
Every path advances the vertex index by two. These paths are not
interchangeable with the zero-bias mode restrictions.

## Actual data ownership

| Owner | Address | Bytes | Interpretation |
|---|---|---:|---|
| Glyph rectangles | `0x8015B3C0` | 96 | Twelve canonical 8-byte `RECT` entries |
| Texture words | `0x8015B748` | 84 | Existing canonical 21-pair union |
| Ordering-table pointer | `0x8015B7F4` | 4 | Existing generated-data pointer |

The glyph rectangles have x/y/width/height at offsets 0/2/4/6. All twelve
observed dimensions are 15 by 15; their 96-byte extent ends at `0x8015B420`.
That following address is separate effect-1 configuration, not part of
the glyph table. The renderer reads texture pair 9 at offsets 0x24/0x26
of the canonical union rather than inventing another prefix declaration.

Twenty target-GCC constants verified these sizes and offsets, packet
coordinate/tpage/clut offsets, vertex/digit array extents and the 32-bit
SDK `long`. Each of the three data symbols has exactly one non-executable
generated-data input-object definition, with its measured size and original
bytes. The final symbols are real section-backed objects at their original
addresses, not linker-script absolute aliases. The final linker combines
code and data into `.module`; its executable flag is not the input-data
ownership test. The renderer object defines no data, BSS or constant storage.

The digit count, digit conversion, alternating-sign helper and FT4 submitter
remain four real executable overlay functions with inventory-backed extents.
All 71 accepted Spanish C entries from `42dd72c81932dd50da80fda34098ac9030584807`
are preserved unchanged.

## Validation and scope

Before registration, the candidate and every accepted C group independently
reproduced the complete 90,112-byte bank. Production
`make spanish-match-overlays` then reproduced every configured Spanish
image. All seven actual terrain copies at `7193 + 240 * terrain` were
compared in full against the production bank:
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.

This adds one function / 1,024 bytes: 72 of 85 bank functions / 39,732 C
bytes, or 196 of 209 configured Spanish function instances / 95,584 C
bytes. Thirteen bank functions remain unmatched, including the curve
generator. No other regional renderer is registered by this change.
Boot, MODEL/SU and opaque overworld fragments remain separate open scope.

The historical failures, four final argument-order probes, source hashes,
whole-bank preflight and production owner/layout receipts remain local
under `tmp/number87-proof`, `tmp/number-renderer-full` and
`tmp/number89-*.json`. No retail bytes or generated artifacts are tracked.
