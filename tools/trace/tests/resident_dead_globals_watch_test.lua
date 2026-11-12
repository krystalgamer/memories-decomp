-- Run from the repository root:
--   luajit tools/trace/tests/resident_dead_globals_watch_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/resident_dead_globals_watch.lua'

local RAM_BASE = 0x80000000
local CONTROL, BOOT = 0x8001306c, 0x80012b50
local MOVIE_CALL, EMPTY_CALL = 0x8005c374, 0x8003d32c
local GP = 0x8009af08
local WINDOW = 300 * 60
local OP = {lbu = 0x24, lhu = 0x25, lw = 0x23, sb = 0x28, sh = 0x29, sw = 0x2b}
local WIDTH = {lbu = 1, lhu = 2, lw = 4, sb = 1, sh = 2, sw = 4}
local TARGETS = {
    [0x8009b0a4] = 4, [0x8009b0b0] = 4, [0x8009b0bc] = 4, [0x8009b0d1] = 1,
    [0x8009b0d4] = 4, [0x8009b268] = 1, [0x8009b2b2] = 1, [0x8009b2de] = 2,
    [0x8009b4a0] = 3,
}
-- Retail words the script verifies before trusting a silent capture.
local WORDS = {
    [CONTROL] = 0x27bdffe0, [BOOT] = 0x27bdffe8, [EMPTY_CALL] = 0x03e00008,
    [0x80012bd4] = 0xa38001c9, [0x800130d0] = 0x8f8201a8,
    [0x800130d4] = 0x8f83019c, [0x800130e8] = 0x8f8201b4,
    [0x800130ec] = 0x8f8301cc, [0x80013118] = 0x8f82019c,
    [0x8001311c] = 0x8f8401cc, [0x80013128] = 0xaf8201a8,
    [0x8001312c] = 0xaf8401b4, [0x8002d460] = 0xa3820360,
    [0x8002d4cc] = 0xa020b0d1, [0x8002d54c] = 0xa3800360,
    [0x8002d568] = 0xa3800360, [0x8002d5c0] = 0x93840360,
    [0x8002d794] = 0xa3820360, [0x8002f158] = 0xa022b268,
    [0x8002f218] = 0xa022b268, [0x8002fa30] = 0xa022b268,
    [0x800301ac] = 0xa38203aa, [0x80030d34] = 0xa022b268,
    [0x80030fbc] = 0xa38003aa, [0x80031370] = 0xa78003d6,
    [0x80031384] = 0x978203d6, [0x8003f7f4] = 0xa020b0d1,
    [0x8005c374] = 0xa3840598, [0x8005c378] = 0xa3850599,
    [0x8005c37c] = 0xa386059a,
}

