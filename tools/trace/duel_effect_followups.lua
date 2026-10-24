-- duel_effect_followups.lua
--
-- WHAT THIS ANSWERS
--   Three doubts left in the duel_effects bank after #7865. One route covers
--   all three, so they share one script instead of three overlapping ones.
--
--   A. DuelEffect_DrawAtlasColumnQuad (NA 0x80148F98) has no static caller.
--      Does it ever run, from where, with which atlas index? Its sibling
--      DuelEffect_DrawFixedTextureQuad (NA 0x80148E88) is called only by
--      effects 6 and 7 (direct-damage spells), so it is the positive
--      control the previous trace never reached.
--   B. Effect 17 (DuelEffect_RunBoardDestruction) passes a NULL offset to
--      DuelEffect_DrawLayeredTileQuad (NA 0x80147E08) at effect_17.c:232
--      (burst, every phase) and :259 (paths, phase 1 only). addVector still
--      reads the offset, so the quads are moved by whatever sits at RAM
--      0..5. Dark Hole (phase 0) is covered by duel_effect_null_offset.lua
--      (#7872), so this part reports only phase 1 (Dragon Capture Jar):
--      which calls pass NULL, which XYZ do they read, and what is drawn?
--   C. Effect 21 (Swords of Revealing Light, func_8014E3EC, NA 0x8014EFC4)
--      initializes only tiles[8] of the ten its draw loop consumes. What
--      do tiles[0..9] hold before init and at the first draw, and does
--      any other code write them in between?
--
--   Every effect gets a live #NN START/END row; every new finding prints a
--   row the moment it happens.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file, and start a Free Duel with a
--      deck holding as many of these as you can:
--        - Sparks, Hinotama, Final Flame, Ookazi or Tremendous Fire
--        - Dragon Capture Jar (with a dragon on the field)
--        - Swords of Revealing Light (best played after a few other effects)
--   3. Play them in any order; 3D battles and leaving the duel do not stop
--      the trace. It prints its document once all three parts are covered,
--      six minutes after the first effect, or after 60 effects.
--   4. Copy the whole document into
--      tools/trace/result/duel_effect_followups.txt and fill in the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   One line per #NN: which card it was and what you saw. For Dragon
--   Capture Jar, say whether a glowing square appeared somewhere odd
--   (for example stuck at the centre or a corner). For the swords, say
--   whether every sword looked right on its first frame.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_effect_followups'
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
    {0x80148f98, 'atlas', 'DuelEffect_DrawAtlasColumnQuad'},
    {0x80148e88, 'sibling', 'DuelEffect_DrawFixedTextureQuad'},
}
local LAYERED = 0x80147e08                  -- DuelEffect_DrawLayeredTileQuad
local SWORDS_ID = 21
local SWORDS_HANDLER = {0x8014efc4, 0x690}  -- func_8014E3EC
local SHARED_BASE_PTR = 0x80010000          -- gRuntime_pSharedWorkBase
local SWORDS_SLOTS = {0x2800, 0x2880}
local TILES, COUNT, ENABLED = 0x50, 0x64, 0x6a

local MAX_EPISODES = 60
local MAX_ROWS = 260
local MAX_NULL_ROWS = 24
local MAX_WRITER_ROWS = 30
local WINDOW_FRAMES = 360 * FPS
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
local helperCalls = {}
local callers = {}
local callerOrder = {}
local nullCallers = {}
local nullOrder = {}
local nullRows = 0
local nullCalls = 0
local offsetCalls = 0
local deferredNullCalls = 0
local seenPhase17 = {}
local swordsBefore = 0
local swordsFirstDraw = 0
local sharedBase = nil
local writers = {}
local writerOrder = {}
local writerRows = 0
local ownWrites = 0
local dispatchedEffects = {}
local controlHits = 0
local dispatchHits = 0
local foreignHits = 0
local mismatches = {}
local callbackError = nil
local sawDuel = false
local inDuel = false
local done = false

