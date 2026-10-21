# Model fog-driver colour state

`gModel_dwFogBaseColor` is the former NA `D_8009B300`.
The two game-owned fog-textured HMD drivers copy the whole word into their
scratch `CVECTOR` palettes for normal/depth-cued colours. The flags setter
resets it to `0x808080`; the field-card fade reads its low byte, steps by
eight and writes equal RGB channels. The overlay colour command publishes
explicit packed channels.

The object remains `u32`. Whole-word stores, byte-view reads and the literal
`D_8009B300_IN_DATA` selector retain the default/forced-`.data` declarations
and addressing behavior. No RGB structure, narrower storage, colour mask,
new validation or altered fade/reset policy is introduced.

## Japanese collision protection

The Japanese fog provider is `gJapanese_D_8009B300`. The separately named
Japanese `D_8009B300` used by `display_object_core.c` is instead the accepted
unlink-count provider. That complete wrapper and its unrelated macro remain
byte-identical; matching spelling does not imply matching identity.

The legacy colour-command spelling bridge is confined to the overlay-private
`duel_effects/color_helpers.h`, after the canonical declaration is included.
It is deliberately not placed in public `sorted_entry.h`, where a global
macro could poison the unrelated Japanese alias through include expansion.
The protected Japanese core translation unit must retain its complete
compiler/MASPSX output, not merely its source-file hash.

Both historical overlay C bodies remain unchanged. Native provider bindings
retain their original addresses and attributes, and three obsolete Japanese
fog-colour macros retire only for the actual colour consumers. Existing
controls migrate identifiers only. Profiles, GTE code, grouped sources,
attempts, overlay fingerprints and the complete Lua tree remain untouched.
