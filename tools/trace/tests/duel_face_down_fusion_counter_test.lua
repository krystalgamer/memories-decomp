-- Run from the repository root:
--   luajit tools/trace/tests/duel_face_down_fusion_counter_test.lua
-- Replays the two-turn polling flow with mock PCSX events and real FFI RAM.
local ffi = require('ffi')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/duel_face_down_fusion_counter.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local ACTIVE_SIDE = 0x8009b1d5
local OPPONENT_ID = 0x8009b361
local PLAYER_STATS = 0x800e9ff0

local function capture(mode, side, opponent)
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local events = {}
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
        Events = {
            createEventListener = function(name, callback)
                events[name] = callback
                return {}
            end,
        },
    }
    listener_duel_face_down_fusion_counter_reset = nil
    listener_duel_face_down_fusion_counter = nil
    set8(MAIN_MODE, mode or 3)
    set8(ACTIVE_SIDE, side or 0)
    set8(OPPONENT_ID, opponent or 1)
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

    function result:side(value)
        set8(ACTIVE_SIDE, value)
    end

    function result:opponent(value)
        set8(OPPONENT_ID, value)
    end

    function result:counter(offset, value)
        set8(PLAYER_STATS + offset, value)
    end

    function result:frames(count)
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:reset()
        events['ExecutionFlow::Reset']()
    end

    return result
end

local normal = capture(3, 0, 1)
normal:frames(1)
normal:counter(0x04, 1)
normal:frames(1)
normal:side(1)
normal:frames(1)
normal:counter(0x01, 1)
normal:counter(0x18, 5)
normal:side(0)
normal:frames(1)
normal:counter(0x08, 1)
normal:frames(1)
normal:side(1)
normal:frames(1)
assert(normal:has(
    'turn_delta=1(single_face_down_control) turns=0 face_down=1 '
        .. 'fusions=0 equips=0 draw_cursor=0'
))
assert(normal:has(
    'turn_delta=2(multi_card_fusion_face_down) turns=0 face_down=0 '
        .. 'fusions=1 equips=0 draw_cursor=0'
))
assert(normal:has('status: captured single-card and multi-card face-down turns'))

local creditedFusion = capture(3, 0, 1)
creditedFusion:frames(1)
creditedFusion:counter(0x04, 1)
creditedFusion:side(1)
creditedFusion:frames(1)
creditedFusion:side(0)
creditedFusion:frames(1)
creditedFusion:counter(0x04, 2)
creditedFusion:counter(0x08, 1)
creditedFusion:side(1)
creditedFusion:frames(1)
assert(creditedFusion:has(
    'turn_delta=2(multi_card_fusion_face_down) turns=0 face_down=1 '
        .. 'fusions=1 equips=0 draw_cursor=0'
))

local waitsForPlayer = capture(3, 1, 1)
waitsForPlayer:frames(60)
assert(not waitsForPlayer:has('turn_start=1'))
waitsForPlayer:side(0)
waitsForPlayer:frames(1)
assert(waitsForPlayer:has('turn 1 started; play one card face-down, then end'))
waitsForPlayer:reset()
assert(waitsForPlayer:has('turn_start=1(single_face_down_control)'))

local twoPlayer = capture(3, 0, 0xff)
twoPlayer:frames(1)
assert(twoPlayer:has('status: two-player duel detected; use an ordinary CPU duel'))

local partial = capture(3, 0, 1)
partial:frames(1)
partial:mode(8)
partial:frames(1)
assert(partial:has(
    'status: left duel mode before both controlled turns completed'
))

local wrapped = capture(3, 0, 1)
wrapped:counter(0x04, 0xff)
wrapped:frames(1)
wrapped:counter(0x04, 0)
wrapped:side(1)
wrapped:frames(1)
wrapped:side(0)
wrapped:frames(1)
wrapped:counter(0x08, 1)
wrapped:side(1)
wrapped:frames(1)
assert(wrapped:has(
    'turn_delta=1(single_face_down_control) turns=0 face_down=1 '
        .. 'fusions=0 equips=0 draw_cursor=0'
))

local reset = capture(3, 0, 1)
reset:frames(1)
reset:reset()
assert(reset:has(
    'status: reset observed; rerun after the first player hand is drawn'
))

local silent = capture(8, 0, 1)
silent:frames(108000)
assert(silent:has('duel mode not observed'))
assert(silent:has('status: timed out before the first controlled player turn'))

print = hostPrint
print('duel_face_down_fusion_counter: all eight polling cases passed')
