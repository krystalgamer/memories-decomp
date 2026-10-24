-- model_slot_requests.lua
--
-- WHAT THIS ANSWERS
--   Two model-slot requests whose values have no names yet:
--
--   draw_mode_request (ModelSlot +0xE11). Model_SetSlotDrawMode (0x80059284)
--     writes it; Model_DrawSlot publishes it to gModel_bCurrentDrawMode and
--     resets it to 0 unless it is 4. Static writers: the 3-D battle
--     controller (Model_UpdateAnimatedBattle) asks for 3 on slots 0 and 2,
--     the fusion merge (Model_LoadMonsterMergeStage) for 4,
--     Model_SetSlotAmbient chooses 0 or 3, Model_InitSlot stores
--     flags & 0x7F and Model_DrawSlotUnitsOnly forces 1. Which value is on
--     screen when, in which slot?
--   Tint requests. Model_QueueTintRequest (0x80058938) and its two front
--     ends (ForParts 0x80058838, ForAllParts 0x80059AF8) are called only
--     from per-monster MODEL variant overlays. Both front ends pass a
--     selection whose bit 0 is always clear, so only a direct overlay caller
--     could set it. Which monsters tint which slot and parts, from which
--     colour to which, for how long, and is the selection bit ever set?
--
--   Each 3-D scene (one or more model slots active) is a live #NN episode.
--   Every SetSlotDrawMode call and every tint request prints a row at once;
--   other draw_mode_request stores print a row the first time a
--   (writer, slot, value) combination appears.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file.
--   3. Start a Free Duel. Fuse two monsters once, then attack with a few
--      different monsters so several 3-D battles play.
--   4. It prints its document after 12 scenes, five minutes after the first
--      scene, or after 20 minutes in total.
--   5. Copy the whole document into tools/trace/result/model_slot_requests.txt
--      and fill in the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   One line per #NN: which monsters were on screen, and whether one of
--   them flashed or changed colour (which one, and to what colour).

local ffi = require('ffi')

local SCRIPT_NAME = 'model_slot_requests'
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
local DRAW_MODE = 0xe11
local IS_ACTIVE = 0xe1f

local CONTROL = {0x8001306c, 0x27bdffe0, 'control Main_RunFrameServices'}
local SET_DRAW_MODE = {0x80059284, 0x3c03800f, 'Model_SetSlotDrawMode'}
local QUEUE = {0x80058938, 0x27bdfff8, 'Model_QueueTintRequest'}
local FRONT_ENDS = {
    {0x80058838, 0x27bdffd8, 'ForParts'},
    {0x80059af8, 0x27bdffe0, 'ForAllParts'},
}
-- Named resident writers of draw_mode_request, for the store rows.
local WRITERS = {
    {0x80059284, 0x28, 'Model_SetSlotDrawMode'},
    {0x800540b4, 0x1634, 'Model_DrawSlot'},
    {0x800595c8, 0xd4, 'Model_SetSlotAmbient'},
    {0x8004cb0c, 0x628, 'Model_InitSlot'},
    {0x800590dc, 0x58, 'Model_DrawSlotUnitsOnly'},
}

local MAX_SCENES = 12
local MAX_ROWS = 300
local MAX_CALL_ROWS = 120
local MAX_STORE_ROWS = 60
local WINDOW_FRAMES = 300 * FPS
local NO_CONTROL_FRAMES = 300 * FPS
local TIMEOUT_FRAMES = 1200 * FPS

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr ~= nil and addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function u32(addr)
    if not inRam(addr, 4) or addr % 4 ~= 0 then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local function show(v, fmt)
    if v == nil then return 'unreadable' end
    return string.format(fmt or '%d', v)
end

local frames = 0
local rows = {}
local callRows = 0
local storeRows = 0
local sceneCount = 0
local scene = nil
local firstSceneFrame = nil
local controlHits = 0
local drawModeCalls = 0
local tintCalls = 0
local selectionSet = 0
local storeCount = 0
local stores = {}
local storeOrder = {}
local drawModeKinds = {}
local drawModeOrder = {}
local tintKinds = {}
local tintOrder = {}
local frontEnd = nil
local mismatches = {}
local callbackError = nil
local done = false

