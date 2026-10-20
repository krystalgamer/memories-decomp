-- Run from the repository root:
--   luajit tools/trace/tests/unreferenced_helper_callers_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/unreferenced_helper_callers.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local PAD1_PRESSED = 0x8009b398
local ROW, COLUMN, QUAD, CONTROL = 0x8002c400, 0x8002c484, 0x8005949c, 0x8001306c
local WORDS = {
    [ROW] = 0x2882000a, [COLUMN] = 0x3c026666, [QUAD] = 0x000410c0,
    [CONTROL] = 0x27bdffe0,
}

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a0 = 0, ra = 0 } } }
    local events, breakpoints, output = {}, {}, {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u16at(a) return ffi.cast('uint16_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end
    for addr, word in pairs(WORDS) do
        u32at(addr)[0] = options.corrupt == addr and 0 or word
    end
    u32at(0x80168000)[0] = 0x00000008
    u32at(0x80168004)[0] = 0x8016a37c

    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = { enabled = true, callback = callback }
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
    breakpoints_unreferenced_helper_callers = nil
    listener_unreferenced_helper_callers = nil
    dofile(scriptPath)

    local r = {}
    function r:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function r:frames(n) for _ = 1, n do events['GPU::Vsync']() end end
    function r:mode(v) u8at(MAIN_MODE)[0] = v; r:frames(1) end
    function r:call(addr, a0, ra)
        registers.GPR.n.a0 = (a0 or 0) % 0x100000000
        registers.GPR.n.ra = ra or 0
        if breakpoints[addr].enabled then breakpoints[addr].callback() end
    end
    function r:press(mask)
        u16at(PAD1_PRESSED)[0] = mask
        r:frames(1)
        u16at(PAD1_PRESSED)[0] = 0
        r:frames(1)
    end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do if bp.enabled then return false end end
        return true
    end
    return r
end

-- The case the script exists for: each helper reached from some caller,
-- with the value its formula returns and the resident overlay.
local live = capture()
assert(live:has('4 breakpoints installed, 0 signature mismatches'))
live:mode(3)
live:call(CONTROL)
live:call(ROW, 3, 0x80170010)
live:call(ROW, 12, 0x80170010)
live:call(COLUMN, 23, 0x80170020)
live:call(QUAD, 1, 0x8016c000)
live:mode(8)
live:mode(1)
live:press(0x100)
live:press(0x100)
assert(live:has('status: stopped by Select twice after visiting several screens'))
assert(live:has('zone-offset func_8002C400 caller=0x80170010 arg=3 returns=56 '
                .. 'main=0x03 overlay=00000008 8016A37C'))
assert(live:has('arg=12 returns=202'), 'v >= 10 uses the +174 row')
assert(live:has('zone-offset func_8002C484 caller=0x80170020 arg=23 returns=418'))
assert(live:has('slot-quad func_8005949C caller=0x8016C000 arg=1 '
                .. 'returns=0x800F4810'))
assert(live:has('modes_seen: 0x01,0x03,0x08'))
assert(live:has('execution_breakpoint=observed'))
assert(live:allDisabled())

-- Negative arguments truncate toward zero, as the C division does.
local negative = capture()
negative:call(CONTROL)
negative:call(COLUMN, -3, 0x80170020)
negative:frames(18000)
negative:mode(3)
negative:mode(1)
negative:press(0x100)
negative:press(0x100)
assert(negative:has('arg=-3 returns=-34'))

-- One caller cannot crowd out the others, and every call is counted.
local busy = capture()
busy:call(CONTROL)
for _ = 1, 50 do busy:call(ROW, 1, 0x80170010) end
busy:call(ROW, 1, 0x80170030)
busy:mode(3)
busy:mode(8)
busy:mode(1)
busy:press(0x100)
busy:press(0x100)
assert(busy:has('caller=0x80170030'))
assert(not busy:has('row=06 '), 'four rows per caller')
assert(busy:has('zone-offset func_8002C400') and busy:has(' 51'))

-- Select before visiting several screens does not stop the capture.
local early = capture()
early:press(0x100)
early:press(0x100)
assert(not early:has('status:'))

-- Bounded output.
local flood = capture()
for i = 1, 70 do flood:call(QUAD, 0, 0x80170000 + i * 4) end
flood:frames(1)
assert(flood:has('status: captured 60 rows'))
assert(flood:has('rows: 60'))

-- A wrong entry word is named; no control hit is a setup failure.
local corrupt = capture({ corrupt = COLUMN })
assert(corrupt:has('1 signature mismatches'))
local silent = capture()
silent:frames(18000)
assert(silent:has('status: five minutes without a control hit'))
assert(silent:has('execution_breakpoint=NOT-observed'))

print = hostPrint
print('unreferenced_helper_callers: all six callback-replay cases passed')
