-- duel_result_rewards.lua
--
-- WHAT THIS ANSWERS
--   The duel-result and reward plumbing has four address-named functions and
--   one table whose meaning is not settled:
--
--   func_80020BE4  the file-transfer stage callback (descriptor, mode) that
--                  DuelScene_UpdateResultOutro registers for the result
--                  package. Modes 0-2 program VRAM upload rectangles. Is it
--                  the result screen's *_LoadPackageStage?
--   func_80032328  queues one async read of file 0x2189 (0x4C sectors) with
--                  func_80032184 as its four-phase stage callback, then
--                  waits. Called by BuildDeck_InitScreen and Main_RunTrade.
--                  What does file 0x2189 hold?
--   func_80032370  clears every gDuel_awRecentCardDrops entry whose chest
--                  count (gLibrary_abCardChest, 0x56C below) is zero, then
--                  compacts the 16-entry table. BuildDeck_InitScreen calls it
--                  and then turns the table into a per-card rank column.
--
--   Duel_AwardCard pushes each awarded card onto the front of the same
--   table. Whether it is a "new cards" list or the last duel's drops is the
--   open question src/game/duel_reward_setup.h records.
--
--   This trace logs every stage call of both callbacks with the descriptor
--   before and after (rectangle, sizes, destination, LBA), every award with
--   the table after it, and every compaction with the table and chest counts
--   before and after. BuildDeck_InitScreen and DuelScene_UpdateResultOutro
--   are controls: they show the breakpoints fire and frame each phase.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file and confirm the setup line.
--   3. Win one duel and note the card(s) you are awarded. Let the result
--      screens play out.
--   4. Open Build Deck. Look at the card list and its markers, put the
--      awarded card into the deck if you can, leave Build Deck, and open it
--      again.
--   5. The trace prints two quiet minutes after the second Build Deck visit,
--      or after 80 rows.
--   6. Copy the whole document into
--      tools/trace/result/duel_result_rewards.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Which opponent you beat and which cards you were awarded. What the
--   result screens showed while the "stage" rows were logged. In Build Deck,
--   which cards carried a "new" mark, number or other highlight, and whether
--   that changed after you moved a card into the deck.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_result_rewards'

-- Breakpoints fire only while the debugger setting is on and the interpreter
-- CPU is the engine actually running. PCSX-Redux reads the debugger flag live,
-- so a script can switch it on itself. The CPU engine is chosen at emulator
-- start and Lua cannot query it: the Dynarec setting only says what the next
-- start will use. The setup therefore counts as confirmed only once one of
-- this script's execution breakpoints has fired. Runs before any breakpoint
-- is installed.
local TRACE_SETUP = (function()
    local okSettings, emulator = pcall(function()
        return PCSX.settings.emulator
    end)
    if not okSettings or emulator == nil then
        return 'unknown: PCSX.settings unavailable; enable the debugger and '
            .. 'select the interpreter CPU by hand'
    end
    local debugger = 'unknown'
    local okDebug, debugOn = pcall(function() return emulator.Debug.Debug end)
    if okDebug and debugOn == true then
        debugger = 'on'
    elseif okDebug and debugOn == false then
        pcall(function() emulator.Debug.Debug = true end)
        local _, now = pcall(function() return emulator.Debug.Debug end)
        debugger = now == true and 'enabled-by-script'
            or 'OFF: enable Configuration -> Emulation -> Debugger'
    end
    local configured = 'unknown'
    local okDynarec, dynarecOn = pcall(function() return emulator.Dynarec end)
    if okDynarec and dynarecOn == false then
        configured = 'interpreter'
    elseif okDynarec and dynarecOn == true then
        configured = 'DYNAREC: breakpoints will not fire; select the '
            .. 'interpreter CPU and restart the emulator'
    end
    return 'debugger=' .. debugger .. ' configured_cpu=' .. configured
        .. ' active_cpu=unverified'
end)()
print(SCRIPT_NAME .. ': setup ' .. TRACE_SETUP)

-- Every breakpoint goes through here so the first real hit can confirm the
-- running engine. The wrapped invoker's return value is passed through.
local traceExecutionObserved = false
local function traceAddBreakpoint(address, kind, width, cause, invoker)
    if invoker == nil then
        return PCSX.addBreakpoint(address, kind, width, cause)
    end
    return PCSX.addBreakpoint(address, kind, width, cause, function(...)
        if not traceExecutionObserved then
            traceExecutionObserved = true
            pcall(print, SCRIPT_NAME .. ': execution breakpoint observed; '
                .. 'setup confirmed')
        end
        return invoker(...)
    end)
