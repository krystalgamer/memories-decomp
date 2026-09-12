# The loader state words at 0x8009B0F4 and 0x8009B134

## 0x8009B0F4

`D_8009B0F4` is the resident loader's request-and-state word. It is the most
widely shared global in the tree: 51 translation units declare and use it,
against 37 for the next-busiest address. One more, `func_80030D5C` (then in
`frontend_scene_states.c`, now `src/candidates/func_80030D5C.c`), reaches it
from inline assembly without declaring it.

Before this pass every one of those 51 units declared the word for itself, and
the declarations did not agree. Fifteen distinct spellings were in use:

| Spelling | Units |
|---|---:|
| `extern volatile u32 D_8009B0F4;` | 11 |
| `extern volatile s32 D_8009B0F4;` | 11 |
| `extern volatile u32 D_8009B0F4 __attribute__((section(".data")));` | 7 |
| `extern u32 D_8009B0F4 __attribute__((section(".data")));` | 5 |
| `extern u32 D_8009B0F4;` | 4 |
| `extern s32 D_8009B0F4 __attribute__((section(".data")));` | 3 |
| `extern volatile s32 D_8009B0F4 __attribute__((section(".data")));` | 2 |
| `extern u32 D_8009B0F4[];` | 2 |
| `extern s32 D_8009B0F4[];` | 2 |
| `extern s32 D_8009B0F4[2];` | 1 |
| `extern int D_8009B0F4[3];` | 1 |
| `extern s32 D_8009B0F4;` | 1 |
| `extern volatile int D_8009B0F4;` (co-declared) | 1 |
| `extern volatile s32 D_8009B0F4_signed asm("D_8009B0F4");` | 1 |
| `extern u32 D_8009B0F4_array[] asm("D_8009B0F4");` | 1 |

They disagreed about signedness, about `volatile`, about the section
attribute, and about whether the word is a scalar or an array of two, three or
an unspecified number of elements. `c_symbols.ld` also carried a third name,
`Base2_8009B0F4`, for the same address.

### What the word actually is

A single 32-bit value, read and written whole and only ever through bit masks.
No use anywhere indexes past element zero, so none of the array spellings
described the storage; they were a way of steering the addressing form. The
constants in `file_transfer.h` are already its bits:
`FILE_TRANSFER_STATE_PRIMARY_ACTIVE` (`0x10`),
`FILE_TRANSFER_STATE_SECONDARY_PENDING` (`0x20`),
`FILE_TRANSFER_STATE_PRIMARY_REQUEST_LOCKED` (`0x40`),
`FILE_TRANSFER_STATE_COMMAND_BUSY` (`0x400`),
`FILE_TRANSFER_STATE_POSITION_QUERY_BUSY` (`0x800`),
`FILE_TRANSFER_STATE_POSITION_QUERY_PENDING` (`0x1000`),
`FILE_TRANSFER_FLAG_SECTOR_RANGE` (`0x80000`) and the composite
`FILE_TRANSFER_REQUEST_BLOCKED_MASK` (`0x02000030`).

The four arbitration names follow complete producer/consumer paths.
`File_RequestAsyncTransfer` raises the request lock before touching the primary
descriptor, and `func_800144B8` refuses to promote the secondary request while
that lock survives. Every successful `DsCommand`/`DsPacket` submission in the
resident stepper raises command-busy, and every corresponding completion
callback clears it. `func_80014308` raises position-query-pending after the
preceding packet completes; `func_8001455C` then issues `DsCommand(0x10)` and
raises position-query-busy, which `func_80014390` clears on completion.

`volatile` is part of the type, not decoration. Dropping it from the plain
declaration builds a 0x1D0668-byte executable; dropping it from the absolute
declaration builds a 0x1D071C-byte one. Retail is 0x1D0800.

### Why two names survive

The retail image reaches 0x8009B0F4 through two different addressing forms.
Among the functions still held as assembly -- the 171 listings under
`src/candidates_target/`, four of them under overlay subdirectories --
`func_80013C28` uses `%gp_rel(D_8009B0F4)($gp)` on seven lines and
`func_80014220` on two, while ten other listings, one of them an overlay, use
`lui %hi` / `%lo` on eighty-six.

A `-G8` translation unit cannot produce both forms from one declaration,
because the assembler picks the form from whether the symbol is small-data
eligible, which maspsx communicates with an `.extern D_8009B0F4, 4` directive
it emits only for a symbol GCC treated as small. `file_transfer.h` therefore
declares two names for the one word:

