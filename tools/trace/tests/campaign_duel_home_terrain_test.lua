-- Run from the repository root:
--   luajit tools/trace/tests/campaign_duel_home_terrain_test.lua
-- Replays the fixed-address polling callbacks with mock PCSX events and a
-- real LuaJIT FFI memory buffer.
local ffi = require('ffi')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/campaign_duel_home_terrain.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local CAMPAIGN_SCENE = 0x8009b27a
local OPPONENT_ID = 0x8009b361
local TERRAIN = 0x8009b364
local DUEL_BGM = 0x8009b36a
local FOLLOWUP_A = 0x8009b370
local FOLLOWUP_B = 0x8009b372
local VALUE_374 = 0x8009b374

local function capture(initialMode)
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local events = {}
    local output = {}

    local function set8(addr, value)
        ffi.cast('uint8_t*', memory + (addr - RAM_BASE))[0] =
            value % 0x100
    end

    local function set16(addr, value)
        set8(addr, value)
        set8(addr + 1, math.floor(value / 0x100))
    end

    set8(MAIN_MODE, initialMode or 0)

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
    listener_campaign_duel_home_terrain_reset = nil
    listener_campaign_duel_home_terrain = nil
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

    function result:duel(opponent, terrain, scene, followupA, followupB)
        set8(OPPONENT_ID, opponent)
        set8(TERRAIN, terrain)
        set8(CAMPAIGN_SCENE, scene or 0)
        set16(DUEL_BGM, 0x7270)
        set16(FOLLOWUP_A, followupA or 0)
        set16(FOLLOWUP_B, followupB or 0)
        set16(VALUE_374, 0x7280)
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

local normal = capture(2)
normal:duel(21, 5, 0x44, 0x0051, 0x0052)
normal:mode(3)
normal:frames(1)
normal:frames(59)
assert(not normal:has('status: captured target duel through sixty stable frames'),
       'capture must remain active before the full stability window')
normal:frames(1)
assert(normal:has('status: captured target duel through sixty stable frames'))
assert(normal:has('opponent_id=21 terrain=5(Umi)'))
assert(normal:has('scene_index=0x44'))
assert(normal:has('D_8009B370=0x0051 D_8009B372=0x0052'))
assert(normal:has('changed_after_entry=false'))

local skipped = capture(2)
skipped:duel(1, 0, 0x20, 0x21, 0x22)
skipped:mode(3)
skipped:frames(1)
skipped:mode(2)
skipped:frames(1)
skipped:duel(35, 6, 0x60, 0x61, 0x62)
skipped:mode(3)
skipped:frames(61)
assert(skipped:has('skipped_campaign_duels=1'))
assert(skipped:has('opponent_id=35 terrain=6(Yami)'))

local freeDuel = capture(6)
freeDuel:duel(21, 5, 0x44, 0x51, 0x52)
freeDuel:mode(3)
freeDuel:frames(1)
freeDuel:mode(2)
freeDuel:frames(1)
freeDuel:mode(3)
freeDuel:frames(61)
assert(freeDuel:has('ignored_non_campaign_duels=1'))
assert(freeDuel:has('target_found=true'))

local unknownTerrain = capture(2)
unknownTerrain:duel(36, 9, 0x70, 0x71, 0x72)
unknownTerrain:mode(3)
unknownTerrain:frames(61)
assert(unknownTerrain:has('terrain=9(unknown(9))'))

local changed = capture(2)
changed:duel(37, 6, 0x80, 0x81, 0x82)
changed:mode(3)
changed:frames(1)
changed:duel(37, 5, 0x80, 0x81, 0x82)
changed:frames(60)
assert(changed:has(
    'status: captured target duel; values changed during stability window'
))
assert(changed:has('changed_after_entry=true'))
assert(changed:has('first_change'))

local reset = capture(2)
reset:reset()
assert(reset:has(
    'status: reset observed; rerun immediately before the target duel'
))
assert(reset:has('target_found=false'))

local silent = capture(2)
silent:frames(36000)
assert(silent:has(
    'status: timed out before a target campaign duel; arm in mode 2'
))

print = hostPrint
print('campaign_duel_home_terrain: all seven polling cases passed')
