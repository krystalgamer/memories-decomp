# Emulator traces

Lua scripts for [PCSX-Redux](https://pcsx-redux.consoledev.net/Lua/introduction/)
that answer questions the static code cannot. Each script exists to remove a
specific doubt about what a symbol means, not to dump state in general.

Only a human can run these, because only a human can say what was on screen.

## Layout

```
tools/trace/<name>.lua           the script
tools/trace/result/<name>.txt    the output, pasted back by whoever ran it
```

A script with no result is waiting to be run. A script with a result is
waiting to be acted on.

Before writing another script, inventory both directories. Any completed
script/result pair takes priority over the number of unanswered scripts; the
"fewer than 5" allowance applies only when no completed pair remains.

## Running one

1. Open PCSX-Redux with the game and enable the debugger
   (Configuration -> Emulation -> Debugger).
2. Select the **interpreter** CPU. Breakpoints do not fire on the dynarec, and
   switching CPU only takes effect after restarting the emulator.
3. Debug -> Lua editor, paste the script, let it run, and confirm its armed or
   breakpoint-installed message appears. Scripts that use breakpoints first
   print a `setup` line. `debugger=on` or `debugger=enabled-by-script` and
   `configured_cpu=interpreter` are necessary but not sufficient: the CPU
   engine is chosen when the emulator starts, so an unticked Dynarec box
   without a restart still runs the dynarec, and Lua cannot ask which engine
   is running (`active_cpu=unverified`). The setup is confirmed only when the
   script prints `execution breakpoint observed; setup confirmed`. Without
   that line, the run proves nothing either way.
4. Follow the instructions the script prints, then copy the whole document it
   produces into `tools/trace/result/<name>.txt`.

PCSX-Redux accepts Lua breakpoints while its debugger setting is off but never
fires them. Five traces came back with no breakpoint hit at all, even on
always-live controls, while their polled data arrived normally; a disabled
debugger or a dynarec still running are both plausible, and neither was
measured. Every breakpoint trace therefore checks `PCSX.settings.emulator`
before installing anything and switches `Debug.Debug` on if needed (the
interpreter reads it live). It reports the configured CPU, installs every
breakpoint through `traceAddBreakpoint` so the first real hit is announced,
and ends its document with a `setup:` line that says whether an execution
breakpoint fired.

PCSX-Redux removes Lua breakpoints during a console reset. A trace that spans
a hard or soft reset must keep global `ExecutionFlow::Reset` and
`ExecutionFlow::ShellReached` listeners, clear its capture state on reset,
and reinstall its breakpoints when the BIOS shell is reached. Such a script
should print both reset and reinstall milestones; silence after reset is a
setup failure, not a negative result.

Every script prints the surrounding document itself, so the only thing left to
write by hand is the context section:

```
==== USER CONTEXT ====

what you did and what you saw

==== TRACE RESULT =====

produced by the script
```

The context is the part that matters. The trace can report that a slot holds a
sprite at x=480, but only a person can say that the entry which slid in from
the right was "Card Shop".

## Acting on a result

Fold the finding into the source and the notes, then **delete both the script
and its result** — they are scaffolding, not a record. What was learned belongs
in `notes/`, in an inventory `notes` column, or in a symbol name.

If a result turns out inconclusive, either delete both, or keep the script and
delete only the result so it can be improved and run again. Say in the commit
message which of those happened and why. If the improved control has a new
purpose or expected output name, rename the script too so
`tools/trace/<name>.lua` and `tools/trace/result/<name>.txt` still match.

## Writing one

- Keep the human task short: one route of a minute or two, a fixed window
  that ends the trace by itself, and a context section of one or two lines.
  A trace that needs a full duel and a page of notes is tiring to run and
  rarely comes back.
- Print each logged row to the console the moment it happens, with a time
  stamp, as well as in the final document. Rows printed only at the end
  cannot be matched to what was on screen.
- Name the doubt in a header comment. A script that cannot say which symbol it
  would let us name is not ready to be written.
- Guard every pointer before dereferencing it. A bad read crashes the emulator
  rather than raising an error, and takes the user's session with it.
- If the script installs breakpoints, copy the `TRACE_SETUP` block from
  `tests/trace_setup_block.lua` (everything below its header) directly below
  `SCRIPT_NAME`, install every breakpoint through
  `traceAddBreakpoint`, and print `setup:` with `traceExecutionStatus()` after
  `script:` in the result. `tests/trace_setup_preflight_test.lua` enforces
  all three for every breakpoint trace. A silent result counts as negative
  evidence only when a control breakpoint in the same run fired.
- Wrap the breakpoint body in `pcall`; the callback runs outside the safe Lua
  environment.
- Keep the returned breakpoint object in a global. If it is collected, the
  breakpoint disappears.
- Stop after a bounded number of hits and print once. An unbounded trace buries
  the useful part.
- Overlay addresses are only valid while that overlay is resident, and several
  modules share `0x80168000`. Dump a signature so the result can be checked
  against the module it claims to describe.

## Testing one

A trace runs once, in someone else's session, and a script that crashes or
prints nothing wastes that session. `tools/trace/tests/<name>_test.lua` replays
the script's callbacks against a mock `PCSX` table and a real LuaJIT FFI
buffer, so the breakpoint body, the guards and the finish conditions can be
exercised without the emulator:

```
luajit tools/trace/tests/<name>_test.lua
```

Every current top-level trace script has a same-name replay, and
`trace_setup_preflight_test.lua` additionally loads every breakpoint trace
under mocked emulator settings. Nothing runs these automatically; run the
individual test above, or run the complete suite from the repository root:

```sh
(
    for test in tools/trace/tests/*_test.lua; do
        luajit "$test" || exit $?
    done
)
```

These replays validate script mechanics against mocked state. They do not
replace the requested emulator route, visible observation, or saved result.
Copy the `capture()` scaffold from an existing test, or from git history when
none is left (for example
`git show 7cd1467c72:tools/trace/tests/title_idle_watch_test.lua`). It stubs `getMemPtr`,
`getRegisters`, `addBreakpoint` and `createEventListener`, captures `print`,
and hands back helpers that drive one scenario.

Worth covering, because these are the failures that cost a session rather than
a rerun: every pointer the script might be handed that it must refuse (zero,
unaligned, outside RAM), the bounded-output path, and the case where nothing
happens at all — silence has to name the likely setup mistake. Where a script
assumes something about timing, pin the boundary in a test so the assumption is
visible in the result rather than implied.
