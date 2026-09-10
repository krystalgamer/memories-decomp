-- Run from the repository root:
--   luajit tools/trace/tests/options_input_map_test.lua
-- Replays Options input samples with mock PCSX events and real FFI RAM.
local ffi = require('ffi')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/options_input_map.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local OPTIONS_STATE = 0x8009b37c
local OPTIONS_OUTPUT_TYPE = 0x8009b37d
local OPTIONS_SELECTION = 0x8009b384
local PAD1_PRESSED = 0x8009b398
local STORED_OUTPUT_TYPE = 0x8009b408

local function capture(mode)
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

    set8(MAIN_MODE, mode or 11)
    set8(OPTIONS_STATE, 1)
    set8(OPTIONS_OUTPUT_TYPE, 0)
    set8(OPTIONS_SELECTION, 0)
    set8(STORED_OUTPUT_TYPE, 0)

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
    listener_options_input_map_reset = nil
    listener_options_input_map = nil
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

    function result:mode(value)
        set8(MAIN_MODE, value)
    end

    function result:input(pressed)
        set16(PAD1_PRESSED, pressed or 0)
    end

    function result:frames(count)
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:sample(pressed, mutate)
        self:input(pressed)
        self:frames(1)
        if mutate ~= nil then
            mutate(self)
        end
        self:input(0)
        self:frames(10)
    end

    function result:reset()
        events['ExecutionFlow::Reset']()
    end

    return result
end

local normal = capture()
normal:sample(0x2000, function(r)
    r:set8(OPTIONS_OUTPUT_TYPE, 1)
    r:set8(STORED_OUTPUT_TYPE, 1)
end)
normal:sample(0x8000, function(r)
    r:set8(OPTIONS_OUTPUT_TYPE, 0)
    r:set8(STORED_OUTPUT_TYPE, 0)
end)
normal:sample(0x4000, function(r)
    r:set8(OPTIONS_SELECTION, 1)
end)
normal:sample(0x4000, function(r)
    r:set8(OPTIONS_SELECTION, 2)
end)
normal:input(0x40)
normal:frames(1)
normal:set8(OPTIONS_STATE, 3)
normal:frames(1)
normal:set8(OPTIONS_STATE, 1)
normal:input(0)
normal:frames(9)
normal:frames(1)
normal:sample(0x1000, function(r)
    r:set8(OPTIONS_SELECTION, 1)
end)
normal:sample(0x1000, function(r)
    r:set8(OPTIONS_SELECTION, 0)
end)
normal:input(0x20)
normal:frames(1)
normal:set8(OPTIONS_STATE, 0)
normal:mode(8)
normal:input(0)
normal:frames(1)
assert(normal:has('status: captured input that left Options'))
assert(normal:has('samples=8'))
assert(normal:has('pressed=0x2000'))
assert(normal:has('pressed=0x0040'))
assert(normal:has('output_type=0->1 stored_output_type=0->1'))
assert(normal:has('selection=1->2'))
assert(normal:has('sample_change=05.01'))
assert(normal:has('state=0x01'))
assert(normal:has('state=0x03'))
assert(normal:has('mode=11->8'))

local earlyExit = capture()
earlyExit:input(0x20)
earlyExit:frames(1)
earlyExit:set8(OPTIONS_STATE, 0)
earlyExit:mode(8)
earlyExit:input(0)
earlyExit:frames(1)
assert(earlyExit:has('samples=1'))
assert(earlyExit:has('status: captured input that left Options'))

local quiet = capture()
for _ = 1, 8 do
    quiet:sample(0x10)
end
quiet:frames(117)
assert(not quiet:has('status: captured eight inputs followed by quiet time'))
quiet:frames(1)
assert(quiet:has('status: captured eight inputs followed by quiet time'))

local latched = capture()
latched:input(0x2000)
latched:frames(1)
latched:input(0x8000)
latched:frames(12)
latched:input(0)
latched:frames(1)
latched:reset()
assert(latched:has('samples=1'))

local unsettled = capture()
unsettled:input(0x4000)
unsettled:frames(1)
unsettled:input(0)
for frame = 1, 120 do
    unsettled:set8(OPTIONS_SELECTION, frame % 2)
    unsettled:frames(1)
end
unsettled:reset()
assert(unsettled:has('reason=settlement timeout'))

local badMode = capture(8)
assert(badMode:has('main mode 8 is not Options mode 11'))

local reset = capture()
reset:reset()
assert(reset:has('status: reset observed; rerun from Options'))

local silent = capture()
silent:frames(600)
assert(silent:has(
    'no input captured yet; perform the documented sequence'
))
silent:frames(35400)
assert(silent:has('status: timed out after a partial Options input trace'))

print = hostPrint
print('options_input_map: all eight polling cases passed')
