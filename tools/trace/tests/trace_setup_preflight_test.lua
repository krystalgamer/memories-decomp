-- Run from the repository root:
--   luajit tools/trace/tests/trace_setup_preflight_test.lua
--
-- Every trace that installs a breakpoint must check the emulator setup first.
-- PCSX-Redux accepts Lua breakpoints while its debugger setting is off but
-- never fires them, and five results came back silent that way. This loads
-- each such script against mocked settings and asserts that the debugger is
-- on before the first breakpoint is installed, that a running dynarec is
-- named, and that the result document carries the setup line.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print

local scripts = {}
local listing = io.popen('ls tools/trace/*.lua')
for path in listing:lines() do
    local handle = io.open(path)
    local text = handle:read('*a')
    handle:close()
    if text:find('PCSX.addBreakpoint', 1, true) then
        scripts[#scripts + 1] = { path = path, text = text }
    end
end
listing:close()
assert(#scripts >= 20, 'expected the breakpoint traces, found ' .. #scripts)

local memory = ffi.new('uint8_t[?]', 0x200000)

local function load(path, settings)
    local output = {}
    local debugAtFirstBreakpoint = nil
    print = function(text) output[#output + 1] = tostring(text) end
    local registers = { GPR = { n = setmetatable({}, {
        __index = function() return 0 end }) }, pc = 0 }
    PCSX = setmetatable({
        settings = settings,
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function()
            if debugAtFirstBreakpoint == nil then
                debugAtFirstBreakpoint = settings ~= nil
                    and settings.emulator.Debug.Debug == true
            end
            local handle = {}
            function handle:disable() end
            function handle:enable() end
            function handle:remove() end
            function handle:isEnabled() return true end
            return handle
        end,
        Events = { createEventListener = function() return {} end },
    }, { __index = function() return function() return 0 end end })
    local ok, err = pcall(dofile, path)
    print = hostPrint
    assert(ok, path .. ': failed to load: ' .. tostring(err))
    return output, debugAtFirstBreakpoint
end

local function has(output, text)
    for _, line in ipairs(output) do
        if line:find(text, 1, true) then return true end
    end
    return false
end

local armed = 0
for _, script in ipairs(scripts) do
    local path = script.path

    -- The setup line has to reach the result document, not just the console.
    assert(script.text:find("print('setup: ' .. TRACE_SETUP)", 1, true),
           path .. ': result document must print the setup line')

    -- Debugger off: the script turns it on before arming anything.
    local off = { emulator = { Debug = { Debug = false }, Dynarec = false } }
    local output, armedWithDebugger = load(path, off)
    assert(off.emulator.Debug.Debug == true, path .. ': debugger not enabled')
    -- Scripts that verify retail code words first may refuse to arm
    -- against zeroed mock memory; any that do arm must find it on.
    assert(armedWithDebugger ~= false,
           path .. ': a breakpoint was installed before the debugger was on')
    if armedWithDebugger then armed = armed + 1 end
    assert(has(output, 'setup debugger=enabled-by-script dynarec=off'),
           path .. ': enabling the debugger must be reported')

    -- Already on, but the dynarec is running: say so plainly.
    local dynarec = { emulator = { Debug = { Debug = true }, Dynarec = true } }
    output = load(path, dynarec)
    assert(has(output, 'setup debugger=on dynarec=ON: breakpoints will not fire'),
           path .. ': a running dynarec must be named')

    -- Older builds without the settings table still load and say so.
    output = load(path, nil)
    assert(has(output, 'setup unknown: PCSX.settings unavailable'),
           path .. ': missing settings must be reported')

    PCSX = nil
end

assert(armed >= #scripts / 2, 'too few scripts armed under the mock to '
       .. 'prove the ordering: ' .. armed)
print(string.format('trace_setup_preflight: %d breakpoint traces checked, '
                    .. '%d armed under the mock', #scripts, armed))
