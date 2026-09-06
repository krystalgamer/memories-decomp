-- opening_heishin_text_skip.lua
--
-- WHAT THIS ANSWERS
--   The "Remover Heishin" tutorial replaces the first three bytes of global
--   text string 1350 at 0x801B127A:
--
--     F7 05 00 -> FD 1C 13
--
--   Static analysis proves that FD calls Text_SetCursorOffset and that 1C 13
--   redirects the text cursor to 0x801B131C. It does not establish exactly
--   what the player sees and hears after that jump.
--
--   This trace waits for the retail text bytes, applies the three-byte patch
--   only in emulated RAM, records the matching Text_SetCursorOffset call and
--   surrounding script/text state, then restores the original bytes when it
--   finishes. Human context supplies the visible presentation.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec.
--   3. Debug -> Lua editor, paste this file, and let it auto-run.
--   4. Start a new game and proceed normally into the opening campaign
--      sequence. Do not apply any separate Heishin-skip patch.
--   5. When the script reports the target jump, note the portrait, background,
--      music, and first visible line before and after the skipped section.
--   6. Continue until the text box finishes or the screen changes.
--      If you stop the script early, reset the emulator to discard its
--      temporary RAM patch.
--   7. Copy the whole document into
--      tools/trace/result/opening_heishin_text_skip.txt and fill in the
--      context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm a clean retail boot and interpreter CPU. Describe the last visible
--   text/image before the reported target jump, the first visible text/image
--   after it, any music or sound transition, and whether the intro remained
--   playable through the next screen.

local ffi = require('ffi')

local SCRIPT_NAME = 'opening_heishin_text_skip'
local MAIN_MODE = 0x8009b26c
local CAMPAIGN_MODE = 2
local SCRIPT_STREAM = 0x8009b290
local SCRIPT_STATE = 0x8009b2a4
local SCRIPT_COMMAND = 0x8009b27c
local TEXT_CHANNEL_BASE = 0x800eb0f8
local TEXT_CHANNEL_SIZE = 0x64
local TEXT_CHANNEL_COUNT = 4
local TEXT_SET_CURSOR_OFFSET = 0x80038ba8
local PATCH_ADDRESS = 0x801b127a
local PATCH_TARGET_LOW = 0x131c
local ORIGINAL_BYTES = {0xf7, 0x05, 0x00}
local PATCH_BYTES = {0xfd, 0x1c, 0x13}
local TEXT_BOX_FLAG_DONE = 0x2000
local MIN_POST_JUMP_FRAMES = 60
local MAX_POST_JUMP_FRAMES = 1800
local MAX_JUMP_HITS = 32
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

