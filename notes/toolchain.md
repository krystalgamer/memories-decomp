# Toolchain Fingerprint

## Current conclusion

The resident executable strongly supports a late PsyQ GCC/CCPSX toolchain.
Assembler output in the main game cohort matches the **ASPSX 2.81/2.86
behavior class**. The available binary does not distinguish 2.81 from 2.86 and
does not independently identify the complete SDK package.

The user independently evaluated the SDK with multiple tools and selected
**Psy-Q 4.6**. Its Win32 toolset contains GCC 2.8.1, which is the project's
only compiler. The unusual `LIBDS.LIB` was an online patch
distributed before Psy-Q 4.7, explaining the apparent 4.7 association without
changing the SDK version.

The working hypotheses are:

| Component | Current assessment |
|---|---|
| C compiler | Optimized GCC-based CCPSX, likely a 2.8-era backend |
| Optimization | Likely `-O2`; exact flags remain unselected |
| Main game small-data setting | Approximately `-G8` |
| Later game/engine objects | Probable separate `-G0` cohort |
| Resident SDK libraries | Prebuilt PsyQ/SN archive members, generally no GP-relative access |
| Assembler | ASPSX 2.81 or 2.86 behavior |
| Linker | PSYLINK-compatible layout |
| EXE wrapper | CPE2X-style PS-X EXE conversion |

The project-local GNU binutils 2.42 toolchain reproduces the exact assembly
baseline. It is a deterministic replacement build tool, not a claim about the
original compiler or linker.

## Open-source GCC 2.8.1 probe

The project pins the public decompals/old-gcc PSX branch:

| Property | Value |
|---|---|
| Repository commit | `3fc018e71fcbd150c0887185703f7550723189ff` |
| Repository tree | `42e6f7d501097920b3a4bde96a9190f3c0b8e114` |
| GCC source SHA-256 | `3b30fbfdf93e628373d90d174243f3267b0eec9ebe792bb64fd15b8828c2ea4c` |
| Target | `mips-sony-psx` |
| GCC version | `2.8.1` |
| Local prefix | `tools/toolchains/gcc-2.8.1-psx/` |
| Compiler executable | `tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc` |

The prefix has two mutually exclusive installation paths:

| Make target | Installed compiler | Compiler identity | Provenance |
|---|---|---|---|
| `make compiler-281` | Native host build | `mips-sony-psx` | GCC source and the pinned decompals patch commit above |
| `make compiler-281-prebuilt` | Shell wrapper plus `libexec/gcc-2.8.1-psx` | Packaged driver reports `mips-linux-gnu` | decompals release `0.17`, archive SHA-256 `2421097de375a67939713480ea2aa17ce4df6c92cf0c476e68e9ac08c09c759e` |

The source-build path is stock GNU GCC 2.8.1 patched for the PSX target. The
prebuilt path installs its driver behind the same project executable name and
supplies the same `cc1`/`cpp` layout expected by the build. The installed
`build-manifest.json` records which distribution occupies the prefix. Neither
path is Sony CCPSX or replaces the selected Psy-Q 4.6 SDK identity.

The historical patch has two important defects:

1. Its configure-time PSX predefine assignment is unquoted, so `psx`,
   `__psx__`, and `__psx` are not reliably defined.
2. Its default-endian expression produces a negative target flag mask, which
   can clear intended defaults such as soft float.

The matching pipeline therefore always supplies explicit flags:

```text
-O2 -G8 -mel -mips1 -mcpu=R3000 -msoft-float -fno-builtin
-Dpsx -D__psx__ -D__psx
```

Compiler assembly is normalized with:

```text
maspsx --aspsx-version=2.81 --expand-div -G8
```

`gcc_2_8_1_g8_no_sched1` adds only `-fno-schedule-insns` to the base G8
profile, leaving post-allocation scheduling and both G8 thresholds unchanged.
`Text_ExtendGlyphCode` (`0x80037D2C`) uses it to preserve the selector-load
then glyph-code-load order without register pins or assembly barriers.
Its glyph-code accesses are volatile so the completed code is published before
the existing volatile completion flag. This is a measured match for that
function, not evidence that every game object disabled first-pass scheduling.

