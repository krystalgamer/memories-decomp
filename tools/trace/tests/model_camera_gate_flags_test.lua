-- Run from the repository root:
--   luajit tools/trace/tests/model_camera_gate_flags_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/model_camera_gate_flags.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local FLAG_A = 0x8009b07b
local FLAG_B = 0x8009b07c
local FLAG_079 = 0x8009b079
local COUNTER_07A = 0x8009b07a
local SET_A = 0x8005f180
local SET_B = 0x8005f198
local MOVE = 0x80052d2c
local REQUEST_SLOTS = 0x80059f18
local REQUEST_VALUE = 0x80059ebc
local WORDS = {
    [SET_A] = 0xa3840173, [SET_B] = 0xa3840174, [MOVE] = 0x27bdffc0,
    [REQUEST_SLOTS] = 0x27bdffd8, [REQUEST_VALUE] = 0x27bdffe0,
}

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a0 = 0, a1 = 0, a2 = 0, a3 = 0, ra = 0 } } }
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
    breakpoints_model_camera_gate_flags = nil
    listener_model_camera_gate_flags = nil
    dofile(scriptPath)

    local r = {}
    function r:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function r:frames(n) for _ = 1, n do events['GPU::Vsync']() end end
    function r:call(addr, ra, a0, a1, a2, a3)
        local n = registers.GPR.n
        n.ra, n.a0, n.a1, n.a2, n.a3 = ra, (a0 or 0) % 0x100000000,
            (a1 or 0) % 0x100000000, (a2 or 0) % 0x100000000,
            (a3 or 0) % 0x100000000
        if breakpoints[addr].enabled then breakpoints[addr].callback() end
        -- The setters' effect, after the breakpoint has seen the old value.
        if addr == SET_A then u8at(FLAG_A)[0] = a0 % 256 end
        if addr == SET_B then u8at(FLAG_B)[0] = a0 % 256 end
    end
    function r:poke(addr, v) u8at(addr)[0] = v % 256 end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do if bp.enabled then return false end end
        return true
    end
    function r:count() local n = 0 for _ in pairs(breakpoints) do n = n + 1 end return n end
    return r
end

-- Normal: the guard passes with the second byte clear and suppresses with
-- both bytes at 1; a direct move and both setters are recorded.
local normal = capture()
assert(normal:count() == 5)
assert(normal:has('5 breakpoints installed, 0 signature mismatches'))
normal:frames(1)
normal:call(SET_A, 0x8004f000, 1)
normal:call(REQUEST_SLOTS, 0x8004f100, 1, -1, 0, 30)
normal:call(MOVE, 0x80059f60, 1, 0, 1, 30)
normal:call(SET_B, 0x80056000, 1)
normal:call(REQUEST_VALUE, 0x8004f200, 2)
normal:call(SET_B, 0x80056010, 0)
normal:frames(10801)
assert(normal:has('status: three quiet minutes without a new row'))
assert(normal:has('set first byte func_8005F180 value=1 caller=0x8004F000 '
                  .. 'before: first=0 second=0 mode=0x03'))
assert(normal:has('Model_RequestCameraMoveToSlots args=1,-1,0,30 '
                  .. 'caller=0x8004F100 first=1 second=0 mode=0x03 passed'))
assert(normal:has('camera move func_80052D2C args=1,0,1,30'))
assert(normal:has('func_80059EBC args=2,0,0,0 caller=0x8004F200 '
                  .. 'first=1 second=1 mode=0x03 SUPPRESSED'))
assert(normal:has('gated_requests_passed: 1 gated_requests_suppressed: 1'))
assert(normal:allDisabled())

-- The per-frame bracket around overlay handlers must be counted but not
-- flood the rows.
local busy = capture()
for _ = 1, 300 do
    busy:call(SET_B, 0x80056000, 1)
    busy:call(SET_B, 0x80056010, 0)
end
busy:frames(10801)
assert(busy:has('count set second byte func_8005F198 value=1 caller=0x80056000 300'))
assert(busy:has('row=04'))
assert(not busy:has('row=05'), 'two rows per setter/caller/value')

-- Neighbouring block bytes are polled.
local poll = capture()
poll:frames(1)
poll:poke(COUNTER_07A, -1)
poll:frames(1)
poll:poke(FLAG_079, 1)
poll:frames(10801)
assert(poll:has('poll D_8009B079 0->0 D_8009B07A 0->-1'))
assert(poll:has('poll D_8009B079 0->1 D_8009B07A -1->-1'))

-- A wrong retail word is reported.
local corrupt = capture({ corrupt = MOVE })
assert(corrupt:has('1 signature mismatches'))

-- Bounded output.
local flood = capture()
for i = 1, 70 do flood:call(MOVE, 0x80050000 + i * 4, i) end
flood:frames(1)
assert(flood:has('status: captured 64 rows'))

-- Silence names the likely setup mistake.
local silent = capture()
silent:frames(36000)
assert(silent:has('status: nothing in ten minutes'))

print = hostPrint
print('model_camera_gate_flags: all six callback-replay cases passed')