local function bytesMatch(addr, values)
    if not isRam(addr, #values) then
        return false
    end
    for i, value in ipairs(values) do
        if u8(addr + i - 1) ~= value then
            return false
        end
    end
    return true
end

local function bytesText(addr, count)
    if not isRam(addr, count) then
        return 'invalid'
    end
    local values = {}
    for i = 0, count - 1 do
        values[#values + 1] = string.format('%02X', u8(addr + i))
    end
    return table.concat(values, ' ')
end

local function patchContextMatches()
    return isRam(PATCH_ADDRESS - 4, 11)
        and u8(PATCH_ADDRESS - 4) == 0xf6
        and u8(PATCH_ADDRESS - 3) == 0x3e
        and u8(PATCH_ADDRESS - 2) == 0x80
        and u8(PATCH_ADDRESS - 1) == 0xff
        and u8(PATCH_ADDRESS + 3) == 0x41
        and u8(PATCH_ADDRESS + 4) == 0xf8
        and u8(PATCH_ADDRESS + 5) == 0x10
        and u8(PATCH_ADDRESS + 6) == 0x03
end

local lines = {}
local frames = 0
local jumpHits = 0
local patchApplications = 0
local patchState = 'waiting_for_retail_bytes'
local patchedByScript = false
local targetJumpSeen = false
local targetJumpFrame = nil
local targetObject = nil
local postJumpSnapshot = false
local lastMode = nil
local callbackError = nil
local hitLimitReached = false
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function channelIndex(object)
    local delta = object - TEXT_CHANNEL_BASE
    if delta < 0
        or delta >= TEXT_CHANNEL_SIZE * TEXT_CHANNEL_COUNT
        or delta % TEXT_CHANNEL_SIZE ~= 0 then
        return -1
    end
    return math.floor(delta / TEXT_CHANNEL_SIZE)
end

local function cursorState(object)
    if not isRam(object, 0x5c) then
        return -1, 0, 0
    end

    local slot = s8(object + 0x58)
    local pointerAddress = object + slot * 4
    if not isRam(pointerAddress, 4) then
        return slot, pointerAddress, 0
    end
    return slot, pointerAddress, u32(pointerAddress)
end

local function captureState(reason, object)
    object = object or TEXT_CHANNEL_BASE
    local slot, pointerAddress, cursor = cursorState(object)
    local channel = channelIndex(object)
    local stringId = 'invalid'
    local flags = 'invalid'

    if isRam(object, 0x38) then
        stringId = tostring(u16(object + 0x36))
        flags = string.format('0x%04X', u16(object + 0x34))
    end

    emit(string.format(
        'snapshot frame=%06d reason=%s mode=%d script_state=0x%04X '
        .. 'script_command=0x%04X script_stream=0x%08X channel=%d '
        .. 'object=0x%08X string_id=%s flags=%s cursor_slot=%d '
        .. 'cursor_pointer=0x%08X cursor=0x%08X patch_bytes=%s',
        frames,
        reason,
        mainMode(),
        u16(SCRIPT_STATE),
        u16(SCRIPT_COMMAND),
        u32(SCRIPT_STREAM),
        channel,
        object,
        stringId,
        flags,
        slot,
        normalize32(pointerAddress),
        normalize32(cursor),
        bytesText(PATCH_ADDRESS, 3)
    ))
end

local function applyPatch(reason)
    patchedByScript = true
    for i, value in ipairs(PATCH_BYTES) do
        write8(PATCH_ADDRESS + i - 1, value)
    end
    if not bytesMatch(PATCH_ADDRESS, PATCH_BYTES) then
        error('failed to write the in-memory text patch')
    end

    patchApplications = patchApplications + 1
    patchState = 'applied_by_script'
    emit(string.format(
        'patch frame=%06d reason=%s address=0x%08X '
        .. 'original=F7 05 00 patched=%s',
        frames, reason, PATCH_ADDRESS, bytesText(PATCH_ADDRESS, 3)
    ))
    print(SCRIPT_NAME .. ': in-memory text patch applied')
end

local function restorePatch()
    if not patchedByScript or not patchContextMatches() then
        return false
    end

    for i, original in ipairs(ORIGINAL_BYTES) do
        local current = u8(PATCH_ADDRESS + i - 1)
        if current ~= original and current ~= PATCH_BYTES[i] then
            return false
        end
    end

    for i, value in ipairs(ORIGINAL_BYTES) do
        write8(PATCH_ADDRESS + i - 1, value)
    end
    return bytesMatch(PATCH_ADDRESS, ORIGINAL_BYTES)
end

local function onCursorJump()
    if done then
        return
    end

    local regs = PCSX.getRegisters()
    local object = normalize32(tonumber(regs.GPR.n.a0))
    local callsite = normalize32(tonumber(regs.GPR.n.ra) - 8)
    local slot, pointerAddress, cursor = cursorState(object)
    local target = nil
    local targetText = 'invalid'
    local predictedText = 'invalid'
    local stringId = 'invalid'
    local flags = 'invalid'

    if isRam(cursor, 2) then
        target = u16(cursor)
        targetText = string.format('0x%04X', target)
        predictedText = string.format(
            '0x%08X',
            math.floor(cursor / 0x10000) * 0x10000 + target
        )
    end
    if isRam(object, 0x38) then
        stringId = tostring(u16(object + 0x36))
        flags = string.format('0x%04X', u16(object + 0x34))
    end

    jumpHits = jumpHits + 1
    emit(string.format(
        'jump hit=%02d frame=%06d callsite=0x%08X channel=%d '
        .. 'object=0x%08X string_id=%s flags=%s cursor_slot=%d '
        .. 'cursor_pointer=0x%08X cursor_before=0x%08X '
        .. 'control_address=0x%08X encoded_target=%s '
        .. 'predicted_cursor_after=%s',
        jumpHits,
        frames,
        callsite,
        channelIndex(object),
        object,
        stringId,
        flags,
        slot,
        normalize32(pointerAddress),
        normalize32(cursor),
        normalize32(cursor - 1),
        targetText,
        predictedText
    ))

    if not targetJumpSeen
        and target ~= nil
        and cursor - 1 == PATCH_ADDRESS
        and target == PATCH_TARGET_LOW then
        targetJumpSeen = true
        targetJumpFrame = frames
        targetObject = object
        print(SCRIPT_NAME .. ': target jump observed; note the visible scene')
    end

    if jumpHits >= MAX_JUMP_HITS then
        hitLimitReached = true
    end
end

local function finish(reason)
    if done then
        return
    end
    done = true

    if breakpoint_opening_heishin_text_skip ~= nil then
        breakpoint_opening_heishin_text_skip:disable()
    end

    local restoreOk, restoreResult = pcall(restorePatch)
    local restored = restoreOk and restoreResult
    if not restoreOk then
        emit('restore_error=' .. tostring(restoreResult))
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm a clean retail boot and interpreter CPU; describe the last')
    print(' visible text, portrait, background, and sound before the target jump;')
    print(' describe the first visible presentation after it; confirm whether the')
    print(' intro remained playable through the next screen>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: patch_state=%s applications=%d jump_hits=%d '
        .. 'target_jump_seen=%s restored=%s',
        patchState,
        patchApplications,
        jumpHits,
        tostring(targetJumpSeen),
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

    if callbackError ~= nil then
        finish('breakpoint callback error: ' .. callbackError)
        return
    end

    if patchContextMatches() then
        if bytesMatch(PATCH_ADDRESS, ORIGINAL_BYTES) and not targetJumpSeen then
            applyPatch(
                patchApplications == 0
                    and 'retail text bank detected'
                    or 'text bank reloaded before target jump'
            )
        elseif bytesMatch(PATCH_ADDRESS, PATCH_BYTES)
            and patchState == 'waiting_for_retail_bytes' then
            patchState = 'already_present'
            emit(string.format(
                'patch frame=%06d reason=patch already present '
                .. 'address=0x%08X bytes=%s',
                frames, PATCH_ADDRESS, bytesText(PATCH_ADDRESS, 3)
            ))
        end
    end

    local mode = mainMode()
    if mode ~= lastMode then
        emit(string.format(
            'mode frame=%06d old=%s new=%d',
            frames,
            lastMode == nil and 'none' or tostring(lastMode),
            mode
        ))
        lastMode = mode
    end

    if hitLimitReached then
        finish('maximum cursor-jump hit count reached')
        return
    end

    if targetJumpSeen then
        local elapsed = frames - targetJumpFrame
        if not postJumpSnapshot and elapsed >= 1 then
            captureState('first VSync after target jump', targetObject)
            postJumpSnapshot = true
        end

        if elapsed >= MIN_POST_JUMP_FRAMES
            and isRam(targetObject, 0x36)
            and math.floor(u16(targetObject + 0x34) / TEXT_BOX_FLAG_DONE) % 2
                == 1 then
            captureState('text box reported done', targetObject)
            finish('target jump observed and text box completed')
            return
        end

        if elapsed >= 1 and mode ~= CAMPAIGN_MODE then
            captureState('left campaign mode after target jump', targetObject)
            finish('target jump observed and campaign screen changed')
            return
        end

        if elapsed >= MAX_POST_JUMP_FRAMES then
            captureState('post-jump observation limit', targetObject)
            finish('target jump observed; post-jump observation complete')
            return
        end
    end

    if frames >= TIMEOUT_FRAMES then
        if patchState == 'waiting_for_retail_bytes' then
            finish('timed out before the retail text bank was detected')
        elseif targetJumpSeen then
            finish('timed out after observing the target jump')
        else
            finish('timed out without the target cursor jump; use interpreter CPU')
        end
    end
end

breakpoint_opening_heishin_text_skip = PCSX.addBreakpoint(
    TEXT_SET_CURSOR_OFFSET,
    'Exec',
    4,
    'Trace the opening Heishin text skip',
    function()
        local ok, err = pcall(onCursorJump)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_opening_heishin_text_skip = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print(SCRIPT_NAME .. ': armed; start a new game and enter the campaign intro')
