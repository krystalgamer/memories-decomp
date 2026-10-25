-- Run from the repository root:
--   luajit tools/trace/tests/duel_effect_jar_null_offset_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local originalPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/duel_effect_jar_null_offset.lua'

local RAM_BASE, ENTRY_SP = 0x80000000, 0x801ff000
local HELPER, HANDLER, CONTROL = 0x80147e08, 0x80158e84, 0x8002c6c8
local BURST, PATH, PARTICLE = 0x80159f34, 0x8015a130, 0x8015950c
local MODULE = {
    [0x801462b0] = 0x27bdffc8, [HELPER] = 0x27bdff88,
    [HANDLER] = 0x27bdfef8, [BURST - 4] = 0x00003821,
    [PATH - 4] = 0x00003821,
}
local LOADS = {
    {0xb8, 1, 0}, {0xcc, 1, 1}, {0xe0, 1, 2},
    {0x158, 2, 0}, {0x16c, 2, 1}, {0x180, 2, 2},
}

local function capture()
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = {pc = 0, GPR = {n = {}}}
    local events, breakpoints, output = {}, {}, {}
    local function word(address, value)
        ffi.cast('uint32_t*', memory + (address - RAM_BASE))[0] = value
    end
    for address, value in pairs(MODULE) do word(address, value) end
    word(CONTROL, 0x27bdffd8)
    word(HELPER + 0x1c8, 0x03e00008)
    for _, load in ipairs(LOADS) do
        word(HELPER + load[1] - 8, 0x96430000 + load[3] * 2)
        word(HELPER + load[1], 0x00431021)
    end
    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        settings = {emulator = {Debug = {Debug = true}, Dynarec = false}},
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, size, label, callback)
            assert(kind == 'Exec' and size == 4)
            assert(breakpoints[address] == nil)
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
    breakpoints_duel_effect_jar_null_offset = nil
    listeners_duel_effect_jar_null_offset = nil
    dofile(scriptPath)

    local r = {output = output, word = function(_, ...) word(...) end}
    function r:has(text) return self:countOf(text) > 0 end
    function r:countOf(text)
        local count = 0
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then count = count + 1 end
        end
        return count
    end
    function r:frames(count)
        for _ = 1, count do events['GPU::Vsync']() end
    end
    function r:reset() events['ExecutionFlow::Reset']() end
    function r:call(address, values, force)
        local n = registers.GPR.n
        for _, name in ipairs({'a0', 'a1', 'a2', 'a3', 'ra', 'sp', 's2', 'v1'}) do
            n[name] = values[name] or 0
        end
        local bp = assert(breakpoints[address])
        if bp.enabled or force then bp.callback() end
    end
    function r:enter(ra, pointer)
        self:call(HELPER, {
            ra = ra, a3 = pointer or 0, sp = ENTRY_SP, a1 = 32, a2 = 1,
        })
    end
    function r:sample(pass, vertex, axis, overrides)
        local value = ({0, 0x7fff, 0x8000, 0xffff})[
            ((pass - 1) * 12 + vertex * 3 + axis) % 4 + 1]
        local values = {
            sp = ENTRY_SP - 0x78, s2 = 0,
            a2 = vertex + (axis == 0 and 0 or 1), v1 = value,
        }
        for name, replacement in pairs(overrides or {}) do
            values[name] = replacement
        end
        self:call(HELPER + LOADS[(pass - 1) * 3 + axis + 1][1], values)
    end
    function r:samples(overrides, limit)
        local count = 0
        for pass = 1, 2 do
            for vertex = 0, 3 do
                for axis = 0, 2 do
                    if count == (limit or 24) then return end
                    self:sample(pass, vertex, axis, overrides)
                    count = count + 1
                end
            end
        end
    end
    function r:returnCall() self:call(HELPER + 0x1c8, {}) end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do
            if bp.enabled then return false end
        end
        return true
    end
    local count = 0
    for _ in pairs(breakpoints) do count = count + 1 end
    assert(count == 10, 'ten distinct execution breakpoints')
    return r
