-- card_active_pane.lua
--
-- WHAT THIS ANSWERS
--   D_8009B0AC changes between 0 and 1 when the player crosses between the
--   two panes on the Library and Build Deck screens. Two live observations
--   support an active-pane role, but the meaning of each value is not pinned.
--
--   This records the value only when one of those screens is active and
--   includes the card-grid cursor and selected-card value for context.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game.
--   2. Debug -> Lua editor, paste this file, and let it auto-run.
--   3. Open either Build Deck or Library.
--   4. Move from one pane to the other and back at least once.
--   5. Copy the whole document into
--      tools/trace/result/card_active_pane.txt and fill in the context.
--
--   No breakpoint, debugger pause, or interpreter CPU is required.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For each sample, state which pane was visibly active. On Build Deck,
--   distinguish the chest list from the deck list. On Library, describe the
--   left and right panels and what control caused the transition.

local ffi = require('ffi')

local SCRIPT_NAME = 'card_active_pane'
local MAIN_MODE = 0x8009b26c
local LIBRARY_MODE = 4
local BUILD_DECK_MODE = 7
local ACTIVE_PANE = 0x8009b0ac
local CURSOR_COLUMN = 0x8009b258
local CURSOR_ROW = 0x8009b259
local SELECTED_CARD = 0x8009b338
local MIN_SAMPLES = 3
local MAX_SAMPLES = 12
local SETTLE_FRAMES = 1200
local TIMEOUT_FRAMES = 36000

local mem = PCSX.getMemPtr()

local function phys(addr) return addr - 0x80000000 end
local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end
local function u16(addr)
    return tonumber(ffi.cast('uint16_t*', mem + phys(addr))[0])
end

local function screenName(mode)
    if mode == LIBRARY_MODE then
        return 'Library'
    end
    return 'Build Deck'
end

local lines = {}
local samples = 0
local frames = 0
local quiet = 0
local lastKey = nil
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function capture(mode)
    samples = samples + 1
    emit('')
    emit(string.format('--- sample %d of %d ---', samples, MAX_SAMPLES))
    emit(string.format(
        '  screen=%s mode=0x%02X active_pane=%d',
        screenName(mode), u8(MAIN_MODE), u8(ACTIVE_PANE)
    ))
    emit(string.format(
        '  grid_column=%d grid_row=%d selected_card=%d',
        u8(CURSOR_COLUMN), u8(CURSOR_ROW), u16(SELECTED_CARD)
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
    print('<for each sample, identify the visible active pane and the control')
    print(' used to switch panes>')
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
    if mode ~= LIBRARY_MODE and mode ~= BUILD_DECK_MODE then
        if frames >= TIMEOUT_FRAMES then
            finish('timed out before Library or Build Deck was observed')
        end
        return
    end

    local key = string.format('%d,%d', mode, u8(ACTIVE_PANE))
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
    if samples >= MIN_SAMPLES and quiet >= SETTLE_FRAMES then
        finish('no pane transition for twenty seconds')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out before enough pane transitions were observed')
    end
end

listener_card_active_pane = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('card_active_pane: waiting for Library or Build Deck; switch panes')