breakpoints_duel_effect_followups = {}

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

local function label(ep)
    return ep and string.format('#%02d/e%d', ep.number, ep.id) or 'outside'
end

local function rowTag(ep)
    return ep and string.format('#%02d', ep.number) or '--'
end

local function sortedKeys(t)
    local keys = {}
    for k in pairs(t) do keys[#keys + 1] = k end
    table.sort(keys)
    return keys
end

local function closeEpisode(req, ep, reason)
    local parts = {}
    for _, key in ipairs(sortedKeys(ep.counts)) do
        parts[#parts + 1] = key .. 'x' .. ep.counts[key]
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

local function bump(ep, key)
    if ep ~= nil then ep.counts[key] = (ep.counts[key] or 0) + 1 end
end

-- Part C: one line per swords slot, read straight from shared work.
local function swordsSlot(index)
    local base = u32(SHARED_BASE_PTR)
    if base == nil then return nil, 'base unreadable' end
    local slot = base + SWORDS_SLOTS[index + 1]
    if not inRam(slot, 0x80) or slot % 4 ~= 0 then
        return nil, string.format('base=0x%08X implausible', base)
    end
    local tiles = {}
    for i = 0, 9 do tiles[#tiles + 1] = show(u16(slot + TILES + i * 2)) end
    return slot, string.format('slot%d@0x%08X enabled=%s count=%s tiles=%s',
                               index, slot, show(u16(slot + ENABLED)),
                               show(u16(slot + COUNT)),
                               table.concat(tiles, ','))
end

local function swordsRow(ep, what, index)
    local _, text = swordsSlot(index)
    live(string.format('%s %s swords %s %s', rowTag(ep), stamp(), what, text))
end

local function partsCovered()
    return (helperCalls.sibling or 0) > 0 and seenPhase17[1]
        and swordsFirstDraw > 0
end

-- A write breakpoint fires before the store, so the stored value is decoded
-- from the store instruction itself (sb/sh/sw only; nil otherwise).
local STORE_WIDTH = {[0x28] = 1, [0x29] = 2, [0x2b] = 4}
local function decodeStore(pc)
    local w = u32(pc)
    if w == nil then return nil end
    local width = STORE_WIDTH[math.floor(w / 0x4000000)]
    if width == nil then return nil end
    local gpr = PCSX.getRegisters().GPR.r
    local imm = w % 0x10000
    if imm >= 0x8000 then imm = imm - 0x10000 end
    local addr = (tonumber(gpr[math.floor(w / 0x200000) % 32]) + imm)
        % 0x100000000
    local value = tonumber(gpr[math.floor(w / 0x10000) % 32])
        % (2 ^ (8 * width))
    return addr, width, value
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_duel_effect_followups) do bp:disable() end
    for req, ep in pairs(episodes) do
        local ok, err = pcall(closeEpisode, req, ep, 'still running')
        if not ok and callbackError == nil then
            callbackError = 'closing episode: ' .. tostring(err)
        end
    end
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<one line per #NN: the card you played and what you saw. For')
    print(' Dragon Capture Jar: any glowing square in an odd')
    print(' place? For the swords: did every sword look right at once?>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d duel_frames: %d episodes: %d '
                        .. 'control_hits: %d dispatch_hits: %d '
                        .. 'foreign_module_hits: %d', frames, duelFrames,
                        episodeCount, controlHits, dispatchHits, foreignHits))
    print(string.format('shared_work_base: %s', show(sharedBase, '0x%08X')))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    local effects = sortedKeys(dispatchedEffects)
    for i, id in ipairs(effects) do effects[i] = tostring(id) end
    local seen = table.concat(effects, ',')
    print('effects_seen: ' .. (seen ~= '' and seen or 'none'))

    -- Part A.
    local atlas, sibling = helperCalls.atlas or 0, helperCalls.sibling or 0
    if atlas > 0 then
        print('A verdict: DuelEffect_DrawAtlasColumnQuad executed ' .. atlas
              .. ' times; see the atlas callers below')
    elseif sibling == 0 then
        print('A verdict: inconclusive; the sibling control never ran, so '
              .. 'play Sparks, Hinotama, Final Flame, Ookazi or '
              .. 'Tremendous Fire')
    else
        print('A verdict: DuelEffect_DrawAtlasColumnQuad never executed '
              .. 'while its sibling control ran ' .. sibling .. ' times')
    end
    -- Part B.
    local phases = {}
    for _, p in ipairs(sortedKeys(seenPhase17)) do
        phases[#phases + 1] = tostring(p)
    end
    print(string.format('B: effect17_phases=%s null_offset_calls=%d '
                        .. 'non_null_calls=%d phase0_null_calls_left_to_'
                        .. 'duel_effect_null_offset=%d',
                        #phases > 0 and table.concat(phases, ',') or 'none',
                        nullCalls, offsetCalls, deferredNullCalls))
    -- Part C.
    print(string.format('C: before_init_rows=%d first_draw_rows=%d '
                        .. 'handler_tile_writes=%d other_tile_writers=%d',
                        swordsBefore, swordsFirstDraw, ownWrites,
                        #writerOrder))
    print('')
    for _, key in ipairs(callerOrder) do
        local c = callers[key]
        local indices = sortedKeys(c.indices)
        for i, v in ipairs(indices) do indices[i] = tostring(v) end
        print(string.format('A caller %s %s ra=0x%08X calls=%d sizes=%s '
                            .. 'indices=%s effects=%s', c.kind, c.name, c.ra,
                            c.calls, table.concat(sortedKeys(c.sizes), ','),
                            c.kind == 'atlas' and table.concat(indices, ',')
                                or 'n/a', table.concat(c.effects, ',')))
    end
    for _, key in ipairs(nullOrder) do
        local c = nullCallers[key]
        print(string.format('B null caller ra=0x%08X calls=%d effects=%s '
                            .. 'ram0=%s', c.ra, c.calls,
                            table.concat(c.effects, ','), c.ram0))
    end
    for _, key in ipairs(writerOrder) do
        local w = writers[key]
        print(string.format('C tile writer pc=0x%08X writes=%d effects=%s',
                            w.pc, w.writes, table.concat(w.effects, ',')))
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function noteEffect(list, seenSet, ep)
    local name = label(ep)
    if seenSet[name] then return false end
    seenSet[name] = true
    list[#list + 1] = name
    return true
end

-- Part A: the atlas helper and its sibling control.
local function onHelper(h)
    if not moduleResident() then
        foreignHits = foreignHits + 1
        return
    end
    helperCalls[h[2]] = (helperCalls[h[2]] or 0) + 1
    local n = PCSX.getRegisters().GPR.n
    local ra = tonumber(n.ra)
    -- atlas(size, index, offset); sibling(color, size, offset).
    local size, index = tonumber(n.a0) % 0x10000, nil
    if h[2] == 'atlas' then
        index = tonumber(n.a1) % 0x10000
    else
        size = tonumber(n.a1) % 0x10000
    end
    local ep = current
    bump(ep, h[2])
    local key = string.format('%s:%08X', h[2], ra)
    local c = callers[key]
    if c == nil then
        c = {kind = h[2], name = h[3], ra = ra, calls = 0, sizes = {},
             indices = {}, effects = {}, effectSeen = {}}
        callers[key] = c
        callerOrder[#callerOrder + 1] = key
    end
    c.calls = c.calls + 1
    c.sizes[size] = true
    local newIndex = index ~= nil and c.indices[index] == nil
    if index ~= nil then c.indices[index] = true end
    if not (noteEffect(c.effects, c.effectSeen, ep) or newIndex) then return end
    local offset = tonumber(n.a2)
    local offsetText = 'offset=unreadable'
    if offset % 2 == 0 and inRam(offset, 6) then
        offsetText = string.format('offset=(%d,%d,%d)', s16(offset),
                                   s16(offset + 2), s16(offset + 4))
    end
    live(string.format('%s %s A %s ra=0x%08X size=%d index=%s %s', rowTag(ep),
                       stamp(), h[2], ra, size, tostring(index), offsetText))
end

-- Part B: DuelEffect_DrawLayeredTileQuad(color, size, depth, offset). A NULL offset is read
-- through kuseg 0, which mirrors physical RAM at 0x80000000.
local function onLayered()
    if not moduleResident() then
        foreignHits = foreignHits + 1
        return
    end
    local n = PCSX.getRegisters().GPR.n
    local ep = current
    if tonumber(n.a3) ~= 0 then
        offsetCalls = offsetCalls + 1
        bump(ep, 'layered')
        return
    end
    if ep ~= nil and ep.id == 17 and ep.phase == 0 then
        deferredNullCalls = deferredNullCalls + 1
        return
    end
    bump(ep, 'layered_null')
    nullCalls = nullCalls + 1
    local ra = tonumber(n.ra)
    local ram0 = string.format('(%s,%s,%s)', show(s16(RAM_BASE)),
                               show(s16(RAM_BASE + 2)),
                               show(s16(RAM_BASE + 4)))
    local key = string.format('%08X', ra)
    local c = nullCallers[key]
    if c == nil then
        c = {ra = ra, calls = 0, effects = {}, effectSeen = {}, ram0 = ram0}
        nullCallers[key] = c
        nullOrder[#nullOrder + 1] = key
    end
    c.calls = c.calls + 1
    local changed = c.ram0 ~= ram0
    c.ram0 = ram0
    if not (noteEffect(c.effects, c.effectSeen, ep) or changed)
        or nullRows >= MAX_NULL_ROWS then
        return
    end
    nullRows = nullRows + 1
    live(string.format('%s %s B null offset ra=0x%08X phase=%s size=%d '
                       .. 'depth=%d reads ram0=%s', rowTag(ep), stamp(), ra,
                       ep and tostring(ep.phase) or '?',
                       tonumber(n.a1) % 0x10000, signed(n.a2), ram0))
end

-- Part C: a store into either swords slot's tiles[0..9].
local function onTileWrite()
    local pc = tonumber(PCSX.getRegisters().pc)
    if pc >= SWORDS_HANDLER[1] and pc < SWORDS_HANDLER[1] + SWORDS_HANDLER[2]
        and moduleResident() then
        ownWrites = ownWrites + 1
        return
    end
    local key = string.format('%08X', pc)
    local w = writers[key]
    if w == nil then
        w = {pc = pc, writes = 0, effects = {}, effectSeen = {}}
        writers[key] = w
        writerOrder[#writerOrder + 1] = key
    end
    w.writes = w.writes + 1
    if not noteEffect(w.effects, w.effectSeen, current)
        or writerRows >= MAX_WRITER_ROWS then
        return
    end
    writerRows = writerRows + 1
    local addr, width, value = decodeStore(pc)
    live(string.format('%s %s C tile write pc=0x%08X addr=%s width=%s '
                       .. 'value=%s', rowTag(current), stamp(), pc,
                       show(addr, '0x%08X'), show(width), show(value, '0x%X')))
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

local function swordsField(index, offset)
    local slot = swordsSlot(index)
    if slot == nil then return nil end
    return u16(slot + offset)
end

-- Phase >= 0 disables an enabled slot, otherwise initialises it. Phases
-- 0/1 start count at 0, so a negative call only draws once count > 0.
local function onSwords(ep, phase)
    ep.swords = ep.swords or {}
    if phase >= 0 then
        local index = math.abs(phase) % 2
        local enabled = swordsField(index, ENABLED)
        if enabled ~= nil and enabled ~= 0 then
            swordsRow(ep, 'disable (slot was enabled)', index)
        else
            swordsBefore = swordsBefore + 1
            swordsRow(ep, 'before init phase=' .. phase, index)
        end
    else
        local index = math.abs(phase + 1) % 2
        local count = swordsField(index, COUNT)
        if count ~= nil and count > 0 and not ep.swords[index] then
            ep.swords[index] = true
            swordsFirstDraw = swordsFirstDraw + 1
            swordsRow(ep, 'first draw', index)
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
        ep = {number = episodeCount, id = id, phase = phase,
              startFrame = frames, calls = 0, counts = {}}
        episodes[req] = ep
        openCount = openCount + 1
        live(string.format('#%02d %s START effect=%d phase=%d', ep.number,
                           stamp(), id, phase))
    end
    ep.calls = ep.calls + 1
    ep.phase = phase
    current = ep
    if id == 17 then seenPhase17[phase] = true end
    if id == SWORDS_ID then onSwords(ep, phase) end
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
    local duel = mode ~= nil and mode % 32 == DUEL_MODE
    if duel then duelFrames = duelFrames + 1 end
    if duel and not inDuel then
        live(string.format('-- %s %s; effects are numbered from here', stamp(),
                           sawDuel and 'back on the duel board'
                               or 'duel started'))
        if not sawDuel then
            local base = u32(SHARED_BASE_PTR)
            if base ~= sharedBase then
                live(string.format('-- %s shared work base is %s, not the '
                                   .. 'watched 0x%08X; tile writes are not '
                                   .. 'watched', stamp(),
                                   show(base, '0x%08X'), sharedBase))
            end
            swordsRow(nil, 'at duel start', 0)
            swordsRow(nil, 'at duel start', 1)
        end
        sawDuel = true
    end
    inDuel = duel
    if partsCovered() and openCount == 0 then
        finish('all three parts covered')
    elseif episodeCount >= MAX_EPISODES and openCount == 0 then
        finish('captured ' .. MAX_EPISODES .. ' effects')
    elseif firstEpisodeFrame ~= nil
        and frames - firstEpisodeFrame >= WINDOW_FRAMES then
        finish('six-minute window after the first effect ended')
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

local function add(address, kind, width, label, handler)
    breakpoints_duel_effect_followups[#breakpoints_duel_effect_followups + 1] =
        traceAddBreakpoint(address, kind, width, SCRIPT_NAME .. ': ' .. label,
                           guarded(handler))
end

local function install(target, handler)
    local word = u32(target[1])
    if word ~= target[2] then
        mismatches[#mismatches + 1] = string.format(
            '0x%08X expected 0x%08X read %s', target[1], target[2],
            show(word, '0x%08X'))
    end
    add(target[1], 'Exec', 4, target[3], handler)
end

install(CONTROL, function() controlHits = controlHits + 1 end)
install(ALLOCATE, onAllocate)
install(RETURN, function() current = nil end)
-- The overlay is usually not loaded yet, so its words are checked per hit.
add(DISPATCH, 'Exec', 4, 'dispatcher', onDispatch)
for _, h in ipairs(HELPERS) do
    local helper = h
    add(helper[1], 'Exec', 4, helper[2] .. ' ' .. helper[3],
        function() onHelper(helper) end)
end
add(LAYERED, 'Exec', 4, 'DuelEffect_DrawLayeredTileQuad', onLayered)
-- The shared work base is set at boot; 0x80100000 is its retail value.
sharedBase = u32(SHARED_BASE_PTR)
if sharedBase == nil or not inRam(sharedBase + 0x2900, 0) then
    sharedBase = 0x80100000
end
for i, offset in ipairs(SWORDS_SLOTS) do
    add(sharedBase + offset + TILES, 'Write', 20,
        'swords slot' .. (i - 1) .. ' tiles', onTileWrite)
end

listener_duel_effect_followups = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches; '
                    .. 'start a Free Duel', SCRIPT_NAME,
                    #breakpoints_duel_effect_followups, #mismatches))
