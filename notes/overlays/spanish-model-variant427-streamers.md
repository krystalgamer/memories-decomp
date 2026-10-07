# Spanish MODEL427 streamers

Helper `+0x2DF8..+0x35BC` in the two Spanish MODEL379 stage 9/10 loads is
matching C. It is the 1,988-byte coiled-streamer routine: two 17-point
streamers drawn as positive-depth `POLY_G4` strips. The loader entry does not
reach it.

## Source

The Spanish images are identical to the French MODEL379 stage 9/10 images.
The French release already accepted C for this routine in #7166. Both
modules are therefore registered against
`src/overlays/french_model_variant/variant427_streamers.c` and its slot-1
wrapper, so the two releases share one implementation for the shared
function. `test_french_model_spanish_reuse` requires them to agree.

An independent Spanish reconstruction also matched exactly. It was dropped in
favour of the accepted French source.

## Integration

The MODEL427 binding list gains the SDK aliases `RotTransPers` and `ratan2`.
Their addresses were already bound as `func_spanish_80087868` and
`func_spanish_80089928`.

## Tests

The regressions check:

- the registration against the French metadata
- the inventory in the MODEL427 suite
- relocations and callees of the linked French object against both Spanish
  images
