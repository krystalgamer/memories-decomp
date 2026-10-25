-- duel_effect_jar_null_offset.lua
--
-- QUESTION: When Dragon Capture Jar (effect 17, phase 1) has a target, do
-- its two null-offset calls to the shared quad helper execute, and which
-- halfwords does the CPU load for the offset?
--
-- NA only. Follow-up to #7930: with no opponent monster the jar made no
-- helper calls at all, so the phase-one null path is still unobserved.
-- With Dragon Capture Jar in hand and at least one Dragon-type monster on
-- the OPPONENT's side, enable the debugger and select the interpreter
-- (restart before loading the duel if needed). Paste this in the Lua editor
-- BEFORE playing the jar, then activate it once in the 2-D duel. Do not
-- reset during the run. The live #NN marker identifies the effect episode.
--
-- Stops three seconds after the first targeted null call returns, after
-- two minutes, on reset, or if the observed duel bank goes away.
-- Paste the final document into
-- tools/trace/result/duel_effect_jar_null_offset.txt.
-- Context: one sentence per #NN saying which monsters were on the field,
-- what happened to the dragon, and how the effect looked.
--
-- This observes all 24 offset LHU results in the first call from each known
-- null caller. Breakpoints are after the load-delay instruction, before the
-- addition consumes v1. No Lua read dereferences the guest null pointer.
-- The Dark Hole result (phase 0, #7930) is not presumed for phase 1.

local ffi = require('ffi')
local SCRIPT_NAME = 'duel_effect_jar_null_offset'

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

local HELPER = 0x80147e08
local HANDLER = 0x80158e84
local CONTROL = 0x8002c6c8
local PARTICLE_RETURN = 0x8015950c
local FPS, MAX_ROWS = 60, 90
local mem = PCSX.getMemPtr()
local rows, sites = {}, {
    {ra = 0x80159f34, name = 'burst', calls = 0, samples = {}, count = 0},
    {ra = 0x8015a130, name = 'path', calls = 0, samples = {}, count = 0},
}
local loads = {
    {0xb8, 1, 0}, {0xcc, 1, 1}, {0xe0, 1, 2},
    {0x158, 2, 0}, {0x16c, 2, 1}, {0x180, 2, 2},
}
local frames, event, controlHits, handlerHits = 0, 0, 0, 0
local phaseOne, helperHits, particleCalls, foreignHits = 0, 0, 0, 0
local guardFailures, dropped, unknownNulls = 0, 0, 0
local current, firstReturnedFrame
local sawHandler, done = false, false
breakpoints_duel_effect_jar_null_offset = {}

local function word(address)
    if mem == nil or address < 0x80000000 or address + 4 > 0x80200000
        or address % 4 ~= 0 then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (address - 0x80000000))[0])
end

local function resident()
    return word(0x801462b0) == 0x27bdffc8
        and word(HELPER) == 0x27bdff88
        and word(HANDLER) == 0x27bdfef8
        and word(0x80159f30) == 0x00003821
        and word(0x8015a12c) == 0x00003821
end

local function signed16(value)
    if value >= 0x8000 then return value - 0x10000 end
    return value
end

