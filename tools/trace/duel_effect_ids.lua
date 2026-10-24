-- duel_effect_ids.lua
--
-- WHAT THIS ANSWERS
--   The duel-effect dispatcher (NA 0x801462B0, labelled func_80146258) maps
--   effect ids 0-24 to 25 address-named handlers. Statically we only know a
--   few starters (swords 0x12, trap resolution 8, resume 0xB, Harpie's
--   Feather Duster 0x17). Which animation does each id play? Once a row is
--   tied to what was on screen, its handler can be named.
--
--   Every effect request becomes one numbered episode. The START row prints
--   the moment the dispatcher first runs it, with the effect id, its handler,
--   the starting phase, the request payload and the resident code that
--   created it. The END row prints when the request finishes. Rows appear in
--   the Lua console live, so you can note the number of the row that appeared
--   with each animation. DuelEffect_UpdateRequests is the control that proves
--   breakpoints fire.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file, and start a Free Duel.
--   3. Keep the Lua console visible. Over about two minutes do as many
--      different things as you can: summon face-up and face-down, attack a
--      monster and attack directly, fuse two hand cards, equip, activate
--      every magic or trap card you hold, change a monster's position, and let
--      the opponent play. Each new #NN row is one animation.
--   4. The trace prints its document three minutes after the first effect,
--      after 40 effects, or when you leave the duel, whichever comes first.
--   5. Copy the whole document into tools/trace/result/duel_effect_ids.txt
--      and fill in the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   One line per #NN: what you had just done (card name if you know it) and
--   what the animation looked like, for example "#04 activated Raigeki:
--   lightning strikes every enemy monster".

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_effect_ids'

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
local DUEL_MODE = 3
local FPS = 60
local REQUESTS = 0x800ead88
local REQUEST_SIZE = 0x20
local REQUEST_COUNT = 8
local FLAG_ACTIVE = 0x80

-- duel_effects is one of several modules loaded at 0x80146000. These retail
-- words identify the North American build of it; hits are ignored otherwise.
local MODULE_WORDS = {
    {0x801462b0, 0x27bdffc8}, {0x801462b4, 0xafb7002c},
    {0x80148f98, 0x27bdff98}, {0x8015013c, 0x30c60003},
}
local DISPATCH = 0x801462b0
local CONTROL = {0x8002c6c8, 0x27bdffd8, 'control DuelEffect_UpdateRequests'}
local CREATE = {0x8002c68c, 0x27bdffe8, 'DuelEffect_CreateRequest'}
local ALLOCATE = {0x8002c604, 0x27bdffe8, 'DuelEffect_AllocateRequest'}
local CREATE_END = 0x8002c6c8

-- NA entry of the handler each effect id reaches (French-numbered label).
local HANDLERS = {
    [0] = {0x80155104, 'func_80154688'}, {0x80156438, 'func_80157794'},
    {0x80153dd8, 'func_80153200'}, {0x8014aa68, 'func_80149F90'},
    {0x8015874c, 'func_80159AAC'}, {0x80156ab4, 'func_80157E10'},
    {0x801555ac, 'func_80154B30'}, {0x8015747c, 'func_801587D8'},
    {0x801499dc, 'func_80147B18'}, {0x80158048, 'func_801593A8'},
    {0x8014d5d4, 'func_8014C8FC'}, {0x801467b8, 'func_80146760'},
    {0x801519d8, 'func_80150E00'}, {0x80150fd0, 'func_801503F8'},
    {0x8014a06c, 'func_801481A8'}, {0x80150b18, 'func_8014FF40'},
    {0x80152130, 'func_80151558'}, {0x80158e84, 'func_80148BA4'},
    {0x80154b00, 'func_80154084'}, {0x801546b4, 'func_80153ADC'},
    {0x80152130, 'func_80151558 phase 1'}, {0x8014efc4, 'func_8014E3EC'},
    {0x8014b3bc, 'func_8014A8E4'}, {0x80152c20, 'func_80152048'},
    {0x8014e050, 'func_8014D3E8'},
}

local MAX_EPISODES = 40
local MAX_ROWS = 120
local WINDOW_FRAMES = 180 * FPS
local NO_CONTROL_FRAMES = 600 * FPS
local TIMEOUT_FRAMES = 1200 * FPS

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr ~= nil and addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function u16(addr)
    if not inRam(addr, 2) or addr % 2 ~= 0 then return nil end
    return tonumber(ffi.cast('uint16_t*', mem + (addr - RAM_BASE))[0])
end
local function s16(addr)
    local v = u16(addr)
    if v ~= nil and v >= 0x8000 then v = v - 0x10000 end
    return v
