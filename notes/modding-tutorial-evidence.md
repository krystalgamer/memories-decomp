# Modding Tutorial Evidence

This note records claims recovered from the community modding tutorials
attached to issue #368. The tutorials describe edits from a hex-editor
perspective, so each claim is mapped back to resident code or data before it
is accepted.

Confidence levels:

- **Confirmed**: the retail bytes, disassembly, and independently recovered
  symbols or behavior agree.
- **High**: the code and tutorial agree, but part of the surrounding semantic
  interpretation is not independently named.
- **Tentative**: the patch effect is plausible, but its target or broader
  behavior still needs corroboration.

## Initial duel life points

**Tutorial:** `Alterar LP - Por Jael Rivera.txt`

The tutorial identifies SLUS file offset `0x7DD0`, where the retail halfword
`40 1F` represents decimal 8000 in little-endian order. It suggests replacing
that halfword with another 16-bit value, for example `28 23` for decimal 9000.

The offset maps to VRAM `0x800175D0`:

```text
0x80010000 + (0x7DD0 - 0x800) = 0x800175D0
```

At that address, `func_800175A0` contains:

```mips
addiu $v0, $zero, 0x1F40
sh    $v0, 0x2($sp)
sh    $v0, 0x0($sp)
```

The function uses these two stack halfwords while initializing the two
`0x20`-byte duel-side records rooted at `D_800E9FF0`. For each side it:

- clears the displayed-life-point field at record offset `+0x12`;
- writes the selected initial value at offsets `+0x14` and `+0x16`;
- clears the remaining per-side display state.

The existing RAM-map-backed symbols identify:

| Record | Displayed LP (`+0x12`) | Authoritative LP (`+0x14`) | Adjacent value (`+0x16`) |
|---|---:|---:|---:|
| Player | `gDuel_wPlayerLifePointDisplay` (`0x800EA002`) | `gDuel_wPlayerLifePoint` (`0x800EA004`) | `0x800EA006` |
| Opponent | `gDuel_wOpponentLifePointDisplay` (`0x800EA022`) | `gDuel_wOpponentLifePoint` (`0x800EA024`) | `0x800EA026` |

Therefore the patch does not directly initialize the displayed LP to 8000.
It initializes the authoritative LP and the adjacent LP value, while the
displayed counter begins at zero and is updated afterward. The exact role of
the `+0x16` halfword is not yet independently named; its initialization to the
same value strongly suggests an LP display target or previous-value field.

The constant is used only when `gDuel_bOpponentID >= 0`, the ordinary duel
path. When the opponent ID is negative, `func_800175A0` instead initializes
the two values from `D_8009B234` and `D_8009B236`, so changing `0x7DD0` does
not affect that alternate path.

**Confidence:**

- **Confirmed** that file offset `0x7DD0` is the 8000 immediate in
  `func_800175A0`.
- **Confirmed** that changing it changes the initial authoritative player and
  opponent LP values on the ordinary duel path.
- **High** that the adjacent `+0x16` fields are LP animation targets or
  previous values; their exact role remains unnamed.

For a value representable by one 16-bit halfword, the tutorial's byte-edit
method preserves the instruction opcode: decimal 9000 is hexadecimal
`0x2328`, so the complete instruction changes from `40 1F 02 24` to
`28 23 02 24`.

## Millennium Eye opponent-card display

**Tutorial:** `Olho do Milenio.txt`

The tutorial identifies SLUS offset `0x8864`, whose retail bytes are
`FF 00 02 24`. The offset maps to `0x80018064`, inside the exact matching C
for `func_80018004`:

```text
0x80010000 + (0x8864 - 0x800) = 0x80018064
```

The instruction is:

```mips
addiu $v0, $zero, 0xFF
```

It supplies the final `field_67` value when the current duel-card display
state byte at `+0x1F` is negative. The matching source first selects the
card's normal display-data index plus one, then overrides that selection with
`0xFF` for the negative mode:

```c
if (D_8009B1C8->field_1F != 0) {
    object->flags |= 0x2000;
    result->field_67 = object->child->field_04 + 1;
    if (D_8009B1C8->field_1F < 0)
        result->field_67 = 0xFF;
}
```

`func_8003AAE4` later passes `field_67` to `func_8003A1EC`, which uses it as
the resource index for image lookup and display-object setup. Replacing the
immediate byte with `0x00` or `0x01` therefore changes the selected opponent
card image resource; it does not alter the duel's underlying card data.

`func_80017DB4` contains the same negative-mode `field_67 = 0xFF` override
for another card-object update path. The tutorial's one-byte edit does not
change that site, so static evidence alone does not prove that every
opponent-card object keeps the replacement index through all later updates.

The tutorial reports that value `0` exposes the opponent's card faces and
value `1` keeps the cards hidden while showing their order numbers. The
static call chain confirms that these values select different image
resources, but the exact appearance of those resource indices has not yet
been independently verified against extracted art or a runtime trace.

**Confidence:**

- **Confirmed** that `0x8864` is the low immediate byte of the negative-mode
  `field_67 = 0xFF` assignment in `func_80018004`.
- **Confirmed** that `field_67` becomes an image resource index and does not
  modify card identity or duel state.
- **Tentative** that resource indices `0` and `1` have exactly the two visual
  meanings reported by the tutorial; runtime or asset-level corroboration is
  still required.
