-- Run from the repository root:
--   luajit tools/trace/tests/library_input_map_test.lua
-- Replays Library input samples with mock PCSX events and real FFI RAM.
local ffi = require('ffi')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/library_input_map.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local CURSOR_COLUMN = 0x8009b258
local CURSOR_ROW = 0x8009b259
local SELECTED_CARD_ID = 0x8009b338
local PAD1_PRESSED = 0x8009b398
local PAD1_HELD = 0x8009b3a4
local LIBRARY_STATE = 0x800ea1e8

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
    listener_library_input_map_reset = nil
    listener_library_input_map = nil
    set8(MAIN_MODE, mode or 4)
    set8(LIBRARY_STATE, 1)
    set8(CURSOR_ROW, 0)
    set8(CURSOR_COLUMN, 0)
    set16(SELECTED_CARD_ID, 1)
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

    function result:mode(value)
        set8(MAIN_MODE, value)
    end

    function result:state(value)
        set8(LIBRARY_STATE, value)
    end

    function result:row(value)
        set8(CURSOR_ROW, value)
    end

    function result:column(value)
        set8(CURSOR_COLUMN, value)
    end

    function result:selected(value)
        set16(SELECTED_CARD_ID, value)
    end

    function result:input(held, pressed)
        set16(PAD1_HELD, held or 0)
        set16(PAD1_PRESSED, pressed or 0)
    end

    function result:frames(count)
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:sample(held, pressed, mutate)
        self:input(held, pressed)
        self:frames(1)
        if mutate ~= nil then
            mutate(self)
        end
        self:input(0, 0)
        self:frames(9)
    end

    function result:reset()
        events['ExecutionFlow::Reset']()
    end

    return result
end

local normal = capture()
normal:sample(0x2000, 0, function(r) r:column(1) end)
normal:sample(0x8000, 0, function(r) r:column(0) end)
normal:sample(0x4000, 0, function(r) r:row(1) end)
normal:sample(0x1000, 0, function(r) r:row(0) end)
normal:sample(0x8, 0, function(r) r:row(10) end)
normal:sample(0x4, 0, function(r) r:row(0) end)
normal:sample(0, 0x40, function(r)
    r:state(2)
    r:selected(1)
end)
normal:sample(0, 0x20, function(r) r:state(1) end)
normal:input(0, 0x20)
normal:frames(1)
normal:mode(8)
normal:input(0, 0)
normal:frames(1)
assert(normal:has('status: captured input that left the Library'))
assert(normal:has('samples=9'))
assert(normal:has('held=0x2000'))
assert(normal:has('held=0x0008'))
assert(normal:has('pressed=0x0040'))
assert(normal:has('pressed=0x0020'))
assert(normal:has('sample_end=09'))
assert(normal:has('mode_changed=true'))

local badMode = capture(8)
assert(badMode:has('main mode 8 is not Library mode 4'))

local quiet = capture()
for _ = 1, 9 do
    quiet:sample(0, 0x10)
end
quiet:frames(117)
assert(not quiet:has('status: captured nine inputs followed by quiet time'))
quiet:frames(1)
assert(quiet:has('status: captured nine inputs followed by quiet time'))

local latched = capture()
latched:input(0x2000, 0)
latched:frames(1)
latched:input(0x8000, 0)
latched:frames(10)
latched:input(0, 0)
latched:frames(1)
latched:reset()
assert(latched:has('samples=1'))

local reset = capture()
reset:reset()
assert(reset:has('status: reset observed; rerun after entering the Library'))

local silent = capture()
silent:frames(36000)
assert(silent:has('status: timed out after a partial Library input trace'))

print = hostPrint
print('library_input_map: all six polling cases passed')
