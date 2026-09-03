# Copilot CLI OOM Diagnosis - 2026-09-01

## Outcome

The crash recorded in
`tmp/report.20260901.161950.280136.0.001.json` was an internal Copilot CLI
JavaScript heap exhaustion. It was not a compiler failure and was not caused
by the kernel terminating a large parallel build.

The former repository-wide four-process build ceiling is superseded. The
evidence identifies long-session context and tool-output retention as the
primary failure mode, not parallel compilation. Make now follows the host CPU
count; memory-constrained systems such as the diagnosed 8 GiB, no-swap host
must set a lower job count explicitly. Parallel or verbose agent operations
remain amplifiers for CLI heap growth.

## Crash evidence

The Node diagnostic report records:

| Measurement | Value |
| --- | ---: |
| Failure | `Allocation failed - JavaScript heap out of memory` |
| Copilot CLI version in report | `1.0.71` |
| Node version | `24.16.0` |
| V8 heap limit | 2,150,629,376 bytes |
| V8 used heap | 1,867,489,752 bytes |
| Old-space used | 1,843,310,472 bytes |
| Old-space available | 168,160 bytes |
| Process RSS | 7,047,389,184 bytes |
| Host physical memory | 8,277,184,512 bytes |
| Host available memory | 223,756,288 bytes |
| Active Node workers at capture | 0 |
| Active libuv handles at capture | 0 |
| Swap | none |

Old-space was effectively full and garbage collection could not free enough
strongly referenced data for another allocation. The command line included
Node's `--optimize-for-size`, and this process had a roughly 2 GiB V8 heap
limit.

The process RSS was much larger than the reported V8 heap. The diagnostic
cannot assign every native allocation to a component, but this pattern is
consistent with retained JavaScript data plus native allocator fragmentation
or other private anonymous allocations accumulated during the session.

## Reproduction indicators

The replacement CLI process reached approximately 2.1 GiB RSS within ten
minutes while:

- no compiler or build child was running;
- only the Copilot CLI process was large;
- about 1.95 GiB was private anonymous memory;
- the host otherwise had several GiB available.

This rules out unrelated user processes as the immediate cause and shows that
the session itself has a high memory baseline.

That replacement process subsequently crashed and wrote
`report.20260901.165334.280534.0.001.json`. Its heap was again exhausted at
approximately 1.87 GiB used against the same 2.15 GiB limit. More importantly,
the second report contained 30,033 libuv handles:

| Handle type | Count |
| --- | ---: |
| `async` | 30,003 |
| `signal` | 19 |
| all other types | 11 |

Of the async handles, 30,002 were referenced and 30,003 were active. No Node
worker was active. This is direct evidence of an internal asynchronous-handle
leak or unbounded handle retention in the CLI/session/tool path. It explains
why limiting compiler and agent concurrency did not stop the crashes: the
leaked objects belong to the long-lived Copilot process itself.

A later report, `tmp/report.20260902.011413.381967.0.001.json`, reproduced the
other failure signature:

| Measurement | Value |
| --- | ---: |
| V8 used heap | 1,866,923,712 bytes |
| V8 heap limit | 2,150,629,376 bytes |
| Old-space available | 141,608 bytes |
| Process RSS | 4,488,863,744 bytes |
| Host available memory | 2,773,397,504 bytes |
| Active workers | 0 |
| libuv handles | 0 |

This confirms two independent manifestations of the same long-session
retention problem: one dump retained approximately 30,000 async handles, while
this dump exhausted old-space with no active handles or workers. Limiting
compiler processes cannot prevent either form.

The failing process accumulated about 621,000 filesystem reads and ran for
roughly fourteen minutes. Immediately before the failure, the work repeatedly
loaded source, configuration, generated assembly, and collaborator-reference
content while restoring a long-running decompilation session. Four-way tool
batches reduced compiler fan-out but still returned multiple large payloads
to the same CLI context.

## External corroboration

Open issue `github/copilot-cli#4664`, labeled `area:sessions` and
`area:context-memory`, reports the same failure when resuming a long-standing
session with substantial conversation and tool history. It remained open
during this diagnosis.

Copilot CLI 1.0.81 added recent-history-first loading for large sessions, but
the 1.0.82 changelog does not state that large-session heap exhaustion was
fixed. The local crash report predates the current 1.0.82 runtime and identifies
1.0.71, but upgrading alone is therefore not sufficient evidence that the
failure is resolved.

## Contributing causes

1. A long-lived resumed session with extensive decompilation and tool history.
2. At least one CLI/session/tool path retaining tens of thousands of active,
   referenced `uv_async` handles.
3. Repeated large tool results retained in the CLI context.
4. Reading several files in one parallel tool batch, which increases the
   transient and retained payload even when the process count stays at four.
5. An approximately 2 GiB V8 heap ceiling in the crashing process.
6. No swap and only about 8 GiB of host RAM, leaving little safety margin once
   native RSS grew beyond the V8 heap.

## Operational response

- Use one process and one tool call at a time for normal work.
- Do not apply the former four-process ceiling as a repository-wide build
  policy; use the host CPU count for Make and lower it explicitly on
  memory-constrained systems.
- Avoid background agents and broad repository/reference scans.
- Search before reading and cap every returned range or command output.
- Consolidate a complete bounded phase into one standalone sequential script
  that writes compact results to disk, instead of issuing one CLI tool call per
  candidate.
- Stop after one substantial subsystem batch or two small atomic commits, push
  the checkpoint, and start a fresh CLI session.
- Refuse `make` commands when the parent Copilot process reaches 2560 MiB RSS
  via `tools/project/session_memory_guard.py`.
- Put verbose compiler, linker, and comparison logs under `tmp/`.
- Process collaborator candidates through a sequential driver that writes a
  compact manifest and result ledger rather than returning per-candidate
  disassembly in the conversation.
- Commit and push bounded batches so a fresh session can resume entirely from
  tracked notes and ledgers.
- Do not use `NODE_OPTIONS=--max-old-space-size=8192` on this host. An 8 GiB
  heap allowance on an 8 GiB, no-swap machine can destabilize the entire host
  and only postpones unbounded retention.
