-- model_slot_dc0_watch.lua
--
-- WHAT THIS ANSWERS
--   ModelSlot.field_DC0 (+0xDC0, eight bytes per model slot) is read by
--   Model_DrawSlot every frame. Static reading splits it as:
--     bytes 0..2  target RGB      Model_SetSlotTintTarget, scene fades (+-2)
--     byte  3     blend selector  0 opaque, 1..4 PSX rate 0..3, 5 additive
--                                 plus a second-pass CLUT word
--     bytes 4..6  current RGB     stepped toward 0..2 while byte 7 % 6 != 0
--     byte  7     fade/one-shot   5 from Model_InitSlot flag 0x80, 6 makes
--                                 Model_UpdateSlotControl queue an endpoint
--   None of these is named, because nobody has matched the values to what
--   is on screen. Which values does each slot pass through when a monster
--   attacks, dies, fades in or out, or the scene darkens?
--
--   Each 3-D scene (one or more model slots active) is a live #NN episode.
--   Printed the moment they happen:
--     * every Model_SetSlotTintTarget call (first per scene per caller and
--       arguments; repeats are counted),
--     * every change of byte 3 or byte 7, with the writing function,
--     * the first store per scene from each writer to the colour bytes,
--     * every finished change of a slot's eight bytes: from, to, and how
--       many frames it took ("ramp" rows).
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file.
--   3. Start a Free Duel. Play a few attacks so several 3-D battles run:
--      at least one where the defending monster is destroyed and, if you
--      can, one where your own attacker is destroyed instead.
--   4. It prints its document after 8 scenes, four minutes after the first
--      scene, after 15 minutes in total, or on reset.
--   5. Copy the whole document into tools/trace/result/model_slot_dc0_watch.txt
--      and fill in the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   One line per #NN: the two monsters, who won, and anything that faded,
--   flashed, darkened or turned see-through (which monster, roughly when).

local ffi = require('ffi')

local SCRIPT_NAME = 'model_slot_dc0_watch'
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
local FPS = 60
local SLOTS = 0x800f2c40
local SLOT_SIZE = 0xe20
local SLOT_COUNT = 3
local DC0 = 0xdc0
local IS_ACTIVE = 0xe1f

local CONTROL = {0x8001306c, 0x27bdffe0, 'control Main_RunFrameServices'}
local SET_TINT_TARGET = {0x80059590, 0x000410c0, 'Model_SetSlotTintTarget'}
local DRAW_SLOT_WORD = {0x800540b4, 0x27bdf738, 'Model_DrawSlot'}

-- Resident model functions, so rows name their writer or caller.
local FUNCTIONS = {
    {0x8004cb0c, 0x628, 'Model_InitSlot'},
    {0x8004e9a0, 0x160, 'Model_StepBackgroundVramAnimation'},
    {0x8004eb00, 0x132c, 'Model_UpdateAnimatedBattle'},
    {0x8004fe2c, 0x758, 'Model_UpdateExodiaPresentation'},
    {0x80050584, 0x24c, 'Model_UpdateCreditsMonsterSlot'},
    {0x800507d0, 0x754, 'Model_UpdateCreditsPresentation'},
    {0x80050f24, 0x148, 'Model_StartBattleSlotAction'},
    {0x80051350, 0x6f8, 'Model_SeparateViewFromSlots'},
    {0x80051a48, 0xae0, 'Model_UpdateCamera'},
    {0x800528ac, 0x480, 'Model_UpdateTintRequests'},
    {0x80053248, 0x190, 'Model_SetSlotProperties'},
    {0x800533d8, 0xe0, 'Model_ResetSceneState'},
    {0x800540b4, 0x1634, 'Model_DrawSlot'},
    {0x800556e8, 0x2ec, 'Model_UpdateSlotAnimation'},
    {0x800559d4, 0x748, 'Model_UpdateSlotControl'},
    {0x8005611c, 0x134, 'Model_ResetSlot'},
    {0x80056250, 0x2b4, 'Model_LoadSlot'},
    {0x80056504, 0x324, 'Model_LoadMonsterMerge'},
    {0x80056d7c, 0x7c8, 'Model_LoadMonsterMergeStage'},
    {0x80057af4, 0x32c, 'Model_ControlSlotAnimation'},
    {0x800582c0, 0x174, 'Model_ApplyTextureTint'},
    {0x80058838, 0x100, 'Model_QueueTintRequestForParts'},
    {0x80058938, 0x144, 'Model_QueueTintRequest'},
    {0x800590dc, 0x58, 'Model_DrawSlotUnitsOnly'},
    {0x80059520, 0x70, 'Model_GetSlotTintDescriptor'},
    {0x80059590, 0x38, 'Model_SetSlotTintTarget'},
    {0x800595c8, 0xd4, 'Model_SetSlotAmbient'},
    {0x8005b5fc, 0x24, 'Model_FillWords'},
    {0x8006c37c, 0x9fc, 'Model_UpdateDiscEffect'},
    {0x8006cd78, 0x243c, 'Model_UpdateBurstEffect'},
    {0x8006f1b4, 0x13d8, 'Model_UpdateSparkEffect'},
}

