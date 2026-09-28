# French resident matching

The French `SLES-03948` target is built with the existing GCC 2.8.1/MASPSX
pipeline and shared regional linker driver. Its complete executable SHA-256 is
`57ecdfb9a9e1faf8b342fe7c7304c23723810861f3ab2fa3bef9eb27b5146b44`.

Supply legally obtained inputs at `game/france/SLES_039.48`,
`game/france/DATA/SU.MRG`, and `game/france/DATA/WA_MRG.MRG`. Their sizes and
hashes are fixed in `config/sles_03948/target.yaml` and `files.sha256`. The
French CI workflow obtains these from `YGOFM_SLES_03948_URL`,
`YGOFM_FRA_SU_MRG_URL`, and `YGOFM_FRA_WA_MRG_URL`, respectively.

From the repository root:

```sh
make verify-french-inputs
MAKEFLAGS=-j4 make french-match
```

`french-match` cleans generated split/build output, compiles the configured C,
assembles remaining text, and compares the complete executable. It does not
accept function-only or relocation-normalized comparisons. All other regional
build targets and matching sources remain unchanged.

## First resident batch

The first batch matches **53 functions / 4,228 bytes** using **41 existing
European/shared translation units**, without adding or copying C. The named
compiler profiles are unchanged from the European matches. Each grouped
translation unit preserves source-definition order, contiguous function
coverage, and one profile. The inventory names and exact sizes of the C
functions were verified against the linked French ELF.

The correspondence CSV attached to issue #6460 was used only to select
candidate addresses. For each selected complete source group, French input
bytes at those addresses equal the European function bytes. Compiling and
linking every selected group then reproduces the entire French input exactly;
the CSV alone is not acceptance evidence.

The experiment sequence was:

| Candidate | Result |
|---|---|
| Same-address reuse | No complete matching C functions; French text and data relocations differ. |
| Correspondence-address reuse, unchanged European profiles | 53 functions in 41 whole translation units have identical input instruction bytes. |
| Link those C groups without named data aliases | Unresolved references to six shared data symbols; no source/compiler change required. |
| Resolve the six symbols from the identical absolute-address instruction pairs | Exact complete executable, including a clean `make french-match`. |

The six data aliases in `link_symbols.ld` retain their European addresses:
the instruction pairs that reference them are unchanged in the French image.
This is not a general assumption that French globals share European addresses.
The remaining large binary-backed tail is preserved, not declared decompiled.

## Relocation-backed resident expansion

The relocation-backed batch matches **726 functions / 214,696 bytes** from
**431 unchanged European/shared translation units**. This adds **673
functions / 210,468 bytes** while retaining every source, profile, address,
and size from the first batch. No new C, compiler flags, or inline assembly
are introduced.

The European executable was rebuilt exactly before its objects were used
as reference evidence. Candidate selection required whole-source contiguity,
unchanged non-relocation instruction bits, and no additional allocated data
sections. French addresses were then recovered from J26, absolute 32-bit,
HI16/LO16, and GP-relative relocations. Object addends and the European ELF
symbol values validate each interpretation; French GP is `0x8009C298`.
Bindings must agree across all uses and with the actual addresses of selected
C definitions.

The relocation scan examined 7,527 sites and recovered 1,085 distinct external
symbol bindings. Thirteen source groups with unsupported common-symbol
ownership were deferred, not forced into C.
Groups requiring separate rodata, small-data or BSS integration remain for a
later batch. These screening decisions do not replace the final image gate.

| Experiment | Result |
|---|---|
| 726 functions with recovered legacy C bindings | Link rejected an address-name collision between relocated C and generated French assembly. |
| Disambiguate the overlapping C function name | Link succeeded, but 120 instruction words differed: generated French assembly was using legacy numeric aliases intended only for the shared C. |
| Give conflicting generated French symbols an address-based `_French` suffix | Exact full executable, followed by a clean production `make french-match`. |