end

local function stopped(r, status)
    assert(r:has('status: ' .. status))
    assert(r:countOf('script: duel_effect_jar_null_offset') == 1)
    assert(r:allDisabled())
    r:frames(7201)
    r:reset()
    r:call(CONTROL, {}, true)
    r:call(HELPER, {}, true)
    assert(r:countOf('script: duel_effect_jar_null_offset') == 1)
    assert(r:countOf('==== TRACE RESULT') == 1)
    assert(r:countOf('--- end of trace') == 1,
        'termination emits one final document, even after late callbacks')
end

local function incomplete(r)
    assert(not r:has('verdict: a null caller executed both passes'))
end

-- Both known callers qualify independently for the phase-one route.
for _, site in ipairs({{BURST, 'burst'}, {PATH, 'path'}}) do
    local r = capture()
    r:call(CONTROL, {})
    r:call(HANDLER, {a1 = 1})
    r:call(HANDLER, {a1 = 0xffffffff})
    assert(r:countOf('START effect=17 phase=1') == 1)
    assert(r:has('#01 '))
    r:frames(37)
    r:enter(site[1])
    r:samples()
    for pass = 1, 2 do
        for vertex = 0, 3 do
            for axis = 0, 2 do
                local key = string.format('%s pass=%d vertex=%d axis=%s ',
                    site[2], pass, vertex, ({'X', 'Y', 'Z'})[axis + 1])
                assert(r:countOf(key) == 1, 'one row per distinct sample')
            end
        end
    end
    assert(r:countOf('effective=') == 24)
    r:samples()
    assert(r:countOf('effective=') == 24, 'duplicate callbacks do not add samples')
    assert(r:has('loaded_u16=0x0000 as_s16=0'))
    assert(r:has('loaded_u16=0x7FFF as_s16=32767'))
    assert(r:has('loaded_u16=0x8000 as_s16=-32768'))
    assert(r:has('loaded_u16=0xFFFF as_s16=-1'))
    r:returnCall()
    r:frames(179)
    assert(not r:has('script: duel_effect_jar_null_offset'))
    r:frames(1)
    assert(r:has('frames: 217 '))
    assert(r:has(site[2] .. string.format(' return=0x%08X ', site[1])
        .. 'null_calls=1 loads=24/24 returned=true'))
    assert(r:has('verdict: a null caller executed both passes'))
    stopped(r, 'three seconds after the first target null return')
end

local interrupted = capture()
interrupted:enter(BURST)
interrupted:samples(nil, 12)
interrupted:enter(PARTICLE, 0x80100000)
interrupted:enter(BURST)
interrupted:samples()
interrupted:returnCall()
interrupted:reset()
assert(interrupted:has('null_calls=2 loads=12/24 returned=false'))
assert(interrupted:has('particle_calls: 1'))
incomplete(interrupted)
stopped(interrupted, 'reset; reload the script after returning to the duel')

local partial = capture()
partial:enter(BURST)
partial:samples(nil, 23)
partial:returnCall()
partial:enter(BURST)
partial:samples()
partial:returnCall()
partial:frames(180)
assert(partial:has('null_calls=2 loads=23/24 returned=true'))
incomplete(partial)
stopped(partial, 'three seconds after the first target null return')

local separate = capture()
for index, ra in ipairs({BURST, PATH}) do
    if index == 2 then separate:frames(100) end
    separate:enter(ra)
    separate:samples(nil, 12)
    separate:returnCall()
end
separate:frames(79)
assert(not separate:has('script: duel_effect_jar_null_offset'))
separate:frames(1)
assert(separate:countOf('null_calls=1 loads=12/24 returned=true') == 2)
incomplete(separate)
stopped(separate, 'three seconds after the first target null return')

