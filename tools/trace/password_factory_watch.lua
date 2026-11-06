-- password_factory_watch.lua
--
-- WHAT THIS ANSWERS: whether func_801680B4, the display-object factory in
-- the Password overlay (src/overlays/password/func_801680B4.c), runs on the
-- Password screen or the Name Entry screen, who calls it (return address),
-- with which X/Y, and what the returned object looks like. No C caller is
-- known, so it is still address-named; a caller and an on-screen object
-- would let it take a <Subsystem>_<Verb><Object> name.
--
-- North American SLUS-014.11 only. Several overlays share 0x80168000, so
-- every hit is checked against all 33 instruction words of the function as
-- built from the US password module (sha1 d5d166e4..., the
-- config/slus_01411/overlays/password.yaml target) plus the entry words of
-- the four screen functions below. A mismatch is reported, never counted.
--
-- ROUTE (about two minutes; the trace ends itself after three):
--   1. Enable the debugger and select the interpreter CPU (restart the
--      emulator after changing CPU). Reach the main menu, then paste this
--      script into the Lua editor.
--   2. Open Password. Enter three or four digits, delete them again, then
--      back out to the main menu.
--   3. If time allows, choose New Game to reach Name Entry and type two or
--      three letters. Do not reset.
-- Each factory call prints a #NN row the moment it happens. In the context
-- section, say for each #NN (or the first few) what appeared or changed on
-- screen at that moment, and which screen you were on.
-- Paste the final document into tools/trace/result/password_factory_watch.txt.

local ffi = require('ffi')
local SCRIPT_NAME = 'password_factory_watch'

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

local FACTORY = 0x801680B4
local FACTORY_RETURN = 0x80168130 -- jr ra: v0 holds the object, ra the caller
local FACTORY_JAL = 0x0C05A02D    -- jal 0x801680B4
local FACTORY_WORDS = {
    0x27bdffd0, 0xafb10024, 0x00808821, 0xafb20028,
    0x00a09021, 0xafbf002c, 0x0c01000b, 0xafb00020,
    0x00402021, 0x0c01002b, 0x24050002, 0x00408021,
    0x02002021, 0x02202821, 0x24020017, 0xafa20018,
    0x24020101, 0x02403021, 0x00003821, 0xafa00010,
    0xafa00014, 0x0c010133, 0xafa2001c, 0x96030008,
    0x02001021, 0x34630008, 0xa4430008, 0x8fbf002c,
    0x8fb20028, 0x8fb10024, 0x8fb00020, 0x03e00008,
    0x27bd0030,
}
local SCREENS = {
    {address = 0x801683EC, word = 0x27bdffc8, name = 'NameEntry_Init',
     screen = 'name-entry'},
    {address = 0x80169C08, word = 0x27bdffe8, name = 'NameEntry_PollCompletion',
     screen = 'name-entry'},
    {address = 0x8016A080, word = 0x27bdffc0, name = 'Password_InitShopScreen',
     screen = 'password'},
    {address = 0x8016A37C, word = 0x27bdffe0, name = 'Password_UpdateShopScreen',
     screen = 'password'},
}
local CONTROL, CONTROL_WORD = 0x8001306C, 0x27bdffe0 -- Main_RunFrameServices
local POOL, RECORD, CAPACITY = 0x800EFE48, 0x70, 96   -- gDisplayObject_aPool
local RAM_BASE, RAM_END = 0x80000000, 0x80200000
local FPS, WINDOW_FRAMES = 60, 180 * 60
local MAX_FACTORY_ROWS, MAX_MISMATCH_ROWS, MAX_ROWS = 64, 8, 240
local LATER_FRAMES = 30

local mem = PCSX.getMemPtr()
local rows, followups = {}, {}
local frames, controlHits, factoryHits, factoryReturns = 0, 0, 0, 0
local mismatches, mismatchRows, refused, guardFailures = 0, 0, 0, 0
local droppedRows, unloggedFactoryHits = 0, 0
local screenHits = {}
local screen, pending = 'none', nil
local done = false
breakpoints_password_factory_watch = {}

local function readable(address, size)
    return mem ~= nil and address >= RAM_BASE and address + size <= RAM_END
        and address % size == 0
end
local function word(address)
    if not readable(address, 4) then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (address - RAM_BASE))[0])
end
local function half(address)
    if not readable(address, 2) then return nil end
    return tonumber(ffi.cast('uint16_t*', mem + (address - RAM_BASE))[0])
end
local function byte(address)
    if not readable(address, 1) then return nil end
    return tonumber(mem[address - RAM_BASE])
end
local function s16(value)
    if value >= 0x8000 then return value - 0x10000 end
    return value
