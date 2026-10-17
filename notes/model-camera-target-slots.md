# Camera target-slot selectors

`Model_SetCameraTargetSlots`, originally `func_8005A010` at `0x8005A010`,
stores the camera move's two target-slot selectors. Its name is supported
by the actual stores and consumers, not solely by symmetry with
`Model_SetCameraEyeSlots`.

The inputs are assigned to `ModelCameraMove.target.pair_slot` and
`target.slot`. The controller captures both at entry: the first selects
the slot used for target-start tracking, and the second supplies
target-end and direct reference-point follow behavior. The resident intro
requests `(-1, selected_slot)` before continuing its animation state.

## Exact request boundary

The first state getter is always called; the second is consulted only
when the first returns `1`. If both agree at `1`, no selector store occurs.
The setter neither reports success nor guarantees an unguarded write.

The two `s32` inputs and their narrowing into existing signed halfwords
remain unchanged, as does store order. No validation, clamping, immediate
view update, duration, flags, eye selectors or movement initialization is
added. Existing caller values, including `-1`, remain untouched.

## Object and regional identity

The function stays in the seven-function `model_effect_requests.c` group
under `gcc_2_8_1_g0_split`, preserving its `0x64` extent:

| Build | Address |
|---|---|
| North America | `0x8005A010` |
| Japan | `0x8005CE40` |
| European base | `0x8005E4D4` |
| French, German, Italian, Spanish | `0x8005D118` |

Both Japanese adapters rename only their shared macro LHS; the local
`func_8005CE40` export, inventory and binding remain intact. Other regions
rename their existing common binding without changing its address.

The owner declaration remains in `model_effect_requests.h`. Complete
implementation/caller/adapter comparisons retain every token except the
identifier. No source move, profile, stored layout, SDK/overlay
implementation, integer-address ABI or guest-pointer cast changes.