end
local function u32(addr)
    if not inRam(addr, 4) or addr % 4 ~= 0 then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local function signed(v)
    v = tonumber(v) or 0
    if v >= 0x80000000 then v = v - 0x100000000 end
    return v
end
local function show(v, fmt)
    if v == nil then return 'unreadable' end
    return string.format(fmt or '%d', v)
end

local frames = 0
local duelFrames = 0
local firstEpisodeFrame = nil
local rows = {}
local episodes = {}
local openCount = 0
local episodeCount = 0
local pending = {}
local lastCreate = nil
local byEffect = {}
local controlHits = 0
local dispatchHits = 0
local foreignHits = 0
local poolFull = 0
local mismatches = {}
local callbackError = nil
local sawDuel = false
local done = false

breakpoints_duel_effect_ids = {}

local function stamp()
    return string.format('t=%6.1fs', frames / FPS)
end

-- Printed the moment it happens, and kept for the final document.
local function live(text)
    print(text)
    if #rows < MAX_ROWS then rows[#rows + 1] = text end
end

local function moduleResident()
    for _, w in ipairs(MODULE_WORDS) do
        if u32(w[1]) ~= w[2] then return false end
    end
    return true
end

local function slotOf(req)
    local offset = req - REQUESTS
    if offset >= 0 and offset < REQUEST_SIZE * REQUEST_COUNT
        and offset % REQUEST_SIZE == 0 then
        return tostring(offset / REQUEST_SIZE)
    end
    return 'outside-pool'
end

local function handlerText(id)
    local h = HANDLERS[id]
    if h == nil then return 'none (id outside 0-24)' end
    return string.format('0x%08X %s', h[1], h[2])
end