`make compiler-281` builds a native host executable; on a 64-bit host without
32-bit multilib headers and libraries, that produces a 64-bit compiler.
`make compiler-281-prebuilt` instead installs a POSIX shell wrapper and a
statically linked 32-bit i386 driver at
`tools/toolchains/gcc-2.8.1-psx/libexec/gcc-2.8.1-psx`. It avoids host
compiler and multilib dependencies on x86 systems, but requires a host capable
of executing i386 binaries. Both are suitable matching probes, while broader
compiler selection still requires comparison with genuine Psy-Q 4.6 tools.

`AiScript_Print` (`0x800736C4`) is the first confirmed match: GCC 2.8.1 with
the explicit flags above and maspsx emits its exact 64-byte instruction
sequence, and the complete PS-X EXE retains the target SHA-256.

`gcc_2_8_1_g0_split_no_cse_follow_jumps` adds only
`-fno-cse-follow-jumps` to the existing G0 split-address profile.
`func_80059700` uses it to retain the positive-magnitude copies across its
state dispatch without register pins. Split addressing alone removes those
copies; the no-CSE-follow-jumps profile without split addressing keeps them
but changes the address/register setup. Both compiler and assembler remain
at G0. This is a measured requirement for that source, not a project-wide
compiler rule.

## Original CC1PSX default options

The original Psy-Q 4.6 Win32 compiler backend identifies itself and reports
the following state when invoked only with `-version`:

```text
GNU C version 2.8.1 SN32 BUILD 4.0.0010 (PSX) compiled by CC.
options passed:
options enabled:  -fpeephole -ffunction-cse -fkeep-static-consts
 -fpcc-struct-return -fcommon -fgnu-linker -msplit-addresses -mgas -mgpOPT
 -mgpopt -msoft-float -mcpu=R3000
```

The empty `options passed` line is important: this is the backend's default
state, not a recovered game build command. A translation unit could override
these defaults, so they are candidate-exploration evidence rather than proof
that every object used the same settings.

| Reported option | Effect | Matching significance |
|---|---|---|
| `-fpeephole` | Allows machine-specific peephole rewrites during optimized compilation. | Can change short adjacent instruction sequences near final assembly output. |
| `-ffunction-cse` | Allows repeated constant function addresses to be placed in and reused from registers. | Can change direct-call setup, common-subexpression elimination, and register pressure. |
| `-fkeep-static-consts` | Emits unreferenced file-local `static const` objects when optimization is disabled. | Primarily affects object contents; it does not force an unused constant to remain in an optimized build. |
| `-fpcc-struct-return` | Returns short `struct` and `union` values indirectly through memory rather than registers. | This is an ABI choice; callers and callees returning aggregates must agree. |
| `-fcommon` | Emits tentative uninitialized global definitions as common symbols rather than allocated BSS definitions. | Affects symbol coalescing, section allocation, and duplicate-definition behavior at link time. |
| `-fgnu-linker` | Emits global-initialization metadata, such as C++ constructor and destructor records, in the GNU linker form. | Normally inert for plain C objects without global constructors and not evidence that the final executable was linked by GNU `ld`. |
| `-msplit-addresses` | Loads the high and low parts of symbolic addresses separately, allowing redundant high-part loads to be optimized away. | Changes address temporaries, scheduling, and HI16/LO16 relocation shape. The manifest exposes explicit `*_split` and `*_no_split` profiles. |
| `-mgas` | Selects the GCC backend's GNU assembler output conventions. | It is required by stock GCC 2.8.1 address splitting, but describes compiler output syntax rather than identifying the downstream assembler binary. |
| `-mgpOPT`, `-mgpopt` | Two accepted spellings for the same MIPS global-pointer optimization bit; the duplicate report does not represent two passes. The mode writes data declarations before text so short data can use one-instruction GP-relative references. | Evaluate it together with the `-G` threshold: `-G0` disables small-data references, while the main game cohort commonly matches near `-G8`. |
| `-msoft-float` | Avoids hardware floating-point instructions and lowers floating-point operations to helper calls. | Affects code generation and calling convention when floating-point values are present; the retail executable contains no COP1 instructions. |
| `-mcpu=R3000` | Uses the R3000 scheduling model. It does not enable post-MIPS-I instructions unless a higher ISA option is also selected. | Can alter instruction ordering and delay-slot choices without changing source semantics. |

