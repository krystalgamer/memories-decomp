# Suspected maspsx divergences

`tools/vendor/maspsx` is a pinned, gitignored checkout and the build rejects a
dirty one. Suspected divergences from ASPSX are reported here for verification
rather than patched.

Only file something here once everything else has been ruled out. Most apparent
maspsx faults are not faults: the small-extern load-delay `nop` looked like one
for a long time, was measured against ASPSX, and turned out to be faithful
emulation with a source-side fix — define the global the translation unit owns
and build with `--use-comm-section`. See the correction in
`notes/research/matching-evidence.md`.

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
