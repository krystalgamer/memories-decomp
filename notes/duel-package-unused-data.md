# Duel package negative-space audit

## Scope

This note resolves two regions previously left as generic unknowns:

- terrain-package phase 11, the `0x2800` bytes loaded to `0x80100000`;
- the final `0x68` bytes of every `0x1800`-byte per-duelist block.

The conclusions are deliberately different. Phase 11 contains structured,
non-fill data but no byte consumer has been established. The duelist tail is
uniform `0xFF` fill, ends exactly at a sector boundary, and has no addressable
consumer.

Evidence comes from the verified North American `WA_MRG.MRG`, exact-matching
resident source, the extracted duel overlay, and direct MIPS address scans.
No emulator trace is involved.

## Terrain-package phase 11

`Duel_LoadPackageStage` phase 11 copies five sectors from package-relative
`+0x63000` to the arena pointer at `D_80010000`, whose first value is
`0x80100000`:

```text
relative sectors: 198-203
size:             0x2800
RAM:              0x80100000-0x801027FF
```

All seven terrain packages carry the same bytes. The extracted phase has
SHA-256:

```text
4d7c12766dec03a2d8faca71801dbc336db55144d52b2aad2f95efc263a2e237
```

The only duel initialization call that appears to hand this arena to another
routine is:

```c
func_80056250(2, D_80010000[0].payload_bases[0], 0x63000, 4);
```

That call does not consume the phase. Matching `func_80056250` checks whether
its second argument is null, then operates entirely on model slot 2. It never
dereferences that argument and never uses the `0x63000` or `4` arguments.
Thus the loaded pointer enables the model-slot layout pass, but the pass does
not read the loaded bytes.

A MIPS scan of the complete `0x80146000` duel overlay found no instruction
forming or accessing an address in `0x80100000-0x801027FF`, and no aligned
pointer literal into that range. The exact `0x2800`-byte payload occurs at
the same phase in each of the seven terrain records and nowhere else on a
sector boundary in WA. Its first `0x80` bytes also do not occur in `MODEL.MRG`
or `SU.MRG`.

The defensible conclusion is therefore:

- the transfer is real and terrain-invariant;
- the bytes are structured rather than zero/`0xFF` fill;
- the current retail path uses only the destination pointer's non-null value;
- no semantic payload name is justified without a byte consumer or external
  format evidence.

## Per-duelist `0xFF` tail

Each per-duelist record occupies three sectors (`0x1800` bytes):

| Offset | Size | Contents |
|---:|---:|---|
| `+0x0000` | `0x5B4` | deck weights |
| `+0x05B4` | `0x5B4` | S/A POW drop weights |
| `+0x0B68` | `0x5B4` | B/C/D drop weights |
| `+0x111C` | `0x5B4` | S/A TEC drop weights |
| `+0x16D0` | `0xC8` | ten rank-rule rows, five threshold/change pairs each |
| `+0x1798` | `0x68` | sector-end `0xFF` padding |

The four weight tables end at `+0x16D0`. The rank table is exactly
`10 * 5 * 4 = 0xC8` bytes, ending at `+0x1798`. The next loaded global,
`gDuel_awRitualData`, begins at `0x801799D8`, exactly `0x68` bytes after the
rank table's RAM end at `0x80179970`.

All 40 indexed duelist blocks have byte-for-byte identical tails:

```text
size:       0x68
contents:   FF repeated 104 times
SHA-256:    f71c209c8df8eeb07005a11966d0300bd86d153006ba2e05302caf822246d90f
```

The ten callers of `Duel_CalcRankScoreChange` use rule constants 0 through 9,
and the shared row type fixes five four-byte entries per rule. No rank path
therefore reaches beyond `+0x1797`.

Adjusted-high address scans of resident text and the duel overlay found no
instruction access or address formation in
`0x80179970-0x801799D7`, and neither image contains an aligned pointer literal
into the range. Combined with the uniform fill and exact sector boundary,
this establishes padding rather than an unresolved table.

## Reproduction

The tracked archive boundaries make both checks reproducible without an
emulator:

```sh
# Phase 11 of terrain zero.
dd if=game/DATA/WA_MRG.MRG bs=2048 skip=$((5830 + 198)) count=5

# Forty three-sector duelist records.
dd if=game/DATA/WA_MRG.MRG bs=2048 skip=7475 count=120
```

Derived files belong under `tmp/` and are not repository sources.