end
local function s32(value)
    value = tonumber(value) % 0x100000000
    if value >= 0x80000000 then return value - 0x100000000 end
    return value
end
local function u32(value) return tonumber(value) % 0x100000000 end

-- nil when the US password module is resident, else the first difference.
local function signatureProblem()
    for i, expected in ipairs(FACTORY_WORDS) do
        local address = FACTORY + (i - 1) * 4
        local actual = word(address)
        if actual ~= expected then
            return string.format('0x%08X=%s want 0x%08X', address,
                actual and string.format('0x%08X', actual) or 'unreadable',
                expected)
        end
    end
    for _, s in ipairs(SCREENS) do
        local actual = word(s.address)
        if actual ~= s.word then
            return string.format('0x%08X=%s want 0x%08X (%s)', s.address,
                actual and string.format('0x%08X', actual) or 'unreadable',
                s.word, s.name)
        end
    end
    return nil
end

local function stamp() return string.format('t=%.2fs', frames / FPS) end

local function live(text)
    if #rows < MAX_ROWS then
        rows[#rows + 1] = text
        print(text)
    else
        droppedRows = droppedRows + 1
    end
end

local function mismatch(where, problem)
    mismatches = mismatches + 1
    pending = nil
    if mismatchRows < MAX_MISMATCH_ROWS then
        mismatchRows = mismatchRows + 1
        live(string.format('%s signature mismatch at %s: %s (another overlay '
            .. 'is resident; hit not counted)', stamp(), where, problem))
    end
end

-- nil for a pool record, else why the pointer is refused.
local function objectProblem(pointer)
    if pointer == 0 then return 'null' end
    if pointer % 4 ~= 0 then return 'unaligned' end
    if pointer < RAM_BASE or pointer + RECORD > RAM_END then
        return 'outside RAM'
    end
    local offset = pointer - POOL
    if offset < 0 or offset >= CAPACITY * RECORD or offset % RECORD ~= 0 then
        return 'not a gDisplayObject_aPool record'
    end
    return nil
end

local function describe(pointer)
    local function h(o) return half(pointer + o) end
    local function b(o) return byte(pointer + o) end
    return string.format('slot=%d flags=0x%04X list=%d xy=(%d,%d) '
        .. 'size=(%d,%d) clut=(%d,%d) w5C=0x%04X w5E=0x%04X b66=0x%02X '
        .. 'res=%d/%d/%d attr=0x%08X update=0x%08X',
        (pointer - POOL) / RECORD, h(0x08), s16(h(0x1E)), s16(h(0x30)),
        s16(h(0x32)), h(0x3C), h(0x3E), h(0x40), h(0x42), h(0x5C), h(0x5E),
        b(0x66), b(0x67), b(0x68), b(0x69), word(pointer + 0x04),
        word(pointer + 0x24))
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_password_factory_watch) do
        pcall(function() bp:disable() end)
    end
    local screenTotal = 0
    for _, count in pairs(screenHits) do screenTotal = screenTotal + count end
    print('\n==== USER CONTEXT ====\n')
    print('Which screens you visited, and for each #NN row what appeared or '
        .. 'changed on screen when it printed.\n')
    print('==== TRACE RESULT =====\n')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d control_hits: %d factory_hits: %d '
        .. 'factory_returns: %d unlogged_factory_hits: %d refused_objects: %d '
        .. 'guard_failures: %d signature_mismatches: %d dropped_rows: %d',
        frames, controlHits, factoryHits, factoryReturns,
        unloggedFactoryHits, refused, guardFailures, mismatches, droppedRows))
    for _, s in ipairs(SCREENS) do
        print(string.format('%s 0x%08X hits=%d', s.name, s.address,
            screenHits[s.name] or 0))
    end
    if factoryHits > 0 then
        print('verdict: func_801680B4 ran with the US password module '
            .. 'resident; the rows give its callers, X/Y and objects')
    elseif controlHits > 0 and screenTotal > 0 then
        print('verdict: negative evidence: the control and the screen '
            .. 'functions fired with the module resident, but func_801680B4 '
            .. 'was never called on the screens visited in this window')
    elseif controlHits > 0 then
        print('verdict: the control fired but no Password or Name Entry '
            .. 'function ran with the US module resident; the route was not '
            .. 'followed (or this is not SLUS-014.11), so silence is not '
            .. 'evidence')
    else
        print('verdict: no breakpoint fired at all, not even the '
            .. 'Main_RunFrameServices control: the debugger is off or the '
            .. 'dynarec is still running (select the interpreter and restart '
            .. 'the emulator); silence is not evidence')
    end
    if mismatches > 0 then
        print('note: signature mismatches mean another module occupied '
            .. '0x80168000 at those moments; they were not counted')
    end
    print('')
    for _, row in ipairs(rows) do print(row) end
    print('\n--- end of trace, copy everything above into '
        .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onScreen(s)
    local problem = signatureProblem()
    if problem ~= nil then
        mismatch(s.name, problem)
        return
    end
    screenHits[s.name] = (screenHits[s.name] or 0) + 1
    if s.screen ~= screen then
        screen = s.screen
        live(string.format('%s SCREEN %s (first seen via %s)', stamp(),
            screen, s.name))
    end
end

local function onFactory()
    local problem = signatureProblem()
    if problem ~= nil then
        mismatch('func_801680B4 entry', problem)
        return
    end
    factoryHits = factoryHits + 1
    pending = nil
    if factoryHits > MAX_FACTORY_ROWS then
        unloggedFactoryHits = unloggedFactoryHits + 1
        return
    end
    local n = PCSX.getRegisters().GPR.n
    local ra = u32(n.ra)
    local site = word(ra - 8)
    local kind = 'unreadable call site'
    if site == FACTORY_JAL then
        kind = 'jal'
    elseif site ~= nil then
        kind = string.format('not a direct jal (word 0x%08X)', site)
    end
    pending = {id = factoryHits, ra = ra, sp = u32(n.sp)}
    live(string.format('#%02d %s CALL screen=%s x=%d y=%d ra=0x%08X '
        .. 'call_site=0x%08X %s', factoryHits, stamp(), screen, s32(n.a0),
        s32(n.a1), ra, ra - 8, kind))
end

local function onReturn()
    local call = pending
    pending = nil
    if call == nil then return end
    local problem = signatureProblem()
    if problem ~= nil then
        mismatch('func_801680B4 return', problem)
        return
    end
    local n = PCSX.getRegisters().GPR.n
    if u32(n.ra) ~= call.ra or u32(n.sp) ~= call.sp - 0x30 then
        guardFailures = guardFailures + 1
        live(string.format('#%02d %s RETURN guard failed: ra=0x%08X '
            .. 'sp=0x%08X', call.id, stamp(), u32(n.ra), u32(n.sp)))
        return
    end
    factoryReturns = factoryReturns + 1
    local object = u32(n.v0)
    local why = objectProblem(object)
    if why ~= nil then
        refused = refused + 1
        live(string.format('#%02d %s RETURN object=0x%08X refused: %s',
            call.id, stamp(), object, why))
        return
    end
    live(string.format('#%02d %s RETURN object=0x%08X %s', call.id, stamp(),
        object, describe(object)))
    followups[#followups + 1] = {id = call.id, object = object,
        due = frames + LATER_FRAMES}
end

local function onFrame()
    frames = frames + 1
    local index = 1
    while index <= #followups do
        local f = followups[index]
        if frames >= f.due then
            table.remove(followups, index)
            if signatureProblem() ~= nil then
                live(string.format('#%02d %s LATER skipped: module no longer '
                    .. 'resident', f.id, stamp()))
            elseif (half(f.object + 0x08) or 0) % 0x100 < 0x80 then
                live(string.format('#%02d %s LATER object=0x%08X released',
                    f.id, stamp(), f.object))
            else
                live(string.format('#%02d %s LATER object=0x%08X %s', f.id,
                    stamp(), f.object, describe(f.object)))
            end
        else
            index = index + 1
        end
    end
    if frames >= WINDOW_FRAMES then finish('three-minute window ended') end
end

local function add(address, label, callback)
    local bp = traceAddBreakpoint(address, 'Exec', 4,
        SCRIPT_NAME .. ': ' .. label, function()
            if done then return end
            local ok, err = pcall(callback)
            if not ok then finish('callback error: ' .. tostring(err)) end
        end)
    breakpoints_password_factory_watch[
        #breakpoints_password_factory_watch + 1] = bp
end

add(CONTROL, 'Main_RunFrameServices control', function()
    if word(CONTROL) == CONTROL_WORD then controlHits = controlHits + 1 end
end)
add(FACTORY, 'func_801680B4 entry', onFactory)
add(FACTORY_RETURN, 'func_801680B4 return', onReturn)
for _, s in ipairs(SCREENS) do
    local watched = s
    add(watched.address, watched.name, function() onScreen(watched) end)
end

listeners_password_factory_watch = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        if done then return end
        local ok, err = pcall(onFrame)
        if not ok then finish('frame error: ' .. tostring(err)) end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        finish('reset; reload the script from the main menu')
    end),
}

print(SCRIPT_NAME .. ': 7 breakpoints installed. Route: from the main menu '
    .. 'open Password, enter and delete a few digits, back out; then, if '
    .. 'time allows, start New Game and type a couple of letters at Name '
    .. 'Entry. The trace ends itself after three minutes.')
