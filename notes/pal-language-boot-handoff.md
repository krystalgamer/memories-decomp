# PAL language package and debug-font handoff

Two European resident helpers connect `Main_RunBootSequence` to the
existing PAL language-options commit path:

| Canonical name | Native ABI label | UK address / size | Other PAL address / size |
|---|---|---|---|
| `Main_InitDebugFont` | `func_80043B7C` | `0x80043B7C` / `0x4C` | `0x80043D7C` / `0x4C` |
| `Main_RequestLanguageBootPackage` | `func_80043BC8` | `0x80043BC8` / `0x74` | `0x80043DC8` / `0x74` |

These are source names for existing functions, not new implementations,
resident storage or linker-defined replacements. Their original native
labels and plain compatibility prototypes remain; explicit extern/asm
declarations in the existing owning `main_run_boot_sequence.h` bind the
semantic definitions and calls to those same labels. The regional
inventories change only the two name fields in each of five releases.

## Request, optional wait and font setup

The request helper retains the full original signature
`void (s32 language, s32 wait)` and passes all eight original arguments to
`File_RequestAsyncTransfer`: language selects sector
`language * 110 + 0x1962`, the request spans 110 sectors, and
`Main_LoadBootPackageStage` is the staged callback. It does not clamp or
validate the signed language, normalize the wait word, expose a success
result or check the transfer request's return value.

Any nonzero wait still calls `File_WaitForTransfers` before font setup.
It waits through the existing transfer mechanism, not an invented
language-specific completion object. With zero wait, the helper only
requests the package; no font setup, polling or synchronization is added.

The font helper retains `FntLoad(0x2C0, 0)`, followed by `FntOpen` at
`(0x10, 0x10)` using the existing regional width/height, zero background
flag and 1,000-character buffer, followed by `SetDumpFnt`. It opens and
selects the debug-font context as well as loading the font; the name does
not assert an exclusive startup lifetime or a recovered handle owner.

## Both caller lifecycles remain distinct

The nonzero boot mode requests the current resident language with wait1
before sound-link replacement and four frame advances. The cold path
keeps both logos, BIOS-region module calls, CD reinitialization and frame
holds; it stores the build language and requests that package with wait0,
then retains fade/reset, the original transfer-state/cancel wait, debug
font setup and the main-menu package request. The final transfer wait
still runs in its original place. Japanese and NA implementations are
untouched and do not call these PAL-only helpers.

`PalOptions_RequestLanguage` still suppresses a request when the language
matches, otherwise stores `gText_bLanguageIndex` before requesting with
wait0. No rollback, validation or duplicate-request behavior is invented.
The existing update state3 requests once, fades sound/display, and waits
for both fade completion and the original transfer/cancel conditions
before `PalOptions_StoreLanguageImage`.

That image routine still sets up the font before its `0x30` by `0x10`
VRAM save, blocking draw sync, language-byte store, upload and second
sync. The existing rectangles, stored transfer-buffer pointer, byte
offsets and provider views are unchanged. Calling the font helper here
is not replaced with an assumed generic language-save callback.

## Complete native and control conservation

Thirty unique actual source/profile routes consume the owning header:
seven complete boot-sequence units, seven complete main-init units and
sixteen complete PAL options units. The latter are shared by the tracked
French and Spanish images; every active regional manifest is accounted.
No wrapper, named compiler profile, native symbol map, overlay linker
import, aggregate declaration or existing test/control changes.

Untouched baselines and candidates retain complete compiler and MASPSX
assembly, symbol tables and disassembly/relocations in all thirty units.
The two named resident functions retain their combined 192-byte extent.
Including the unchanged boot caller and main-init unit, the resident
coverage is 2 functions / 1,008 instruction bytes in NA, 2 / 1,132 in
Japan, 4 / 1,172 in UK and 4 / 1,180 in each other PAL release. The
sixteen options definitions span 4,664 bytes in each tracked image.

In particular, existing French/Spanish controls still require the old
native undefined imports, section-defined resident owners and exact
retail bodies. Semantic calls compile to those unchanged native names,
so neither dependency sets nor ownership/body cases require migration.
No new synthetic/generated-C/source-string harness or testcase is added,
and no cloud/regional/witness suite is executed locally.

The clean complete NA resident retains SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
Native conservation and that NA gate are not complete local PAL image
execution: ordinary applicable exact-head regional PR gates remain
mandatory. See also the earlier
[French options ownership evidence](overlays/french-options.md) and the
[boot/frontend sequence](boot-frontend-sequence.md).
