# Memory-card work-controller state and frame checksum

| Former identity | Canonical name | Preserved view |
| --- | --- | --- |
| `D_8009B3CC` | `gMemCardWork_wFlags` | nonvolatile `u16` |
| `D_8009B3DD` | `gMemCardWork_bSlotIndex` | nonvolatile `u8` |
| `D_8009B3D5` | `gMemCardWork_bOperationResult` | nonvolatile `u8` |
| `D_8009B3C8` | `gMemCardWork_nRequestResult` | nonvolatile `s32`, original PAL assembler binding |
| `D_8009B3E4` | `gMemCardWork_nCompletedRequest` | nonvolatile `s32` |
| `D_8021007F` | `gMemCardWork_bFrameChecksum` | `u8`, `.data`, existing frame interior |

These are the request/event work controller's identities, not the separate
LIBMCRD modal dialog's flags, result word or step byte. The existing
[work-controller contract](memory-card-work-controller.md) retains its
operation, callback, workspace, source-group and inherited return-ABI details.

## Flags and the observed slot selector

`MemCardWork_InitializeOperation` writes flags `0x8000` and slot index zero
in its original six-store sequence. `MemCardWork_UpdateOperation` separately
tests and updates flags for request polling, result consumption, directory
diagnostics and message work. The name does not assign a meaning to every
bit; in particular, the initial `0x8000` bit is not given a new policy.

The slot byte directly indexes `gMemCardWork_Area.slots`. Its only observed
explicit writer stores zero. The existing bounded two-slot backing view
does not prove two active slots or make arbitrary byte indices safe.
Neither initialization nor indexing gains a clear, allocation, bound or
fallback.

## Native request outputs versus the controller result byte

The controller passes the completed-request word and request-result word
to `MemCard_ProcessRequest` as its first and second output pointers.
Only that function's finish path publishes them: it writes the native
I/O result, then the request id, clears the driver's request to `-1` and
returns `1`. Idle and busy returns do not publish fresh values.

The controller consumes those outputs under its existing flags and
completion condition. The request id is the last completed request,
not a continuously current request or a new busy flag. The result word
retains its raw SDK/driver result values.

The operation-result byte is independently derived by the controller.
Directory paths produce `0`, `1` or `2`; chunked read/write paths retain
their distinct progress and completion behavior. Successful intermediate
chunks can leave the byte unchanged. Exact `1`/`2` and zero/nonzero
comparisons remain, without a Boolean conversion or new result enum.

The initializer does not clear either output word or the result byte.
Existing conditional and unconditional create-file traces remain.
Request processing, signed/narrowed chunk arithmetic, message pumping,
transition dispatch and operation-state fallthrough retain their order.
The shared transition byte `D_8009B3C1` is not renamed as work-only state.

## PAL request-result linkage is a distinct surface

The European and Spanish dialog wrappers already give the work-result
C identifier `European_D_8009B3C8` the explicit assembler label
`D_8009B3F0`. This provider is at `0x8009C324` in the UK and
`0x8009C744` in France, Germany, Italy and Spain.

It is not `gEuropean_D_8009B3F0`, the modal-runtime word four bytes earlier.
The canonical declaration therefore retains `asm("D_8009B3F0")` under
the existing `VERSION_EUROPE` selection. Other consumers keep the plain
`s32` view. The old wrappers, declarations and aliases remain unchanged.

Actual PAL compiler outputs still import that original assembler label;
the canonical C identifier is not substituted for it in object linkage.
The native canonical linker alias also targets that same work provider.
Compiler conservation tracks effective assembler identities separately
from CPP-expanded C identifiers.

## Checksum remains an interior byte

The existing sector-write setup XORs frame bytes `0..0x7E` and stores the
result at `+0x7F` before requesting the original sector write.
The checksum's canonical name is another identity for that existing byte,
not a separate allocation or a relocated field.

The CPU-addressed 128-byte frame, `.data` declaration, literal frame reads,
five other footer-byte stores, checksum extent and signed sector
conversion remain unchanged. The other footer bytes are not assigned
guessed semantics, and no RAM-mirror equivalence is inferred.

## Namespace and conservation scope

The new header follows the old `mem_card_work.h` owner in the two resident
C sources. All six identities remain externally provided; native linker
aliases use canonical names equal to their original providers.
There are 42 active regional roles. Local maps establish 37 numeric
bindings; five original Japanese/UK imports remain symbolic rather than
being assigned guessed addresses. Fresh regional full-image PR checks
remain mandatory.

The existing controller witness changes only identifiers and gains one
linker namespace alias after the existing fixed-address alias loop:
canonical checksum equal to its existing legacy checksum provider.
The old loop is unchanged. No
physical address, fixture, case, assertion, mutation, startup or compiler
option is changed. Reversing the identifiers and removing that one alias
restores the entire file.

The separate legacy frame/ownership probe and its wrong-checksum-alias
negative control stay frozen, as do the old header, wrappers, modal
runtime, profiles and source groups. No harness, fixture, probe or test
case is added; cloud/regional/overlay/witness suites are not run locally.
Conservation compares complete ordinary, native compiler/MASPSX and
fallback assembly, with only scoped original assembler identities
reversed and no instruction, directive, relocation or order filtering.