local function capture()
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local r = {}
    for i = 0, 31 do r[i] = 0 end
    r[28] = GP
    local names = {a0 = 4, a1 = 5, a2 = 6, ra = 31}
    local registers = {pc = 0, GPR = {r = r, n = setmetatable({}, {
        __index = function(_, k) return names[k] and r[names[k]] end})}}
    local events, breakpoints, output = {}, {}, {}
    local function word(address, value)
        ffi.cast('uint32_t*', memory + (address - RAM_BASE))[0] = value
    end
    for address, value in pairs(WORDS) do word(address, value) end
    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        settings = {emulator = {Debug = {Debug = true}, Dynarec = false}},
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, width, label, callback)
            assert(kind == 'Exec' or kind == 'Read' or kind == 'Write')
            local key = kind .. string.format('%08X', address)
            assert(breakpoints[key] == nil, 'duplicate ' .. key)
            if kind ~= 'Exec' then
                assert(TARGETS[address] == width, 'width of ' .. key)
            end
            local handle = {enabled = true, callback = callback}
            function handle:disable() self.enabled = false end
            breakpoints[key] = handle
            return handle
        end,
        Events = {createEventListener = function(name, callback)
            events[name] = callback
            return {}
        end},
    }
    breakpoints_resident_dead_globals_watch = nil
    listeners_resident_dead_globals_watch = nil
    dofile(scriptPath)

    local t = {output = output, memory = memory}
    function t:countOf(text)
        local n = 0
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then n = n + 1 end
        end
        return n
    end
    function t:has(text) return self:countOf(text) > 0 end
    function t:installed()
        local n = 0
        for _ in pairs(breakpoints) do n = n + 1 end
        return n
    end
    function t:frames(n) for _ = 1, n do events['GPU::Vsync']() end end
    function t:reset(shell)
        breakpoints = {}
        events['ExecutionFlow::Reset']()
        if shell ~= false then events['ExecutionFlow::ShellReached']() end
    end
    function t:fire(kind, address, ...)
        local bp = breakpoints[kind .. string.format('%08X', address)]
        assert(bp, 'no ' .. kind .. ' breakpoint at ' .. address)
        if bp.enabled then bp.callback(...) end
    end
    function t:exec(address, ra, a0, a1, a2)
        r[31], r[4], r[5], r[6] = ra or 0, a0 or 0, a1 or 0, a2 or 0
        registers.pc = address
        self:fire('Exec', address, address, 4, 'exec')
    end
    -- One load or store at pc, through register 8 (+0x10) unless `raw`
    -- supplies the retail instruction; value in r5 or r2/r4 for retail
    -- stores. `bpBase` is the watched symbol whose breakpoint fires.
    function t:access(name, pc, address, bpBase, ra, value, raw)
        if raw == nil then
            word(pc, OP[name] * 0x4000000 + 8 * 0x200000 + 5 * 0x10000 + 0x10)
        end
        r[8], r[5], r[31] = address - 0x10, value or 0, ra or 0
        r[2], r[4], r[1] = value or 0, value or 0, 0x800a0000
        registers.pc = pc
        local store = name:sub(1, 1) == 's'
        self:fire(store and 'Write' or 'Read', bpBase, address, WIDTH[name],
                  'mem')
        if store then
            for k = 0, WIDTH[name] - 1 do
                memory[address + k - RAM_BASE] =
                    math.floor((value or 0) / 2 ^ (8 * k)) % 0x100
            end
        end
    end
    function t:word(address, value) word(address, value) end
    function t:rowLines()
        local n = 0
        for _, line in ipairs(output) do
            if line:sub(1, 3) == 'vs=' then n = n + 1 end
        end
        return n
    end
    function t:allDisabled()
        for _, bp in pairs(breakpoints) do
            if bp.enabled then return false end
        end
        return true
    end
    return t
end

local function finished(t)
    assert(t:countOf('==== TRACE RESULT =====') == 1)
    assert(t:countOf('==== USER CONTEXT ====') == 1)
    assert(t:countOf('script: resident_dead_globals_watch') == 1)
    assert(t:has('setup: debugger=on configured_cpu=interpreter'))
    assert(t:allDisabled())
    t:frames(100)
    t:reset()
    t:frames(WINDOW + 1)
    assert(t:countOf('==== TRACE RESULT =====') == 1, 'document printed once')
    assert(t:countOf('--- end of trace') == 1)
end

local function booted()
    local t = capture()
    t:reset()
    t:exec(BOOT, 0x80010100)
    for _ = 1, 5 do t:exec(CONTROL) end
    return t
end

