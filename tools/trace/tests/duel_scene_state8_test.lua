-- Run from the repository root:
--   luajit tools/trace/tests/duel_scene_state8_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/duel_scene_state8.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local SCENE = 0x8009b23a
local SUBSTATE = 0x8009b174
local AMOUNT = 0x8009b154
local OBJECT_SLOT = 0x800e9ef0
local TEXT_BOX = 0x800eb0f8
local CARD_RECORDS = 0x801a7ad8

local function capture()
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local events, output = {}, {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u16at(a) return ffi.cast('uint16_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end

    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        getMemPtr = function() return memory end,
        Events = {
            createEventListener = function(name, callback)
                events[name] = callback
                return {}
            end,
        },
    }
    listener_duel_scene_state8 = nil
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
    function r:scene(v) u16at(SCENE)[0] = v; r:frames(1) end
    function r:sub(v) u8at(SUBSTATE)[0] = v; r:frames(1) end
    function r:amount(v) u16at(AMOUNT)[0] = v % 0x10000 end
    function r:text(v) u16at(TEXT_BOX + 0x36)[0] = v end
    function r:object(addr) u32at(OBJECT_SLOT)[0] = addr end
    function r:placed(object, index, card, flags, modifier)
        u8at(object + 0x68)[0] = 0x05
        u8at(object + 0x6a)[0] = index
        local rec = CARD_RECORDS + index * 0x1c
        u16at(rec + 0x0c)[0] = card
        u16at(rec + 0x16)[0] = flags
        u16at(rec + 0x12)[0] = modifier % 0x10000
    end
    return r
end

-- Normal: an equip episode with a Reverse Trap answer, read as phases.
local normal = capture()
assert(normal:has('waiting for duel scene state 8'))
normal:mode(3)
normal:scene(4)
normal:scene(7)
normal:object(0x800f0000)
normal:placed(0x800f0000, 6, 42, 0x8000, 500)
normal:amount(500)
normal:scene(0x4008)
normal:sub(1)
normal:sub(3)
normal:text(0x021)
normal:sub(0x83)
normal:sub(5)
normal:sub(6)
normal:placed(0x800f0000, 6, 42, 0x8000, -500)
normal:scene(5)
normal:mode(1)
normal:frames(1)
assert(normal:has('status: left the duel'))
assert(normal:has('=== episode 1 enters state 8 ==='))
assert(normal:has('before: scene 0x0004 -> 0x0007'))
assert(normal:has('scene 0x0007 -> 0x4008 sub=0x00 amount=500'))
assert(normal:has('sub=0x03 amount=500 deck_record=0 f218=0 c210=0 text=0x000'))
assert(normal:has('sub=0x83 amount=500 deck_record=0 f218=0 c210=0 text=0x021'))
assert(normal:has('object=0x800F0000 type68=0x05 record=6 card=42 '
                  .. 'flags=0x8000 stat_modifier=500'))
assert(normal:has('leaves to scene 0x0005'))
assert(normal:has('stat_modifier=-500'))

-- Guards: no placed object, a misaligned one, an out-of-table record.
local bad = capture()
bad:mode(3)
bad:frames(1)
bad:object(0)
bad:scene(8)
bad:object(0x800f0002)
bad:scene(5)
bad:object(0x800f0100)
bad:placed(0x800f0100, 0xff, 1, 0, 0)
bad:scene(8)
bad:scene(5)
bad:mode(0)
bad:frames(1)
assert(bad:has('object=0x00000000'))
assert(bad:has('object=0x800F0002'))
assert(bad:has('type68=0x05 record=255'))
assert(not bad:has('record=255 card='), 'an out-of-table record is not read')

-- Leaving a duel that never reached state 8 says so.
local none = capture()
none:mode(3)
none:scene(5)
none:mode(0)
none:frames(1)
assert(none:has('status: left the duel without ever entering state 8'))

-- Bounded by episodes.
local many = capture()
many:mode(3)
many:frames(1)
for _ = 1, 6 do
    many:scene(8)
    many:scene(5)
end
assert(many:has('status: captured 6 episodes'))

-- Bounded by rows inside one long episode.
local long = capture()
long:mode(3)
long:scene(8)
for i = 1, 80 do long:sub(i) end
assert(long:has('status: captured 64 rows'))

print = hostPrint
print('duel_scene_state8: all five callback-replay cases passed')
