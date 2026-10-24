-- duel_effect_textures.lua
--
-- WHAT THIS ANSWERS
--   The duel-effect bank keeps three texture tables, still address-named:
--
--   D_8015A1E4  (NA 0x8015A270) 21 GsIMAGE records: where each effect
--               texture sits in VRAM.
--   D_8015A430  (NA 0x8015A4BC) 21 blend modes, overlapped by five variant
--               GsIMAGE records that effect 11 picks from by phase.
--   D_8015B748  (NA 0x8015B7E0) the 21 tpage/CLUT word pairs the dispatcher
--               builds from the first two, read whenever a quad is drawn.
--               Pair 10 feeds func_80156C40 and pair 11 the atlas-quad
--               helper func_80156D50.
--
--   Which visible texture is each slot? The script prints every slot's VRAM
--   rectangle once, then numbers each effect live (#NN, the same START/END
--   rows as duel_effect_ids) and prints the first time that effect reads a
--   slot, a variant image or an image record, with the reading code. Seeing
--   "#05 lightning" next to "#05 reads slot 3" names slot 3.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file, and start a Free Duel.
--   3. Keep the Lua console visible. Over about two minutes do as many
--      different things as you can: summon, attack a monster and attack
--      directly, fuse two hand cards, equip, activate every magic or trap card
--      you hold, and let the opponent play.
--   4. Optional, once the "slot table" rows appear: open Debug -> GPU -> VRAM
--      viewer and look at two or three of the listed rectangles.
--   5. The trace prints its document three minutes after the first effect,
--      after 40 effects, or when you leave the duel, whichever comes first.
--   6. Copy the whole document into tools/trace/result/duel_effect_textures.txt
--      and fill in the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   One line per #NN: what you had just done and what was drawn (sparks,
--   a ring, stars, a sword, numbers, ...). If you looked at VRAM, one line
--   per slot you checked: "slot 3: a white star".

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_effect_textures'

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

local IMAGES = 0x8015a270
local IMAGE_SIZE = 28
local SLOT_COUNT = 21
local MODES = 0x8015a4bc
local VARIANTS = MODES + 0x28
local VARIANT_COUNT = 5
local WORDS = 0x8015b7e0
-- func_8014F564 converts every image into its word pair on the first call
-- of each effect; its reads say nothing about the effect.
local INIT_FROM, INIT_TO = 0x8015013c, 0x801501a8

local MAX_EPISODES = 40
local MAX_ROWS = 400
local MAX_READ_ROWS = 160
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
local resident = false
local tableDumped = false
local dumpDue = false
local readRows = 0
local readsSeen = {}
local slotUsers = {}
local controlHits = 0
local dispatchHits = 0
local foreignHits = 0
local tableReads = 0
local mismatches = {}
local callbackError = nil
local sawDuel = false
local done = false

breakpoints_duel_effect_textures = {}

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

local DEPTHS = {[0] = '4bit', '8bit', '15bit', 'reserved'}

local function texels(pmode, pw)
    if pw == nil or pmode == nil then return nil end
    local depth = pmode % 4
    if depth == 0 then return pw * 4 end
    if depth == 1 then return pw * 2 end
    return pw
end

local function imageText(rec)
    local pmode, pw, ph = u32(rec), u16(rec + 8), u16(rec + 10)
    return string.format('image vram=(%s,%s) %sx%s halfwords = %sx%s texels '
                         .. '%s, clut vram=(%s,%s) %s colors',
                         show(s16(rec + 4)), show(s16(rec + 6)), show(pw),
                         show(ph), show(texels(pmode, pw)), show(ph),
                         pmode and DEPTHS[pmode % 4] or 'unreadable',
                         show(s16(rec + 16)), show(s16(rec + 18)),
                         show(u16(rec + 20)))
end

local function wordsText(slot)
    local page, clut = u16(WORDS + slot * 4), u16(WORDS + slot * 4 + 2)
    if page == nil or clut == nil then return 'words unreadable' end
    return string.format('tpage=0x%04X (x=%d y=%d %s abr=%d) clut=0x%04X '
                         .. '(x=%d y=%d)', page, (page % 16) * 64,
                         math.floor(page / 16) % 2 * 256,
                         DEPTHS[math.floor(page / 128) % 4],
                         math.floor(page / 32) % 4, clut, (clut % 64) * 16,
                         math.floor(clut / 64) % 512)
end

local function dumpTable()
    tableDumped = true
    live(string.format('-- %s slot table (VRAM coordinates in halfwords)',
                       stamp()))
    for slot = 0, SLOT_COUNT - 1 do
        live(string.format('slot %02d %s mode=%s %s', slot, wordsText(slot),
                           show(u16(MODES + slot * 2)),
                           imageText(IMAGES + slot * IMAGE_SIZE)))
    end
    for v = 0, VARIANT_COUNT - 1 do
        live(string.format('variant %d %s', v,
                           imageText(VARIANTS + v * IMAGE_SIZE)))
    end
end

