-- model_handler_families.lua
--
-- WHAT THIS ANSWERS
--   tent_ModelHandlerRegistry (0x800F5918) is an eighty-entry table of
--   (handler_value, key) pairs filled by Model_RegisterHandlerKey. Two
--   address-named functions in src/game/model_handler_registry.c open-code
--   the same reverse lookup -- value to key -- and then dispatch on the key's
--   high halfword (group) and low halfword (kind) to return a handler
--   pointer:
--
--   func_8005FC1C  "the second dispatch family"; Model_RunSlotHandlers passes
--                  it as the callback for each slot's handler list.
--   func_8005FE44  the other family; Model_DrawSlot passes it to
--                  Model_RemapPrimitiveHandlers for each handler-run entry.
--
--   Neither name can be given until it is known which kinds of model data
--   reach each family and what they return for them. This trace logs, for
--   every distinct value each family is asked about, the registered key, its
--   group and kind, the handler pointer returned (read at the call's return
--   address), the slot being run or drawn, and the main mode. It also logs
--   the registrations and dumps the populated registry at the end, so every
--   value can be matched to its key.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file BEFORE loading a save or
--      starting a duel, so the registrations are seen, and wait for
--      "execution breakpoint observed; setup confirmed".
--   3. Play one duel far enough to see two different monsters' 3-D battle
--      animations, including a magic card's effect if you can. Then view one
--      monster and one non-monster card in 3-D in the Library.
--   4. The trace prints after 90 rows, or three quiet minutes after the
--      first lookup.
--   5. Copy the whole document into
--      tools/trace/result/model_handler_families.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   The models you saw, in order, with what each did (idle, attack, hit,
--   effect, rotate) and roughly when. Note any visible difference in how a
--   model was drawn -- flat, textured, shaded, transparent, glowing.

local ffi = require('ffi')

local SCRIPT_NAME = 'model_handler_families'

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
local REGISTRY = 0x800f5918
local REGISTRY_COUNT = 80
local SENTINEL = 0x80089e20 -- GsU_00000000; the lookups skip it

local FAMILIES = {
    {0x8005fc1c, 0x3c02800f, 'family func_8005FC1C'},
    {0x8005fe44, 0x3c02800f, 'family func_8005FE44'},
}
local REGISTER = {0x80060170, 0x3c02800f, 'Model_RegisterHandlerKey'}
local FIND = {0x800601d0, 0x3c02800f, 'Model_FindHandlerKey'}
local RUN_SLOT = {0x80059dd8, 0x27bdffd0, 'context Model_RunSlotHandlers'}
local DRAW_SLOT = {0x800540b4, 0x27bdf738, 'context Model_DrawSlot'}
local CONTROL = {0x8001306c, 0x27bdffe0, 'control Main_RunFrameServices'}

local MAX_ROWS = 90
local REGISTER_ROWS = 24
local FIND_ROWS = 6
-- A return breakpoint that has not fired after this many frames is dropped.
local RETURN_TIMEOUT_FRAMES = 60
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

local frames = 0
local rows = {}
local counts = {}
local seen = {}
local returns = {}
local registerRows, findRows = 0, 0
local controlHits = 0
local currentSlot = 'none'
local lastRowFrame = nil
local mismatches = {}
local callbackError = nil
local done = false

breakpoints_model_handler_families = {}

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

-- The same reverse lookup both families perform, so each row names the key
-- the code is about to dispatch on.
local function lookup(value)
    if value == SENTINEL then return nil end
    for i = 0, REGISTRY_COUNT - 1 do
        local entry = REGISTRY + i * 8
        if u32(entry) == value then return u32(entry + 4) end
    end
    return nil
end