The suffixes distinguish symbol namespaces; they do not assert new semantics.
The legacy C aliases in `link_symbols.ld` keep the recovered French address,
while the explicitly named French symbols retain their actual numeric
addresses. Generated Splat files were not patched. The shared source still
compiles normally, and the existing assembly fallback still reconstructs its
original bytes.

## Rodata-backed resident expansion

The rodata-backed batch matches **806 functions / 269,960 text bytes** in
**462 unchanged European/shared translation units**. This batch adds **80
functions / 55,264 text bytes** from **31 complete translation units**, with
**2,149 bytes of compiler-owned rodata**. Every previous matched address,
size, source, and compiler profile is retained. The added sources contain
neither inline assembly nor source-local extern declarations.

This includes transfer control, duel package loading and ritual effects,
library and menu control, memory-card handling, model packet processing, and
AI card queries. The European inventory establishes game ownership; SDK
callees remain external bindings, not newly claimed C.

The verified European objects and linked ELF were used to interpret **4,090
relocation sites**. Local rodata bases are recovered from French text
references, rather than inferred from the correspondence CSV. Object
addends and European symbol addresses disambiguate reused HI16/LO16 pairs.
Local section references, jump tables, GP-relative references, and external
bindings must agree. The original sources and named profiles are unchanged.

The split assigns each recovered rodata interval to its owning C object.
Unclaimed data remains generated assembly, with explicit original padding.
Symbols defined by linked C objects are not duplicated as absolute linker
aliases: this preserves their ELF type, size, and ownership as well as bytes.
The accepted inventory checks every C function against its exact linked
address and size.

| Experiment | Result |
|---|---|
| Link all 31 rodata-backed groups with recovered bindings | Splat rejected the old generated `func_8003C3C4_French` name at the newly matched `File_RequestEgyptOverworldPackage` address. |
| Retire that replaced fallback alias | The complete image differed starting at file offset `0x1F82`: generated raw rodata omitted two trailing zero bytes, shifting later sections. |
| Preserve the explicit two-byte pad and normal object alignment | The entire executable matched byte-for-byte. |
| Validate exact ELF C symbols | An unnecessary absolute alias shadowed an existing C definition; remove aliases for all linked C definitions, not only the new group. |
| Limit function-namespace disambiguation to actual fallback symbols | Avoid artificial disassembler boundaries while retaining the exact image and all 1,838 provisional inventory boundaries. |
| Clean production build and linked-inventory validation | 806 exact linked C functions; complete executable SHA-256 matches the fixed French target. |

No COMMON, small-data, or BSS ownership is guessed for this batch. Unsupported
groups remain assembly fallback. Full-image matching, rather than a masked
relocation comparison or function-only check, is the acceptance criterion.

## Measured COMMON-backed resident expansion

This independent batch adds **16 functions / 20,260 text bytes** in **15
unchanged European/shared translation units**, including **140 bytes of
compiler-owned rodata**. Its baseline has 806 resident matches; the batch
build contains **822 functions / 290,220 text bytes** in **477 translation
units**. Every baseline source, profile, address, and size is retained.

The matching groups cover duel placement, field and battle actions; main
mode transitions; dialog/text-box dispatch; and controller backup/restore.
They use the existing headers and named profiles, without adding C, inline
assembly, or source-local extern declarations. European inventory ownership
identifies them as game code, not SDK implementations.

The earlier relocation campaign deferred COMMON definitions. Here, the
verified European objects/ELF and French relocation sites establish their
actual French addresses independently. The scan examines 1,633 sites across
the considered groups, including the deferred group below. All 29 COMMON
declarations in accepted groups resolve to 21 distinct measured symbols.
Each fits the startup-cleared BSS interval `0x8009C408..0x800FFC30`, with its
object-declared size and alignment. Repeated declarations must agree.

For example, `D_8009B26C` consistently resolves to `0x8009C60A` across the
main-mode handlers. The unused `D_8009B26E` declaration in the options group
uses its previously measured binding, independently confirmed by the trade
group at `0x8009C60B`. The six input backup values are individually recovered;
their order is not inferred from declaration order or a regional delta.

