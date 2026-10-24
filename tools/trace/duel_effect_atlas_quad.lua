-- duel_effect_atlas_quad.lua
--
-- WHAT THIS ANSWERS
--   func_80156D50 (NA 0x80148F98) draws one 32x32 cell of a texture atlas:
--   tpage/CLUT pair 11 of D_8015B748, UV column index*32, grey RGB, at an
--   SVECTOR offset. Nothing in the North American duel_effects module or
--   the executable calls it, or even holds its address. Does it ever run?
--   If it does, who calls it and which atlas indices occur?
--
--   Its sibling func_80156C40 (NA 0x80148E88), which draws slot 10's 64x64
--   rectangle, is called statically from the handlers of effects 6 and 7,
--   so it is the positive control: if the sibling runs and the helper does
--   not, the helper is dead in this route.
--
--   Every effect gets a live #NN START/END row (the same numbering as
--   duel_effect_ids), and every new caller of either helper prints a row the
--   moment it happens, with ra, size, index and offset.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file, and start a Free Duel.
--   3. Keep the Lua console visible. Over about two minutes do as many
--      different things as you can: summon, attack a monster and attack
--      directly, fuse two hand cards, equip, activate every magic or trap card
--      you hold, and let the opponent play.
--   4. The trace prints its document three minutes after the first effect,
--      after 40 effects, or when you leave the duel, whichever comes first.
--   5. Copy the whole document into
--      tools/trace/result/duel_effect_atlas_quad.txt and fill in the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   One line per #NN that has an "atlas" or "sibling" row: what you had just
--   done and what the small squares looked like. Other #NN rows only need a
--   word or two.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_effect_atlas_quad'

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
local ALLOCATE = {0x8002c604, 0x27bdffe8, 'DuelEffect_AllocateRequest'}
-- The instruction after DuelEffect_UpdateRequests' call to the dispatcher.
local RETURN = {0x8002c784, 0x00002021, 'dispatcher return'}
local HELPERS = {
    {0x80148f98, 'atlas', 'func_80156D50'},
    {0x80148e88, 'sibling', 'func_80156C40'},
}

local MAX_EPISODES = 40
local MAX_ROWS = 200
local MAX_CALL_ROWS = 80
local MAX_INDICES = 32
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
local current = nil
local callRows = 0
local callers = {}
local callerOrder = {}
local helperCalls = {}
local dispatchedEffects = {}
local controlHits = 0
local dispatchHits = 0
local foreignHits = 0
local mismatches = {}
local callbackError = nil
local sawDuel = false
local done = false

breakpoints_duel_effect_atlas_quad = {}

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

