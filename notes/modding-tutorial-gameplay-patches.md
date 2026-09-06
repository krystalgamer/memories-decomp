# Modding Tutorial Gameplay Patches

This note verifies gameplay-related patches from the community tutorials
attached to issue #368. Tutorial file offsets are mapped back to resident code
before their claims are accepted.

Use this note with
[`modding-tutorial-evidence.md`](modding-tutorial-evidence.md), which holds
the companion text, asset, and cross-cutting patch analysis. Both files are
evidence ledgers: detailed offsets and uncertainty stay here, while reconciled
game behavior belongs in
[`research/the-game.md`](research/the-game.md). A tutorial observation is not
promoted there merely because its patch address is confirmed.

Confidence levels:

- **Confirmed**: retail bytes, disassembly, and matching C or independently
  recovered symbols agree.
- **High**: the patch site is established, but some surrounding semantics are
  not independently named.
- **Tentative**: the claimed effect depends on an external editor or modified
  input that is not present in the verified retail target.

## Extra drops require the tutorial's earlier tool patch

**Tutorial:** `Drops 16 até 37.docx`

The tutorial begins by requiring the codes from a separate drops tool, which
it says already adds 15 drops. Its follow-up table encodes a requested count
`N` as hexadecimal values `N + 1` and `N`: the 37-drop example uses `26`
and `25`. These are edits to the tool's modified archive, not established
retail drop-count settings. The document does not supply the prerequisite
tool payload.

All 22 listed WA offsets fall in phase 9 of the seven terrain packages.
The package stride is `0x75800`; phase 9 spans package-relative
`0x5D800-0x5EFFF` and is copied to `0x801A9800-0x801AAFFF` by
`Duel_LoadPackageStage`. The normal-terrain sites reduce to four locations:

| Normal WA offset | Phase offset | Loaded address | Retail bytes | Tutorial byte for 37 drops |
|---:|---:|---:|---|---|
| `0xBC17E4` | `0xFE4` | `0x801AA7E4` | `00 00 02 3F` | `26` |
| `0xBC1C78` | `0x1478` | `0x801AAC78` | `00 00 03 1F` | `26` |
| `0xBC1D74` | `0x1574` | `0x801AAD74` | `04 01 AB 8F` | `26` |
| `0xBC1DEC` | `0x15EC` | `0x801AADEC` | `C8 19 82 94` | `25` |

The last three rows recur at the same offsets within all seven terrain
packages. The first row is listed only for Normal; the document provides no
corresponding edit for the other six copies. That asymmetry is preserved here
rather than silently adding patch sites.

The complete retail `0x1800`-byte phase is identical in all seven packages:

```text
SHA-256: d8bf4e5b8d135721ac2e2a45e45ca8c6b1d9d3ade9e5c9dbb668130f127e2fac
```

This is a live script buffer: `func_8001D670` passes its base `0x801A9800`
to `AiScript_Init` at the call at `0x8001D7D8`. That establishes the block's
retail consumer, not the meaning of each byte after the drops tool rewrites it.

**Confidence:**

- **Confirmed** that the 22 listed sites map to the phase and bytes above in
  the verified retail archive, and that the seven phase copies are identical.
- **Confirmed** that the tutorial requires a prior tool patch and lists
  count-plus-one/count values, rather than a self-contained retail patch.
- **Tentative** that 37 is a working maximum or that the asymmetric list is
  complete. Establishing either requires the prerequisite tool output and
  validation of its injected logic; these edits alone do not prove it.

## Deck copy limits

**Tutorials:**

- `Alterar Limite de Cartas - Por Jael Rivera.txt`
- `Inserir Limites e Personalizar - Por Wladmir Ghost.txt`
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

### Custom per-card limit injection

`Inserir Limites e Personalizar` explains the provenance of the separate
`0x1BFB24` region. It is not a hidden retail table: the tutorial first writes a
`0x1AC`-byte code-and-data payload at SLUS offset `0x1BFA48` (VRAM
`0x801CF248`). Every byte in that destination range is zero in the verified
retail executable.

The tutorial then installs two hooks:

| SLUS offset | VRAM | Retail sequence | Injected behavior |
|---:|---:|---|---|
| `0x22418` | `0x80031C18` | Load the selected card's deck count and test it against `3` | Jump to `0x801CF248`, select a per-card limit, update the list-display comparison and its color constant, then resume at `0x80031C24` |
| `0x24064` | `0x80033864` | Load the selected card's deck count before `sltiu $v0, $v0, 3` | Jump to `0x801CF364`, return the selected limit in `$a0`, and replace the fixed comparison with `sltu $v0, $v0, $a0` |

The first hook lies in unmatched `func_80031874`; the second lies in the exact
matching deck-add path `func_800336F0`.

