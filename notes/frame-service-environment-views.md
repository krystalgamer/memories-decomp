# Frame-service countdown and drawing-environment views

The system-service and graphics-frame units name two existing data roles:

| Former identity | Canonical name | Representation |
|---|---|---|
| `runtime_gp` | `gMain_nServiceResyncCountdown` | signed `s32` |
| `D_800FE048`, `gLibGs_DrawEnvView` | `GsDRAWENV` (libgs) | scalar `DRAWENV` |

## Countdown, not an inferred monitor protocol

`Main_RunFrameServices` first runs its existing rendering and service
callbacks. It then keeps the original signed comparisons of the two monitor
pairs. If either comparison passes, it selects the reset path immediately.
Otherwise it decrements the countdown, stores it and selects reset only
when the signed result is negative.

Reset stores `0x3C` and copies the two existing current words to their
paired words. The new name describes this exact countdown/resynchronization
role, not the unknown meaning of those four words. No event handle, queue,
heap, passive-snapshot, crash-reset or elapsed-time interpretation is added.
In particular, the decrement-before-underflow behavior is not replaced by
a claim that reset occurs after exactly sixty idle calls.

`main_services.c` defines the initialized word as
`gMain_nServiceResyncCountdown` in `.sdata`, and every region binds that name
directly; the former `runtime_gp` symbol and its link alias are gone.
Its regional placement is unchanged.

## SDK object with a distinct game-facing view

The original SDK address/type evidence is recorded in
[`graphics-frame-environments.md`](graphics-frame-environments.md).
`GsSetDrawBuffClip` passes the same native base to `PutDrawEnv` as a
`DRAWENV *`; the game writes the existing dither, background-clear and RGB
bytes at their independently asserted offsets.

The game writes the libgs `GsDRAWENV` scalar directly. Startup stores
through a volatile cast at each use, while frame updates use the plain name.
Every byte store retains its original order and value, including startup's
ones. No storage allocation or array length is introduced.
The four bytes following the `0x5C` environment remain unclassified.

## Actual image providers

| Image | Countdown word | Drawing environment |
|---|---|---|
| North America | `0x8009AF08` | `0x800FE048` |
| Japan | `0x8009AE48` | `0x800FB738` |
| United Kingdom | `0x8009BE84` | `0x800FEFB0` |
| France | `0x8009C298` | `0x800FF3D0` |
| Germany | `0x8009C298` | `0x800FF3D0` |
| Italy | `0x8009C298` | `0x800FF3D0` |
| Spain | `0x8009C298` | `0x800FF3D0` |

The `_French` compatibility label at the NA environment address is not the
actual French provider. Canonical exports alias the selected old native
identities rather than introducing new per-image address literals.

## Conservation

Header aliases expand to the original data identifiers before compilation.
The initialized word, both environment declaration arms, SDK bodies,
callback order, monitor words, scalar/array accesses and all pointer policy
remain unchanged. No C declaration or layout changes.

The final ELF keeps all original symbol metadata and adds only the two
same-value aliases. Their symbol types follow the existing providers;
NA's initialized countdown is section-relative data while its SDK
environment label is absolute. No whole-ELF byte-identity claim is made.

Existing witnesses, oracles, guards, mutation controls and compiler flags
retain their complete bytes. No new harness, generated C, fixture or
runtime logic is introduced.
