-- Run from the repository root:
--   luajit tools/trace/tests/duel_display_callbacks_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/duel_display_callbacks.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local TRANSITION = 0x8009b162
local CARD_RECORDS = 0x801a7ad8
local EASE, EASE_TINT, MARKER = 0x8001ec70, 0x8001ed20, 0x80019bd0
local FLIP, TURN_BACK, FLY, ORBIT = 0x800229f4, 0x80022674, 0x80020ee8, 0x80020d4c
local SPRITE, CONTROL = 0x80018150, 0x80024200
local WORDS = {
    [EASE] = 0x27bdffe8, [EASE_TINT] = 0x27bdffe8, [MARKER] = 0x27bdffe8,
    [FLIP] = 0x27bdffe0, [TURN_BACK] = 0x27bdffe0, [FLY] = 0x27bdffe8,
    [ORBIT] = 0x27bdffc8, [SPRITE] = 0x27bdffd0, [CONTROL] = 0x9782025a,
}

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a0 = 0, a1 = 0, ra = 0 } } }
    local events, breakpoints, output = {}, {}, {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u16at(a) return ffi.cast('uint16_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end
    for addr, word in pairs(WORDS) do
        u32at(addr)[0] = options.corrupt == addr and 0 or word
    end

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
    breakpoints_duel_display_callbacks = nil
    listener_duel_display_callbacks = nil
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
    function r:object(addr, fields)
        for offset, value in pairs(fields.u8 or {}) do u8at(addr + offset)[0] = value end
        for offset, value in pairs(fields.u16 or {}) do
            u16at(addr + offset)[0] = value % 0x10000
        end
    end
    function r:card(index, id, flags)
        local rec = CARD_RECORDS + index * 0x1c
        u16at(rec + 0x0c)[0] = id
        u16at(rec + 0x16)[0] = flags
    end
    function r:cardFlags(index, flags) u16at(CARD_RECORDS + index * 0x1c + 0x16)[0] = flags end
    function r:call(addr, a0, a1, ra)
        local n = registers.GPR.n
        n.a0, n.a1, n.ra = a0, a1 or 0, ra or 0
        if breakpoints[addr].enabled then breakpoints[addr].callback() end
    end
    function r:count() local n = 0 for _ in pairs(breakpoints) do n = n + 1 end return n end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do if bp.enabled then return false end end
        return true
    end
    return r
end

-- The case the script exists for: a sprite on transition bit 0x02, a card
-- sliding with ease+tint, the placement marker clearing its card flag, and
-- the confetti fly-off after the orbit, all in one duel.
local duel = capture()
assert(duel:count() == 9, 'nine breakpoints')
assert(duel:has('9 breakpoints installed, 0 signature mismatches'))
duel:mode(3)
duel:frames(1)
duel:call(CONTROL, 0)
assert(duel:has('execution breakpoint observed; setup confirmed'))
duel:transition(2)
duel:frames(1)
duel:call(SPRITE, 0, 0xf0, 0x80023700)
duel:card(6, 42, 0xa000)
duel:object(0x800f0000, { u8 = { [0x6a] = 6, [0x22] = 0x10 },
    u16 = { [0x30] = 10, [0x32] = 20, [0x28] = 100, [0x2a] = 140, [0x2c] = 8 } })
for _ = 1, 4 do
    duel:call(EASE_TINT, 0x800f0000)
    duel:frames(2)
end
duel:object(0x800f0000, { u8 = { [0x22] = 0 }, u16 = { [0x30] = 100, [0x32] = 140 } })
duel:frames(6)
duel:object(0x800f0100, { u8 = { [0x6a] = 6 } })
duel:call(MARKER, 0x800f0100)
duel:cardFlags(6, 0x8000)
duel:frames(6)
duel:object(0x800f0200, { u8 = { [0x6a] = 0xff, [0x21] = 0xff } })
duel:call(ORBIT, 0x800f0200)
duel:frames(6)
duel:call(FLY, 0x800f0200)
duel:frames(6)
duel:mode(1)
duel:frames(1)
assert(duel:has('status: left the duel'))
assert(duel:has('setup: unknown: PCSX.settings unavailable'))
assert(duel:has('execution_breakpoint=observed'))
assert(duel:has('transition frame=000002 0x0000 -> 0x0002'))
assert(duel:has('sprite frame=000002 sprite Duel_CreateFieldCardPanel x=0 y=240 '
                .. 'caller=0x80023700'))
assert(duel:has('ease+tint func_8001ED20 object=0x800F0000 record=6 side=0 '
                .. 'card=42 calls=4 start=(10,20) target=(100,140) '
                .. 'frames_field=8 end=(100,140)'))
assert(duel:has('tint=00,00,10,00->00,00,00,00'))
assert(duel:has('marker func_80019BD0 object=0x800F0100 record=6'))
assert(duel:has('card_flags=0xA000->0x8000'),
       'the marker episode must show its card flag being cleared')
assert(duel:has('orbit DuelResult_UpdateOrbitSprite object=0x800F0200 '
                .. 'record=255 (not a card record)'))
assert(duel:has('fly-off func_80020EE8 object=0x800F0200'))
assert(duel:has('control_hits: 1'))
assert(duel:allDisabled())

-- Flip/turn-back: the mode chosen inside the first call is read afterwards.
local turn = capture()
turn:mode(3)
turn:frames(1)
turn:object(0x800f0300, { u8 = { [0x6a] = 20 } })
turn:call(TURN_BACK, 0x800f0300)
turn:object(0x800f0300, { u16 = { [0x2e] = 2 } })
turn:frames(8)
turn:mode(0)
turn:frames(1)
assert(turn:has('turn-back func_80022674 object=0x800F0300 record=20 side=1'))
assert(turn:has('mode2E=2'))

-- Guards: bad objects are never read.
local bad = capture()
bad:mode(3)
bad:frames(1)
bad:call(EASE, 0)
bad:call(EASE, 0x800f0002)
bad:call(EASE, 0x801ffff0)
bad:mode(0)
bad:frames(1)
assert(bad:has('IMPLAUSIBLE ease func_8001EC70 object=0x00000000'))
assert(bad:has('IMPLAUSIBLE ease func_8001EC70 object=0x800F0002'))
assert(bad:has('IMPLAUSIBLE ease func_8001EC70 object=0x801FFFF0'))

-- Per-callback row caps hold inside the callback, and hits keep counting.
local busy = capture()
busy:mode(3)
busy:frames(1)
for i = 1, 30 do busy:call(EASE, 0x800f0000 + i * 0x80) end
busy:frames(6)
busy:mode(0)
busy:frames(1)
assert(busy:has('episode=16 ease'))
assert(not busy:has('episode=17 '), 'ease episodes are capped at 16')
assert(busy:has('ease func_8001EC70                   30'))

-- Bounded output overall.
local flood = capture()
flood:mode(3)
for i = 1, 120 do
    flood:transition(i)
    flood:frames(1)
end
assert(flood:has('status: captured 96 rows'))
assert(flood:has('rows: 96'))

-- A corrupted entry word is reported.
local corrupt = capture({ corrupt = MARKER })
assert(corrupt:has('1 signature mismatches'))

-- Ten minutes of duel without even the control is a setup failure.
local silent = capture()
silent:mode(3)
silent:frames(36000)
assert(silent:has('status: ten minutes in a duel without a control hit'))
assert(silent:has('execution_breakpoint=NOT-observed'))

print = hostPrint
print('duel_display_callbacks: all seven callback-replay cases passed')
