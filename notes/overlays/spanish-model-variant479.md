# Spanish MODEL479 radial curtains

Independently recover the previously unmatched 1,216-byte helper at image
`+0x3860..+0x3D20`, using named `gcc_2_8_1_g0_split` (GCC 2.8.1 / MASPSX 2.81).
A current cross-region inventory check found no accepted normalized instruction
shape among 5,833 C registrations, including fourteen same-size entries.
This is new decompilation, not a regional port.

## Physical images and complete code inventory

An exhaustive scan of Spanish MODEL records and stages 7 through 10 finds
exactly MODEL379, record329, stages7/8 with headers479/629, at sectors90984
and90994. Both select request645000. Load bases are `0x8013B000` and
`0x8017B000`; the source symbols remain `func_8013E860` and `func_8017E860`.
The second source changes only the symbol.

| Image range | Bytes | Owner |
|---|---|---|
| `+0x0004..+0x0FD0` | 4,044 | Assembly entry |
| `+0x0FD0..+0x14F8` | 1,320 | Assembly |
| `+0x14F8..+0x1F68` | 2,672 | Assembly |
| `+0x1F68..+0x2B90` | 3,112 | Assembly |
| `+0x2B90..+0x309C` | 1,292 | Exact C [offset sheets](spanish-model-variant479-sheets.md) |
| `+0x309C..+0x3860` | 1,988 | Assembly; no entry-call edge observed |
| `+0x3860..+0x3D20` | 1,216 | Selected exact C |

All seven CFGs are closed and contiguous, with every word visited, one return
and no indirect calls. Entry calls the five helpers other than `+0x309C`.
Fourteen function instances are inventoried: four C instances (5,016 instruction
bytes) and ten retained assembly instances, including the independently recovered
offset sheets documented separately.

The four-byte headers and 4,832-byte suffixes at `+0x3D20` remain raw owners.
Known descriptors in the suffix do not classify every remaining byte. Both
complete 20,480-byte images match their own hashes in the instance CSV.

## Local layout, caller and conditional bank overlap

| Context field | Offset / extent |
|---|---|
| Five 1,272-byte primary records | `+0x0000..+0x18D8`; size at record `+0x4EC` |
| Signed pulse scale | `+0x2EB8` |
| Five 280-byte curtains | `+0x38C0..+0x3E38`; two 17-point SVECTOR arrays each |
| Reused 52-byte GT4 packet | `+0x3E6C..+0x3EA0` |
| Five VECTOR positions | `+0x4078..+0x40C8` |
| Frame / step / signed gate | `+0x4164 / +0x4170 / +0x4198` |
| Inner / outer RGB / angle | `+0x41D0 / +0x41D4 / +0x41D8` |

The `0x41DC` state extent is a partial helper view, not allocation capacity.
Entry instructions establish the primary and curtain bases, strides and
five-record bounds. Each geometry loop writes seventeen points per row;
sixteen projected strips use adjacent points within those arrays.

The selected call at entry `+0xDE4` passes `s6` in its delay slot. A
reaching-definition walk establishes entry `+0xC` (original `a0`) as the only
definition reaching that call, despite `s6` also serving as an initialization
temporary on another path. Entry selects this call in phases3..5. The helper
keeps its incoming context in `s4` and packet pointer in `s2`; it separately
tests the signed gate before drawing each curtain.

**Do not apply a blanket nonoverlap assumption to this family.** Resident slot
setup supplies contexts `0x80136000/0x80176000`. The helper view extends 476 bytes
into the corresponding `0x8013A000/0x8017A000` primary-load bank, but ends before
the secondary executable at `0x8013B000/0x8017B000`.

The actual record's three command words are `(645000, 593000, -2)`. The last
word is the primary command, copied from transfer metadata `+0x118` into slot
`+0xD10`. Target-compiled layout checks tie those offsets to the seven-word
metadata copy and the controller's command view. At resident `0x80058BA8`,
the controller reads that signed primary command; `bltz` at `0x80058BB0`
skips the primary callback at `0x80058BEC`, branching to `0x80058BFC`.
This model's normal dispatch therefore leaves that callback inactive.
The two loaded primary images at sectors91024/91026 have return-two entry
stubs; their hashes and bytes are checked, not treated as newly recovered C.

This is evidence for the actual command-controlled overlap, not a claim
that the primary bank is generally unused or that arbitrary larger contexts
are valid. Resident object/map checks preserve all34 callee bindings, the
four loader/dispatcher owners and eight stored slot-pointer values.

## Recovered rendering

The signed pulse is `((frame & 1) << 5) * pulse_scale / 4096`. The inner radius
comes from `(rsin(1300) * 256) >> 12`; the outer radius adds160 and the pulse,
with outer height `-(256 + pulse)`. For each of five curtains, seventeen
angles separated by256 rebuild the two rows. Translation uses that curtain's
position, subtracting `(rcos(1300) * 256) >> 12` from Y, and uniform scale
comes from the corresponding primary record.

Below scale4096, use the stored RGB values. Otherwise each byte is multiplied
by `8192 - size` and divided by4096 with signed truncation, then narrowed to
`u8`; no extra clamp is introduced. Each strip receives inner RGB on its
first two corners and outer RGB on its last two. Sorting requires nonnegative
depth and flag and narrows depth to16 bits. Angle advances by `step * 80`
even when the gate prevents drawing.

Entry descriptor arithmetic establishes 44-byte records rooted at image
`+0x3E1C`. The selected request uses record zero, including inner RGB
`(180, 192, 192)`. The helper does not directly read descriptor timing fields.

## Recovery and regression evidence

Nine archived comparisons progress from independently recovered geometry to
an exact match. Local SDK `setVector` comma expressions recover the target
geometry scheduling; packet initialization, cursor update order, explicit
base-radius arithmetic and the first fade expression resolve the remaining
differences. Every pointer stays within its corresponding array. No forced
registers, artificial frame padding, instruction edits or new flags are used.

The attempt CSV retains all nine comparisons and two complete-image terminal
records. Dependency fingerprints hash production C followed by its private
header. Eight regression tests cover the physical census and primary command,
metadata, closed CFGs and original-context reachability, bounded geometry,
32 target-compiled layout constants, all input/final C/assembly/raw owners,
thirteen selected call relocations per image, resident ownership and the
conditional overlap, and terminal fingerprints.
