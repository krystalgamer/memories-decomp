-- library_input_map.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md records that Right or Cross can open a seen
--   Library card in 3-D, but leaves the rest of the Library button map among
--   the unverified per-screen controls. Matching func_8002A788 reads held
--   directions and L1/R1 plus newly pressed Cross/Circle while the Library
--   state is 1.
--
--   This trace captures nine isolated physical inputs with their raw held and
--   pressed masks plus stable pre/post Library state, cursor row/column,
--   selected card ID, and main mode. Human context identifies each visible
--   grid, 3-D, return, or exit response.
--
-- HOW TO RUN
--   1. Enter the Library on a save where the current card is visible.
--   2. Paste this script while browsing the card grid. No interpreter CPU or
--      debugger breakpoint is required.
--   3. Tap one input at a time, waiting for "sample settled" before the next:
--      Right, Left, Down, Up, R1, L1, Cross, Circle, Circle.
--   4. Use the first Circle to return from the 3-D/card view and the second to
--      leave the Library. If the visible flow differs, record exactly what
--      happened rather than forcing the expected sequence.
--   5. Copy the whole document into
--      tools/trace/result/library_input_map.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For every sample, name the physical button and describe the visible
--   cursor/page/3-D response. Name the selected card used for Cross and state
--   what each Circle press did.

local ffi = require('ffi')

local SCRIPT_NAME = 'library_input_map'
local MAIN_MODE = 0x8009b26c
local CURSOR_COLUMN = 0x8009b258
local CURSOR_ROW = 0x8009b259
local SELECTED_CARD_ID = 0x8009b338
local PAD1_PRESSED = 0x8009b398
local PAD1_HELD = 0x8009b3a4
local LIBRARY_STATE = 0x800ea1e8
local LIBRARY_MODE = 4
local TARGET_SAMPLES = 9
local MAX_SAMPLES = 12
local STABLE_FRAMES = 8
local MAX_SETTLE_FRAMES = 120
local QUIET_FRAMES = 120
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
    return u8(addr) + u8(addr + 1) * 0x100
end

local function s16(addr)
    local value = u16(addr)
    if value >= 0x8000 then
        return value - 0x10000
    end
    return value
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function snapshot()
    return {
        modeRaw = u8(MAIN_MODE),
        mode = mainMode(),
        state = u8(LIBRARY_STATE),
        row = s8(CURSOR_ROW),
        column = s8(CURSOR_COLUMN),
        selected = s16(SELECTED_CARD_ID),
    }
end

local function snapshotsEqual(left, right)
    return left.modeRaw == right.modeRaw
        and left.state == right.state
        and left.row == right.row
        and left.column == right.column
        and left.selected == right.selected
end

local function snapshotText(prefix, frame, value)
    return string.format(
        '%s frame=%06d mode_raw=0x%02X mode=%d state=0x%02X '
            .. 'state_low=%d row=%d column=%d selected_card=%d',
        prefix,
        frame,
        value.modeRaw,
        value.mode,
        value.state,
        value.state % 16,
        value.row,
        value.column,
        value.selected
    )
end

local lines = {}
local frames = 0
local sampleCount = 0
local quietFrames = 0
local inputLatched = false
local pending = nil
local callbackError = nil
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function finish(reason)
    if done then
        return
    end
    done = true

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<for every sample, name the physical button and visible grid/page/3-D')
    print(' response; name the card used for Cross and state what each Circle did>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    local final = snapshot()
    print(string.format(
        'summary: samples=%d mode=%d state=0x%02X row=%d column=%d '
            .. 'selected_card=%d',
        sampleCount,
        final.mode,
        final.state,
        final.row,
        final.column,
        final.selected
    ))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function beginSample(held, pressed)
    local before = snapshot()
    sampleCount = sampleCount + 1
    pending = {
        number = sampleCount,
        held = held,
        pressed = pressed,
        before = before,
        last = before,
        stable = 0,
        age = 0,
    }
    quietFrames = 0
    emit(string.format(
        'sample_start=%02d frame=%06d held=0x%04X pressed=0x%04X',
        pending.number,
        frames,
        held,
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

local function completeSample(reason)
    local after = snapshot()
    local current = pending

    emit(snapshotText(
        string.format('sample_post=%02d', current.number),
        frames,
        after
    ))
    emit(string.format(
        'sample_end=%02d frame=%06d reason=%s row_delta=%d '
            .. 'column_delta=%d state_changed=%s selected_changed=%s '
            .. 'mode_changed=%s',
        current.number,
        frames,
        reason,
        after.row - current.before.row,
        after.column - current.before.column,
        tostring(after.state ~= current.before.state),
        tostring(after.selected ~= current.before.selected),
        tostring(after.mode ~= current.before.mode)
    ))
    pending = nil
    print(string.format(
        '%s: sample %d settled; perform the next isolated input',
        SCRIPT_NAME,
        current.number
    ))
end

local function pollPending()
    local current = snapshot()

    pending.age = pending.age + 1
    if snapshotsEqual(pending.last, current) then
        pending.stable = pending.stable + 1
    else
        pending.last = current
        pending.stable = 0
    end

    if current.mode ~= LIBRARY_MODE then
        completeSample('Library mode changed')
        finish('captured input that left the Library')
        return
    end

    if u16(PAD1_HELD) == 0
        and u16(PAD1_PRESSED) == 0
        and pending.stable >= STABLE_FRAMES then
        completeSample('state stable after input release')
    elseif pending.age >= MAX_SETTLE_FRAMES then
        completeSample('maximum settle window reached')
    end
end

local function poll()
    if done then
        return
    end

    frames = frames + 1

    if callbackError ~= nil then
        finish('callback error: ' .. callbackError)
        return
    end

    if pending ~= nil then
        pollPending()
    elseif mainMode() ~= LIBRARY_MODE then
        finish('left Library mode after captured input')
        return
    end

    if pending == nil and not done then
        local held = u16(PAD1_HELD)
        local pressed = u16(PAD1_PRESSED)
        local active = held ~= 0 or pressed ~= 0

        if active and not inputLatched then
            inputLatched = true
            beginSample(held, pressed)
        elseif not active then
            inputLatched = false
        end
    end

    if sampleCount >= MAX_SAMPLES and pending == nil then
        finish('maximum input sample count reached')
        return
    end

    if sampleCount >= TARGET_SAMPLES and pending == nil then
        quietFrames = quietFrames + 1
        if quietFrames >= QUIET_FRAMES then
            finish('captured nine inputs followed by quiet time')
            return
        end
    end

    if frames >= TIMEOUT_FRAMES then
        finish('timed out after a partial Library input trace')
    end
end

if mainMode() ~= LIBRARY_MODE then
    print(string.format(
        '%s: main mode %d is not Library mode %d',
        SCRIPT_NAME,
        mainMode(),
        LIBRARY_MODE
    ))
    return
end

listener_library_input_map_reset =
    PCSX.Events.createEventListener(
        'ExecutionFlow::Reset',
        function()
            local ok, err = pcall(function()
                finish('reset observed; rerun after entering the Library')
            end)
            if not ok then
                callbackError = tostring(err)
            end
        end
    )

listener_library_input_map =
    PCSX.Events.createEventListener(
        'GPU::Vsync',
        function()
            local ok, err = pcall(poll)
            if not ok then
                callbackError = tostring(err)
                finish('script error: ' .. callbackError)
            end
        end
    )

print(SCRIPT_NAME .. ': armed; perform Right, Left, Down, Up, R1, L1, '
    .. 'Cross, Circle, Circle')
