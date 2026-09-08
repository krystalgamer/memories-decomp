-- name_entry_scene_ids.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md lists scene ID 0x002 as name entry, but marks
--   the Data Crystal scene index unverified. The surrounding low IDs have not
--   been measured either, so this trace does not assume that 0x002 is correct.
--
--   This trace records Text_LookupString calls in the low 0x000-0x007 range.
--   Each unique context includes the caller and main mode. Human context maps
--   the reported ID to the visible name-entry screen or another screen reached
--   along the title -> NEW GAME route.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with a clean retail executable and enable the debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec.
--   3. At the title or initial menu, open Debug -> Lua editor, paste this file,
--      and confirm that it prints "breakpoint installed".
--   4. Press Start if needed, choose NEW GAME, and stop after the name-entry
--      screen appears. Do not enter or confirm a name.
--   5. Whenever a lookup is reported, note the exact visible text or screen
--      and whether the lookup happened before, during, or after it appeared.
--   6. After one distinct low scene ID and five quiet seconds, copy the whole
--      document into tools/trace/result/name_entry_scene_ids.txt and fill in
--      the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm the interpreter CPU, whether the script began at the title or
--   initial menu, the exact buttons used to reach NEW GAME, and that no name
--   was entered or confirmed. For every hit, identify the exact visible text
--   or screen and its timing relative to the lookup.

local ffi = require('ffi')

local SCRIPT_NAME = 'name_entry_scene_ids'
local TEXT_LOOKUP = 0x8003b744
local TEXT_LOOKUP_ENTRY_WORD = 0x00a03021
local MAIN_MODE = 0x8009b26c
local FIRST_LOW_SCENE_ID = 0x000
local LAST_LOW_SCENE_ID = 0x007
local TARGET_UNIQUE_IDS = 1
local QUIET_FRAMES = 300
local NO_HIT_WARNING_FRAMES = 600
local TIMEOUT_FRAMES = 36000
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

    if breakpoint_name_entry_scene_ids ~= nil then
        pcall(function()
            breakpoint_name_entry_scene_ids:disable()
        end)
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm interpreter CPU, title/initial-menu starting point,')
    print(' exact buttons used to select NEW GAME, and that no name was')
    print(' entered or confirmed; for each hit identify the visible text')
    print(' or screen and whether it appeared before, during, or after the')
    print(' reported lookup>')
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
    if done or callbackError ~= nil then
        return
    end

    local regs = PCSX.getRegisters()
    local id = normalize32(tonumber(regs.GPR.n.a1))

    if id < FIRST_LOW_SCENE_ID or id > LAST_LOW_SCENE_ID then
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
        '%s: hit %d id=0x%03X; note the exact visible text or screen',
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
            .. ': no low scene IDs observed; confirm interpreter CPU and '
            .. 'continue from the title into NEW GAME')
    end

    if uniqueIDs >= TARGET_UNIQUE_IDS
        and lastHitFrame ~= nil
        and frames - lastHitFrame >= QUIET_FRAMES then
        finish('captured a low scene ID on the name-entry route')
    elseif frames >= TIMEOUT_FRAMES then
        if hits > 0 then
            finish('timed out after a partial low-scene trace')
        else
            finish('timed out without a low-scene lookup')
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

breakpoint_name_entry_scene_ids = PCSX.addBreakpoint(
    TEXT_LOOKUP,
    'Exec',
    4,
    'Trace name-entry scene IDs',
    function()
        local ok, err = pcall(onLookup)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_name_entry_scene_ids_reset = PCSX.Events.createEventListener(
    'ExecutionFlow::Reset',
    function()
        local ok, err = pcall(function()
            breakpoint_name_entry_scene_ids = nil
            finish('reset observed; rerun the script after reset')
        end)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_name_entry_scene_ids = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print(SCRIPT_NAME .. ': breakpoint installed')
print(SCRIPT_NAME .. ': armed; choose NEW GAME and stop before confirming a name')