local function live(text)
    if #rows < MAX_ROWS then
        rows[#rows + 1] = text
        print(text)
    else
        dropped = dropped + 1
    end
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_duel_effect_jar_null_offset) do
        pcall(function() bp:disable() end)
    end
    print('\n==== USER CONTEXT ====\n')
    print('One sentence per #NN: monsters on the field, what happened to '
        .. 'the dragon, and how the effect looked.\n')
    print('==== TRACE RESULT =====\n')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d control_hits: %d effect17_hits: %d '
        .. 'phase1_initializations: %d helper_hits: %d particle_calls: %d '
        .. 'foreign_hits: %d guard_failures: %d unknown_null_callers: %d '
        .. 'dropped_rows: %d', frames, controlHits, handlerHits, phaseOne,
        helperHits, particleCalls, foreignHits, guardFailures, unknownNulls,
        dropped))
    local complete = 0
    for _, site in ipairs(sites) do
        print(string.format('%s return=0x%08X null_calls=%d loads=%d/24 '
            .. 'returned=%s', site.name, site.ra, site.calls, site.count,
            tostring(site.returned == true)))
        if site.returned and site.count == 24 then complete = complete + 1 end
    end
    if complete > 0 then
        print('verdict: a null caller executed both passes of offset loads; '
            .. 'rows report actual post-LHU register values, not a Lua '
            .. 'dereference or an optional-null contract')
    elseif sites[1].calls + sites[2].calls > 0 then
        print('verdict: null entry observed but complete load evidence is '
            .. 'missing; do not infer zero/safe coordinates')
    elseif phaseOne > 0 and particleCalls > 0 and controlHits > 0 then
        print('verdict: phase-one and non-null particle controls worked, '
            .. 'but neither target null caller was observed in this window')
    else
        print('verdict: requested route/control evidence is incomplete; '
            .. 'silence is not a negative result')
    end
    print('Unobserved calls/phases remain unresolved. No global '
        .. 'reachability, host-C safety or artwork claim follows.')
    print('')
    for _, row in ipairs(rows) do print(row) end
    print('\n--- end of trace, copy everything above into '
        .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function guardModule()
    if resident() then return true end
    foreignHits = foreignHits + 1
    current = nil
    return false
end

local function onHandler()
    if not guardModule() then return end
    handlerHits = handlerHits + 1
    sawHandler = true
    local phase = tonumber(PCSX.getRegisters().GPR.n.a1)
    if phase >= 0x80000000 then phase = phase - 0x100000000 end
    if phase >= 0 then
        event = event + 1
        if phase == 1 then phaseOne = phaseOne + 1 end
        live(string.format('#%02d t=%.2fs START effect=17 phase=%d',
            event, frames / FPS, phase))
    end
end

local function onHelper()
    current = nil
    if not guardModule() then return end
    helperHits = helperHits + 1
    local n = PCSX.getRegisters().GPR.n
    local ra, pointer = tonumber(n.ra), tonumber(n.a3)
    if ra == PARTICLE_RETURN and pointer ~= 0 then
        particleCalls = particleCalls + 1
    end
    if pointer ~= 0 then return end
    for _, site in ipairs(sites) do
        if ra == site.ra then
            site.calls = site.calls + 1
            if site.started then return end
            site.started = true
            current = {site = site, sp = tonumber(n.sp) - 0x78, event = event}
            live(string.format('#%02d t=%.2fs NULL %s ra=0x%08X '
                .. 'size=%d depth=%d offset=0', event, frames / FPS,
                site.name, ra, tonumber(n.a1) % 0x10000,
                signed16(tonumber(n.a2) % 0x10000)))
            return
        end
    end
    unknownNulls = unknownNulls + 1
end

local function onLoad(load)
    local context = current
    if context == nil or not guardModule() then return end
    local address, pass, axis = HELPER + load[1], load[2], load[3]
    local n = PCSX.getRegisters().GPR.n
    local vertex = tonumber(n.a2) - (axis == 0 and 0 or 1)
    local value = tonumber(n.v1)
    if word(address - 8) ~= 0x96430000 + axis * 2
        or word(address) ~= 0x00431021
        or tonumber(n.sp) ~= context.sp or tonumber(n.s2) ~= 0
        or vertex < 0 or vertex > 3 or vertex % 1 ~= 0
        or value < 0 or value > 0xffff then
        guardFailures = guardFailures + 1
        return
    end
    local key = pass .. ':' .. vertex .. ':' .. axis
    local site = context.site
    if site.samples[key] ~= nil then return end
    site.samples[key] = value
    site.count = site.count + 1
    live(string.format('#%02d t=%.2fs %s pass=%d vertex=%d axis=%s '
        .. 'effective=0x%08X loaded_u16=0x%04X as_s16=%d',
        context.event, frames / FPS, site.name, pass, vertex,
        ({'X', 'Y', 'Z'})[axis + 1], axis * 2, value, signed16(value)))
end

local function onReturn()
    if current == nil or not guardModule() then return end
    if word(HELPER + 0x1c8) ~= 0x03e00008 then
        guardFailures = guardFailures + 1
        current = nil
        return
    end
    local site = current.site
    site.returned = true
    firstReturnedFrame = firstReturnedFrame or frames
    live(string.format('#%02d t=%.2fs RETURN %s loads=%d/24',
        current.event, frames / FPS, site.name, site.count))
    current = nil
end

local function add(address, label, callback)
    local bp = traceAddBreakpoint(address, 'Exec', 4,
        SCRIPT_NAME .. ': ' .. label, function()
            if done then return end
            local ok, err = pcall(callback)
            if not ok then finish('callback error: ' .. tostring(err)) end
        end)
    breakpoints_duel_effect_jar_null_offset[
        #breakpoints_duel_effect_jar_null_offset + 1] = bp
end

add(CONTROL, 'resident update control', function()
    if word(CONTROL) == 0x27bdffd8 then controlHits = controlHits + 1 end
end)
add(HANDLER, 'effect 17 entry', onHandler)
add(HELPER, 'layered tile entry', onHelper)
for _, load in ipairs(loads) do
    local watched = load
    add(HELPER + watched[1], 'post-LHU', function() onLoad(watched) end)
end
add(HELPER + 0x1c8, 'layered tile return', onReturn)

listeners_duel_effect_jar_null_offset = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        if done then return end
        local ok, err = pcall(function()
            frames = frames + 1
            if firstReturnedFrame ~= nil
                and frames - firstReturnedFrame >= 3 * FPS then
                finish('three seconds after the first target null return')
            elseif sawHandler and not resident() then
                finish('observed duel bank is no longer resident')
            elseif frames >= 120 * FPS then
                finish('two-minute window ended')
            end
        end)
        if not ok then finish('frame error: ' .. tostring(err)) end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        finish('reset; reload the script after returning to the duel')
    end),
}
print(SCRIPT_NAME .. ': 10 breakpoints installed; with Dragon Capture Jar '
    .. 'in hand and an opponent Dragon on the field, activate it once in '
    .. 'the 2-D duel')
