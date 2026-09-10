-- Run from the repository root:
--   luajit tools/trace/tests/record_dispatch_runtime_role_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
--
-- The record arguments are read as halfwords, so this test emphasizes null,
-- odd and out-of-RAM pointers along with the bounded and no-hit finish paths.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/record_dispatch_runtime_role.lua'

local DISPATCH = 0x8005f91c
local CALLER = 0x8005d994
local MAIN_MODE = 0x8009b26c
local QUEUE_COUNT = 0x8009b078
local QUEUE_ACTIVE = 0x8009b079
local QUEUE_BASE = 0x800f5788
local RAM_BASE = 0x80000000

local function capture()
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = {
        GPR = { n = { a0 = 0, a1 = 0, a2 = 0, a3 = 0, ra = 0 } },
    }
    local events = {}
    local breakpoints = {}
    local output = {}

    print = function(text)
        output[#output + 1] = tostring(text)
    end
    PCSX = {
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = { enabled = true, callback = callback }
            function handle:disable() self.enabled = false end
            function handle:remove() self.enabled = false end
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
    breakpoint_record_dispatch_runtime_role = nil
    breakpoints_record_dispatch_callers = nil
    listener_record_dispatch_runtime_role = nil
    dofile(scriptPath)

    local result = {}

    local function u8at(addr)
        return ffi.cast('uint8_t*', memory + (addr - RAM_BASE))
    end

    local function u16at(addr)
        return ffi.cast('uint16_t*', memory + (addr - RAM_BASE))
    end

    function result:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then
                return true
            end
        end
        return false
    end

    function result:frames(count)
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:mode(value)
        u8at(MAIN_MODE)[0] = value
    end

    function result:vector(addr, a, b, c, d)
        u16at(addr)[0] = a % 0x10000
        u16at(addr + 2)[0] = b % 0x10000
        u16at(addr + 4)[0] = c % 0x10000
        u16at(addr + 6)[0] = d % 0x10000
    end

    function result:queue(count, active)
        u8at(QUEUE_COUNT)[0] = count
        u8at(QUEUE_ACTIVE)[0] = active
    end

    function result:queueRecord(index, values)
        local addr = QUEUE_BASE + index * 0x28
        self:vector(addr, values[1], values[2], values[3], values[4])
        self:vector(
            addr + 8, values[5], values[6], values[7], values[8]
        )
        u16at(addr + 0x20)[0] = values[9] % 0x10000
        u16at(addr + 0x22)[0] = values[10] % 0x10000
        u16at(addr + 0x24)[0] = values[11] % 0x10000
        u8at(addr + 0x26)[0] = values[12]
    end

    function result:caller(address)
        local breakpoint = breakpoints[address]
        assert(breakpoint and breakpoint.enabled)
        breakpoint.callback(address)
    end

    function result:dispatch(mode, left, right, arg3, callsite)
        registers.GPR.n.a0 = mode
        registers.GPR.n.a1 = left
        registers.GPR.n.a2 = right
        registers.GPR.n.a3 = arg3
        registers.GPR.n.ra = callsite + 8
        local breakpoint = breakpoints[DISPATCH]
        assert(breakpoint and breakpoint.enabled)
        breakpoint.callback()
    end

    function result:breakRegisters()
        PCSX.getRegisters = function()
            error('mock register failure')
        end
    end

    function result:disabled()
        if breakpoints[DISPATCH].enabled then
            return false
        end
        for _, address in ipairs({
            0x8005d994, 0x8005f714, 0x8005f7b0, 0x8005f828,
        }) do
            if breakpoints[address].enabled then
                return false
            end
        end
        return true
    end

    return result
end

local normal = capture()
normal:mode(0x23)
normal:queue(1, 1)
normal:vector(0x80001000, 1, -2, 3, -4)
normal:vector(0x80001010, 5, 6, 7, 8)
normal:queueRecord(
    0, {10, -20, 30, -40, 50, 60, 70, 80, -90, 100, -110, 0xa5}
)
normal:caller(CALLER)
for sequence = 1, 4 do
    normal:dispatch(0, 0x80001000, 0x80001010, -7, CALLER)
    if sequence == 1 then
        normal:dispatch(1, 0, 0x80001001, 9, 0x8005f714)
    end
    normal:dispatch(2, 0x80001000, 0x80001010, 11, CALLER)
end
normal:frames(1800)
assert(normal:has(
    'status: captured four processed sequences followed by quiet time'
), 'four completed sequences must end after the quiet window')
assert(normal:has('summary: hits=9 caller_hits=1 sequences=4 completed=4'),
       'the bounded summary must retain hit and sequence counts')
assert(normal:has('modes_0_1_2=4/1/4'),
       'all three dispatcher modes must be counted')
assert(normal:has('left=[1,-2,3,-4]/[0001,FFFE,0003,FFFC]'),
       'record arguments must include signed and raw halfword views')
assert(normal:has('right=invalid(0x80001001)'),
       'an odd halfword pointer must be rejected')
assert(normal:has('left=null'), 'a null record argument must stay explicit')
assert(normal:has(
    'queue sequence=1 slot=0 left=[10,-20,30,-40]'
), 'mode 2 must dump the bounded queued record')
assert(normal:has('arg3=-7/0xFFFFFFF9'),
       'signed and raw argument views must agree')
assert(normal:has('caller_probe frame=000000 main_mode=0x23'),
       'caller probes must retain the screen mode')
assert(normal:disabled(), 'a finished trace must disable every breakpoint')

local outside = capture()
outside:dispatch(0, 0x7ffffffe, 0x801ffffa, 0, CALLER)
outside:dispatch(2, 0x80200000, 0, 0, CALLER)
outside:frames(72000)
assert(outside:has('left=invalid(0x7FFFFFFE)'),
       'an address below RAM must be rejected')
assert(outside:has('right=invalid(0x801FFFFA)'),
       'a record crossing the RAM ceiling must be rejected')
assert(outside:has('left=invalid(0x80200000)'),
       'an address at the RAM ceiling must be rejected')
assert(outside:has('status: timed out after partial dispatcher activity'),
       'partial activity must end with a useful timeout')

local silent = capture()
silent:frames(600)
assert(silent:has(
    'no caller or dispatcher hits; confirm interpreter CPU'
), 'silence must identify the likely setup mistake')
silent:frames(71400)
assert(silent:has(
    'status: timed out without a dispatcher hit; use interpreter CPU'
), 'a no-hit run must finish with the interpreter diagnosis')
assert(silent:disabled())

local callerOnly = capture()
callerOnly:caller(CALLER)
callerOnly:frames(600)
assert(callerOnly:has('caller probes fired but dispatcher did not'),
       'caller-only activity must be distinguished from total silence')
callerOnly:frames(71400)
assert(callerOnly:has(
    'status: timed out after caller activity without a dispatcher hit'
), 'caller-only activity must retain its own final status')

local broken = capture()
broken:breakRegisters()
broken:dispatch(0, 0, 0, 0, CALLER)
broken:frames(1)
assert(broken:has('status: breakpoint callback error:'),
       'callback failures must be surfaced on the next frame')
assert(broken:disabled())

local limited = capture()
for _ = 1, 96 do
    limited:dispatch(1, 0, 0, 0, CALLER)
end
limited:frames(1)
assert(limited:has('status: maximum hit count reached; partial trace follows'),
       'the dispatcher capture must stop at its hit bound')
assert(limited:has('summary: hits=96'), 'the hit bound must be visible')
assert(limited:disabled())

print = hostPrint
print('record_dispatch_runtime_role: all six callback-replay cases passed')
