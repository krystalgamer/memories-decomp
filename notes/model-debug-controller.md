# Model-debug controller

`func_800534B8` (`0x800534B8`, `0xBFC` bytes) is the returning controller used
by `Main_RunAnimatedBattle` outside the intro mode. Its load diagnostics,
decimal model-ID editor, stage selector, and per-model flag display support
the descriptive controller name; the function retains its address-based symbol.

## Behavior retained

The signed phase -2 returns one immediately; phases at least 40 return zero.
Either pad's pressed cancel bit sets phase 40. Otherwise the controller loads
the first unavailable slot in stage, model 0, model 1 order. It respects the
primary transfer mask and independent secondary word, or advances the model
loader when the slot is already in progress.

When the stage becomes ready, the record halfword at +0xA4 is copied to the
upper half of `D_8009B004` and interpreted as a signed height. A nonnull stage
coordinate unit receives zero rotation and translation `(0, height, 0)`.
Its pointer is reloaded between writes; null still reaches `func_8005922C`
with null. The other model slots use record byte +0xA1 to select orientation.

The low byte of `D_8009B008` gates the editing display and can be toggled by
the two-pad input combination. Editing retains modulo-seven stage selection,
modulo-six digit selection, three-decimal-digit wrapping, signed halfword
height adjustment, and independent XOR-one flag changes. The real per-test
pressed/repeat halfword reads use existing input-owner declaration arms.
Pad 2 uses a true-width nonvolatile absolute halfword view, not an enlarged
array.

A phase changed to -2 by a loader can reach rejection sampling later in the
same call, even though -2 at entry returns immediately. Invalid samples,
the excluded ID ranges and ID 720, and IDs equal to the currently loaded
model are rejected in the original order. A valid but equal candidate is
written to the cache before retrying. Diagnostics precede the reload:
wait for transfers, reset the scene, then set slots 0, 1 and 2. The canonical
variadic property setter takes five properties for each model but only one
for stage slot 2; the extra arguments guessed by the reference export are
not real arguments.

## Data ownership and matching

The initialized, four-byte `D_8009B004` definition moves from the middle of
the handler-state data unit into the controller. Its low half remains the
existing scene-state bytes; no whole-word semantic rename is justified.
The original small-data run is now three contiguous sections:

| Address | Bytes | Owner |
|---|---:|---|
| `0x8009AFE4` | `0x20` | `model_handler_state.c` |
| `0x8009B004` | `4` | `func_800534B8.c` |
| `0x8009B008` | `0x50` | `model_handler_diagnostics.c` |

The definition deliberately lets the named uniform-G8 compiler profile
select small data. GCC's `.sdata` directive tells MASPSX the word's extent,
so it emits the required load-delay NOP before the upper-halfword store.
An extern declaration, or GCC's explicit-section `.section` spelling, does
not provide that information to this MASPSX parser. No common-symbol
diagnostic, duplicate backing, linker alias, patched compiler, generated
assembly edit or synthetic padding is promoted.

Scalar halfword views of the known upper word recover placement scheduling.
Branch-local modulo expressions and indexed digit writes recover the
arithmetic allocation. Assigning the excluded-card invariant in the sample
loop lets old GCC hoist it in the original order. The complete text and the
four owned data bytes match under `gcc_2_8_1_g8_split`.

The cache declarations live in `model_scene_setup.h`, the canonical loader
declaration in `model_load_step.h`, and the returning controller in
`model_debug_controller.h`. The controller includes the loader owner directly;
`model.h` does not provide a duplicate declaration or fallback.
The existing diagnostic owner supplies absolute-address string views while
the real definitions retain their eight-byte extents (four for the newline).

## Witnesses and limitations

The native controller and diagnostic data compile as separate ILP32
translation units at `-O0` and `-O2`. The fixture checks 158 scenarios,
1,997 formatting observations, and nine scripted random samples. Coverage
includes signed exit phases, load priority and completion, transfer gates,
null/live placement units, input wrap boundaries, unsigned ID displays,
both XOR flag bytes, live values across formatting calls, and slot-specific
property arity. Whole slot images, coordinate-unit images, guard bytes,
and unrelated halves of the shared words are checked. Four deliberately
mutated sources must fail.

Scripted RNG values cover rejection paths beyond the ordinary SDK RNG's
promised range; they are contract probes, not a claim that the game normally
produces all those values. The fixture models helper boundaries, not the
entire loader, GPU or input system.

Old-GCC ownership controls cover the actual definition and callers, signed
property caches, input widths, diagnostic extents and SDK field offsets.
A miniature little-endian link reproduces all `0x74` bytes of the original
small-data run, asserts the three ownership boundaries, and fails when
the controller's real word is omitted. This bounded proof does not replace
the complete executable and overlay match gates.
