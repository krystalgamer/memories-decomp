-- Run from the repository root:
--   luajit tools/trace/tests/nitemare_password_flag_test.lua
-- Replays target flag helper callbacks with mock PCSX events and real FFI RAM.
local ffi = require('ffi')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/nitemare_password_flag.lua'

local RAM_BASE = 0x80000000
local TEST_HELPER = 0x8002cca8
local UPDATE_HELPER = 0x8002cce4
local MAIN_MODE = 0x8009b26c
local CAMPAIGN_SCENE = 0x8009b27a
local OPPONENT_ID = 0x8009b361
local FLAG_BYTE = 0x801d06d6
local TARGET_FLAG = 0x5f7
local CLEAR_MODIFIER = 0x8000

local function capture()
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = {GPR = {n = {a0 = 0, ra = 0}}}
    local events = {}
    local breakpoints = {}
    local output = {}

    local function byte(addr)
        return ffi.cast('uint8_t*', memory + (addr - RAM_BASE))
    end

    local function set8(addr, value)
        byte(addr)[0] = value % 0x100
    end

    print = function(...)
        local values = {}
        for index = 1, select('#', ...) do
            values[index] = tostring(select(index, ...))
        end
        output[#output + 1] = table.concat(values, '\t')
    end

    PCSX = {
        getMemPtr = function()
            return memory
        end,
        getRegisters = function()
            return registers
        end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = {enabled = true, callback = callback}
            function handle:disable()
                self.enabled = false
            end
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
    breakpoint_nitemare_password_flag_test = nil
    breakpoint_nitemare_password_flag_update = nil
    listener_nitemare_password_flag_reset = nil
    listener_nitemare_password_flag = nil
    set8(MAIN_MODE, 2)
    set8(CAMPAIGN_SCENE, 0x5d)
    set8(OPPONENT_ID, 38)
    dofile(scriptPath)

    local result = {}

    function result:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then
                return true
            end
        end
        return false
    end

    function result:setFlag(value)
        set8(FLAG_BYTE, value and 1 or 0)
    end

    function result:mode(value)
        set8(MAIN_MODE, value)
    end

    function result:frames(count)
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:test(raw, callsite)
        registers.GPR.n.a0 = raw
        registers.GPR.n.ra = (callsite or 0x80038d80) + 8
        assert(breakpoints[TEST_HELPER].enabled)
        breakpoints[TEST_HELPER].callback()
    end

    function result:update(raw, callsite)
        registers.GPR.n.a0 = raw
        registers.GPR.n.ra = (callsite or 0x8016a760) + 8
        assert(breakpoints[UPDATE_HELPER].enabled)
        breakpoints[UPDATE_HELPER].callback()
        self:setFlag(raw < CLEAR_MODIFIER)
    end

    function result:reset()
        events['ExecutionFlow::Reset']()
    end

    function result:disabled()
        return not breakpoints[TEST_HELPER].enabled
            and not breakpoints[UPDATE_HELPER].enabled
    end

    return result
end

local clear = capture()
clear:setFlag(false)
clear:test(TARGET_FLAG, 0x80038d40)
clear:mode(15)
clear:frames(1)
assert(clear:has('status: target test captured and credits mode began'))
assert(clear:has('flag_set=false'))
assert(clear:has('predicted_return=0x00000000'))
assert(clear:has('callsite=0x80038D40'))
assert(clear:disabled())

local set = capture()
set:setFlag(true)
set:test(TARGET_FLAG)
set:mode(15)
set:frames(1)
assert(set:has('flag_set=true'))
assert(set:has('predicted_return=0x00000001'))

local inverted = capture()
inverted:setFlag(false)
inverted:test(CLEAR_MODIFIER + TARGET_FLAG)
inverted:mode(15)
inverted:frames(1)
assert(inverted:has('inverted=true'))
assert(inverted:has('predicted_return=0x00000001'))

local writes = capture()
writes:setFlag(false)
writes:update(TARGET_FLAG)
writes:frames(1)
writes:update(CLEAR_MODIFIER + TARGET_FLAG)
writes:frames(1)
writes:test(TARGET_FLAG)
writes:mode(15)
writes:frames(1)
assert(writes:has('kind=write_entry'))
assert(writes:has('operation=set'))
assert(writes:has('operation=clear'))
assert(writes:has('post_write event=01'))
assert(writes:has('post_write event=02'))
assert(writes:has('writes=2'))

local ignored = capture()
ignored:test(TARGET_FLAG - 1)
ignored:update(TARGET_FLAG + 1)
ignored:frames(600)
assert(ignored:has('no target test yet'))
ignored:reset()
assert(ignored:has('tests=0 writes=0 events=0'))

local limited = capture()
for _ = 1, 33 do
    limited:test(TARGET_FLAG)
end
limited:frames(1)
assert(limited:has(
    'status: maximum target event count reached; partial trace follows'
))
assert(limited:has('tests=33 writes=0 events=32'))
assert(limited:disabled())

local reset = capture()
reset:reset()
assert(reset:has('status: reset observed; rerun before the Nitemare win text'))
assert(reset:disabled())

local silent = capture()
silent:frames(72000)
assert(silent:has(
    'status: timed out without a flag 0x5F7 test; use interpreter CPU'
))
assert(silent:disabled())

print = hostPrint
print('nitemare_password_flag: all eight callback-replay cases passed')
