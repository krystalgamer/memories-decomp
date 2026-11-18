-- model_variant373_slot1_helper_reach.lua
--
-- WHAT THIS ANSWERS
--   The header-373 helpers FrenchModelVariant373_DrawStrip, DrawRibbons,
--   DrawQuads and DrawRays are matched C in every Cosmo Queen variant image,
--   but no caller has been found: the entry only calls DrawRisingRings,
--   DrawSphere and DrawSparks. An earlier run (#418) covered the slot-0
--   image (header 356, 0x8013B000) in two Cosmo Queen attacks: the three
--   controls fired and the four candidates never did. The slot-1 image
--   (header 506, 0x8017B000) never loaded, because the game skips the 3D
--   battle when the CPU attacks. Are the four helpers reached in the slot-1
--   image, expected to be the one loaded when Cosmo Queen DEFENDS? If they
--   stay silent there too while the controls fire, the notes can call them
--   unreached in the US build; if they fire, the caller and the phase name
--   what they draw.
--
--   Both images are loaded when ModelSlot.guardian_star_selection is 0
--   (Model_LoadMonsterMergeStage stages 7 and 8), her first star, Venus:
--     slot 0  model_variant_707_pos0_slot0    header 356  at 0x8013B000
--     slot 1  model_variant_707_stage8_slot1  header 506  at 0x8017B000
--   Execution breakpoints sit on all seven helpers in both images. Other
--   variant images load at the same addresses, so a hit only counts while
--   the slot's header word is 356 (slot 0) or 506 (slot 1) and the helper's
--   first instruction matches. DrawRisingRings, DrawSphere and DrawSparks
--   are the live controls: they must fire in the same slot before candidate
--   silence means anything. Slot 0 stays tracked as the known-good control.
--
-- HOW TO RUN (about five minutes once the script is loaded)
--   You need two memory-card saves for the main menu's 2P DUEL, and player
--   2's deck must hold Cosmo Queen.
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Start a 2P DUEL. Play until player 2 has Cosmo Queen face-up in
--      ATTACK position with the VENUS guardian star, and player 1 has a
--      monster that can attack her. Do not load the script before this
--      point.
--   3. Debug -> Lua editor, paste this file. Check that the setup line and
--      the "breakpoints installed" line appear.
--   4. Player 1 attacks Cosmo Queen (she defends). Watch the 3D battle.
--   5. If she survives: on player 2's turn let Cosmo Queen attack a player-1
--      monster (the slot-0 control), and if time allows have player 1 attack
--      her once more.
--   6. It prints its document after two slot-1 episodes, after four
--      episodes, five minutes after it was loaded, or on reset. Copy the
--      whole document into
--      tools/trace/result/model_variant373_slot1_helper_reach.txt and fill in
--      the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   One line per #NN: who attacked whom (did Cosmo Queen attack or defend),
--   the guardian star she had, who won, and anything on screen that looked
--   like a strip, ribbons, four quads or a burst of rays.
--
-- LUA STACK OVERFLOW
--   PCSX-Redux builds before grumpycoders/pcsx-redux d303204c (2026-10-18)
--   leave two tables on an event listener's coroutine stack on every event.
--   A GPU::Vsync listener therefore overflows LuaJIT's 65,500 slots after
--   about 32,750 frames (nine minutes), which printed a page of "(Table)"
--   lines and "stack overflow" during the previous run. This script stops
--   itself after five minutes, well inside that budget. After you have
--   copied the document, a later "stack overflow" message from an old build
--   is harmless; restart the emulator or update PCSX-Redux to clear it.

local ffi = require('ffi')

local SCRIPT_NAME = 'model_variant373_slot1_helper_reach'
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
local FPS = 60
local SLOTS = 0x800f2c40
local SLOT_SIZE = 0xe20
local VARIANT_DATA = 0xdec
local MODEL_ID = 0xdf8
local GUARDIAN_STAR = 0xdfe
local PHASE = 0x2484
local COSMO_QUEEN_MODEL = 707
local IMAGES = {
    [0] = {base = 0x8013b000, header = 356},
    [1] = {base = 0x8017b000, header = 506},
}
-- offset in the image, first instruction word, name, control
local HELPERS = {
    {0x10a8, 0x27bdfec8, 'DrawRisingRings', true},
    {0x1770, 0x27bdfee0, 'DrawSphere', true},
    {0x1ee4, 0x27bdfef0, 'DrawSparks', true},
    {0x2240, 0x27bdff00, 'DrawStrip', false},
    {0x2784, 0x27bdfee0, 'DrawRibbons', false},
    {0x2e50, 0x27bdfee0, 'DrawQuads', false},
    {0x33d8, 0x27bdfec8, 'DrawRays', false},
}

local MAX_EPISODES = 4
local MAX_SLOT1_EPISODES = 2
local MAX_ROWS = 300
local MAX_HIT_ROWS = 40
-- Old PCSX-Redux builds leak two Lua stack slots per Vsync event and
-- overflow at about 32,750 events; five minutes stays well inside that.
local TIMEOUT_FRAMES = 300 * FPS

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr ~= nil and addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if mem == nil or not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function u16(addr)
    if mem == nil or not inRam(addr, 2) or addr % 2 ~= 0 then return nil end
    return tonumber(ffi.cast('uint16_t*', mem + (addr - RAM_BASE))[0])
end
local function u32(addr)
    if mem == nil or not inRam(addr, 4) or addr % 4 ~= 0 then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local function s32(addr)
    local v = u32(addr)
    if v ~= nil and v >= 0x80000000 then v = v - 0x100000000 end
    return v
end
local function show(v, fmt)
    if v == nil then return 'unreadable' end
    return string.format(fmt or '%d', v)
end

local frames = 0
local rows, dropped = {}, 0
local hitRows = 0
local episodes = 0
local slotEpisodes = {[0] = 0, [1] = 0}
local closedSlot1 = 0
local current = {}
-- totals[slot][name]
local totals = {[0] = {}, [1] = {}}
local foreign, mismatched = 0, 0
local callbackError = nil
local summaries = {}
local done = false

-- Printing never raises into the emulator's callback path.
local function say(text)
    pcall(print, text)
end

local function stamp()
    return string.format('t=%6.1fs', frames / FPS)
end

local function live(text)
    say(text)
    if #rows < MAX_ROWS then rows[#rows + 1] = text else dropped = dropped + 1 end
end

local function loaded(slot)
    local image = IMAGES[slot]
    return u32(image.base) == image.header
end

local function work(slot)
    local w = u32(SLOTS + slot * SLOT_SIZE + VARIANT_DATA)
    if w == nil or w % 4 ~= 0 or not inRam(w, PHASE + 4) then return nil end
    return w
end

local function closeEpisode(slot, reason)
    local e = current[slot]
    if e == nil then return end
    current[slot] = nil
    if slot == 1 then closedSlot1 = closedSlot1 + 1 end
    local parts = {}
    for _, h in ipairs(HELPERS) do
        parts[#parts + 1] = h[3] .. '=' .. (e.hits[h[3]] or 0)
    end
    summaries[#summaries + 1] = string.format(
        '#%02d slot=%d header=%d frames=%d %s', e.number, slot,
        IMAGES[slot].header, frames - e.startFrame, table.concat(parts, ' '))
    live(string.format('#%02d %s END   slot=%d reason=%s', e.number, stamp(),
                       slot, reason))
end

local function slotLine(slot)
    local controls, candidates = 0, 0
    for _, h in ipairs(HELPERS) do
        local n = totals[slot][h[3]] or 0
        if h[4] then controls = controls + n else candidates = candidates + n end
    end
    local verdict
    if slotEpisodes[slot] == 0 then
        verdict = slot == 1
            and 'NOT LOADED: Cosmo Queen never defended in a 3D battle; '
                .. 'slot 1 is still untested'
            or 'not loaded'
    elseif controls == 0 then
        verdict = 'WARNING: image loaded but no control fired; candidate '
            .. 'silence is not evidence'
    elseif candidates == 0 then
        verdict = 'controls fired, candidates silent'
    else
        verdict = 'CANDIDATE REACHED'
    end
    return string.format('slot%d header=%d episodes=%d control_hits=%d '
                         .. 'candidate_hits=%d: %s', slot, IMAGES[slot].header,
                         slotEpisodes[slot], controls, candidates, verdict)
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_model_variant373_slot1_helper_reach) do
        pcall(function() bp:disable() end)
    end
    for slot = 0, 1 do
        if current[slot] ~= nil then closeEpisode(slot, 'still running') end
    end
    local lines = {
        '',
        '==== USER CONTEXT ====',
        '',
        '<one line per #NN: who attacked whom (did Cosmo Queen attack or',
        ' defend), her guardian star, who won, and anything that looked like',
        ' a strip, ribbons, four quads or a burst of rays>',
        '',
        '==== TRACE RESULT =====',
        '',
        'script: ' .. SCRIPT_NAME,
    }
    for _, line in ipairs(lines) do say(line) end
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    say('status: ' .. reason)
    say(string.format('frames: %d episodes: %d foreign_hits: %d '
                      .. 'signature_mismatches: %d dropped_rows: %d',
                      frames, episodes, foreign, mismatched, dropped))
    say(slotLine(1))
    say(slotLine(0))
    if callbackError ~= nil then say('callback_error: ' .. callbackError) end
    say('')
    for _, line in ipairs(summaries) do say('summary ' .. line) end
    say('')
    for _, line in ipairs(rows) do say(line) end
    say('')
    say('--- end of trace, copy everything above into '
        .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onHelper(slot, helper)
    if done then return end
    local image = IMAGES[slot]
    if not loaded(slot) then
        foreign = foreign + 1
        return
    end
    if u32(image.base + helper[1]) ~= helper[2] then
        mismatched = mismatched + 1
        return
    end
    local e = current[slot]
    local name = helper[3]
    totals[slot][name] = (totals[slot][name] or 0) + 1
    if e == nil then return end
    local first = e.hits[name] == nil
    e.hits[name] = (e.hits[name] or 0) + 1
    if first and hitRows < MAX_HIT_ROWS then
        hitRows = hitRows + 1
        local regs = PCSX.getRegisters()
        local ra = regs and tonumber(regs.GPR.n.ra) or nil
        local a0 = regs and tonumber(regs.GPR.n.a0) or nil
        local w = work(slot)
        live(string.format('#%02d %s first %s slot=%d ra=%s a0=%s work=%s '
                           .. 'phase=%s%s', e.number, stamp(), name, slot,
                           show(ra, '0x%08X'), show(a0, '0x%08X'),
                           show(w, '0x%08X'), show(w and s32(w + PHASE)),
                           helper[4] and '' or ' CANDIDATE'))
    end
end

local function pollSlot(slot)
    local e = current[slot]
    local isLoaded = loaded(slot)
    local w = work(slot)
    local base = SLOTS + slot * SLOT_SIZE
    if e ~= nil and (not isLoaded or w ~= e.work) then
        closeEpisode(slot, 'image unloaded or work area changed')
        e = nil
    end
    if e == nil and isLoaded and w ~= nil and episodes < MAX_EPISODES then
        episodes = episodes + 1
        slotEpisodes[slot] = slotEpisodes[slot] + 1
        current[slot] = {number = episodes, work = w, startFrame = frames,
                         hits = {}}
        local model = u16(base + MODEL_ID)
        live(string.format('#%02d %s START slot=%d header=%d model=%s%s '
                           .. 'star=%s work=0x%08X phase=%s', episodes,
                           stamp(), slot, IMAGES[slot].header, show(model),
                           model == COSMO_QUEEN_MODEL and '' or ' (not 707)',
                           show(u8(base + GUARDIAN_STAR)), w,
                           show(s32(w + PHASE))))
    end
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    pollSlot(0)
    pollSlot(1)
    if closedSlot1 >= MAX_SLOT1_EPISODES then
        finish('captured ' .. MAX_SLOT1_EPISODES .. ' slot-1 episodes')
    elseif episodes >= MAX_EPISODES and current[0] == nil
        and current[1] == nil then
        finish('captured ' .. MAX_EPISODES .. ' episodes')
    elseif frames >= TIMEOUT_FRAMES then
        finish('five minutes elapsed')
    end
end

-- Installed once, at load. Reloading the script replaces these globals.
breakpoints_model_variant373_slot1_helper_reach = {}
for slot = 0, 1 do
    for _, helper in ipairs(HELPERS) do
        local s, h = slot, helper
        local list = breakpoints_model_variant373_slot1_helper_reach
        list[#list + 1] =
            traceAddBreakpoint(IMAGES[s].base + h[1], 'Exec', 4,
                               SCRIPT_NAME .. ': slot' .. s .. ' ' .. h[3],
                               function()
                                   local ok, err = pcall(onHelper, s, h)
                                   if not ok and callbackError == nil then
                                       callbackError = tostring(err)
                                   end
                               end)
    end
end

listeners_model_variant373_slot1_helper_reach = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        local ok, err = pcall(poll)
        if not ok then
            callbackError = callbackError or tostring(err)
            pcall(finish, 'script error: ' .. tostring(err))
        end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        pcall(finish, 'reset; reload the script and start again')
    end),
}

say(string.format('%s: %d breakpoints installed; player 1 now attacks '
                  .. 'player 2\'s Cosmo Queen (Venus)', SCRIPT_NAME,
                  #breakpoints_model_variant373_slot1_helper_reach))
