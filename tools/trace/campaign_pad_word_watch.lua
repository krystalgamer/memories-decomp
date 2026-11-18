-- campaign_pad_word_watch.lua
--
-- WHAT THIS ANSWERS
--   What the resident halfword D_800C4E68 (North America) holds. The campaign
--   map's alternate-location code tests it with controller button masks:
--   0x20 cancels and 0xC0 confirms in North America, while Japan uses 0x40
--   and 0xA0, the regions' swapped Circle and Cross roles, and the tick tests
--   0x4, 0x100 and 0x800 (L1, Select and Start). That makes it a pad word,
--   but static code cannot say whether it is the pressed edges, the held
--   buttons or the auto-repeat word, or a copy filtered by the map. This
--   trace compares it every frame with gInput_awPadPressed[0],
--   gInput_awPadHeld[0] and gInput_awPadRepeat[0] and counts which one it
--   equals, and logs D_800C4E74 beside it.
--
--   It only polls memory once per frame; it installs no breakpoints, so the
--   debugger and CPU settings do not matter.
--
-- WHAT YOU DO (about two minutes)
--   1. PCSX-Redux with the game (North American SLUS_014.11).
--   2. Load a campaign save and stand on the campaign map.
--   3. Debug -> Lua editor, paste this file. It starts watching at once and
--      stops by itself after two minutes.
--   4. Move the cursor around, hold a direction for a few seconds, tap
--      Cross, Circle, Square, Triangle, L1 and Start, and open and close any
--      location menu once.
--   Every change is printed at once with a time stamp. The trace prints by
--   itself; copy the whole document into
--   tools/trace/result/campaign_pad_word_watch.txt.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Where you were (for example "campaign map before the coup") and what
--   you pressed or held, with rough times. One or two lines is enough.

local ffi = require('ffi')

local SCRIPT_NAME = 'campaign_pad_word_watch'

local RAM_BASE = 0x80000000
local RAM_END = 0x80200000

-- gMain_bActiveModeState; its low five bits select the main mode.
local MAIN_MODE = 0x8009b26c
local PAD_REPEAT = 0x8009b394
local PAD_PRESSED = 0x8009b398
local PAD_HELD = 0x8009b3a4
local WORD = 0x800c4e68
local NEIGHBOUR = 0x800c4e74

local WINDOW_FRAMES = 7200
local MAX_ROWS = 200

local mem = PCSX.getMemPtr()

local function u8(addr)
    if addr < RAM_BASE or addr + 1 > RAM_END then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function u16(addr)
    if addr < RAM_BASE or addr + 2 > RAM_END or addr % 2 ~= 0 then
        return nil
    end
    return tonumber(ffi.cast('uint16_t*', mem + (addr - RAM_BASE))[0])
end
local function hex(v)
    if v == nil then return '????' end
    return string.format('%04X', v)
end

local frames = 0
local rows = {}
local dropped = 0
local previous = nil
local done = false
-- Frames on which the word was nonzero, and on which it equalled each pad
-- word.
local nonzero = 0
local equal = {pressed = 0, held = 0, repeat_ = 0}
local mismatches = 0

local function line(s, changed)
    return string.format('t=%6.1fs mode=%s D_800C4E68=%s pressed=%s held=%s '
        .. 'repeat=%s D_800C4E74=%s | changed: %s', frames / 60, s.mode,
        hex(s.word), hex(s.pressed), hex(s.held), hex(s.repeat_),
        hex(s.neighbour), changed)
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
    print('<where you were and what you pressed or held, for example')
    print(' "campaign map before the coup, held Right t=10-15s, tapped Cross">')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format('frames: %d rows: %d dropped: %d', frames, #rows,
                        dropped))
    print(string.format('nonzero frames: %d equal to pressed: %d held: %d '
        .. 'repeat: %d none of them: %d', nonzero, equal.pressed, equal.held,
        equal.repeat_, mismatches))
    if nonzero == 0 then
        print('note: D_800C4E68 stayed zero; it may only update on the '
              .. 'campaign map, or the game is not the North American build')
    end
    print('')
    for _, text in ipairs(rows) do print(text) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local KEYS = {'mode', 'word', 'pressed', 'held', 'repeat_', 'neighbour'}

local function poll()
    if done then return end
    local m = u8(MAIN_MODE)
    local s = {
        mode = m and string.format('%02X', m % 32) or '??',
        word = u16(WORD),
        pressed = u16(PAD_PRESSED),
        held = u16(PAD_HELD),
        repeat_ = u16(PAD_REPEAT),
        neighbour = u16(NEIGHBOUR),
    }
    if s.word ~= nil and s.word ~= 0 then
        nonzero = nonzero + 1
        local any = false
        for _, k in ipairs({'pressed', 'held', 'repeat_'}) do
            if s[k] == s.word then
                equal[k] = equal[k] + 1
                any = true
            end
        end
        if not any then mismatches = mismatches + 1 end
    end
    if previous == nil then
        record(line(s, 'start'))
    else
        local changed = {}
        for _, k in ipairs(KEYS) do
            if s[k] ~= previous[k] then changed[#changed + 1] = k end
        end
        if #changed > 0 then record(line(s, table.concat(changed, ','))) end
    end
    previous = s
    frames = frames + 1
    if frames >= WINDOW_FRAMES then finish('window ended') end
end

listener_campaign_pad_word_watch = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: watching now; stopping by itself in %d seconds',
                    SCRIPT_NAME, WINDOW_FRAMES / 60))
