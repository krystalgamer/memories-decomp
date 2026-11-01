# French MODEL image 12048 strands helper

`func_8013BF38` (image offset `0xF38`, 0xC78 bytes) in the French and
Spanish `model_image_model_12048_8013b000` images draws a set of swaying
strands. The same body sits at `func_8017BF38` in the
`model_image_model_12058_8017b000` images. Both slots compile
`image12048_strands.c` itself; it defines `FrenchModelVariant403_DrawStrands`,
which each image binds at its own address, `0x8013BF3C`/`0x8017BF3C` in the
North American and Japanese header-386 images (see [Per-image symbol
bindings](model-variants.md#per-image-symbol-bindings)).

- **Relative.** It closely follows the
  [12048 chains helper](french-model-image12048-chains.md). It uses
  seven-joint, 0x180-byte strands, and the timing record's count sets how
  many strands it draws.
- **Shape.** The angle comes from `ratan2(target[2], target[0])`, and the
  second `ratan2` result is discarded, as in the chains helper. While the
  phase is positive, each strand spreads its joints in sixths of a per-strand
  reach vector. The joints bend by a per-strand tilt, and the inner joints
  also follow a twist and sway wave. Each strand is projected through its
  own origin before the next strand is built. The draw loop picks quad pairs
  0, 2 or 4 from the strand's mode and alternates the pair with the frame
  parity. Phase 1 grows a strand's length to six, raises its fade to 0x400
  and steps its mode. Phases 2-3 hand over after the last strand, and phase
  4 onward regrows the length. Phase 5 also shrinks the edge width over the
  timing span. The view lives in `french_model_variant/image12048_strands.h`.
- **Register choice.** The loop-1 and loop-2 forms follow the chains helper.
  The four spilled locals are declared in stack order (sway, offset, tilt,
  angle). The fade step is written as `step * 128`: with `step << 7`, GCC
  narrows the step load to `lhu`.
- **Bindings.** Calls use the current resident layout, so all four images
  reuse their region's `model_variant472_linker_symbols.txt`.

All four images rebuild exactly. The
[attempt ledger](french-model-image12048-strands-attempts.csv) records the
probes.
