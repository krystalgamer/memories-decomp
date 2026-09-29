# Complete Spanish duel effect 15

`func_8014FF40` at `8014FF40..801503F8` is a complete 1,208-byte C
translation unit selected by dispatcher effect 15. It uses
`gcc_2_8_1_g0_split`, independently recovered local layouts and canonical
resident declarations. The independent 90,112-byte bank and production Spanish
overlay image both reproduce SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.

Based on accepted master through `9b68de492`, this addition retains all 65
accepted Spanish manifest entries verbatim and reaches 66/85 C functions,
28,356 C bytes and 19 explicit assembly boundaries. Pending effects 2/21 and
8/12 are not stacked into this branch or counted in these totals.

## Real layouts and declaration owners

The work view is `0x14` bytes: configuration pointer at `+0`, unsigned
halfword count/scale/state/visibility/timer/cross-counter at `+4/6/8/A/C/E`
and color at `+10`. Configuration stride is eight bytes: color followed by
the signed word selector. Eight records at `8015AC08` occupy exactly 64 bytes.
The initial scale vector at `80146168` retains its 16-byte owner.

The shared output at `8015B7A0` is 21 `u32` words, ending exactly at the
accepted ordering-table pointer `8015B7F4`. Its 84-byte generated data owner
is verified independently; it is not an absolute alias. The canonical
collectors already document their output as zero-terminated object-address
words. Twenty objects plus a terminator fit the general negative-selector
path; the actual eight retail selectors are
`3, 2, 14, 1500, 9, 18, 12, 999`.
Their matching/front-row or sentinel/back-row paths each emit at most five
objects plus the terminator.

No private card-object structure or conflicting collector prototype is added.
The routine includes `duel_card.h` and `display_object.h`, and casts each
returned address to the canonical `DisplayObject` before reading its
`30/32/34` halfwords. Target-GCC layout checks cover those offsets as well as
every work/configuration field and the output-array size.

The Spanish resident bindings are the established
`Duel_CollectFieldRowCardObjects` (`8002CB0C`),
`Duel_CollectMatchingFieldCardObjects` (`8002CB88`) and
`Model_GetLightSourceMatrix` (`8005C328`).
Texture selection uses pair 13 of the complete accepted 21-pair texture union.

## Preserved lifecycle

Phases below eight select the corresponding configuration; higher
nonnegative phases use the 180-update crossed-line fallback. Normal rendering
keeps the frame-step getter call even though its return value is unused,
the frame-step override, captured world matrix, shrinking scale and
scale-dependent UV rectangles.

Every collected card deliberately receives two identical textured-quad
submissions. This is not deduplicated. Color transition, visibility blinking
and final fading remain distinct. The timer increments only when state is one
and its prior value is below 16. Reaching 16 toggles visibility in that update;
the state-two transition occurs on the following update, not immediately.
Both independent completion paths retain the resident completion-byte write.

## Experiments and ownership proof

The initial compile exposed a missing include for the existing completion-byte
declaration; the canonical header was added rather than redeclaring the global.
The first compiled candidate was 1,204 bytes with 43 differing words.
Delaying work-pointer assignment until after local initialization and using
the original halfword remainder expression (`timer % 4`, rather than `& 3`)
retained the target's separate halfword truncation and produced all 1,208 bytes
with zero differences. No inline assembly, forced register or new compiler
option was needed.

Promotion followed a complete independent bank link. Production checks verify
all accepted C object/final-ELF extents and executable types, nine real overlay
callees, and the exact bytes, sizes and unique input-object storage of all
three new data symbols. Candidate snapshots and receipts remain local under
`tmp/`.

Spanish duplicate-copy verification, repository metadata policy, the accepted
dispatcher/texture ownership gate and all 106 duel-focused tests pass. No
cross-region C registration or shared executable code changes are included.
