-- build_deck_card_grid_cursor.lua
--
-- WHAT THIS ANSWERS
--   notes/research/Unchiga_Symbols/findings.md records that
--   gCardGrid_bCursorColumn/Row were live-confirmed in the Library, but their
--   scope in Build Deck remains unverified. notes/research/the-game.md says
--   Build Deck keeps list-local cursor/window state rather than scrolling the
--   global viewport.
--
--   This trace observes both sides of that question. The active Build Deck
--   CardList first/target/cursor/current-card fields and pane/viewport state
--   are the positive control that visible input was processed. The Library
--   column/row globals and the shared selected-card global are captured beside
--   them, with separate change summaries, to show which state tracks the same
--   visible movement.
--
-- HOW TO RUN
--   1. Enter Build Deck (main mode 7) and wait until one pane is idle.
--   2. Paste this script. No interpreter CPU or breakpoint is required.
--   3. Tap one input at a time and wait for "sample settled" before the next:
--      Down, Up, R1, L1, the horizontal direction that switches panes, then
--      the opposite horizontal direction to return.
--   4. For every sample, note the visible pane, cursor row, and card ID. Do
--      not confirm a card or leave Build Deck during this control run.
--   5. Copy the whole document into
--      tools/trace/result/build_deck_card_grid_cursor.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   State whether the first pane was trunk or deck. For every sample, name
--   the physical button and visible pane/cursor/card change. Explicitly note
--   any input that produced no visible change.

local ffi = require('ffi')

local SCRIPT_NAME = 'build_deck_card_grid_cursor'
local MAIN_MODE = 0x8009b26c
local BUILD_DECK_ROOT_POINTER = 0x8009b2fc
local VIEWPORT_X = 0x8009b146
local LIBRARY_COLUMN = 0x8009b258
local LIBRARY_ROW = 0x8009b259
local LIBRARY_SELECTED_CARD = 0x8009b338
local PAD1_REPEAT = 0x8009b394
local PAD1_PRESSED = 0x8009b398
local PAD1_HELD = 0x8009b3a4
local BUILD_DECK_MODE = 7
local ROOT_STATE_OFFSET = 0x633e
local ROOT_NEXT_STATE_OFFSET = 0x6340
local ROOT_PANE_OFFSET = 0x6342
local PANE_STRIDE = 0x2d4c
local FIRST_LIST_OFFSET = 4
local LIST_FIRST_OFFSET = 0x2d3c
local LIST_FIRST_TARGET_OFFSET = 0x2d3e
local LIST_ROW_COUNT_OFFSET = 0x2d40
local LIST_CURSOR_OFFSET = 0x2d48
local ENTRY_SIZE = 0x10
local ENTRY_ID_OFFSET = 4
local ENTRY_FLAGS_OFFSET = 0x0d
local TARGET_SAMPLES = 6
local MAX_SAMPLES = 8
local MIN_SETTLE_FRAMES = 30
local STABLE_FRAMES = 8
local MAX_SETTLE_FRAMES = 240
local QUIET_FRAMES = 120
local NO_INPUT_WARNING_FRAMES = 600
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

local function s16(addr)
    local value = u16(addr)
    if value >= 0x8000 then
        return value - 0x10000
    end
    return value
end

local function u32(addr)
    return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0])
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function isRootValid(root)
    return root % 4 == 0
        and root >= 0x80000000
        and root + ROOT_PANE_OFFSET < 0x80200000
end

local function readSnapshot()
    local root = u32(BUILD_DECK_ROOT_POINTER)
    if not isRootValid(root) then
        return nil, string.format('invalid Build Deck root pointer 0x%08X', root)
    end

    local pane = u8(root + ROOT_PANE_OFFSET)
    if pane > 1 then
        return nil, string.format('invalid Build Deck pane index %d', pane)
    end

    local list = root + FIRST_LIST_OFFSET + pane * PANE_STRIDE
    local first = s16(list + LIST_FIRST_OFFSET)
    local rowCount = s16(list + LIST_ROW_COUNT_OFFSET)
    local cursor = s8(list + LIST_CURSOR_OFFSET)
    local index = first + cursor
    if rowCount <= 0 or index < 0 or index >= rowCount then
        return nil, string.format(
            'invalid active CardList index first=%d cursor=%d rows=%d',
            first,
            cursor,
            rowCount
        )
    end

    local entry = list + index * ENTRY_SIZE
    if entry < 0x80000000 or entry + ENTRY_FLAGS_OFFSET >= 0x80200000 then
        return nil, string.format('active CardList entry outside RAM 0x%08X', entry)
    end

    return {
        modeRaw = u8(MAIN_MODE),
        mode = mainMode(),
        root = root,
        rootState = u16(root + ROOT_STATE_OFFSET),
        nextState = u16(root + ROOT_NEXT_STATE_OFFSET),
        pane = pane,
        viewportX = s16(VIEWPORT_X),
        first = first,
        firstTarget = s16(list + LIST_FIRST_TARGET_OFFSET),
        rowCount = rowCount,
        cursor = cursor,
        activeIndex = index,
        activeCard = u16(entry + ENTRY_ID_OFFSET),
        activeFlags = u8(entry + ENTRY_FLAGS_OFFSET),
        libraryColumn = s8(LIBRARY_COLUMN),
        libraryRow = s8(LIBRARY_ROW),
        selectedGlobal = s16(LIBRARY_SELECTED_CARD),
    }, nil