local function keyText(key)
    if key == nil then return 'key=none(-1)' end
    return string.format('key=0x%08X group=0x%04X kind=0x%04X', key,
                         math.floor(key / 0x10000), key % 0x10000)
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_model_handler_families) do bp:disable() end
    for ra, pending in pairs(returns) do
        pending.handle:disable()
        row(pending.text .. ' returns=not-observed (capture ended)')
        returns[ra] = nil
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<the models you saw, in order, with what each did and roughly when;')
    print(' any visible difference in how a model was drawn>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d rows: %d control_hits: %d', frames, #rows,
                        controlHits))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    for _, t in ipairs({FAMILIES[1], FAMILIES[2], REGISTER, FIND}) do
        print(string.format('count %-28s %d', t[3], counts[t[1]] or 0))
    end
    print('registry:')
    for i = 0, REGISTRY_COUNT - 1 do
        local value, key = u32(REGISTRY + i * 8), u32(REGISTRY + i * 8 + 4)
        if value ~= nil and key ~= nil and (value ~= 0 or key ~= 0) then
            print(string.format('  [%02d] value=0x%08X %s', i, value,
                                keyText(key)))
        end
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onFamily(family)
    if done then return end
    counts[family[1]] = (counts[family[1]] or 0) + 1
    local n = PCSX.getRegisters().GPR.n
    local value, ra = n.a0, n.ra
    local key = string.format('%08X:%08X', family[1], value)
    if seen[key] or #rows >= MAX_ROWS then return end
    -- One return capture per call site at a time; a later call retries.
    if returns[ra] ~= nil or ra % 4 ~= 0 or not inRam(ra, 4) then return end
    seen[key] = true
    local text = string.format('%s value=0x%08X %s caller=0x%08X slot=%s main=%s',
                               family[3], value, keyText(lookup(value)), ra,
                               currentSlot, mode())
    local pending = { text = text, frame = frames }
    pending.handle = traceAddBreakpoint(ra, 'Exec', 4,
        SCRIPT_NAME .. ': return of ' .. family[3], function()
            if returns[ra] ~= pending then return false end
            returns[ra] = nil
            local ok, err = pcall(function()
                row(string.format('%s returns=0x%08X', text,
                                  PCSX.getRegisters().GPR.n.v0))
            end)
            if not ok then callbackError = tostring(err) end
            return false
        end)
    returns[ra] = pending
end

local function onRegister()
    if done then return end
    counts[REGISTER[1]] = (counts[REGISTER[1]] or 0) + 1
    if registerRows >= REGISTER_ROWS then return end
    local n = PCSX.getRegisters().GPR.n
    if row(string.format('%s %s value=0x%08X caller=0x%08X main=%s',
                         REGISTER[3], keyText(n.a0), n.a1, n.ra, mode())) then
        registerRows = registerRows + 1
    end
end

local function onFind()
    if done then return end
    counts[FIND[1]] = (counts[FIND[1]] or 0) + 1
    if findRows >= FIND_ROWS then return end
    local n = PCSX.getRegisters().GPR.n
    if row(string.format('%s value=0x%08X %s caller=0x%08X slot=%s main=%s',
                         FIND[3], n.a0, keyText(lookup(n.a0)), n.ra,
                         currentSlot, mode())) then
        findRows = findRows + 1
    end
end

local function onContext(label)
    return function()
        if done then return end
        currentSlot = string.format('%s(%d)', label,
                                    PCSX.getRegisters().GPR.n.a0)
    end
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    for ra, pending in pairs(returns) do
        if frames - pending.frame > RETURN_TIMEOUT_FRAMES then
            pending.handle:disable()
            returns[ra] = nil
            row(pending.text .. ' returns=not-observed')
        end
    end
    if #rows >= MAX_ROWS then
        finish('captured ' .. MAX_ROWS .. ' rows')
    elseif lastRowFrame ~= nil and frames - lastRowFrame >= QUIET_FRAMES then
        finish('three quiet minutes without a new row')
    elseif controlHits == 0 and frames >= NO_HIT_WARNING_FRAMES then
        finish('five minutes without a control hit; the setup is not '
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
    breakpoints_model_handler_families[#breakpoints_model_handler_families + 1] =
        traceAddBreakpoint(target[1], 'Exec', 4,
            SCRIPT_NAME .. ': ' .. target[3], function()
                local ok, err = pcall(handler)
                if not ok then callbackError = tostring(err) end
            end)
end

for _, family in ipairs(FAMILIES) do
    local f = family
    install(f, function() onFamily(f) end)
end
install(REGISTER, onRegister)
install(FIND, onFind)
install(RUN_SLOT, onContext('run'))
install(DRAW_SLOT, onContext('draw'))
install(CONTROL, function() controlHits = controlHits + 1 end)

listener_model_handler_families = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches',
                    SCRIPT_NAME, #breakpoints_model_handler_families,
                    #mismatches))
