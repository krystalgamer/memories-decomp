# Candidate targets

Each `func_XXXXXXXX.S` (in `<module>/` for an overlay candidate) is the
verified retail assembly target for the matching source under
`src/candidates/`, cut from the generated Splat assembly by
`tools/project/candidate_builds.py --write-target`. The build-integrated
candidate validator checks its address, size, symbol name, instruction rows,
and byte hash.
