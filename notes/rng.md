# Random-number generation

Forbidden Memories uses the Psy-Q C runtime `rand` and `srand` routines rather
than a game-specific core generator. Community speedrunning research documents
how deterministic seed timing affects starting decks, duel AI, and rewards.
This note separates facts visible in the executable from behavior that still
needs runtime tracing.

## Runtime implementation

The resident functions are:

| Address | Role | Repository status |
|---|---|---|
| `0x8008E590` | Psy-Q `rand` | Preserved SDK assembly |
| `0x8008E5C0` | Psy-Q `srand` | Preserved SDK assembly |
| `0x800FE6F8` | PRNG state | `gRand_dwSeed` |

`rand` updates the 32-bit state with the standard ANSI C linear congruential
constants:

```text
state = state * 1103515245 + 12345
return (state >> 16) & 0x7FFF
```

The multiplication and addition use the low 32 bits of the MIPS result. The
observable return value is therefore 15 bits even though the stored state is
32 bits. This distinction matters when reproducing or searching a seed stream:
the community description calls the generator modulus `2^31`, while the
executable stores the full wrapped 32-bit recurrence and masks only the
returned value.

`srand` directly stores its argument into `gRand_dwSeed`.

## Seeding

`Main_Init` calls `srand(0x55555555)` at `0x80012C44`. This confirms the
community-reported boot seed. It is not safe to assume that value remains the
active seed through all startup phases: `func_80013154` later calls
`srand(0x56)` at `0x80013338`. Any frame or route model must account for all
reseed calls as well as ordinary `rand` calls.

## Known game consumers

The generator feeds multiple systems. Confirmed matching-C examples include:

| Consumer | Use |
|---|---|
| `Rand_GetInterval` (`0x800358FC`) | Returns `rand() % divisor` |
| `AiScript_JumpRandom` (`0x80070C60`) | Chooses whether a scripted branch is taken |
| `AiScript_SetRandom` (`0x80070E20`) | Writes a value in a scripted inclusive range |
| `duel_rewards.c` | Selects post-duel rewards |
| `main_run_menu.c` | Advances randomness while a menu mode runs |

Additional matching and unmatched callers use the same SDK routine for duel
state, animation timing, and selection logic. Search for calls to
`func_8008E590` until the SDK symbol itself is promoted to `rand`.

Modulo expressions such as `rand() % divisor` are biased unless the divisor
evenly divides 32768. Analyses that predict deck, AI, or drop outcomes must
reproduce that exact operation rather than scale the return value.

## Community timing observations

The supplied speedrunning write-up reports:

- RNG usually advances once per frame.
- Some menus and the intro do not follow that rule.
- Duel progression is driven by actions rather than continuous frame
  advancement.
- The starting deck is selected after the final name-entry text box.
- Identical deck, seed, and duel inputs reproduce AI decisions and the final
  card drop.
- Historical routes targeted starting-deck frame 14810 and Heishin duel frames
  640 or 642 to manipulate Meteor B. Dragon drops.

These are useful hypotheses and route observations, not executable-level proof.
The broad "once per frame" model in particular should be replaced with a call
trace: record every `rand` and `srand` invocation, caller address, old state,
new state, return value, game mode, and frame counter. That trace can identify
which menus or duel actions consume values and explain apparently exceptional
timing without relying on visual frames alone.

## Research checklist

1. Name the SDK functions only after confirming the Psy-Q signature evidence.
2. Inventory every caller of `0x8008E590` and every reseed call to
   `0x8008E5C0`.
3. Trace boot through name entry to locate the exact starting-deck consumption
   range.
4. Trace a deterministic duel and separate AI, animation, and reward
   consumption.
5. Compare emulator frame counts against the PRNG call index; do not treat them
   as interchangeable.

## Sources

- Community RNG manipulation write-up:
  <https://gist.github.com/anonymous/542a280f999d3f7bece2a0fe3569d474>
- Retail executable assembly at `0x8008E590`, `0x8008E5C0`, `0x80012C44`, and
  `0x80013338`
- `tmp/references/ram_map.txt` for the seed symbol evidence

