-- opponent_card_resource_indices.lua
--
-- WHAT THIS ANSWERS
--   The "Olho do Milenio" tutorial changes the immediate at 0x80018064 from
--   0xFF to 0 or 1. Static code proves that this becomes the display object's
--   image resource index when the opponent-card display mode is negative, but
--   not what resources 0 and 1 look like or whether a later update restores
--   0xFF.
--
--   This trace runs two phases in one duel. It patches only the low immediate
--   byte in emulated RAM, first to 0 and then to 1. For each phase it captures
--   the first negative-mode return from func_80018004, records the resulting
--   field_67 value, and watches that object for later changes. The retail 0xFF
--   byte is restored when the trace finishes.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec, and the code-byte patch must be fetched from RAM.
--   3. Start a one-player duel before the opponent reveals or places a card.
--   4. Debug -> Lua editor, paste this file, and let it auto-run.
--   5. Trigger an opponent-card display. When phase 0 is captured, note the
--      exact card image, face/back state, and any order number.
--   6. After ten seconds the script switches to resource 1. Trigger another
--      opponent-card display if the current object is not refreshed
--      automatically, then record the same visual details.
--   7. Copy the whole document into
--      tools/trace/result/opponent_card_resource_indices.txt and fill in the
--      context.
--
--   If you stop the script early, reset the emulator to restore the retail
--   instruction byte.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm a clean retail executable and interpreter CPU. For phases 0 and 1,
--   identify the opponent action and card if known, describe the visible
--   image and number overlay, and say whether it later changed or reverted.

local ffi = require('ffi')

local SCRIPT_NAME = 'opponent_card_resource_indices'
local FUNCTION_ENTRY = 0x80018004
local FUNCTION_RETURN = 0x8001806c
local PATCH_INSTRUCTION = 0x80018064
local PATCH_BYTE = 0x80018064
local RETAIL_WORD = 0x240200ff
local INSTRUCTION_PREFIX = 0x24020000
local PRECEDING_WORD = 0x04410002
local FOLLOWING_WORD = 0xa0820067
local EPILOGUE_WORD = 0x8fbf0014
local STATE_POINTER = 0x8009b1c8
local MAIN_MODE = 0x8009b26c
local PLAYER_LP = 0x800ea004
local OPPONENT_LP = 0x800ea024
local OBSERVE_FRAMES = 600
local MAX_HITS = 16384
local MAX_RESOURCE_CHANGES = 32
local TIMEOUT_FRAMES = 108000

local mem = PCSX.getMemPtr()

local function phys(addr)
    return addr - 0x80000000
end

local function normalize32(value)
    return value % 0x100000000
end

local function isRam(addr, size)
    addr = normalize32(addr)
    return addr >= 0x80000000
        and addr < 0x80200000
        and size >= 0
        and addr + size <= 0x80200000
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

local function u32(addr)
    return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0])
end

local function write8(addr, value)
    ffi.cast('uint8_t*', mem + phys(addr))[0] = value
end

local lines = {}
local frames = 0
local hits = 0
local phase = 0
local phaseSeen = {[0] = false, [1] = false}
local observeFrames = 0
local trackedObject = nil
local lastTrackedValue = nil
local pendingNegative = false
local pendingInputObject = 0
local pendingDisplayMode = 0
local callbackError = nil
local hitLimitReached = false
local resourceChanges = 0
local changeLimitReported = false
local patchedByScript = false
local armed = false
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function displayMode()
    local state = u32(STATE_POINTER)
    if not isRam(state, 0x20) then
        return nil
    end
    return s8(state + 0x1f)
end

local function patchContextMatches()
    return u32(PATCH_INSTRUCTION - 4) == PRECEDING_WORD
        and u32(PATCH_INSTRUCTION + 4) == FOLLOWING_WORD
        and u32(PATCH_INSTRUCTION + 8) == EPILOGUE_WORD
end

local function patchValue(value)
    local word = u32(PATCH_INSTRUCTION)
    if not patchContextMatches()
        or math.floor(word / 0x100) * 0x100 ~= INSTRUCTION_PREFIX then
        error(string.format(
            'unexpected instruction context at 0x%08X: 0x%08X',
            PATCH_INSTRUCTION, word
        ))
    end

    patchedByScript = true
    write8(PATCH_BYTE, value)
    if u8(PATCH_BYTE) ~= value then
        error('failed to write the resource-index immediate')
    end
    emit(string.format(
        'patch frame=%06d phase=%d instruction=0x%08X immediate=0x%02X',
        frames, phase, u32(PATCH_INSTRUCTION), value
    ))
end

local function restorePatch()
    if not patchedByScript then
        return false
    end

    local word = u32(PATCH_INSTRUCTION)
    if not patchContextMatches()
        or math.floor(word / 0x100) * 0x100 ~= INSTRUCTION_PREFIX then
        return false
    end
    write8(PATCH_BYTE, 0xff)
    return u32(PATCH_INSTRUCTION) == RETAIL_WORD
end

