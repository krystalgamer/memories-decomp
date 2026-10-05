# Spanish MODEL405 expanding bands

Independently recover the previously unmatched 1,008-byte helper at
`+0x1774..+0x1B64` with `gcc_2_8_1_g0_split` (GCC 2.8.1 / MASPSX 2.81).
A fresh check of 5,845 accepted regional C registrations, including four
same-size entries, found no accepted normalized instruction shape. This
is new decompilation, not a port of accepted regional C.

## Physical images and ownership

An exhaustive Spanish MODEL record scan identifies eight physical images:
MODEL57 and MODEL562 at stages9/10, and MODEL149 and MODEL419 at stages7/8.
Headers405/555 load at `0x8013B000/0x8017B000`. The instance CSV records
each record index, sector, command and complete-image hash; no distinct
image is represented as a duplicate sector.

| Image range | Bytes | Owner |
|---|---:|---|
| `+0x0004..+0x0D00` | 3,324 | Assembly entry |
| `+0x0D00..+0x12CC` | 1,484 | Assembly helper |
| `+0x12CC..+0x1774` | 1,192 | Assembly helper |
| `+0x1774..+0x1B64` | 1,008 | Independently recovered C |
| `+0x1B64..+0x1FBC` | 1,112 | Exact C [outer bands](spanish-model-variant405-outer-bands.md) |

All five contiguous CFGs are closed, visit every instruction word, have
one return and no indirect calls. **No entry-reachable call to the selected
helper is observed.** Entry calls the other three helpers. The selected
code and its initialized state are recoverable, but this does not establish
that normal gameplay invokes it.

The four-byte headers and 12,356-byte suffixes remain raw, including known
descriptor records. With the independently recovered outer-band helper,
sixteen C instances account for 16,960 instruction bytes;
24 function instances remain assembly. All eight complete 20,480-byte
images reproduce their own retail hashes. Existing registrations are
preserved unchanged.

## Entry-proven partial layout

| Context field | Offset / extent |
|---|---|
| Three 288-byte band records | `+0x0504..+0x0864` |
| Sixteen 52-byte GT4 packets | `+0x2E08..+0x3148` |
| Three signed translation words | `+0x31D0..+0x31DC` |
| Signed step | `+0x3224` |
| Stored descriptor pointer (`G32`) | `+0x3230` |

Each band contains two seventeen-point SVECTOR arrays at record offsets
`0x00/0x88`, inner/outer colors at `0x110/0x114`, signed size at `0x118`,
and a completion counter at `0x11C`. Entry establishes the `0x504` base,
three-record bound and `0x120` stride; the next array starts at `0x864`.
It initializes seventeen points per row and the three sizes to
`0, -4096, -8192`, with zero completion counters.

The initialized rings have radii256 and384 in the XY plane. Sixteen
adjacent-point strips stay within both seventeen-point arrays. Packet
colors are written only to the twelve RGB bytes; the sixteen-packet
cursor remains inside its initialized packet bank.

Entry selects 32-byte descriptors rooted at image `+0x1FF4`, indexed by
the request's low three decimal digits. The private descriptor is only
a sixteen-byte view through its signed count at `+0xC`, not a declaration
of the complete record. Requests571000/571001 select three bands;
MODEL419's571002 selects two. The helper reloads this count at each loop
test rather than replacing it with a constant.

The `0x3234` context view is partial, not allocation capacity. Resident
stored context pointers `0x80136000/0x80176000` place that view before
the corresponding primary and secondary load banks. Entry's original
context reaches these initialization offsets; absence of a selected
call means no caller-to-helper argument provenance is claimed.

## Rendering and updates

Positive-size bands render sixteen GT4 strips at the context translation,
with zero rotation and uniform signed scale. Six separate byte temporaries
hold the two RGB colors. Above size3072, each channel is multiplied by
`4096 - size`, divided by1024 with signed truncation and narrowed to a
byte; otherwise the original channel is used. No extra clamp is added.
Inner RGB covers the first two corners, outer RGB the last two.

Sorting requires both nonnegative depth and flag, and narrows priority to
sixteen bits. Size growth is independent of drawing: any size below4096
adds `step * 128`. Reaching or exceeding4096 clamps the size and increments
the completion counter. Nonpositive sizes therefore still advance.

## Recovery and regression evidence

The first independent reconstruction had the exact size but33 instruction
differences. Initializing the inner loop index before its vertex cursor
left13 induction-register differences. Natural indexed corners removed
the separate cursor and matched every byte, without forced registers,
artificial frame padding or new compiler flags. A separate slot1 build
also matched.

The attempt ledger records all four comparisons and eight complete-image
terminal results. Production dependency fingerprints hash body C followed
by its private header.

Seven focused regressions cover the exhaustive physical census, commands,
descriptor selection, complete CFGs, non-call caveat, geometry and packet
bounds,24 target-compiled layout constants, complete-image C/assembly/raw
input and final owners, the selected linker objects, ten external call
relocations and one local jump per C object, all34 resident bindings,
loader/dispatcher owners, stored slot pointers and terminal fingerprints.