Do not add this entire list to a canonical profile merely because CC1PSX
reported it. Existing byte-exact matches outrank the default report. For an
unresolved candidate, use target evidence to select the relevant knob:
address and relocation shape for `-msplit-addresses`, object size and
GP-relative accesses for `-mgpopt` plus `-G`, scheduling for `-mcpu=R3000`,
and aggregate or floating-point interfaces for the ABI options. Output-only
defaults should be explored only when section or symbol evidence calls for
them.

## Retired legacy fallback

The project previously kept a separately pinned public MIPS build of the
Psy-Q 4.6 DOS-era compiler alongside GCC 2.8.1. It was retired: it never
produced a committed match, every accepted `matching_c` entry names a
`gcc_2_8_1_*` profile, and its recorded campaign passes returned zero exact
results. Its installer, profiles, host patch, and assembly filter are gone.

GCC 2.8.1 with MASPSX 2.81 is the only compiler pipeline. A future candidate
that genuinely needs a different cohort must reintroduce it deliberately with
evidence, not resurrect the retired profiles.

## Address conversion

For the supplied executable:

```text
virtual address = file offset + 0x8000F800
file offset     = virtual address - 0x8000F800
```

The entry point `0x800129D8` is therefore at file offset `0x31D8`.

## Assembler evidence

The game uses GP-relative address formation such as:

```asm
800140B8  addiu  a1, gp, 0x1FC
```

There are 68 compiler-shaped `addiu reg,gp,offset` address formations across
36 game functions. The local maspsx fixture
`tools/vendor/maspsx/aspsx/fixtures/la.yaml` records:

- ASPSX through 2.79: `lui` followed by `addiu`
- ASPSX 2.81 and 2.86: one `addiu reg,gp,offset`

This is the strongest evidence for the late assembler class.

Other constraints:

- Positive small immediates overwhelmingly use `addiu reg,zero,imm`.
- Compiler-shaped code consistently avoids trapping `addi`.
- Expanded signed and unsigned division sequences use `break 7` and `break 6`
  checks rather than the `tge zero,zero,93` behavior associated with ASPSX
  2.05/2.08.
- Full division checks are present, so the objects were not uniformly assembled
  with the reduced `-0` expansion mode.

## Compiler evidence

The game region has conventional optimized GCC code generation:

- 922 detected stack prologues, all eight-byte aligned.
- Common stack frames are 24, 32, and 40 bytes.
- Calls and returns routinely fill their delay slots.
- Absolute switch tables are common.
- Multiplication by constants such as 140000 and 20832 is reduced to
  shift/add/sub sequences.
- Eight-byte unaligned copies use `lwl`, `lwr`, `swl`, and `swr`.
- No COP1 instructions are present; the target is MIPS-I/R3000 fixed-point code.

GP-based instructions occur throughout the main cohort and end at
`0x8005FB50`. No GP-based instruction occurs in the resident SDK region
beginning at `0x80073704`. This supports separate object/flag cohorts rather
than one uniform compiler invocation.

These patterns are compatible with a late CCPSX/GCC 2.8-era compiler, probably
at `-O2`, but they are not unique enough to select an exact compiler binary.

## Linker and small-data evidence

At file offset `0x80EEC` is the tuple:

```text
800129D8 0007DD08
800906E0 0000A828
8009B4A8 00063280
```

It describes:

