-- duel_result_outro_skip.lua
--
-- WHAT THIS ANSWERS
--   Matching Duel_ResultRuntime phase 3 seeds a 0x258 countdown and waits
--   while the sound path remains busy. A newly pressed Circle, Cross, or
--   Square bypasses that wait and sets the 0x40 departure flag, retargeting
--   the seven result sprites. Static code does not establish the visible
--   timing, whether the sound continues, or what the player sees change.
--
--   This trace records phase-3 entry, timer/state at each pressed input, the
--   first departure-flag observation, and the following phase change. Human
--   context compares a Triangle control with one Circle press.
--
-- HOW TO RUN
--   1. Win an ordinary one-player duel and paste this script before the result
--      outro finishes. No breakpoint or interpreter CPU is required.
--   2. Wait for "phase 3 observed". Tap Triangle once and note any effect.
--   3. After a short pause, tap Circle once and note the visible animation,
--      result-page timing, and whether music or sound continues.
--   4. If the outro departs automatically before Circle, do not interfere;
--      describe exactly what was visible when the automatic departure began.
--   5. Copy the whole document into
--      tools/trace/result/duel_result_outro_skip.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Name the duel/result and visible result page. State what Triangle did,
--   whether Circle visibly shortened the wait or changed the confetti/sprites,
--   whether audio continued, and whether departure happened before the input.

local ffi = require('ffi')
local bit = require('bit')

local SCRIPT_NAME = 'duel_result_outro_skip'
local MAIN_MODE = 0x8009b26c
local RESULT_STATE = 0x8009b174
local RESULT_TIMER = 0x8009b1d0
local DUEL_STATE = 0x8009b23a
local OPPONENT_ID = 0x8009b361
local PAD1_PRESSED = 0x8009b398
local DUEL_MODE = 3
local OUTRO_PHASE = 3
local DEPARTURE_FLAG = 0x40
local MAX_INPUTS = 8
local NO_PHASE_WARNING_FRAMES = 600
local PHASE_WAIT_TIMEOUT_FRAMES = 36000
local PHASE_OBSERVE_TIMEOUT_FRAMES = 3600
local POST_DEPARTURE_FRAMES = 120

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
    local state = u8(RESULT_STATE)
    return {
        modeRaw = u8(MAIN_MODE),
        mode = mainMode(),
        state = state,
        phase = state % 16,
        timer = u16(RESULT_TIMER),
        duelState = u16(DUEL_STATE),
        opponentID = s8(OPPONENT_ID),
        pressed = u16(PAD1_PRESSED),
    }
end

local function snapshotText(prefix, frame, value)
    return string.format(
        '%s frame=%06d mode_raw=0x%02X mode=%d result_state=0x%02X '
            .. 'phase=%d timer=%d duel_state=0x%04X opponent_id=%d '
            .. 'pressed=0x%04X',
        prefix,
        frame,
        value.modeRaw,
        value.mode,
        value.state,
        value.phase,
        value.timer,
        value.duelState,
        value.opponentID,
        value.pressed
    )
end

local lines = {}
local frames = 0
local phaseObserved = false
local phaseStartFrame = nil
local phaseStartTimer = nil
local departureFrame = nil
local inputCount = 0
local inputLatched = false
local lastInput = 0
local lastState = nil
local warned = false
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
    print('<name the duel and visible result page; describe Triangle, Circle,')
    print(' confetti/sprite motion, page timing, and audio; say whether the')
    print(' departure began before Circle rather than inferring from numbers>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: phase_observed=%s phase_start_timer=%s inputs=%d '
            .. 'departure=%s mode=%d phase=%d timer=%d',
        tostring(phaseObserved),
        tostring(phaseStartTimer),
        inputCount,
        tostring(departureFrame ~= nil),
        final.mode,
        final.phase,
        final.timer
    ))
    print(snapshotText('baseline', 0, baseline))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function observePhase(value)
    phaseObserved = true
    phaseStartFrame = frames
    phaseStartTimer = value.timer
    lastState = value.state
    emit(snapshotText('phase_start', frames, value))
    print(SCRIPT_NAME
        .. ': phase 3 observed; tap Triangle, pause, then tap Circle')

    if bit.band(value.state, DEPARTURE_FLAG) ~= 0 then
        departureFrame = frames
        emit(string.format(
            'departure frame=%06d timer=%d last_input=0x%04X '
                .. 'already_set_at_arm=true',
            frames,
            value.timer,
            lastInput
        ))
    end
