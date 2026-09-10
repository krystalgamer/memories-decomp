-- Run from the repository root:
--   luajit tools/trace/tests/duel_new_monster_attack_test.lua
-- Replays the polling flow with mock PCSX events and real LuaJIT FFI memory.
--
-- These cases test whether the trace records and rejects evidence correctly.
-- They do not establish the gameplay conclusion that the real emulator run
-- exists to answer.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/duel_new_monster_attack.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local ACTIVE_SIDE = 0x8009b1d5
local OPPONENT_ID = 0x8009b361
local OPPONENT_LP = 0x800ea024
local CARD_RECORDS = 0x801a7ad8
local CARD_RECORD_SIZE = 0x1c
local CARD_ID_OFFSET = 0x0c
local FLAGS_OFFSET = 0x16
local FLAG_OCCUPIED = 0x8000
local FLAG_USED_THIS_TURN = 0x4000

local function capture(mode, side, opponent, opponentLP)
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local events = {}
    local output = {}

    local function u8at(addr)
        return ffi.cast('uint8_t*', memory + (addr - RAM_BASE))
    end

    local function u16at(addr)
        return ffi.cast('uint16_t*', memory + (addr - RAM_BASE))
    end

    local function set8(addr, value)
        u8at(addr)[0] = value % 0x100
    end

    local function set16(addr, value)
        u16at(addr)[0] = value % 0x10000
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
    listener_duel_new_monster_attack = nil
    set8(MAIN_MODE, mode or 3)
    set8(ACTIVE_SIDE, side or 0)
    set8(OPPONENT_ID, opponent or 1)
    set16(OPPONENT_LP, opponentLP or 8000)
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

    function result:mode(value)
        set8(MAIN_MODE, value)
    end

    function result:side(value)
        set8(ACTIVE_SIDE, value)
    end

    function result:lp(value)
        set16(OPPONENT_LP, value)
    end

    function result:record(index, cardID, flags)
        local base = CARD_RECORDS + index * CARD_RECORD_SIZE
        set16(base + CARD_ID_OFFSET, cardID)
        set16(base + FLAGS_OFFSET, flags)
    end

    return result
end

local normal = capture(8, 0, 1, 8000)
normal:frames(10)
assert(not normal:has('entered one-player duel'))
normal:mode(3)
normal:frames(1)
normal:record(2, 123, FLAG_OCCUPIED)
normal:frames(1)
normal:record(2, 123, FLAG_OCCUPIED + FLAG_USED_THIS_TURN)
normal:frames(1)
normal:lp(7000)
normal:frames(1)
normal:frames(120)
assert(normal:has(
    'status: same-turn used-bit transition and opponent LP drop captured'
), 'the complete simulated evidence must end after stable quiet time')
assert(normal:has(
    'summary: watched_index=2 card_id=123 placement_used=false '
        .. 'used_transition=true opponent_lp_drop=true turn_changed=false'
), 'the successful summary must retain each independent observation')
assert(normal:has('used_transition frame='),
       'the used-bit transition must be recorded')
assert(normal:has('old=8000 new=7000'),
       'the opponent LP change must be recorded')

local twoPlayer = capture(3, 0, 0xff, 8000)
twoPlayer:frames(1)
assert(twoPlayer:has(
    'status: entered a two-player duel; use an ordinary CPU duel'
), 'a negative opponent id must reject the run')

local leftDuel = capture(3, 0, 1, 8000)
leftDuel:frames(1)
leftDuel:mode(8)
leftDuel:frames(1)
assert(leftDuel:has('status: left duel mode after a partial capture'),
       'leaving duel mode must stop a partial run')

local turnChanged = capture(3, 0, 1, 8000)
turnChanged:frames(1)
turnChanged:record(4, 456, FLAG_OCCUPIED)
turnChanged:frames(1)
turnChanged:side(1)
turnChanged:frames(1)
assert(turnChanged:has(
    'status: active side changed before complete direct-attack evidence'
), 'a turn change must stop an incomplete observation')
assert(turnChanged:has('turn_changed=true'),
       'the final summary must preserve the turn-change reason')

local removed = capture(3, 0, 1, 8000)
removed:frames(1)
removed:record(6, 321, FLAG_OCCUPIED)
removed:frames(1)
removed:record(6, 0, 0)
removed:frames(1)
assert(removed:has(
    'status: watched card left its field record before completion'
), 'a recycled or removed watched record cannot count as evidence')

local alreadyUsed = capture(3, 0, 1, 8000)
alreadyUsed:frames(1)
alreadyUsed:record(
    1, 222, FLAG_OCCUPIED + FLAG_USED_THIS_TURN
)
alreadyUsed:frames(1)
alreadyUsed:lp(7500)
alreadyUsed:frames(1)
alreadyUsed:frames(3599)
assert(alreadyUsed:has(
    'status: placement observation window completed without full evidence'
), 'an already-used placement must not invent a used-bit transition')
assert(alreadyUsed:has(
    'placement_used=true used_transition=false opponent_lp_drop=true'
), 'the incomplete evidence must remain explicit in the summary')

local silent = capture(8, 0, 1, 8000)
silent:frames(72000)
assert(silent:has(
    'status: timed out before one-player duel mode was observed'
), 'a no-hit run must explain the likely setup problem')

local limited = capture(3, 1, 1, 8000)
limited:frames(1)
for change = 1, 64 do
    limited:record(0, change, 0)
    limited:frames(1)
end
assert(limited:has('status: maximum player-record change count reached'),
       'the record-change capture must stop at its output bound')

print = hostPrint
print('duel_new_monster_attack: all eight polling cases passed')