The injected lookup contains 26 little-endian halfword slots:

- 16 slots at `0x1BFB24-0x1BFB43`, whose limit immediate is stored at
  `0x1BFB8C` and defaults to `2`;
- 10 slots at `0x1BFB44-0x1BFB57`, whose limit immediate is stored at
  `0x1BFBCC` and defaults to `1`;
- cards absent from both groups use the immediate at `0x1BFBD0`, which defaults
  to the retail limit `3`.

The tutorial encodes each selected card as `card_id + 0x7ADC`. The injected
routine derives the same key from the selected-card pointer before scanning
the two groups. Its initial `0x7ADC` entries represent empty slots because
retail card IDs begin at one.

The older instruction to zero 52 bytes at `0x1BFB24` is therefore still a
no-op on the untouched executable, but it now has a supported modified-build
interpretation: it clears the 26-entry table created by this injection. It
does not reveal a retail restricted-card table.

**Confidence:**

- **Confirmed** that `0x24020` controls the five Exodia-piece count test.
- **Confirmed** that `0x24070` controls the general per-card count test.
- **Confirmed** that `0x1BFA48-0x1BFBF3`, including the table at `0x1BFB24`,
  is zero-filled in the verified retail target.
- **Confirmed** that the custom payload replaces the fixed limit with
  `2`/`1`/`3` selection over two injected card groups.
- **High** that tutorials which only clear `0x1BFB24-0x1BFB57` assume this or
  a compatible injected limit system rather than retail data.

## Rebuy cards from the password shop

**Tutorial:** `Comprar Cartas Repetidas - Por Claudio Lima.txt`

The tutorial replaces four bytes at `WA_MRG.MRG` offset `0xFBD6E0`:

```text
05 00 40 10    beqz $v0, 0x8016A6F8
BE A9 05 08    j     0x8016A6F8
```

The password overlay begins at archive sector 8054 (`0xFBB000`) and loads at
`0x80168000`, so the file edit maps to overlay address `0x8016A6E0`. The code
has just called `Campaign_TestStoryFlag` for flag `0x400 + card_id`. Retail
branches to `0x8016A6F8` only when that per-card password-used flag is clear.
When it is set, the fall-through path displays message `0xE5` and exits the
purchase attempt.

Replacing the conditional branch with a jump always enters the ordinary
purchase flow. That path still compares `gLibrary_dwStarchips` with the
card's cost, displays the affordability result, and on confirmation calls
`Library_UpdateCardUsedFlag(0x400 + card_id)` and `Duel_AwardCard(card_id)`.
The patch therefore removes only the one-purchase-per-password gate. It does
not make purchases free or deliver cards through a separate path.

The same instruction is independently recorded in
[`research/gameshark-codes.md`](research/gameshark-codes.md) because an
equivalent conditional GameShark patch targets runtime address
`0x8016A6E0`.

**Confidence:**

- **Confirmed** that archive offset `0xFBD6E0` maps to the password overlay
  branch at `0x8016A6E0`.
- **Confirmed** that the replacement always bypasses the used-password
  rejection and preserves the normal cost and award logic.

## Non-monster effect-group table

**Tutorial:** `CPU Entender as Cartas e os Efeitos - Por Wladmir Ghost.docx`

The tutorial identifies bytes at SLUS offsets `0x812D4`-`0x81338`. They map
to resident addresses `0x80090AD4`-`0x80090B38` and form the 101 meaningful
entries of the `0x80090AD4` effect-group table. The accepted card ranges and
their zero-based table indices are:

| Card IDs | Table indices |
|---|---:|
| `301`-`350` | `0`-`49` |
| `651`-`700` | `50`-`99` |
| `721` | `100` |

Exact matching C in `func_80026BA4` enforces those three ranges and performs
the index conversion. `func_80026B34` then reads the table byte, doubles it,
adds the active-side selector, and calls the corresponding handler in the
30-entry table at `0x80090A5C`.

Retail values `0`-`13` select the effect families already established in the
duel documentation: no play-time action, terrain, LP recovery, direct damage,
field destruction, type/threshold destruction, Stop Defense, Raigeki,
Dark-piercing Light, attack reduction, Swords of Revealing Light,
Cursebreaker, ritual, and Harpie's Feather Duster. Changing a byte changes the
effect handler used when that non-monster card is activated.

This table is not established as an AI-priority or card-type table. The
resident usage inventory has one direct consumer, the duel effect dispatcher,
while the AI reads card type from bits `26`-`30` of
`gDuel_adwCardStats`. A card must still reach the normal activation path for
its edited effect group to run.

The tutorial's offset list contains two card-number errors:

- `0x8132F` is table index `91`, card ID `692` (Turtle Oath), not a second
  entry for card `691`.
