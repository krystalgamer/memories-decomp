-- password_input_map.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md describes the Password shop but leaves its
--   physical button map among the unverified per-screen controls. Matching
--   shop.c reads held horizontal directions, repeated vertical directions,
--   newly pressed Circle and newly pressed Cross, but static masks alone are
--   not a runtime observation of which visible action each physical input
--   produces.
--
--   This trace captures eight isolated input samples with their raw held,
--   repeat and pressed masks plus the Password state, digit index, eight
--   digits and main mode before and after handling. Human context names the
--   physical button and visible response, including ignored controls.
--
-- HOW TO RUN
--   1. Enter the Password screen with all digits zero.
--   2. Paste this script. It requires the retail Password overlay signature
--      and main mode 10; no interpreter CPU or breakpoint is required.
--   3. Tap one input at a time, waiting for "sample settled" before the next:
--      Right, Left, Up, Down, Triangle, Start, Cross, then Circle.
--   4. Keep the all-zero password for Cross so the invalid-password behavior
--      is isolated. Circle should be last because it leaves the screen.
--   5. Copy the whole document into
--      tools/trace/result/password_input_map.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For each sample number, name the physical button and describe the visible
--   digit/cursor/message response. Confirm Cross used 00000000 and Circle was
--   pressed last.

local ffi = require('ffi')

local SCRIPT_NAME = 'password_input_map'
local PASSWORD_UPDATE = 0x8016a37c
local PASSWORD_UPDATE_FIRST_WORD = 0x27bdffe0
local MAIN_MODE = 0x8009b26c
local PAD1_REPEAT = 0x8009b394
local PAD1_PRESSED = 0x8009b398
local PAD1_HELD = 0x8009b3a4
local PASSWORD_DIGITS = 0x8016d410
local PASSWORD_STATE = 0x8016d424
local PASSWORD_DIGIT_INDEX = 0x8016d428
local PASSWORD_MODE = 10
local TARGET_SAMPLES = 8
local MAX_SAMPLES = 12
local SETTLE_FRAMES = 2
local QUIET_FRAMES = 120
local TIMEOUT_FRAMES = 36000

local mem = PCSX.getMemPtr()

local function phys(addr)
    return addr - 0x80000000
end

local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end

local function u16(addr)
    return u8(addr) + u8(addr + 1) * 0x100
end

local function u32(addr)
    return u16(addr) + u16(addr + 2) * 0x10000
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function digitsText()
    local digits = {}
    for index = 0, 7 do
        digits[#digits + 1] = tostring(u8(PASSWORD_DIGITS + index))
    end
    return table.concat(digits)
end

local function snapshot()
    return {
        modeRaw = u8(MAIN_MODE),
        mode = mainMode(),
        state = u16(PASSWORD_STATE),
        index = u32(PASSWORD_DIGIT_INDEX),
        digits = digitsText(),
    }
end

local function snapshotText(prefix, frame, value)
    return string.format(
        '%s frame=%06d mode_raw=0x%02X mode=%d state=0x%04X '
            .. 'state_low=%d digit_index=%d digits=%s',
        prefix,
        frame,
        value.modeRaw,
        value.mode,
        value.state,
        value.state % 32,
        value.index,
        value.digits
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
    print('<for every sample number, name the physical button and visible')
    print(' cursor/digit/message response; confirm Cross used 00000000 and')
    print(' Circle was last>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: samples=%d signature=0x%08X mode=%d state=0x%04X '
            .. 'digit_index=%d digits=%s',
        sampleCount,
        u32(PASSWORD_UPDATE),
        mainMode(),
        u16(PASSWORD_STATE),
        u32(PASSWORD_DIGIT_INDEX),
        digitsText()
    ))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function beginSample(held, repeatMask, pressed)
    sampleCount = sampleCount + 1
    pending = {
        number = sampleCount,
        held = held,
        repeatMask = repeatMask,
        pressed = pressed,
        before = snapshot(),
        remaining = SETTLE_FRAMES,
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
        pending.before
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
        'sample_end=%02d frame=%06d reason=%s index_delta=%d '
            .. 'state_changed=%s digits_changed=%s mode_changed=%s',
        current.number,
        frames,
        reason,
        after.index - current.before.index,
        tostring(after.state ~= current.before.state),
        tostring(after.digits ~= current.before.digits),
        tostring(after.mode ~= current.before.mode)
    ))
    pending = nil
    print(string.format(
        '%s: sample %d settled; perform the next isolated input',
        SCRIPT_NAME,
        current.number
    ))
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
        if mainMode() ~= PASSWORD_MODE then
            completeSample('Password mode changed')
            finish('captured input that left the Password screen')
            return
        end
        pending.remaining = pending.remaining - 1
        if pending.remaining <= 0 then
            completeSample('settle window elapsed')
        end
    elseif mainMode() ~= PASSWORD_MODE then
        finish('left Password mode after captured input')
        return
    end

    if pending == nil and not done then
        local held = u16(PAD1_HELD)
        local repeatMask = u16(PAD1_REPEAT)
        local pressed = u16(PAD1_PRESSED)
        local active = held ~= 0 or repeatMask ~= 0 or pressed ~= 0

        if active and not inputLatched then
            inputLatched = true
            beginSample(held, repeatMask, pressed)
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
            finish('captured eight inputs followed by quiet time')
            return
        end
    end

    if frames >= TIMEOUT_FRAMES then
        finish('timed out after a partial Password input trace')
    end
end

if u32(PASSWORD_UPDATE) ~= PASSWORD_UPDATE_FIRST_WORD then
    print(string.format(
        '%s: Password overlay signature mismatch expected=0x%08X actual=0x%08X',
        SCRIPT_NAME,
        PASSWORD_UPDATE_FIRST_WORD,
        u32(PASSWORD_UPDATE)
    ))
    return
end

if mainMode() ~= PASSWORD_MODE then
    print(string.format(
        '%s: main mode %d is not Password mode %d',
        SCRIPT_NAME,
        mainMode(),
        PASSWORD_MODE
    ))
    return
end

listener_password_input_map_reset =
    PCSX.Events.createEventListener(
        'ExecutionFlow::Reset',
        function()
            local ok, err = pcall(function()
                finish('reset observed; rerun after entering Password')
            end)
            if not ok then
                callbackError = tostring(err)
            end
        end
    )

listener_password_input_map =
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

print(SCRIPT_NAME .. ': retail Password overlay signature confirmed')
print(SCRIPT_NAME .. ': armed; perform Right, Left, Up, Down, Triangle, '
    .. 'Start, Cross, Circle')