| Section group | Range |
|---|---|
| Resident text plus alignment | `0x800129D8-0x800906E0` |
| Ordinary initialized data through small-data start | `0x800906E0-0x8009AF08` |
| Small initialized data | `0x8009AF08-0x8009B090` |
| Small BSS | `0x8009B090-0x8009B4A8` |
| Ordinary BSS | `0x8009B4A8-0x800FE728` |

The runtime GP is `0x8009AF08`, the start of small initialized data. Small
strings and objects occupy positive GP offsets, which is consistent with a
small-data threshold near eight bytes.

## PsyQ library anchors

| Embedded evidence | Virtual address | File offset |
|---|---:|---:|
| `intr.c` revision 1.75, 1997-02-07 | `0x800119B8` | `0x21B8` |
| `bios.c` revision 1.86, 1997-03-28 | `0x80011D70` | `0x2570` |
| `sys.c` revision 1.140, 1998-01-12 | `0x80012148` | `0x2948` |
| Sony library copyright 1993-1997 | `0x800919A8` | `0x821A8` |

The latest RCS string requires library source no earlier than January 1998.
These dates constrain library members; they are not PsyQ package-version
banners and must not be used alone to select an SDK package.

## Compiler decision and continuing probes

The project matching path is selected: use the local `mips-sony-psx` GCC 2.8.1
probe first and MASPSX 2.81 for every new game function. The exact proprietary
Psy-Q compiler binary is not present, so this remains a reproducible
open-source stand-in rather than a claim that the local executable is Sony's
original CCPSX.

The decision is supported by 623 independent full-executable matches totaling
60,596 bytes (`0xECB4`). Their profile distribution is:

| Profile | Matching functions |
|---|---:|
| `gcc_2_8_1_g8` | 324 |
| `gcc_2_8_1_g8_split` | 66 |
| `gcc_2_8_1_g0` | 195 |
| `gcc_2_8_1_g0_split` | 38 |

No committed matching function has ever required a compiler other than
GCC 2.8.1.

The profile manifest also exposes measured GCC 2.8.1 experiments for reviewed
partial functions whose target code provides specific evidence:

| Profile family | Measured knob |
|---|---|
| `gcc_2_8_1_o1_g*` | Lower optimization level |
| `gcc_2_8_1_g*_no_sched2` | Disable the second instruction-scheduling pass |
| `gcc_2_8_1_cc_g8_as_g0` | Compiler small-data threshold 8, assembler threshold 0 |
| `gcc_2_8_1_cc_g0_as_g8` | Compiler small-data threshold 0, assembler threshold 8 |
| `gcc_2_8_1_g*_no_split` | Explicit `-mgas -mno-split-addresses` |

These are not new defaults and do not change the Psy-Q 4.6 or ASPSX 2.81
selection. Use one only when instruction scheduling or relocation shape points
to that knob. Preserve the resulting object and mismatch evidence so later
profiles build on measurements instead of repeating them.

Continuing compiler probes should still cover multiple independent code shapes,
including relocation and section behavior:

Probe cases:

1. `1024 / signed_short`, including full divide-by-zero and overflow handling.
2. Taking the address of an approximately `-G8` static object.
3. A seven-case switch with an absolute jump table.
4. Multiplication by 140000 and 20832.
5. An unaligned eight-byte structure copy.
6. A five-argument callback call with filled delay slots.
7. Synthetic `.rdata`, `.text`, `.data`, `.sdata`, `.sbss`, and `.bss`
   objects linked around `_gp = 0x8009AF08`.

Test the Psy-Q 4.6 Win32 GCC 2.8.1 path across `-O1`/`-O2`,
`-G0`/`-G4`/`-G8`/`-G16`, and ASPSX 2.81/2.86 behavior.
Preserve compiler-generated assembly before maspsx so compiler and
assembler differences remain separable. Record complete hashes for every
supplied compiler, assembler, linker, and library artifact. Treat the patched
`LIBDS.LIB` as supporting library evidence rather than a 4.7 package marker.

Exact assembly remains the fallback for a documented dead end. There is no
secondary compiler.