These COMMON declarations bind to existing, measured runtime storage. This
does not allocate replacement BSS or claim the raw resident tail as C-owned.
The existing regional linker mechanism resolves those addresses; the full
executable and startup clear bounds remain unchanged. Three groups additionally
own their recovered rodata ranges, with jump-table relocations checked.

| Experiment | Result |
|---|---|
| Reconsider deferred COMMON groups and three rodata/COMMON groups | 15 whole groups have consistent relocations and all COMMON storage inside verified startup BSS. |
| Apply the same BSS ownership requirement to `func_800540b4.c` | Deferred: `D_8009BF68` resolves to `0x8009C370`, outside startup-cleared BSS. No guessed placement or forced C promotion. |
| Link the 15 supported groups with existing profiles and measured bindings | Exact complete French executable. |
| Integrate with the separately merged rodata batch and repeat clean production/ELF verification | 822 exact linked C functions; the full executable retains its fixed French SHA-256. |

Initialized small-data, local SBSS, and the rejected out-of-range COMMON
case remain separate work. Partial instruction or relocation-masked agreement
is never sufficient for acceptance.

## Compiler-owned initialized small data

This batch adds **11 functions / 4,772 text bytes** from the unchanged
`european/duel_trap_resolution.c` and `duel_card_effects.c` translation units.
Their existing named profiles produce **19 bytes of initialized `.sdata`**.
The batch retains all prior matches and introduces no new C, inline assembly,
source-local extern declarations, or compiler flags.

French GP-relative references recover three array addresses independently:

| Compiler-defined array | French address |
|---|---|
| `gDuel_abTrapAttackThresholds` | `0x8009C2B4` |
| `gDuel_abLifePointRecoveryUnits` | `0x8009C2C8` |
| `gDuel_abDirectDamageUnits` | `0x8009C2D0` |

Five local-data relocation sites agree with the exact European ELF values
after accounting for object addends and each region's GP. The two compiler
sections, of 6 and 13 bytes, equal the corresponding retail data byte-for-byte.
The wider scan checks 287 relocation sites and consistent external bindings.

The split now assigns those exact intervals to the compiled C objects.
Untouched intervening and trailing bytes remain binary-backed `.sdata`-order
gaps, with the original alignment byte preserved. The rest of the tail is
not claimed as C-owned. The arrays are real linked section definitions, not
absolute aliases substituted for their initializers.

The first complete candidate linked exactly. A subsequent clean
`make french-match`, linked-function inventory check, and explicit check of
the three non-absolute data symbols confirm the complete executable hash.
After additive integration with the merged COMMON-backed batch and another
clean acceptance build, the resident manifest contains **833 functions /
294,992 text bytes** in **479 translation units**, preserving all 822
previous matched entries.

## Preserved-storage resident matches

This independent batch adds **3 functions / 6,396 text bytes** from unchanged
`graphics_frame.c` and `european/func_800540b4.c`, using their existing named
profiles. After additive integration with the merged initialized-data batch,
its 833-function baseline becomes **836 functions / 301,388 text bytes** in
**481 translation units**, with every prior matched entry retained.
No C, inline assembly, source-local extern declaration, or profile is added.

The scan validates 234 relocation sites. All ten COMMON declarations have
measured French references, consistent alignment, and preserved storage bytes
equal to their verified European counterparts. The eight graphics declarations
use the same address-binding mechanism as the European build.

The earlier BSS-only screen correctly deferred `func_800540b4.c`: its two
tentative definitions are not startup-cleared BSS. European ELF provenance
instead identifies them as `.initialized_data`; French references independently
recover `D_8009BF68` at `0x8009C370` (4 bytes) and `D_8009BF6C` at `0x8009C374`
(1 byte). Those original bytes remain preserved. Their tentative C declarations
do not allocate new storage or claim the initialized region as C-owned.

