-- Run from the repository root:
--   luajit tools/trace/tests/duel_pure_magic_counter_test.lua
-- Replays pure-magic counter polling with mock PCSX events and real FFI RAM.
-- It does not establish any card type or visible effect.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/duel_pure_magic_counter.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local ACTIVE_SIDE = 0x8009b1d5
local OPPONENT_ID = 0x8009b361
local PLAYER_STATS = 0x800e9ff0
local OFFSETS = {
    turns = 0x01,
    pure_magic = 0x05,
    traps = 0x06,
    fusions = 0x08,
    equips = 0x09,
    cards_used = 0x18,
}

local function capture(mode, side, counters)
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local events = {}
    local output = {}

    local function byte(addr)
        return ffi.cast('uint8_t*', memory + (addr - RAM_BASE))
    end

    local function set8(addr, value)
        byte(addr)[0] = value % 0x100
    end

    set8(MAIN_MODE, mode or 0x43)
    set8(ACTIVE_SIDE, side or 0)
    set8(OPPONENT_ID, 5)
    for name, offset in pairs(OFFSETS) do
        set8(PLAYER_STATS + offset, counters and counters[name] or 0)
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
    listener_duel_pure_magic_counter_reset = nil
    listener_duel_pure_magic_counter = nil
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

    function result:counter(name, value)
        set8(PLAYER_STATS + OFFSETS[name], value)
    end

    function result:side(value)
        set8(ACTIVE_SIDE, value)
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
normal:counter('pure_magic', 1)
normal:counter('cards_used', 1)
normal:frames(1)
normal:frames(120)
assert(normal:has(
    'status: captured pure-magic increment and follow-up window'
))
assert(normal:has('pure_magic_increment frame=000001 delta=1'))
assert(normal:has(
    'delta turns=0 pure_magic=1 traps=0 fusions=0 equips=0 cards_used=1'
))

local wrapped = capture(nil, nil, {pure_magic = 0xff})
wrapped:counter('pure_magic', 0)
wrapped:frames(1)
wrapped:frames(120)
assert(wrapped:has('pure_magic_increment frame=000001 delta=1'))

local wrongCounter = capture()
wrongCounter:counter('traps', 1)
wrongCounter:frames(1)
wrongCounter:side(1)
wrongCounter:frames(1)
assert(wrongCounter:has(
    'status: player turn ended without a pure-magic increment'
))
assert(wrongCounter:has('delta turns=0 pure_magic=0 traps=1'))

local waiting = capture(nil, 1)
waiting:frames(600)
assert(waiting:has('waiting for player side 0'))
waiting:side(0)
waiting:frames(1)
waiting:counter('pure_magic', 1)
waiting:frames(1)
waiting:reset()
assert(waiting:has('started=true'))
assert(waiting:has('pure_magic_observed=true'))

local waitingTimeout = capture(nil, 1)
waitingTimeout:frames(72000)
assert(waitingTimeout:has(
    'status: timed out before a player turn was observed'
))

local waitingBoundary = capture(nil, 1)
waitingBoundary:frames(71999)
waitingBoundary:side(0)
waitingBoundary:frames(1)
assert(not waitingBoundary:has(
    'status: timed out before a player turn was observed'
))
waitingBoundary:reset()
assert(waitingBoundary:has('started=true'))

local limited = capture()
for change = 1, 32 do
    limited:counter('traps', change)
    limited:frames(1)
end
assert(limited:has('status: maximum counter change count reached'))
assert(limited:has('changes=32'))

local leftMode = capture()
leftMode:mode(8)
leftMode:frames(1)
assert(leftMode:has('status: left duel mode before observation completed'))

local timeout = capture()
timeout:frames(36000)
assert(timeout:has('status: timed out without a pure-magic increment'))

local timeoutBoundary = capture()
timeoutBoundary:frames(35999)
timeoutBoundary:counter('pure_magic', 1)
timeoutBoundary:frames(1)
timeoutBoundary:frames(120)
assert(timeoutBoundary:has(
    'pure_magic_increment frame=036000 delta=1'
))
assert(timeoutBoundary:has(
    'status: captured pure-magic increment and follow-up window'
))
assert(not timeoutBoundary:has(
    'status: timed out without a pure-magic increment'
))

local sideTransition = capture()
sideTransition:side(1)
sideTransition:frames(1)
assert(sideTransition:has(
    'baseline frame=000000 mode=3 active_side=0 opponent_id=5'
))
assert(sideTransition:has(
    'final frame=000001 mode=3 active_side=1 opponent_id=5'
))

local reset = capture()
reset:counter('cards_used', 1)
reset:frames(1)
reset:reset()
assert(reset:has('status: reset observed; rerun from the controlled duel'))
assert(reset:has('changes=1'))

local wrongMode = capture(8)
assert(wrongMode:has('status: main mode 8 is not duel mode 3'))
assert(not wrongMode:installed())

print = hostPrint
print('duel_pure_magic_counter: all thirteen polling cases passed')
