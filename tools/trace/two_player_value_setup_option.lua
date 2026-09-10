-- two_player_value_setup_option.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md records the two-player starting-LP values and
--   input rules, but leaves the shared option's caption and complete
--   visibility behavior unproved. Matching MainMenu_UpdateValueSetup shows
--   that each pad can move from its LP value to shared choice 0/1, change that
--   choice horizontally, and return to its LP value.
--
--   This trace captures those focus and choice transitions with the live
--   overlay side modes, shared choice, target/display LP pairs, resident
--   output values, and physical input masks. Human context identifies the
--   caption, the text or marker at choice 0 versus 1, and when it is visible.
--
-- HOW TO RUN
--   1. Enter the 2P Duel starting-LP setup (main mode 16), then paste this
--      script. It requires the retail MainMenu_UpdateValueSetup signature; no
--      interpreter CPU or breakpoint is required.
--   2. Tap one input at a time and wait for "sample settled" before the next:
--      pad 1 Up, Left, Right, Down; pad 2 Up, Left, Right, Down; pad 1 Start.
--   3. For each sample, note the visible focus, caption, option text/marker,
--      and LP value. Do not use Circle during this control run.
--   4. Copy the whole document into
--      tools/trace/result/two_player_value_setup_option.txt and fill in
--      context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   State the initial shared-option caption and whether it was visible while
--   both sides were on their LP values. For every sample, name the physical
--   pad/button, visible focus, highlighted choice text or marker position,
--   and whether the caption remained visible after returning Down.

local ffi = require('ffi')

local SCRIPT_NAME = 'two_player_value_setup_option'
local MAIN_MODE = 0x8009b26c
local RESIDENT_TOGGLE = 0x8009b230
local RESIDENT_FIRST_LP = 0x8009b234
local RESIDENT_SECOND_LP = 0x8009b236
local PAD1_REPEAT = 0x8009b394
local PAD2_REPEAT = 0x8009b396
local PAD1_PRESSED = 0x8009b398
local PAD2_PRESSED = 0x8009b39a
local VALUE_SETUP_UPDATE = 0x801812b4
local VALUE_SETUP_UPDATE_FIRST_WORD = 0x27bdffd8
local SIDE_MODES = 0x801845bc
local VALUE_STATE = 0x801845c0
local VALUE_SETUP_MODE = 16
local TARGET_SAMPLES = 9
local MAX_SAMPLES = 12
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

local function u16(addr)
    return tonumber(ffi.cast('uint16_t*', mem + phys(addr))[0])
end

local function u32(addr)
    return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0])
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function snapshot()
    return {
        signature = u32(VALUE_SETUP_UPDATE),
        modeRaw = u8(MAIN_MODE),
        mode = mainMode(),
        residentToggle = u16(RESIDENT_TOGGLE),
        residentFirst = u16(RESIDENT_FIRST_LP),
        residentSecond = u16(RESIDENT_SECOND_LP),
        firstMode = u8(SIDE_MODES),
        secondMode = u8(SIDE_MODES + 1),
        choice = u8(SIDE_MODES + 2),
        firstTarget = u16(VALUE_STATE),
        firstDisplay = u16(VALUE_STATE + 2),
        secondTarget = u16(VALUE_STATE + 0xC),
        secondDisplay = u16(VALUE_STATE + 0xE),
    }
end

local function snapshotsEqual(left, right)
    return left.modeRaw == right.modeRaw
        and left.residentToggle == right.residentToggle
        and left.residentFirst == right.residentFirst
        and left.residentSecond == right.residentSecond
        and left.firstMode == right.firstMode
        and left.secondMode == right.secondMode
        and left.choice == right.choice
        and left.firstTarget == right.firstTarget
        and left.firstDisplay == right.firstDisplay
        and left.secondTarget == right.secondTarget
        and left.secondDisplay == right.secondDisplay
end

local function snapshotText(prefix, frame, value)
    return string.format(
        '%s frame=%06d signature=0x%08X mode_raw=0x%02X mode=%d '
            .. 'resident_toggle=0x%04X resident_first=%d resident_second=%d '
            .. 'first_mode=%d second_mode=%d choice=%d '
            .. 'first_target=%d first_display=%d '
            .. 'second_target=%d second_display=%d',
        prefix,
        frame,
        value.signature,
        value.modeRaw,
        value.mode,
        value.residentToggle,
        value.residentFirst,
        value.residentSecond,
        value.firstMode,
        value.secondMode,
        value.choice,
        value.firstTarget,
        value.firstDisplay,
        value.secondTarget,
        value.secondDisplay
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
local baseline = snapshot()

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
    print('<state the initial shared-option caption/visibility; for every sample')
    print(' name the physical pad/button, visible focus, highlighted choice text')
    print(' or marker position, LP value, and visibility after returning Down>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: samples=%d signature=0x%08X mode=%d '
            .. 'resident_toggle=0x%04X first_mode=%d second_mode=%d choice=%d',
        sampleCount,
        final.signature,
        final.mode,
        final.residentToggle,
        final.firstMode,
        final.secondMode,
        final.choice
    ))
    print(snapshotText('baseline', 0, baseline))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function allInput()
    return u16(PAD1_REPEAT),
           u16(PAD2_REPEAT),
           u16(PAD1_PRESSED),
           u16(PAD2_PRESSED)