`graphics_frame.c` also defines two unused viewport scalars in `.sbss`.
There are no viewport relocations in this object, so they cannot be recovered
from this candidate alone. The exact European ELF resolves both as `SHN_ABS`;
the French linker already has independently established bindings at
`0x8009C4C0` and `0x8009C4C2` from earlier matched users. The object offsets
agree with those bindings, and its four zero bytes agree with preserved
French storage. The batch retains those bindings unchanged and does not add
a replacement SBSS allocation.

| Experiment | Result |
|---|---|
| Require COMMON declarations to reside in startup-cleared BSS | The initialized-data case was deferred in the earlier batch, not force-promoted. |
| Verify initialized-data provenance and recover both French references | `func_800540b4.c` matches as a complete C object and full executable. |
| Require viewport relocations in `graphics_frame.c` | No such references exist; both declarations are unused in this object. |
| Reuse the already verified viewport bindings, checking ELF provenance and object offsets | Both whole source groups match, followed by clean production matching and exact linked-function inventory verification. |

The acceptance hash remains the full French executable SHA-256, not a
function-only or relocation-normalized comparison. Other regional sources
and all French overlay manifests are unchanged.

## Whole-source recovery beyond the correspondence CSV

This batch adds **243 functions / 40,536 text bytes** in **55 whole existing
European/shared translation units**. After integrating the separately merged
storage batch, its 836-function baseline becomes **1,079 functions / 341,924
text bytes** in **536 translation units**, retaining
every previous matched source, profile, address, and size.

Missing or incomplete correspondence rows are not treated as evidence that
a shared source is incompatible. The verified European object identifies
relocation fields; unchanged instruction runs locate candidates only within
French resident text. The complete source's non-relocation bits must match
at exactly one location, with its original function order and contiguous
coverage. Overlaps with existing C or another candidate are rejected.

This recovered 57 candidate groups / 247 functions, but that screen is not
acceptance. Interpretation of 2,094 relocation sites then checks object
addends, European ELF values, French references, repeated bindings, and local
section addresses. Two groups remain deferred:

- `european/debug_effect_screen.c`: references to
  `gDebugEffect_abPreviewState` imply both `0x8009C2C2` and `0x8009C2C0`;
  a single alias cannot resolve the regional layout difference.
- `func_8004E7B0.c`: the initial bytes at recovered `D_8009AF88` differ from
  the European storage. Its initialized-pointer ownership needs further
  evidence rather than bypassing the storage check.

The accepted groups include file transfer, fade, display objects, sound,
model state, scripts, input, text, and AI operations. They also own two
rodata intervals (56 bytes at file offset `0xA60`, 92 at `0xB1C`) and four
initialized small-data bytes at `0x8CB94`. Existing compiler-owned data,
padding, and raw gaps remain intact.

The European fade wrapper previously contained a source-local assembler-name
alias declaration. Its declaration now lives in the existing `fade.h`, under
`FADE_STATE_ARRAY_ALIAS`, selected only by that wrapper. This preserves the
same array type and assembler symbol without an inline-assembly statement or
source-local extern declaration. No function body or compiler profile changes.
European, French, and Spanish consumers use the same existing named profile;
other users retain the unchanged default header declaration.

| Experiment | Result |
|---|---|
| Require complete CSV correspondence for every source-defined function | Missed sources with absent or incomplete mapping rows. |
| Locate unique whole-source relocation-masked candidates | 57 groups / 247 functions identified; not yet matches. |
| Validate repeated bindings and preserved data | 55 groups accepted for full-image trial; the two cases above deferred. |
| Build all 55 groups | Complete French executable matched. |
| Enforce the source declaration contract | Moved the European fade alias into its existing owning header without changing function bodies. |
| Rebuild after the declaration move and additive integration | Complete European, Spanish, and French executables match; all 1,079 French C entries agree with linked ELF names, addresses, and sizes. |

Recovered French-to-European correspondence is recorded with each newly
matched inventory entry. The incomplete attachment is not used to overwrite
those verified addresses. Generated assembly and the remaining unmatched
groups stay as fallback; no function-only result is promoted.

## Spanish-reference resident expansion

