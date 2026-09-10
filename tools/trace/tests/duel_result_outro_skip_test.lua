-- Run from the repository root:
--   luajit tools/trace/tests/duel_result_outro_skip_test.lua
-- Replays result-outro polling with mock PCSX events and real FFI RAM.
-- It does not establish any visible skip behavior.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/duel_result_outro_skip.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local RESULT_STATE = 0x8009b174
local RESULT_TIMER = 0x8009b1d0
local DUEL_STATE = 0x8009b23a
local OPPONENT_ID = 0x8009b361
local PAD1_PRESSED = 0x8009b398

local function capture(mode, state, timer)
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

    set8(MAIN_MODE, mode or 0x43)
    set8(RESULT_STATE, state or 2)
    set16(RESULT_TIMER, timer or 0)
    set16(DUEL_STATE, 0x800d)
    set8(OPPONENT_ID, 5)
    set16(PAD1_PRESSED, 0)

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
    listener_duel_result_outro_skip_reset = nil
    listener_duel_result_outro_skip = nil
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

    function result:frames(count)
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:phase(stateValue, timerValue)
        set8(RESULT_STATE, stateValue)
        set16(RESULT_TIMER, timerValue)
    end

    function result:input(value)
        set16(PAD1_PRESSED, value or 0)
    end

    function result:mode(value)
        set8(MAIN_MODE, value)
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
normal:frames(10)
normal:phase(0x83, 600)
normal:frames(1)
normal:input(0x0010)
normal:frames(1)
normal:input(0)
normal:frames(1)
normal:phase(0x83, 580)
normal:input(0x0020)
normal:frames(1)
normal:input(0)
normal:phase(0xc3, 579)
normal:frames(1)
normal:phase(4, 579)
normal:frames(1)
assert(normal:has('status: result outro left phase 3'))
assert(normal:has('phase_start frame=000011'))
assert(normal:has('input=01') and normal:has('pressed=0x0010'))
assert(normal:has('input=02') and normal:has('pressed=0x0020'))
assert(normal:has(
    'departure frame=000015 timer=579 last_input=0x0020'
))

local automatic = capture(nil, 0x83, 600)
automatic:phase(0xc3, 590)
automatic:frames(1)
automatic:frames(120)
assert(automatic:has(
    'departure frame=000001 timer=590 last_input=0x0000'
))
assert(automatic:has(
    'status: captured departure flag and follow-up window'
))

local leftMode = capture(nil, 0x83, 600)
leftMode:mode(6)
leftMode:frames(1)
assert(leftMode:has('status: left duel mode after phase 3 was observed'))

local waiting = capture()
waiting:frames(600)
assert(waiting:has('result phase 3 not observed yet'))
waiting:frames(35400)
assert(waiting:has('status: timed out before result phase 3'))

local phaseTimeout = capture(nil, 0x83, 600)
phaseTimeout:frames(3600)
assert(phaseTimeout:has(
    'status: phase 3 observation timed out before departure'
))

local limited = capture(nil, 0x83, 600)
for input = 1, 9 do
    limited:input(input)
    limited:frames(1)
    limited:input(0)
    limited:frames(1)
end
assert(limited:has('status: maximum input count reached'))
assert(limited:has('summary: phase_observed=true phase_start_timer=600 inputs=8'))

local reset = capture(nil, 0x83, 600)
reset:input(0x10)
reset:frames(1)
reset:reset()
assert(reset:has('status: reset observed; rerun from the duel result'))
assert(reset:has('inputs=1'))

local wrongMode = capture(8)
assert(wrongMode:has('status: main mode 8 is not duel mode 3'))
assert(not wrongMode:installed())

print = hostPrint
print('duel_result_outro_skip: all eight polling cases passed')
