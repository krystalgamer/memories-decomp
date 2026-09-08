# Candidate targets

Each `func_XXXXXXXX.S` is the verified retail assembly target for the matching
source under `src/candidates/`. The build-integrated candidate validator checks
its address, size, symbol name, instruction rows, and byte hash.
