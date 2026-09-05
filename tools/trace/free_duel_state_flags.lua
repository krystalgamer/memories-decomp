-- free_duel_state_flags.lua
--
-- WHAT THIS ANSWERS
--   D_8009B365 is reset when the Free Duel flow is initialized. Existing
--   traces suggest bit 0x40 is set when opening Build Deck and bit 0x80 is set
--   when starting a duel, then consulted when the Free Duel screen returns.
--   This captures the byte across both round trips so those roles can be
--   confirmed before assigning a semantic name.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game.
--   2. Debug -> Lua editor, paste this file, and let it auto-run.
--   3. Enter Free Duel, choose the Build Deck tile, then return.
--   4. Select an opponent, finish the duel, and return to Free Duel.
--   5. Copy the whole document into
--      tools/trace/result/free_duel_state_flags.txt and fill in the context.
--
--   No breakpoint, debugger pause, or interpreter CPU is required.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For each transition, state the visible screen and action. For the duel,
--   include whether the player won or lost and whether the W/L record changed
--   after returning to Free Duel.

local ffi = require('ffi')

local SCRIPT_NAME = 'free_duel_state_flags'
local MAIN_MODE = 0x8009b26c
local DUEL_MODE = 3
local FREE_DUEL_MODE = 6
local BUILD_DECK_MODE = 7
local STATE_FLAGS = 0x8009b365
local OPPONENT_ID = 0x8009b361
local CURSOR_COLUMN = 0x8009b366
local CURSOR_ROW = 0x8009b367
local PAD_HELD = 0x8009b3a4
local RETURN_SETTLE_FRAMES = 180
local TIMEOUT_FRAMES = 108000
local MAX_SAMPLES = 32

local mem = PCSX.getMemPtr()

local function phys(addr) return addr - 0x80000000 end
local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end
local function u16(addr)
    return tonumber(ffi.cast('uint16_t*', mem + phys(addr))[0])
end
local function s8(addr)
    local value = u8(addr)
    if value >= 0x80 then
        return value - 0x100
    end
    return value
end
local function hasBit(value, mask)
    return value % (mask * 2) >= mask
end

local function modeName(mode)
    if mode == DUEL_MODE then
        return 'Duel'
    end
    if mode == FREE_DUEL_MODE then
        return 'Free Duel'
    end
    if mode == BUILD_DECK_MODE then
        return 'Build Deck'
    end
    return 'other'
end

local lines = {}
local samples = 0
local frames = 0
local quiet = 0
local lastKey = nil
local sawFreeDuel = false
local sawBuildDeck = false
local sawDuel = false
local returnedFromBuildDeck = false
local returnedFromDuel = false
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function capture(mode)
    local flags = u8(STATE_FLAGS)

    samples = samples + 1
    emit('')
    emit(string.format('--- sample %d of %d ---', samples, MAX_SAMPLES))
    emit(string.format(
        '  screen=%s mode=0x%02X state_flags=0x%02X bit40=%d bit80=%d',
        modeName(mode),
        u8(MAIN_MODE),
        flags,
        hasBit(flags, 0x40) and 1 or 0,
        hasBit(flags, 0x80) and 1 or 0
    ))
    emit(string.format(
        '  cursor_column=%d cursor_row=%d opponent_id=%d pad_held=0x%04X',
        u8(CURSOR_COLUMN),
        u8(CURSOR_ROW),
        s8(OPPONENT_ID),
        u16(PAD_HELD)
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
    print('<identify each screen transition, the action that caused it, the')
    print(' duel result, and whether the selected W/L record changed>')
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

local function poll()
    if done then
        return
    end

    frames = frames + 1
    local mode = u8(MAIN_MODE) % 32
    local flags = u8(STATE_FLAGS)
    local key = string.format('%d,%d', mode, flags)

    if mode == FREE_DUEL_MODE then
        if sawBuildDeck then
            returnedFromBuildDeck = true
        end
        if sawDuel then
            returnedFromDuel = true
        end
        sawFreeDuel = true
    elseif sawFreeDuel and mode == BUILD_DECK_MODE then
        sawBuildDeck = true
    elseif sawFreeDuel and mode == DUEL_MODE then
        sawDuel = true
    end

    if key ~= lastKey then
        lastKey = key
        quiet = 0
        capture(mode)
        if samples >= MAX_SAMPLES then
            finish('maximum sample count reached')
        end
        return
    end

    quiet = quiet + 1
    if returnedFromBuildDeck and returnedFromDuel and
       quiet >= RETURN_SETTLE_FRAMES then
        finish('captured both Free Duel round trips')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out; partial transition data follows')
    end
end

listener_free_duel_state_flags = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('free_duel_state_flags: enter Free Duel, visit Build Deck, then duel')