end
local function traceExecutionStatus()
    if traceExecutionObserved then
        return 'execution_breakpoint=observed'
    end
    return 'execution_breakpoint=NOT-observed: the setup is unverified and '
        .. 'silence is not evidence'
end

local RAM_BASE = 0x80000000
local RAM_END = 0x80200000

local MAIN_MODE = 0x8009b26c
local RECENT_DROPS = 0x801d07bc
local RECENT_COUNT = 16
local CARD_CHEST = 0x801d0250
local DESCRIPTOR_SIZE = 0x48

local STAGES = {
    {0x80020be4, 0x27bdffe0, 'result-stage func_80020BE4'},
    {0x80032184, 0x27bdffe8, 'reward-stage func_80032184'},
}
local REQUEST = {0x80032328, 0x27bdffd8, 'reward-request func_80032328'}
local COMPACT = {0x80032370, 0x3c02801d, 'compact func_80032370'}
local AWARD = {0x80021894, 0x3c02801d, 'Duel_AwardCard'}
local CONTROLS = {
    {0x800323f8, 0x27bdffb8, 'control BuildDeck_InitScreen'},
    {0x80020f4c, 0x27bdffb8, 'control DuelScene_UpdateResultOutro'},
}

local MAX_ROWS = 80
local STAGE_ROWS = 24
local QUIET_FRAMES = 7200
local NO_HIT_WARNING_FRAMES = 54000
local TIMEOUT_FRAMES = 216000

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function u16(addr)
    if not inRam(addr, 2) then return nil end
    return tonumber(ffi.cast('uint16_t*', mem + (addr - RAM_BASE))[0])
end
local function s16(addr)
    local v = u16(addr)
    if v ~= nil and v >= 0x8000 then v = v - 0x10000 end
    return v
end
local function u32(addr)
    if not inRam(addr, 4) then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local function show(v, fmt)
    if v == nil then return 'unreadable' end
    return string.format(fmt or '%d', v)
end

local frames = 0
local rows = {}
local pending = {}
local counts = {}
local stageRows = 0
local controlHits = 0
local buildDeckVisits = 0
local lastRowFrame = nil
local mismatches = {}
local callbackError = nil
local done = false

breakpoints_duel_result_rewards = {}