-- Losing residency invalidates the current first call even if the bank returns.
for address, value in pairs(MODULE) do
    local wrong = capture()
    wrong:word(address, 0)
    wrong:enter(BURST)
    wrong:samples()
    wrong:returnCall()
    wrong:reset()
    assert(wrong:has('null_calls=0 loads=0/24 returned=false'))
    incomplete(wrong)

    local swapped = capture()
    swapped:enter(BURST)
    swapped:sample(1, 0, 0)
    swapped:word(address, 0)
    swapped:sample(1, 0, 1)
    swapped:word(address, value)
    swapped:samples()
    swapped:enter(BURST)
    swapped:samples()
    swapped:returnCall()
    swapped:reset()
    assert(swapped:has('null_calls=2 loads=1/24 returned=false'))
    incomplete(swapped)
end

for _, load in ipairs(LOADS) do
    for _, offset in ipairs({-8, 0}) do
        local wrong = capture()
        wrong:word(HELPER + load[1] + offset, 0)
        wrong:enter(BURST)
        wrong:samples()
        wrong:returnCall()
        wrong:frames(180)
        assert(wrong:has('guard_failures: 4'))
        assert(wrong:has('loads=20/24 returned=true'))
        incomplete(wrong)
    end
end

for _, override in ipairs({
    {sp = ENTRY_SP}, {s2 = 0x80000000},
    {a2 = -1}, {a2 = 5}, {a2 = 0.5}, {v1 = -1}, {v1 = 0x10000},
}) do
    local wrong = capture()
    wrong:enter(BURST)
    wrong:samples(override)
    wrong:returnCall()
    wrong:frames(180)
    assert(wrong:has('guard_failures: 24'))
    assert(wrong:has('loads=0/24 returned=true'))
    incomplete(wrong)
end

local badReturn = capture()
badReturn:enter(BURST)
badReturn:samples()
badReturn:word(HELPER + 0x1c8, 0)
badReturn:returnCall()
badReturn:reset()
assert(badReturn:has('loads=24/24 returned=false'))
assert(badReturn:has('guard_failures: 1'))
incomplete(badReturn)

local unqualified = capture()
unqualified:call(CONTROL, {})
unqualified:call(HANDLER, {a1 = 1})
unqualified:enter(PARTICLE, 0x80100000)
unqualified:enter(BURST, 0x80100000)
unqualified:enter(BURST - 4)
unqualified:samples()
unqualified:reset()
assert(unqualified:has('unknown_null_callers: 1'))
assert(unqualified:has('null_calls=0 loads=0/24 returned=false'))
assert(unqualified:has('but neither target null caller was observed'))
incomplete(unqualified)

-- Dark Hole's phase zero is logged but is not the requested route.
local darkHole = capture()
darkHole:call(CONTROL, {})
darkHole:call(HANDLER, {a1 = 0})
darkHole:enter(PARTICLE, 0x80100000)
darkHole:reset()
assert(darkHole:has('#01 t=0.00s START effect=17 phase=0'))
assert(darkHole:has('phase1_initializations: 0'))
assert(darkHole:has('verdict: requested route/control evidence is incomplete'))
incomplete(darkHole)

local timeout = capture()
timeout:frames(7199)
assert(not timeout:has('script: duel_effect_jar_null_offset'))
timeout:frames(1)
assert(timeout:has('execution_breakpoint=NOT-observed'))
incomplete(timeout)
stopped(timeout, 'two-minute window ended')

local reset = capture()
reset:reset()
stopped(reset, 'reset; reload the script after returning to the duel')

local unload = capture()
unload:call(HANDLER, {a1 = 0})
unload:word(HANDLER, 0)
unload:frames(1)
incomplete(unload)
stopped(unload, 'observed duel bank is no longer resident')

local broken = capture()
PCSX.getRegisters = function() error('registers unavailable') end
broken:enter(BURST)
incomplete(broken)
stopped(broken, 'callback error:')

print, PCSX = originalPrint, nil
breakpoints_duel_effect_jar_null_offset = nil
listeners_duel_effect_jar_null_offset = nil
io.stdout:write('duel_effect_jar_null_offset_test: ok\n')