end

local function snapshotsEqual(left, right)
    return left.modeRaw == right.modeRaw
        and left.rootState == right.rootState
        and left.nextState == right.nextState
        and left.pane == right.pane
        and left.viewportX == right.viewportX
        and left.first == right.first
        and left.firstTarget == right.firstTarget
        and left.rowCount == right.rowCount
        and left.cursor == right.cursor
        and left.activeIndex == right.activeIndex
        and left.activeCard == right.activeCard
        and left.activeFlags == right.activeFlags
        and left.libraryColumn == right.libraryColumn
        and left.libraryRow == right.libraryRow
        and left.selectedGlobal == right.selectedGlobal
end

local function snapshotText(prefix, frame, value)
    return string.format(
        '%s frame=%06d mode_raw=0x%02X mode=%d root=0x%08X '
            .. 'root_state=0x%04X next_state=0x%04X pane=%d viewport_x=%d '
            .. 'first=%d first_target=%d rows=%d cursor=%d '
            .. 'active_index=%d active_card=%d active_flags=0x%02X '
            .. 'library_column=%d library_row=%d selected_global=%d',
        prefix,
        frame,
        value.modeRaw,
        value.mode,
        value.root,
        value.rootState,
        value.nextState,
        value.pane,
        value.viewportX,
        value.first,
        value.firstTarget,
        value.rowCount,
        value.cursor,
        value.activeIndex,
        value.activeCard,
        value.activeFlags,
        value.libraryColumn,
        value.libraryRow,
        value.selectedGlobal
    )
end

local lines = {}
local frames = 0
local sampleCount = 0
local quietFrames = 0
local inputLatched = false
local warned = false
local pending = nil
local done = false
local baseline = nil
local localControlChanged = false
local gridCoordinatesChanged = false
local selectedGlobalChanged = false