local MAX_SCENES = 8
local MAX_ROWS = 400
local MAX_FLAG_ROWS = 120
local MAX_COLOUR_ROWS = 80
local MAX_CALL_ROWS = 80
local MAX_RAMP_ROWS = 120
local WINDOW_FRAMES = 240 * FPS
local NO_CONTROL_FRAMES = 300 * FPS
local TIMEOUT_FRAMES = 900 * FPS
local STORE_WIDTH = {[0x28] = 1, [0x29] = 2, [0x2b] = 4}

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr ~= nil and addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if mem == nil or not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function u32(addr)
    if mem == nil or not inRam(addr, 4) or addr % 4 ~= 0 then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local function show(v, fmt)
    if v == nil then return 'unreadable' end
    return string.format(fmt or '%d', v)
end

local frames = 0
local rows, dropped = {}, 0
local flagRows, colourRows, callRows, rampRows = 0, 0, 0, 0
local sceneCount = 0
local scene = nil
local firstSceneFrame = nil
local controlHits, tintTargetCalls, storeCount, undecoded = 0, 0, 0, 0
local stores, storeOrder = {}, {}
local calls, callOrder = {}, {}
local snapshots, ramps = {}, {}
local mismatches = {}
local callbackError = nil
local done = false

breakpoints_model_slot_dc0_watch = {}

local function stamp()
    return string.format('t=%6.1fs', frames / FPS)
end

local function live(text)
    print(text)
    if #rows < MAX_ROWS then rows[#rows + 1] = text else dropped = dropped + 1 end
end

local function tag()
    return scene and string.format('#%02d', scene.number) or '--'
end

local function functionName(pc)
    for _, f in ipairs(FUNCTIONS) do
        if pc >= f[1] and pc < f[1] + f[2] then return f[3] end
    end
    return 'unknown'
end

local function slotBase(index)
    return SLOTS + index * SLOT_SIZE + DC0
end

local function snapshot(index)
    local bytes = {}
    for k = 0, 7 do
        local v = u8(slotBase(index) + k)
        if v == nil then return nil end
        bytes[k + 1] = v
    end
    return bytes
end

local function hex(bytes)
    if bytes == nil then return 'unreadable' end
    local parts = {}
    for k = 1, 8 do parts[k] = string.format('%02X', bytes[k]) end
    return parts[1] .. parts[2] .. parts[3] .. ':' .. parts[4] .. ':'
        .. parts[5] .. parts[6] .. parts[7] .. ':' .. parts[8]
end

local function activeMask()
    local mask = ''
    for i = 0, SLOT_COUNT - 1 do
        local v = u8(SLOTS + i * SLOT_SIZE + IS_ACTIVE)
        mask = mask .. ((v ~= nil and v ~= 0) and tostring(i) or '-')
    end
    return mask
end

