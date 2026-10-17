# Guarded slot-indexed camera move request

`Model_RequestCameraMoveToSlots`, originally `func_80059F18` at
`0x80059F18`, translates model-slot selectors for the existing camera setup
routine. Resident intro and scene-transition callers supply those slot
indices, including negative/current-endpoint requests.

## Exact argument boundary

Each middle argument is normalized independently: negative becomes zero;
otherwise one is added. These expressions execute before the first state
getter. The first mode word and final timing/control word are forwarded
unchanged. No selector clamp or signed-overflow guarantee is introduced.

The existing first getter and short-circuited second getter determine
whether setup is invoked. The wrapper returns no success result, does not
promise an unguarded start, and does not absorb the callee's endpoint,
duration or zero-duration behavior.

The separate `func_80059EBC` wrapper is deliberately not renamed here.
Its zero-endpoint/zero-timing requests have distinct negative/zero/positive
mode and selector-retention behavior; a simple stop/reset label would
overstate the established role.

## Preserved code and regional identity

Whole implementation, header and resident caller comparisons change only
the identifier. The scene-transition fixture retains its complete oracle
and callback/event arguments under the renamed request stub.

The seven-function `model_effect_requests.c` group and
`gcc_2_8_1_g0_split` profile retain the `0x94` extent:

| Build | Address |
|---|---|
| North America | `0x80059F18` |
| Japan | `0x8005CD48` |
| European base | `0x8005E3DC` |
| French, German, Italian, Spanish | `0x8005D020` |

All regions use the shared export at their original address. No source
move, stored layout, SDK/overlay implementation, guest-pointer cast,
integer-address ABI or compiler setting changes.
