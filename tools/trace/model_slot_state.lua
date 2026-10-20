-- model_slot_state.lua
--
-- WHAT THIS ANSWERS
--   The 3-D model slots behind duel battle animations and the Library's 3-D
--   view still carry a cluster of address-named helpers and state bytes:
--
--   func_80050F24  for slot 0 or 1: plays animation field_DFE + 3, requests a
--                  camera move to the other slot for 2/3 of the animation
--                  (at least 60, overridable from the 0xCF8 block), then sets
--                  the first gate byte from flag bits 0x80/0x40. Attack, a
--                  card-play reaction, or a turn hand-off?
--   func_8005A618  called beside it by Model_UpdateAnimatedBattle; returns a
--                  wrapped angle from func_8005F1A4 plus a quarter or three
--                  quarter turn depending on slot. Which angle is it?
--   func_8005F174/F180/F18C/F198  get/set D_8009B07B and D_8009B07C. The
--                  camera requests do nothing while both read 1. "Camera
--                  locked" or "model data still transferring"?
--   func_80058DD8  returns 2 if slot.field_E14 != 0xFF, else 1 if field_E1F
--                  is set, else 0. Loading / ready / empty?
--   func_80059284  sets slot.field_E11; the controllers pass (2, 3) and
--                  (0, 3). What is mode 3?
--   func_80058DC0  returns D_8009AFA6. Who writes and reads it?
--   func_8005A3D0  Model_InitSlot's coordinate-hierarchy search: the index of
--                  a unit whose super is the given parent and that has a
--                  backlink. "First child" or "next chain link"?
--
--   Every call is logged with its caller and the slot fields it touches;
--   calls made from inside func_80050F24 are tagged so the animation, camera
--   duration and gate values it chose read as one group. The two returning
--   helpers are also broken at their jr ra to read the result, the hierarchy
--   search reports parent and result as unit indices with their supers, and
--   writes to D_8009AFA6 are caught with a write breakpoint.
--   Main_RunFrameServices and Model_UpdateAnimatedBattle are controls.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file, and wait for "execution
--      breakpoint observed; setup confirmed".
--   3. In a duel, let several battles play with their 3-D animation: attack
--      with your monster, let the opponent attack you, and if possible a
--      direct attack and a fusion. Then open the Library and view one
--      monster in 3-D.
--   4. The trace prints after 110 rows, or three quiet minutes after the
--      first row.
--   5. Copy the whole document into
--      tools/trace/result/model_slot_state.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Each battle in order: attacker, defender and outcome, and what the
--   camera did (cut, pan, zoom, follow, stay still). Mark the moment each
--   monster model appeared and became fully drawn. In the Library, how long
--   the model took to appear.

local ffi = require('ffi')

local SCRIPT_NAME = 'model_slot_state'

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
local SLOTS = 0x800f2c40
local SLOT_SIZE = 0xe20
local SLOT_COUNT = 3
local GATE_FIRST = 0x8009b07b
local GATE_SECOND = 0x8009b07c
local STATE_AFA6 = 0x8009afa6
local UNIT_SIZE = 0x50
local UNIT_SUPER = 0x4c

local BATTLE_CHOICE = 0x80050f24
local BATTLE_CHOICE_END = 0x8005106c

local T = {
    choice = {0x80050f24, 0x27bdffd8, 'func_80050F24'},
    anim = {0x80057af4, 0x27bdffd0, 'Model_ControlSlotAnimation'},
    angle = {0x8005a618, 0x27bdffe8, 'func_8005A618'},
    angleRet = {0x8005a6a0, 0x03e00008, 'func_8005A618 return'},
    setFirst = {0x8005f180, 0xa3840173, 'set-first func_8005F180'},
    setSecond = {0x8005f198, 0xa3840174, 'set-second func_8005F198'},
    toSlots = {0x80059f18, 0x27bdffd8, 'Model_RequestCameraMoveToSlots'},
    request = {0x80059ebc, 0x27bdffe0, 'Model_RequestCameraMove'},
    start = {0x80052d2c, 0x27bdffc0, 'Model_StartCameraMove'},
    status = {0x80058dd8, 0x000410c0, 'status func_80058DD8'},
    mode = {0x80059284, 0x3c03800f, 'set-E11 func_80059284'},
    getter = {0x80058dc0, 0x9382009e, 'get-AFA6 func_80058DC0'},
    search = {0x8005a3d0, 0x90830e17, 'search func_8005A3D0'},
    searchRet = {0x8005a460, 0x03e00008, 'func_8005A3D0 return'},
    frame = {0x8001306c, 0x27bdffe0, 'control Main_RunFrameServices'},
    battle = {0x8004eb00, 0x27bdffc0, 'control Model_UpdateAnimatedBattle'},
}