local function closeEpisode(req, ep, reason)
    local parts = {}
    for _, h in ipairs(HELPERS) do
        if (ep.helpers[h[2]] or 0) > 0 then
            parts[#parts + 1] = h[2] .. 'x' .. ep.helpers[h[2]]
        end
    end
    live(string.format('#%02d %s END   effect=%d frames=%d calls=%d '
                       .. 'helpers=%s reason=%s', ep.number, stamp(), ep.id,
                       frames - ep.startFrame, ep.calls,
                       #parts > 0 and table.concat(parts, ',') or 'none',
                       reason))
    episodes[req] = nil
    openCount = openCount - 1
    if current == ep then current = nil end
end

local function sortedKeys(t)
    local keys = {}
    for k in pairs(t) do keys[#keys + 1] = k end
    table.sort(keys)
    return keys
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_duel_effect_atlas_quad) do bp:disable() end
    for req, ep in pairs(episodes) do
        local ok, err = pcall(closeEpisode, req, ep, 'still running')
        if not ok and callbackError == nil then
            callbackError = 'closing episode: ' .. tostring(err)
        end
    end
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<one line per #NN with an atlas or sibling row: what you had just')
    print(' done and what the small squares looked like; a word or two for')
    print(' the other #NN rows>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d duel_frames: %d episodes: %d '
                        .. 'control_hits: %d dispatch_hits: %d '
                        .. 'foreign_module_hits: %d atlas_calls: %d '
                        .. 'sibling_calls: %d', frames, duelFrames,
                        episodeCount, controlHits, dispatchHits, foreignHits,
                        helperCalls.atlas or 0, helperCalls.sibling or 0))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    local effects = sortedKeys(dispatchedEffects)
    for i, id in ipairs(effects) do effects[i] = tostring(id) end
    local seen = table.concat(effects, ',')
    if (helperCalls.atlas or 0) > 0 then
        print('verdict: func_80156D50 executed; see the atlas callers below')
    elseif dispatchHits == 0 then
        print('verdict: inconclusive; the NA duel_effects bank never ran')
    elseif (helperCalls.sibling or 0) == 0 then
        print('verdict: func_80156D50 never executed, but neither did the '
              .. 'sibling control (effects seen: ' .. seen .. '); play '
              .. 'effects 6 or 7 to make the negative count')
    else
        print('verdict: func_80156D50 never executed while the bank ran '
              .. dispatchHits .. ' dispatcher calls and the sibling control '
              .. 'ran ' .. helperCalls.sibling .. ' times (effects seen: '
              .. seen .. ')')
    end
    print('')
    for _, key in ipairs(callerOrder) do
        local c = callers[key]
        local indices = sortedKeys(c.indices)
        for i, v in ipairs(indices) do indices[i] = tostring(v) end
        if c.kind ~= 'atlas' then indices = {'n/a'} end
        print(string.format('caller %s %s ra=0x%08X calls=%d sizes=%s '
                            .. 'indices=%s%s effects=%s', c.kind, c.name,
                            c.ra, c.calls,
                            table.concat(sortedKeys(c.sizes), ','),
                            table.concat(indices, ','),
                            c.moreIndices and '+' or '',
                            table.concat(c.effects, ',')))
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onHelper(h)
    if not moduleResident() then
        foreignHits = foreignHits + 1
        return
    end
    helperCalls[h[2]] = (helperCalls[h[2]] or 0) + 1
    local n = PCSX.getRegisters().GPR.n
    local ra, offset = tonumber(n.ra), tonumber(n.a2)
    -- atlas(size, index, offset); sibling(color, size, offset).
    local size, index, color = tonumber(n.a0) % 0x10000, nil, nil
    if h[2] == 'atlas' then
        index = tonumber(n.a1) % 0x10000
    else
        size = tonumber(n.a1) % 0x10000
        color = tonumber(n.a0)
    end
    local ep = current
    if ep ~= nil then ep.helpers[h[2]] = (ep.helpers[h[2]] or 0) + 1 end
    local key = string.format('%s:%08X', h[2], ra)
    local c = callers[key]
    if c == nil then
        c = {kind = h[2], name = h[3], ra = ra, calls = 0, sizes = {},
             indices = {}, indexCount = 0, effects = {}, effectSeen = {}}
        callers[key] = c
        callerOrder[#callerOrder + 1] = key
    end
    c.calls = c.calls + 1
    c.sizes[size] = true
    local newIndex = index ~= nil and c.indices[index] == nil
    if newIndex then
        if c.indexCount < MAX_INDICES then
            c.indices[index] = true
            c.indexCount = c.indexCount + 1
        else
            c.moreIndices = true
            newIndex = false
        end
    end
    local label = ep and string.format('#%02d/e%d', ep.number, ep.id)
        or 'outside'
    local newEffect = not c.effectSeen[label]
    if newEffect then
        c.effectSeen[label] = true
        c.effects[#c.effects + 1] = label
    end
    if not (newEffect or newIndex) or callRows >= MAX_CALL_ROWS then return end
    callRows = callRows + 1
    local offsetText = 'offset=unreadable'
    if offset % 2 == 0 and inRam(offset, 6) then
        offsetText = string.format('offset=(%d,%d,%d)', s16(offset),
                                   s16(offset + 2), s16(offset + 4))
    end
    local argText = 'index=' .. tostring(index)
    if index == nil then
        argText = 'rgb=unreadable'
        if inRam(color, 3) then
            argText = string.format('rgb=(%d,%d,%d)', u8(color),
                                    u8(color + 1), u8(color + 2))
        end
    end
    live(string.format('%s %s %s %s ra=0x%08X size=%d %s %s',
                       ep and string.format('#%02d', ep.number) or '--',
                       stamp(), h[2], h[3], ra, size, argText, offsetText))
end

local function onAllocate()
    for i = 0, REQUEST_COUNT - 1 do
        local req = REQUESTS + i * REQUEST_SIZE
        local flags = u8(req + 0x1c)
        if flags ~= nil and math.floor(flags / FLAG_ACTIVE) % 2 == 0 then
            pending[req] = true
            return
        end
    end
end

local function onDispatch()
    current = nil
    if not moduleResident() then
        foreignHits = foreignHits + 1
        return
    end
    dispatchHits = dispatchHits + 1
    local n = PCSX.getRegisters().GPR.n
    local id, phase, req = signed(n.a0), signed(n.a1), tonumber(n.a3)
    dispatchedEffects[id] = true
    if req % 4 ~= 0 or not inRam(req, REQUEST_SIZE) then
        live(string.format('-- %s IMPLAUSIBLE request=0x%08X effect=%d',
                           stamp(), req, id))
        return
    end
    local ep = episodes[req]
    if ep ~= nil and (pending[req] or ep.id ~= id) then
        closeEpisode(req, ep, 'replaced')
        ep = nil
    end
    if ep == nil then
        pending[req] = nil
        if episodeCount >= MAX_EPISODES then return end
        episodeCount = episodeCount + 1
        firstEpisodeFrame = firstEpisodeFrame or frames
        ep = {number = episodeCount, id = id, startFrame = frames, calls = 0,
              helpers = {}}
        episodes[req] = ep
        openCount = openCount + 1
        live(string.format('#%02d %s START effect=%d phase=%d', ep.number,
                           stamp(), id, phase))
    end
    ep.calls = ep.calls + 1
    current = ep
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

local function add(address, label, handler)
    breakpoints_duel_effect_atlas_quad[#breakpoints_duel_effect_atlas_quad + 1] =
        traceAddBreakpoint(address, 'Exec', 4, SCRIPT_NAME .. ': ' .. label,
                           guarded(handler))
end

local function install(target, handler)
    local word = u32(target[1])
    if word ~= target[2] then
        mismatches[#mismatches + 1] = string.format(
            '0x%08X expected 0x%08X read %s', target[1], target[2],
            show(word, '0x%08X'))
    end
    add(target[1], target[3], handler)
end

install(CONTROL, function() controlHits = controlHits + 1 end)
install(ALLOCATE, onAllocate)
install(RETURN, function() current = nil end)
-- The overlay is usually not loaded yet, so its words are checked per hit.
add(DISPATCH, 'dispatcher', onDispatch)
for _, h in ipairs(HELPERS) do
    local helper = h
    add(helper[1], helper[2] .. ' ' .. helper[3], function() onHelper(helper) end)
end

listener_duel_effect_atlas_quad = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches; '
                    .. 'start a Free Duel', SCRIPT_NAME,
                    #breakpoints_duel_effect_atlas_quad, #mismatches))
