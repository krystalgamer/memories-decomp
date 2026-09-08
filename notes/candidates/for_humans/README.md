# Human candidate bundles

Each `func_XXXXXXXX/` directory is generated from one remaining resident
candidate note and contains:

- `candidate.c`: the note's single self-contained source block, with include
  paths adjusted so it can be compiled in place from the repository root;
- `target.S`: that function's expected spimdisasm assembly block from the
  verified local split;
- `build.json`: the exact named compiler profile, GCC/MASPSX/assembler options,
  source hash, target-word hash, address, size, and current symbol names.

Regenerate every bundle after a candidate, profile, or target split changes:

```sh
MAKEFLAGS=-j"$(nproc)" make candidate-bundles
```

Verify tracked bundles without requiring the retail executable or generated
split:

```sh
make check-candidate-bundles
```

When `tmp/splat/asm/` exists, the check additionally compares each tracked
`target.S` byte-for-byte with the current generated spimdisasm block. Metadata
CI always checks the source extraction, build options, directory coverage,
target address/size/word hashes, and contiguous instruction rows.

Compile every exported source through the exact recorded GCC/MASPSX/assembler
profile:

```sh
make check-candidate-bundle-builds
```

Objects and intermediate assembly stay under `tmp/candidate-human-build/`.

The bundles are measurements, not accepted implementations. Continue from the
candidate note's evidence and use the repository's ordinary exact-match
workflow before promoting any source.

Once a candidate is build-integrated, its source and target move to
`src/candidates/` and `src/candidates_target/`, its metadata moves to
`config/slus_01411/candidates.json`, and this generated note bundle is removed.
