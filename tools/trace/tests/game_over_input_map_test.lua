-- Run from the repository root:
--   luajit tools/trace/tests/game_over_input_map_test.lua
-- Replays Game Over input samples with mock PCSX events and real FFI RAM.
-- It does not establish visible choices or destination semantics.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/game_over_input_map.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local FRONTEND_REQUEST = 0x8009b268
local GAME_OVER_DESTINATION = 0x8009b269
local FRONTEND_MENU_ID = 0x8009b26d
local GAME_OVER_OBJECT = 0x8009b378
local PAD1_PRESSED = 0x8009b398
local FADE_STATE = 0x800e9ec8
local FADE_LEVEL = FADE_STATE + 4
local FADE_TARGET = FADE_STATE + 5
local FADE_FLAGS = FADE_STATE + 6
local FADE_STEP = FADE_STATE + 7

local function capture(mode, fadeFlags)
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local events = {}
    local output = {}

    local function byte(addr)
        return ffi.cast('uint8_t*', memory + (addr - RAM_BASE))
    end

    local function word(addr)
        return ffi.cast('uint32_t*', memory + (addr - RAM_BASE))
    end

    local function set8(addr, value)
        byte(addr)[0] = value % 0x100
    end

    local function set16(addr, value)
        set8(addr, value)
        set8(addr + 1, math.floor(value / 0x100))
    end

    set8(MAIN_MODE, mode or 0x4c)
    set8(FRONTEND_REQUEST, 0)
    set8(GAME_OVER_DESTINATION, 2)
    set8(FRONTEND_MENU_ID, 5)
    word(GAME_OVER_OBJECT)[0] = 0x80123456
    set16(PAD1_PRESSED, 0)
    set8(FADE_LEVEL, 0xff)
    set8(FADE_TARGET, 0xff)
    set8(FADE_FLAGS, fadeFlags or 0)
    set8(FADE_STEP, 2)

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
    listener_game_over_input_map_reset = nil
    listener_game_over_input_map = nil
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

    function result:installed()
        return events['GPU::Vsync'] ~= nil
    end

    return result
end

local normal = capture()
normal:sample(0x1000)
normal:sample(0x4000)
normal:sample(0x0800)
normal:sample(0x0100)
normal:sample(0x0010, function(result)
    result:set8(FADE_LEVEL, 0xfe)
end)
normal:input(0x0020)
normal:frames(1)
normal:set8(FRONTEND_REQUEST, 1)
normal:set8(FRONTEND_MENU_ID, 0)
normal:set8(FADE_FLAGS, 0x80)
normal:mode(8)
normal:input(0)
normal:frames(1)
assert(normal:has('status: captured input that left Game Over'))
assert(normal:has('summary: samples=6 mode=8 destination=2'))
assert(normal:has('pressed=0x1000'))
assert(normal:has('pressed=0x0020'))
assert(normal:has('sample_change=05.01'))
assert(normal:has('mode=12->8 destination=2->2'))
assert(normal:has('frontend_request=0->1 menu_id=5->0'))

local alternateExit = capture()
alternateExit:input(0x0040)
alternateExit:frames(1)
alternateExit:set8(GAME_OVER_DESTINATION, 0)
alternateExit:mode(0)
alternateExit:input(0)
alternateExit:frames(1)
assert(alternateExit:has('summary: samples=1 mode=0 destination=0'))
assert(alternateExit:has('mode=12->0 destination=2->0'))

local quiet = capture()
for _, pressed in ipairs({0x1000, 0x4000, 0x0800, 0x0100, 0x0010, 0x0004}) do
    quiet:sample(pressed)
end
quiet:frames(120)
assert(quiet:has('status: captured six inputs followed by quiet time'))

local unsettled = capture()
unsettled:input(0x1000)
unsettled:frames(1)
unsettled:input(0)
for frame = 1, 120 do
    unsettled:set8(FADE_LEVEL, frame % 2)
    unsettled:frames(1)
end
unsettled:reset()
assert(unsettled:has('reason=settlement timeout'))

local reset = capture()
reset:sample(0x1000)
reset:reset()
assert(reset:has('status: reset observed; rerun from Game Over'))
assert(reset:has('summary: samples=1'))

local silent = capture()
silent:frames(600)
assert(silent:has(
    'no input captured yet; wait for the fade, then perform'
))
silent:frames(35400)
assert(silent:has('status: timed out after a partial Game Over input trace'))

local wrongMode = capture(8)
assert(wrongMode:has('status: main mode 8 is not Game Over mode 12'))
assert(not wrongMode:installed())

local fading = capture(nil, 0x80)
assert(fading:has('armed while fade flag 0x80 is active'))
fading:reset()
assert(fading:has('status: reset observed; rerun from Game Over'))

print = hostPrint
print('game_over_input_map: all eight polling cases passed')
