# Suspected maspsx divergences

`tools/vendor/maspsx` is a pinned, gitignored checkout and the build rejects a
dirty one. Suspected divergences from ASPSX are reported here for verification
rather than patched.

Only file something here once everything else has been ruled out. **So far
every suspected fault has turned out not to be one**, and both were measured
against ASPSX and found to agree:

- The **small-extern load-delay `nop`** was faithful emulation. The fix was to
  define the global the translation unit owns and build with
  `--use-comm-section`; see `func_80025028`.
- The **unfilled reorder-mode branch delay slot** was also faithful — ASPSX
  pads such a branch too. Where retail has a filled slot, *GCC* filled it, so
  the question is why GCC handed the assembler a reorder-mode branch at all.
  `func_80012DB4` was filed here, then matched with no tooling change: a
  ternary instead of an `if`/`else` made GCC fill the slot, and dropping a
  `volatile` freed a call argument to sink into another. See the corrections in
  `notes/research/matching-evidence.md`.

The standing lesson is that a difference which looks like the assembler's is
almost always the shape of the C. Exhaust that first.

## Layout

One directory per divergence, named for the behaviour rather than the function:

```
tools/maspsx_bugs/<behaviour>/
    README.md            the report
    <function>.c         the attempt: the C believed to be correct
    <function>.s         the original game assembly, spimdisasm output via splat
    build.sh             compile -> maspsx -> assemble, with every flag
```

## Rules

- **Use the real function, never a reduced example.** A cut-down case usually
  stops reproducing — GCC starts filling its own delay slots once the
  surrounding code is gone, and the report then demonstrates nothing. Shipping
  the real function also means a fix is a free match rather than a diagnostic.
- **The `.c` must be self-contained.** Inline the typedefs and declare any
  Psy-Q entry points directly. It has to build with nothing but the compiler,
  with no project headers and no include paths.
- **The `.s` is the original game assembly**, not the assembly generated from
  the `.c`. Take it from `tmp/splat/asm/generated/` after `make split`.
- **`build.sh` carries every flag** — compiler, maspsx and assembler — and names
  the profile it corresponds to in `config/slus_01411/compiler_profiles.json`.
  It writes all intermediates under `tmp/` and nothing into the report
  directory.
- **State what was ruled out**: every profile, the source shapes tried, and why
  the C cannot move the difference.
- **Quote the retail bytes** for the instructions in question, read from
  `game/SLUS_014.11`, so the expected output is evidence rather than assertion.
