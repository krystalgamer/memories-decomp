-- record_dispatch_runtime_role.lua
--
-- WHAT THIS ANSWERS
--   func_8005F91C manages up to ten 0x28-byte records through three modes:
--   mode 0 resets the list and may append, mode 1 appends, and mode 2 processes
--   every queued record. Positional-sound callers reach it, but nearby model
--   and effect state makes a sound-only name premature.
--
--   This trace records each mode, caller, input pair, and the complete queued
--   records presented to mode 2. Human context establishes whether each
--   sequence coincides with an audible sound, a visible model effect, or both.
--   That evidence can decide whether func_8005F91C should receive a sound
--   queue/dispatcher name or retain a broader model/effect role.
--
--   A first run produced no dispatcher hits. This version also probes the
--   four matching-C caller entries, so it can distinguish an action that
--   never reaches this subsystem from a dispatcher-breakpoint/setup failure.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec.
--   3. Debug -> Lua editor, paste this file, and let it auto-run.
--   4. Perform four isolated actions likely to create positional effects,
--      waiting at least three seconds between them. Good controls include a
--      card placement, an attack, a spell/trap animation, and a menu action
--      with a clearly audible sound but no model animation.
--   5. Note what was visible and audible when each numbered sequence starts
--      and when the script reports that its queued records were processed.
--      Caller-probe messages are useful too: record the action that caused
--      each address to appear.
--   6. Copy the whole document into
--      tools/trace/result/record_dispatch_runtime_role.txt and fill in the
--      context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For every sequence number, state the screen, exact action, visible model
--   or particle effect, audible sound, and whether either began before or
--   after the trace reported mode 2.

local ffi = require('ffi')

local SCRIPT_NAME = 'record_dispatch_runtime_role'
local DISPATCH = 0x8005f91c
local CALLERS = {0x8005d994, 0x8005f714, 0x8005f7b0, 0x8005f828}
local MAIN_MODE = 0x8009b26c
local QUEUE_COUNT = 0x8009b078
local QUEUE_ACTIVE = 0x8009b079
local QUEUE_BASE = 0x800f5788
local QUEUE_CAPACITY = 10
local QUEUE_RECORD_SIZE = 0x28
local TARGET_SEQUENCES = 4
local MAX_HITS = 96
local NO_HIT_WARNING_FRAMES = 600
local MIN_CAPTURE_FRAMES = 1800
local QUIET_FRAMES = 180
local TIMEOUT_FRAMES = 72000

local mem = PCSX.getMemPtr()

local function phys(addr)
    return addr - 0x80000000
end

local function normalize32(value)
    return value % 0x100000000
end

local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
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

local function signed32(value)
    value = normalize32(value)
    if value >= 0x80000000 then
        return value - 0x100000000
    end
    return value
end

local function isRam(addr, size)
    addr = normalize32(addr)
    return addr >= 0x80000000
        and addr < 0x80200000
        and size >= 0
        and addr + size <= 0x80200000
end

local function recordText(addr)
    addr = normalize32(addr)
    if addr == 0 then
        return 'null'
    end
    if not isRam(addr, 8) then
        return string.format('invalid(0x%08X)', addr)
    end
    return string.format(
        '[%d,%d,%d,%d]/[%04X,%04X,%04X,%04X]',
        s16(addr), s16(addr + 2), s16(addr + 4), s16(addr + 6),
        u16(addr), u16(addr + 2), u16(addr + 4), u16(addr + 6)
    )
end

local function modeName(mode)
    if mode == 0 then
        return 'reset_and_append'
    end
    if mode == 1 then
        return 'append'
    end
    if mode == 2 then
        return 'process_queue'
    end
    return 'ignored'
end

local lines = {}
local frames = 0
local quietFrames = 0
local hits = 0
local sequence = 0
local completedSequences = 0
local sequenceOpen = false
local modeCounts = {[0] = 0, [1] = 0, [2] = 0}
local callerHits = 0
local seenCallers = {}
local firstHitFrame = nil
local callbackError = nil
local hitLimitReached = false
local noHitWarningPrinted = false
local armed = false
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function callSite(regs)
    return normalize32(tonumber(regs.GPR.n.ra) - 8)
end

local function onCaller(address)
    if done or callbackError ~= nil then
        return
    end

    address = normalize32(address)
    callerHits = callerHits + 1
    if not seenCallers[address] then
        seenCallers[address] = true
        emit(string.format(
            'caller_probe frame=%06d main_mode=0x%02X address=0x%08X',
            frames, u8(MAIN_MODE), address
        ))
        print(string.format(
            '%s: caller 0x%08X reached; note the action and presentation',
            SCRIPT_NAME, address
        ))
    end
end

local function dumpQueue(sequenceNumber, count)
    local boundedCount = math.min(count, QUEUE_CAPACITY)
    for index = 0, boundedCount - 1 do
        local record = QUEUE_BASE + index * QUEUE_RECORD_SIZE
        emit(string.format(
            'queue sequence=%d slot=%d left=%s right=%s '
            .. 'arg3=%d field22=%d field24=%d byte26=0x%02X',
            sequenceNumber,
            index,
            recordText(record),
            recordText(record + 8),
            s16(record + 0x20),
            s16(record + 0x22),
            s16(record + 0x24),
            u8(record + 0x26)
        ))
    end
