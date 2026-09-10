-- debug_menu_entry_labels.lua
--
-- WHAT THIS ANSWERS
--   Static code maps the Debug Menu's 20 cursor positions through one of two
--   adjacent dispatch tables. Selector 0 uses the 21-entry table at
--   0x80090D84. Selector 1 starts at the two-entry table at 0x80090D7C, so
--   cursor positions above zero continue into the adjacent table. The code
--   establishes those targets, but not the labels visible on either page or
--   whether the alternate page visibly supports all 20 cursor positions.
--
--   This trace records every selector/cursor pair and the resident function
--   pointer that confirmation would dispatch. It never activates an entry.
--   Human context supplies the exact highlighted label for each row and notes
--   blanks, duplicates, stale text, or cursor positions that are not visible.
--
-- HOW TO RUN
--   1. Use the repository-documented `80184594 000B` code to enter the Debug
--      Menu, then disable the code after the menu appears.
--   2. Paste this script. It polls RAM only; interpreter mode is not required.
--   3. Do NOT press Circle, Cross, or Square. Visit all 20 cursor positions
--      with the d-pad, noting each highlighted label.
--   4. Press Select once to toggle the page, then visit all 20 positions again.
--      The trace finishes after all 40 selector/cursor pairs are observed.
--   5. Copy the whole document into
--      tools/trace/result/debug_menu_entry_labels.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Name the heading and every highlighted row for selector 0 and selector 1.
--   State whether selector 1 visibly has two rows or 20, whether any positions
--   are blank/duplicated, and whether the cursor can visit every reported row.
--   Confirm that no entry was activated during the capture.

local ffi = require('ffi')

local SCRIPT_NAME = 'debug_menu_entry_labels'
local MAIN_MODE = 0x8009b26c
local DEBUG_ACTION = 0x8009b2eb
local DEBUG_SELECTOR = 0x8009b2f0
local DEBUG_CURSOR = 0x8009b2f1
local PAD1_REPEAT = 0x8009b394
local PAD1_PRESSED = 0x8009b398
local SELECTOR_1_TABLE = 0x80090d7c
local SELECTOR_0_TABLE = 0x80090d84
local DEBUG_MODE = 0
local CURSOR_COUNT = 20
local TARGET_CONTEXTS = CURSOR_COUNT * 2
local NO_MOVEMENT_WARNING_FRAMES = 600
local TIMEOUT_FRAMES = 36000

local mem = PCSX.getMemPtr()

local function phys(addr)
    return addr - 0x80000000
end

local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end

local function s8(addr)
    local value = u8(addr)
    if value >= 0x80 then
        return value - 0x100
    end
    return value
end

local function u16(addr)
    return tonumber(ffi.cast('uint16_t*', mem + phys(addr))[0])
end

local function u32(addr)
    return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0])
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function normalizedSelector(raw)
    if raw == 0 then
        return 0
    end
    return 1
end

local function tableAddress(selector)
    if selector == 0 then
        return SELECTOR_0_TABLE
    end
    return SELECTOR_1_TABLE
end

local function snapshot()
    local selectorRaw = u8(DEBUG_SELECTOR)
    return {
        modeRaw = u8(MAIN_MODE),
        mode = mainMode(),
        action = u8(DEBUG_ACTION),
        selectorRaw = selectorRaw,
        selector = normalizedSelector(selectorRaw),
        cursor = s8(DEBUG_CURSOR),
        repeatMask = u16(PAD1_REPEAT),
        pressedMask = u16(PAD1_PRESSED),
    }
end

