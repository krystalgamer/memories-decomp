-- campaign_opening_scene_ids.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md keeps the Data Crystal campaign scene index as
--   unverified. Its opening range labels 0x500 as the introduction and
--   0x501-0x506 as Simon Muran scenes, but those numeric mappings have not
--   been measured while the corresponding text is visible.
--
--   This trace records Text_LookupString calls in the 0x500-0x506 range
--   during a controlled New Game opening. Each unique context includes the
--   caller and main mode. Human context maps the reported ID to the visible
--   introduction, Simon dialogue, choice, duel lead-in, result, or other
--   screen without assuming the community label is correct.
--
-- HOW TO RUN
--   1. Open PCSX-Redux at the initial menu with a clean retail executable and
--      enable the debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec.
--   3. Open Debug -> Lua editor, paste this file, and confirm that it prints
--      "breakpoint installed" before selecting New Game.
--   4. Start New Game, finish name entry, and follow the opening normally.
--      Use the <Run away> branch, return to Pharaoh's Palace, and continue
--      through at least one Simon prompt or duel lead-in.
--   5. Whenever a lookup is reported, note the exact text, choice, or screen
--      currently visible. Also note if the lookup precedes the visible text.
--   6. Do not reset during the capture. After four distinct IDs and five quiet
--      seconds, copy the whole document into
--      tools/trace/result/campaign_opening_scene_ids.txt.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm the interpreter CPU, New Game route, entered player name, and
--   every choice made. For each hit number, identify the visible text or
--   screen and whether it appeared before, during, or after the reported hit.

local ffi = require('ffi')

local SCRIPT_NAME = 'campaign_opening_scene_ids'
local TEXT_LOOKUP = 0x8003b744
local TEXT_LOOKUP_ENTRY_WORD = 0x00a03021
local MAIN_MODE = 0x8009b26c
local FIRST_OPENING_ID = 0x500
local LAST_OPENING_ID = 0x506
local TARGET_UNIQUE_IDS = 4
local QUIET_FRAMES = 300
local NO_HIT_WARNING_FRAMES = 600
local TIMEOUT_FRAMES = 216000
local MAX_HITS = 48

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

    if breakpoint_campaign_opening_scene_ids ~= nil then
        pcall(function()
            breakpoint_campaign_opening_scene_ids:disable()
        end)
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm interpreter CPU, New Game route, entered name, and every')
    print(' choice; for each hit identify the exact visible text or screen and')
    print(' whether it appeared before, during, or after the reported lookup>')
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

    if id < FIRST_OPENING_ID or id > LAST_OPENING_ID then
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
            .. ': no opening-scene lookups observed; confirm interpreter CPU '
            .. 'and continue through New Game')
    end

    if uniqueIDs >= TARGET_UNIQUE_IDS
        and lastHitFrame ~= nil
        and frames - lastHitFrame >= QUIET_FRAMES then
        finish('captured four distinct opening-scene IDs')
    elseif frames >= TIMEOUT_FRAMES then
        if hits > 0 then
            finish('timed out after a partial opening-scene trace')
        else
            finish('timed out without an opening-scene lookup')
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

breakpoint_campaign_opening_scene_ids = PCSX.addBreakpoint(
    TEXT_LOOKUP,
    'Exec',
    4,
    'Trace campaign opening scene IDs',
    function()
        local ok, err = pcall(onLookup)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_campaign_opening_scene_ids_reset = PCSX.Events.createEventListener(
    'ExecutionFlow::Reset',
    function()
        local ok, err = pcall(function()
            breakpoint_campaign_opening_scene_ids = nil
            finish('reset observed; rerun the script after reset')
        end)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_campaign_opening_scene_ids = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print(SCRIPT_NAME .. ': breakpoint installed')
print(SCRIPT_NAME .. ': armed; start New Game without resetting the console')
