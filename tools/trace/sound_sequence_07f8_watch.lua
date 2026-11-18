-- sound_sequence_07f8_watch.lua
--
-- WHAT THIS ANSWERS
--   What SDSecondaryState.sequence.field_07F8 (gSD_pSecondaryState + 0x7F8,
--   North America) means. The sequence player compares it with the track
--   index to decide which track's tempo advances sequence_tick_count
--   (src/game/sound_sequence_tracks.c), and the loop-start and loop-end
--   controllers only save and restore the track state while it is nonzero
--   (src/game/sound_sequence_parser.c). The open path clears it and no
--   matched C sets it, so it is either written by code that is still
--   assembly or always zero. Static code cannot tell a "tick track" from a
--   "loops enabled" flag; this trace logs its value for every sequence the
--   game opens, beside the open token, the track count and the timebase.
--
--   It only polls memory once per frame; it installs no breakpoints, so the
--   debugger and CPU settings do not matter.
--
-- WHAT YOU DO (about three minutes)
--   1. PCSX-Redux with the game (North American SLUS_014.11).
--   2. Debug -> Lua editor, paste this file at the title screen. It starts
--      watching at once and stops by itself after three minutes.
--   3. Go from the title screen to the main menu, open Free Duel, start a
--      duel and play a turn, so several pieces of music start. Let at least
--      one piece play long enough to loop back to its start if you can.
--   Every change is printed at once with a time stamp. The trace prints by
--   itself; copy the whole document into
--   tools/trace/result/sound_sequence_07f8_watch.txt.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Which screens you visited and roughly when the music changed or looped
--   (for example "t=50s duel music started, t=150s it looped"). One or two
--   lines is enough.

local ffi = require('ffi')

local SCRIPT_NAME = 'sound_sequence_07f8_watch'

local RAM_BASE = 0x80000000
local RAM_END = 0x80200000

-- gMain_bActiveModeState; its low five bits select the main mode.
local MAIN_MODE = 0x8009b26c
local STATE_POINTER = 0x8009b458
local STATE_SIZE = 0x848
local OPEN_TOKEN = 0x7e0
local SEQUENCE_STATE = 0x7e2
local SOURCE = 0x7e8
local TICK_COUNT = 0x7f0
local FIELD_07F8 = 0x7f8
local TRACK_COUNT = 0x7fa
local TIMEBASE = 0x7fc
local TRACKS = 0x518
local TRACK_SIZE = 0x2c
local TRACK_ENDED = 0x24
local TRACK_LIMIT = 16

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
local function u16(addr)
    if not inRam(addr, 2) or addr % 2 ~= 0 then return nil end
    return tonumber(ffi.cast('uint16_t*', mem + (addr - RAM_BASE))[0])
end
local function u32(addr)
    if not inRam(addr, 4) or addr % 4 ~= 0 then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local function hex(v, digits)
    if v == nil then return string.rep('?', digits) end
    return string.format('%0' .. digits .. 'X', v)
end

-- The secondary state pointer, or nil with a reason when it cannot be read.
local function statePointer()
    local p = u32(STATE_POINTER)
    if p == nil then return nil, 'pointer unreadable' end
    if p == 0 then return nil, 'pointer null' end
    if p % 4 ~= 0 or not inRam(p, STATE_SIZE) then
        return nil, string.format('pointer 0x%08X outside RAM', p)
    end
    return p
end

local function snapshot()
    local m = u8(MAIN_MODE)
    local s = {mode = m and hex(m % 32, 2) or '??'}
    local p, why = statePointer()
    if p == nil then
        s.state = why
        return s
    end
    s.state = string.format('0x%08X', p)
    s.f7f8 = hex(u16(p + FIELD_07F8), 4)
    s.tracks = hex(u16(p + TRACK_COUNT), 4)
    s.timebase = hex(u16(p + TIMEBASE), 4)
    s.token = hex(u16(p + OPEN_TOKEN), 4)
    s.seqstate = hex(u16(p + SEQUENCE_STATE), 4)
    s.source = hex(u32(p + SOURCE), 8)
    local count = u16(p + TRACK_COUNT) or 0
    if count > TRACK_LIMIT then count = TRACK_LIMIT end
    local live = 0
    for i = 0, count - 1 do
        if u8(p + TRACKS + i * TRACK_SIZE + TRACK_ENDED) == 0 then
            live = live + 1
        end
    end
    s.live = tostring(live)
    s.ticks = tostring(u32(p + TICK_COUNT) or '?')
    return s
end

-- Changes to these keys make a row; the tick count only rides along.
local KEYS = {'mode', 'state', 'f7f8', 'tracks', 'timebase', 'token',
              'seqstate', 'source', 'live'}

local frames = 0
local rows = {}
local dropped = 0
local previous = nil
local seen07f8 = {}
local done = false

local function line(s, changed)
    if s.f7f8 == nil then
        return string.format('t=%6.1fs mode=%s state=%s | changed: %s',
                             frames / 60, s.mode, s.state, changed)
    end
    return string.format('t=%6.1fs mode=%s field_07F8=%s tracks=%s live=%s '
        .. 'timebase=%s token=%s seq_state=%s source=%s ticks=%s | changed: %s',
        frames / 60, s.mode, s.f7f8, s.tracks, s.live, s.timebase, s.token,
        s.seqstate, s.source, s.ticks, changed)
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
    local values = {}
    for v in pairs(seen07f8) do values[#values + 1] = v end
    table.sort(values)
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<which screens you visited and roughly when the music changed or')
    print(' looped, for example "t=50s duel music started, t=150s it looped">')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format('frames: %d rows: %d dropped: %d', frames, #rows,
                        dropped))
    print('field_07F8 values seen: ' .. (#values > 0
        and table.concat(values, ', ') or 'none (state pointer never valid)'))
    print('')
    for _, text in ipairs(rows) do print(text) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function poll()
    if done then return end
    local s = snapshot()
    if s.f7f8 ~= nil then seen07f8[s.f7f8] = true end
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

listener_sound_sequence_07f8_watch = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: watching now; stopping by itself in %d seconds',
                    SCRIPT_NAME, WINDOW_FRAMES / 60))