local lines = {}
local seen = {}
local frames = 0
local contextCount = 0
local selectorCounts = {0, 0}
local lastContextFrame = 0
local warned = false
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function finish(reason)
    if done then
        return
    end
    done = true

    local final = snapshot()
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<name both page headings and every highlighted row by selector/cursor;')
    print(' note blank, duplicate, hidden, or unreachable rows; state whether')
    print(' selector 1 visibly has two entries or 20; confirm no entry activated>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: contexts=%d selector_0=%d selector_1=%d '
            .. 'mode=%d selector=%d cursor=%d action=%d',
        contextCount,
        selectorCounts[1],
        selectorCounts[2],
        final.mode,
        final.selector,
        final.cursor,
        final.action
    ))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function captureContext()
    local value = snapshot()
    if value.cursor < 0 or value.cursor >= CURSOR_COUNT then
        finish(string.format(
            'cursor %d outside expected range 0-%d',
            value.cursor,
            CURSOR_COUNT - 1
        ))
        return
    end

    local key = string.format('%d:%02d', value.selector, value.cursor)
    if not seen[key] then
        seen[key] = true
        contextCount = contextCount + 1
        selectorCounts[value.selector + 1] =
            selectorCounts[value.selector + 1] + 1
        lastContextFrame = frames

        local dispatchIndex = (value.cursor + 1) % 32
        local table = tableAddress(value.selector)
        local target = u32(table + dispatchIndex * 4)
        emit(string.format(
            'context=%02d frame=%06d selector_raw=0x%02X selector=%d '
                .. 'cursor=%02d dispatch_index=%02d table=0x%08X '
                .. 'target=0x%08X repeat=0x%04X pressed=0x%04X',
            contextCount,
            frames,
            value.selectorRaw,
            value.selector,
            value.cursor,
            dispatchIndex,
            table,
            target,
            value.repeatMask,
            value.pressedMask
        ))
        print(string.format(
            '%s: captured selector %d cursor %d; note the highlighted label',
            SCRIPT_NAME,
            value.selector,
            value.cursor
        ))
    end

    if value.action ~= 0 then
        finish(string.format(
            'debug action %d started; rerun without Circle/Cross/Square',
            value.action
        ))
    elseif contextCount >= TARGET_CONTEXTS then
        finish('captured all 40 selector/cursor contexts')
    end
end

local function poll()
    if done then
        return
    end

    frames = frames + 1
    if mainMode() ~= DEBUG_MODE then
        finish('left Debug Menu after a partial label capture')
        return
    end

    captureContext()
    if done then
        return
    end

    if not warned
        and frames - lastContextFrame >= NO_MOVEMENT_WARNING_FRAMES then
        warned = true
        print(SCRIPT_NAME
            .. ': no new cursor position observed; use only d-pad and Select')
    end
    if frames >= TIMEOUT_FRAMES then
        finish('timed out after a partial Debug Menu label capture')
    end
end

local expectedTableWords = {
    {SELECTOR_1_TABLE, 0x80031078},
    {SELECTOR_1_TABLE + 4, 0x80031000},
    {SELECTOR_0_TABLE, 0x80031078},
}
local signatureError = nil
for _, expected in ipairs(expectedTableWords) do
    local actual = u32(expected[1])
    if actual ~= expected[2] then
        signatureError = string.format(
            'dispatch table signature mismatch at 0x%08X: '
                .. 'expected 0x%08X, got 0x%08X',
            expected[1],
            expected[2],
            actual
        )
        break
    end
end

if signatureError ~= nil then
    finish(signatureError)
elseif mainMode() ~= DEBUG_MODE then
    finish(string.format(
        'main mode %d is not Debug Menu mode %d',
        mainMode(),
        DEBUG_MODE
    ))
else
    captureContext()
    if not done then
        listener_debug_menu_entry_labels_reset =
            PCSX.Events.createEventListener(
                'ExecutionFlow::Reset',
                function()
                    local ok, err = pcall(function()
                        finish('reset observed; rerun from Debug Menu')
                    end)
                    if not ok then
                        finish('reset callback error: ' .. tostring(err))
                    end
                end
            )

        listener_debug_menu_entry_labels =
            PCSX.Events.createEventListener(
                'GPU::Vsync',
                function()
                    local ok, err = pcall(poll)
                    if not ok then
                        finish('script error: ' .. tostring(err))
                    end
                end
            )

        print(SCRIPT_NAME
            .. ': armed; visit all 20 d-pad positions, press Select, '
            .. 'then visit all 20 again; do not confirm or cancel')
    end
end