The verified Spanish implementation supplies **37 additional functions /
9,424 text bytes** across **14 unchanged whole translation units**. This
independent batch starts from the merged 1,079-function baseline and retains
every previous address, size, source and profile. The resulting resident
manifest has **1,116 functions / 351,348 text bytes in 550 translation units**.
No C source, header, compiler profile, or overlay manifest changes.

Unique whole-source non-relocation comparisons identify candidates only.
Recovery then validates 684 relocation sites against the exact Spanish ELF,
object addends, French instructions, repeated symbol bindings and local
section ownership. Both independently verified executables use GP
`0x8009C298`. Original function order and complete grouped-object coverage
are retained.

The batch includes debug-effect controls, the card-pick cursor, save payload
and transfer routines, memory-card dialogs and callbacks, dialog transitions,
and sound callback setup. Two actual C-owned rodata intervals are added:
20 bytes at file offset `0xC44` and 48 bytes at `0xC5C`. Existing C-owned
initialized sections and preserved gaps remain unchanged.

Spanish evidence resolves both previously deferred European-reference cases:

- The existing Spanish debug-effect wrapper already selects the measured
  field offsets and seven-byte state layout. All French references agree;
  no conflicting aliases or new regional macro are needed.
- `func_8004E7B0.c` has nine tentative declarations whose French addresses,
  sizes and alignment agree with their Spanish bindings. The exact Spanish
  ELF uses `SHN_ABS` for these preserved-storage declarations. All original
  storage bytes agree, including the pointer at `0x8009C318`, whose value is
  `0x800924CC`. The prior European pointer-byte discrepancy is therefore not
  bypassed. The French build preserves these bytes and does not allocate
  replacement storage or claim them as C-owned initializers.

| Experiment | Result |
|---|---|
| Recover bindings from the verified Spanish objects and ELF | All 14 groups pass, including the two previously deferred cases. |
| Compile and link the complete French image | Exact executable bytes, including both new rodata intervals. |
| Check every matched function's linked ELF ownership | Caught new external bindings shadowing prior C symbols: the linked output section is `.main`, not the input section name `.text`. |
| Remove aliases using exact baseline function addresses, sizes and ELF types | Clean full-image match and all 1,116 exact-size section-defined C symbols pass; all 1,079 baseline entries remain unchanged. |

The ambiguous tiny `func_8004A6D8.c` candidate remains assembly: eight
relocation-masked locations are not sufficient evidence to choose its
identity. No masked-only result or speculative ownership is promoted.

## Localized script images and anchored wrappers

This batch adds **11 functions / 1,064 text bytes** from **six unchanged
Spanish/shared translation units**. All 1,116 prior entries remain unchanged;
the resident total becomes **1,127 functions / 352,412 text bytes in 556
translation units**. No source, header, compiler-profile, data-ownership or
overlay changes are required.

The four-function Spanish `script_image_objects.c` group occupies
`0x8002DFE8..0x8002E3C0` (984 bytes). Its existing localized image-ID mapping
and public declaration are reused without another wrapper. Complete-object
comparison identifies one French location, and the reference ELF, object
addends and French instructions recover consistent external bindings.

Identical empty bodies cannot independently establish function identity.
Six eight-byte handlers instead fill exact gaps bounded by complete
already-matched Spanish/French source groups with the same profiles,
definition order and sizes:

| French range | Shared source | Preceding function | Following function |
|---|---|---|---|
| `0x80028474..0x8002847C` | `func_800283EC.c` | `DuelEffect_UpdateDialogState` | `DuelEffect_UpdateCardViewerState` |
| `0x8002BC30..0x8002BC38` | `european/library_runtime_8002BAAC.c` | `func_8002BAA0` | `func_8002BAB4` |
| `0x8002C734..0x8002C744` | `noop_callbacks.c` | `Library_CheckCardOwned` | `func_8002C570` |
| `0x8002F694..0x8002F6A4` | `european/script_noop_halt15.c` | `func_8002EF3C` | `Script_OpFadeOut` |

