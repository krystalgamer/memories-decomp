# French MODEL437 three-point bands

MODEL103 stages 9/10 now own the 1,884-byte bands helper at image `+0xD4C`,
`func_8013BD4C` / `func_8017BD4C`, through
`src/overlays/french_model_variant/variant437_bands{,_slot1}.c`.
Both compile with the existing `gcc_2_8_1_g0_split` profile,
GCC 2.8.1 and MASPSX 2.81.

## Evidence and shared source

A broader scan of already-registered regional overlay C found the accepted
French [MODEL449 bands body](french-model-image449-bands.md), including its
raw-image registration at `0x8013D458`. No raw-image inventory is changed.
The normalized fingerprint only located the donor; original instruction
comparison and native compilation establish the reuse.

After renaming the function and supplying the existing French resident
`RotTransPers3` destination, the donor has the correct `0x75C` extent and
exact scheduling/register allocation. Its 22 differing instructions are
context-relative accesses, all `0x600` earlier in MODEL103. The measured band
starts at `0x888`, projected-colour induction base at `0x8DC`, second GT4
packet at `0x1424`, origin at `0x1514`, factor at `0x1590`, scale at `0x1598`,
spread at `0x159C`, delta at `0x15B0`, axis pair at `0x15C0/0x15C2`, flags at
`0x15DC`, and direction at `0x164C`. The band remains `0xB4` bytes.

`MODEL_VARIANT437_BANDS` changes only the initial opaque padding in
`variant449_bands.h`, from `0xE88` to `0x888`. Slot wrappers select that
measured layout and the original function name. The accepted body, record
definition, all later relative field gaps, and default MODEL449 layout are
unchanged. This is not a runtime pointer adjustment. It preserves the
three-point projection, factor clamp, signed drift, scale bias, mirrored
two-sided draw sequence, and positive-depth submission gate.

The canonical `RotTransPers3 = 0x80087898` binding is added to the shared
French437 linker symbols without removing its address-based Spanish alias.
Its destination is already a French resident function start.

## Investigation and production acceptance

The bounded investigation used four probes in about 74 seconds: an explicit
wrong-function-symbol error, an explicit missing-binding error, the
22-word layout mismatch, and the exact smaller-layout result. Failed links
were not cached as successes. Local probes and disassemblies remain beneath
`tmp/fr-probe/`; no compiler sweep or allocation search was needed.

Production acceptance then rebuilt and matched all 3,594 complete French
overlay images, including both MODEL103 targets and the four existing
MODEL449 donor images. Both new objects define original `0x75C` STT_FUNC
owners at section offset zero; final ELFs retain the original addresses and
sizes without helper `.NON_MATCHING` symbols.

The [attempt ledger](french-model-variant437-bands-attempts.csv) records the
initial errors, measured mismatch and terminal source/header/body fingerprints.
A replay row is appended to the existing MODEL449 ledger after the shared
header change; historical records are preserved. Regressions target-compile
both layouts and check registrations, both wrappers, retained bindings,
fingerprints, full image hashes and sized object/ELF owners.

This adds two French C instances and 3,768 bytes. Sheet/streamer registrations,
the unmatched entry and third helper, archive registrations and generated
reports are unchanged.
