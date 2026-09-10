-- ai_script_pass_timing.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md and notes/ai-structures.md establish the exact
--   AiScript_Run loop but leave two runtime quantities unmeasured: how many
--   bytecode commands execute in one interpreter pass, and how those passes
--   line up with visible opponent decisions.
--
--   This trace breaks at the matching VM's entry, indirect handler dispatch,
--   return from VSync(1), and common exit. Each completed pass reports its
--   command count, bounded opcode/handler sequence, timing-query values,
--   interpreter result and VSync-frame span. The active opcode is read from
--   AiScriptState.previous_cursor only after checking that pointer against
--   retail RAM. Human context supplies the action and visible delay.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the retail game, enable the debugger, and select
--      the interpreter CPU.
--   2. Enter a one-player duel and paste this script shortly before or during
--      an opponent turn. Confirm that it reports four installed breakpoints.
--   3. Let the opponent make decisions until eight interpreter passes have
--      completed. Note each visible action, pause, animation, or apparent
--      no-op when the script prints a pass summary.
--   4. Copy the whole document into
--      tools/trace/result/ai_script_pass_timing.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Name the opponent, duel mode and turn phase. For each reported pass, state
--   what the opponent visibly did, whether the action began before or after
--   the pass message, and whether several passes occurred before one visible
--   decision. List any fast-forward, cheat, patch or intro skip in use.

local ffi = require('ffi')

local SCRIPT_NAME = 'ai_script_pass_timing'
local VM_ENTRY = 0x80070650
local VM_ENTRY_SIGNATURE = 0x27bdffd8
local VM_DISPATCH = 0x800706a8
local VM_DISPATCH_SIGNATURE = 0x0040f809
local VM_TIMING_RETURN = 0x800706e0
local VM_TIMING_RETURN_SIGNATURE = 0x284200f0
local VM_EXIT = 0x800706f0
local VM_EXIT_SIGNATURE = 0x8fbf0024
local PREVIOUS_CURSOR = 0x800f5bf4
local MAIN_MODE = 0x8009b26c
local OPPONENT_ID = 0x8009b361
local RAM_BASE = 0x80000000
local RAM_END = 0x80200000
local TARGET_PASSES = 8
local MAX_STARTED_PASSES = 16
local MAX_SEQUENCE_COMMANDS = 64
local MAX_QUERY_VALUES = 32
local MAX_TOTAL_COMMANDS = 512
local NO_PASS_WARNING_FRAMES = 600
local TIMEOUT_FRAMES = 36000

local mem = PCSX.getMemPtr()

local function phys(addr)
    return addr - RAM_BASE
end

local function normalize32(value)
    return value % 0x100000000
end

local function inRam(addr, size)
    addr = normalize32(addr)
    return addr >= RAM_BASE
        and size >= 0
        and addr + size <= RAM_END
end

local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end

local function u32(addr)
    return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0])
end

local function signed32(value)
    value = normalize32(value)
    if value >= 0x80000000 then
        return value - 0x100000000
    end
    return value
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function signaturesMatch()
    return u32(VM_ENTRY) == VM_ENTRY_SIGNATURE
        and u32(VM_DISPATCH) == VM_DISPATCH_SIGNATURE
        and u32(VM_TIMING_RETURN) == VM_TIMING_RETURN_SIGNATURE
        and u32(VM_EXIT) == VM_EXIT_SIGNATURE
end

local function resultName(value)
    if value == 0 then
        return 'yield'
    end
    if value == 1 then
        return 'end_hand'
    end
    if value == 2 then
        return 'play_field_card'
    end
    if value == 3 then
        return 'end_field'
    end
    return 'unknown'
end

local function show(value)
    if value == nil then
        return 'none'
    end
    return tostring(value)
end

local lines = {}
local frames = 0
local passesStarted = 0
local passesCompleted = 0
local incompletePasses = 0
local totalCommands = 0
local orphanDispatches = 0
local orphanTimingQueries = 0
local orphanExits = 0
local activePass = nil
local commandLimitReached = false
local warningPrinted = false
local callbackError = nil
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function callSite()
    local regs = PCSX.getRegisters()
    return normalize32(tonumber(regs.GPR.n.ra) - 8)
end

local function sequenceText(values, total, limit)
    if #values == 0 then
        return 'none'
    end
    local text = table.concat(values, ',')
    if total > limit then
        text = text .. ',...'
    end
    return text
end