breakpoints_model_slot_requests = {}

local function stamp()
    return string.format('t=%6.1fs', frames / FPS)
end

local function live(text)
    print(text)
    if #rows < MAX_ROWS then rows[#rows + 1] = text end
end

local function tag()
    return scene and string.format('#%02d', scene.number) or '--'
end

local function writerName(pc)
    for _, w in ipairs(WRITERS) do
        if pc >= w[1] and pc < w[1] + w[2] then return w[3] end
    end
    return 'unknown'
end

local function activeMask()
    local mask = ''
    for i = 0, SLOT_COUNT - 1 do
        local v = u8(SLOTS + i * SLOT_SIZE + IS_ACTIVE)
        mask = mask .. ((v ~= nil and v ~= 0) and tostring(i) or '-')
    end
    return mask
end

local function slotModes()
    local parts = {}
    for i = 0, SLOT_COUNT - 1 do
        parts[#parts + 1] = show(u8(SLOTS + i * SLOT_SIZE + DRAW_MODE))
    end
    return table.concat(parts, '/')
end

local function count(kinds, order, key)
    if kinds[key] == nil then
        kinds[key] = 0
        order[#order + 1] = key
    end
    kinds[key] = kinds[key] + 1
end

local function closeScene(reason)
    live(string.format('#%02d %s END   frames=%d draw_mode_calls=%d '
                       .. 'tints=%d reason=%s', scene.number, stamp(),
                       frames - scene.startFrame, scene.drawModeCalls,
                       scene.tints, reason))
    scene = nil
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_model_slot_requests) do bp:disable() end
    if scene ~= nil then
        local ok, err = pcall(closeScene, 'still running')
        if not ok and callbackError == nil then
            callbackError = 'closing scene: ' .. tostring(err)
        end
    end
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<one line per #NN: the monsters on screen, and which one flashed')
    print(' or changed colour, to what colour>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d scenes: %d control_hits: %d '
                        .. 'set_draw_mode_calls: %d tint_requests: %d '
                        .. 'selection_bit_set: %d draw_mode_stores: %d',
                        frames, sceneCount, controlHits, drawModeCalls,
                        tintCalls, selectionSet, storeCount))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    print('')
    for _, key in ipairs(drawModeOrder) do
        print(string.format('set_draw_mode %s calls=%d', key,
                            drawModeKinds[key]))
    end
    for _, key in ipairs(tintOrder) do
        print(string.format('tint %s calls=%d', key, tintKinds[key]))
    end
    for _, key in ipairs(storeOrder) do
        print(string.format('store %s count=%d', key, stores[key]))
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onSetDrawMode()
    local n = PCSX.getRegisters().GPR.n
    local index, value, ra = tonumber(n.a0), tonumber(n.a1), tonumber(n.ra)
    drawModeCalls = drawModeCalls + 1
    if scene ~= nil then scene.drawModeCalls = scene.drawModeCalls + 1 end
    local key = string.format('ra=0x%08X slot=%d value=%d', ra, index,
                              value % 0x100)
    count(drawModeKinds, drawModeOrder, key)
    if callRows >= MAX_CALL_ROWS then return end
    callRows = callRows + 1
    live(string.format('%s %s set_draw_mode %s active=%s request_now=%s',
                       tag(), stamp(), key, activeMask(), slotModes()))
end

local function onFrontEnd(f)
    local n = PCSX.getRegisters().GPR.n
    frontEnd = {name = f[3], ra = tonumber(n.ra), a1 = tonumber(n.a1)}
end

local function colour(word)
    word = tonumber(word) or 0
    return string.format('%02X%02X%02X:%02X', word % 0x100,
                         math.floor(word / 0x100) % 0x100,
                         math.floor(word / 0x10000) % 0x100,
                         math.floor(word / 0x1000000) % 0x100)
end

