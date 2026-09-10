# Duelist code generation and uses

The persistent word at save-state offset `+0x334` is the **duelist code**.
Matching code now names its live alias `gSaveData_dwDuelistCode` and models the
same location as `SaveDataState.duelist_code`. This resolves the older
`gSave_dwGameID?` hypothesis: the value is not a general game identifier.
It is a nonzero per-save comparison token with one unrelated presentation use
in the credits.

## Generation

`NameEntry_Main` creates the code only while initializing a new save. After
name entry finishes and the starter deck has been built, it XORs all twelve
bytes of `SaveDataState.player_name_sjis` and stores:

```text
(gMain_dwVBlankTick << 8) | name_byte_xor
```

`gMain_dwVBlankTick` is zeroed by `Main_Init` and incremented once by
`Main_VBlankCB`. Other matching consumers fold its low six or seven bits into
UI colour pulses, confirming that it is the boot-lifetime VBlank animation
tick rather than the separate persisted counter at
`gSaveData_dwVBlankCounter`.

The name XOR occupies only the low byte. The left shift discards the upper
eight bits of the tick and places its low 24 bits in code bits 8-31. If the
combined word is zero, `NameEntry_Main` repeatedly substitutes `rand() << 8`
for the high portion until the stored word is nonzero.

This establishes only a **nonzero** guarantee. The routine does not compare
against an existing save, does not enforce global uniqueness, and uses an XOR
rather than a collision-resistant checksum. Equal names and equal timing can
produce equal codes, and different names can share the same XOR byte.

## Persistence and comparison

The code lies inside the `0x680`-byte persistent state copied by
`SaveData_RequestWrite` and restored by `SaveData_PollLoad`. It is not rebuilt
when a save is loaded.

`SaveData_HasSameDuelistCode` compares only this 32-bit field. The two-save
memory-card path uses equality to recognize two loaded records as the same
duelist; `SaveData_MatchesDuelistAndCurrentSequence` then additionally requires
the candidate record's `save_sequence` at `+0x404` to equal the current runtime
sequence. The sequence is therefore a separate freshness/version condition,
not part of the duelist code.

FM-Online's patch at `0x8003FAE8`/`0x8003FAF0` bypasses the retail failure path
reached after equal duelist codes, allowing copied saves to proceed through
that two-player flow. This agrees with the matching comparison semantics and
does not imply that the code is cryptographic authentication.

## Credits use

`Main_RunCredits` reads `gSaveData_dwDuelistCode`, computes the remainder
modulo five, and selects one of the five entries in
`gCredits_awSecretNumbers`. This makes the displayed `SECRET NO.` stable for a
save but does not give the duelist code a broader identity contract: only the
remainder is used, and five possible display values intentionally collapse
many codes.

## Evidence boundary

The field name is established by matching producer and comparison code, the
two-save callers, and the FM-Online bypass. The VBlank tick identity is
established by `Main_Init`, `Main_VBlankCB`, and its matching UI consumers.
No emulator trace was required. The code's exact collision frequency and any
server-side meaning are not established by local retail code.
