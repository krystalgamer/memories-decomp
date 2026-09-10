-- Run from the repository root:
--   luajit tools/trace/tests/duel_pad2_remap_test.lua
-- Replays the fixed-address polling callback with mock PCSX events and real
-- LuaJIT FFI RAM.
local ffi = require('ffi')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/duel_pad2_remap.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local OPPONENT_ID = 0x8009b361
local PLAYING_SIDE = 0x8009b1d5
local INPUT_SIDE_OVERRIDE = 0x8009b238
local SAVE_POINTER_1 = 0x8009b1d8
local SAVE_POINTER_2 = 0x8009b1dc
local PAD1_REPEAT = 0x8009b394
local PAD2_REPEAT = 0x8009b396
local PAD1_PRESSED = 0x8009b398
local PAD2_PRESSED = 0x8009b39a
local PAD1_HELD = 0x8009b3a4
local PAD2_HELD = 0x8009b3a6

local function capture()
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
        value = value % 0x100000000
        set16(addr, value % 0x10000)
        set16(addr + 2, math.floor(value / 0x10000))
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
    listener_duel_pad2_remap = nil
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

    function result:count(text)
        local count = 0
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then
                count = count + 1
            end
        end
        return count
    end

    function result:frames(count)
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:mode(value)
        set8(MAIN_MODE, value)
    end

    function result:opponent(value)
        set8(OPPONENT_ID, value)
    end

    function result:pointers(first, second)
        set32(SAVE_POINTER_1, first or 0)
        set32(SAVE_POINTER_2, second or 0)
    end

    function result:selector(playing, override)
        set8(PLAYING_SIDE, playing)
        set8(INPUT_SIDE_OVERRIDE, override)
    end

    function result:input(pad, held, pressed, repeatValue)
        local heldAddr = pad == 1 and PAD1_HELD or PAD2_HELD
        local pressedAddr = pad == 1 and PAD1_PRESSED or PAD2_PRESSED
        local repeatAddr = pad == 1 and PAD1_REPEAT or PAD2_REPEAT
        set16(heldAddr, held or 0)
        set16(pressedAddr, pressed or 0)
        set16(repeatAddr, repeatValue or 0)
    end

    function result:twoPlayer(playing, override)
        self:mode(3)
        self:opponent(0xff)
        self:pointers(0x801d0000, 0x801d1000)
        self:selector(playing or 0, override == nil and 0xff or override)
    end

    return result
end

local normal = capture()
normal:twoPlayer(0, 0xff)
normal:frames(1)
normal:input(1, 0x0040, 0x0040, 0)
normal:frames(1)
normal:input(1, 0, 0, 0)
normal:frames(1)
normal:input(1, 0x0040, 0x0040, 0)
normal:frames(1)
normal:input(1, 0, 0, 0)
normal:frames(1)
normal:selector(1, 0xff)
normal:frames(1)
normal:input(2, 0x0020, 0x0020, 0)
normal:frames(1)
normal:input(2, 0, 0, 0)
normal:frames(1)
normal:frames(1797)
assert(not normal:has(
    'status: observed both playing-side values and then settled'
))
normal:frames(1)
assert(normal:has(
    'status: observed both playing-side values and then settled'
))
assert(normal:has(
    'summary: samples=4 saw_playing_side_0=true saw_playing_side_1=true'
))
assert(normal:count(
    'first pad 1 pressed mask 0x0040 for selected side 0'
) == 1)
assert(normal:has(
    'first pad 2 pressed mask 0x0020 for selected side 1'
))
assert(normal:has('selected_input_side=0'))
assert(normal:has('selected_input_side=1'))

local override = capture()
override:twoPlayer(0, 1)
override:frames(1)
override:selector(0, 0xff)
override:frames(1)
override:mode(2)
override:frames(1)
assert(override:has('status: left the two-player duel'))
assert(override:has(
    'playing_side=0 input_side_override=1 selected_input_side=1'
))
assert(override:has(
    'playing_side=0 input_side_override=-1 selected_input_side=0'
))

local singlePlayer = capture()
singlePlayer:mode(3)
singlePlayer:opponent(5)
singlePlayer:pointers(0x801d0000, 0x801d1000)
singlePlayer:selector(1, 1)
singlePlayer:frames(1)
singlePlayer:mode(2)
singlePlayer:frames(1)
assert(singlePlayer:has('status: left the two-player duel'))
assert(singlePlayer:has(
    'playing_side=1 input_side_override=1 selected_input_side=0'
))

local pointerGuards = capture()
pointerGuards:mode(3)
pointerGuards:opponent(0xff)
pointerGuards:selector(0, 0xff)
pointerGuards:pointers(0, 0x801d1000)
pointerGuards:frames(1)
pointerGuards:pointers(0x801d0000, 0x80200000)
pointerGuards:frames(1)
pointerGuards:pointers(0x801d0000, 0x801d1000)
pointerGuards:frames(1)
pointerGuards:mode(2)
pointerGuards:frames(1)
assert(pointerGuards:has('status: left the two-player duel'))
assert(pointerGuards:has('summary: samples=1'))

local bounded = capture()
bounded:twoPlayer(0, 0xff)
bounded:frames(1)
for mask = 1, 63 do
    bounded:input(1, mask, mask, 0)
    bounded:frames(1)
end
assert(bounded:has('status: maximum sample count reached'))
assert(bounded:has('summary: samples=64'))

local silent = capture()
silent:frames(215999)
assert(not silent:has(
    'status: timed out before a two-player duel was observed'
))
silent:frames(1)
assert(silent:has(
    'status: timed out before a two-player duel was observed'
))
assert(silent:has(
    'summary: samples=0 saw_playing_side_0=false '
        .. 'saw_playing_side_1=false'
))

print = hostPrint
print('duel_pad2_remap: all six polling cases passed')