The library handler also agrees with its previously recovered direct-call
binding. The independently bound French script table `D_80090C50` at
`0x80092068` has slots 14 and 15 pointing to `0x8002F694` and `0x8002F69C`,
confirming the two script handlers' order. Unreferenced empty callbacks keep
their existing address-based names; no new semantics are inferred.

The previously ambiguous `func_8004A6D8.c` wrapper is now resolved at
`0x8004AB68` (32 bytes). Of its eight relocation-masked candidate locations,
only this one calls the already matched `SD_ResetSecondaryPlayback` at
`0x8004A9A8`. It independently agrees with the caller-derived linker
binding for `func_8004A6D8`. This is not a first-hit byte-pattern choice.

| Experiment | Result |
|---|---|
| Unique-only screening of empty bodies and the tiny wrapper | Four empty groups and one wrapper remain ambiguous. |
| Require both complete neighbor anchors, existing calls and script-table order | Six empty handlers have consistent identities and exact compiled bytes. |
| Require the known sound callee and independent caller binding | One of eight wrapper locations remains. |
| Recover 50 relocation sites and compile all six complete groups | Exact complete French executable. |
| Clean production rebuild and linked-function ownership check | All 1,127 C functions have exact section-defined names, addresses and sizes; no previous entries change. |

## French fixed-language runtime

The four localized runtime groups add **9 functions / 2,884 text bytes**,
bringing the manifest to **1,136 functions / 355,296 text bytes in 560
translation units**, with all 1,127 prior entries unchanged.

| French entry | Functions | Bytes | Source selection |
|---|---:|---:|---|
| `0x80012A44` | 1 | 404 | French `main_init.c`, language index 1 |
| `0x800309F4` | 3 | 1,284 | French `debug_menu_sound_entry.c`, language index 1 |
| `0x80036EF0` | 2 | 420 | Unchanged Spanish `dialog_choice_cursor.c` |
| `0x80043D7C` | 3 | 776 | French `main_run_boot_sequence.c`, language index 1 |

The three new French wrappers follow the existing Spanish wrapper structure,
selecting the existing `BUILD_LANGUAGE_INDEX` as **1** over the same shared
European implementations. No shared body, header, declaration or compiler
profile changes. The choice-cursor group is byte-identical to Spanish and
reuses its wrapper directly, including its measured previous-held R1 store.

French and Spanish main initialization differ at relative instruction offset
`0x60`: `0x24020001` versus `0x24020004`. The debug language handler has the
same measured difference at `0xCC`. These establish the French selector
without inferring it from language names.

The boot path is not a one-instruction patch: 130 instruction words differ
between its French and Spanish bodies. Compiling the shared implementation
with selector 1 naturally reproduces the French extra saved register,
register allocation and scheduling, as well as the language store and
package request. Instruction bytes are not edited or copied into C.

The three scratch wrappers first reproduce their complete object text using
the original named profiles. Spanish symbol values are hypotheses, not
blanket regional equivalence: 282 relocation sites are checked against the
compiled French objects, original French instructions, object addends and
the exact linked Spanish symbol values. Repeated uses and all previously
established French bindings agree. These objects own no additional data.

| Experiment | Result |
|---|---|
| Reuse Spanish language index 4 | Main initialization and debug handler each differ by one immediate; the boot body differs in 130 words. |
| Compile existing shared C with the measured index 1 | All three complete French object texts match; the unchanged Spanish choice group also matches. |
| Link all nine functions from scratch candidates | Complete French executable matches. |
| Promote three five-line wrappers and run the clean production build | Complete SHA-256 and all 1,136 exact linked C names, addresses and sizes pass. |

Once `Main_Init` is compiled, its `__main` call is no longer visible to the
fallback disassembler. An explicit symbol retains the previously verified
`0x8001296C..0x800129DC` CRT boundary rather than allowing it to merge into
the entry point and lose its SDK classification. All three existing CRT
records and all prior C records are preserved after another clean match.

Only the four-function result-orbit/outro group remains among the Spanish
inventory's resident C targets. This comparison is not yet the authoritative
French denominator: SDK/handwritten ownership and provisional fallback
boundaries still require durable classification. All six overlay manifests,
including their explicit uninventoried-code caveat, remain unchanged.

