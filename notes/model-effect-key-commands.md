# Model effect-key queue builders

These builders share the existing queue dispatcher and
[batching state](model-effect-key-batching.md). Their duplicated ordinal
tests, increments and two-byte transfer guards remain separate and in their
original order. No shared helper, validation or key layout is introduced.

## Relative camera orbit

`Model_QueueRelativeCameraOrbit` is the former `func_8005D994`.
It reads the current camera metrics, constructs a slot-relative reference
endpoint and a kind-four distance/yaw/pitch endpoint, then submits the pair
with the existing magnitude argument and batch-selected begin/append mode.
Yaw is biased by the slot side and reduced against the current yaw; pitch
is reduced against the current pitch. Signed halfword views, division-based
remainders and wrap thresholds retain their exact expressions.

Slot two still copies its coordinates and changes the reference endpoint's
kind. The optional `SVECTOR` keeps its side-dependent x/z signs and unsigned
middle-component read. Pointer types, getter/guard order, aligned temporary
arrays and all casts are unchanged.

The NA, Japanese and European source wrappers and public header have semantic
paths. Every native matching manifest and regional static split selects the
same original function extent and named `gcc_2_8_1_g8` profile.

`Model_QueueCameraOrbitFromPreset` is the former `func_8005F5C8`
(Japanese `func_8004F018`). It selects an existing preset before its guard,
preserves the parallel signed halfword distance-adjustment expressions,
and forwards the resulting distance, yaw, pitch and optional offset to
the relative-orbit builder. It does not reuse the direct camera setter.
The preset type, unchecked selector and initialized table are unchanged.

## Endpoint pairs and shake

`Model_QueueEffectEndpointPair` is the former `func_8005F714`
(Japanese `func_8004F164`). Each negative index selects a null channel;
nonnegative indices select the existing endpoint table without a new bound.
The slot controller's `(-1, slot + 2, magnitude)` request demonstrates
one-sided use. Endpoint selection remains before the guard, and the
original signed ordinal and magnitude forwarding are retained.

`Model_QueueCameraShake` is the former `func_8005F7B0`
(Japanese `func_8004F200`). Its aligned `s16[4]` record remains
`{value, 0, 0, 5}` and is passed to both channels. The evaluator's kind-five
case makes four `rand` calls per active channel, offsets x/z, and restores
captured x/z at completion. Both channels count as persistent, causing
the existing forced orbit refresh. The magnitude, random divisor, call
order and completion path are not rewritten or guarded.

All three grouped builders remain in each native 23-function state object
with the existing `gcc_2_8_1_g8_split` profile. Their sizes remain `0x14C`,
`0x9C` and `0x78`; the relative-orbit builder remains `0x210`.

## Historical overlay callers

Seven matching C bodies retain their original bytes and attempt fingerprints:
French variants 107, 129, 198, 782 and 829, NA variant 448, and the Spanish
primary effect. Three narrow old-spelling bridges in the owning resident
headers make those bodies emit canonical imports. Only the Spanish owner's
header include path changes; the effect body does not.

Native bindings and complete per-image symbol maps publish canonical names
at unchanged addresses. No historical attempt, reuse record, compiler claim
or profile is rewritten. Obsolete Japanese provider/caller and endpoint
bridges retire rather than redirecting a canonical name to a numeric symbol.
Existing controls migrate identifiers/include paths only, and the full
Lua trace tree remains unchanged.
