# Complete Spanish duel effect 1

`func_80157794` at `80157794..80157E10` is a complete 1,660-byte C
translation unit, selected by dispatcher effect 1 and built with
`gcc_2_8_1_g0_split`. Independent full-bank and production Spanish overlay
links reproduce all 90,112 bytes and SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.

Together with effect 15, this branch adds 2,868 exact C bytes while retaining
all 65 accepted Spanish manifest entries verbatim. It reaches 67/85 C
functions, 30,016 C bytes and 18 explicit assembly boundaries. Pending effects
2/21 and 8/12 are not included in these figures.

## Recovered layout and real storage

Target-GCC layout verification fixes the work view at `0x484` bytes:

| Offset | Field |
| --- | --- |
| `000` | Configuration pointer |
| `004` | 12 rotation vectors |
| `064` / `264` | 64 particle positions / velocities |
| `464` / `466` | Rotation step / stage halfwords |
| `468` | Unsigned growth accumulator |
| `46C` / `470` | Independent frame / tick words |
| `474` | Cross-effect counter |
| `476` / `47A` / `47E` | Base / beam / screen colors |

The two configuration records at `8015B420` are 24 bytes each: color at `+0`,
initial rotation step / acceleration / beam step at `+4/6/8`, particle speed
at `+A`, sprite size at `+C`, two widths at `+E`, height at `+12` and the two
frame thresholds at `+14/16`. Both retail records contain initial rotation 16,
acceleration 16, beam rotation 32, particle speed 16 and thresholds 32/80.
The fixed initialization and rendering counts fit the 12- and 64-element views.

The 48-byte configuration table and 16-byte initial vector `80146208` have
unique generated data-object owners with exact sizes, types, addresses and
retail bytes. They are not absolute data aliases. Production verification also
checks all C object/final-ELF extents and all 16 distinct overlay callees.

## Lifecycle and retained details

The two ordinary positive phases select their configuration and apply the
frame-step override during initialization. Larger phases use the crossed-line
fallback and its 180-update counter.

Rendering preserves the initial base-color expansion, the two frame thresholds,
beam-color transition and growth, twelve independently rotated strips, later
64-particle movement and separate screen flash. The frame accumulates the
captured resident frame step while the tick increments by one. Completion
requires all three colors to finish; the screen path separately marks the
canonical request's `field_1D`.

The loop deliberately retains a quarter-color copy even though the strip
submission uses the original beam color. The doubled-tick multiplication keeps
the explicit shift before multiplication, preserving the original instruction
operands without forced registers or inline assembly.

## Measured experiments

The first candidate was 1,656 bytes with 348 differing words. Moving the
rotation-step initialization before the independent counter clears retained
the target load-delay instruction, yielding 1,660 bytes with three differences.
The remaining source-expression experiments were:

| Doubled-tick expression | Differing words |
| --- | ---: |
| `rotation_step * (tick * 2)` | 3 |
| `(tick * 2) * rotation_step` | 2 |
| `(tick << 1) * rotation_step` | 1 |
| `rotation_step * (tick << 1)` | 0 |

Every compiled experiment uses the same named profile and has a local candidate
snapshot/fingerprint. The terminal instruction result was followed by a complete
independent bank proof before promotion, then target-GCC layout, real-data,
callee and accepted-entry checks on the production build.

The combined effect-15/1 batch also passes Spanish duplicate-copy verification,
repository metadata policy, accepted dispatcher/texture ownership and all 109
duel-focused tests. Shared executable sources and other regional registrations
are unchanged.

## Accepted effects 8/12 integration

After additive rebase onto accepted `3d43de926`, all 67 accepted Spanish
manifest entries and the reviewed lifecycle sources remain unchanged.
The combined branch has 69/85 C functions / 32,744 bytes,
16 explicit assembly boundaries and 193/209 configured C
instances / 88,596 bytes. Accepted effect-8/12 sources, helper
contracts, bindings, configurations and tests are retained.

## Accepted effect 9 integration

Additive rebase onto accepted `308ad56d2` preserves all 68 accepted Spanish
entries, effect 9's signed-number behavior, real owners and tests.
The previously reviewed lifecycle source/header files remain byte-identical.
The combined branch has 70/85 bank C functions / 34,540 bytes,
15 explicit assembly boundaries and 194/209 configured C
instances / 90,392 bytes. Complete production Spanish images,
linked ownership, metadata and duel regressions are checked on this head.

## Accepted effects 4/5/10 integration

Additive rebase onto accepted `2ef4aaba9` preserves all 71 accepted Spanish
entries, including every accepted lifecycle, real owner and test.
The previously reviewed lifecycle source/header files remain byte-identical.
The combined branch has 73/85 bank C functions / 41,576 bytes,
12 explicit assembly boundaries and 197/209 configured C
instances / 97,428 bytes. Complete production Spanish images,
linked ownership, metadata and duel regressions are checked on this head.
