-- two_player_save_flow_state.lua
--
-- WHAT THIS ANSWERS
--   notes/fm-online.md records that the broader meaning of D_8009B3EA is
--   unresolved. Matching func_8003F8D4 uses its low nibble as the shared
--   two-save load/validation state, while func_8003FD14 sets raw state 0x0A
--   before checking both 40-card decks and func_80031000 enters main mode
--   0x10 after a successful two-player load.
--
--   This trace maps each raw/low D_8009B3EA transition to visible two-player
--   setup activity. It records the surrounding request step, dialog flags and
--   messages, I/O result, deck-error side, and physical button presses so
--   human context can distinguish prompts, card-slot loads, validation, and
--   the successful handoff.
--
-- HOW TO RUN
--   1. Start from the loaded main menu with two distinct valid saves, one on
--      each memory card, and enable this script before selecting 2P Duel.
--   2. Follow the ordinary successful load flow. Use Cross when asked to load
--      each card and do not trigger intentional card or deck errors.
--   3. For every printed transition, note the visible prompt, animation, or
--      setup screen and which physical controller/button caused it.
--   4. Continue until the starting-LP setup appears and the script finishes.
--   5. Copy the whole document into
--      tools/trace/result/two_player_save_flow_state.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm the two saves were distinct and both decks contained 40 cards.
--   For every transition number, describe the visible text/screen and any
--   button just pressed. Identify when card 1 finished, card 2 finished,
--   validation ran, and the starting-LP setup first appeared.

local ffi = require('ffi')

local SCRIPT_NAME = 'two_player_save_flow_state'
local MAIN_MODE = 0x8009b26c
local FLOW_OWNER_FLAGS = 0x8009b2eb
local FAILURE_MESSAGE = 0x8009b3c0
local DISPLAY_MESSAGE = 0x8009b3c6
local REQUEST_STEP = 0x8009b3de
local LOAD_STATE = 0x8009b3ea
local LOAD_INIT_FLAGS = 0x8009b3ed
local EFFECT_CHANNEL = 0x8009b3ee
local REQUEST_OUTCOME = 0x8009b3ef
local DIALOG_FLAGS = 0x8009b3fa
local IO_RESULT = 0x8009b450
local DECK_ERROR_SIDE = 0x801d5648
local PAD1_PRESSED = 0x8009b398
local PAD2_PRESSED = 0x8009b39a
local POST_LOAD_MODE = 0x10
local MAX_TRANSITIONS = 64
local EXIT_SETTLE_FRAMES = 120
local NO_CHANGE_WARNING_FRAMES = 600
local TIMEOUT_FRAMES = 72000

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

local function s32(addr)
    local value = u32(addr)
    if value >= 0x80000000 then
        return value - 0x100000000
    end
    return value
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function snapshot()
    local state = u8(LOAD_STATE)
    return {
        modeRaw = u8(MAIN_MODE),
        mode = mainMode(),
        ownerFlags = u8(FLOW_OWNER_FLAGS),
        failureMessage = u8(FAILURE_MESSAGE),
        displayMessage = u8(DISPLAY_MESSAGE),
        requestStep = u8(REQUEST_STEP),
        loadState = state,
        loadLow = state % 16,
        loadHigh = state - state % 16,
        initFlags = u8(LOAD_INIT_FLAGS),
        effectChannel = u8(EFFECT_CHANNEL),
        requestOutcome = u8(REQUEST_OUTCOME),
        dialogFlags = u16(DIALOG_FLAGS),
        ioResult = s32(IO_RESULT),
        deckErrorSide = s32(DECK_ERROR_SIDE),
        pad1Pressed = u16(PAD1_PRESSED),
        pad2Pressed = u16(PAD2_PRESSED),
    }
end

local function snapshotKey(value)
    return string.format(
        '%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%04X:%d:%d:%04X:%04X',
        value.modeRaw,
        value.ownerFlags,
        value.failureMessage,
        value.displayMessage,
        value.requestStep,
        value.loadState,
        value.initFlags,
        value.effectChannel,
        value.requestOutcome,
        value.dialogFlags,
        value.ioResult,
        value.deckErrorSide,
        value.pad1Pressed,
        value.pad2Pressed
    )
end

