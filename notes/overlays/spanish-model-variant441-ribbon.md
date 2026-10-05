# Spanish MODEL441 retained ribbon

`func_8013CDDC` / `func_8017CDDC`, `+1DDC..+23FC`, is independently recovered
game-owned C: 1,568 bytes in each of the ten physical MODEL441 images.
A refreshed screen of 6,057 configured regional C entries, including accepted
framebuffer rings and French MODEL129, found no same-size body. No regional
C was ported. The existing `gcc_2_8_1_g0_split` GCC 2.8.1 / MASPSX 2.81
profile matches both load slots and every complete 20 KiB image.

This adds ten C instances / 15,680 bytes. All three previously accepted
helpers, their ledgers, the instance CSV and every module record are preserved.
The family now has forty C instances / 61,600 bytes, four unique routines,
and fifty remaining physical ASM functions.

The broader discovery also contains eight unmatched normalized-shape
counterparts at `+1E60` in MODEL417/567 images (models 134, 232, 354 and 535).
Those are not integrated here, are not additional unique recoveries, and
require their own context/layout and complete-image proof.

## Retained code and ownership

As with the retained line helper, **no runtime caller has been recovered**.
The closed entry call graph does not reach this helper, and no in-image
direct jump/call or aligned entry-address reference exists. This is not a
claim of universal unreachability through external code. No dispatch is
added and no original-context call edge is invented.

The helper captures its own `a0` in `s4` at `+1DE4` and holds its packet
pointer in `s2` from `+1E24` until the epilogue. Entry initialization
corroborates field ownership, not a runtime call: its stable `s7` points at
`context+256C`; `+264` initializes `POLY_GT4`, `+2A0` enables semitransparency,
and `+2AC` disables raw-texture mode. Page, palette and UV values initialized
by the entry are retained by this helper.

## Layout and two-panel rendering

One `6C`-byte group begins at context `FD8`, ending at `1044`. Three rows of
two `SVECTOR` endpoints occupy `0..30`; three rows of two packed signed
projection words occupy `30..48`. Bytes `48..64` remain opaque. Two signed
depths occupy `64..6C`. The packet is `256C..25A0`.

Origin is `2690`, direction `26A4`, projected direction `26B4`, signed time
`26D4`, descriptor pointer `26E4`, and descriptor iteration index `26F8`.
Signed halfwords at `2714/2716` hold progress and width; signed phase is
`271C`. The private view ends at `2720`, not necessarily the whole context.
Thirty-nine target sizes/offsets and four coexisting private headers are checked.

When phase is positive, signed width divided by 32 supplies the radius.
Each endpoint has outer points at trig angles 1024 and 3072 and a zero
center point. Rotation uses the projected direction. Endpoint zero is
translated to origin; endpoint one to `origin+direction*progress/1024`.
The original matrix calls, including `ReadRotMatrix`, the second
`RotMatrix`, uniform scaling by 4096 and `SetRotMatrix`, are preserved.

`RotTransPers3` writes three packed screen coordinates and one depth per
endpoint. Two quads connect each outer row to the center row, colored
blue `(0,0,128)` outside and gray `(192,192,192)` inside. Packed x narrows
to the low halfword; packed y uses signed right shift by 16.
Each submission requires **strictly positive depth**, narrowed to `u16`.
The GTE flag is written but never tested. No texture state is reinitialized.

## Signed timing and exact recovery

Updates run only when `index+1` equals the descriptor's unsigned count at
`+18`. At phase one and progress at most 1024, time at/after `+20` computes
`(time-start)*1024/(end-start)` using signed fields `+20/+24`, then narrows
to the stored halfword. Progress at least 1024 clamps to 1024 and advances
phase two; this clamp is outside the time gate. Positive width similarly
fades using `+28/+2C`. Nonpositive width clamps to zero regardless of phase,
but only phase three advances to four. Actual descriptors have count one
and positive expansion/fade intervals.

The first candidate was 1,564 bytes with a natural 256-byte frame.
Signed-16 radius and row-relative point expressions recovered the 1,568-byte
extent, leaving ten words. Packed projection words recovered eight signed-y
loads; descriptor-first comparison recovered the remaining two load-order
words. No SDK types, compiler flags, forced registers, fake locals, padding
or inline assembly were introduced.

The fourteen-row ledger includes all three candidates, independent slot-one
compilation and ten terminal whole-image matches. Three SDK aliases preserve
all original names and all 37 resident addresses. Checks cover packet/context
lifetimes, actual descriptors, compiler/retained ASM/raw-data owners, sixteen
call relocations to twelve resident addresses, and the one local jump.
