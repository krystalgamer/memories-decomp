-- model_variant373_helper_reach.lua
--
-- WHAT THIS ANSWERS
--   The header-373 helpers FrenchModelVariant373_DrawStrip, DrawRibbons,
--   DrawQuads and DrawRays are matched C in every Cosmo Queen variant image,
--   but no caller has been found: the entry only calls DrawLayers, DrawGrid
--   and DrawPoints, and no function-pointer table naming them is known. Are
--   they dead code left in the image, or reached some way the static
--   reading missed? (If dead, the notes and the record comments can say so;
--   if reached, the caller and the phase it runs in name what they draw.)
--
--   In the US build both Cosmo Queen images with these helpers are loaded
--   when ModelSlot.guardian_star_selection is 0 (Model_LoadMonsterMergeStage
--   stages 7 and 8), expected to be her first guardian star, Venus. The
--   star= column of each START row confirms that mapping as a side result:
--     slot 0  model_variant_707_pos0_slot0    header 356  at 0x8013B000
--     slot 1  model_variant_707_stage8_slot1  header 506  at 0x8017B000
--   Execution breakpoints sit on all seven helpers in both images. Other
--   variant images load at the same addresses, so a hit only counts while
--   the slot's header word is 356 (slot 0) or 506 (slot 1) and the helper's
--   first instruction matches. DrawLayers, DrawGrid and DrawPoints are the
--   live controls: they must fire during a Cosmo Queen battle.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file. Check that the setup line and
--      the "breakpoints installed" line appear.
--   3. Start a Free Duel with Cosmo Queen and play her with the Venus
--      guardian star. Let her fight twice if you can: once attacking (her
--      image in slot 0) and once being attacked (slot 1).
--   4. It prints its document after 4 Cosmo Queen episodes, four minutes
--      after the first one, after 15 minutes in total, or on reset.
--   5. Copy the whole document into
--      tools/trace/result/model_variant373_helper_reach.txt and fill in the
--      context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   One line per #NN: Cosmo Queen's opponent, the guardian star you picked,
--   whether she attacked or defended, who won, and anything on screen that
--   looked like a strip, ribbons, four quads or a burst of rays.

local ffi = require('ffi')

local SCRIPT_NAME = 'model_variant373_helper_reach'
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
    {0x10a8, 0x27bdfec8, 'DrawLayers', true},
    {0x1770, 0x27bdfee0, 'DrawGrid', true},
    {0x1ee4, 0x27bdfef0, 'DrawPoints', true},
    {0x2240, 0x27bdff00, 'DrawStrip', false},
    {0x2784, 0x27bdfee0, 'DrawRibbons', false},
    {0x2e50, 0x27bdfee0, 'DrawQuads', false},
    {0x33d8, 0x27bdfec8, 'DrawRays', false},
}

local MAX_EPISODES = 4
local MAX_ROWS = 300
local MAX_HIT_ROWS = 40
local WINDOW_FRAMES = 240 * FPS
local NOTHING_FRAMES = 600 * FPS
local TIMEOUT_FRAMES = 900 * FPS

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
local firstEpisodeFrame = nil
local current = {}
local totals = {}
local foreign, mismatched = 0, 0
local callbackError = nil
local summaries = {}
local done = false

breakpoints_model_variant373_helper_reach = {}

local function stamp()
    return string.format('t=%6.1fs', frames / FPS)
end

local function live(text)
    print(text)
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
    local parts = {}
    for _, h in ipairs(HELPERS) do
        parts[#parts + 1] = h[3] .. '=' .. (e.hits[h[3]] or 0)
    end
    local line = string.format('#%02d slot=%d frames=%d %s', e.number, slot,
                               frames - e.startFrame, table.concat(parts, ' '))
    summaries[#summaries + 1] = line
    live(string.format('#%02d %s END   slot=%d reason=%s', e.number, stamp(),
                       slot, reason))
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_model_variant373_helper_reach) do
        pcall(function() bp:disable() end)
    end
    for slot = 0, 1 do
        if current[slot] ~= nil then closeEpisode(slot, 'still running') end
    end
    local controls, uncalled = 0, 0
    for _, h in ipairs(HELPERS) do
        local n = totals[h[3]] or 0
        if h[4] then controls = controls + n else uncalled = uncalled + n end
    end
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<one line per #NN: Cosmo Queen\'s opponent, the guardian star you')
    print(' picked, whether she attacked or defended, who won, and anything that')
    print(' looked like a strip, ribbons, four quads or a burst of rays>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d episodes: %d control_hits: %d '
                        .. 'candidate_hits: %d foreign_hits: %d '
                        .. 'signature_mismatches: %d dropped_rows: %d',
                        frames, episodes, controls, uncalled, foreign,
                        mismatched, dropped))
    if episodes > 0 and controls == 0 then
        print('warning: a Cosmo Queen image was loaded but no control helper '
              .. 'fired; candidate silence is not evidence')
    end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    print('')
    for _, line in ipairs(summaries) do print('summary ' .. line) end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
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
    totals[name] = (totals[name] or 0) + 1
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
    local model = u16(base + MODEL_ID)
    if e ~= nil and (not isLoaded or w ~= e.work) then
        closeEpisode(slot, 'image unloaded or work area changed')
        e = nil
    end
    if e == nil and isLoaded and w ~= nil and episodes < MAX_EPISODES then
        episodes = episodes + 1
        firstEpisodeFrame = firstEpisodeFrame or frames
        current[slot] = {number = episodes, work = w, startFrame = frames,
                         hits = {}}
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
    if episodes >= MAX_EPISODES and current[0] == nil and current[1] == nil then
        finish('captured ' .. MAX_EPISODES .. ' episodes')
    elseif firstEpisodeFrame ~= nil
        and frames - firstEpisodeFrame >= WINDOW_FRAMES then
        finish('four-minute window after the first episode ended')
    elseif firstEpisodeFrame == nil and frames >= NOTHING_FRAMES then
        finish('ten minutes without a Cosmo Queen image; play her with the '
               .. 'Venus guardian star')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

for slot = 0, 1 do
    for _, helper in ipairs(HELPERS) do
        local s, h = slot, helper
        breakpoints_model_variant373_helper_reach[
            #breakpoints_model_variant373_helper_reach + 1] =
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

listeners_model_variant373_helper_reach = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        finish('reset; reload the script and start again')
    end),
}

print(string.format('%s: %d breakpoints installed; start a Free Duel and let '
                    .. 'Cosmo Queen (Venus) fight', SCRIPT_NAME,
                    #breakpoints_model_variant373_helper_reach))
