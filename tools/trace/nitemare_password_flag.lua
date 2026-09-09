-- nitemare_password_flag.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md records an unread branch in Nitemare's win text
--   that tests flag 0x5F7. The shared flag encoding proves that this is
--   CAMPAIGN_FLAG_PASSWORD_USED_BASE + 503: the password-used bit for Turtle
--   Raccoon. Static code does not establish what changes on screen when that
--   bit is clear or set during the Nitemare dialogue.
--
--   This trace records every read or write of flag 0x5F7, including the call
--   site, modifier, live bit, predicted test result, campaign scene, opponent
--   and main mode. Human context supplies the exact Nitemare text around the
--   test. Runs from both a used-password and unused-password save can settle
--   the branch's visible effect without modifying emulated memory.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec.
--   3. Load a save before Nitemare. Know whether Turtle Raccoon's password
--      (17441953, card 503) has already been bought on that save.
--   4. Paste this script before defeating Nitemare and confirm that both
--      breakpoints are installed.
--   5. Advance through Nitemare's win dialogue and note the exact text shown
--      immediately before and after the script reports a target test.
--   6. Continue until credits begin, or wait for the post-test timeout, then
--      copy the whole document into
--      tools/trace/result/nitemare_password_flag.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   State whether Turtle Raccoon's password had been bought on this save,
--   quote or precisely summarize the text immediately before and after the
--   target-test notice, and say whether credits began normally.

local ffi = require('ffi')

local SCRIPT_NAME = 'nitemare_password_flag'
local TEST_HELPER = 0x8002cca8
local UPDATE_HELPER = 0x8002cce4
local MAIN_MODE = 0x8009b26c
local CAMPAIGN_SCENE = 0x8009b27a
local OPPONENT_ID = 0x8009b361
local FLAG_BYTE = 0x801d06d6
local TARGET_FLAG = 0x5f7
local FLAG_ID_MODULUS = 0x800
local CLEAR_MODIFIER = 0x8000
local FLAG_MASK = 0x01
local CREDITS_MODE = 15
local MAX_EVENTS = 32
local NO_HIT_WARNING_FRAMES = 600
local POST_TEST_TIMEOUT_FRAMES = 3600
local TIMEOUT_FRAMES = 72000

local mem = PCSX.getMemPtr()

local function phys(addr)
    return addr - 0x80000000
end

local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end

local function normalize32(value)
    return value % 0x100000000
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function flagId(raw)
    return normalize32(raw) % FLAG_ID_MODULUS
end

local function hasClearModifier(raw)
    return math.floor(normalize32(raw) / CLEAR_MODIFIER) % 2 == 1
end

local function flagSet()
    return u8(FLAG_BYTE) % 2 == 1
end

local function callSite(regs)
    return normalize32(tonumber(regs.GPR.n.ra) - 8)
end

local function predictedReturn(raw, isSet)
    if hasClearModifier(raw) then
        return isSet and 0 or 1
    end
    return isSet and FLAG_MASK or 0
end

local lines = {}
local pendingWrites = {}
local frames = 0
local eventCount = 0
local testCount = 0
local writeCount = 0
local firstTestFrame = nil
local callbackError = nil
local eventLimitReached = false
local noHitWarningPrinted = false
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function disableBreakpoint(handle)
    if handle ~= nil then
        pcall(function()
            handle:disable()
        end)
    end
end

local function record(kind, raw, detail)
    if eventCount >= MAX_EVENTS then
        eventLimitReached = true
        return nil
    end

    eventCount = eventCount + 1
    emit(string.format(
        'event=%02d frame=%06d kind=%s callsite=0x%08X '
            .. 'raw_arg=0x%08X inverted=%s flag_set=%s '
            .. 'mode=%d scene_index=0x%02X opponent_id=%d %s',
        eventCount,
        frames,
        kind,
        detail.callsite,
        normalize32(raw),
        tostring(hasClearModifier(raw)),
        tostring(detail.flagSet),
        mainMode(),
        u8(CAMPAIGN_SCENE),
        u8(OPPONENT_ID),
        detail.suffix
    ))
    return eventCount
end