- `0x81338` is table index `100`, card ID `721` (Dark Magic Ritual), not card
  `722`. Card `722` is a monster and is rejected by the non-monster range
  guard.

The table occupies 104 bytes through `0x80090B3B`, but only the first 101 are
reachable through the accepted card ranges; the final three bytes are padding
before the next resident symbol at `0x80090B3C`.

**Confidence:**

- **Confirmed** that the 101 listed offsets map to the three accepted
  non-monster card-ID ranges.
- **Confirmed** that each byte selects the duel effect-handler group.
- **Confirmed** that the tutorial's card `691` and `722` labels at the final
  offsets are off by one.
- **Unverified** that editing this table alone changes CPU prioritization or
  understanding; no direct AI consumer is currently known.

## Disable Exodia win detection

**Tutorial:** `Desativar exodia.txt`

The tutorial changes one immediate byte at each of two SLUS offsets:

| SLUS offset | VRAM | Retail bytes | Instruction | Purpose |
|---:|---:|---|---|---|
| `0x952C` | `0x80018D2C` | `11 00 07 24` | `addiu $a3, $zero, 0x11` | First required Exodia card ID |
| `0x959C` | `0x80018D9C` | `16 00 E2 28` | `slti $v0, $a3, 0x16` | Exclusive end of the required-ID range |

Both instructions are in the exact matching C for
`Duel_HasAllExodiaPieces`. The function copies five hand-slot indices, then
searches those slots for each card ID from `0x11` through `0x15`. A matched
slot is replaced with `-1`, so
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
  immediates in `Duel_HasAllExodiaPieces`.
- **Confirmed** that the retail function requires all five distinct IDs from
  `0x11` through `0x15`.
- **Confirmed** that the two-byte patch makes the check return zero for normal
  retail card data.
- **High** that caller state `0xE` is specifically the Exodia summon/win
  presentation; the caller and existing duel research agree, but the state
  itself does not yet have a semantic symbol.

## Editable Duel Master K deck

**Tutorials:**

- `Deck Duel Master K editavel.txt`
- `Duel Master K - Por Geiner Quintero.txt`

Both copies give the same edit: change the byte at SLUS offset `0x8585` from
`0x00` to `0x01`. The second copy explicitly describes `0x00` as copying the
player's deck and `0x01` as using Duel Master K's own editable deck. That
offset is the second byte of the instruction beginning at VRAM `0x80017D84`:

| SLUS bytes | Instruction | Comparison limit |
|---|---|---:|
| `27 00 62 28` | `slti $v0, $v1, 0x27` | `39` |
| `27 01 62 28` | `slti $v0, $v1, 0x127` | `295` |

The edited byte is therefore not a Boolean deck-mode value. It is the high
byte of a signed 16-bit immediate. The surrounding code has loaded the signed
opponent ID into `$v1` and prepared the player deck pointer in `$a0`. It then
selects the second source passed to `Duel_ShuffleBothDecks`:

```mips
slti  $v0, $v1, 39
bnez  $v0, use_sources
move  $a1, $zero
move  $a1, $a0
use_sources:
jal   Duel_ShuffleBothDecks
```

Opponent IDs below `39` call the shuffle helper with `(player_deck, NULL)`.
Duel Master K is opponent `39`, so the retail comparison fails and the
fallthrough changes the call to `(player_deck, player_deck)`. The matching C
for `Duel_ShuffleBothDecks` passes those two arguments independently to the
player and CPU `Duel_ShuffleDeck` calls, explaining why Duel Master K receives
a copy of the player's deck.

After the patch, every nonnegative signed-byte opponent ID is below `0x127`.
Duel Master K therefore follows the ordinary `(player_deck, NULL)` path
instead of supplying the player deck as both sources. Existing data research
shows that his stored deck block is a placeholder, so making that deck useful
still requires editing the associated opponent deck data as the tutorial
intends.

**Confidence:**

- **Confirmed** that `0x8585` changes the comparison limit from `39` to
  `295`, not a standalone mode flag from zero to one.
- **Confirmed** that opponent `39` changes from
  `(player_deck, player_deck)` to the ordinary `(player_deck, NULL)` shuffle
  path.
- **High** that the null second source selects the configured opponent deck;
  the tutorial behavior and existing Duel Master K data research agree, while
  `Duel_ShuffleDeck` itself remains assembly.

## Allow opponent cards 721 and 722

**Tutorial:** `Adversario usar cartas 721 e 722.txt`

The tutorial changes the byte at SLUS offset `0x14CCC` from `0xD0` to
`0xD2`. That byte is the low byte of the 16-bit immediate in the instruction
at `0x800244CC`:

| SLUS bytes | Instruction | Last possible card ID |
|---|---|---:|
| `D0 02 22 2A` | `slti $v0, $s1, 720` | `720` |
| `D2 02 22 2A` | `slti $v0, $s1, 722` | `722` |