local function emitPass(pass, status, result)
    local duration = frames - pass.entryFrame
    local resultText = 'none'
    if result ~= nil then
        resultText = string.format('%d(%s)', result, resultName(result))
    end
    emit(string.format(
        'pass=%02d status=%s entry_frame=%06d exit_frame=%06d '
            .. 'duration_frames=%d mode=%d opponent=%d callsite=0x%08X '
            .. 'result=%s commands=%d timing_queries=%d '
            .. 'vsync_min=%s vsync_max=%s vsync_last=%s '
            .. 'yield_after_command=%s invalid_cursors=%d '
            .. 'opcodes=%s query_values=%s',
        pass.number,
        status,
        pass.entryFrame,
        frames,
        duration,
        pass.mode,
        pass.opponent,
        pass.callsite,
        resultText,
        pass.commands,
        pass.timingQueries,
        show(pass.vsyncMin),
        show(pass.vsyncMax),
        show(pass.vsyncLast),
        show(pass.yieldAfterCommand),
        pass.invalidCursors,
        sequenceText(
            pass.opcodes,
            pass.commands,
            MAX_SEQUENCE_COMMANDS
        ),
        sequenceText(
            pass.queryValues,
            pass.timingQueries,
            MAX_QUERY_VALUES
        )
    ))
end

local function disableBreakpoints()
    if breakpoint_ai_script_pass_entry ~= nil then
        breakpoint_ai_script_pass_entry:disable()
    end
    if breakpoint_ai_script_pass_dispatch ~= nil then
        breakpoint_ai_script_pass_dispatch:disable()
    end
    if breakpoint_ai_script_pass_timing ~= nil then
        breakpoint_ai_script_pass_timing:disable()
    end
    if breakpoint_ai_script_pass_exit ~= nil then
        breakpoint_ai_script_pass_exit:disable()
    end
end

local function finish(reason)
    if done then
        return
    end
    done = true

    if activePass ~= nil then
        incompletePasses = incompletePasses + 1
        emitPass(activePass, 'partial', nil)
        activePass = nil
    end
    disableBreakpoints()

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<name the opponent, duel mode and turn phase; for every pass state')
    print(' the visible action or pause and whether it began before or after')
    print(' the pass message; note passes with no visible change and any cheats>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: frames=%d passes_started=%d passes_completed=%d '
            .. 'incomplete_passes=%d commands=%d '
            .. 'orphan_dispatches=%d orphan_timing_queries=%d '
            .. 'orphan_exits=%d',
        frames,
        passesStarted,
        passesCompleted,
        incompletePasses,
        totalCommands,
        orphanDispatches,
        orphanTimingQueries,
        orphanExits
    ))
    print(string.format(
        'signatures: entry=0x%08X dispatch=0x%08X '
            .. 'timing_return=0x%08X exit=0x%08X',
        u32(VM_ENTRY),
        u32(VM_DISPATCH),
        u32(VM_TIMING_RETURN),
        u32(VM_EXIT)
    ))
    if callbackError ~= nil then
        print('callback_error: ' .. callbackError)
    end
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onEntry()
    if done or callbackError ~= nil then
        return
    end
    if passesStarted >= MAX_STARTED_PASSES then
        finish('maximum started-pass count reached')
        return
    end
    if activePass ~= nil then
        incompletePasses = incompletePasses + 1
        emitPass(activePass, 'interrupted_by_entry', nil)
    end

    passesStarted = passesStarted + 1
    activePass = {
        number = passesStarted,
        entryFrame = frames,
        mode = mainMode(),
        opponent = u8(OPPONENT_ID),
        callsite = callSite(),
        commands = 0,
        timingQueries = 0,
        vsyncMin = nil,
        vsyncMax = nil,
        vsyncLast = nil,
        yieldAfterCommand = nil,
        invalidCursors = 0,
        opcodes = {},
        queryValues = {},
    }
    print(string.format(
        '%s: pass %d started; note the next visible opponent decision',
        SCRIPT_NAME,
        passesStarted
    ))
end

