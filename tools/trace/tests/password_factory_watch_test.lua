-- Run from the repository root:
--   luajit tools/trace/tests/password_factory_watch_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local originalPrint = print
local scriptPath = 'tools/trace/password_factory_watch.lua'

local RAM_BASE = 0x80000000
local FACTORY, FACTORY_RETURN, CONTROL = 0x801680B4, 0x80168130, 0x8001306C
local NAME_INIT, NAME_POLL = 0x801683EC, 0x80169C08
local PASS_INIT, PASS_UPDATE = 0x8016A080, 0x8016A37C
local POOL, RECORD = 0x800EFE48, 0x70
local ENTRY_SP = 0x801FF000
-- US password module (config/slus_01411/overlays/password.yaml target).
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
local SCREEN_WORDS = {
    [NAME_INIT] = 0x27bdffc8, [NAME_POLL] = 0x27bdffe8,
    [PASS_INIT] = 0x27bdffc0, [PASS_UPDATE] = 0x27bdffe0,
}
local CALLER_RA = 0x8016A200 -- arbitrary site holding jal 0x801680B4 at ra-8

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = {pc = 0, GPR = {n = {}}}
    local events, breakpoints, output = {}, {}, {}
    local function word(address, value)
        ffi.cast('uint32_t*', memory + (address - RAM_BASE))[0] = value
    end
    local function half(address, value)
        ffi.cast('uint16_t*', memory + (address - RAM_BASE))[0] = value
    end
    local function install()
        for i, value in ipairs(FACTORY_WORDS) do
            word(FACTORY + (i - 1) * 4, value)
        end
        for address, value in pairs(SCREEN_WORDS) do word(address, value) end
    end
    if options.resident ~= false then install() end
    word(CONTROL, 0x27bdffe0)
    word(CALLER_RA - 8, 0x0C05A02D)
    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        settings = {emulator = {Debug = {Debug = true}, Dynarec = false}},
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, size, label, callback)
            assert(kind == 'Exec' and size == 4)
            assert(breakpoints[address] == nil, 'duplicate breakpoint')
            local handle = {enabled = true, callback = callback}
            function handle:disable() self.enabled = false end
            breakpoints[address] = handle
            return handle
        end,
        Events = {
            createEventListener = function(name, callback)
                events[name] = callback
                return {}
            end,
        },
    }
    breakpoints_password_factory_watch = nil
    listeners_password_factory_watch = nil
    dofile(scriptPath)
    print = originalPrint

    local r = {output = output, word = word, half = half, install = install}
    function r:has(text) return self:countOf(text) > 0 end
    function r:countOf(text)
        local count = 0
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then count = count + 1 end
        end
        return count
    end
    function r:frames(count)
        print = function(text) output[#output + 1] = tostring(text) end
        for _ = 1, count do events['GPU::Vsync']() end
        print = originalPrint
    end
    function r:reset()
        print = function(text) output[#output + 1] = tostring(text) end
        events['ExecutionFlow::Reset']()
        print = originalPrint
    end
    function r:call(address, values, force)
        local n = registers.GPR.n
        for _, name in ipairs({'a0', 'a1', 'v0', 'ra', 'sp'}) do
            n[name] = values[name] or 0
        end
        local bp = assert(breakpoints[address])
        print = function(text) output[#output + 1] = tostring(text) end
        if bp.enabled or force then bp.callback() end
        print = originalPrint
    end
    -- One complete factory call returning `object`.
    function r:factory(x, y, object, ra)
        ra = ra or CALLER_RA
        self:call(FACTORY, {a0 = x, a1 = y, ra = ra, sp = ENTRY_SP})
        self:call(FACTORY_RETURN, {v0 = object, ra = ra, sp = ENTRY_SP - 0x30})
    end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do
            if bp.enabled then return false end
        end
        return true
    end
    local count = 0
    for _ in pairs(breakpoints) do count = count + 1 end
    assert(count == 7, 'seven distinct execution breakpoints')
    return r
end

local function slot(index) return POOL + index * RECORD end

local function finishedOnce(r)
    assert(r:countOf('==== TRACE RESULT') == 1)
    assert(r:countOf('script: password_factory_watch') == 1)
    assert(r:countOf('setup: debugger=on configured_cpu=interpreter') == 1)
    assert(r:countOf('--- end of trace') == 1)
    assert(r:allDisabled())
end

-- A normal hit on the Password screen logs x/y/ra and the object's fields,
-- then a later snapshot of the same slot.
do
    local r = capture()
    r:call(CONTROL, {})
    r:call(PASS_INIT, {})
    r:call(PASS_UPDATE, {})
    local object = slot(20)
    r.half(object + 0x08, 0x00C8)
    r.half(object + 0x1E, 2)
    r.half(object + 0x30, 0xFFF0) -- x = -16
    r.half(object + 0x32, 120)
    r.half(object + 0x40, 0x0100)
    r.word(object + 0x24, 0x8016A500)
    r:factory(0xFFFFFFF0, 120, object)
    assert(r:has('SCREEN password (first seen via Password_InitShopScreen)'))
    assert(r:countOf('SCREEN password') == 1, 'screen logged once per change')
    assert(r:has('#01 t=0.00s CALL screen=password x=-16 y=120 ra=0x8016A200 '
        .. 'call_site=0x8016A1F8 jal'))
    assert(r:has('#01 t=0.00s RETURN object=0x800F0708 slot=20 flags=0x00C8 '
        .. 'list=2 xy=(-16,120)'))
    assert(r:has('update=0x8016A500'))
    r:frames(29)
    assert(not r:has('LATER'))
    r:frames(1)
    assert(r:has('#01 t=0.50s LATER object=0x800F0708 slot=20'))
    r.half(object + 0x08, 0x0008)
    r:factory(1, 2, object)
    r:frames(30)
    assert(r:has('#02 t=1.00s LATER object=0x800F0708 released'))
    r:frames(180 * 60)
    finishedOnce(r)
    assert(r:has('factory_hits: 2 factory_returns: 2'))
    assert(r:has('Password_UpdateShopScreen 0x8016A37C hits=1'))
    assert(r:has('verdict: func_801680B4 ran'))
end

-- Signature mismatch: another module at 0x80168000 is refused, not counted.
do
    local r = capture({resident = false})
    r:call(CONTROL, {})
    r:call(FACTORY, {a0 = 5, a1 = 6, ra = CALLER_RA, sp = ENTRY_SP})
    r:call(FACTORY_RETURN, {v0 = slot(1), ra = CALLER_RA,
        sp = ENTRY_SP - 0x30})
    r:call(PASS_UPDATE, {})
    assert(r:has('signature mismatch at func_801680B4 entry: '
        .. '0x801680B4=0x00000000 want 0x27BDFFD0'))
    assert(not r:has('CALL'), 'a foreign hit must not log a call')
    assert(not r:has('RETURN'), 'nor a return')
    -- One changed word deep in the body is still a mismatch.
    r.install()
    r.word(FACTORY + 0x70, 0)
    r:factory(5, 6, slot(1))
    assert(r:has('0x80168124=0x00000000 want 0x8FB20028'))
    for _ = 1, 20 do r:call(PASS_UPDATE, {}) end
    assert(r:countOf('signature mismatch') == 8, 'mismatch rows are bounded')
    r:frames(180 * 60)
    finishedOnce(r)
    assert(r:has('factory_hits: 0'))
    assert(r:has('signature_mismatches: 23'))
    assert(r:has('verdict: the control fired but no Password or Name Entry'))
    assert(r:has('note: signature mismatches'))
end

-- Returned pointers that are not pool records are refused without reads.
do
    local r = capture()
    r:call(NAME_INIT, {})
    r:factory(1, 1, 0)
    r:factory(1, 1, slot(3) + 2)
    r:factory(1, 1, 0x1F800000)
    r:factory(1, 1, 0x801FFFF8)
    r:factory(1, 1, slot(3) + 0x10)
    r:factory(1, 1, slot(96))
    assert(r:has('SCREEN name-entry (first seen via NameEntry_Init)'))
    assert(r:has('#01 t=0.00s RETURN object=0x00000000 refused: null'))
    assert(r:has('#02 t=0.00s RETURN object=0x800EFF9A refused: unaligned'))
    assert(r:has('#03 t=0.00s RETURN object=0x1F800000 refused: outside RAM'))
    assert(r:has('#04 t=0.00s RETURN object=0x801FFFF8 refused: outside RAM'))
    assert(r:has('#05 t=0.00s RETURN object=0x800EFFA8 refused: not a '
        .. 'gDisplayObject_aPool record'))
    assert(r:has('#06 t=0.00s RETURN object=0x800F2848 refused: not a '
        .. 'gDisplayObject_aPool record'))
    -- A return that does not belong to the logged call is a guard failure.
    r:call(FACTORY, {a0 = 1, a1 = 1, ra = CALLER_RA, sp = ENTRY_SP})
    r:call(FACTORY_RETURN, {v0 = slot(4), ra = 0x80012345, sp = ENTRY_SP})
    assert(r:has('#07 t=0.00s RETURN guard failed'))
    -- An indirect caller is named as such.
    r:factory(1, 1, slot(4), 0x80168500)
    assert(r:has('call_site=0x801684F8 not a direct jal (word 0x00000000)'))
    r:frames(31)
    r:frames(180 * 60)
    finishedOnce(r)
    assert(r:has('refused_objects: 6 guard_failures: 1'))
end

-- Bounded output: only the first 64 calls are logged; the rest are counted.
do
    local r = capture()
    r:call(PASS_INIT, {})
    for i = 1, 70 do r:factory(i, i, slot(i % 96)) end
    assert(r:countOf(' CALL ') == 64)
    assert(r:has('#64 '))
    assert(not r:has('#65 '))
    r:frames(180 * 60)
    finishedOnce(r)
    assert(r:has('factory_hits: 70 factory_returns: 64 '
        .. 'unlogged_factory_hits: 6'))
end

-- Silent with the control and screens live: negative evidence.
do
    local r = capture()
    r:call(CONTROL, {})
    r:call(PASS_INIT, {})
    r:call(NAME_POLL, {})
    r:frames(180 * 60 - 1)
    assert(not r:has('==== TRACE RESULT'), 'the window is exactly 3 minutes')
    r:frames(1)
    finishedOnce(r)
    assert(r:has('verdict: negative evidence'))
    assert(r:has('NameEntry_PollCompletion 0x80169C08 hits=1'))
    assert(r:has('SCREEN name-entry (first seen via NameEntry_PollCompletion)'))
end

-- Nothing fired at all: the result names the setup mistake.
do
    local r = capture()
    r:frames(180 * 60)
    finishedOnce(r)
    assert(r:has('execution_breakpoint=NOT-observed'))
    assert(r:has('verdict: no breakpoint fired at all'))
    assert(r:has('the dynarec is still running'))
    -- Late callbacks, frames and a reset never print a second document.
    r:frames(100)
    r:reset()
    r:call(FACTORY, {a0 = 1, a1 = 1, ra = CALLER_RA, sp = ENTRY_SP}, true)
    finishedOnce(r)
end

-- A reset ends the trace once.
do
    local r = capture()
    r:call(CONTROL, {})
    r:reset()
    finishedOnce(r)
    assert(r:has('status: reset'))
    r:frames(180 * 60)
    finishedOnce(r)
end

PCSX = nil
print('password_factory_watch: all scenarios passed')
