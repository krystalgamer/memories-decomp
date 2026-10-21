-- Run from the repository root:
--   luajit tools/trace/tests/trace_setup_preflight_test.lua
--
-- Every trace that installs a breakpoint must check the emulator setup first.
-- PCSX-Redux accepts Lua breakpoints while its debugger setting is off but
-- never fires them, and five results came back silent that way. The CPU
-- engine is fixed when the emulator starts and Lua cannot query it, so the
-- Dynarec setting only describes the configured engine; the setup counts as
-- confirmed only after one of the script's own execution breakpoints fires.
-- This loads each breakpoint trace against mocked settings and checks that:
--   * the setup block is the same in every script,
--   * the debugger is on before the first breakpoint is installed,
--   * a configured dynarec is named, and the active engine is never claimed,
--   * nothing reports a confirmed setup until a breakpoint actually fires,
--   * the result document carries the setup and execution status.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print

local BLOCK_START = '\n-- Breakpoints fire only while the debugger setting is on and the interpreter\n'
local BLOCK_END = '        .. \'silence is not evidence\'\nend\n'
local RESULT_LINE = "print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())"

local scripts = {}
local listing = io.popen('ls tools/trace/*.lua')
for path in listing:lines() do
    local handle = io.open(path)
    local text = handle:read('*a')
    handle:close()
    if text:find('PCSX.addBreakpoint', 1, true)
        or text:find('traceAddBreakpoint(', 1, true) then
        scripts[#scripts + 1] = { path = path, text = text }
    end
end
listing:close()
assert(#scripts >= 1, 'expected at least one breakpoint trace')

local function block(script)
    local i = script.text:find(BLOCK_START, 1, true)
    assert(i, script.path .. ': setup block missing')
    local _, j = script.text:find(BLOCK_END, i, true)
    assert(j, script.path .. ': setup block truncated')
    return script.text:sub(i, j), j
end

-- One shared block, and nothing below it bypasses the wrapper.
local reference = block(scripts[1])
for _, script in ipairs(scripts) do
    local text, finish = block(script)
    assert(text == reference, script.path .. ': setup block differs')
    assert(not script.text:find('PCSX.addBreakpoint(', finish, true),
           script.path .. ': a breakpoint bypasses traceAddBreakpoint')
    assert(script.text:find(RESULT_LINE, 1, true),
           script.path .. ': result document must print the setup line')
end

-- The block on its own: mocked settings, mocked breakpoints, direct access to
-- its helpers.
local function probe(settings)
    local output, installed = {}, {}
    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        settings = settings,
        addBreakpoint = function(address, kind, width, cause, invoker)
            installed[#installed + 1] = { address = address, invoker = invoker }
            return {}
        end,
    }
    local chunk = assert(loadstring("local SCRIPT_NAME = 'probe'\n" .. reference
        .. '\nreturn { add = traceAddBreakpoint, status = traceExecutionStatus, '
        .. 'setup = TRACE_SETUP }'))
    local api = chunk()
    print = hostPrint
    api.output, api.installed = output, installed
    return api
end

local function has(output, text)
    for _, line in ipairs(output) do
        if line:find(text, 1, true) then return true end
    end
    return false
end

-- Debugger off, interpreter configured: switched on, and the active engine is
-- still not claimed.
local off = { emulator = { Debug = { Debug = false }, Dynarec = false } }
local api = probe(off)
assert(off.emulator.Debug.Debug == true)
assert(api.setup == 'debugger=enabled-by-script configured_cpu=interpreter '
       .. 'active_cpu=unverified')

-- Configured for the interpreter but still running the dynarec because the
-- emulator was not restarted: the settings look right, no breakpoint ever
-- fires, and the result must not call the setup valid.
local stale = probe({ emulator = { Debug = { Debug = true }, Dynarec = false } })
stale.add(0x80012345, 'Exec', 4, 'probe', function() end)
assert(stale.setup:find('configured_cpu=interpreter active_cpu=unverified', 1, true))
assert(stale.status():find('execution_breakpoint=NOT-observed', 1, true),
       'a silent run must leave the setup unverified')
assert(not has(stale.output, 'setup confirmed'))

-- A configured dynarec is named as configuration.
local dynarec = probe({ emulator = { Debug = { Debug = true }, Dynarec = true } })
assert(dynarec.setup:find('debugger=on configured_cpu=DYNAREC', 1, true))

-- No settings table at all.
assert(probe(nil).setup:find('unknown: PCSX.settings unavailable', 1, true))

-- The first real hit confirms the setup once, and the wrapped invoker's
-- arguments and return value pass through unchanged.
local live = probe({ emulator = { Debug = { Debug = true }, Dynarec = false } })
local seen = nil
live.add(0x80012345, 'Exec', 4, 'probe', function(address, width, cause)
    seen = { address, width, cause }
    return false
end)
live.add(0x80012349, 'Exec', 4, 'probe', function() return nil end)
local captured = {}
print = function(text) captured[#captured + 1] = tostring(text) end
local first = live.installed[1].invoker(0x80012345, 4, 'probe')
local second = live.installed[2].invoker(0x80012349, 4, 'probe')
live.installed[1].invoker(0x80012345, 4, 'probe')
print = hostPrint
assert(first == false, 'returning false must still remove the breakpoint')
assert(second == nil)
assert(seen[1] == 0x80012345 and seen[2] == 4 and seen[3] == 'probe')
local confirmations = 0
for _, line in ipairs(captured) do
    if line == 'probe: execution breakpoint observed; setup confirmed' then
        confirmations = confirmations + 1
    end
end
assert(confirmations == 1, 'confirm once, not on every hit')
assert(live.status() == 'execution_breakpoint=observed')

-- A missing invoker keeps PCSX-Redux's default pause behaviour.
local plain = probe({ emulator = { Debug = { Debug = true }, Dynarec = false } })
plain.add(0x80012345, 'Exec', 4, 'probe')
assert(plain.installed[1].invoker == nil)

-- Every script: loads under the mock, never arms with the debugger off.
local memory = ffi.new('uint8_t[?]', 0x200000)
local function load(path, settings)
    local output, invokers, listeners = {}, {}, {}
    local debugAtFirstBreakpoint = nil
    print = function(text) output[#output + 1] = tostring(text) end
    local registers = { GPR = { n = setmetatable({}, {
        __index = function() return 0 end }) }, pc = 0 }
    PCSX = setmetatable({
        settings = settings,
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, width, cause, invoker)
            if debugAtFirstBreakpoint == nil then
                debugAtFirstBreakpoint = settings ~= nil
                    and settings.emulator.Debug.Debug == true
            end
            invokers[#invokers + 1] = invoker
            local handle = {}
            function handle:disable() end
            function handle:enable() end
            function handle:remove() end
            function handle:isEnabled() return true end
            return handle
        end,
        Events = { createEventListener = function(name, callback)
            listeners[name] = callback
            return {}
        end },
    }, { __index = function() return function() return 0 end end })
    local ok, err = pcall(dofile, path)
    print = hostPrint
    assert(ok, path .. ': failed to load: ' .. tostring(err))
    return {
        output = output, invokers = invokers, listeners = listeners,
        armedWithDebugger = debugAtFirstBreakpoint,
    }
end

local armed = 0
for _, script in ipairs(scripts) do
    local off = { emulator = { Debug = { Debug = false }, Dynarec = false } }
    local run = load(script.path, off)
    assert(off.emulator.Debug.Debug == true, script.path .. ': debugger not enabled')
    -- Scripts that verify retail code words first may refuse to arm against
    -- zeroed mock memory; any that do arm must find the debugger on.
    assert(run.armedWithDebugger ~= false,
           script.path .. ': a breakpoint was installed before the debugger was on')
    if run.armedWithDebugger then armed = armed + 1 end
    assert(has(run.output, 'setup debugger=enabled-by-script '
                   .. 'configured_cpu=interpreter active_cpu=unverified'),
           script.path .. ': the setup line must be printed at install')
    assert(not has(run.output, 'setup confirmed'),
           script.path .. ': nothing may confirm the setup before a hit')
    PCSX = nil
end
assert(armed >= #scripts / 2, 'too few scripts armed under the mock to '
       .. 'prove the ordering: ' .. armed)

-- End to end on one script: the result document reports whether an execution
-- breakpoint fired, so a silent run cannot pass for a negative result.
local function endToEnd(hit)
    local settings = { emulator = { Debug = { Debug = true }, Dynarec = false } }
    local run = load('tools/trace/title_idle_watch.lua', settings)
    local captured = {}
    print = function(text) captured[#captured + 1] = tostring(text) end
    if hit then run.invokers[1](0x8002c400, 4, 'probe') end
    for _ = 1, 5400 do run.listeners['GPU::Vsync']() end
    print = hostPrint
    PCSX = nil
    return captured
end
local silent = endToEnd(false)
assert(has(silent, 'setup: debugger=on configured_cpu=interpreter '
               .. 'active_cpu=unverified execution_breakpoint=NOT-observed'))
local confirmed = endToEnd(true)
assert(has(confirmed, 'title_idle_watch: execution breakpoint '
                  .. 'observed; setup confirmed'))
assert(has(confirmed, 'active_cpu=unverified execution_breakpoint=observed'))

print(string.format('trace_setup_preflight: %d breakpoint traces checked, '
                    .. '%d armed under the mock', #scripts, armed))