-- Static accesses are labelled, an overlay reader is UNEXPECTED and named by
-- its signature, a non-zero write and a movie call are reported.
do
    local t = capture()
    assert(t:installed() == 4 + 18, 'four exec and eighteen memory watches')
    assert(t:has('signatures 31/31 US words ok'))
    assert(t:has('armed. Route: soft-reset'))
    t:reset()
    assert(t:has('reset observed; state cleared'))
    assert(t:has('22 breakpoints reinstalled at BIOS shell'))
    t:exec(BOOT, 0x80010100)
    assert(t:has('milestone Main_Init entered'))
    t:frames(1)
    t:access('sb', 0x80012bd4, 0x8009b0d1, 0x8009b0d1, 0, 0, true)
    assert(t:has('static write pc=0x80012BD4 in=resident(Main_Init) '
                 .. 'ra=0x00000000 ra_in=unidentified'))
    assert(t:has('at=D_8009B0D1 width=1 00 -> 00 decode=ok'))
    for _ = 1, 3 do
        t:exec(CONTROL)
        t:access('lw', 0x800130d4, 0x8009b0a4, 0x8009b0a4, 0, 0, true)
    end
    assert(t:countOf('static read pc=0x800130D4') == 2, 'static rows capped')
    -- main_menu overlay is resident and reads D_8009B0D1.
    t:word(0x80180000, 0x0f)
    t:word(0x80180004, 0x8018416c)
    t:word(0x80180008, 0x80183514)
    t:access('lbu', 0x80181234, 0x8009b0d1, 0x8009b0d1, 0x80180500)
    assert(t:has('UNEXPECTED read pc=0x80181234 in=main_menu+0x1234 '
                 .. 'ra=0x80180500 ra_in=main_menu+0x500 at=D_8009B0D1 '
                 .. 'width=1 value=00 decode=ok'))
    -- A non-zero store to D_8009B2DE from unidentified code.
    t:access('sh', 0x801f0000, 0x8009b2de, 0x8009b2de, 0, 0x1234)
    assert(t:has('UNEXPECTED write pc=0x801F0000 in=unidentified[code '))
    assert(t:has('at=D_8009B2DE width=2 0000 -> 1234 decode=ok'))
    -- A byte inside the three-byte movie triple.
    t:access('lbu', 0x80020000, 0x8009b4a2, 0x8009b4a0, 0)
    assert(t:has('UNEXPECTED read pc=0x80020000 in=resident ra=0x00000000 '
                 .. 'ra_in=unidentified'))
    assert(t:has('at=D_8009B4A0+2 width=1'))
    t:exec(MOVIE_CALL, 0x80181000, 1, 2, 3)
    assert(t:has('CALL func_8005C374 ra=0x80181000 ra_in=main_menu+0x1000 '
                 .. 'a0=0x00000001 a1=0x00000002 a2=0x00000003'))
    t:exec(EMPTY_CALL, 0x80031000)
    assert(t:has('CALL func_8003D32C ra=0x80031000 ra_in=resident'))
    t:frames(1)
    assert(t:has('change D_8009B2DE 0000->1234'))
    t:frames(WINDOW - 1)
    assert(t:has('status: 300-second window ended'))
    assert(t:has('qualified: yes'))
    assert(t:has('verdict: UNEXPECTED accesses=3 and calls=2 '
                 .. '(func_8005C374=1 func_8003D32C=1)'))
    assert(t:has('symbol D_8009B0D1 static_reads=0 static_writes=1 '
                 .. 'UNEXPECTED_reads=1 UNEXPECTED_writes=0 nonzero_writes=0'))
    assert(t:has('symbol D_8009B2DE static_reads=0 static_writes=0 '
                 .. 'UNEXPECTED_reads=0 UNEXPECTED_writes=1 nonzero_writes=1'))
    assert(t:has('symbol D_8009B0A4 static_reads=3'))
    assert(t:has('overlays_at_end: main_menu'))
    assert(t:has('pc read pc=0x800130D4 D_8009B0A4 static count=3'))
    assert(t:has('calls: func_8005C374=1 func_8003D32C=1'))
    finished(t)
end

-- The right pc touching the wrong symbol is not the static access.
do
    local t = booted()
    t:access('lw', 0x800130d4, 0x8009b0d4, 0x8009b0d4, 0)
    assert(t:has('UNEXPECTED read pc=0x800130D4 in=resident('
                 .. 'Main_RunFrameServices)'))
    t:frames(WINDOW)
    finished(t)
end

-- Overworld modules share code; the data word picks the variant, and an
-- unknown variant still names the family.
do
    local t = booted()
    t:word(0x80168000, 0x14)
    t:word(0x80168004, 0x27bdffe0)
    t:word(0x80168008, 0xafb10014)
    t:word(0x8016a2c8, 0x1111)
    t:access('lbu', 0x80168100, 0x8009b2b2, 0x8009b2b2, 0)
    assert(t:has('UNEXPECTED read pc=0x80168100 in=overworld_after_coup+0x100'))
    t:word(0x8016a2c8, 0x7777)
    t:access('lbu', 0x80168104, 0x8009b2b2, 0x8009b2b2, 0)
    assert(t:has('UNEXPECTED read pc=0x80168104 in=overworld+0x104'))
    t:frames(WINDOW)
    assert(t:has('overlays_at_end: overworld'))
    finished(t)
end

