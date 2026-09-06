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
| `0x8008E590` | `rand` | Preserved Psy-Q SDK assembly |
| `0x8008E5C0` | `srand` | Preserved Psy-Q SDK assembly |
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

### Reverse-stepping a captured state

The multiplier `0x41C64E6D` is odd, so it has a multiplicative inverse modulo
`2^32`:

```text
0x41C64E6D * 0xEEB9EB65 = 1 mod 2^32
```

A trace that captures the full `gRand_dwSeed` value can therefore step
backward exactly:

```text
previous = (state - 0x3039) * 0xEEB9EB65 mod 2^32
```

This is useful for aligning traces taken after a suspected random-consuming
event or checking how many calls separate two captured states. A returned
15-bit `rand()` value is not enough to reverse the stream by itself because
the return mask discards most of the updated state.

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
| `func_8016A930` | Builds the new-game starter deck from seven weighted rows |
| `main_run_frontend_menus.c` | Advances randomness while the main menu runs |

Additional matching and unmatched callers use the same SDK routine for duel
state, animation timing, and selection logic.

`Main_RunMenu` consumes exactly one value on every handler invocation. Its
one-time screen initialization runs first, followed by the unconditional
`rand()` call and then the overlay selection poll. The invocation that
receives a completed selection still consumes that value before it starts the
fade and mode transition. This establishes a call-order invariant, not by
itself a frame-rate invariant: a timing model must still show how often
`Main_Loop` dispatches the menu handler.

Modulo expressions such as `rand() % divisor` are biased unless the divisor
evenly divides 32768. Analyses that predict deck, AI, or drop outcomes must
reproduce that exact operation rather than scale the return value.

`AiScript_SetRandom` reads its lower bound, upper bound, and destination index
before consuming one RNG value, then stores
`rand() % (upper - lower + 1) + lower`. For a valid inclusive range of width
`N`, write `32768 = qN + r`: the first `r` values starting at the lower bound
occur `q + 1` times each, while the remaining values occur `q` times each.
The result is uniform only when `N` divides 32768. The handler does not
validate or reorder the bounds, so AI scripts are responsible for supplying a
nonempty range in ascending order.

### Range transformations

Because `rand` returns each value from 0 through 32767, power-of-two masks
preserve an even distribution while other modulo operations generally do not:

| Expression | Output range | Distribution |
|---|---:|---|
| `rand() & 0xFF` | 0-255 | Each result has 128 source values |
| `rand() & 7` | 0-7 | Each result has 4096 source values |
| `rand() & 3` | 0-3 | Each result has 8192 source values |
| `rand() & 1` | 0-1 | Each result has 16384 source values |
| `rand() % 100` | 0-99 | Results 0-67 occur 328 times; 68-99 occur 327 times |

Adding a constant after a power-of-two mask only shifts that uniform range.
For example, `func_80037A58` applies `(rand() & 7) - 4` and
`(rand() & 3) - 2` to two saved coordinates. The first result is uniformly
distributed from -4 through 3 and the second from -2 through 1. When the
update flag is set, the function consumes two consecutive values in that
order; reproducing only the final offsets is not enough to preserve the
subsequent stream position.

`func_8003B378` and `func_80039F44` each initialize a delay with
`(rand() & 0xFF) + 0x3C`. That produces every value from 60 through 315 with
equal frequency. The destination is a signed 16-bit field, so no truncation or
sign wrap occurs for this range.

`func_8002712C` conditionally stores `rand() & 1` in the play-command record,
producing an unbiased zero-or-one value. It consumes no RNG value when
`Duel_CollectFieldCardsByType` finds no candidate entries, and it also skips
the call when `func_80026C0C(DUEL_FIELD_SIDE_ZONE_COUNT)` returns a negative
index. Only the path with both a nonempty candidate list and a nonnegative
index advances the stream,
after the record's other fields have already been initialized. Deterministic
duel traces must reproduce those branch outcomes, not assume that every
invocation of `func_8002712C` consumes a value.

`func_8004149C` treats its data argument as a count byte followed by that many
little-endian 16-bit offsets. It consumes one RNG value, selects
`rand() % count`, clears object halfword `+0x58`, and sets object pointer
`+0x50` to the base pointer at `+0x54` plus the selected offset. The call to
`rand` occurs before the count is used, so even a malformed zero-count table
advances the stream before the generated signed-division guard traps. For a
valid count `N`, the same `32768 = qN + r` modulo bias applies: entries
`0` through `r - 1` have one more source value than the remaining entries.

The drop selector is also uniform: masking with
`DUEL_DROP_WEIGHT_TOTAL - 1` produces 0-2047 exactly 16 times each before the
code adds one. By contrast, `AiScript_JumpRandom` uses `% 100`, so probability
thresholds from 1 through 99 are slightly more likely than their nominal
percentage because the extra source values are concentrated at results 0-67.
Seed tools and traces should preserve these integer transforms instead of
substituting floating-point probabilities.

### Starter-deck stream consumption

The matching starter-deck generator `func_8016A930` does not consume one RNG
value per card. Each selection attempt first computes
`(rand() & 0x7FF) + 1`, then scans zero-based card indices `0`-`719`. Before
adding each examined weight to the accumulator, it calls `rand()` once more
and discards that result. Selecting card ID `n` therefore consumes `n + 1`
values on that attempt: one threshold value and one discarded value for each
scanned ID through `n`.

The generator also tracks how many copies of each card have already been
dealt. A selection that would exceed three copies is discarded and the draw
is retried, consuming another threshold and variable-length scan. A threshold
that is not reached within the first 720 weights consumes 721 values but adds
no card. In the retail tables the final two weights are zero and each reachable
row still totals 2048, so the stock path fills all 40 slots; an edited row can
produce a short deck if its first 720 weights total less than 2048.

Starting-deck prediction must reproduce these discarded calls and redraws.
The selected cards determine how far the stream advances, so the deck and the
RNG state immediately after generation cannot be modeled independently.

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

`tools/trace/rng_boot_timing.lua` implements the boot-through-title slice of
that trace. It arms on `srand(0x55555555)`, records each `rand`/`srand` caller,
state transition, mode, and VSync frame through the first three seconds of
mode 8, and prints the human-context scaffold. It requires PCSX-Redux's
interpreter CPU because it uses execution breakpoints. No result has been
submitted yet, so the community timing observations above remain unconfirmed.

## Research checklist

1. Name the SDK functions only after confirming the Psy-Q signature evidence.
2. Inventory every caller of `0x8008E590` and every reseed call to
   `0x8008E5C0`.
3. Run `tools/trace/rng_boot_timing.lua`, then extend the trace through name
   entry to locate the exact starting-deck consumption range.
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
