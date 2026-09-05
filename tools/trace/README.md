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
"fewer than ten" allowance applies only when no completed pair remains.

## Running one

1. Open PCSX-Redux with the game and enable the debugger.
2. Select the **interpreter** CPU. Breakpoints do not fire on the dynarec.
3. Debug -> Lua editor, paste the script, let it auto-run.
4. Follow the instructions the script prints, then copy the whole document it
   produces into `tools/trace/result/<name>.txt`.

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

- Name the doubt in a header comment. A script that cannot say which symbol it
  would let us name is not ready to be written.
- Guard every pointer before dereferencing it. A bad read crashes the emulator
  rather than raising an error, and takes the user's session with it.
- Wrap the breakpoint body in `pcall`; the callback runs outside the safe Lua
  environment.
- Keep the returned breakpoint object in a global. If it is collected, the
  breakpoint disappears.
- Stop after a bounded number of hits and print once. An unbounded trace buries
  the useful part.
- Overlay addresses are only valid while that overlay is resident, and several
  modules share `0x80168000`. Dump a signature so the result can be checked
  against the module it claims to describe.
