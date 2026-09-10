-- game_over_input_map.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md currently says Game Over offers retry or return
--   to title. Matching code proves less: after the fade completes,
--   Circle/Cross/Square share one exit test, and D_8009B269 already holds the
--   value that determines the following frontend path. It does not establish
--   the visible choices, highlight behavior, or which destination the player
--   sees for a particular loss.
--
--   This trace samples non-action controls before one Circle exit. It records
--   the main mode, preselected destination, frontend request bytes, fade state,
--   Game Over object pointer, and every intermediate change. Human context
--   supplies the visible screen and the actual destination.
--
-- HOW TO RUN
--   1. Lose an ordinary campaign duel and wait until the Game Over screen and
--      fade are fully settled. Paste this script; no breakpoint is required.
--   2. Tap one input at a time and wait for "sample settled" before the next:
--      Up, Down, Start, Select, Triangle, then Circle.
--   3. For each input, note any highlight, animation, sound, text change, or
--      no-op. After Circle, state exactly which screen or duel state appeared.
--   4. Copy the whole document into
--      tools/trace/result/game_over_input_map.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Name the duel that was lost and whether it was an ordinary campaign loss.
--   Describe every visible Game Over label or option, any highlighted choice,
--   each input's effect, and Circle's destination. Do not call a destination
--   retry/title solely from the recorded numeric state.

local ffi = require('ffi')

local SCRIPT_NAME = 'game_over_input_map'
local MAIN_MODE = 0x8009b26c
local FRONTEND_REQUEST = 0x8009b268
local GAME_OVER_DESTINATION = 0x8009b269
local FRONTEND_MENU_ID = 0x8009b26d
local GAME_OVER_OBJECT = 0x8009b378
local PAD1_PRESSED = 0x8009b398
local FADE_STATE = 0x800e9ec8
local FADE_LEVEL = FADE_STATE + 4
local FADE_TARGET = FADE_STATE + 5
local FADE_FLAGS = FADE_STATE + 6
local FADE_STEP = FADE_STATE + 7
local GAME_OVER_MODE = 12
local TARGET_SAMPLES = 6
local MAX_SAMPLES = 8
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
        modeRaw = u8(MAIN_MODE),
        mode = mainMode(),
        frontendRequest = u8(FRONTEND_REQUEST),
        destination = u8(GAME_OVER_DESTINATION),
        menuID = u8(FRONTEND_MENU_ID),
        fadeLevel = u8(FADE_LEVEL),
        fadeTarget = u8(FADE_TARGET),
        fadeFlags = u8(FADE_FLAGS),
        fadeStep = u8(FADE_STEP),
        object = u32(GAME_OVER_OBJECT),
    }
end

local function snapshotsEqual(left, right)
    return left.modeRaw == right.modeRaw
        and left.frontendRequest == right.frontendRequest
        and left.destination == right.destination
        and left.menuID == right.menuID
        and left.fadeLevel == right.fadeLevel
        and left.fadeTarget == right.fadeTarget
        and left.fadeFlags == right.fadeFlags
        and left.fadeStep == right.fadeStep
        and left.object == right.object
end

local function snapshotText(prefix, frame, value)
    return string.format(
        '%s frame=%06d mode_raw=0x%02X mode=%d destination=%d '
            .. 'frontend_request=%d menu_id=%d fade_level=%d '
            .. 'fade_target=%d fade_flags=0x%02X fade_step=%d '
            .. 'object=0x%08X',
        prefix,
        frame,
        value.modeRaw,
        value.mode,
        value.destination,
        value.frontendRequest,
        value.menuID,
        value.fadeLevel,
        value.fadeTarget,
        value.fadeFlags,
        value.fadeStep,
        value.object
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
local lastObserved = baseline

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
    print('<name the lost duel and every visible Game Over label/option; for')
    print(' each sample describe highlight, animation, sound, text, or no-op;')
    print(" state Circle's exact destination without inferring it from numbers>")
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: samples=%d mode=%d destination=%d frontend_request=%d '
            .. 'menu_id=%d fade_flags=0x%02X',
        sampleCount,
        final.mode,
        final.destination,
        final.frontendRequest,
        final.menuID,
        final.fadeFlags
    ))
    print(snapshotText('baseline', 0, baseline))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function beginSample(pressed, before)
    if sampleCount >= MAX_SAMPLES then
        finish('maximum sample count reached')
        return
    end

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
            .. 'mode=%d->%d destination=%d->%d '
            .. 'frontend_request=%d->%d menu_id=%d->%d '
            .. 'fade=0x%02X/%d/%d->0x%02X/%d/%d',
        current.number,
        frames,
        reason,
        current.changes,
        current.before.mode,
        after.mode,
        current.before.destination,
        after.destination,
        current.before.frontendRequest,
        after.frontendRequest,
        current.before.menuID,
        after.menuID,
        current.before.fadeFlags,
        current.before.fadeLevel,
        current.before.fadeTarget,
        after.fadeFlags,
        after.fadeLevel,
        after.fadeTarget
    ))
    pending = nil
    lastObserved = after
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

    if current.mode ~= GAME_OVER_MODE then
        completeSample('input left Game Over')
        finish('captured input that left Game Over')
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

    local current = snapshot()
    local pressed = u16(PAD1_PRESSED)
    if current.mode ~= GAME_OVER_MODE then
        if pressed ~= 0 and not inputLatched then
            inputLatched = true
            beginSample(pressed, lastObserved)
            completeSample('input and mode transition observed together')
            finish('captured input that left Game Over')
        elseif sampleCount == 0 then
            finish('left Game Over before any input was captured')
        else
            finish('left Game Over after captured inputs')
        end
        return
    end

    lastObserved = current
    if pressed ~= 0 and not inputLatched then
        inputLatched = true
        beginSample(pressed, current)
        return
    elseif pressed == 0 then
        inputLatched = false
    end

    if sampleCount >= TARGET_SAMPLES then
        quietFrames = quietFrames + 1
        if quietFrames >= QUIET_FRAMES then
            finish('captured six inputs followed by quiet time')
            return
        end
    elseif sampleCount == 0
        and frames == NO_INPUT_WARNING_FRAMES
        and not warned then
        warned = true
        print(SCRIPT_NAME
            .. ': no input captured yet; wait for the fade, then perform '
            .. 'Up, Down, Start, Select, Triangle, Circle')
    end

    if frames >= TIMEOUT_FRAMES then
        finish('timed out after a partial Game Over input trace')
    end
end

if mainMode() ~= GAME_OVER_MODE then
    finish(string.format(
        'main mode %d is not Game Over mode %d',
        mainMode(),
        GAME_OVER_MODE
    ))
else
    listener_game_over_input_map_reset =
        PCSX.Events.createEventListener(
            'ExecutionFlow::Reset',
            function()
                local ok, err = pcall(function()
                    finish('reset observed; rerun from Game Over')
                end)
                if not ok then
                    finish('reset callback error: ' .. tostring(err))
                end
            end
        )

    listener_game_over_input_map =
        PCSX.Events.createEventListener(
            'GPU::Vsync',
            function()
                local ok, err = pcall(poll)
                if not ok then
                    finish('script error: ' .. tostring(err))
                end
            end
        )

    if baseline.fadeFlags % 0x100 >= 0x80 then
        print(SCRIPT_NAME
            .. ': armed while fade flag 0x80 is active; wait for it to clear')
    end
    print(SCRIPT_NAME
        .. ': armed; perform Up, Down, Start, Select, Triangle, then Circle')
end
