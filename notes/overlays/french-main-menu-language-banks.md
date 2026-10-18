# French main-menu language banks

The French SU archive contains five distinct 32 KiB main-menu images. The
existing `french_main_menu` registration selects language bank 0; the four
`french_main_menu_language_1` through `_4` registrations cover the remaining
physical images. They reuse all 31 accepted European main-menu C units,
unchanged, with the existing French symbol bindings and named profiles:
25 units use `gcc_2_8_1_g0_split`, while the six card comparators use
`gcc_2_8_1_g0_no_sched2_split`.
No new C implementation, declaration, compiler flag, or assembly fallback is
introduced.

## Runtime selection

The actual French `File_RequestMainMenuPackage` at `0x8006B560` and
`MainMenu_LoadPackageStage` at `0x8006B350` are defined by the selected
`src/game/european/main_menu_load_package_stage.c` object. The request starts
at `D_8009C02B * 136` sectors in SU. Stages 0, 1, and 2 consume 64, 32, and
2 sectors; stage 3 loads 16 sectors through the pointer at `0x8001002C`,
whose stored value is `0x80180000`. Stage 4 loads the following sector to
`0x801AF800`.

| Language index | Code sector | Complete-image SHA-256 |
|---|---|---|
| 0 | 98 | `50cb0bc724960a22d586fc38fc6d49cfcf4884a341cff463edbb9850bada5a40` |
| 1 | 234 | `afc3703f8a57196d54b6a15bd4a96679aef83ad7382c892d94fac11e809410f0` |
| 2 | 370 | `23f62a099f3bb7f5a65f0ba1d75fea741e7abbb25f6821351d7eda47ac820fc6` |
| 3 | 506 | `3ca844f9208d227f135c7579ba138955aba00a3bce01a1b65b078e7d829e7878` |
| 4 | 642 | `7156ea148be0550e54a10e91127d6fb3b1142dfae2c7bf631dcfa12236e9d58a` |

The canonical language byte `D_8009C02B` binds to French RAM `0x8009C44B`.
Its retail storage byte is zero; the actual French `Main_Init` C owner sets
the build default to 1. These are different observations, not competing
initialization claims. The live options input handler wraps language
selection through 0..4, and its update dispatcher passes the selected byte
to `func_80168D34`, which updates the resident language byte. The resident
`Main_RunMenu` calls the package request and the actual initialization,
update, and destruction entrypoints in this bank.

## Ownership and boundaries

All five images were independently extracted, freshly compiled, linked,
and compared in full. Their 31 function bodies are identical, but the
complete images are not; they must not be represented as equal-hash
duplicate sectors. Each image has these exact boundaries:

| Image offsets | Owner | Bytes |
|---|---|---|
| `0000..0004` | Generated raw header | 4 |
| `0004..001C` | Generated raw rodata | 24 |
| `001C..4784` | 31 selected C objects | 18,280 |
| `4784..8000` | Generated raw data | 14,460 |

Map-assisted relinking confirmed identical ELFs and 155 actual selected
C-function definitions plus 15 raw spans across the five images. Each
function has its measured address, size, executable section, and selected
input-object definition. The same source produces identical compiler objects
in every bank. Some final local data symbols are absolute aliases emitted by
Splat; their same-named definitions were independently verified inside the
actual selected raw-data object, at the correct offset and with a real size.
An absolute alias alone is not ownership evidence.

The existing exact French resident ELF and its selected objects supply
34 checked function bindings, including the loader, runner, initializer, and
language loader. SDK definitions remain SDK assembly, not game C.
Fourteen resident data bindings also have real backing input sections.
Five external resource bindings retain the accepted main-menu contract:
`D_801A8000`, `D_801AF800`, `D_801D1200`, `gDuel_adwCardStats` at
`0x801D4244`, and `gCard_asNameSortKey` at `0x801D4D8E`.
Initial resident backing is not proof of every later resource provider;
only the directly traced stage-4 transfer is claimed here.

The added coverage is four images, 124 C instances, and 73,120 instruction
bytes, not 124 newly discovered function implementations or whole-release
completion. Existing language bank 0 remains unchanged. Other runtime
images, unmatched functions, and unclassified material remain separate work.

## Reproduction

With the verified French retail inputs and local toolchain, run from the
repository root:

```sh
MAKEFLAGS=-j4 make french-match
MAKEFLAGS=-j4 make french-match-overlays
tools/environments/python/bin/python -m unittest \
  tools.project.tests.test_french_main_menu_banks \
  tools.project.tests.test_french_options
```

The focused checks cover physical sectors, unchanged C registrations and
profiles, exact instruction boundaries, distinct complete retail images,
real selected C definitions, all three raw spans, and the already accepted
options caller/storage ownership. Retail/build-dependent checks explicitly
skip when their inputs are absent; acceptance requires them to run.