local ROWS_PER_KEY = 3
local MAX_ROWS = 110
local QUIET_FRAMES = 10800
local NO_HIT_WARNING_FRAMES = 18000
local TIMEOUT_FRAMES = 216000

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function u32(addr)
    if not inRam(addr, 4) then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local function show(v, fmt)
    if v == nil then return 'unreadable' end
    return string.format(fmt or '%d', v)
end
local function signed(v)
    if v >= 0x80000000 then return v - 0x100000000 end
    return v
end

local frames = 0
local rows = {}
local counts = {}
local logged = {}
local frameHits, battleHits = 0, 0
local suppressed, passed = 0, 0
local lastE11 = {}
local lastAFA6 = nil
local pendingAngle, pendingSearch = nil, nil
local lastRowFrame = nil
local mismatches = {}
local callbackError = nil
local done = false

breakpoints_model_slot_state = {}

local function slotAddress(index)
    if index < 0 or index >= SLOT_COUNT then return nil end
    return SLOTS + index * SLOT_SIZE
end

local function slotFields(index)
    local slot = slotAddress(index)
    if slot == nil then return string.format('slot=%d (out of range)', index) end
    return string.format('slot=%d DFE=%s E0E=%s E11=%s E14=%s E1F=%s', index,
                         show(u8(slot + 0xdfe)), show(u8(slot + 0xe0e)),
                         show(u8(slot + 0xe11)), show(u8(slot + 0xe14), '0x%02X'),
                         show(u8(slot + 0xe1f)))
end

local function gates()
    local mode = u8(MAIN_MODE)
    return string.format('gates=%s,%s main=%s', show(u8(GATE_FIRST)),
                         show(u8(GATE_SECOND)),
                         mode and string.format('0x%02X', mode % 32)
                         or 'unreadable')
end

local function fromChoice(ra)
    return ra >= BATTLE_CHOICE and ra < BATTLE_CHOICE_END
end