local function onQueue()
    local n = PCSX.getRegisters().GPR.n
    local sp, ra = tonumber(n.sp), tonumber(n.ra)
    local slot, selection = tonumber(n.a0), tonumber(n.a1)
    tintCalls = tintCalls + 1
    if selection % 2 == 1 then selectionSet = selectionSet + 1 end
    if scene ~= nil then scene.tints = scene.tints + 1 end
    local duration = u32(sp + 0x10)
    local maskPtr = u32(sp + 0x14)
    local mask = 'all'
    if maskPtr ~= nil and maskPtr ~= 0 then
        mask = 'unreadable'
        if inRam(maskPtr, 8) then
            local bytes = {}
            for i = 0, 7 do bytes[#bytes + 1] = string.format('%02X', u8(maskPtr + i)) end
            mask = table.concat(bytes, ' ')
        end
    elseif maskPtr == nil then
        mask = 'unreadable'
    end
    local via = 'direct'
    local caller = ra
    if frontEnd ~= nil then
        via = string.format('%s(a1=0x%X)', frontEnd.name, frontEnd.a1)
        caller = frontEnd.ra
        frontEnd = nil
    end
    count(tintKinds, tintOrder, string.format('caller=0x%08X via=%s slot=%d',
                                              caller, via, slot))
    if callRows >= MAX_CALL_ROWS then return end
    callRows = callRows + 1
    live(string.format('%s %s tint caller=0x%08X via=%s slot=%d selection=0x%X '
                       .. 'start=%s end=%s duration=%s mask=%s active=%s',
                       tag(), stamp(), caller, via, slot, selection,
                       colour(n.a2), colour(n.a3), show(duration), mask,
                       activeMask()))
end

-- A write breakpoint fires before the store; sb is the only width that
-- writes this byte in retail, so the value is the low byte of rt.
local function onStore(index)
    local regs = PCSX.getRegisters()
    local pc = tonumber(regs.pc)
    local value = 'unreadable'
    local w = u32(pc)
    if w ~= nil and math.floor(w / 0x4000000) == 0x28 then
        value = tostring(tonumber(regs.GPR.r[math.floor(w / 0x10000) % 32])
                         % 0x100)
    end
    storeCount = storeCount + 1
    local key = string.format('pc=0x%08X %s slot=%d value=%s', pc,
                              writerName(pc), index, value)
    local new = stores[key] == nil
    count(stores, storeOrder, key)
    if not new or storeRows >= MAX_STORE_ROWS then return end
    storeRows = storeRows + 1
    live(string.format('%s %s store %s', tag(), stamp(), key))
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
            scene = {number = sceneCount, startFrame = frames,
                     drawModeCalls = 0, tints = 0}
            live(string.format('#%02d %s START active=%s mode=%s '
                               .. 'request_now=%s', sceneCount, stamp(), mask,
                               show(u8(MAIN_MODE)), slotModes()))
        end
    elseif scene ~= nil and not mask:find('%d') then
        closeScene('no slot active')
    end
    if sceneCount >= MAX_SCENES and scene == nil then
        finish('captured ' .. MAX_SCENES .. ' scenes')
    elseif firstSceneFrame ~= nil
        and frames - firstSceneFrame >= WINDOW_FRAMES then
        finish('five-minute window after the first scene ended')
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
    breakpoints_model_slot_requests[#breakpoints_model_slot_requests + 1] =
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
install(SET_DRAW_MODE, onSetDrawMode)
install(QUEUE, onQueue)
for _, f in ipairs(FRONT_ENDS) do
    local front = f
    install(front, function() onFrontEnd(front) end)
end
for i = 0, SLOT_COUNT - 1 do
    local index = i
    add(SLOTS + index * SLOT_SIZE + DRAW_MODE, 'Write', 1,
        'slot' .. index .. ' draw_mode_request', function() onStore(index) end)
end

listener_model_slot_requests = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches; '
                    .. 'start a Free Duel', SCRIPT_NAME,
                    #breakpoints_model_slot_requests, #mismatches))