local function onTest()
    local regs = PCSX.getRegisters()
    local raw = normalize32(tonumber(regs.GPR.n.a0))

    if flagId(raw) ~= TARGET_FLAG then
        return
    end

    local isSet = flagSet()
    testCount = testCount + 1
    if firstTestFrame == nil then
        firstTestFrame = frames
    end
    record('test', raw, {
        callsite = callSite(regs),
        flagSet = isSet,
        suffix = string.format(
            'predicted_return=0x%08X',
            predictedReturn(raw, isSet)
        ),
    })
    print(string.format(
        '%s: target test %d saw flag_set=%s; note the surrounding text',
        SCRIPT_NAME,
        testCount,
        tostring(isSet)
    ))
end

local function onUpdate()
    local regs = PCSX.getRegisters()
    local raw = normalize32(tonumber(regs.GPR.n.a0))

    if flagId(raw) ~= TARGET_FLAG then
        return
    end

    local before = flagSet()
    local operation = hasClearModifier(raw) and 'clear' or 'set'
    writeCount = writeCount + 1
    local event = record('write_entry', raw, {
        callsite = callSite(regs),
        flagSet = before,
        suffix = 'operation=' .. operation,
    })
    if event ~= nil then
        pendingWrites[#pendingWrites + 1] = {
            event = event,
            operation = operation,
        }
    end
    print(string.format(
        '%s: target write %s entered; current flag_set=%s',
        SCRIPT_NAME,
        operation,
        tostring(before)
    ))
end

local function flushPendingWrites()
    if #pendingWrites == 0 then
        return
    end

    local after = flagSet()
    for _, pending in ipairs(pendingWrites) do
        emit(string.format(
            'post_write event=%02d frame=%06d operation=%s flag_set=%s',
            pending.event,
            frames,
            pending.operation,
            tostring(after)
        ))
    end
    pendingWrites = {}
end

local function finish(reason)
    if done then
        return
    end
    done = true

    disableBreakpoint(breakpoint_nitemare_password_flag_test)
    disableBreakpoint(breakpoint_nitemare_password_flag_update)

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<state whether Turtle Raccoon password 17441953 had been bought on')
    print(' this save; quote or precisely summarize the Nitemare text immediately')
    print(' before and after the target-test notice; say whether credits began>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: tests=%d writes=%d events=%d final_flag_set=%s '
            .. 'mode=%d scene_index=0x%02X opponent_id=%d',
        testCount,
        writeCount,
        eventCount,
        tostring(flagSet()),
        mainMode(),
        u8(CAMPAIGN_SCENE),
        u8(OPPONENT_ID)
    ))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function poll()
    if done then
        return
    end

    frames = frames + 1

    if callbackError ~= nil then
        finish('breakpoint callback error: ' .. callbackError)
        return
    end

    flushPendingWrites()

    if eventLimitReached then
        finish('maximum target event count reached; partial trace follows')
        return
    end

    if testCount > 0 and mainMode() == CREDITS_MODE then
        finish('target test captured and credits mode began')
        return
    end

    if firstTestFrame ~= nil
        and frames - firstTestFrame >= POST_TEST_TIMEOUT_FRAMES then
        finish('target test captured; post-test observation window elapsed')
        return
    end

    if not noHitWarningPrinted
        and frames >= NO_HIT_WARNING_FRAMES
        and testCount == 0 then
        noHitWarningPrinted = true
        print(SCRIPT_NAME
            .. ': no target test yet; confirm interpreter CPU and continue '
            .. 'through Nitemare win text')
    end

    if frames >= TIMEOUT_FRAMES then
        finish('timed out without a flag 0x5F7 test; use interpreter CPU')
    end
end

breakpoint_nitemare_password_flag_test = PCSX.addBreakpoint(
    TEST_HELPER,
    'Exec',
    4,
    'Trace tests of Nitemare password flag 0x5F7',
    function()
        local ok, err = pcall(onTest)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

breakpoint_nitemare_password_flag_update = PCSX.addBreakpoint(
    UPDATE_HELPER,
    'Exec',
    4,
    'Trace writes of Nitemare password flag 0x5F7',
    function()
        local ok, err = pcall(onUpdate)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_nitemare_password_flag_reset =
    PCSX.Events.createEventListener(
        'ExecutionFlow::Reset',
        function()
            local ok, err = pcall(function()
                finish('reset observed; rerun before the Nitemare win text')
            end)
            if not ok then
                callbackError = tostring(err)
            end
        end
    )

listener_nitemare_password_flag =
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

print(SCRIPT_NAME .. ': test and update breakpoints installed')
print(SCRIPT_NAME .. ': armed; continue through Nitemare win text')
