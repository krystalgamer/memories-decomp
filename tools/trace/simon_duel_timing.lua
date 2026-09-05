-- simon_duel_timing.lua
--
-- WHAT THIS ANSWERS
--   Guides disagree on whether Simon Muran's optional palace duel is offered
--   before or only after the festival. Story flag 0x6F is set when Seto
--   challenges the player at the festival, so its value when opponent 1
--   enters duel mode distinguishes the two possibilities.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game.
--   2. Debug -> Lua editor, paste this file, and let it auto-run.
--   3. Start from a new-game save before the festival.
--   4. Visit Simon at the Pharaoh's Palace whenever the campaign allows it.
--      If Duel is offered, choose it. If it is not, continue through the
--      festival and revisit the palace.
--   5. Copy the whole document into
--      tools/trace/result/simon_duel_timing.txt and fill in the context.
--
--   No breakpoint, debugger pause, or interpreter CPU is required.
--
-- WHAT TO WRITE IN THE CONTEXT
--   State where the script was started, every palace visit and choice shown,
--   whether the festival had happened, and the exact visit that entered the
--   Simon duel.

local ffi = require('ffi')

local SCRIPT_NAME = 'simon_duel_timing'
local MAIN_MODE = 0x8009b26c
local CAMPAIGN_MODE = 2
local DUEL_MODE = 3
local CAMPAIGN_MAP_MODE = 5
local GAME_OVER_MODE = 12
local OPPONENT_ID = 0x8009b361
local SCRIPT_STREAM = 0x8009b290
local SCRIPT_STATE = 0x8009b2a4
local SCRIPT_COMMAND = 0x8009b27c
local STORY_FLAGS = 0x801d0618
local SIMON_ID = 1
local EVENING_LECTURE_FLAG = 0x6e
local FESTIVAL_CHALLENGE_FLAG = 0x6f
local MAX_SAMPLES = 64
local TIMEOUT_FRAMES = 216000

local mem = PCSX.getMemPtr()

local function phys(addr) return addr - 0x80000000 end
local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end
local function u16(addr)
    return tonumber(ffi.cast('uint16_t*', mem + phys(addr))[0])
end
local function u32(addr)
    return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0])
end
local function s8(addr)
    local value = u8(addr)
    if value >= 0x80 then
        return value - 0x100
    end
    return value
end
local function storyFlag(id)
    local byte = u8(STORY_FLAGS + math.floor(id / 8))
    local mask = math.floor(0x80 / (2 ^ (id % 8)))
    return math.floor(byte / mask) % 2
end

local lines = {}
local samples = 0
local frames = 0
local lastKey = nil
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function modeName(mode)
    if mode == CAMPAIGN_MODE then
        return 'Campaign dialogue'
    end
    if mode == DUEL_MODE then
        return 'Duel'
    end
    if mode == CAMPAIGN_MAP_MODE then
        return 'Campaign map'
    end
    if mode == GAME_OVER_MODE then
        return 'Game over'
    end
    return 'other'
end

local function capture(reason)
    if samples >= MAX_SAMPLES then
        return
    end

    samples = samples + 1
    local mode = u8(MAIN_MODE) % 32
    emit('')
    emit(string.format(
        '--- sample %d of %d: %s ---',
        samples, MAX_SAMPLES, reason
    ))
    emit(string.format(
        '  screen=%s mode=0x%02X opponent_id=%d',
        modeName(mode), u8(MAIN_MODE), s8(OPPONENT_ID)
    ))
    emit(string.format(
        '  evening_flag_006E=%d festival_flag_006F=%d',
        storyFlag(EVENING_LECTURE_FLAG),
        storyFlag(FESTIVAL_CHALLENGE_FLAG)
    ))
    emit(string.format(
        '  script_state=0x%04X script_command=0x%04X script_stream=0x%08X',
        u16(SCRIPT_STATE), u16(SCRIPT_COMMAND), u32(SCRIPT_STREAM)
    ))
end

local function finish(reason)
    if done then
        return
    end
    done = true

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<state the start point, each palace visit and its choices,')
    print(' whether the festival occurred, and which visit entered the duel>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function key(mode)
    return string.format(
        '%d,%d,%d,%d',
        mode,
        s8(OPPONENT_ID),
        storyFlag(EVENING_LECTURE_FLAG),
        storyFlag(FESTIVAL_CHALLENGE_FLAG)
    )
end

local function poll()
    if done then
        return
    end

    frames = frames + 1
    local mode = u8(MAIN_MODE) % 32
    local opponent = s8(OPPONENT_ID)

    if mode == DUEL_MODE and opponent == SIMON_ID then
        capture('entered Simon Muran duel')
        finish('captured Simon duel timing')
        return
    end

    local currentKey = key(mode)
    if currentKey ~= lastKey then
        lastKey = currentKey
        capture('mode, opponent, or timing flag changed')
        if samples >= MAX_SAMPLES then
            finish('maximum sample count reached before Simon duel')
        end
        return
    end

    if frames >= TIMEOUT_FRAMES then
        capture('timeout snapshot')
        finish('timed out before Simon duel was observed')
    end
end

listener_simon_duel_timing = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('simon_duel_timing: visit Simon before and after the festival as needed')
