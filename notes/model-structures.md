# Model Shared Structures

This phase is limited to the shared model storage rooted at `D_800F2C40` and
the handler registry at `D_800F5918`. `src/game/model.h` records only locally
verified widths, offsets, strides, and extents. Offset-based names remain
where semantics, signedness, array bounds, or pointee layouts are uncertain.
GMS and the Unchiga decompilation/recompilation inventories were used as
corroborating evidence, not as sources of guessed reference types.

## `D_800F2C40`: three `0xE20`-byte slots

`ModelSlot` has size `0xE20`, and `D_800F2C40` is declared as three entries.
The stride is independently established by exact matching C and target
instructions in multiple functions:

- `func_80058E3C`, `func_80058E68`, and `func_80058E94` all compile the index
  as `((index * 8 - index) * 16 + index) * 32`, which is `index * 0xE20`,
  before accessing three different late-slot fields.
- `func_80058F74`, `Model_CopySlotU16Values`, `func_80059284`,
  `func_80059590`, and `func_80059AA8` independently produce the same stride
  while accessing unrelated pointers, arrays, and bytes.
- `func_80057E20`, `func_80059000`, `func_800592AC`, `func_800593D0`, and
  `func_800595C8` provide further exact target sequences with the same
  multiplication. GMS describes the base as `dword_800F2C40[904 * index]`,
  independently giving `904 * 4 == 0xE20`.
- Unchiga's same-address decompilation sources consistently use `0xE20`;
  its recompilation seed inventory identifies the same function family.
  Local recompilation and full-executable matching remain decisive.

The extent is also locally bounded. Linker aliases `D_800F39B0`,
`D_800F39F0`, and `D_800F3A10` are respectively `+0xD70`, `+0xDB0`, and
`+0xDD0` from the base, matching fields inside slot zero. `D_800F569F` is
`D_800F2C40 + 3 * 0xE20 - 1`, and `D_800F56A0` follows immediately. The
interior aliases retain their existing declarations and users.

Verified shared fields and partial arrays are:

| Offset | Shared member | Exact local evidence |
|---:|---|---|
| `0x000` | `field_000`, partial array of `0x8`-byte entries | `func_800593D0` indexes `(arg1 + 1) * 8`; `func_80059DD8` advances by 8 and reads the pointer at `+4` |
| `0x1E0` | `field_1E0`, partial pointer array | `func_800597C8` and `func_8005A468` advance pointers by 4, bounded at runtime by `field_E1B` |
| `0x7C4` | `field_7C4`, partial array of `0x76`-byte entries | `func_80058EC0` uses `field_BF5 * 118` and reads the leading `u16` |
| `0xBF5` | `field_BF5` | direct reads in `func_80058E68`, `func_80058EC0`, and `func_800597C8` |
| `0xCF8` | `field_CF8[10]` | `func_80057E20` and `func_80059000` read bytes `+7`, `+8`, and `+9` |
| `0xD14` | `field_D14` | 80-byte entry selection in `func_80058F20`, `func_80058F74`, and `func_800593D0` |
| `0xD18` | `field_D18` | `func_800592AC` repeatedly reads pointee halfwords `+0x44`, `+0x46`, and `+0x48` |
| `0xD70` | `field_D70[3]` | `Model_InitLightTriplet` writes three `0x10`-byte records |
| `0xDA0` | `field_DA0[3]` | three adjacent clamped `s32` writes in `func_800595C8` |
| `0xDB0` | `field_DB0` | four-word copy/reset block in `func_800594C0` |
| `0xDC0` | `field_DC0[8]` | byte writes in `func_80059590` and selection in `func_80059520` |
| `0xDC8` | `field_DC8[4]` | exact eight-byte copies in `func_80057E20` and `func_80059000`; element 3 is cleared by `func_800597C8` |
| `0xDD0` | `field_DD0[4]` | four adjacent `u16` reads in `Model_CopySlotU16Values` |
| `0xE00` | model-data size, `u16` | `Model_HasInsufficientBufferSpace` subtracts this value from the remaining model-data bank capacity |
| `0xE06` | `field_E06` | shifted read in `func_80058E94`; write/read in `func_800597C8` |
| `0xE0D` | `field_E0D` | `func_80058E3C` reads it and `func_8005969C` writes it |
| `0xE11` | `field_E11` | `func_800590DC`, `func_80059284`, and `func_800595C8` |
| `0xE12` | `field_E12` | read/conditional write in `func_80059AA8` |
| `0xE14` | `field_E14` | sentinel test in `func_80058DD8` |
| `0xE16`-`0xE18` | byte fields | notification test in `func_8005969C`; clamp/index fields in `func_80058F20` and `func_80058F74` |
| `0xE1A` | `field_E1A` | head-entry count in `func_80059DD8` |
| `0xE1B` | `field_E1B` | pointer-array count in `func_800597C8` and `func_8005A468` |
| `0xE1F` | `field_E1F` | status tests in `func_80058DD8` and `func_80059DD8` |

