# French MODEL423 four-funnel renderer

MODEL385 stages 7/8 contain the independently recovered 1,472-byte helper
at image `+0x3214`, `func_8013E214` / `func_8017E214`. The existing
`gcc_2_8_1_g0_split` profile uses GCC 2.8.1 and MASPSX 2.81. Both complete
20,480-byte images match their original hashes with real sized C owners.
The entry, other helpers, raw suffixes, archive slices and resident bindings
are unchanged. This adds two C instances and 2,944 bytes.

## Behavior and accessed layout

Four 0x118-byte records at context `+0x15B4` each hold two seventeen-point
vector rows at record `+0/+0x88`, a size word at `+0x110`, and an opaque tail.
The renderer emits sixteen quads per record using one GT4 at `+0x1B60`.
All depth and projection-flag tests remain nonnegative.

Sheet one of the two accepted `ModelVariantSheet` views at `+0xE1C` gives
the frame-parity flicker. The inner radius is `rsin(1300) *384 >>12`;
the outer radius adds 160 and flicker, with y `-(flicker+512)`. Matrix y
translation subtracts both `rcos(1300) *384 >>12` and `rsin(276) *384 >>12`.

Funnel zero scales all axes by the common size at `+0x1DB8`. Later funnels
scale x/z by common size plus nonnegative local size, retaining common
y scale. After the existing phase-five global colour fade, each byte
component subtracts its signed `component * local_size /8192` contribution.
This second attenuation is independent of the global fade.

Local sizes advance by `step <<7` and wrap at 8192, even when common size
is nonpositive and rendering is skipped. Spin advances by `step *80`.
Position is three halfwords at `+0x1D58`, direction three words at `+0x1D60`,
frame `+0x1D8C`, step `+0x1D98`, fade `+0x1DC8`, inner/outer colours
`+0x1DF0/+0x1DF4`, spin `+0x1DF8`, phase `+0x1E0C`.
These local views establish a minimum accessed extent, not allocation capacity.

## Source, experiments and acceptance

`src/overlays/french_model_variant/variant423_funnel.{c,h}` uses independently
measured declarations and the common rendering sequence of the accepted
Spanish `variant426_funnel.c`. The slot-one wrapper changes only the function
symbol. No shared regional body, header or compiler profile changes.

The accepted template's empty pointer-initialization loop and unused angle
calculations are preserved: removing the latter loses the original delay-slot
addition. No register declarations, volatile values, inline assembly or
fabricated memory operations are used.

Candidate sources and detailed comparison logs remain under
`tmp/fr-probe/u385/`. All experiments use the existing split profile:

| Experiment | Bytes | Different words |
|---|---:|---:|
| Initial four-funnel recovery | 1460 | 361 |
| Hoisted negative height | 1424 | 358 |
| Separate first/later scale branches | 1468 | 360 |
| Preserve accepted template angle calculations | 1472 | 60 |
| Load spin before radius setup | 1472 | 58 |
| Direct reach expression | 1472 | 3 |
| Bias plus radius plus constant | 1472 | 5 |
| Constant plus radius plus bias | 1472 | 3 |
| Bias plus parenthesized radius/constant | 1472 | 2 |
| Two-stage reach assignment | 1472 | 59 |
| Reused scale temporary for base radius | 1472 | 9 |
| Radius plus parenthesized bias/constant | 1472 | 0 |
| Radius plus parenthesized constant/bias | 1472 | 0 |
| Block-local base radius | 1472 | 0 |

The last two differing instructions were not merely load order. They selected
which summand received the constant addition. The natural expression
`radius + (bias + 160)` reproduces the original operand selection without
masking relocations or adjusting assembly.

The subsequently measured French MODEL425 sibling selects the same body through
`MODEL_VARIANT425_FUNNEL`: radius multiplier 512 instead of 384, a shorter
sheet-to-funnel opaque interval, and four extra opaque bytes before the palette.
The default MODEL423 view and all original instruction bytes are preserved.
Two replay rows append the new shared source/header fingerprints without
rewriting the original terminal evidence. See the
[MODEL425 funnel notes](french-model-variant425-funnel.md).

Both slot bodies were recompiled and linked at their actual addresses. After
the production full-image gate, each object defines a 0x5C0-byte C function
and the final ELF defines it at `0x8013E214` / `0x8017E214` without a helper
`.NON_MATCHING` owner. The
[terminal ledger](french-model-variant423-funnel-attempts.csv) records source
and header fingerprints. Dedicated tests cover registrations, native
target-compiler offsets/size, wrapper shape, signed attenuation, complete
hashes, object ownership and linked symbols.
