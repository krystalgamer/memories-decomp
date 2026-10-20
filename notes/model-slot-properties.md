# Model slot property interfaces

The six-function `model_slot_properties.c` group remains one contiguous
`gcc_2_8_1_g0` object, from resident `0x800593D0` through `0x8005969C`.
The common NA/JP source and European wrapper keep their original definition
order, types, layouts, native addresses and profiles. These names describe
local SDK-backed behavior, not a new property representation.

## Part vertex transformation

`Model_TransformPartVertex` is the former `func_800593D0`. It selects a
slot, part and vertex, then calls `PushMatrix`, `GsGetLwUnit`,
`GsSetLsMatrix`, `RotTrans` and `PopMatrix`. The SDK declarations establish
the `SVECTOR` input, `VECTOR` output, `MATRIX` scratch and `PSXLONG` flags.
The function returns the first flag word. The name does not claim screen
projection or an observed gameplay coordinate system.

The matching MODEL418 and French MODEL338/393 callers copy the output
components into matrix translation. All eleven native overlay import
bindings follow the same identity; French and Spanish imports still use
`0x8005C4D8`. The two existing French control files change that identifier
only, not values, stubs, assertions or execution paths.

The part lookup deliberately remains an integer sum. A typed indexed
`GsUNIT` expression folds the extra eight bytes into a load and loses an
instruction required by retail. There is no new bounds check, result
interpretation, declaration adapter or compiler flag.