local function onDispatch()
    if done or callbackError ~= nil then
        return
    end
    if activePass == nil then
        orphanDispatches = orphanDispatches + 1
        return
    end

    local regs = PCSX.getRegisters()
    local handler = normalize32(tonumber(regs.GPR.n.v0))
    local cursor = normalize32(u32(PREVIOUS_CURSOR))
    local opcodeText

    activePass.commands = activePass.commands + 1
    totalCommands = totalCommands + 1
    if inRam(cursor, 1) then
        opcodeText = string.format('%02X@%08X', u8(cursor), handler)
    else
        activePass.invalidCursors = activePass.invalidCursors + 1
        opcodeText = string.format('invalid(0x%08X)@%08X', cursor, handler)
    end
    if #activePass.opcodes < MAX_SEQUENCE_COMMANDS then
        activePass.opcodes[#activePass.opcodes + 1] = opcodeText
    end

    if totalCommands >= MAX_TOTAL_COMMANDS then
        commandLimitReached = true
    end
end

local function onTimingReturn()
    if done or callbackError ~= nil then
        return
    end
    if activePass == nil then
        orphanTimingQueries = orphanTimingQueries + 1
        return
    end

    local regs = PCSX.getRegisters()
    local value = signed32(tonumber(regs.GPR.n.v0))

    activePass.timingQueries = activePass.timingQueries + 1
    activePass.vsyncLast = value
    if activePass.vsyncMin == nil or value < activePass.vsyncMin then
        activePass.vsyncMin = value
    end
    if activePass.vsyncMax == nil or value > activePass.vsyncMax then
        activePass.vsyncMax = value
    end
    if value >= 0xf0 and activePass.yieldAfterCommand == nil then
        activePass.yieldAfterCommand = activePass.commands
    end
    if #activePass.queryValues < MAX_QUERY_VALUES then
        activePass.queryValues[#activePass.queryValues + 1] = tostring(value)
    end
end

local function onExit()
    if done or callbackError ~= nil then
        return
    end
    if activePass == nil then
        orphanExits = orphanExits + 1
        return
    end

    local regs = PCSX.getRegisters()
    local result = signed32(tonumber(regs.GPR.n.v0))
    local current = activePass
    activePass = nil
    passesCompleted = passesCompleted + 1
    emitPass(current, 'complete', result)
    print(string.format(
        '%s: pass %d completed with %d command(s), result %d (%s)',
        SCRIPT_NAME,
        current.number,
        current.commands,
        result,
        resultName(result)
    ))

    if passesCompleted >= TARGET_PASSES then
        finish('captured eight AI interpreter passes')
    elseif commandLimitReached then
        finish('maximum command count reached; partial trace follows')
    end
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
    if not signaturesMatch() then
        finish('resident AI VM signature changed during capture')
        return
    end
    if commandLimitReached then
        finish('maximum command count reached; partial trace follows')
        return
    end
    if passesStarted == 0
        and not warningPrinted
        and frames >= NO_PASS_WARNING_FRAMES then
        warningPrinted = true
        print(SCRIPT_NAME
              .. ': no AI pass seen; confirm interpreter CPU and opponent turn')
    end
    if frames >= TIMEOUT_FRAMES then
        finish('timed out before eight completed AI interpreter passes')
    end
end

listener_ai_script_pass_timing_reset =
    PCSX.Events.createEventListener(
        'ExecutionFlow::Reset',
        function()
            local ok, err = pcall(function()
                finish('reset observed; rerun during an opponent turn')
            end)
            if not ok then
                callbackError = tostring(err)
            end
        end
    )

listener_ai_script_pass_timing =
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

if not signaturesMatch() then
    finish('resident AI VM signatures do not match retail')
else
    local ok, err = pcall(function()
        breakpoint_ai_script_pass_entry = PCSX.addBreakpoint(
            VM_ENTRY,
            'Exec',
            4,
            'Trace AiScript_Run entry',
            function()
                local callbackOk, callbackErr = pcall(onEntry)
                if not callbackOk then
                    callbackError = tostring(callbackErr)
                end
            end
        )
        breakpoint_ai_script_pass_dispatch = PCSX.addBreakpoint(
            VM_DISPATCH,
            'Exec',
            4,
            'Trace AiScript_Run handler dispatch',
            function()
                local callbackOk, callbackErr = pcall(onDispatch)
                if not callbackOk then
                    callbackError = tostring(callbackErr)
                end
            end
        )
        breakpoint_ai_script_pass_timing = PCSX.addBreakpoint(
            VM_TIMING_RETURN,
            'Exec',
            4,
            'Trace AiScript_Run VSync return',
            function()
                local callbackOk, callbackErr = pcall(onTimingReturn)
                if not callbackOk then
                    callbackError = tostring(callbackErr)
                end
            end
        )
        breakpoint_ai_script_pass_exit = PCSX.addBreakpoint(
            VM_EXIT,
            'Exec',
            4,
            'Trace AiScript_Run common exit',
            function()
                local callbackOk, callbackErr = pcall(onExit)
                if not callbackOk then
                    callbackError = tostring(callbackErr)
                end
            end
        )
    end)
    if not ok then
        callbackError = tostring(err)
        finish('breakpoint installation error: ' .. callbackError)
    else
        print(SCRIPT_NAME
              .. ': four breakpoints installed; wait for an opponent turn')
    end
end