local function closeEpisode(req, ep, reason)
    local phases = {}
    for _, p in ipairs(ep.phases) do phases[#phases + 1] = tostring(p) end
    live(string.format('#%02d %s END   effect=%d frames=%d calls=%d '
                       .. 'phases=%s reason=%s', ep.number, stamp(), ep.id,
                       frames - ep.startFrame, ep.calls,
                       table.concat(phases, ','), reason))
    episodes[req] = nil
    openCount = openCount - 1
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_duel_effect_ids) do bp:disable() end
    for req, ep in pairs(episodes) do
        local ok, err = pcall(closeEpisode, req, ep, 'still running')
        if not ok and callbackError == nil then
            callbackError = 'closing episode: ' .. tostring(err)
        end
    end
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<one line per #NN: what you had just done (card name if known)')
    print(' and what the animation looked like>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d duel_frames: %d episodes: %d '
                        .. 'control_hits: %d dispatch_hits: %d '
                        .. 'foreign_module_hits: %d pool_full: %d',
                        frames, duelFrames, episodeCount, controlHits,
                        dispatchHits, foreignHits, poolFull))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    if controlHits > 0 and dispatchHits == 0 then
        print('verdict: requests ran but the NA duel_effects dispatcher was '
              .. 'never reached; check foreign_module_hits')
    end
    print('')
    for id = 0, 24 do
        local e = byEffect[id]
        if e ~= nil then
            local creators = {}
            for c in pairs(e.creators) do creators[#creators + 1] = c end
            table.sort(creators)
            print(string.format('effect %02d handler=%s episodes=%s '
                                .. 'created_by=%s', id, handlerText(id),
                                table.concat(e.numbers, ','),
                                table.concat(creators, ',')))
        end
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

-- The allocator takes the first inactive entry, so the script can tell which
-- request the new id will occupy before the dispatcher first sees it.
local function onAllocate()
    local n = PCSX.getRegisters().GPR.n
    local caller = tonumber(n.ra)
    if caller >= CREATE[1] and caller < CREATE_END and lastCreate ~= nil
        and lastCreate.frame == frames then
        caller = lastCreate.ra
    end
    for i = 0, REQUEST_COUNT - 1 do
        local req = REQUESTS + i * REQUEST_SIZE
        local flags = u8(req + 0x1c)
        if flags ~= nil and math.floor(flags / FLAG_ACTIVE) % 2 == 0 then
            pending[req] = {id = signed(n.a0), caller = caller}
            return
        end
    end
    poolFull = poolFull + 1
    live(string.format('-- %s request pool full, effect=%d caller=0x%08X',
                       stamp(), signed(n.a0), caller))
end

local function onCreate()
    lastCreate = {ra = tonumber(PCSX.getRegisters().GPR.n.ra), frame = frames}
end

local function onDispatch()
    if not moduleResident() then
        foreignHits = foreignHits + 1
        return
    end
    dispatchHits = dispatchHits + 1
    local n = PCSX.getRegisters().GPR.n
    local id, phase, req = signed(n.a0), signed(n.a1), tonumber(n.a3)
    if req % 4 ~= 0 or not inRam(req, REQUEST_SIZE) then
        live(string.format('-- %s IMPLAUSIBLE request=0x%08X effect=%d',
                           stamp(), req, id))
        return
    end
    local ep = episodes[req]
    local start = pending[req]
    if ep ~= nil and (start ~= nil or ep.id ~= id) then
        closeEpisode(req, ep, 'replaced')
        ep = nil
    end
    if ep == nil then
        pending[req] = nil
        if episodeCount >= MAX_EPISODES then return end
        episodeCount = episodeCount + 1
        firstEpisodeFrame = firstEpisodeFrame or frames
        ep = {number = episodeCount, id = id, startFrame = frames, calls = 0,
              phases = {}}
        episodes[req] = ep
        openCount = openCount + 1
        local creator = 'unseen'
        if start ~= nil then
            creator = string.format('0x%08X', start.caller)
            if start.id ~= id then
                creator = creator .. string.format(' (allocated as %d)',
                                                   start.id)
            end
        end
        local e = byEffect[id] or {numbers = {}, creators = {}}
        byEffect[id] = e
        e.numbers[#e.numbers + 1] = string.format('#%02d', ep.number)
        e.creators[creator] = true
        live(string.format(
            '#%02d %s START effect=%d handler=%s phase=%d request=%s '
                .. 'payload=(%s,%s,%s) field_10=%s variant=%s field_1D=%s '
                .. 'flags=%s created_by=%s',
            ep.number, stamp(), id, handlerText(id), phase, slotOf(req),
            show(s16(req)), show(s16(req + 2)), show(s16(req + 4)),
            show(u16(req + 0x10), '0x%04X'), show(s16(req + 0x12)),
            show(u8(req + 0x1d)), show(u8(req + 0x1c), '0x%02X'), creator))
    end
    ep.calls = ep.calls + 1
    ep.lastFrame = frames
    local seen = false
    for _, p in ipairs(ep.phases) do if p == phase then seen = true end end
    if not seen and #ep.phases < 6 then ep.phases[#ep.phases + 1] = phase end
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    for req, ep in pairs(episodes) do
        local flags = u8(req + 0x1c)
        if flags == nil or math.floor(flags / FLAG_ACTIVE) % 2 == 0 then
            closeEpisode(req, ep, 'finished')
        elseif s16(req + 0x18) ~= ep.id then
            closeEpisode(req, ep, 'id changed')
        end
    end
    local mode = u8(MAIN_MODE)
    if mode ~= nil and mode % 32 == DUEL_MODE then
        if not sawDuel then
            live(string.format('-- %s duel started; effects are numbered '
                               .. 'from here', stamp()))
        end
        sawDuel = true
        duelFrames = duelFrames + 1
    elseif sawDuel then
        finish('left the duel')
        return
    end
    if episodeCount >= MAX_EPISODES and openCount == 0 then
        finish('captured ' .. MAX_EPISODES .. ' effects')
    elseif firstEpisodeFrame ~= nil
        and frames - firstEpisodeFrame >= WINDOW_FRAMES then
        finish('three-minute window after the first effect ended')
    elseif duelFrames >= NO_CONTROL_FRAMES and controlHits == 0 then
        finish('ten minutes in a duel without a control hit; the setup is '
               .. 'not working, check the setup line')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

local function guarded(handler)
    return function()
        if done then return end
        local ok, err = pcall(handler)
        if not ok and callbackError == nil then callbackError = tostring(err) end
    end
end

local function install(target, handler)
    local word = u32(target[1])
    if word ~= target[2] then
        mismatches[#mismatches + 1] = string.format(
            '0x%08X expected 0x%08X read %s', target[1], target[2],
            show(word, '0x%08X'))
    end
    breakpoints_duel_effect_ids[#breakpoints_duel_effect_ids + 1] =
        traceAddBreakpoint(target[1], 'Exec', 4,
                           SCRIPT_NAME .. ': ' .. target[3], guarded(handler))
end

install(CONTROL, function() controlHits = controlHits + 1 end)
install(CREATE, onCreate)
install(ALLOCATE, onAllocate)
-- The overlay is usually not loaded yet, so its words are checked per hit.
breakpoints_duel_effect_ids[#breakpoints_duel_effect_ids + 1] =
    traceAddBreakpoint(DISPATCH, 'Exec', 4, SCRIPT_NAME .. ': dispatcher',
                       guarded(onDispatch))

listener_duel_effect_ids = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches; '
                    .. 'start a Free Duel', SCRIPT_NAME,
                    #breakpoints_duel_effect_ids, #mismatches))