end

local function beginSample(repeat1, repeat2, pressed1, pressed2)
    if sampleCount >= MAX_SAMPLES then
        finish('maximum sample count reached')
        return
    end

    local before = snapshot()
    sampleCount = sampleCount + 1
    pending = {
        number = sampleCount,
        repeat1 = repeat1,
        repeat2 = repeat2,
        pressed1 = pressed1,
        pressed2 = pressed2,
        before = before,
        last = before,
        stable = 0,
        age = 0,
    }
    quietFrames = 0
    emit(string.format(
        'sample_start=%02d frame=%06d pad1_repeat=0x%04X '
            .. 'pad2_repeat=0x%04X pad1_pressed=0x%04X '
            .. 'pad2_pressed=0x%04X',
        pending.number,
        frames,
        repeat1,
        repeat2,
        pressed1,
        pressed2
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
        'sample_end=%02d frame=%06d reason=%s first_mode=%d->%d '
            .. 'second_mode=%d->%d choice=%d->%d '
            .. 'first_target=%d->%d second_target=%d->%d '
            .. 'resident_toggle=0x%04X->0x%04X mode=%d->%d',
        current.number,
        frames,
        reason,
        current.before.firstMode,
        after.firstMode,
        current.before.secondMode,
        after.secondMode,
        current.before.choice,
        after.choice,
        current.before.firstTarget,
        after.firstTarget,
        current.before.secondTarget,
        after.secondTarget,
        current.before.residentToggle,
        after.residentToggle,
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
    local current = snapshot()
    local repeat1, repeat2, pressed1, pressed2 = allInput()

    pending.age = pending.age + 1
    if snapshotsEqual(pending.last, current) then
        pending.stable = pending.stable + 1
    else
        pending.last = current
        pending.stable = 0
    end

    if current.mode ~= VALUE_SETUP_MODE then
        completeSample('input left value setup')
        finish('captured input that left two-player value setup')
    elseif pending.age >= MAX_SETTLE_FRAMES then
        completeSample('settlement timeout')
    elseif repeat1 == 0
        and repeat2 == 0
        and pressed1 == 0
        and pressed2 == 0
        and pending.age >= MIN_SETTLE_FRAMES
        and pending.stable >= STABLE_FRAMES then
        completeSample('state stable after release')
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

    local repeat1, repeat2, pressed1, pressed2 = allInput()
    local anyInput = repeat1 ~= 0
        or repeat2 ~= 0
        or pressed1 ~= 0
        or pressed2 ~= 0

    if anyInput and not inputLatched then
        inputLatched = true
        beginSample(repeat1, repeat2, pressed1, pressed2)
        return
    elseif not anyInput then
        inputLatched = false
    end

    if mainMode() ~= VALUE_SETUP_MODE then
        if sampleCount == 0 then
            finish('left two-player value setup before any input was captured')
        else
            finish('left two-player value setup after captured inputs')
        end
        return
    end

    if sampleCount >= TARGET_SAMPLES then
        quietFrames = quietFrames + 1
        if quietFrames >= QUIET_FRAMES then
            finish('captured nine inputs followed by quiet time')
            return
        end
    elseif frames == NO_INPUT_WARNING_FRAMES and not warned then
        warned = true
        print(SCRIPT_NAME
            .. ': no input captured yet; perform the documented sequence')
    end

    if frames >= TIMEOUT_FRAMES then
        finish('timed out after a partial two-player value-setup trace')
    end
end

if u32(VALUE_SETUP_UPDATE) ~= VALUE_SETUP_UPDATE_FIRST_WORD then
    finish(string.format(
        'MainMenu_UpdateValueSetup signature mismatch: expected 0x%08X '
            .. 'actual 0x%08X',
        VALUE_SETUP_UPDATE_FIRST_WORD,
        u32(VALUE_SETUP_UPDATE)
    ))
elseif mainMode() ~= VALUE_SETUP_MODE then
    finish(string.format(
        'main mode %d is not two-player value-setup mode %d',
        mainMode(),
        VALUE_SETUP_MODE
    ))
else
    listener_two_player_value_setup_option_reset =
        PCSX.Events.createEventListener(
            'ExecutionFlow::Reset',
            function()
                local ok, err = pcall(function()
                    finish('reset observed; rerun from two-player value setup')
                end)
                if not ok then
                    finish('reset callback error: ' .. tostring(err))
                end
            end
        )

    listener_two_player_value_setup_option =
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
        .. ': retail value-setup signature confirmed; perform the nine inputs')
end
