-- memory_card_dialog_ids.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md keeps the memory-card dialogue IDs as an
--   unverified 0x0BA-0x0DF Data Crystal range, while notes/data-crystal.md
--   records 0x0BC-0x0E0. The message text is known, but the numeric endpoints
--   and individual ID-to-message mapping have not been measured in-game.
--
--   This trace records Text_LookupString calls across the union 0x0BA-0x0E0
--   range. Each unique context includes the caller, main mode, shared
--   memory-card dialogue flags, operation selector, result ID, and I/O result.
--   Human context supplies the message visible for each reported lookup.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with a normal formatted memory card and enable the
--      debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec.
--   3. From the loaded menu, open Debug -> Lua editor, paste this file, and
--      confirm that it prints "breakpoint installed".
--   4. Perform one ordinary successful Save. When the script reports a lookup,
--      note the exact prompt, progress message, result, or condition visible.
--   5. Return to the title and perform one ordinary successful Load, recording
--      the same context. Do not remove, replace, or format the card for this
--      control trace.
--   6. After six distinct IDs and five quiet seconds, copy the whole document
--      into tools/trace/result/memory_card_dialog_ids.txt.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm a retail executable, interpreter CPU, and an unchanged formatted
--   memory card. For each hit number, write the exact visible message and
--   whether it belonged to Save or Load. Note any lookup that occurred before
--   its message became visible or while another message was still on screen.

local ffi = require('ffi')

local SCRIPT_NAME = 'memory_card_dialog_ids'
local TEXT_LOOKUP = 0x8003b744
local TEXT_LOOKUP_ENTRY_WORD = 0x00a03021
local MAIN_MODE = 0x8009b26c
local DIALOG_RESULT_ID = 0x8009b3c6
local DIALOG_OPERATION = 0x8009b3de
local DIALOG_SLOT = 0x8009b3ee
local DIALOG_STATUS = 0x8009b3ef
local DIALOG_FLAGS = 0x8009b3fa
local IO_RESULT = 0x8009b450
local FIRST_MEMORY_CARD_ID = 0x0ba
local LAST_MEMORY_CARD_ID = 0x0e0
local TARGET_UNIQUE_IDS = 6
local QUIET_FRAMES = 300
local NO_HIT_WARNING_FRAMES = 600
local TIMEOUT_FRAMES = 72000
local MAX_HITS = 64

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

    if breakpoint_memory_card_dialog_ids ~= nil then
        pcall(function()
            breakpoint_memory_card_dialog_ids:disable()
        end)
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm retail executable, interpreter CPU, and unchanged formatted')
    print(' card; for each hit number write the exact visible message, whether')
    print(' it belonged to Save or Load, and whether the lookup preceded display')
    print(' or occurred while another message was still visible>')
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

    if id < FIRST_MEMORY_CARD_ID or id > LAST_MEMORY_CARD_ID then
        return
    end

    local site = callSite(regs)
    local flags = u16(DIALOG_FLAGS)
    local operation = u8(DIALOG_OPERATION)
    local resultID = u8(DIALOG_RESULT_ID)
    local key = string.format(
        '%03X:%08X:%04X:%02X:%02X',
        id,
        site,
        flags,
        operation,
        resultID
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
            .. 'mode_raw=0x%02X mode=%02d flags=0x%04X operation=%d '
            .. 'result_id=0x%02X slot=%d status=%d io_result=%d',
        hits,
        frames,
        id,
        site,
        u8(MAIN_MODE),
        mainMode(),
        flags,
        operation,
        resultID,
        u8(DIALOG_SLOT),
        u8(DIALOG_STATUS),
        s32(IO_RESULT)
    ))
    print(string.format(
        '%s: hit %d id=0x%03X; note the exact visible message and operation',
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
            .. ': no matching lookups observed; confirm interpreter CPU '
            .. 'and start a Save or Load operation')
    end

    if uniqueIDs >= TARGET_UNIQUE_IDS
        and lastHitFrame ~= nil
        and frames - lastHitFrame >= QUIET_FRAMES then
        finish('captured six distinct memory-card dialogue IDs')
    elseif frames >= TIMEOUT_FRAMES then
        if hits > 0 then
            finish('timed out after a partial memory-card dialogue trace')
        else
            finish('timed out without a matching lookup; use interpreter CPU')
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

breakpoint_memory_card_dialog_ids = PCSX.addBreakpoint(
    TEXT_LOOKUP,
    'Exec',
    4,
    'Trace memory-card dialogue string IDs',
    function()
        local ok, err = pcall(onLookup)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_memory_card_dialog_ids_reset = PCSX.Events.createEventListener(
    'ExecutionFlow::Reset',
    function()
        local ok, err = pcall(function()
            breakpoint_memory_card_dialog_ids = nil
            finish('reset observed; rerun the script after reset')
        end)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_memory_card_dialog_ids = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print(SCRIPT_NAME .. ': breakpoint installed')
print(SCRIPT_NAME .. ': armed; perform one successful Save and Load')