```c
extern volatile u32 D_8009B0F4;
extern volatile u32 D_8009B0F4_abs __attribute__((section(".data")));
```

`c_symbols.ld` ties `D_8009B0F4_abs` to the same address. 29 `.c` files under
`src/` name `D_8009B0F4` as a whole word and 20 name `D_8009B0F4_abs`; no file
names both. The split follows the unit's
compiler profile: every unit on the absolute view is a `-G8` profile, and no
unit assembled at `-G0` needs it.

This is the same shape as `fade.h`, which already publishes
`gFade_State` and `D_800E9EC8_arr` for one address.

### Checked negatives

Four of the old spellings looked load-bearing and are not. Each was removed
and the full executable still matched:

- `D_8009B0F4_signed asm("D_8009B0F4")` in `file_transfer_flags.c`. Its uses
  are `|=`, `&` and a whole-word assignment, none of which depend on
  signedness, and the plain name reproduces them.
- `D_8009B0F4_array[] asm("D_8009B0F4")` in `movie_playback_control.c`, which
  carried a comment claiming the array-form load had to be preserved.
- `Base2_8009B0F4` in the same unit and in `c_symbols.ld`, which carried a
  comment claiming a separate linker name was needed to stop GCC retaining the
  address. That unit now reaches the word through one name in
  both of its statements, and the `c_symbols.ld` entry is gone.
- The two signed comparisons in `func_8001455C` within
  [`file_transfer_runtime.c`](../src/game/file_transfer_runtime.c) (`< 0` and
  `>= 0`, testing bit 31) do not need a signed declaration; an `(s32)` cast at
  the two use sites reproduces both sign-bit branches unchanged.

### What is left

`func_80030D5C` (now `src/candidates/func_80030D5C.c`) reaches the word from
an inline assembly block that spells `%hi`/`%lo` itself. That is not a C
declaration site and is unchanged.

Naming the word, and naming the remaining bits, is not attempted here.
`0x100`, `0x10000`, `0x20000`,
`0x100000`, `0x200000`, `0x400000`, `0x800000`, `0x2000000`, `0x40000000` and
bit 31 all have live consumers, and several are only ever cleared as part of
a composite mask (`0xFFDCFFFF`, `0xFFDDFFFF`, `0x230000`), so a single
consumer does not establish what an individual bit means.

## 0x8009B134

The other half of the same predicate. Eighteen units ask whether a transfer
is still in flight by evaluating
`(D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | D_8009B134`, and six of
them spelled the mask as a bare `0x2000030` or `0x02000030` rather than using
the constant that was already in `file_transfer.h`.

22 units declared this word too, in eleven spellings:

| Spelling | Units |
|---|---:|
| `extern s32 D_8009B134 __attribute__((section(".data")));` | 6 |
| `extern u32 D_8009B134 __attribute__((section(".data")));` | 4 |
| `extern u32 D_8009B134;` | 3 |
| `extern s32 D_8009B134;` | 3 |
| `extern volatile u32 D_8009B134;` | 1 |
| `extern volatile int D_8009B134;` (co-declared) | 1 |
| `extern u32 D_8009B134[];` | 1 |
| `extern s32 D_8009B134[];` | 1 |
| `extern s32 D_8009B134[2];` | 1 |
| `extern int D_8009B134[3];` | 1 |
| `extern s32 D_8009B134_signed asm("D_8009B134");` | 1 |

It behaves as a small state word rather than a bitfield: `func_80014FA4` and
`func_800144B8` raise it to `0x80`, the frame pump `func_80014A5C` latches
`0x40` into it exactly once (setting the primary descriptor's `done` to 5 and
its `substate` to 0) and clears the word outright when the primary transfer
is not active, and `File_InitTransferState` zeroes it along with the rest of
the loader block. As with 0x8009B0F4, nothing indexes it.

`u32`, two addressing views, same split rule. 8 units take the gp-relative
view and 14 take the absolute one.

### It is not volatile, and that is measured

This is the one place the two neighbouring words differ. `D_8009B0F4` needs
`volatile` and `D_8009B134` must not have it. Declaring 0x8009B134 volatile
compiles and links but overshoots `.text` by 16 bytes, because `func_80014A5C`
then re-loads the word for the `& 0x40` test and again for the `|= 0x40`,
where retail issues one `lw $3` and keeps it live across the zero test, the
bit test and the or. Two globals sixteen words apart in the same subsystem,
with opposite answers; neither was assumed.

`D_8009B134_signed asm("D_8009B134")` in `file_transfer_flags.c` is a fifth
checked negative: its only use is inside the blocked predicate, and the plain
name reproduces it.