local function snapshotText(number, frame, reason, value)
    return string.format(
        'transition=%02d frame=%06d reason=%s mode_raw=0x%02X mode=%d '
            .. 'owner_flags=0x%02X load_raw=0x%02X load_low=%d '
            .. 'load_high=0x%02X init_flags=0x%02X '
            .. 'failure_message=0x%02X display_message=0x%02X '
            .. 'request_step=%d request_outcome=%d effect_channel=%d '
            .. 'dialog_flags=0x%04X io_result=%d deck_error_side=%d '
            .. 'pad1_pressed=0x%04X pad2_pressed=0x%04X',
        number,
        frame,
        reason,
        value.modeRaw,
        value.mode,
        value.ownerFlags,
        value.loadState,
        value.loadLow,
        value.loadHigh,
        value.initFlags,
        value.failureMessage,
        value.displayMessage,
        value.requestStep,
        value.requestOutcome,
        value.effectChannel,
        value.dialogFlags,
        value.ioResult,
        value.deckErrorSide,
        value.pad1Pressed,
        value.pad2Pressed
    )
end

local lines = {}
local seenStates = {}
local frames = 0
local transitions = 0
local quietFrames = 0
local enteredFlow = false
local warned = false
local done = false
local baseline = snapshot()
local last = baseline
local lastKey = snapshotKey(baseline)

local function emit(text)
    lines[#lines + 1] = text
end

local function seenStateText()
    local states = {}
    for state in pairs(seenStates) do
        states[#states + 1] = state
    end
    table.sort(states)

    local values = {}
    for _, state in ipairs(states) do
        values[#values + 1] = string.format('0x%02X', state)
    end
    if #values == 0 then
        return 'none'
    end
    return table.concat(values, ',')
end

local function finish(reason)
    if done then
        return
    end
    done = true

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm distinct saves and two 40-card decks; for every transition')
    print(' describe the visible prompt/screen and physical button, and identify')
    print(' card-1 completion, card-2 completion, validation, and LP setup entry>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: transitions=%d unique_raw_states=%s final_mode=%d '
            .. 'final_load_raw=0x%02X final_load_low=%d',
        transitions,
        seenStateText(),
        last.mode,
        last.loadState,
        last.loadLow
    ))
    emit(snapshotText(0, 0, 'baseline', baseline))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function flowActive(value)
    return value.ownerFlags >= 0x80
        or value.loadState ~= baseline.loadState
        or value.dialogFlags ~= 0
end

local function capture(reason, value)
    if transitions >= MAX_TRANSITIONS then
        finish('maximum transition count reached')
        return
    end

    transitions = transitions + 1
    last = value
    seenStates[value.loadState] = true
    emit(snapshotText(transitions, frames, reason, value))
    print(string.format(
        '%s: transition %d raw_state=0x%02X low_state=%d; note the visible step',
        SCRIPT_NAME,
        transitions,
        value.loadState,
        value.loadLow
    ))

    if transitions >= MAX_TRANSITIONS then
        finish('maximum transition count reached')
    end
end

local function poll()
    if done then
        return
    end

    frames = frames + 1
    local current = snapshot()
    local currentKey = snapshotKey(current)

    if not enteredFlow then
        if flowActive(current) then
            enteredFlow = true
            quietFrames = 0
            capture('flow_started', current)
            last = current
            lastKey = currentKey
        elseif frames == NO_CHANGE_WARNING_FRAMES and not warned then
            warned = true
            print(SCRIPT_NAME
                .. ': no flow observed yet; enter 2P Duel from the loaded menu')
        end
    elseif currentKey ~= lastKey then
        quietFrames = 0
        capture('state_changed', current)
        last = current
        lastKey = currentKey
    else
        quietFrames = quietFrames + 1
    end

    if done then
        return
    end

    if enteredFlow and current.mode == POST_LOAD_MODE then
        last = current
        finish('reached two-player post-load mode 16')
    elseif enteredFlow
        and current.ownerFlags < 0x80
        and current.dialogFlags == 0
        and quietFrames >= EXIT_SETTLE_FRAMES then
        last = current
        finish('two-save flow ended before post-load mode; partial trace')
    elseif frames >= TIMEOUT_FRAMES then
        last = current
        if enteredFlow then
            finish('timed out during the two-save flow; partial trace')
        else
            finish('timed out before the two-save flow was observed')
        end
    end
end

listener_two_player_save_flow_state_reset =
    PCSX.Events.createEventListener(
        'ExecutionFlow::Reset',
        function()
            local ok, err = pcall(function()
                finish('reset observed; rerun from the loaded main menu')
            end)
            if not ok then
                finish('reset callback error: ' .. tostring(err))
            end
        end
    )

listener_two_player_save_flow_state =
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
    .. ': armed; enter 2P Duel and complete the normal two-save load flow')