The one-element declarations at `field_000`, `field_1E0`, and `field_7C4`
express verified element layout and stride only. Runtime counts establish
that they are arrays, but the complete static bounds and intervening storage
are not yet proven, so the header does not guess them.

The initial typed-migration snapshot had 24 pure-C users of `D_800F2C40`;
all include the shared header:
`func_80057E20`, `func_80058DD8`, `func_80058E3C`, `func_80058E68`,
`func_80058E94`, `func_80058EC0`, `func_80058F20`, `func_80058F74`,
`Model_CopySlotU16Values`, `func_80059000`, `func_800590DC`,
`Model_InitLightTriplet`, `func_80059284`, `func_800592AC`,
`func_800593D0`, `func_800594C0`, `func_80059520`, `func_80059590`,
`func_800595C8`, `func_8005969C`, `func_800597C8`, `func_80059AA8`,
`func_80059DD8`, and `func_8005A468`.

## `D_800F5918`: 80 handler registry entries

`ModelHandlerRegistryEntry` is an eight-byte pair: `handler_value` at `+0`
and `key` at `+4`. `Model_RegisterHandlerKey` and `Model_FindHandlerKey`
both advance by eight bytes for exactly 80 iterations. The target assembly,
GMS's paired `dword_800F5918`/`dword_800F591C` views, and Unchiga's
same-address functions agree on the order and count. The modeled extent is
`80 * 8 == 0x280`; `D_800F5B98` starts exactly at `D_800F5918 + 0x280`.

Both pure-C users, `Model_RegisterHandlerKey` and `Model_FindHandlerKey`, use
the shared entry type and typed extern. Assembly users `func_8005FC1C` and
`func_8005FE44` remain unchanged.

## Migration snapshot and exact-code exceptions

After later matches, `notes/global-usage.csv` reports 27 matching-C users and
26 assembly users. The generated report is the authority for current status.
The 26 assembly users at this snapshot are:
`func_8004CB0C`, `func_8004D58C`, `func_8004D75C`, `func_8004D914`,
`func_8004EB00`, `func_8004FE2C`, `func_80050584`, `func_800507D0`,
`func_80050F24`, `func_8005106C`, `func_80051350`, `func_80051A48`,
`func_800528AC`, `func_80053248`, `func_800534B8`, `func_800540B4`,
`func_800556E8`, `func_800559D4`, `func_80056250`, `Model_LoadMonsterMerge`,
`func_80056828`, `func_80056D7C`, `func_800577B0`, `func_80057AF4`,
`func_800580D4`, and `func_80058938`.

Three additional matching-C functions retain raw model-base views:

- `func_8005611C` clears and initializes one `0xE20`-byte slot through
  explicit offsets while preserving its accepted compiler schedule.
- `func_80059700` uses a private late-slot layout for `type` and `state`
  while selecting the signed velocity passed to `func_8005A468`.
- `Model_HasInsufficientBufferSpace` includes `model.h` for
  `MODEL_SLOT_SIZE`, but retains a byte-array extern so its `+0xE00` size load
  keeps the accepted address construction.

Two typed pure-C functions retain raw local byte views:

- `Model_InitLightTriplet` keeps its offset writes. Replacing them with
  `field_D70[3]` member writes changed the exact instruction schedule, with
  the first mismatch at VRAM `0x8005914C`.
- `func_800593D0` keeps its byte-pointer chase through the leading
  eight-byte entries and `field_D14`. Direct `ModelSlotHeadEntry` member
  syntax changed the resident text size.

`func_80059000` retains its packed eight-byte local copy type because the
target uses `lwl`/`lwr` and `swl`/`swr`; the model base and source fields are
typed. These retained local views are code-generation constraints, not
competing declarations.

## Validation

Each migration batch was compared against the complete executable. The
final validation retained SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`
with `MAKEFLAGS=-j2 make match`. `make basic-types`, `make global-usage`,
and `make check-global-usage` also passed.