local function closeEpisode(req, ep, reason)
    local used = {}
    for _, key in ipairs(ep.used) do
        used[#used + 1] = key .. 'x' .. ep.reads[key]
    end
    live(string.format('#%02d %s END   effect=%d frames=%d calls=%d '
                       .. 'reads=%s reason=%s', ep.number, stamp(), ep.id,
                       frames - ep.startFrame, ep.calls,
                       #used > 0 and table.concat(used, ',') or 'none',
                       reason))
    episodes[req] = nil
    openCount = openCount - 1
    if current == ep then current = nil end
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_duel_effect_textures) do bp:disable() end
    for req, ep in pairs(episodes) do
        local ok, err = pcall(closeEpisode, req, ep, 'still running')
        if not ok and callbackError == nil then
            callbackError = 'closing episode: ' .. tostring(err)
        end
    end
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<one line per #NN: what you had just done and what was drawn;')
    print(' if you looked at VRAM, one line per slot you checked>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d duel_frames: %d episodes: %d '
                        .. 'control_hits: %d dispatch_hits: %d '
                        .. 'foreign_module_hits: %d table_reads: %d',
                        frames, duelFrames, episodeCount, controlHits,
                        dispatchHits, foreignHits, tableReads))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    if not tableDumped then
        print('verdict: the slot table was never built; no NA duel_effects '
              .. 'texture initialization was seen')
    end
    print('')
    local keys = {}
    for key in pairs(slotUsers) do keys[#keys + 1] = key end
    table.sort(keys)
    for _, key in ipairs(keys) do
        local u = slotUsers[key]
        local pcs = {}
        for pc in pairs(u.pcs) do pcs[#pcs + 1] = string.format('0x%08X', pc) end
        table.sort(pcs)
        print(string.format('%s used_by=%s pcs=%s', key,
                            table.concat(u.effects, ','),
                            table.concat(pcs, ',')))
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

-- One live row the first time an effect touches a slot; afterwards only a
-- count, because quads read their pair every frame.
local function noteRead(key, describe)
    tableReads = tableReads + 1
    local ep = current
    local seenKey = (ep and ep.number or 0) .. ':' .. key
    if ep ~= nil then
        if ep.reads[key] == nil then
            ep.reads[key] = 0
            ep.used[#ep.used + 1] = key
        end
        ep.reads[key] = ep.reads[key] + 1
    end
    if readsSeen[seenKey] then return end
    readsSeen[seenKey] = true
    local regs = PCSX.getRegisters()
    local pc, ra = tonumber(regs.pc), tonumber(regs.GPR.n.ra)
    local u = slotUsers[key] or {effects = {}, pcs = {}}
    slotUsers[key] = u
    u.effects[#u.effects + 1] = ep and string.format('#%02d/e%d', ep.number,
                                                    ep.id) or 'outside'
    u.pcs[pc] = true
    if readRows >= MAX_READ_ROWS then return end
    readRows = readRows + 1
    live(string.format('%s %s effect=%s reads %s pc=0x%08X ra=0x%08X %s',
                       ep and string.format('#%02d', ep.number) or '--',
                       stamp(), ep and tostring(ep.id) or 'none', key, pc, ra,
                       describe()))
end

local function onWords(address)
    if not resident then return end
    local slot = math.floor((address - WORDS) / 4)
    if slot < 0 or slot >= SLOT_COUNT then return end
    noteRead(string.format('slot%02d', slot), function()
        return wordsText(slot)
    end)
end

local function onVariant(address)
    if not resident then return end
    local v = math.floor((address - VARIANTS) / IMAGE_SIZE)
    if v < 0 or v >= VARIANT_COUNT then return end
    noteRead('variant' .. v, function()
        return imageText(VARIANTS + v * IMAGE_SIZE)
    end)
end

local function onImage(address)
    if not resident then return end
    local pc = tonumber(PCSX.getRegisters().pc)
    if pc >= INIT_FROM and pc < INIT_TO then return end
    local slot = math.floor((address - IMAGES) / IMAGE_SIZE)
    if slot < 0 or slot >= SLOT_COUNT then return end
    noteRead(string.format('image%02d', slot), function()
        return imageText(IMAGES + slot * IMAGE_SIZE)
    end)
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
    resident = moduleResident()
    if not resident then
        foreignHits = foreignHits + 1
        return
    end
    dispatchHits = dispatchHits + 1
    local n = PCSX.getRegisters().GPR.n
    local id, phase, req = signed(n.a0), signed(n.a1), tonumber(n.a3)
    if phase >= 0 and not tableDumped then dumpDue = true end
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
              reads = {}, used = {}}
        episodes[req] = ep
        openCount = openCount + 1
        live(string.format('#%02d %s START effect=%d phase=%d variant=%s',
                           ep.number, stamp(), id, phase,
                           show(s16(req + 0x12))))
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
    resident = moduleResident()
    if dumpDue and resident then
        dumpDue = false
        dumpTable()
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
    return function(...)
        if done then return end
        local ok, err = pcall(handler, ...)
        if not ok and callbackError == nil then callbackError = tostring(err) end
    end
end

local function add(address, kind, width, label, handler)
    breakpoints_duel_effect_textures[#breakpoints_duel_effect_textures + 1] =
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
add(WORDS, 'Read', SLOT_COUNT * 4, 'D_8015B748 reads', onWords)
add(VARIANTS, 'Read', VARIANT_COUNT * IMAGE_SIZE, 'variant image reads',
    onVariant)
add(IMAGES, 'Read', SLOT_COUNT * IMAGE_SIZE, 'D_8015A1E4 reads', onImage)

listener_duel_effect_textures = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches; '
                    .. 'start a Free Duel', SCRIPT_NAME,
                    #breakpoints_duel_effect_textures, #mismatches))