local function emit(text)
    lines[#lines + 1] = text
end

local function finish(reason)
    if done then
        return
    end
    done = true

    local final = select(1, readSnapshot()) or baseline
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<state whether the first pane was trunk or deck; for every sample')
    print(' name the physical button and visible pane/cursor/card change;')
    print(' explicitly note any input that produced no visible change>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    if final ~= nil then
        print(string.format(
            'summary: samples=%d local_control_changed=%s '
                .. 'grid_coordinates_changed=%s '
                .. 'selected_card_global_changed=%s final_pane=%d '
                .. 'final_first=%d final_cursor=%d final_card=%d',
            sampleCount,
            tostring(localControlChanged),
            tostring(gridCoordinatesChanged),
            tostring(selectedGlobalChanged),
            final.pane,
            final.first,
            final.cursor,
            final.activeCard
        ))
    else
        print(string.format(
            'summary: samples=%d local_control_changed=%s '
                .. 'grid_coordinates_changed=%s '
                .. 'selected_card_global_changed=%s',
            sampleCount,
            tostring(localControlChanged),
            tostring(gridCoordinatesChanged),
            tostring(selectedGlobalChanged)
        ))
    end
    if baseline ~= nil then
        print(snapshotText('baseline', 0, baseline))
    end
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function inputMasks()
    return u16(PAD1_HELD), u16(PAD1_REPEAT), u16(PAD1_PRESSED)
end

local function beginSample(held, repeatMask, pressed)
    if sampleCount >= MAX_SAMPLES then
        finish('maximum sample count reached')
        return
    end

    local before, err = readSnapshot()
    if before == nil then
        finish(err)
        return
    end

    sampleCount = sampleCount + 1
    pending = {
        number = sampleCount,
        held = held,
        repeatMask = repeatMask,
        pressed = pressed,
        before = before,
        last = before,
        stable = 0,
        age = 0,
    }
    quietFrames = 0
    emit(string.format(
        'sample_start=%02d frame=%06d held=0x%04X repeat=0x%04X '
            .. 'pressed=0x%04X',
        pending.number,
        frames,
        held,
        repeatMask,
        pressed
    ))
    emit(snapshotText(
        string.format('sample_pre=%02d', pending.number),
        frames,
        before
    ))
    print(string.format(
        '%s: sample %d captured; release it and wait for settlement',
        SCRIPT_NAME,
        pending.number
    ))
end

local function completeSample(reason, after)
    local current = pending

    if current.before.pane ~= after.pane
        or current.before.viewportX ~= after.viewportX
        or current.before.first ~= after.first
        or current.before.firstTarget ~= after.firstTarget
        or current.before.cursor ~= after.cursor
        or current.before.activeCard ~= after.activeCard then
        localControlChanged = true
    end
    if current.before.libraryColumn ~= after.libraryColumn
        or current.before.libraryRow ~= after.libraryRow then
        gridCoordinatesChanged = true
    end
    if current.before.selectedGlobal ~= after.selectedGlobal then
        selectedGlobalChanged = true
    end

    emit(snapshotText(
        string.format('sample_post=%02d', current.number),
        frames,
        after
    ))
    emit(string.format(
        'sample_end=%02d frame=%06d reason=%s pane=%d->%d '
            .. 'viewport_x=%d->%d first=%d->%d first_target=%d->%d '
            .. 'cursor=%d->%d active_card=%d->%d '
            .. 'library_column=%d->%d library_row=%d->%d '
            .. 'selected_global=%d->%d mode=%d->%d',
        current.number,
        frames,
        reason,
        current.before.pane,
        after.pane,
        current.before.viewportX,
        after.viewportX,
        current.before.first,
        after.first,
        current.before.firstTarget,
        after.firstTarget,
        current.before.cursor,
        after.cursor,
        current.before.activeCard,
        after.activeCard,
        current.before.libraryColumn,
        after.libraryColumn,
        current.before.libraryRow,
        after.libraryRow,
        current.before.selectedGlobal,
        after.selectedGlobal,
        current.before.mode,
        after.mode
    ))
    pending = nil
    print(string.format(
        '%s: sample %d settled; perform the next isolated input',
        SCRIPT_NAME,
        current.number
    ))
end

local function pollPending()
    local current, err = readSnapshot()
    if current == nil then
        finish(err)
        return
    end
    local held, repeatMask, pressed = inputMasks()

    pending.age = pending.age + 1
    if snapshotsEqual(pending.last, current) then
        pending.stable = pending.stable + 1
    else
        pending.last = current
        pending.stable = 0
    end

    if current.mode ~= BUILD_DECK_MODE then
        completeSample('input left Build Deck', current)
        finish('captured input that left Build Deck')
    elseif pending.age >= MAX_SETTLE_FRAMES then
        completeSample('settlement timeout', current)
    elseif held == 0
        and repeatMask == 0
        and pressed == 0
        and pending.age >= MIN_SETTLE_FRAMES
        and pending.stable >= STABLE_FRAMES then
        completeSample('state stable after release', current)
    end
end

local function poll()
    if done then
        return
    end

    frames = frames + 1
    if pending ~= nil then
        pollPending()
        return
    end

    local held, repeatMask, pressed = inputMasks()
    local anyInput = held ~= 0 or repeatMask ~= 0 or pressed ~= 0
    if anyInput and not inputLatched then
        inputLatched = true
        beginSample(held, repeatMask, pressed)
        return
    elseif not anyInput then
        inputLatched = false
    end

    if mainMode() ~= BUILD_DECK_MODE then
        if sampleCount == 0 then
            finish('left Build Deck before any input was captured')
        else
            finish('left Build Deck after captured inputs')
        end
        return
    end

    if sampleCount >= TARGET_SAMPLES then
        quietFrames = quietFrames + 1
        if quietFrames >= QUIET_FRAMES then
            finish('captured six inputs followed by quiet time')
            return
        end
    elseif frames == NO_INPUT_WARNING_FRAMES and not warned then
        warned = true
        print(SCRIPT_NAME
            .. ': no input captured yet; perform the documented sequence')
    end

    if frames >= TIMEOUT_FRAMES then
        finish('timed out after a partial Build Deck cursor trace')
    end
end

if mainMode() ~= BUILD_DECK_MODE then
    finish(string.format(
        'main mode %d is not Build Deck mode %d',
        mainMode(),
        BUILD_DECK_MODE
    ))
else
    local err
    baseline, err = readSnapshot()
    if baseline == nil then
        finish(err)
    else
        listener_build_deck_card_grid_cursor_reset =
            PCSX.Events.createEventListener(
                'ExecutionFlow::Reset',
                function()
                    local ok, callbackErr = pcall(function()
                        finish('reset observed; rerun from Build Deck')
                    end)
                    if not ok then
                        finish('reset callback error: ' .. tostring(callbackErr))
                    end
                end
            )

        listener_build_deck_card_grid_cursor =
            PCSX.Events.createEventListener(
                'GPU::Vsync',
                function()
                    local ok, callbackErr = pcall(poll)
                    if not ok then
                        finish('script error: ' .. tostring(callbackErr))
                    end
                end
            )

        print(SCRIPT_NAME
            .. ': armed; perform Down, Up, R1, L1, pane switch, return')
    end
end