local function count(kinds, order, key)
    if kinds[key] == nil then
        kinds[key] = 0
        order[#order + 1] = key
    end
    kinds[key] = kinds[key] + 1
end

local function closeRamp(index, reason)
    local ramp = ramps[index]
    if ramp == nil then return end
    ramps[index] = nil
    if rampRows >= MAX_RAMP_ROWS then return end
    rampRows = rampRows + 1
    live(string.format('%s %s ramp slot=%d %s -> %s frames=%d%s', ramp.tag,
                       stamp(), index, hex(ramp.from), hex(ramp.to),
                       ramp.last - ramp.start + 1,
                       reason and (' ' .. reason) or ''))
end

local function closeScene(reason)
    for i = 0, SLOT_COUNT - 1 do closeRamp(i, 'open') end
    live(string.format('#%02d %s END   frames=%d tint_target_calls=%d '
                       .. 'flag_changes=%d reason=%s', scene.number, stamp(),
                       frames - scene.startFrame, scene.calls, scene.flags,
                       reason))
    scene = nil
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_model_slot_dc0_watch) do
        pcall(function() bp:disable() end)
    end
    if scene ~= nil then
        local ok, err = pcall(closeScene, 'still running')
        if not ok and callbackError == nil then
            callbackError = 'closing scene: ' .. tostring(err)
        end
    end
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<one line per #NN: the two monsters, who won, and what faded,')
    print(' flashed, darkened or turned see-through, roughly when>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d scenes: %d control_hits: %d '
                        .. 'tint_target_calls: %d dc0_stores: %d '
                        .. 'undecoded_stores: %d dropped_rows: %d',
                        frames, sceneCount, controlHits, tintTargetCalls,
                        storeCount, undecoded, dropped))
    print('bytes are shown as RGB(0..2):byte3:RGB(4..6):byte7')
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    print('')
    for _, key in ipairs(callOrder) do
        print(string.format('tint_target %s calls=%d', key, calls[key]))
    end
    for _, key in ipairs(storeOrder) do
        local s = stores[key]
        print(string.format('store %s count=%d changed=%d values=%s', key,
                            s.count, s.changed, s.values))
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onTintTarget()
    local n = PCSX.getRegisters().GPR.n
    local ra, sp = tonumber(n.ra), tonumber(n.sp)
    local slot, mode = tonumber(n.a0), tonumber(n.a1)
    tintTargetCalls = tintTargetCalls + 1
    if scene ~= nil then scene.calls = scene.calls + 1 end
    local key = string.format('caller=%s(0x%08X) slot=%d mode=%d rgb=%s/%s/%s',
                              functionName(ra), ra, slot, mode % 0x100,
                              show(tonumber(n.a2) % 0x100),
                              show(tonumber(n.a3) % 0x100),
                              show(u32(sp + 0x10) and u32(sp + 0x10) % 0x100))
    count(calls, callOrder, key)
    local sceneKey = 'call ' .. key
    if scene == nil or scene.seen[sceneKey] then return end
    scene.seen[sceneKey] = true
    if callRows >= MAX_CALL_ROWS then return end
    callRows = callRows + 1
    live(string.format('%s %s tint_target %s', tag(), stamp(), key))
end

