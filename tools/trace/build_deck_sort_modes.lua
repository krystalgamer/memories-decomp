-- build_deck_sort_modes.lua
--
-- WHAT THIS ANSWERS
--   func_80032C48 selects one of its sort-key builders from byte +0x2D45 of
--   the list workspace. The known qsort bases place the two candidate bytes
--   at:
--
--     0x80102D49  trunk view (722 records at 0x80100004)
--     0x80105A95  deck view  (40 records at 0x80102D50)
--
--   The visible names and numeric encoding of the sort modes are not yet
--   known. This samples both candidate bytes whenever START changes either
--   one; the human context identifies which pane and label were visible.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game.
--   2. Debug -> Lua editor, paste this file, and let it auto-run.
--   3. Enter Build Deck. In the chest pane, press START through every sort
--      label, then switch to the deck pane and repeat.
--   4. Copy the whole document into
--      tools/trace/result/build_deck_sort_modes.txt and fill in the context.
--
--   No breakpoint, debugger pause, or interpreter CPU is required.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For each sample, record the visible sort label and whether the chest or
--   deck pane was active. Note the order in which START cycles the labels.

local ffi = require('ffi')

local SCRIPT_NAME = 'build_deck_sort_modes'
local MAIN_MODE = 0x8009b26c
local BUILD_DECK_MODE = 7
local SELECTED_CARD = 0x8009b338
local TRUNK_COUNT = 0x80102d46
local TRUNK_SORT_MODE = 0x80102d49
local DECK_COUNT = 0x80105a92
local DECK_SORT_MODE = 0x80105a95
local MIN_SAMPLES = 10
local MAX_SAMPLES = 20
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

local lines = {}
local samples = 0
local frames = 0
local quiet = 0
local lastKey = nil
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function capture()
    samples = samples + 1
    emit('')
    emit(string.format('--- sample %d of %d ---', samples, MAX_SAMPLES))
    emit(string.format(
        '  mode=0x%02X selected_card=%d',
        u8(MAIN_MODE), u16(SELECTED_CARD)
    ))
    emit(string.format(
        '  trunk: count=%d sort_mode=%d',
        u16(TRUNK_COUNT), u8(TRUNK_SORT_MODE)
    ))
    emit(string.format(
        '  deck:  count=%d sort_mode=%d',
        u16(DECK_COUNT), u8(DECK_SORT_MODE)
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
    print('<for each sample, record the visible sort label and whether the')
    print(' chest or deck pane was active>')
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

local function key()
    return string.format(
        '%d,%d',
        u8(TRUNK_SORT_MODE), u8(DECK_SORT_MODE)
    )
end

local function poll()
    if done then
        return
    end

    frames = frames + 1
    if u8(MAIN_MODE) % 32 ~= BUILD_DECK_MODE then
        if frames >= TIMEOUT_FRAMES then
            finish('timed out before Build Deck was observed')
        end
        return
    end

    local currentKey = key()
    if currentKey ~= lastKey then
        lastKey = currentKey
        quiet = 0
        capture()
        if samples >= MAX_SAMPLES then
            finish('maximum sample count reached')
        end
        return
    end

    quiet = quiet + 1
    if samples >= MIN_SAMPLES and quiet >= SETTLE_FRAMES then
        finish('no sort transition for twenty seconds')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out before enough sort transitions were observed')
    end
end

listener_build_deck_sort_modes = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('build_deck_sort_modes: waiting for Build Deck; cycle START in both panes')