local function onEntry()
    if done or not armed then
        return
    end

    local regs = PCSX.getRegisters()
    local mode = displayMode()
    hits = hits + 1
    pendingInputObject = normalize32(tonumber(regs.GPR.n.a0))
    pendingNegative = mode ~= nil and mode < 0
    pendingDisplayMode = mode or 0
    if hits >= MAX_HITS then
        hitLimitReached = true
    end
end

local function onReturn()
    if done or not pendingNegative then
        pendingNegative = false
        return
    end

    local regs = PCSX.getRegisters()
    local result = normalize32(tonumber(regs.GPR.n.a0))
    if not isRam(result, 0x68) then
        error(string.format('invalid result object 0x%08X', result))
    end
    local resource = u8(result + 0x67)

    if not phaseSeen[phase] then
        phaseSeen[phase] = true
        observeFrames = 0
        trackedObject = result
        lastTrackedValue = resource
        emit(string.format(
            'capture frame=%06d phase=%d input_object=0x%08X '
            .. 'result_object=0x%08X display_mode=%d resource=%d '
            .. 'main_mode=0x%02X player_lp=%d opponent_lp=%d',
            frames,
            phase,
            pendingInputObject,
            result,
            pendingDisplayMode,
            resource,
            u8(MAIN_MODE),
            u16(PLAYER_LP),
            u16(OPPONENT_LP)
        ))
        print(string.format(
            '%s: phase %d captured resource %d; note the visible card',
            SCRIPT_NAME, phase, resource
        ))
    end

    pendingNegative = false
end

local function finish(reason)
    if done then
        return
    end
    done = true

    if breakpoint_opponent_card_resource_entry ~= nil then
        breakpoint_opponent_card_resource_entry:disable()
    end
    if breakpoint_opponent_card_resource_return ~= nil then
        breakpoint_opponent_card_resource_return:disable()
    end

    local restoreOk, restoreResult = pcall(restorePatch)
    local restored = restoreOk and restoreResult
    if not restoreOk then
        emit('restore_error=' .. tostring(restoreResult))
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm a clean retail executable and interpreter CPU; for phases')
    print(' 0 and 1 identify the opponent action/card, describe the visible')
    print(' image and number overlay, and say whether it later changed/reverted>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: hits=%d phase0=%s phase1=%s resource_changes=%d restored=%s',
        hits,
        tostring(phaseSeen[0]),
        tostring(phaseSeen[1]),
        resourceChanges,
        tostring(restored)
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

    if not armed then
        if not patchContextMatches()
            or u32(PATCH_INSTRUCTION) ~= RETAIL_WORD then
            finish(string.format(
                'retail instruction signature not present: 0x%08X',
                u32(PATCH_INSTRUCTION)
            ))
            return
        end
        patchValue(0)
        armed = true
        print(SCRIPT_NAME
            .. ': phase 0 armed; trigger an opponent-card display')
    end

    if callbackError ~= nil then
        finish('breakpoint callback error: ' .. callbackError)
        return
    end
    if hitLimitReached then
        finish('maximum function-entry hit count reached')
        return
    end

    if phaseSeen[phase] then
        observeFrames = observeFrames + 1
        if trackedObject ~= nil and isRam(trackedObject, 0x68) then
            local value = u8(trackedObject + 0x67)
            if value ~= lastTrackedValue then
                if resourceChanges < MAX_RESOURCE_CHANGES then
                    resourceChanges = resourceChanges + 1
                    emit(string.format(
                        'resource_change frame=%06d phase=%d object=0x%08X '
                        .. 'old=%d new=%d',
                        frames, phase, trackedObject, lastTrackedValue, value
                    ))
                elseif not changeLimitReported then
                    emit('resource_change output limit reached')
                    changeLimitReported = true
                end
                lastTrackedValue = value
            end
        end

        if observeFrames >= OBSERVE_FRAMES then
            emit(string.format(
                'phase_end frame=%06d phase=%d object=0x%08X resource=%d',
                frames,
                phase,
                trackedObject or 0,
                lastTrackedValue or -1
            ))
            if phase == 0 then
                phase = 1
                trackedObject = nil
                lastTrackedValue = nil
                observeFrames = 0
                patchValue(1)
                print(SCRIPT_NAME
                    .. ': switched to resource 1; trigger another opponent card')
            else
                finish('captured both resource-index phases')
                return
            end
        end
    end

    if frames >= TIMEOUT_FRAMES then
        finish('timed out before both resource-index phases were captured')
    end
end

breakpoint_opponent_card_resource_entry = PCSX.addBreakpoint(
    FUNCTION_ENTRY,
    'Exec',
    4,
    'Trace opponent-card resource entry',
    function()
        local ok, err = pcall(onEntry)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

breakpoint_opponent_card_resource_return = PCSX.addBreakpoint(
    FUNCTION_RETURN,
    'Exec',
    4,
    'Trace opponent-card resource result',
    function()
        local ok, err = pcall(onReturn)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_opponent_card_resource_indices = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print(SCRIPT_NAME .. ': loaded; phase 0 will arm on the next VSync')
