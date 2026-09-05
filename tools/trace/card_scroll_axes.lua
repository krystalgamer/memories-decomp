-- card_scroll_axes.lua
--
-- WHAT THIS ANSWERS
--   D_8009B148 is known to follow vertical card-grid movement, while
--   D_8009B146 is initialized beside it and is animated by func_800338E4.
--   This records both signed values with pane, cursor, and D-pad context to
--   determine whether D_8009B146 is the corresponding horizontal scroll
--   offset or serves another screen-specific role.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game.
--   2. Debug -> Lua editor, paste this file, and let it auto-run.
--   3. Enter Build Deck or Library.
--   4. Move vertically far enough to scroll a list, then use LEFT/RIGHT to
--      switch panes and exercise any horizontal movement available.
--   5. Repeat on the other screen if possible.
--   6. Copy the whole document into
--      tools/trace/result/card_scroll_axes.txt and fill in the context.
--
--   No breakpoint, debugger pause, or interpreter CPU is required.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For each change, identify the visible screen and pane, the direction
--   pressed, and whether the card list or another object moved horizontally
--   or vertically.

local ffi = require('ffi')

local SCRIPT_NAME = 'card_scroll_axes'
local MAIN_MODE = 0x8009b26c
local LIBRARY_MODE = 4
local BUILD_DECK_MODE = 7
local ACTIVE_PANE = 0x8009b0ac
local CURSOR_COLUMN = 0x8009b258
local CURSOR_ROW = 0x8009b259
local SELECTED_CARD = 0x8009b338
local PAD_EDGE = 0x8009b3a4
local VALUE_146 = 0x8009b146
local VALUE_148 = 0x8009b148
local MIN_SAMPLES = 8
local MAX_SAMPLES = 48
local SETTLE_FRAMES = 1200
local TIMEOUT_FRAMES = 72000

local mem = PCSX.getMemPtr()

local function phys(addr) return addr - 0x80000000 end
local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end
local function u16(addr)
    return tonumber(ffi.cast('uint16_t*', mem + phys(addr))[0])
end
local function s16(addr)
    local value = u16(addr)
    if value >= 0x8000 then
        return value - 0x10000
    end
    return value
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
local enteredScreen = false
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
        '  cursor_column=%d cursor_row=%d selected_card=%d pad_edge=0x%04X',
        u8(CURSOR_COLUMN), u8(CURSOR_ROW), u16(SELECTED_CARD), u16(PAD_EDGE)
    ))
    emit(string.format(
        '  value_8009B146=%d value_8009B148=%d',
        s16(VALUE_146), s16(VALUE_148)
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
    print('<for each sample, identify the visible pane, direction pressed,')
    print(' and whether the list or another object moved on either axis>')
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

local function currentKey(mode)
    return string.format(
        '%d,%d,%d,%d,%d,%d,%d,%d',
        mode,
        u8(ACTIVE_PANE),
        u8(CURSOR_COLUMN),
        u8(CURSOR_ROW),
        u16(SELECTED_CARD),
        s16(VALUE_146),
        s16(VALUE_148),
        u16(PAD_EDGE)
    )
end

local function poll()
    if done then
        return
    end

    frames = frames + 1
    local mode = u8(MAIN_MODE) % 32
    if mode ~= LIBRARY_MODE and mode ~= BUILD_DECK_MODE then
        if enteredScreen and samples >= MIN_SAMPLES then
            finish('left the observed screens')
        elseif frames >= TIMEOUT_FRAMES then
            finish('timed out before Library or Build Deck was observed')
        end
        return
    end

    enteredScreen = true
    local key = currentKey(mode)
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
        finish('no tracked change for twenty seconds')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out before enough navigation was observed')
    end
end

listener_card_scroll_axes = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('card_scroll_axes: waiting for Library or Build Deck navigation')
