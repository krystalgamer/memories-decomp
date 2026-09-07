-- password_scene_ids.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md keeps Data Crystal IDs 0x0E1-0x0E3 as an
--   unverified Password-screen range. Matching code directly requests 0x0E1
--   for the starchip display and decimal 226 (0x0E2) for the initial message,
--   but the visible text mapping and the role of 0x0E3 have not been measured.
--
--   This trace records Text_LookupString calls in the 0x0E1-0x0E3 range.
--   Each unique context includes the caller and main mode. Human context maps
--   every reported ID to the visible starchip label, prompt, result, or other
--   Password-screen text without assuming the community labels are correct.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with a loaded save and a clean retail executable, then
--      enable the debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec.
--   3. From the loaded menu, open Debug -> Lua editor, paste this file, and
--      confirm that it prints "breakpoint installed".
--   4. Enter Password and note the exact visible text for each reported hit.
--   5. Enter one deliberately invalid eight-digit value and dismiss its
--      result. Do not confirm a purchase or modify the save for this control.
--   6. Do not reset during the capture. After all three IDs and five quiet
--      seconds, copy the whole document into
--      tools/trace/result/password_scene_ids.txt.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm the interpreter CPU, loaded-save state, and digits used for the
--   invalid control. For each hit number, identify the exact visible text and
--   whether it appeared before, during, or after the reported lookup.

local ffi = require('ffi')

local SCRIPT_NAME = 'password_scene_ids'
local TEXT_LOOKUP = 0x8003b744
local TEXT_LOOKUP_ENTRY_WORD = 0x00a03021
local MAIN_MODE = 0x8009b26c
local FIRST_PASSWORD_ID = 0x0e1
local LAST_PASSWORD_ID = 0x0e3
local TARGET_UNIQUE_IDS = 3
local QUIET_FRAMES = 300
local NO_HIT_WARNING_FRAMES = 600
local TIMEOUT_FRAMES = 72000
local MAX_HITS = 32

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

local function u32(addr)
    return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0])
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local lines = {}
local seenContexts = {}
local seenIDs = {}
local frames = 0
local hits = 0
local uniqueIDs = 0
local lastHitFrame = nil
local callbackError = nil
local hitLimitReached = false
local noHitWarningPrinted = false
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function callSite(regs)
    return normalize32(tonumber(regs.GPR.n.ra) - 8)
end

local function observedIDText()
    local ids = {}
    for id in pairs(seenIDs) do
        ids[#ids + 1] = id
    end
    table.sort(ids)

    local values = {}
    for _, id in ipairs(ids) do
        values[#values + 1] = string.format('0x%03X', id)
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

    if breakpoint_password_scene_ids ~= nil then
        pcall(function()
            breakpoint_password_scene_ids:disable()
        end)
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm interpreter CPU, loaded save, and invalid digits; for each')
    print(' hit identify the exact visible text and whether it appeared before,')
    print(' during, or after the reported lookup>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: hits=%d unique_ids=%d observed_ids=%s',
        hits,
        uniqueIDs,
        observedIDText()
    ))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onLookup()
    local regs = PCSX.getRegisters()
    local id = normalize32(tonumber(regs.GPR.n.a1))

    if id < FIRST_PASSWORD_ID or id > LAST_PASSWORD_ID then
        return
    end

    local site = callSite(regs)
    local rawMode = u8(MAIN_MODE)
    local key = string.format('%03X:%08X:%02X', id, site, rawMode)
    if seenContexts[key] then
        return
    end

    if hits >= MAX_HITS then
        hitLimitReached = true
        return
    end

    seenContexts[key] = true
    hits = hits + 1
    lastHitFrame = frames
    if not seenIDs[id] then
        seenIDs[id] = true
        uniqueIDs = uniqueIDs + 1
    end

    emit(string.format(
        'hit=%02d frame=%06d id=0x%03X callsite=0x%08X '
            .. 'mode_raw=0x%02X mode=%02d',
        hits,
        frames,
        id,
        site,
        rawMode,
        mainMode()
    ))
    print(string.format(
        '%s: hit %d id=0x%03X; note the exact visible Password text',
        SCRIPT_NAME,
        hits,
        id
    ))
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
        finish('maximum context count reached; partial trace follows')
        return
    end

    if not noHitWarningPrinted
        and hits == 0
        and frames >= NO_HIT_WARNING_FRAMES then
        noHitWarningPrinted = true
        print(SCRIPT_NAME
            .. ': no Password lookups observed; confirm interpreter CPU '
            .. 'and enter the Password screen')
    end

    if uniqueIDs >= TARGET_UNIQUE_IDS
        and lastHitFrame ~= nil
        and frames - lastHitFrame >= QUIET_FRAMES then
        finish('captured all three Password-screen IDs')
    elseif frames >= TIMEOUT_FRAMES then
        if hits > 0 then
            finish('timed out after a partial Password-screen trace')
        else
            finish('timed out without a Password-screen lookup')
        end
    end
end

if u32(TEXT_LOOKUP) ~= TEXT_LOOKUP_ENTRY_WORD then
    print(string.format(
        '%s: Text_LookupString signature mismatch at 0x%08X: 0x%08X',
        SCRIPT_NAME,
        TEXT_LOOKUP,
        u32(TEXT_LOOKUP)
    ))
    return
end

breakpoint_password_scene_ids = PCSX.addBreakpoint(
    TEXT_LOOKUP,
    'Exec',
    4,
    'Trace Password screen IDs',
    function()
        local ok, err = pcall(onLookup)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_password_scene_ids_reset = PCSX.Events.createEventListener(
    'ExecutionFlow::Reset',
    function()
        local ok, err = pcall(function()
            breakpoint_password_scene_ids = nil
            finish('reset observed; rerun the script after reset')
        end)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_password_scene_ids = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print(SCRIPT_NAME .. ': breakpoint installed')
print(SCRIPT_NAME .. ': armed; enter Password without resetting the console')
