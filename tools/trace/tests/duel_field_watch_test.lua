-- Run from the repository root:
--   luajit tools/trace/tests/duel_field_watch_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/duel_field_watch.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local REGISTRY = 0x800f5918
local AFA6 = 0x8009afa6
local CONTROL = 0x8001306c
local START = 0x8002cee8
local WINDOW = 7200
local WORDS = {
    [0x8002c400] = 0x2882000a, [0x8002c484] = 0x3c026666,
    [0x8005949c] = 0x000410c0, [0x800135fc] = 0x27bdffd8,
    [0x8002c570] = 0x3c02801d, [0x8003ffd8] = 0x27bdffe8,
    [0x80059b90] = 0x27bdff68, [0x8005c374] = 0xa3840598,
    [0x80058dc0] = 0x9382009e, [0x8005fc1c] = 0x3c02800f,
    [0x8005fe44] = 0x3c02800f, [CONTROL] = 0x27bdffe0,
}
if START ~= nil then WORDS[START] = 0x93830364 end

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local n = { a0 = 0, a1 = 0, a2 = 0, ra = 0 }
    local registers = { GPR = { n = n }, pc = 0 }
    local events, breakpoints, output = {}, {}, {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end
    for addr, word in pairs(WORDS) do
        u32at(addr)[0] = options.corrupt == addr and 0 or word
    end
    u8at(MAIN_MODE)[0] = 3

    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = { enabled = true, callback = callback, kind = kind }
            function handle:disable() self.enabled = false end
            breakpoints[address] = handle
            return handle
        end,
        Events = {
            createEventListener = function(name, callback)
                events[name] = callback
                return {}
            end,
        },
    }
    breakpoints_duel_field_watch = nil
    listener_duel_field_watch = nil
    dofile(scriptPath)

    local r = { breakpoints = breakpoints, u32at = u32at, u8at = u8at }
    function r:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function r:index(text)
        for i, line in ipairs(output) do
            if line:find(text, 1, true) then return i end
        end
        return nil
    end
    function r:frames(count) for _ = 1, count do events['GPU::Vsync']() end end
    function r:call(address, regs)
        for k, v in pairs(regs or {}) do n[k] = v % 0x100000000 end
        local bp = breakpoints[address]
        if bp.enabled then bp.callback() end
    end
    function r:write(pc)
        registers.pc = pc
        if breakpoints[AFA6].enabled then breakpoints[AFA6].callback() end
    end
    function r:start()
        if START ~= nil then r:call(START) end
    end
    function r:count() local c = 0 for _ in pairs(breakpoints) do c = c + 1 end return c end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do if bp.enabled then return false end end
        return true
    end
    return r
end

local EXPECTED = 13 + (START ~= nil and 1 or 0)

-- The route: watched functions fire, each first row is printed at once, the
-- family row carries its registry key, a write is caught, and the window
-- ends the trace by itself.
local route = capture()
assert(route:count() == EXPECTED, 'every watch, the write, the control and start')
assert(route.breakpoints[AFA6].kind == 'Write')
assert(route:has(string.format('%d breakpoints installed, 0 signature mismatches',
                               EXPECTED)))
route:start()
route:call(CONTROL)
route:frames(120)
route:call(0x8002c400, { a0 = 12, ra = 0x80170010 })
local live = route:index('duel_field_watch: row=01 t=2s main=0x03 func_8002C400 zone '
                         .. 'offset caller=0x80170010 a0=12')
assert(live ~= nil, 'a row is printed the moment it happens')
route.u32at(REGISTRY)[0] = 0x80089f00
route.u32at(REGISTRY + 4)[0] = 0x0002000d
route:call(0x8005fe44, { a0 = 0x80089f00, ra = 0x8005a5b0 })
route:write(0x80055000)
route:frames(WINDOW)
assert(route:has('status: window ended'))
assert(route:index('==== USER CONTEXT ====') > live, 'the document comes after')
assert(route:has('execution_breakpoint=observed'))
assert(route:has('func_8005FE44 handler family caller=0x8005A5B0 '
                 .. 'a0=-2146918656 a1=0 a2=0 key=0x0002000D'))
assert(route:has('write D_8009AFA6 pc=0x80055000 before=0'))
assert(route:has('called func_8002C400 zone offset x1'))
assert(route:has('written D_8009AFA6 x1'))
assert(route:has('not called: func_8002C484 zone offset, Model_GetSlotScale'))
assert(route:allDisabled())

-- Three rows per caller; every call is counted.
local busy = capture()
busy:start()
busy:call(CONTROL)
for _ = 1, 40 do busy:call(0x80058dc0, { ra = 0x80056000 }) end
busy:call(0x80058dc0, { ra = 0x80056010 })
busy:frames(WINDOW)
assert(busy:has('row=04 '))
assert(not busy:has('row=05 '))
assert(busy:has('called func_80058DC0 get D_8009AFA6 x41'))

-- Bounded output.
local flood = capture()
flood:start()
flood:call(CONTROL)
for i = 1, 70 do flood:call(0x8003ffd8, { ra = 0x80040000 + i * 4 }) end
flood:frames(WINDOW)
assert(flood:has('rows: 60'))
assert(not flood:has('row=61 '))

-- The window without a single control hit is a setup failure.
local silent = capture()
silent:start()
silent:frames(WINDOW)
assert(silent:has('status: window ended without a control hit'))
-- A route-start breakpoint is itself a real hit; only a script that starts
-- at once can end with nothing observed.
assert(silent:has(START == nil and 'execution_breakpoint=NOT-observed'
                  or 'execution_breakpoint=observed'))

-- A wrong retail word is named.
local corrupt = capture({ corrupt = 0x8005c374 })
assert(corrupt:has('1 signature mismatches'))

-- Nothing before the route starts counts against the window, and a route
-- that never starts is said so.
local waiting = capture()
waiting:call(CONTROL)
waiting:frames(WINDOW + 10)
assert(not waiting:has('status:'), 'the window opens only at the route start')
waiting:frames(36000)
assert(waiting:has('status: the route never started within ten minutes'))

print = hostPrint
print('duel_field_watch: all callback-replay cases passed')
