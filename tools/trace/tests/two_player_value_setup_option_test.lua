-- Run from the repository root:
--   luajit tools/trace/tests/two_player_value_setup_option_test.lua
-- Replays value-setup input samples with mock PCSX events and real FFI RAM.
local ffi = require('ffi')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/two_player_value_setup_option.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local RESIDENT_TOGGLE = 0x8009b230
local RESIDENT_FIRST_LP = 0x8009b234
local RESIDENT_SECOND_LP = 0x8009b236
local PAD1_REPEAT = 0x8009b394
local PAD2_REPEAT = 0x8009b396
local PAD1_PRESSED = 0x8009b398
local PAD2_PRESSED = 0x8009b39a
local VALUE_SETUP_UPDATE = 0x801812b4
local VALUE_SETUP_UPDATE_FIRST_WORD = 0x27bdffd8
local SIDE_MODES = 0x801845bc
local VALUE_STATE = 0x801845c0

local function capture(signature, mode)
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local events = {}
    local output = {}

    local function byte(addr)
        return ffi.cast('uint8_t*', memory + (addr - RAM_BASE))
    end

    local function set8(addr, value)
        byte(addr)[0] = value % 0x100
    end

    local function set16(addr, value)
        set8(addr, value)
        set8(addr + 1, math.floor(value / 0x100))
    end

    local function set32(addr, value)
        set16(addr, value % 0x10000)
        set16(addr + 2, math.floor(value / 0x10000))
    end

    set32(VALUE_SETUP_UPDATE, signature or VALUE_SETUP_UPDATE_FIRST_WORD)
    set8(MAIN_MODE, mode or 16)
    set16(RESIDENT_TOGGLE, 1)
    set16(RESIDENT_FIRST_LP, 8000)
    set16(RESIDENT_SECOND_LP, 8000)
    set8(SIDE_MODES, 2)
    set8(SIDE_MODES + 1, 2)
    set8(SIDE_MODES + 2, 0)
    set16(VALUE_STATE, 8000)
    set16(VALUE_STATE + 2, 8000)
    set16(VALUE_STATE + 0xC, 8000)
    set16(VALUE_STATE + 0xE, 8000)

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
        Events = {
            createEventListener = function(name, callback)
                events[name] = callback
                return {}
            end,
        },
    }
    listener_two_player_value_setup_option_reset = nil
    listener_two_player_value_setup_option = nil
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

    function result:set8(addr, value)
        set8(addr, value)
    end

    function result:set16(addr, value)
        set16(addr, value)
    end

    function result:mode(value)
        set8(MAIN_MODE, value)
    end

    function result:input(repeat1, repeat2, pressed1, pressed2)
        set16(PAD1_REPEAT, repeat1 or 0)
        set16(PAD2_REPEAT, repeat2 or 0)
        set16(PAD1_PRESSED, pressed1 or 0)
        set16(PAD2_PRESSED, pressed2 or 0)
    end

    function result:frames(count)
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:sample(repeat1, repeat2, pressed1, pressed2, mutate)
        self:input(repeat1, repeat2, pressed1, pressed2)
        self:frames(1)
        if mutate ~= nil then
            mutate(self)
        end
        self:input(0, 0, 0, 0)
        self:frames(40)
    end

    function result:reset()
        events['ExecutionFlow::Reset']()
    end

    return result
end

local normal = capture()
normal:sample(0x1000, 0, 0, 0, function(r)
    r:set8(SIDE_MODES, 0)
end)
normal:sample(0x8000, 0, 0, 0, function(r)
    r:set8(SIDE_MODES, 0)
    r:set8(SIDE_MODES + 2, 0)
end)
normal:sample(0x2000, 0, 0, 0, function(r)
    r:set8(SIDE_MODES, 1)
    r:set8(SIDE_MODES + 2, 1)
end)
normal:sample(0x4000, 0, 0, 0, function(r)
    r:set8(SIDE_MODES, 2)
end)
normal:sample(0, 0x1000, 0, 0, function(r)
    r:set8(SIDE_MODES + 1, 1)
end)
normal:sample(0, 0x8000, 0, 0, function(r)
    r:set8(SIDE_MODES + 1, 0)
    r:set8(SIDE_MODES + 2, 0)
end)
normal:sample(0, 0x2000, 0, 0, function(r)
    r:set8(SIDE_MODES + 1, 1)
    r:set8(SIDE_MODES + 2, 1)
end)
normal:sample(0, 0x4000, 0, 0, function(r)
    r:set8(SIDE_MODES + 1, 2)
end)
normal:input(0, 0, 0x800, 0)
normal:frames(1)
normal:set16(RESIDENT_TOGGLE, 0)
normal:mode(3)
normal:input(0, 0, 0, 0)
normal:frames(1)
assert(normal:has(
    'status: captured input that left two-player value setup'
))
assert(normal:has('samples=9'))
assert(normal:has('pad1_repeat=0x1000'))
assert(normal:has('pad2_repeat=0x2000'))
assert(normal:has('pad1_pressed=0x0800'))
assert(normal:has('first_mode=2->0'))
assert(normal:has('second_mode=2->1'))
assert(normal:has('choice=0->1'))
assert(normal:has('resident_toggle=0x0001->0x0000 mode=16->3'))

local quiet = capture()
for _ = 1, 9 do
    quiet:sample(0, 0, 0x10, 0)
end
quiet:frames(109)
assert(not quiet:has('status: captured nine inputs followed by quiet time'))
quiet:frames(1)
assert(quiet:has('status: captured nine inputs followed by quiet time'))

local unsettled = capture()
unsettled:input(0x1000, 0, 0, 0)
unsettled:frames(1)
unsettled:input(0, 0, 0, 0)
for frame = 1, 240 do
    unsettled:set16(VALUE_STATE + 2, 8000 - (frame % 2))
    unsettled:frames(1)
end
unsettled:reset()
assert(unsettled:has('reason=settlement timeout'))
assert(unsettled:has('sample_end=01'))

local badSignature = capture(0x12345678, 16)
assert(badSignature:has('MainMenu_UpdateValueSetup signature mismatch'))

local badMode = capture(VALUE_SETUP_UPDATE_FIRST_WORD, 8)
assert(badMode:has('main mode 8 is not two-player value-setup mode 16'))

local reset = capture()
reset:reset()
assert(reset:has(
    'status: reset observed; rerun from two-player value setup'
))

local silent = capture()
silent:frames(600)
assert(silent:has(
    'no input captured yet; perform the documented sequence'
))
silent:frames(35400)
assert(silent:has(
    'status: timed out after a partial two-player value-setup trace'
))

print = hostPrint
print('two_player_value_setup_option: all seven polling cases passed')
