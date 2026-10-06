# Spanish MODEL464 advancing quads

The previously unmatched helper at `+0x18EC..+0x1E94` is independently
recovered as 1,448 bytes of C in each of six MODEL464/614 physical images:
six new C instances and 8,688 instruction bytes. Every complete 20,480-byte
image matches retail. The [existing sheets](spanish-model-variant464.md),
their ledger, physical-instance table, and module records are unchanged.

A fresh pre-integration screen covered 6,079 regional C entries and two
same-size bodies, with no accepted normalized match. This is retail-derived
recovery, not a French or other regional port. The authoritative
`gcc_2_8_1_g0_split` profile uses GCC 2.8.1 / MASPSX 2.81.

## Original call and state ownership

Entry `+0x1200` passes `s4` in the call delay slot. Delay-slot-aware CFG
reaching definitions find only the original context capture at `+0xC`.
The unsigned time comparison against descriptor `+0x30` at
`+0x11E8..+0x11F8` gates this call. The helper has no additional time or
phase gate before projecting its groups.

Five initialized `0x84`-byte primary records begin at context `+0x4E0`.
Each holds six points arranged as three two-point rows, three paired
packed screen-coordinate rows at `+0x30..+0x48`, two inner and two outer
colors, a vector/rotation initialized by entry, two depths at `+0x70`,
two progress values at `+0x78`, and active at `+0x80`.
Entry initializes group `i` progress to `-2048*i` and `-2048*i-512`.
These negative delays are retained, not prematurely clamped in storage.

The inherited GT4 packet occupies `+0x1A80..+0x1AB4`. Entry establishes
its `s7` pointer at `+0x4C`, initializes it at `+0x26C`, enables
semi-transparency at `+0x2BC`, and disables raw texture at `+0x2C8`.
The helper keeps context in `s4` and the packet in `s1`; only position
and RGB stores modify the packet.

Origin is three words at `+0x1BD8`, followed by an eight-byte anchor,
direction at `+0x1BEC`, and projected direction at `+0x1BFC`. Frame is
`+0x1C18`, unsigned step is `+0x1C24`, the descriptor pointer is
`+0x1C2C`, signed width is `+0x1C48`, and phase is `+0x1C60`.
The private view ends at `+0x1C64`; this does not describe all entry
accesses or prove an allocation capacity.

## Projection, progression, and drawing

The screen-facing rotation is `ratan2(projected.y, projected.x)+2048`.
Even frames use signed `width/128`; odd frames use `width*40/4096`,
narrowed to a signed halfword. For each of the descriptor's groups,
both columns generate opposite points using `rcos/rsin(1024)` and
`rcos/rsin(3072)`, with a zero middle point.

Translation uses `origin + direction*max(progress,0)/1024`. Scale is
4096 on all axes. Preserve the complete coordinate/light-matrix sequence,
both `RotMatrix` calls, `ReadRotMatrix`, `ScaleMatrix`, and `SetRotMatrix`.
`RotTransPers3` saves all three projections, depth, and a stack flag
in a five-by-two word array.

After each column is projected, progress below 1024 advances by
`step*64`. Reaching 1024 clamps progress, marks that group active, and
changes phase one to two. This update is not descriptor-final-iteration
gated. The projection uses the old progress; drawing uses cached results.

A separate pass draws one quad per group from the two outer point rows.
All four vertices use the second outer color. Packed projections preserve
signed high-halfword y extraction. Submission requires nonnegative first
depth and first flag; only then is the stored depth narrowed to u16.
The second depth/flag does not gate submission.

The existing physical descriptor records have counts five, five, and two
for MODEL175, MODEL182, and MODEL244. Signed count at descriptor `+0x44`
bounds both passes. Thus all retail accesses fit the five groups and
five-by-two stack flag array. Nonpositive counts preserve no-iteration
behavior; no new clamp for hypothetical oversized counts is introduced.

## Matching evidence

Thirty-seven target-compiled layout constants and header-coexistence checks
verify the recovered declarations. Original-call/context, initialization,
packet lifetime, progress clamps, submission gates, and actual descriptor
bounds have focused regressions. All selected compiler, retained C/ASM,
raw-tail, resident-callee, loader and context owners remain checked.
The new object has fifteen call relocations to twelve resident addresses
and one local jump. Four new names alias existing resident addresses;
all original thirty-six names and addresses are retained.

Seven experimental records include six candidates and an independent
slot-one compile, followed by six complete-image terminal matches.
The key recovery used a flat six-point array: row-relative writes and
row-base projection arguments reproduce the original pointer lifetimes,
leaving the loop column in a register and spilling its scaled offset.
Integer-first point addressing recovers the final add operand order.
All accesses remain within that one array. The 312-byte frame is natural;
no forced registers, artificial padding or allocation controls were used.

The family now has twelve matching C instances (15,672 bytes) across two
unique routines and twenty-four remaining ASM instances. The last two
helpers remain non-entry-reachable retained code; the raw suffix remains
unclassified rather than being assumed to contain only data.