-- A write breakpoint fires before the store, so memory still holds the old
-- bytes. The store is decoded from the instruction at pc.
local function onStore(index)
    local regs = PCSX.getRegisters()
    local pc = tonumber(regs.pc)
    local w = u32(pc)
    storeCount = storeCount + 1
    local width = w and STORE_WIDTH[math.floor(w / 0x4000000)]
    if width == nil then
        undecoded = undecoded + 1
        local key = string.format('pc=0x%08X %s slot=%d byte=? width=?', pc,
                                  functionName(pc), index)
        if stores[key] == nil then
            stores[key] = {count = 0, changed = 0, values = 'undecoded'}
            storeOrder[#storeOrder + 1] = key
        end
        stores[key].count = stores[key].count + 1
        return
    end
    local base = tonumber(regs.GPR.r[math.floor(w / 0x200000) % 32])
    local value = tonumber(regs.GPR.r[math.floor(w / 0x10000) % 32])
    local imm = w % 0x10000
    if imm >= 0x8000 then imm = imm - 0x10000 end
    local addr = (base + imm) % 0x100000000
    local first = slotBase(index)
    local fn = functionName(pc)
    if fn == 'Model_FillWords' then
        fn = fn .. '<' .. functionName(tonumber(regs.GPR.n.ra))
    end
    for b = 0, width - 1 do
        local k = addr + b - first
        if k >= 0 and k <= 7 then
            local new = math.floor(value / 2 ^ (8 * b)) % 0x100
            local old = u8(first + k)
            local key = string.format('pc=0x%08X %s slot=%d byte=%d', pc, fn,
                                      index, k)
            local s = stores[key]
            if s == nil then
                s = {count = 0, changed = 0, values = '', seen = {}, n = 0}
                stores[key] = s
                storeOrder[#storeOrder + 1] = key
            end
            s.count = s.count + 1
            if old ~= new then s.changed = s.changed + 1 end
            if not s.seen[new] and s.n < 8 then
                s.seen[new] = true
                s.n = s.n + 1
                s.values = s.values .. (s.values == '' and '' or ',')
                    .. string.format('%02X', new)
            end
            if (k == 3 or k == 7) and old ~= new then
                if scene ~= nil then scene.flags = scene.flags + 1 end
                if flagRows < MAX_FLAG_ROWS then
                    flagRows = flagRows + 1
                    live(string.format('%s %s byte%d slot=%d %s -> %02X by %s '
                                       .. 'pc=0x%08X', tag(), stamp(), k,
                                       index, show(old, '%02X'), new, fn, pc))
                end
            elseif k ~= 3 and k ~= 7 then
                local sceneKey = string.format('colour %08X %d %d', pc,
                                               index, k < 3 and 0 or 4)
                local seen = scene and scene.seen or {}
                if not seen[sceneKey] and colourRows < MAX_COLOUR_ROWS then
                    seen[sceneKey] = true
                    colourRows = colourRows + 1
                    live(string.format('%s %s colour slot=%d bytes=%s by %s '
                                       .. 'pc=0x%08X store=%s->%02X before=%s',
                                       tag(), stamp(), index,
                                       k < 3 and '0..2' or '4..6', fn, pc,
                                       show(old, '%02X'), new,
                                       hex(snapshot(index))))
                end
            end
        end
    end
end

local function same(a, b)
    if a == nil or b == nil then return a == b end
    for k = 1, 8 do if a[k] ~= b[k] then return false end end
    return true
end

local function pollSlots()
    for i = 0, SLOT_COUNT - 1 do
        local now = snapshot(i)
        local before = snapshots[i]
        if before ~= nil and now ~= nil and not same(before, now) then
            local ramp = ramps[i]
            if ramp == nil then
                ramps[i] = {from = before, to = now, start = frames,
                            last = frames, tag = tag()}
            else
                ramp.to, ramp.last = now, frames
            end
        elseif ramps[i] ~= nil and frames - ramps[i].last >= 2 then
            closeRamp(i)
        end
        snapshots[i] = now
    end
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    local mask = activeMask()
    if scene == nil and mask:find('%d') then
        if sceneCount < MAX_SCENES then
            sceneCount = sceneCount + 1
            firstSceneFrame = firstSceneFrame or frames
            scene = {number = sceneCount, startFrame = frames, calls = 0,
                     flags = 0, seen = {}}
            live(string.format('#%02d %s START active=%s mode=%s '
                               .. 'dc0=%s|%s|%s', sceneCount, stamp(), mask,
                               show(u8(MAIN_MODE)), hex(snapshot(0)),
                               hex(snapshot(1)), hex(snapshot(2))))
        end
    elseif scene ~= nil and not mask:find('%d') then
        closeScene('no slot active')
    end
    pollSlots()
    if sceneCount >= MAX_SCENES and scene == nil then
        finish('captured ' .. MAX_SCENES .. ' scenes')
    elseif firstSceneFrame ~= nil
        and frames - firstSceneFrame >= WINDOW_FRAMES then
        finish('four-minute window after the first scene ended')
    elseif frames >= NO_CONTROL_FRAMES and controlHits == 0 then
        finish('five minutes without a control hit; the setup is not '
               .. 'working, check the setup line')
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
    breakpoints_model_slot_dc0_watch[#breakpoints_model_slot_dc0_watch + 1] =
        traceAddBreakpoint(address, kind, width, SCRIPT_NAME .. ': ' .. label,
                           guarded(handler))
end

local function check(target)
    local word = u32(target[1])
    if word ~= target[2] then
        mismatches[#mismatches + 1] = string.format(
            '%s 0x%08X expected 0x%08X read %s', target[3], target[1],
            target[2], show(word, '0x%08X'))
    end
end

check(CONTROL)
check(SET_TINT_TARGET)
check(DRAW_SLOT_WORD)
add(CONTROL[1], 'Exec', 4, CONTROL[3], function()
    controlHits = controlHits + 1
end)
add(SET_TINT_TARGET[1], 'Exec', 4, SET_TINT_TARGET[3], onTintTarget)
for i = 0, SLOT_COUNT - 1 do
    local index = i
    add(slotBase(index), 'Write', 8, 'slot' .. index .. ' field_DC0',
        function() onStore(index) end)
end

listeners_model_slot_dc0_watch = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        finish('reset; reload the script and start again')
    end),
}

print(string.format('%s: %d breakpoints installed, %d signature mismatches; '
                    .. 'start a Free Duel and play a few attacks', SCRIPT_NAME,
                    #breakpoints_model_slot_dc0_watch, #mismatches))
