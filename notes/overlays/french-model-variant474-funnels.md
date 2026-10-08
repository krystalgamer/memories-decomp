# French header-474 paired funnels

The `0x688` helper at image offset `0x1738` is matching C in four French images:
MODEL121 stages 7/8 and MODEL431 stages 9/10. Slot 0 owns `func_8013C738`;
the rename-only slot-1 wrapper owns `func_8017C738`. Both use the supported
`gcc_2_8_1_g0_split` pipeline, GCC 2.8.1 / MASPSX 2.81.

This is a first-ever body recovery. The current seven-release overlay
registrations had no matching-C owner for its J/JAL-masked body. Four matching
resident functions of the same extent were separately hashed and excluded;
the remaining resident inventories had no same-sized candidate. The entry's
call at `+0xC24` targets this helper and passes its context in the delay slot.

## Measured layout and behaviour

Two `0x118`-byte records at context `0` contain inner/outer rows of seventeen
`SVECTOR`s and a size word at `0x110`. The packet is a `POLY_GT4` at `0x1964`.
The target, direction, frame, step, inner/outer colours, spin and side selector
are at `0x1B5C`, `0x1B64`, `0x1B90`, `0x1B9C`, `0x1C00`/`0x1C04`,
`0x1C08` and `0x1C34`.

The partial trigger view is anchored at `context + 0x230`; only its signed word
at relative `0x264` (absolute `0x494`) is interpreted. The declaration is a
measured local view, not a claim about a complete owning record or allocation.

Active records build rings at radii 64 and 256, with opposite outer-row z
offsets driven by phases 2048/4096. Side selection mirrors the yaw by 192 units
and retains both transform arms. Frame parity selects a signed-short flicker
of 0 or 1024, promoted to the word-sized scale bias. The real discarded
`rsin(2048)` call is preserved.

Sixteen quads per active record retain signed depth/flag gates. Above size 6144,
both colour rows attenuate by `component * (8192 - size) / 2048`. Record sizes
advance by `step << 8` only when the trigger is **strictly above** 900 for
record zero or 1100 for record one, and clamp above 8192. Spin advances by
`step * 80` even when rendering is skipped.

## Source and acceptance

The local body and declarations were recovered from French retail code.
Accepted neighbouring helper structures were used only as structural evidence;
no foreign compiler claims, flags or guessed reference types are imported.
There are no register pins, inline assembly, volatile workarounds, artificial
stores or empty initialization loops.

Scalar declaration order follows the measured spill slots. The explicit parity
branch, short flicker and word copy retain the two original narrowing operations.
Comma-separated initializers of the actual two-record loop retain the phase
constant's argument-register allocation without inventing dependencies.

Before tracked promotion, temporary C-owned relinks reproduced all four complete
`0x5000`-byte images and verified `STT_FUNC` input/final owners of size `0x688`
at `0x8013C738`/`0x8017C738`. The terminal ledger pins source/header hashes and
the full-image hashes. Normal production rebuilds and regressions independently
check these owners.

Only the four existing `+0x1738` registrations change. The entry, other assembly
helper, four existing C helpers, resident bindings and `0x38BC..0x5000`
unclassified tails remain unchanged. This contributes four matching-C instances
and 6,688 bytes without creating new physical registrations.
