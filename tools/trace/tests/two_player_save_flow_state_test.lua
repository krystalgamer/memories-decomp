-- Run from the repository root:
--   luajit tools/trace/tests/two_player_save_flow_state_test.lua
-- Replays the fixed-address polling callbacks with mock PCSX events and a
-- real LuaJIT FFI memory buffer.
local ffi = require('ffi')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/two_player_save_flow_state.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local FLOW_OWNER_FLAGS = 0x8009b2eb
local FAILURE_MESSAGE = 0x8009b3c0
local DISPLAY_MESSAGE = 0x8009b3c6
local REQUEST_STEP = 0x8009b3de
local LOAD_STATE = 0x8009b3ea
local LOAD_INIT_FLAGS = 0x8009b3ed
local EFFECT_CHANNEL = 0x8009b3ee
local REQUEST_OUTCOME = 0x8009b3ef
local DIALOG_FLAGS = 0x8009b3fa
local IO_RESULT = 0x8009b450
local DECK_ERROR_SIDE = 0x801d5648
local PAD1_PRESSED = 0x8009b398
local PAD2_PRESSED = 0x8009b39a

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
        if value < 0 then
            value = value + 0x100000000
        end
        set16(addr, value % 0x10000)
        set16(addr + 2, math.floor(value / 0x10000))
    end

    set8(MAIN_MODE, 8)
    set32(IO_RESULT, -1)

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
    listener_two_player_save_flow_state_reset = nil
    listener_two_player_save_flow_state = nil
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

    function result:set32(addr, value)
        set32(addr, value)
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

local normal = capture()
normal:set8(FLOW_OWNER_FLAGS, 0x80)
normal:set8(LOAD_INIT_FLAGS, 0x80)
normal:set8(LOAD_STATE, 0x80)
normal:set8(FAILURE_MESSAGE, 0x28)
normal:frames(1)
normal:set8(LOAD_STATE, 1)
normal:set16(PAD1_PRESSED, 0x40)
normal:frames(1)
normal:set16(PAD1_PRESSED, 0)
normal:set8(LOAD_STATE, 2)
normal:set8(REQUEST_STEP, 1)
normal:set8(EFFECT_CHANNEL, 2)
normal:set16(DIALOG_FLAGS, 0x8000)
normal:frames(1)
normal:set8(LOAD_STATE, 0x43)
normal:set8(DISPLAY_MESSAGE, 0xd0)
normal:set8(REQUEST_OUTCOME, 1)
normal:set32(IO_RESULT, 1)
normal:frames(1)
normal:set16(DIALOG_FLAGS, 0)
normal:set8(LOAD_STATE, 0x0a)
normal:set8(FAILURE_MESSAGE, 0x24)
normal:set32(DECK_ERROR_SIDE, 0)
normal:frames(1)
normal:set8(FLOW_OWNER_FLAGS, 0)
normal:set8(MAIN_MODE, 0x10)
normal:frames(1)
assert(normal:has('status: reached two-player post-load mode 16'))
assert(normal:has('load_raw=0x43 load_low=3 load_high=0x40'))
assert(normal:has('failure_message=0x24'))
assert(normal:has('request_step=1 request_outcome=1 effect_channel=2'))
assert(normal:has('pad1_pressed=0x0040'))
assert(normal:has('unique_raw_states=0x01,0x02,0x0A,0x43,0x80'))
assert(normal:has('final_mode=16 final_load_raw=0x0A final_load_low=10'))

local partial = capture()
partial:set8(FLOW_OWNER_FLAGS, 0x80)
partial:set8(LOAD_STATE, 0x80)
partial:frames(1)
partial:set8(FLOW_OWNER_FLAGS, 0)
partial:set8(LOAD_STATE, 0)
partial:frames(1)
partial:frames(119)
assert(not partial:has(
    'status: two-save flow ended before post-load mode; partial trace'
))
partial:frames(1)
assert(partial:has(
    'status: two-save flow ended before post-load mode; partial trace'
))

local capped = capture()
capped:set8(FLOW_OWNER_FLAGS, 0x80)
capped:set8(LOAD_STATE, 0x80)
capped:frames(1)
for value = 1, 63 do
    capped:set8(DISPLAY_MESSAGE, value)
    capped:frames(1)
end
assert(capped:has('status: maximum transition count reached'))
assert(capped:has('transitions=64'))

local reset = capture()
reset:reset()
assert(reset:has('status: reset observed; rerun from the loaded main menu'))
assert(reset:has('transitions=0 unique_raw_states=none'))

local silent = capture()
silent:frames(600)
assert(silent:has(
    'no flow observed yet; enter 2P Duel from the loaded menu'
))
silent:frames(71400)
assert(silent:has('status: timed out before the two-save flow was observed'))

print = hostPrint
print('two_player_save_flow_state: all five polling cases passed')