This loop is inside `Duel_ShuffleDeck`. It walks zero-based card indices and
writes `index + 1` into the shuffled deck when a weighted selection succeeds.
The retail bound therefore examines indices `0`-`719`, which can produce
card IDs `1`-`720`. Raising the bound to `722` also examines indices `720`
and `721`, enabling IDs `721` and `722`.

Opponent deck data contains 722 16-bit weights per table, so the two added
reads remain inside the documented row. The patch only makes the final two
weights reachable: either card still requires a nonzero configured weight and
must win the normal weighted selection.

The name-entry starter-deck generator has its own independent 720-entry loop.
Changing `0x14CCC` affects opponent deck shuffling only; it does not make
cards `721` and `722` available to starter-deck generation.

**Confidence:**

- **Confirmed** that `0x14CCC` changes the loop bound from `720` to `722`.
- **Confirmed** that the loop converts zero-based indices to one-based card
  IDs.
- **Confirmed** that opponent weight rows contain 722 entries.
- **Confirmed** that the separate starter-deck path remains unchanged.

## Direct-damage and LP-recovery tables

**Tutorials:**

- `Efeito e Visual de Dano e Cura.txt`
- `Offset de Traps e Magicas - Por Wladmir Ghost.txt`

Both tutorials identify the same two five-byte tables in the resident
initialized data:

| Effect | SLUS range | Resident range | Retail values | Scale |
|---|---:|---:|---|---:|
| LP recovery | `0x8B730-0x8B734` | `0x8009AF30-0x8009AF34` | `2, 5, 10, 20, 50` | `x100` |
| Direct damage | `0x8B738-0x8B73C` | `0x8009AF38-0x8009AF3C` | `5, 10, 20, 50, 100` | `x10` |

The recovery table maps, in order, to Mooyan Curry, Red Medicine, Goblin's
Secret Remedy, Soul of the Pure, and Dian Keto the Cure Master. Exact
matching C in `func_800250C8` indexes `gDuel_abLifePointRecoveryUnits`,
multiplies the selected byte by `0x64` (decimal `100`), adds it to the
selected duel-side LP at `+0x14`, and clamps the result to that side's maximum
LP at `+0x16`.

The damage table maps to Sparks, Hinotama, Final Flame, Ookazi, and
Tremendous Fire. Exact matching C in `func_8002525C` loads the selected byte
from `gDuel_abDirectDamageUnits`, multiplies it by `10`, subtracts it from the
selected LP halfword at `+0x14`, and clamps a negative result to zero. When
the alternate-state halfword at `0x8009B22A` is zero, the function selects
`D_800E9FF0[playing_side ^ 1]`, the opposing side. Its other application
branch selects `D_800E9FF0[playing_side]`, the active side, establishing the
reflected-damage routing without assigning a semantic name to that state
halfword.

Both handlers use the same resident presentation sequencer when bit `0x20` of
`D_8009B220` is set. Newly matching `func_8001F364` waits for the preceding
screen effect, starts a first `func_80022D94` phase, and arms a 20-frame
counter. When that counter expires it copies the selected card object's
`+0x30`, `+0x32`, and `+0x34` values into a newly allocated type-8 effect
object, updates the corresponding `0x1C`-byte duel-card record through
`func_80024954`, and plays sound `0x17`. It then starts a second
`func_80022D94` phase, waits another 20 frames, increments byte `+0x06` in
the opposing side's `0x20`-byte state record, and reports completion. The
recovery and damage tables change the LP amount; they do not select separate
resident presentation timelines.

The retail values therefore reproduce the documented effects:

| Card sequence | Resulting LP change |
|---|---|
| Recovery | `+200, +500, +1000, +2000, +5000` |
| Damage | `-50, -100, -200, -500, -1000` |

Each SLUS edit changes one unsigned byte before scaling. For example, recovery
byte `50` (`0x32`) produces `5000`, while damage byte `100` (`0x64`) produces
`1000`. The combined trap/magic tutorial correctly states the damage scale as
`x10`, but its final parenthetical prints `10x100`; the listed input and
result still correspond to `100 * 10 = 1000`. The other tutorial's separate
WA offsets control presentation data and are not established by these
resident tables; they remain a separate asset-level investigation.

**Confidence:**

- **Confirmed** that both five-byte resident tables and their scale factors
  produce the listed LP changes.
- **Confirmed** that the normal and alternate branches target the opposing
  and active side records respectively, and clamp the selected LP value to
  zero.
- **Confirmed** that the recovery path's `+0x16` halfword is the maximum LP
  value that caps the authoritative LP at `+0x14`.
- **Confirmed** that recovery and direct damage share the matching
  `func_8001F364` presentation sequence and its two 20-frame waits.

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
