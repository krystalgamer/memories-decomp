# Continuous Game Decompilation Plan

## Campaign baseline

The table below records the inventory snapshot used to launch this bounded
campaign. It is retained for historical context rather than updated as live
progress; the generated progress section in the root
[`README.md`](../README.md) is the authority for current totals.

The resident inventory contains 1,794 functions:

| Category | Count |
|---|---:|
| Game functions | 1,196 |
| Psy-Q CRT/SDK functions | 598 |
| Matching-C game functions | 786 |
| Matching sources still containing GCC inline assembly | 58 |
| Terminal compiler-generated game assembly | 347 |
| Intentional handwritten game assembly | 63 |

The previous bounded campaign produced terminal histories for every remaining
compiler-generated assembly function. Those histories are research indexes,
not a declaration that the project is complete.

## Completion target

Continue the decompile, name, group, and progress loop until:

- every game-owned compiler-generated function is exact matching pure C;
- every matching source is free of inline assembly;
- every game function has an evidence-backed semantic name and concise purpose;
- every function is grouped with its subsystem when address order, compiler
  profile, and declarations permit it;
- any game assembly that remains is independently justified as intentional
  handwritten or hardware-facing code and is semantically documented;
- the complete executable remains byte-identical after every accepted change.

Terminal mismatch histories do not satisfy this completion target. They prevent
blind repetition and identify the concrete compiler/source blocker that future
evidence must resolve.

## Non-negotiable rules

- Never decompile the 598 Psy-Q CRT/SDK functions.
- Try GCC 2.8.1 with MASPSX 2.81 first.
- Use GCC 2.7.2 with MASPSX 2.72 only when code evidence supports the DOS
  cohort or the 2.8.1 budget is exhausted.
- Never repeat a terminal source/profile experiment without genuinely new
  compiler, source, structure, or runtime evidence.
- Preserve the original six-attempt ledgers as immutable history. Any
  post-terminal investigation must begin from one concrete new discriminator,
  remain bounded, and record its result before another variant is considered.
- Treat each terminal history as a set of measurements against specific source
  shapes, not proof of a general capability or compiler ceiling. Audit the full
  history with `make review-deferred` before relying on its explanation.
- Record an exact post-terminal pure-C resolution as
  `post_terminal_resolution` evidence; this one-shot success record does not
  rewrite or append speculative variants to the original six-attempt
  canonical or inline-refinement history.
- Keep exact assembly until a replacement passes the complete executable match.
- Treat GenericMadScientist's IDA decompilation (`gms.c`), Unchiga's decomp,
  and Unchiga's recomp as evidence, not ground truth. Do not copy their guessed
  types.
- Run matching work sequentially; `make` may use two jobs.
- Keep all work inside this repository and all generated candidates/logs under
  `tmp/`.
- Keep commits atomic and push accumulated commits only when about 15 minutes
  have elapsed; never wait idly for the push interval.

## Continuous loop

### 1. Rank unresolved functions

Rebuild the queue from the current inventory and terminal evidence:

1. A recovered exact source or compiler fingerprint not previously tested.
2. One- or two-word residuals with a specific control-flow, declaration, or
   scheduler discriminator.
3. Same-address GenericMadScientist or Unchiga bodies corroborated by local
   callers and data layout.
4. Contiguous subsystem chains where a shared type or declaration can resolve
   several functions.
5. Larger functions with exact-size and exact-relocation candidates.

Do not rerun Unchiga's exhaustive `msearch.py` cross-products. They can contain
hundreds or thousands of variants and would violate this project's six-variant
policy. Extract only a concrete recorded source or residual discriminator.

### 2. Recover and verify source

For one ranked target:

1. Read the target assembly, all preserved candidates, callers/callees, globals,
   GenericMadScientist pseudocode, and Unchiga evidence.
2. State the new discriminator before compiling another candidate.
3. Verify field widths, signedness, prototypes, and aliasing locally.
4. Compile sequentially under the evidence-backed profile.
5. Integrate only exact code and relocation matches.
6. Run the complete executable match immediately.

An exact linked-byte candidate may use a C integer address for an isolated
absolute store only under the documented literal-address exception. Inline
assembly transcription is not a decompilation result.

### 3. Assign semantic meaning

For every accepted match:

1. Derive the role from the matched body, call graph, strings, and data layout.
2. Use project naming conventions and subsystem prefixes.
3. Record confidence, evidence, caveats, and a concise description.
4. Apply the name across inventory, source, symbols, and references with the
   semantic-name tooling.
5. Leave the address-based name when the role is not yet supported; do not
   guess merely to increase the naming count.

### 4. Consolidate the subsystem

After a coherent set matches:

1. Merge contiguous same-profile functions into a subsystem translation unit
   when the grouped-unit invariants hold.
2. Reuse shared headers and structures only where multiple exact functions
   establish compatible layouts.
3. Regenerate the global-use map when declarations or structures change.
4. Preserve raw accesses wherever typed expressions alter exact codegen.

### 5. Publish progress

After each accepted wave:

1. Run `make progress` to refresh the generated README metrics.
2. Update the campaign notes with accepted matches and newly established
   blockers.
3. Run the smallest targeted checks followed by the complete audit.
4. Commit one logical change at a time.
5. Continue working between pushes and synchronize only when the push cadence
   permits it.

Then rebuild the unresolved queue and repeat from step 1.

## Current research priorities

- Resolve compiler-origin questions shared by the register-allocation and
  scheduler residual families before retrying their members.
- Recover external jump-table placement so semantically solved switch functions
  can reference the retail tables exactly.
- Convert the 59 matching inline-assembly sources to pure C when a new compiler
  or source discriminator is available.
- Use the model, sound, AI, duel, file, and main-loop structures already
  recovered to revisit terminal candidates with corrected declaration shapes.
- Expand semantic descriptions alongside matches instead of postponing all
  naming to a final bulk pass.

The queue is exhausted only at the completion target above, not when every
function merely has a terminal attempt history.