end

local function recordInput(value)
    if inputCount >= MAX_INPUTS then
        finish('maximum input count reached; partial trace follows')
        return
    end
    inputCount = inputCount + 1
    lastInput = value.pressed
    emit(snapshotText(
        string.format('input=%02d', inputCount),
        frames,
        value
    ))
    print(string.format(
        '%s: input %d captured at timer %d',
        SCRIPT_NAME,
        inputCount,
        value.timer
    ))
end

local function poll()
    if done then
        return
    end
    frames = frames + 1

    local value = snapshot()
    if value.mode ~= DUEL_MODE then
        if phaseObserved then
            finish('left duel mode after phase 3 was observed')
        else
            finish('left duel mode before result phase 3')
        end
        return
    end

    if not phaseObserved then
        if value.phase == OUTRO_PHASE then
            observePhase(value)
        elseif not warned and frames >= NO_PHASE_WARNING_FRAMES then
            warned = true
            print(SCRIPT_NAME
                .. ': result phase 3 not observed yet; remain in the duel '
                .. 'through the result outro')
        end
        if frames >= PHASE_WAIT_TIMEOUT_FRAMES then
            finish('timed out before result phase 3')
        end
        return
    end

    if value.pressed ~= 0 and not inputLatched then
        inputLatched = true
        recordInput(value)
        if done then
            return
        end
    elseif value.pressed == 0 then
        inputLatched = false
    end

    if value.state ~= lastState then
        emit(snapshotText('state_change', frames, value))
        lastState = value.state
    end
    if departureFrame == nil
        and bit.band(value.state, DEPARTURE_FLAG) ~= 0 then
        departureFrame = frames
        emit(string.format(
            'departure frame=%06d timer=%d last_input=0x%04X '
                .. 'already_set_at_arm=false',
            frames,
            value.timer,
            lastInput
        ))
        print(SCRIPT_NAME
            .. ': departure flag observed; record the visible change')
    end

    if value.phase ~= OUTRO_PHASE then
        finish('result outro left phase 3')
    elseif departureFrame ~= nil
        and frames - departureFrame >= POST_DEPARTURE_FRAMES then
        finish('captured departure flag and follow-up window')
    elseif frames - phaseStartFrame >= PHASE_OBSERVE_TIMEOUT_FRAMES then
        finish('phase 3 observation timed out before departure')
    end
end

if baseline.mode ~= DUEL_MODE then
    finish(string.format(
        'main mode %d is not duel mode %d',
        baseline.mode,
        DUEL_MODE
    ))
else
    if baseline.phase == OUTRO_PHASE then
        observePhase(baseline)
    end
    listener_duel_result_outro_skip_reset =
        PCSX.Events.createEventListener(
            'ExecutionFlow::Reset',
            function()
                local ok, err = pcall(function()
                    finish('reset observed; rerun from the duel result')
                end)
                if not ok then
                    finish('reset callback error: ' .. tostring(err))
                end
            end
        )
    listener_duel_result_outro_skip =
        PCSX.Events.createEventListener(
            'GPU::Vsync',
            function()
                local ok, err = pcall(poll)
                if not ok then
                    finish('script error: ' .. tostring(err))
                end
            end
        )
    if not phaseObserved then
        print(SCRIPT_NAME
            .. ': armed; waiting for duel result phase 3')
    end
end
