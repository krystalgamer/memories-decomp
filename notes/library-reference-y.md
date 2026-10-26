# Library captured and sampled reference-Y views

## Existing scalar views, not new vector storage

| Original label | Canonical view | Library use |
|---|---|---|
| `D_80181002` | `gLibrary_sCapturedReferenceY` | Captured reference-point Y used by the return path |
| `D_80181012` | `gLibrary_sSampledReferenceY` | Sampled reference-point Y used while following model animation |

Both declarations remain signed `s16` scalars with the existing `.data`
attribute. They name independent labels in the Library's fixed reused work
window, not new resident allocations or a permanent ownership/type claim
about other SU images that reuse those addresses.

The narrow header follows the unchanged `library_runtime.h` owner in the
existing runtime root. No literal address is replaced by a named object,
member expression or address macro. That distinction preserves the retail
literal materialization and the independent symbol relocations.

## Captured reference

Under the existing card-view `0x40` guard,
`Model_CopySlotPosition(0, (u16 *)0x80181000)` copies all four slot
halfwords. The already accepted local `SVECTOR *` view supplies the first
three signed coordinates to `view.vrx/vry/vrz`. The independent `+2`
label is the captured Y consumed by the dispatcher's return path.

The globe renderer still copies the same eight bytes from literal
`0x80181000` as an `SVECTOR`. Its whole-word copy, scratchpad destination
and literal `lui/ori` materialization remain unchanged.

This remains a Library-local coordinate view. The later canonical
`Model_CopySlotPosition` name and `ModelSlot.cached_position` member
retain the four-u16-copy ABI and s16[4] storage. The draw producer
separately derives word three from Y; it is not SVECTOR padding or
a new shared position type. See notes/model-slot-properties.md.
The capture guard is unchanged; the name is
not a promise of fresh contents on every entry or invocation.

## Sampled reference and movement

When slot zero is ready and its animation index differs from one, the
dispatcher copies the same four halfwords to literal `0x80181010`.
Its independent `+2` label is the sampled Y target.

The existing step remains `abs(sampled - captured) / 24 + 1`, spelled
through the original signed locals and branches. The first movement
test may write `view.vry`; the exact volatile `PSXLONG` reload and the
second ordered test still follow. The second test can act in the same
call, so this is not rewritten as a generic clamp or promised to
converge monotonically.

The animation-one return branch still uses its separate eight-unit
step and crossing clamps toward the captured value. Sampling frequency,
readiness checks, repeated loads, calls, store order and orbit update
remain. Neither name establishes a fixed duration, new unit conversion,
global model coordinate convention or lifetime guarantee.

The two snapshot addresses are `0x10` apart although each producer
writes eight bytes. No array-of-two, inferred stride, allocation bound
or classification of the gap is added.

## Native and compatibility boundaries

All seven original image providers remain. The four continental PAL
configurations keep their tracked numeric bindings; NA/Japan/UK keep
their original automatic providers behind conditional `PROVIDE`
aliases. No guessed address is pinned, and unreferenced aliases do
not introduce new dependencies into unrelated links.

Actual CPP and named-profile objects cover three runtime contexts and
forty-nine registrations. All six declarations/imports retain signed
halfword loads and absolute `.data` addressing. The accepted NA ELF
independently confirms the two original label addresses.

All old headers, model helper/type, globe source, regional wrappers,
compiler profiles, source groups and tests remain unchanged. In
particular, the existing actual-source capture control keeps its literal
sequence and all signed-coordinate, axis and padding negatives.
No new harness, fixture, probe or testcase is introduced.

See [library-state-layout.md](library-state-layout.md) for the complete
screen-state and local coordinate-view contract.