## Final resident group and complete ownership classification

The unchanged Spanish `duel_result_orbit_sprites.c` group completes the
eligible resident C inventory: **4 functions / 2,404 text bytes** at
`0x80020CB0..0x80021614`, bringing the total to **1,140 functions / 357,700
text bytes in 561 translation units**. All 1,136 previous entries remain
unchanged. The shared implementation and its localized sprite declarations
were already accepted for Spanish; no source or header changes are needed.

All four function bodies are byte-identical between the French and Spanish
images at the same addresses. The verified object contributes 158 relocation
sites, all consistent with original French instructions, object addends,
reference ELF values and existing French bindings. It owns no new data.
Both a full candidate link and the clean French production build reproduce
the complete executable, with every C function's exact name/address/size
checked in the linked ELF.

`function_regions.json` now classifies the entire resident text, instead of
leaving every fallback boundary as an unknown game candidate. The live game
regions are anchored by the exact `Main_Init`, `model_slot_queries`,
`model_slot_support` and final `ai_script_state_ops` source groups. Every byte
inside both game regions belongs to matching C or one of **61 individually
verified handwritten functions**, with no uncovered gaps.

Each handwritten interval has the same address, size and complete instruction
bytes as its independently classified Spanish counterpart, retaining the
North American/European handwritten provenance recorded there. All other game
functions are C; no unknown game function is silently excluded.

The three excluded CRT/SDK intervals are independently byte-identical to
the corresponding classified Spanish intervals:

| French interval | Ownership | SHA-256 of complete interval |
|---|---|---|
| `0x800128CC..0x80012A44` | CRT startup | `257e4158dd209d01299040a48d374de27c0d49179e0058b18d349ca6bfd17a0b` |
| `0x8005C018..0x8005C028` | Embedded SDK getter | `c8ee69b899100f70f5ff84228469bb84f20931b0383a5aecba9c27aa434d204c` |
| `0x80073C4C..0x800918DC` | SN/Psy-Q library region | `f904e317e75250ec3590a8962d32d9f9966069fbfe93b03b61e18a26b84da3aa` |

This establishes **100% of eligible resident game C**, not completion of the
whole French campaign. The preserved overworld tail and other potential
uninventoried runtime code still require explicit coverage analysis.

## Inventory and layout caveats

The initial inventory contains **1,821 discovered resident boundaries**.
After the relocation-backed expansion and a clean split, the current
inventory contains **1,848 boundaries**: **1,140 linked C functions**, **61
handwritten game functions**, **644 SDK boundaries** and the **three verified
CRT functions**. No resident row has unknown ownership or `unmatched_asm`
status. The SDK disassembler boundaries remain provisional: the same complete
SDK bytes have fewer internal boundaries in the Spanish split. Those labels
do not change the independently verified interval ownership or game target
denominator.

`make french-inventory` now uses the existing regional inventory gate after an
exact build, checking the matching manifest against linked C extents and
retaining the verified classifications. Repeated generation is byte-stable.
Changed boundaries, lost matching C or ownership changes fail explicitly.
Changes to matched ranges require updating the matching manifest and verifying
the linked ELF, not merely regenerating CSV.

The startup establishes entry `0x800128CC`, GP `0x8009C298`, and the BSS clear
range `0x8009C408..0x800FFC30`. `french-map` verifies the BSS range against
the first four startup instructions and checks every mapped region's hash.
Resident text ends after the return delay slot at `0x800918D8`. The image map
deliberately groups the remaining tail rather than borrowing unverified
European overlay/data boundaries.

The French SU/WA archives are independently extracted and hash-verified.
All six inventoried overlay instances now match unchanged European C, as
documented in `config/sles_03948/overlays/README.md`. This resident batch does
not change those manifests. The existing report generator still publishes a
French overlay-only section; integrating the now-verified resident denominator
into reporting is separate from this matching change and its report snapshot.