end

local function onDispatch()
    if done or hitLimitReached or callbackError ~= nil then
        return
    end

    local regs = PCSX.getRegisters()
    local dispatchMode = signed32(tonumber(regs.GPR.n.a0))
    local left = normalize32(tonumber(regs.GPR.n.a1))
    local right = normalize32(tonumber(regs.GPR.n.a2))
    local arg3 = normalize32(tonumber(regs.GPR.n.a3))
    local count = u8(QUEUE_COUNT)

    hits = hits + 1
    quietFrames = 0
    if not armed then
        firstHitFrame = frames
    end
    armed = true

    if dispatchMode == 0 or not sequenceOpen then
        sequence = sequence + 1
        sequenceOpen = true
        print(string.format(
            '%s: sequence %d started; note the visible action and sound',
            SCRIPT_NAME, sequence
        ))
    end

    if modeCounts[dispatchMode] ~= nil then
        modeCounts[dispatchMode] = modeCounts[dispatchMode] + 1
    end

    emit(string.format(
        'hit=%03d frame=%06d sequence=%d main_mode=0x%02X '
        .. 'callsite=0x%08X dispatcher_mode=%d(%s) '
        .. 'queue_count_before=%d queue_active=%d '
        .. 'arg3=%d/0x%08X left_ptr=0x%08X left=%s '
        .. 'right_ptr=0x%08X right=%s',
        hits,
        frames,
        sequence,
        u8(MAIN_MODE),
        callSite(regs),
        dispatchMode,
        modeName(dispatchMode),
        count,
        u8(QUEUE_ACTIVE),
        signed32(arg3),
        arg3,
        left,
        recordText(left),
        right,
        recordText(right)
    ))

    if dispatchMode == 2 then
        dumpQueue(sequence, count)
        completedSequences = completedSequences + 1
        sequenceOpen = false
        print(string.format(
            '%s: sequence %d processed %d queued record(s)',
            SCRIPT_NAME, sequence, math.min(count, QUEUE_CAPACITY)
        ))
    end

    if hits >= MAX_HITS then
        hitLimitReached = true
    end
end

local function finish(reason)
    if done then
        return
    end
    done = true

    if breakpoint_record_dispatch_runtime_role ~= nil then
        breakpoint_record_dispatch_runtime_role:disable()
    end
    if breakpoints_record_dispatch_callers ~= nil then
        for _, breakpoint in ipairs(breakpoints_record_dispatch_callers) do
            breakpoint:disable()
        end
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<for every sequence, state the screen and exact action; describe')
    print(' visible model/particle effects and audible sounds; say whether each')
    print(' began before or after the corresponding mode-2 message>')
    print('<also identify the action behind each caller-probe address>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: hits=%d caller_hits=%d sequences=%d completed=%d '
        .. 'modes_0_1_2=%d/%d/%d',
        hits,
        callerHits,
        sequence,
        completedSequences,
        modeCounts[0],
        modeCounts[1],
        modeCounts[2]
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

    if hitLimitReached then
        finish('maximum hit count reached; partial trace follows')
        return
    end

    if not noHitWarningPrinted
        and frames >= NO_HIT_WARNING_FRAMES
        and hits == 0 then
        noHitWarningPrinted = true
        if callerHits == 0 then
            print(SCRIPT_NAME
                .. ': no caller or dispatcher hits; confirm interpreter CPU '
                .. 'and trigger a model/particle action')
        else
            print(SCRIPT_NAME
                .. ': caller probes fired but dispatcher did not; keep '
                .. 'running and record which caller addresses appeared')
        end
    end

    if armed then
        quietFrames = quietFrames + 1
        if completedSequences >= TARGET_SEQUENCES
            and frames - firstHitFrame >= MIN_CAPTURE_FRAMES
            and quietFrames >= QUIET_FRAMES then
            finish('captured four processed sequences followed by quiet time')
            return
        end
    end

    if frames >= TIMEOUT_FRAMES then
        if armed then
            finish('timed out after partial dispatcher activity')
        elseif callerHits > 0 then
            finish('timed out after caller activity without a dispatcher hit')
        else
            finish('timed out without a dispatcher hit; use interpreter CPU')
        end
    end
end

breakpoint_record_dispatch_runtime_role = PCSX.addBreakpoint(
    DISPATCH,
    'Exec',
    4,
    'Trace the three-mode record dispatcher',
    function()
        local ok, err = pcall(onDispatch)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

breakpoints_record_dispatch_callers = {}
for _, address in ipairs(CALLERS) do
    breakpoints_record_dispatch_callers[#breakpoints_record_dispatch_callers + 1] =
        PCSX.addBreakpoint(
            address,
            'Exec',
            4,
            'Trace a record-dispatch caller',
            function(hitAddress)
                local ok, err = pcall(onCaller, hitAddress)
                if not ok then
                    callbackError = tostring(err)
                end
            end
        )
end

listener_record_dispatch_runtime_role = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('record_dispatch_runtime_role: armed; perform four isolated actions')
