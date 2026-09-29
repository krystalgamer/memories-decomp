# French duel-effect 0 and 6 lifecycles

This batch independently reuses the accepted Spanish sources and headers,
unchanged, with `gcc_2_8_1_g0_split` (GCC 2.8.1/MASPSX 2.81). It adds no C
copies, regional flags, compiler profiles, inline assembly or register pins.

| Function | Source | Extent | Bytes |
|---|---|---|---:|
| `func_80154688` | `src/overlays/duel_effects/effect_0.c` | `80154688..80154B30` | 1192 |
| `func_80154B30` | `src/overlays/duel_effects/effect_6.c` | `80154B30..801556F4` | 3012 |

The independent baseline is accepted master `78fc1686f`: 59 bank C functions
and 15,360 bytes. Both complete source objects add 4,204 bytes, producing
61/85 bank C functions and 19,564 bytes; the seven configured French images
contain 185 C instances and 75,416 bytes. All previous manifest entries and
all 85 inventory boundaries are preserved in that independent proof.

Before publication, the maintainer accepted #6568. Additive reconciliation
through master `9b68de492` preserves all 63 accepted entries and both new
lifecycles: **65/85 bank C functions / 27,148 bytes**, 20 assembly boundaries,
and **189 configured C instances / 83,000 bytes**. Both independent proofs
and the combined production build retain the complete bank identity.

After #6575 was accepted, reconciliation through master `b7e78a24` preserves
all 65 accepted entries, including effects 2/21, and both 0/6 lifecycles:
**67/85 bank C functions / 31,096 bytes**, 18 assembly boundaries, and
**191 configured C instances / 86,948 bytes**. Both batches' bindings,
generated data owners, evidence, and regression tests remain present.

## Independent image and ownership proof

The French archive is `game/france/DATA/WA_MRG.MRG`, SHA-256
`e00de6fac1660bcf142a20a2c0c965a020e75382d22cf62c26e0bfc153cdfe7d`.
Each of its seven terrain copies starts at sector `7193 + terrain * 240`,
occupies 44 sectors, and loads at `0x80146000`.

The independently compiled and linked 90,112-byte bank retains SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Each new relocatable object has exactly one defined function whose size
equals its entire `.text` extent. All 61 C functions have the expected
address and size as real executable-section ELF definitions. The 24 distinct
overlay routines named by the two source bodies also resolve to real
functions of their inventoried sizes, not absolute linker aliases.

Four explicit symbols retain real generated-data definitions in both input
objects and the final ELF, with exact bytes and extents:

| Symbol | Bytes | Generated owner | Meaning |
|---|---:|---|---|
| `D_801461E8` | 16 | `header.data.o` | Effect 0 initial SDK VECTOR |
| `D_801461F8` | 16 | `header.data.o` | Effect 6 initial SDK VECTOR |
| `D_8015B0B4` | 600 | `data.data.o` | Thirty 20-byte effect 0 configurations |
| `D_8015B30C` | 180 | `data.data.o` | Six 30-byte effect 6 configurations |

None is supplied through the absolute linker-symbol file. Resident aliases
come independently from the French resident linker map: `D_8009B261` at
`0x8009C600`, `D_8009B264` at `0x8009C5FC`, `PushMatrix` at `0x80087158`,
`PopMatrix` at `0x800871FC`, and `memset` at `0x8008F548`.
`Model_GetFrameStep` at `0x8005BF24` (32 bytes) and
`Model_SetFrameStepOverride` at `0x8005CBF4` (12 bytes) are independently
identified by their matching-C rows in the French resident inventory.

## Preserved behavior and remaining work

The existing [effect 0](duel-effect-0.md) and [effect 6](duel-effect-6.md)
notes describe the recovered layouts and lifecycle contracts. Effect 0
retains its 30 configurations, separate crossed-line path, saved frame
step, delayed drawing, and conditionally increasing, unclamped scale.
Effect 6 retains six configurations, independent frame/tick counters,
sixteen four-vector trails, staggered activation, particles, column and
background drawing, signed random arithmetic, and variant-five completion.
The intentionally unaligned colors in both work structures are unchanged.

The effect 6 number-renderer callee `func_801566D4` remains a real
1,024-byte assembly definition. Its accepted caller declaration is not a C
coverage claim. The randomized curve and other unmatched effects also
remain assembly. Boot, MODEL/SU dynamic loads and overworld-tail ownership
still require runtime-coverage investigation; these configured-image
counts are not an exhaustive French completion claim.
