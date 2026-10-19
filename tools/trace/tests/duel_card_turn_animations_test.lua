-- Run from the repository root:
--   luajit tools/trace/tests/duel_card_turn_animations_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/duel_card_turn_animations.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local SCENE_STATE = 0x8009b23a
local TRANSITION = 0x8009b162
local CARD_RECORDS = 0x801a7ad8
local FLIP = 0x800229f4
local TURN_BACK = 0x80022674

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a0 = 0 } } }
    local events, breakpoints, output = {}, {}, {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u16at(a) return ffi.cast('uint16_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end
    u32at(FLIP)[0] = options.badWord or 0x27bdffe0
    u32at(TURN_BACK)[0] = 0x27bdffe0

    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = { enabled = true, callback = callback }
            function handle:disable() self.enabled = false end
            breakpoints[address] = handle
            return handle
        end,
        Events = {
            createEventListener = function(name, callback)
                events[name] = callback
                return {}
            end,
        },
    }
    breakpoints_duel_card_turn_animations = nil
    listener_duel_card_turn_animations = nil
    dofile(scriptPath)

    local r = {}
    function r:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function r:frames(n) for _ = 1, n do events['GPU::Vsync']() end end
    function r:mode(v) u8at(MAIN_MODE)[0] = v end
    function r:transition(v) u16at(TRANSITION)[0] = v end
    function r:scene(v) u16at(SCENE_STATE)[0] = v end
    function r:card(index, id, flags)
        local rec = CARD_RECORDS + index * 0x1c
        u16at(rec + 0x0c)[0] = id
        u16at(rec + 0x16)[0] = flags
    end
    function r:object(addr, index) u8at(addr + 0x6a)[0] = index end
    function r:setMode(addr, v) u16at(addr + 0x2e)[0] = v end
    -- One callback invocation, as the display list would make it.
    function r:call(addr, object)
        registers.GPR.n.a0 = object
        if breakpoints[addr].enabled then breakpoints[addr].callback() end
    end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do if bp.enabled then return false end end
        return true
    end
    function r:has2(a, b)
        for _, line in ipairs(output) do
            if line:find(a, 1, true) and line:find(b, 1, true) then return true end
        end
        return false
    end
    r.flags = function(index, v) u16at(CARD_RECORDS + index * 0x1c + 0x16)[0] = v end
    return r
end

-- The case the script exists for: the transition word changes, one card
-- runs the turn-back step for several frames at half rate, and its mode and
-- record flags before and after are reported in one episode row.
local normal = capture()
assert(normal:has('2 breakpoints installed, 0 signature mismatches'))
normal:mode(3)
normal:scene(5)
normal:frames(1)
normal:transition(3)
normal:frames(1)
normal:card(6, 0x2a, 0x9000)
normal:object(0x800f0000, 6)
for _ = 1, 5 do
    normal:call(TURN_BACK, 0x800f0000)
    normal:setMode(0x800f0000, 0)
    normal.flags(6, 0x9400)
    normal:frames(2)
end
normal:frames(10)
normal:mode(1)
normal:frames(1)
assert(normal:has('status: left the duel'))
assert(normal:has('transition frame=000002 0x0003 -> 0x0003') == false)
assert(normal:has('0x0000 -> 0x0003 scene=0x0005'), 'transition change logged')
assert(normal:has2('episode=01 turn-back func_80022674', 'frames=5'),
       'a half-rate callback must stay one episode')
assert(normal:has('card=42 side=0 flags_before=0x9000 flags_after=0x9400 mode=0'))
assert(normal:allDisabled())

-- Side 1 is records 15-29.
local opponent = capture()
opponent:mode(3)
opponent:frames(1)
opponent:card(20, 7, 0x8000)
opponent:object(0x800f0100, 20)
opponent:call(FLIP, 0x800f0100)
opponent:setMode(0x800f0100, 1)
opponent:frames(8)
opponent:mode(0)
opponent:frames(1)
assert(opponent:has('flip func_800229F4 object=0x800F0100 record=20 card=7 side=1'))
assert(opponent:has('mode=1'))

-- Guards: none of these may be dereferenced.
local bad = capture()
bad:mode(3)
bad:frames(1)
bad:call(FLIP, 0)
bad:call(FLIP, 0x800f0002)
bad:call(FLIP, 0x801ffff0)
bad:object(0x800f0200, 0xff)
bad:call(FLIP, 0x800f0200)
bad:frames(8)
bad:mode(0)
bad:frames(1)
assert(bad:has('IMPLAUSIBLE flip func_800229F4 object=0x00000000'))
assert(bad:has('IMPLAUSIBLE flip func_800229F4 object=0x800F0002'))
assert(bad:has('IMPLAUSIBLE flip func_800229F4 object=0x801FFFF0'))
assert(bad:has('record=255 card=unreadable side=1 flags_before=unreadable'),
       'an out-of-table record index must not be read')

-- A wrong retail word is reported.
local corrupt = capture({ badWord = 0 })
assert(corrupt:has('1 signature mismatches'))

-- Bounded output.
local flood = capture()
flood:mode(3)
for i = 1, 70 do
    flood:transition(i)
    flood:frames(1)
end
assert(flood:has('status: captured 64 rows'))

-- Silence in a duel names the likely setup mistake.
local silent = capture()
silent:mode(3)
silent:frames(36000)
assert(silent:has('status: no callback hit in ten minutes of duel'))

print = hostPrint
print('duel_card_turn_animations: all six callback-replay cases passed')