local function row(text)
    if #rows >= MAX_ROWS then return false end
    rows[#rows + 1] = string.format('row=%02d frame=%06d ', #rows + 1, frames)
        .. text
    lastRowFrame = frames
    return true
end

local function mode()
    local m = u8(MAIN_MODE)
    return m and string.format('0x%02X', m % 32) or 'unreadable'
end

-- Each entry is card id and, when nonzero, that card's chest count.
local function drops()
    local parts = {}
    for i = 0, RECENT_COUNT - 1 do
        local id = s16(RECENT_DROPS + i * 2)
        if id == nil then
            parts[#parts + 1] = 'unreadable'
        elseif id == 0 then
            parts[#parts + 1] = '0'
        else
            parts[#parts + 1] = string.format('%d:%s', id,
                                              show(u8(CARD_CHEST + id - 1)))
        end
    end
    return '[' .. table.concat(parts, ' ') .. ']'
end

local function descriptor(p)
    return string.format(
        'rect=(%s,%s,%s,%s) vram=(%s,%s) dest08=%s dest0C=%s phase_size=%s '
            .. 'total=%s lba=%s done=%s',
        show(s16(p + 0x00)), show(s16(p + 0x02)), show(s16(p + 0x04)),
        show(s16(p + 0x06)), show(u16(p + 0x30), '0x%X'),
        show(u16(p + 0x32), '0x%X'), show(u32(p + 0x08), '0x%08X'),
        show(u32(p + 0x0c), '0x%08X'), show(u32(p + 0x1c), '0x%X'),
        show(u32(p + 0x10), '0x%X'), show(u32(p + 0x24), '0x%X'),
        show(u8(p + 0x46)))
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_duel_result_rewards) do bp:disable() end
    for _, item in ipairs(pending) do
        local ok, err = pcall(item.flush)
        if not ok and callbackError == nil then
            callbackError = 'flushing a row: ' .. tostring(err)
        end
    end
    pending = {}

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<opponent beaten and cards awarded; what the result screens showed')
    print(' during the stage rows; in Build Deck, which cards were marked new,')
    print(' numbered or highlighted, and whether that changed after moving a')
    print(' card into the deck>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d rows: %d control_hits: %d '
                        .. 'build_deck_visits: %d', frames, #rows,
                        controlHits, buildDeckVisits))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    for _, t in ipairs({STAGES[1], STAGES[2], REQUEST, COMPACT, AWARD}) do
        print(string.format('count %-30s %d', t[3], counts[t[1]] or 0))
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

-- Rows that need the state after the call are completed on the next vsync.
local function later(flush)
    pending[#pending + 1] = { frame = frames, flush = flush }
end

local function onStage(stage)
    if done then return end
    counts[stage[1]] = (counts[stage[1]] or 0) + 1
    if stageRows >= STAGE_ROWS or #rows >= MAX_ROWS then return end
    local n = PCSX.getRegisters().GPR.n
    local p, phase = n.a0, n.a1
    if p % 2 ~= 0 or not inRam(p, DESCRIPTOR_SIZE) then
        row(string.format('IMPLAUSIBLE %s descriptor=0x%08X mode=%d',
                          stage[3], p, phase))
        return
    end
    stageRows = stageRows + 1
    local before = descriptor(p)
    local caller = n.ra
    local at = frames
    later(function()
        row(string.format('%s mode=%d descriptor=0x%08X caller=0x%08X '
                              .. 'main=%s called_frame=%06d before: %s '
                              .. 'after: %s', stage[3], phase, p, caller,
                          mode(), at, before, descriptor(p)))
    end)
end

local function onRequest()
    if done then return end
    counts[REQUEST[1]] = (counts[REQUEST[1]] or 0) + 1
    row(string.format('%s caller=0x%08X main=%s', REQUEST[3],
                      PCSX.getRegisters().GPR.n.ra, mode()))
end

local function onCompact()
    if done then return end
    counts[COMPACT[1]] = (counts[COMPACT[1]] or 0) + 1
    local before = drops()
    local caller = PCSX.getRegisters().GPR.n.ra
    later(function()
        row(string.format('%s caller=0x%08X main=%s before=%s after=%s',
                          COMPACT[3], caller, mode(), before, drops()))
    end)
end

local function onAward()
    if done then return end
    counts[AWARD[1]] = (counts[AWARD[1]] or 0) + 1
    local n = PCSX.getRegisters().GPR.n
    local id = n.a0
    local before = drops()
    local caller = n.ra
    later(function()
        row(string.format('%s card=%d caller=0x%08X main=%s before=%s '
                              .. 'after=%s', AWARD[3], id, caller, mode(),
                          before, drops()))
    end)
end

local function onControl(control)
    if done then return end
    controlHits = controlHits + 1
    if control[1] == 0x800323f8 then
        buildDeckVisits = buildDeckVisits + 1
        row(string.format('%s visit=%d main=%s drops=%s', control[3],
                          buildDeckVisits, mode(), drops()))
    end
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    local keep = {}
    for _, item in ipairs(pending) do
        if frames > item.frame then item.flush() else keep[#keep + 1] = item end
    end
    pending = keep

    if #rows >= MAX_ROWS then
        finish('captured ' .. MAX_ROWS .. ' rows')
    elseif buildDeckVisits >= 2 and lastRowFrame ~= nil
        and frames - lastRowFrame >= QUIET_FRAMES then
        finish('two Build Deck visits, then two quiet minutes')
    elseif controlHits == 0 and frames >= NO_HIT_WARNING_FRAMES then
        finish('fifteen minutes without a control hit; the setup is not '
               .. 'working, check the setup line')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

local function install(target, handler)
    local word = u32(target[1])
    if word ~= target[2] then
        mismatches[#mismatches + 1] = string.format(
            '0x%08X expected 0x%08X read %s', target[1], target[2],
            show(word, '0x%08X'))
    end
    breakpoints_duel_result_rewards[#breakpoints_duel_result_rewards + 1] =
        traceAddBreakpoint(target[1], 'Exec', 4,
            SCRIPT_NAME .. ': ' .. target[3], function()
                local ok, err = pcall(handler)
                if not ok then callbackError = tostring(err) end
            end)
end

for _, stage in ipairs(STAGES) do
    local s = stage
    install(s, function() onStage(s) end)
end
install(REQUEST, onRequest)
install(COMPACT, onCompact)
install(AWARD, onAward)
for _, control in ipairs(CONTROLS) do
    local c = control
    install(c, function() onControl(c) end)
end

listener_duel_result_rewards = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches',
                    SCRIPT_NAME, #breakpoints_duel_result_rewards,
                    #mismatches))
