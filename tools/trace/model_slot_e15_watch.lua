-- model_slot_e15_watch.lua
--
-- WHAT THIS ANSWERS
--   What ModelSlot.field_E15 (gModel_aSlots[n] + 0xE15, North America) means.
--   The 3-D battle controllers in src/game/model.c clear it before polling a
--   slot's variant effect handler, increment it once the handler reports 2,
--   and set it to 0xFF in one place until the defender reaches animation
--   row 8; the credits controller reads it as a tick count. Static code alone
--   cannot say whether it is one "effect finished" latch, a per-phase counter
--   or something else, so notes/credits-runtime-protocol.md leaves it
--   unnamed. This trace logs every change to it beside the presentation
--   phase and each slot's command row, so the values can be matched to what
--   the battle showed.
--
--   It only polls memory once per frame; it installs no breakpoints, so the
--   debugger and CPU settings do not matter.
--
-- WHAT YOU DO (about three minutes)
--   1. PCSX-Redux with the game (North American SLUS_014.11).
--   2. Start a duel where 3-D battles are on (Free Duel is fine).
--   3. Debug -> Lua editor, paste this file. It starts watching at once and
--      stops by itself after three minutes.
--   4. Play one attack that destroys a monster and, if you can, one fusion
--      or one attack that is blocked. Note roughly when each 3-D scene
--      started and ended; every change is printed at once with a time stamp.
--   The trace prints by itself. Copy the whole document into
--   tools/trace/result/model_slot_e15_watch.txt.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Which battles you played, which monster attacked which, and the rough
--   times (for example "t=40s Blue-Eyes attacked face-down monster, it was
--   destroyed"). One or two lines is enough.

local ffi = require('ffi')

local SCRIPT_NAME = 'model_slot_e15_watch'

local RAM_BASE = 0x80000000
local RAM_END = 0x80200000

-- gMain_bActiveModeState; its low five bits select the main mode.
local MAIN_MODE = 0x8009b26c
local PRESENTATION_PHASE = 0x8009af9a
local SLOTS = 0x800f2c40
local SLOT_SIZE = 0xe20
local SLOT_COUNT = 2
local FIELD_E15 = 0xe15
local COMMAND_ROW = 0xe0f
local IS_ACTIVE = 0xe1f

local WINDOW_FRAMES = 10800
local MAX_ROWS = 200

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function show(v)
    if v == nil then return '??' end
    return string.format('%02X', v)
end

-- One snapshot of every watched byte, as a printable string per key.
local function snapshot()
    local s = {
        mode = (function()
            local v = u8(MAIN_MODE)
            return v and show(v % 32) or '??'
        end)(),
        phase = show(u8(PRESENTATION_PHASE)),
    }
    for n = 0, SLOT_COUNT - 1 do
        local base = SLOTS + n * SLOT_SIZE
        s['e15_' .. n] = show(u8(base + FIELD_E15))
        s['row_' .. n] = show(u8(base + COMMAND_ROW))
        s['active_' .. n] = show(u8(base + IS_ACTIVE))
    end
    return s
end

local KEYS = {'mode', 'phase', 'e15_0', 'e15_1', 'row_0', 'row_1',
              'active_0', 'active_1'}

local frames = 0
local rows = {}
local dropped = 0
local previous = nil
local e15Changes = 0
local done = false

local function line(s, changed)
    return string.format('t=%6.1fs mode=%s phase=%s | slot0 e15=%s row=%s '
        .. 'active=%s | slot1 e15=%s row=%s active=%s | changed: %s',
        frames / 60, s.mode, s.phase, s.e15_0, s.row_0, s.active_0,
        s.e15_1, s.row_1, s.active_1, changed)
end

local function record(text)
    if #rows >= MAX_ROWS then
        dropped = dropped + 1
        return
    end
    rows[#rows + 1] = text
    pcall(print, SCRIPT_NAME .. ': ' .. text)
end

local function finish(reason)
    if done then return end
    done = true
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<which battles you played and roughly when, for example')
    print(' "t=40s Blue-Eyes attacked a face-down monster, it was destroyed">')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format('frames: %d rows: %d dropped: %d e15_changes: %d',
                        frames, #rows, dropped, e15Changes))
    if e15Changes == 0 then
        print('note: field_E15 never changed; no 3-D battle ran in the window, '
              .. 'or the game is not the North American build')
    end
    print('')
    for _, text in ipairs(rows) do print(text) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function poll()
    if done then return end
    local s = snapshot()
    if previous == nil then
        record(line(s, 'start'))
    else
        local changed = {}
        for _, k in ipairs(KEYS) do
            if s[k] ~= previous[k] then
                changed[#changed + 1] = k
                if k == 'e15_0' or k == 'e15_1' then
                    e15Changes = e15Changes + 1
                end
            end
        end
        if #changed > 0 then record(line(s, table.concat(changed, ','))) end
    end
    previous = s
    frames = frames + 1
    if frames >= WINDOW_FRAMES then finish('window ended') end
end

listener_model_slot_e15_watch = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: watching now; stopping by itself in %d seconds',
                    SCRIPT_NAME, WINDOW_FRAMES / 60))
