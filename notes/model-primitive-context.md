# Model primitive decoder input records

The resident primitive decoder at `0x8004D134` now takes the SDK
`GsTYPEUNIT` descriptor and `GsARGUNIT` context instead of a halfword
pointer and byte pointer. `Model_InitSlot` supplies both directly from the
`GsScanUnit` dispatch: tags 0 and 1 select the normal/shared primitive
paths, while other tags use different handlers.

Only the descriptor's low 16 type bits are read, exactly as before. The
typed `u16` narrowing deliberately retains those halfword loads, including
the repeated selector read inside the loop. No interpretation of the
descriptor's other flags is added.

## Common SDK context prefix

The tracked Psy-Q `libhmd.h` declares `primp` at offset zero of the common
`GsARGUNIT` prefix. The normal and shared variants both put `primtop` at
`+0x14`. The decoder uses only those two pointers; it does not reinterpret
the variant-specific vertex/normal tables beyond them. Assertions check
that both concrete SDK variants retain the established `primtop` offset.

The decoder owner's `ModelPrimitiveBlockHeader` view describes the eight bytes
reached through `primp`: `descriptor_word_count_le` retains the original
`u8[2]` view of the SDK's little-endian descriptor word count, followed by
the unsigned record count at `+2` and signed word offset at `+4`. Size and offset
assertions preserve the two existing loads. The record address remains
`primtop + word_offset * 4` in bytes: no extra flag mask, unsigned
reinterpretation or guessed primitive-field layout is introduced.

This agrees with the neighboring normal primitive handlers' use of
`arg->primp`, its second halfword and `arg->primtop`, without copying their
different offset masking into this decoder. The eight selector arms,
variable record strides, halfword edits, index maxima and byte totals all
retain their existing logic.

The European path formerly reused the byte-typed context parameter as a
per-record halfword cursor. It now names that cursor separately as bytes,
rather than forming a potentially unaligned SDK-structure pointer into a
primitive. The context itself stays an SDK context throughout its lifetime.
No regional target address or compiler profile is changed.

The former per-component header controls are historical; they are not
revived by these views. Retained tooling and source/ABI policies stay
unchanged. Full North American executable matching remains the code-generation
gate; regional exact-match results are left to PR CI.


## SDK descriptor lengths and complete local dataflow

GsScanUnit reads the unsigned halfword at type-word +4, shifts it by two
and advances to type-word +4 +4*word_count. In the NA SDK routine these
operations are at 0x8008A118..0x8008A12C. This independently identifies the
descriptor length reached through the context's primp pointer: it counts
32-bit descriptor words AFTER the type, not primitive records or pixels.
All 56 measured textured descriptors in the seven PAL auxiliary HMDs carry
two. The primitive header keeps that value as its original u8[2] byte pair;
the separate four-byte image-record prefix keeps its original u16 scalar.
Neither view is widened, merged, initialized or used for new validation.

The COMPLETE primitive decoder names its existing header, primitive base/
record, record count/countdown, page-field byte offset, stride, packet size, page/
CLUT biases, colour depth and kind. The captured primitive-record operand
inside MODEL_MAX_VALUE adopts the same binding name; its structure and
captured binding are unchanged. Reused x/y, European selector/max scratch and
secondary cursors retain their original roles, lifetimes and read sites.
Preserve all eight selector arms, regional gates, packed-field arithmetic,
nullable maximum, repeated selector reads, private switch data and returns.

The COMPLETE image processor distinguishes its context, image/next/CLUT
record cursors, palette data/entry/row/column, record index/count, below-two
branch, texture X, coordinate scratch and image-with-CLUT kind. below_two_slot
is STILL the raw signed model_index<2 test, not a validated nonnegative slot.
Keep first-entry versus later-entry writes, partial record advances, signed
row/column tests, per-region coordinates/clipping, original casted views,
callback lookup/invocation, DrawSync and final SDK-sentinel publication.
No field width, type, expression, macro structure, call or public ABI changes.

## Measured palette-source linkage

These are static observations from the original PAL/NA discs, not emulator
or live-GPU traces. The executable hashes were checked against tracked
targets before reading bounded package prefixes. No retail bytes are tracked.

The seven PAL auxiliary packages at SU sector 0x2A8 +116*index contain
2,946 indexed polygons in 56 textured descriptors. Their measured split is:

| Auxiliary package | CLUT row below 256 | CLUT row at least 256 |
| --- | ---: | ---: |
| 0 | 174 | 272 |
| 1 | 144 | 208 |
| 2 | 0 | 412 |
| 3 | 0 | 449 |
| 4 | 0 | 456 |
| 5 | 395 | 0 |
| 6 | 436 | 0 |

ALL 1,149 low-row records use original CBA 0x3C60, meaning X=512/Y=241.
The PAL slot-2 decoder maps that to X=640/Y=241. This is precisely the
auxiliary callback's stage-three RECT(640,241,256,1), whose first 512 bytes
come from package sector 82. NONE uses the separate original-X<512 branch.

The other 1,797 records have Y>=256, so the decoder deliberately leaves
their CLUTs unchanged. Their actual 16/256-colour extents lie within
X=[512,768), Y=[256,512): the auxiliary callback's initial 64-sector image
bank. Its 64x16-word chunks start at (512,256), with IMAGE_ADVANCE_X cleared.
The transfer driver advances Y by 16, wraps at the 256-row boundary and
advances X by 64, covering four columns. These are not the low-row upload.
No sampled auxiliary HMD contains a type-2 image unit; the demonstrated
upload is the asynchronous package loader, not the SDK image-unit handler.

Independently read ordinary package zero in BOTH discs has 772 indexed
polygons: 538 use CBA 0x28 and 234 use 0x68, all 8-bit palettes. Original
X=640/Y=0 or 1 becomes NA X=slot*256/Y=240 or 241, matching the tint destination
from source rows 248/249. PAL keeps X=640 and selects Y=208/209 or 224/225,
matching its tint destination from source rows 216/217 or 232/233. The actual
two-sector palette source is package sectors 144..145 in both discs.

This closes the measured PAL background and ordinary-sample upload links,
not every input/selector domain. The function therefore keeps its tentative
identity and confidence; the unexercised low-X branch is not invented as a
runtime observation. Only descriptor fields and scoped local bindings gain
names. No new behavioral harness, retired fixture, SDK body or compiler flag.
