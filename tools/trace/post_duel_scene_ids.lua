-- post_duel_scene_ids.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md keeps Data Crystal IDs 0x03D-0x045 as an
--   unverified post-duel range. Matching Duel_CalcRankScore writes selectors
--   0x44, 0x40, and 0x45 for an ordinary result, replacing the middle value
--   with 0x41 for deck-out or 0x42 for Exodia. func_80021480 passes those
--   selectors to TextBox_Create, but static code does not identify the exact
--   visible text or screen associated with each lookup.
--
--   This trace records Text_LookupString calls in the 0x03D-0x045 range.
--   Each unique context includes the caller, main mode, winner side, and the
--   winner's signed end-reason byte. Human context maps each ID to the visible
--   result heading, statistics screen, message, or other presentation without
--   assuming the community labels are correct.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with a clean retail executable and enable the debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec.
--   3. Start a one-player duel and reach a state just before the duel ends.
--   4. Debug -> Lua editor, paste this file, and confirm that it prints
--      "breakpoint installed".
--   5. Finish the duel, then advance normally through every result, statistics,
--      rank, starchip, and card-drop screen until the game returns.
--   6. Whenever a lookup is reported, note the exact visible text or screen
--      and whether the lookup happened before, during, or after it appeared.
--   7. After three distinct IDs and five quiet seconds, copy the whole document
--      into tools/trace/result/post_duel_scene_ids.txt and fill in the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm the interpreter CPU, mode and opponent, win/loss condition, and
--   whether the result was ordinary, deck-out, or Exodia. For every hit,
--   identify the exact visible text/screen and its timing relative to the hit.

local ffi = require('ffi')

local SCRIPT_NAME = 'post_duel_scene_ids'
local TEXT_LOOKUP = 0x8003b744
local TEXT_LOOKUP_ENTRY_WORD = 0x00a03021
local MAIN_MODE = 0x8009b26c
local WINNER_SIDE = 0x8009b165
local DUEL_SIDE_BASE = 0x800e9ff0
local DUEL_SIDE_STRIDE = 0x20
local FIRST_RESULT_ID = 0x03d
local LAST_RESULT_ID = 0x045
local TARGET_UNIQUE_IDS = 3
local QUIET_FRAMES = 300
local NO_HIT_WARNING_FRAMES = 600
local TIMEOUT_FRAMES = 108000
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

local function s8(addr)
    local value = u8(addr)
    if value >= 0x80 then
        return value - 0x100
    end
    return value
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function winnerContext()
    local winner = u8(WINNER_SIDE)
    if winner > 1 then
        return winner, 0, false
    end
    return winner, s8(DUEL_SIDE_BASE + winner * DUEL_SIDE_STRIDE), true
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

    if breakpoint_post_duel_scene_ids ~= nil then
        pcall(function()
            breakpoint_post_duel_scene_ids:disable()
        end)
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm interpreter CPU, opponent and mode, win/loss condition,')
    print(' and ordinary/deck-out/Exodia result; for every hit identify the')
    print(' exact visible text or screen and whether it appeared before, during,')
    print(' or after the reported lookup>')
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

    if id < FIRST_RESULT_ID or id > LAST_RESULT_ID then
        return
    end

    local site = callSite(regs)
    local rawMode = u8(MAIN_MODE)
    local winner, endReason, validWinner = winnerContext()
    local key = string.format(
        '%03X:%08X:%02X:%02X:%d',
        id,
        site,
        rawMode,
        winner,
        endReason
    )
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
            .. 'mode_raw=0x%02X mode=%02d winner=%d '
            .. 'winner_valid=%s end_reason=%d/0x%02X',
        hits,
        frames,
        id,
        site,
        rawMode,
        mainMode(),
        winner,
        tostring(validWinner),
        endReason,
        endReason % 0x100
    ))
    print(string.format(
        '%s: hit %d id=0x%03X; note the exact visible result text or screen',
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
            .. ': no post-duel lookups observed; confirm interpreter CPU '
            .. 'and continue through the result screens')
    end

    if uniqueIDs >= TARGET_UNIQUE_IDS
        and lastHitFrame ~= nil
        and frames - lastHitFrame >= QUIET_FRAMES then
        finish('captured three distinct post-duel IDs')
    elseif frames >= TIMEOUT_FRAMES then
        if hits > 0 then
            finish('timed out after a partial post-duel trace')
        else
            finish('timed out without a post-duel lookup')
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

breakpoint_post_duel_scene_ids = PCSX.addBreakpoint(
    TEXT_LOOKUP,
    'Exec',
    4,
    'Trace post-duel scene IDs',
    function()
        local ok, err = pcall(onLookup)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_post_duel_scene_ids_reset = PCSX.Events.createEventListener(
    'ExecutionFlow::Reset',
    function()
        local ok, err = pcall(function()
            breakpoint_post_duel_scene_ids = nil
            finish('reset observed; rerun the script after reset')
        end)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_post_duel_scene_ids = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print(SCRIPT_NAME .. ': breakpoint installed')
print(SCRIPT_NAME
    .. ': armed; finish the duel and advance through every result screen')
