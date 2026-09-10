-- options_input_map.lua
--
-- WHAT THIS ANSWERS
--   Static analysis establishes stereo/mono on selection 0 and finds no
--   post-initialization writer for the selection byte anywhere in the retail
--   executable. The layout and update code retain two nonzero positions and
--   confirm states, so this trace checks whether ordinary directional input is
--   visibly inert and records any labels that remain on screen.
--
--   This trace captures the selection byte, small state machine, working and
--   stored output types, raw main mode, and each physical pressed mask.
--   It also records intermediate state changes before settlement so the
--   one-frame state-3 path is not hidden by the final snapshot.
--
-- HOW TO RUN
--   1. Enter Options (main mode 11), then paste this script. No interpreter
--      CPU or breakpoint is required.
--   2. Tap one input at a time and wait for "sample settled" before the next:
--      Right, Left, Down, Down, Cross, Up, Up, Circle.
--   3. For each sample, note the highlighted row, visible label/value, and any
--      resulting animation, sound, submenu, or no-op. If the screen exits
--      earlier than expected, record exactly which input caused it.
--   4. Copy the whole document into
--      tools/trace/result/options_input_map.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Name every visible row label and which side of the sound row is mono versus
--   stereo. State whether either Down press moved the highlight despite the
--   static writer search, what Cross did, and what Circle did.

local ffi = require('ffi')

local SCRIPT_NAME = 'options_input_map'
local MAIN_MODE = 0x8009b26c
local OPTIONS_STATE = 0x8009b37c
local OPTIONS_OUTPUT_TYPE = 0x8009b37d
local OPTIONS_SELECTION = 0x8009b384
local PAD1_PRESSED = 0x8009b398
local STORED_OUTPUT_TYPE = 0x8009b408
local OPTIONS_MODE = 11
local TARGET_SAMPLES = 8
local MAX_SAMPLES = 10
local MIN_SETTLE_FRAMES = 8
local STABLE_FRAMES = 8
local MAX_SETTLE_FRAMES = 120
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

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function snapshot()
    return {
        modeRaw = u8(MAIN_MODE),
        mode = mainMode(),
        state = u8(OPTIONS_STATE),
        stateLow = u8(OPTIONS_STATE) % 16,
        selection = s8(OPTIONS_SELECTION),
        outputType = s8(OPTIONS_OUTPUT_TYPE),
        storedOutputType = s8(STORED_OUTPUT_TYPE),
    }
end

local function snapshotsEqual(left, right)
    return left.modeRaw == right.modeRaw
        and left.state == right.state
        and left.selection == right.selection
        and left.outputType == right.outputType
        and left.storedOutputType == right.storedOutputType
end

local function snapshotText(prefix, frame, value)
    return string.format(
        '%s frame=%06d mode_raw=0x%02X mode=%d state=0x%02X '
            .. 'state_low=%d selection=%d output_type=%d stored_output_type=%d',
        prefix,
        frame,
        value.modeRaw,
        value.mode,
        value.state,
        value.stateLow,
        value.selection,
        value.outputType,
        value.storedOutputType
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
    print('<name every visible row and the mono/stereo sides; for every sample')
    print(' describe the highlighted row, visible value, animation/sound/result,')
    print(' and whether the screen exited; say whether Down/Up moved the')
    print(' highlight, and state what Cross and Circle did>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: samples=%d mode=%d state=0x%02X selection=%d '
            .. 'output_type=%d stored_output_type=%d',
        sampleCount,
        final.mode,
        final.state,
        final.selection,
        final.outputType,
        final.storedOutputType
    ))
    print(snapshotText('baseline', 0, baseline))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function beginSample(pressed)
    if sampleCount >= MAX_SAMPLES then
        finish('maximum sample count reached')
        return
    end

    local before = snapshot()
    sampleCount = sampleCount + 1
    pending = {
        number = sampleCount,
        pressed = pressed,
        before = before,
        last = before,
        stable = 0,
        age = 0,
        changes = 0,
    }
    quietFrames = 0
    emit(string.format(
        'sample_start=%02d frame=%06d pressed=0x%04X',
        pending.number,
        frames,
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
        'sample_end=%02d frame=%06d reason=%s changes=%d '
            .. 'selection=%d->%d state=0x%02X->0x%02X '
            .. 'output_type=%d->%d stored_output_type=%d->%d mode=%d->%d',
        current.number,
        frames,
        reason,
        current.changes,
        current.before.selection,
        after.selection,
        current.before.state,
        after.state,
        current.before.outputType,
        after.outputType,
        current.before.storedOutputType,
        after.storedOutputType,
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
    local pressed = u16(PAD1_PRESSED)

    pending.age = pending.age + 1
    if snapshotsEqual(pending.last, current) then
        pending.stable = pending.stable + 1
    else
        pending.changes = pending.changes + 1
        emit(snapshotText(
            string.format(
                'sample_change=%02d.%02d',
                pending.number,
                pending.changes
            ),
            frames,
            current
        ))
        pending.last = current
        pending.stable = 0
    end

    if current.mode ~= OPTIONS_MODE then
        completeSample('input left Options')
        finish('captured input that left Options')
    elseif pending.age >= MAX_SETTLE_FRAMES then
        completeSample('settlement timeout')
    elseif pressed == 0
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

    local pressed = u16(PAD1_PRESSED)
    if pressed ~= 0 and not inputLatched then
        inputLatched = true
        beginSample(pressed)
        return
    elseif pressed == 0 then
        inputLatched = false
    end

    if mainMode() ~= OPTIONS_MODE then
        if sampleCount == 0 then
            finish('left Options before any input was captured')
        else
            finish('left Options after captured inputs')
        end
        return
    end

    if sampleCount >= TARGET_SAMPLES then
        quietFrames = quietFrames + 1
        if quietFrames >= QUIET_FRAMES then
            finish('captured eight inputs followed by quiet time')
            return
        end
    elseif frames == NO_INPUT_WARNING_FRAMES and not warned then
        warned = true
        print(SCRIPT_NAME
            .. ': no input captured yet; perform the documented sequence')
    end

    if frames >= TIMEOUT_FRAMES then
        finish('timed out after a partial Options input trace')
    end
end

if mainMode() ~= OPTIONS_MODE then
    finish(string.format(
        'main mode %d is not Options mode %d',
        mainMode(),
        OPTIONS_MODE
    ))
else
    listener_options_input_map_reset =
        PCSX.Events.createEventListener(
            'ExecutionFlow::Reset',
            function()
                local ok, err = pcall(function()
                    finish('reset observed; rerun from Options')
                end)
                if not ok then
                    finish('reset callback error: ' .. tostring(err))
                end
            end
        )

    listener_options_input_map =
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
        .. ': armed; perform Right, Left, Down, Down, Cross, Up, Up, Circle')
end
