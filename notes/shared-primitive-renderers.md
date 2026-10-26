# Shared primitive cache preparation and polygon renderers

This component names the complete game-owned handwritten shared dispatch
family selected by `Model_GetSharedPrimitiveHandler`: one cache-preparation
stage and sixteen cached-input polygon drivers. All 119 actual native
function records across the seven resident images retain their original
ownership, `handwritten_asm` status, extents and instructions. No C
implementation, wrapper, new call or profile is introduced.

The NA bodies cannot currently be matched under the
`gcc_2_8_1_g0_handwritten_asm` policy, and the attempt ledger records why:

- All sixteen drivers put a vertex-cache `mtc2` in branch delay slots. GCC
  2.8.1 reorg never moves an asm insn into a delay slot (`stop_search_p`), and
  MASPSX pads an unfilled slot with `nop`.
- The Gt4 drivers also share the direct Gt4 `lw`+`lwr` blocker described in
  `notes/direct-polygon-renderers.md`.
- `Model_PrepareSharedPrimitiveCaches` fills three branch delay slots with
  loads. The MIPS1 machine description gives loads `dslot=yes`, so reorg
  never places a load in a delay slot.

## Dispatch and names

The preparation stage is `Model_PrepareSharedPrimitiveCaches`, formerly
`func_80067220` at NA `0x80067220`, size `0x134`. Only group `0x01000000`,
kind zero selects it. It is not one of the polygon draw stages.

The sixteen renderers use `Model_DrawShared`, optional `TwoSided`, optional
`Semi`, `Gt3` or `Gt4`, optional `WindowRecord`, then `Primitives`.

| Packed group | Four callbacks, in table order | Distinction |
| --- | --- | --- |
| `0x01000000` | GT3, GT4, GT3 window, GT4 window | Base template-controlled blend |
| `0x01100000` | Same four formats | Omit NCLIP area rejection |
| `0x01200000` | Same four formats | Force command semi bit `0x02000000` |
| `0x01300000` | Same four formats | Combine both features |

The kinds are `0xD`, `0x15`, `0x20D` and `0x215`, respectively. The selector
and base mapper keep their original switch order, address casts, sentinel,
reverse registry search and unchanged-input fallback. The mapper accepts
a registered callback value, not a packed key.

## Preparation is projection and lighting

The descriptor's first range supplies count, input index and output index
for eight-byte vector/cache entries. RTPS projects each input. The output
depth word is first initialized to `-1`; accepted projections store SXY2
in word zero and SZ3 in the low half of word one, with IR0 in its high half.
Draw mode one bypasses the GTE FLAG rejection; other modes preserve it.
The pipelined read-ahead is unchanged.

When the current draw mode is nonzero, a second descriptor range prepares
normal colors with NCCS and the RGB portion of `gModel_dwPolyGt3Template`.
Both ranges advance entries by eight bytes. Nonzero
`gModel_dwLitColorCacheBase` selects offset four in a color entry rather
than acting as the output address here.

The result is the descriptor pointer plus `0x1C`, not the draw-stage
header-plus-eight result. This stage has no renderer scratch-save contract.
Its existing classification evidence includes immediate-AND expansion
through `$at`, software pipelines and packed IR0/SZ3, not GTE presence alone.

## Draw stages consume caches

All sixteen drivers load cached SXY/depth and cached lit-color words.
They do not execute RTPT/RTPS, lighting or depth-cue color operations, and
none reads GTE FLAG. Projection FLAG rejection belongs to preparation.
The selected polygon templates establish GT3/GT4 output independently of
the names in the dispatch table.

The base GT3 record stride is `0x18`, GT4 `0x1C`. Window-record forms consume
an extra four-byte word and submit the existing E2 texture-window/reset
state. These are not the similarly named wireframe callbacks, which merely
skip that extra input word without emitting E2.

Base triangle drivers execute one NCLIP and base quad drivers two. The
two-sided forms omit these area tests, not sentinel or depth checks; no
normal reversal is inferred. Semi forms force command bit `0x02000000`.
Their additional-pass setup still checks `gModel_bBlendRateActive`,
descriptor TPAGE bits `0x00600000` and `gModel_dwSecondPassClutBits`, and can
select a semi template and descriptor-derived CLUT. Base templates can
already blend, so the absence of `Semi` does not guarantee opaque output.

All sixteen save and restore `$s0..$s3` at caller offsets `0x28..0x34`.
The eight window-record forms additionally save and restore `$s4/$s5`
at `0x38/0x3C`. A zero count or draw mode one reaches the common
restore/cursor/return footer without submitting polygons. Every FIFO write,
sentinel check, cache width/slot, packet/OT link, packet cursor store,
additional-pass branch and header-plus-eight return is preserved.

## Declaration and native compatibility

All seventeen existing declarations are `int name()` with unspecified
parameters, including the preparation stage. The frozen explanations in
`src/unmatched.h` record the earlier correction from incorrect `void(void)`
forms and why a typed pointer/result contract is not established. The new
isolated header preserves that exact declaration shape. It does not copy
the direct family's thirty address-use `void(void)` declarations or its two
typed fog-delegation exceptions.

Native boundaries come from the original root bindings and inventory, not
NA suffix arithmetic. For example, preparation is NA `0x80067220`, Japan
`0x80065BDC`, SLES-03947 `0x80066520`, and the other four PAL images
`0x80065164`. Original logical bindings remain available. The 68 private
native spellings occur only in the four PAL function inventories; no extra
C, header, tool or overlay-import consumer needs rewriting.

Existing headers, overlay C/imports, profiles, manifests, tests, historical
ledgers and traces remain unchanged. The accepted complete semantic ledger
is retained before seventeen new rows. The already accepted strict legacy
alias policy retains frozen numeric declarations without any new policy
exception or validator/test change.

Complete caller compiler/MASPSX and fallback-assembly comparisons reverse
only the seventeen identities. No instruction, register, directive or
filename is filtered. Final clean resident matching and fresh applicable
PR image checks remain required; no local cloud/full regional/overlay
suite is used.