-- Bounded output: unexpected repeats stop at four rows, the console and the
-- document at 200 rows, the table at 64 pairs, and the counts stay exact.
do
    local t = booted()
    for _ = 1, 50 do t:access('lbu', 0x80030000, 0x8009b268, 0x8009b268, 0) end
    assert(t:countOf('UNEXPECTED read pc=0x80030000') == 4)
    for _ = 1, 4 do
        for i = 1, 250 do
            t:access('lbu', 0x80040000 + 4 * i, 0x8009b2b2, 0x8009b2b2, 0)
        end
    end
    assert(t:countOf('row budget of 200 reached') == 1)
    t:frames(WINDOW)
    assert(t:rowLines() == 400, 'two copies of 200 rows: live and document')
    assert(t:countOf('pc read pc=') == 64)
    assert(t:has('accesses: reads=1050 writes=0 static=0 unexpected=1050'))
    assert(t:has('bounds: rows=200/200 tracked_pcs=64/64 '
                 .. 'untracked_pc_accesses=748'))
    finished(t)
end

-- A qualified capture with only static accesses is negative evidence.
do
    local t = booted()
    t:access('lw', 0x800130d4, 0x8009b0a4, 0x8009b0a4, 0, 0, true)
    t:frames(WINDOW)
    assert(t:has('qualified: yes'))
    assert(t:has('verdict: only the statically known accesses and no call '
                 .. 'of func_8005C374 or func_8003D32C during 5 control '
                 .. 'frames of a qualified capture'))
    finished(t)
end

-- No reset after arming: inconclusive, even with the control firing.
do
    local t = capture()
    for _ = 1, 30 do t:exec(CONTROL) end
    t:frames(WINDOW)
    assert(t:has('verdict: inconclusive: no reset observed after arming; '
                 .. 'no Main_RunFrameServices control after the reset; '
                 .. 'silence here is not evidence'))
    assert(not t:has('negative evidence'))
    finished(t)
end

-- Reset but the control never fired: inconclusive, findings still named.
do
    local t = capture()
    t:reset()
    t:access('lbu', 0x80050000, 0x8009b0d1, 0x8009b0d1, 0)
    t:frames(WINDOW)
    assert(t:has('qualified: no (no Main_RunFrameServices control after '
                 .. 'the reset)'))
    assert(t:has('unexpected accesses=1 calls=0 were still seen'))
    finished(t)
end

-- A mismatched US word voids silence.
do
    local t = booted()
    t:word(0x80031384, 0x978203d8)
    t:frames(WINDOW)
    assert(t:has('signatures: 30/31 US words ok'))
    assert(t:has('signature DebugMenu_UpdateBustUpEntry 0x80031384 expected '
                 .. '0x978203D6 read 978203D8 MISMATCH'))
    assert(t:has('verdict: inconclusive: US signature mismatch'))
    finished(t)
end

-- A callback failure ends the trace and voids silence.
do
    local t = booted()
    PCSX.getRegisters = function() error('boom') end
    t:access('lw', 0x80060004, 0x8009b0a4, 0x8009b0a4, 0)
    t:frames(1)
    assert(t:has('status: callback error'))
    assert(t:has('callback_error: '))
    assert(t:has('verdict: inconclusive: window did not complete; a callback '
                 .. 'failed; silence here is not evidence'))
    finished(t)
end

-- Nothing fired at all: the setup is blamed.
do
    local t = capture()
    t:frames(WINDOW)
    assert(t:has('verdict: nothing fired, not even the Main_RunFrameServices '
                 .. 'control: the setup failed'))
    assert(t:has('execution_breakpoint=NOT-observed'))
    finished(t)
end

-- No ShellReached: the first vsync reinstalls and old hits are dropped.
do
    local t = capture()
    t:exec(CONTROL)
    t:access('lbu', 0x80050000, 0x8009b0d1, 0x8009b0d1, 0)
    t:reset(false)
    assert(t:installed() == 0)
    t:frames(1)
    assert(t:has('22 breakpoints reinstalled at the first vsync after reset'))
    t:exec(CONTROL)
    t:frames(WINDOW)
    assert(t:has('accesses: reads=0 writes=0'))
    assert(t:has('qualified: yes'))
    finished(t)
end

print = hostPrint
PCSX = nil
print('resident_dead_globals_watch_test: all scenarios passed')