-- Counts every hit under `key`; logs only the first ROWS_PER_KEY of them.
local function row(key, text)
    counts[key] = (counts[key] or 0) + 1
    if #rows >= MAX_ROWS or (logged[key] or 0) >= ROWS_PER_KEY then
        return false
    end
    logged[key] = (logged[key] or 0) + 1
    rows[#rows + 1] = string.format('row=%03d frame=%06d ', #rows + 1, frames)
        .. text
    lastRowFrame = frames
    return true
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_model_slot_state) do bp:disable() end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<each battle in order: attacker, defender, outcome, and what the')
    print(' camera did; when each monster model appeared and was fully drawn;')
    print(' in the Library, how long the 3-D model took to appear>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d rows: %d frame_services: %d battle_updates: '
                        .. '%d camera_requests_passed: %d suppressed: %d',
                        frames, #rows, frameHits, battleHits, passed,
                        suppressed))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    local keys = {}
    for key in pairs(counts) do keys[#keys + 1] = key end
    table.sort(keys)
    for _, key in ipairs(keys) do
        print(string.format('count %s %d', key, counts[key]))
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local handlers = {}

handlers.choice = function(n)
    row('choice slot=' .. signed(n.a0),
        string.format('=== func_80050F24 %s caller=0x%08X %s', slotFields(signed(n.a0)),
                      n.ra, gates()))
end
handlers.anim = function(n)
    if not fromChoice(n.ra) then return end
    row('anim from func_80050F24', string.format(
        '  from func_80050F24: Model_ControlSlotAnimation slot=%d animation=%d '
            .. 'arg=%d', signed(n.a0), signed(n.a1), signed(n.a2)))
end
handlers.angle = function(n)
    pendingAngle = { slot = signed(n.a0), ra = n.ra, fields = slotFields(signed(n.a0)) }
end
handlers.angleRet = function(n)
    if pendingAngle == nil then return end
    local p = pendingAngle
    pendingAngle = nil
    row(string.format('angle caller=0x%08X slot=%d', p.ra, p.slot), string.format(
        'func_8005A618 caller=0x%08X %s returns=%d (0x%03X of 0x1000)', p.ra,
        p.fields, signed(n.v0), n.v0 % 0x1000))
end
local function setter(label)
    return function(n)
        local tag = fromChoice(n.ra) and '  from func_80050F24: ' or ''
        row(string.format('%s value=%d caller=0x%08X', label, signed(n.a0), n.ra),
            string.format('%s%s value=%d caller=0x%08X before: %s', tag, label,
                          signed(n.a0), n.ra, gates()))
    end
end
handlers.setFirst = setter('set-first func_8005F180')
handlers.setSecond = setter('set-second func_8005F198')
local function cameraRequest(label, gated)
    return function(n)
        local verdict = ''
        if gated then
            local blocked = u8(GATE_FIRST) == 1 and u8(GATE_SECOND) == 1
            if blocked then suppressed = suppressed + 1 else passed = passed + 1 end
            verdict = blocked and ' SUPPRESSED' or ' passed'
        end
        local tag = fromChoice(n.ra) and '  from func_80050F24: ' or ''
        row(string.format('%s caller=0x%08X%s', label, n.ra, verdict),
            string.format('%s%s args=%d,%d,%d,%d caller=0x%08X %s%s', tag, label,
                          signed(n.a0), signed(n.a1), signed(n.a2), signed(n.a3),
                          n.ra, gates(), verdict))
    end
end
handlers.toSlots = cameraRequest('Model_RequestCameraMoveToSlots', true)
handlers.request = cameraRequest('Model_RequestCameraMove', true)
handlers.start = cameraRequest('Model_StartCameraMove', false)
handlers.status = function(n)
    local index = signed(n.a0)
    local slot = slotAddress(index)
    local result = 'out-of-range'
    if slot ~= nil then
        local e14, e1f = u8(slot + 0xe14), u8(slot + 0xe1f)
        if e14 ~= nil and e1f ~= nil then
            result = tostring(e14 ~= 0xff and 2 or (e1f ~= 0 and 1 or 0))
        end
    end
    -- Logged per caller, slot and result so each transition shows once.
    row(string.format('status caller=0x%08X slot=%d returns=%s', n.ra, index, result),
        string.format('func_80058DD8 caller=0x%08X %s returns=%s %s', n.ra,
                      slotFields(index), result, gates()))
end
handlers.mode = function(n)
    row(string.format('set-E11 caller=0x%08X slot=%d value=%d', n.ra,
                      signed(n.a0), signed(n.a1)),
        string.format('func_80059284 caller=0x%08X value=%d before: %s %s', n.ra,
                      signed(n.a1), slotFields(signed(n.a0)), gates()))
end
handlers.getter = function(n)
    row(string.format('get-AFA6 caller=0x%08X value=%s', n.ra,
                      show(u8(STATE_AFA6))),
        string.format('func_80058DC0 caller=0x%08X returns=%s %s', n.ra,
                      show(u8(STATE_AFA6)), gates()))
end
local function unitIndex(entries, unit)
    if entries == nil or unit == nil or unit < entries
        or (unit - entries) % UNIT_SIZE ~= 0 then
        return 'not-a-unit'
    end
    return tostring((unit - entries) / UNIT_SIZE)
end
handlers.search = function(n)
    pendingSearch = { model = n.a0, parent = n.a1, ra = n.ra }
end
handlers.searchRet = function(n)
    if pendingSearch == nil then return end
    local p = pendingSearch
    pendingSearch = nil
    local text
    if p.model % 4 ~= 0 or not inRam(p.model, SLOT_SIZE) then
        text = string.format('IMPLAUSIBLE model=0x%08X', p.model)
    else
        local entries = u32(p.model + 0xd14)
        local count = u8(p.model + 0xe17)
        local result = signed(n.v0)
        local resultSuper = nil
        if entries ~= nil and count ~= nil and result >= 0 and result < count
            and inRam(entries + result * UNIT_SIZE, UNIT_SIZE) then
            resultSuper = u32(entries + result * UNIT_SIZE + UNIT_SUPER)
        end
        local parentSuper = inRam(p.parent, UNIT_SIZE)
            and u32(p.parent + UNIT_SUPER) or nil
        text = string.format(
            'model=0x%08X count=%s parent=%s (super %s) returns=%d (super %s)',
            p.model, show(count), unitIndex(entries, p.parent),
            unitIndex(entries, parentSuper), result,
            unitIndex(entries, resultSuper))
    end
    row(string.format('search caller=0x%08X', p.ra),
        string.format('func_8005A3D0 caller=0x%08X %s', p.ra, text))
end
handlers.frame = function() frameHits = frameHits + 1 end
handlers.battle = function() battleHits = battleHits + 1 end

local function onWriteAFA6()
    if done then return end
    local pc = PCSX.getRegisters().pc
    row(string.format('write-AFA6 pc=0x%08X', pc),
        string.format('write D_8009AFA6 pc=0x%08X before=%s %s', pc,
                      show(u8(STATE_AFA6)), gates()))
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    for index = 0, SLOT_COUNT - 1 do
        local value = u8(slotAddress(index) + 0xe11)
        if lastE11[index] ~= nil and value ~= lastE11[index] then
            row(string.format('poll E11 slot=%d %s->%s', index,
                              show(lastE11[index]), show(value)),
                string.format('poll slot=%d E11 %s->%s %s', index,
                              show(lastE11[index]), show(value), gates()))
        end
        lastE11[index] = value
    end
    local afa6 = u8(STATE_AFA6)
    if lastAFA6 ~= nil and afa6 ~= lastAFA6 then
        row(string.format('poll AFA6 %s->%s', show(lastAFA6), show(afa6)),
            string.format('poll D_8009AFA6 %s->%s %s', show(lastAFA6), show(afa6),
                          gates()))
    end
    lastAFA6 = afa6

    if #rows >= MAX_ROWS then
        finish('captured ' .. MAX_ROWS .. ' rows')
    elseif lastRowFrame ~= nil and frames - lastRowFrame >= QUIET_FRAMES then
        finish('three quiet minutes without a new row')
    elseif frameHits == 0 and frames >= NO_HIT_WARNING_FRAMES then
        finish('five minutes without a control hit; the setup is not '
               .. 'working, check the setup line')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

local function install(target, kind, width, handler)
    if kind == 'Exec' then
        local word = u32(target[1])
        if word ~= target[2] then
            mismatches[#mismatches + 1] = string.format(
                '0x%08X expected 0x%08X read %s', target[1], target[2],
                show(word, '0x%08X'))
        end
    end
    breakpoints_model_slot_state[#breakpoints_model_slot_state + 1] =
        traceAddBreakpoint(target[1], kind, width,
            SCRIPT_NAME .. ': ' .. target[3], function()
                if done then return end
                local ok, err = pcall(handler, PCSX.getRegisters().GPR.n)
                if not ok then callbackError = tostring(err) end
            end)
end

local ORDER = {'choice', 'anim', 'angle', 'angleRet', 'setFirst', 'setSecond',
               'toSlots', 'request', 'start', 'status', 'mode', 'getter',
               'search', 'searchRet', 'frame', 'battle'}
for _, name in ipairs(ORDER) do
    install(T[name], 'Exec', 4, handlers[name])
end
install({STATE_AFA6, 0, 'write D_8009AFA6'}, 'Write', 1, onWriteAFA6)

listener_model_slot_state = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches',
                    SCRIPT_NAME, #breakpoints_model_slot_state, #mismatches))
