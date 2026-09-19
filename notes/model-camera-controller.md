# Camera move and follow controller

`func_80051A48` (`0x80051A48`, `0xAE0` bytes) runs at the end of
`Model_UpdateScene`'s model-scene frame. Its matching C uses the existing
`ModelCameraMove`, `ModelSlot`, `GsRVIEW2` and `Key` owners without adding
storage, aliases, volatile accesses, register pins or executable assembly.
`camera_view.h` supplies its declaration to both definition and caller.

## Retail contract

- All four signed slot selectors are captured before the transfer-status call.
  Negative selectors disable tracking. Nonnegative selectors retain the retail
  unchecked indexing contract; the canonical slot array contains three records.
  No evidence here licenses selectors outside that array.
- In the active-move path, each paired start tracks X/Y only; each end tracks
  X/Y/Z. Signed deltas are limited to `[-30, 30]` before halfword stores.
  Flags select eye and target interpolation independently, including moves
  to explicit coordinates rather than model slots.
- Elapsed advancement halves the signed frame step near either fifth of the
  duration, with a minimum of two in those edge regions. Interior steps are
  not subject to that minimum. The `0xFFFF` duration sentinel skips advancement,
  not tracking, interpolation or the completion comparison.
- Interpolation uses signed full-width differences, products and division,
  then adds the start coordinate. Camera outputs are 32-bit SDK `long` fields.
  Both interpolated and followed Y coordinates have a ceiling of -100.
- An inactive transfer gate or zero flags selects the follow path. Mode zero
  returns without a key lookup. Otherwise end slots follow directly, and orbit
  is suppressed if either requested key endpoint has signed kind 4.
  `func_8005FB14` retains its existing integer-address return ABI: the real
  implementation returns zero or the current `Key *` converted to `s32`.
- Orbit retains the real signed direction byte and the existing caller view
  that places an unused fifth argument on the stack. This is not a change to
  the four-argument callee definition.
- Active moves recompute metrics, perform view adjustment and only then check
  completion. Follow-mode countdown may call the frame-step getter twice;
  both field reads are explicitly sequenced after their calls. The second
  subtraction is not independently clamped.

The normal setup in `func_80052D2C` handles a zero-duration request immediately
and clears its flags. This does not establish that every other writer prevents
zero duration: `func_8005A074` writes a duration directly. The controller's
retail division behavior is preserved, without inventing a zero-divisor
fallback or general signed-overflow guarantee.

## Matching and witnesses

The full reconstruction replaces the historical incomplete controller spines.
Separate two-element selector initializers recover the `0x38` stack frame.
Separate base and indexed-slot locals, nested conditional clamp additions,
quotient-first direct interpolation, and a scoped elapsed temporary recover
the retail allocation and scheduling. An append-only refinement makes the
countdown getter/load ordering explicit without changing any instruction byte.
The original terminal source snapshot remains separate from that refinement.

`test_model_camera_controller.py` compiles the real C in a separate ILP32
translation unit at `-O0` and `-O2`. Its numeric-offset oracle checks 220
scenarios and 1,040 callback observations, full move/view images, untouched
slot storage and surrounding guards. Cases cover gates, flag combinations,
negative and valid selectors, clamp and easing boundaries, signed direction,
sentinel duration, endpoint-kind exclusions, variable countdown returns and
helper-boundary mutations. Six deliberate source mutations must be rejected.
These scripted mutations stress observable load ordering; they do not claim
that the current leaf frame getter actually mutates the move record.

`test_model_camera_ownership.py` uses GCC 2.8.1 for positive and negative
declaration controls, measured field offsets and widths, real definition/caller
compilation, and the explicit five-argument orbit caller view. Neither suite
replaces clean resident-executable or overlay matching.
