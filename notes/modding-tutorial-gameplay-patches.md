# Modding Tutorial Gameplay Patches

This note verifies gameplay-related patches from the community tutorials
attached to issue #368. Tutorial file offsets are mapped back to resident code
before their claims are accepted.

Confidence levels:

- **Confirmed**: retail bytes, disassembly, and matching C or independently
  recovered symbols agree.
- **High**: the patch site is established, but some surrounding semantics are
  not independently named.
- **Tentative**: the claimed effect depends on an external editor or modified
  input that is not present in the verified retail target.

## Deck copy limits

**Tutorials:**

- `Alterar Limite de Cartas - Por Jael Rivera.txt`
- `Tirar o Limite de Cartas.txt`

The tutorials identify two SLUS offsets in the deck editor:

| SLUS offset | VRAM | Retail bytes | Instruction | Purpose |
|---:|---:|---|---|---|
| `0x24020` | `0x80033820` | `01 00 03 24` | `addiu $v1, $zero, 1` | Exodia-piece copy limit |
| `0x24070` | `0x80033870` | `03 00 42 2C` | `sltiu $v0, $v0, 3` | General per-card copy limit |

Both instructions are in `func_800336F0`, whose exact matching C establishes
the surrounding behavior. The function obtains the selected card ID in `r`
and begins with a permitted-copy count of one:

```c
c = 1;
if ((u32)(r - 0x11) < 5) {
    c = (p + r)[0x5AC4] < c;
}
```

Card IDs `0x11` through `0x15` are the five Exodia pieces. At `0x80033820`,
the immediate `1` is the right operand of the unsigned comparison between the
current deck count and the special Exodia limit. Changing the immediate
therefore changes the maximum number of each Exodia piece accepted by this
path.

For all accepted card IDs, the later condition includes:

```c
q[0x5AC4] < 3
```

The `3` compiles to the `sltiu` at `0x80033870`. Changing its immediate changes
the general per-card copy limit. The tutorial's suggested byte `0x40` means
decimal 64, not decimal 40; because a deck contains only 40 cards, either
limit is effectively nonbinding for a valid deck. To set a literal limit of
40, the immediate byte would be `0x28`.

The tutorial also instructs users to zero 52 bytes at file offset `0x1BFB24`.
Those bytes are already zero in the unmodified North American retail
executable. That step is therefore a no-op on the verified target and cannot
be accepted as a retail limited-card table. It may repair data moved by a
third-party card editor, but that editor-specific interpretation needs a
modified input demonstrating the before/after layout.

**Confidence:**

- **Confirmed** that `0x24020` controls the five Exodia-piece count test.
- **Confirmed** that `0x24070` controls the general per-card count test.
- **Confirmed** that `0x1BFB24` is already zero in the verified retail target.
- **Tentative** that the `0x1BFB24` step repairs a particular editor's
  relocated restricted-card data.

## Disable Exodia win detection

**Tutorial:** `Desativar exodia.txt`

The tutorial changes one immediate byte at each of two SLUS offsets:

| SLUS offset | VRAM | Retail bytes | Instruction | Purpose |
|---:|---:|---|---|---|
| `0x952C` | `0x80018D2C` | `11 00 07 24` | `addiu $a3, $zero, 0x11` | First required Exodia card ID |
| `0x959C` | `0x80018D9C` | `16 00 E2 28` | `slti $v0, $a3, 0x16` | Exclusive end of the required-ID range |

Both instructions are in the exact matching C for `func_80018CF8`. The
function copies five hand-slot indices, then searches those slots for each
card ID from `0x11` through `0x15`. A matched slot is replaced with `-1`, so
one card cannot satisfy more than one required ID. In simplified form, the
function returns one only after finding all five pieces:

```c
for (a3 = 0x11; a3 < 0x16; a3++) {
    for (i = 0; i < 5; i++) {
        if (buf[i] >= 0 &&
            D_8015C424.cards[buf[i]].id == a3) {
            buf[i] = -1;
            goto found;
        }
    }
    return 0;
found:;
}
return 1;
```

The caller at `0x80018FC4` keeps duel state `4` when the check returns zero
and changes it to state `0xE` when the check succeeds. Existing duel research
identifies that state transition as the `SUMMON Exodia` instant-win path.

Changing the low immediate bytes from `0x11` and `0x16` to `0x00` makes the
function search for card ID zero and changes the continuation test to
`a3 < 0`. The compiled loop has a peeled first iteration, so this is not
literally a zero-iteration loop. Instead, the first search fails for normal
retail card data, where zero denotes an empty card and valid IDs are
`1`-`722`, and the function returns zero before the Exodia state transition.

**Confidence:**

- **Confirmed** that `0x952C` and `0x959C` are the two Exodia card-range
  immediates in `func_80018CF8`.
- **Confirmed** that the retail function requires all five distinct IDs from
  `0x11` through `0x15`.
- **Confirmed** that the two-byte patch makes the check return zero for normal
  retail card data.
- **High** that caller state `0xE` is specifically the Exodia summon/win
  presentation; the caller and existing duel research agree, but the state
  itself does not yet have a semantic symbol.

## Additional end-of-duel starchips

**Tutorial:** `Alterar Starchips - Por Wladmir Ghost.txt`

The tutorial replaces the four zero bytes at SLUS offset `0x126DC` with an
`addiu` instruction. The file offset maps to `0x80021EDC` in
`Duel_MaybeEndOfDuel`:

```text
0x80010000 + (0x126DC - 0x800) = 0x80021EDC
```

The retail sequence is:

```mips
lbu   $v1, 0x3A($v0)    # duel result's starchip prize
lw    $v0, 0x5E0($a0)   # player's current starchips
nop                     # tutorial patch site
addu  $v0, $v0, $v1
sw    $v0, 0x5E0($a0)
```

The NOP is a load-delay slot. Replacing it with:

```text
C8 00 63 24
```

produces:

```mips
addiu $v1, $v1, 200
```

This does not assign 200 starchips directly. It adds 200 to the duel result's
existing prize byte before that prize is added to the saved total. Thus a
base award of 5 becomes 205, matching the tutorial's explanation. Its
100-starchip example uses an immediate of 95 (`5F 00 63 24`) for the same
reason.

Immediately afterward, the function clamps the resulting total to
`0xF423F`, decimal 999,999. The patch therefore cannot make the persistent
starchip count exceed that cap through this award path. It is also gated by
the surrounding ordinary-opponent checks, so it does not apply to every
possible duel or debug path.

**Confidence:**

- **Confirmed** that `0x126DC` is a safe instruction slot between the prize
  and total loads and their addition.
- **Confirmed** that the immediate increases the existing duel award rather
  than replacing the player's starchip total.
- **Confirmed** that the updated total is capped at 999,999.
