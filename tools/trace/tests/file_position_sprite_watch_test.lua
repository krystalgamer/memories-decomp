-- Run from the repository root:
--   luajit tools/trace/tests/file_position_sprite_watch_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/file_position_sprite_watch.lua'

local RAM_BASE = 0x80000000
local SPRITE, BYTE = 0x800e9df0, 0x8009b0e0
local CONTROL, INIT = 0x8001306c, 0x800136e4
local GP = 0x8009af08
local WINDOW = 180 * 60
local OP = {lbu = 0x24, lhu = 0x25, lw = 0x23, sb = 0x28, sh = 0x29, sw = 0x2b}
local WIDTH = {lbu = 1, lhu = 2, lw = 4, sb = 1, sh = 2, sw = 4}

local function capture()
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local r = {}
    for i = 0, 31 do r[i] = 0 end
    r[28] = GP
    local registers = {pc = 0, GPR = {r = r, n = setmetatable({}, {
        __index = function(_, k) if k == 'ra' then return r[31] end end})}}
    local events, breakpoints, output = {}, {}, {}
    local function word(address, value)
        ffi.cast('uint32_t*', memory + (address - RAM_BASE))[0] = value
    end
    word(CONTROL, 0x27bdffe0)
    word(INIT, 0x27bdffd8)
    word(0x80013770, 0xa38001d8)
    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        settings = {emulator = {Debug = {Debug = true}, Dynarec = false}},
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, width, label, callback)
            assert(kind == 'Exec' or kind == 'Read' or kind == 'Write')
            local key = kind .. string.format('%08X', address)
            assert(breakpoints[key] == nil, 'duplicate ' .. key)
            local handle = {enabled = true, callback = callback,
                            width = width}
            function handle:disable() self.enabled = false end
            breakpoints[key] = handle
            return handle
        end,
        Events = {createEventListener = function(name, callback)
            events[name] = callback
            return {}
        end},
    }
    breakpoints_file_position_sprite_watch = nil
    listeners_file_position_sprite_watch = nil
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
    -- The emulator drops every Lua breakpoint, then reaches the shell.
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
    function t:exec(address, ra)
        r[31] = ra or 0
        registers.pc = address
        self:fire('Exec', address, address, 4, 'exec')
    end
    -- One load or store at pc through base register 4 (+0x10), value in
    -- r5. The callback gets `address` (nil when it is false); the
    -- instruction decodes to `decoded`, which defaults to the same address.
    function t:access(name, pc, address, ra, value, raw, decoded)
        local target = decoded or address
        word(pc, raw or (OP[name] * 0x4000000 + 4 * 0x200000 + 5 * 0x10000
                         + 0x10))
        r[4], r[5], r[31] = target - 0x10, value or 0, ra or 0
        registers.pc = pc
        local store = name:sub(1, 1) == 's'
        local where = address or target
        local bpAddr = (where >= SPRITE and where < SPRITE + 0x24)
            and SPRITE or BYTE
        self:fire(store and 'Write' or 'Read', bpAddr, address or nil,
                  address and WIDTH[name] or nil, 'mem')
        if store and address then
            for k = 0, WIDTH[name] - 1 do
                memory[address + k - RAM_BASE] =
                    math.floor((value or 0) / 2 ^ (8 * k)) % 0x100
            end
        end
    end
    function t:word(address, value) word(address, value) end
    -- Lines printed so far that are rows (they start with a stamp).
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
    assert(t:countOf('script: file_position_sprite_watch') == 1)
    assert(t:has('setup: debugger=on configured_cpu=interpreter'))
    assert(t:allDisabled())
    t:frames(100)
    t:reset()
    t:frames(WINDOW + 1)
    assert(t:countOf('==== TRACE RESULT =====') == 1, 'document printed once')
    assert(t:countOf('--- end of trace') == 1)
end

-- Arm, reset, run the boot init, and start live control frames.
local function booted()
    local t = capture()
    t:reset()
    t:exec(INIT, 0x80012abc)
    for _ = 1, 5 do t:exec(CONTROL) end
    return t
end

-- Arm, reset, boot init, a later read, a later byte write, window end.
do
    local t = capture()
    assert(t:installed() == 6, 'control, init, and four memory watches')
    assert(t:has('armed. Route: soft-reset'))
    t:reset()
    assert(t:has('reset observed; state cleared'))
    assert(t:has('6 breakpoints reinstalled at BIOS shell'))
    assert(t:installed() == 6)
    -- The startup bss clear runs before File_SetPositionTable.
    t:access('sw', 0x80010100, SPRITE + 0x14, 0, 0)
    assert(t:has('sprite pre-init write pc=0x80010100'))
    t:frames(3)
    t:exec(INIT, 0x80012abc)
    assert(t:has('milestone File_SetPositionTable entered ra=0x80012ABC'))
    t:access('sh', 0x80013750, SPRITE + 4, 0, 0x120)
    t:access('sh', 0x80013758, SPRITE + 6, 0, 0xd0)
    t:access('sw', 0x80013768, SPRITE, 0, 0x8000000)
    t:access('sb', 0x80013770, BYTE, 0, 0, 0xa38001d8)
    t:access('sw', 0x80013774, SPRITE + 0x14, 0, 0x808080)
    assert(t:has('sprite init write pc=0x80013750(File_SetPositionTable) '
                 .. 'ra=0x00000000 at=sprite+0x04(x) width=2 0000 -> 0120 '
                 .. 'decode=ok'))
    assert(t:has('byte init write pc=0x80013770(File_SetPositionTable) '
                 .. 'ra=0x00000000 at=D_8009B0E0 width=1 00 -> 00 decode=ok'))
    t:frames(1)
    assert(t:has('change attribute 00000000->08000000 x 0000->0120 '
                 .. 'y 0000->00D0 r 00->80 g 00->80 b 00->80'))
    for _ = 1, 5 do t:exec(CONTROL) end
    t:access('lhu', 0x80020000, SPRITE + 4, 0x80020100)
    assert(t:has('vs=4 f=5 t=   0.1s sprite post-init read pc=0x80020000(?) '
                 .. 'ra=0x80020100 at=sprite+0x04(x) width=2 value=0120 '
                 .. 'decode=ok'))
    t:access('sb', 0x80021000, BYTE, 0x80021100, 1)
    assert(t:has('byte post-init write pc=0x80021000(?) ra=0x80021100 '
                 .. 'at=D_8009B0E0 width=1 00 -> 01 decode=ok'))
    t:frames(1)
    assert(t:has('change D_8009B0E0 00->01'))
    t:frames(WINDOW - 6)
    assert(not t:has('==== TRACE RESULT'), 'window counts from the reset')
    t:frames(1)
    assert(t:has('status: 180-second window ended'))
    assert(t:has('resets: 1 init_entries: 1 init_ra: 0x80012ABC'))
    assert(t:has('accesses: reads=1 writes=7 pre-init=1 init=5 post-init=2 '
                 .. '(post-init reads=1 writes=1)'))
    assert(t:has('boot_init: observed'))
    assert(t:has('qualified: yes'))
    assert(t:has('verdict: post-init accesses observed: reads=1 writes=1 '
                 .. 'from 2 tracked pc/kind pairs (+0 accesses'))
    assert(t:has('signature File_SetPositionTable sb 0x80013770 expected '
                 .. '0xA38001D8 read A38001D8 ok'))
    assert(t:has('pc write pc=0x80021000 byte post-init count=1 changed=1'))
    assert(t:has('final: byte=01 attribute=08000000 x=0120 y=00D0'))
    assert(t:has('==== USER CONTEXT ===='))
    finished(t)
end

-- The callback's address is the ground truth. A decoded instruction that
-- disagrees is named but never substituted, and its store value is dropped;
-- a callback without an address is reported as unknown.
do
    local t = booted()
    t:access('lhu', 0x80070000, SPRITE + 4, 0, 0, nil, SPRITE + 0x20)
    assert(t:has('sprite post-init read pc=0x80070000(?) ra=0x00000000 '
                 .. 'at=sprite+0x04(x) width=2 value=0000 '
                 .. 'decode=mismatch(0x800E9E10/2)'))
    t:access('sb', 0x80070004, BYTE, 0, 7, nil, BYTE + 0x40)
    assert(t:has('byte post-init write pc=0x80070004(?) ra=0x00000000 '
                 .. 'at=D_8009B0E0 width=1 00 -> ?? decode=mismatch'))
    t:access('lw', 0x80070008, SPRITE + 8, 0, 0, 0x00000000)
    assert(t:has('pc=0x80070008(?) ra=0x00000000 at=sprite+0x08(w) '
                 .. 'width=4 value=00000000 decode=none'))
    t:access('lw', 0x8007000c, false, 0, 0, nil, SPRITE + 0x10)
    assert(t:has('pc=0x8007000C(?) ra=0x00000000 '
                 .. 'at=unknown(callback gave no address) decode=mismatch'))
    t:frames(WINDOW)
    assert(t:has('decode_mismatch=3 no_callback_address=1'))
    assert(t:has('pc read pc=0x8007000C sprite post-init count=1 changed=0')
           and t:has('at=unknown'))
    finished(t)
end

-- Bounded output: repeats of one pc stop after four rows, the console and
-- the document both stop at 200 rows, and the per-pc table at 64 pairs
-- while the aggregate counts stay exact.
do
    local t = booted()
    local before = t:rowLines()
    for _ = 1, 50 do t:access('lw', 0x80030000, SPRITE + 0x1c, 0x80030100) end
    assert(t:countOf('sprite post-init read pc=0x80030000') == 4)
    for _ = 1, 4 do
        for i = 1, 250 do
            t:access('lbu', 0x80040000 + 4 * i, BYTE, 0x80040800)
        end
    end
    assert(t:rowLines() - before == 200 - before,
           'live rows stop at the budget')
    assert(t:countOf('row budget of 200 reached') == 1)
    t:frames(WINDOW)
    assert(t:rowLines() == 400, 'two copies of 200 rows: live and document')
    local tableLines = t:countOf('pc read pc=') + t:countOf('pc write pc=')
    assert(tableLines == 64, 'final table capped: ' .. tableLines)
    assert(t:has('accesses: reads=1050 writes=0'))
    assert(t:has('bounds: rows=200/200 tracked_pcs=64/64 '
                 .. 'untracked_pc_accesses=748'))
    assert(t:has('repeat_rows_suppressed=46'))
    assert(t:has('pc read pc=0x80030000 sprite post-init count=50'))
    assert(t:has('pc read pc=0x800400FC byte post-init count=4'))
    assert(not t:has('pc read pc=0x80040100 '))
    assert(t:has('verdict: post-init accesses observed: reads=1050 writes=0 '
                 .. 'from 64 tracked pc/kind pairs (+748 accesses'))
    finished(t)
end

-- A qualified capture with no access at all is negative evidence.
do
    local t = booted()
    t:frames(WINDOW)
    assert(t:has('qualified: yes'))
    assert(t:has('verdict: no CPU read or write of D_800E9DF0 or D_8009B0E0 '
                 .. 'after File_SetPositionTable during 5 control frames of '
                 .. 'a qualified capture'))
    finished(t)
end

-- Control alone, without a reset or the init: inconclusive.
do
    local t = capture()
    for _ = 1, 30 do t:exec(CONTROL) end
    t:frames(WINDOW)
    assert(t:has('boot_init: NOT observed; arm the script, then soft-reset'))
    assert(t:has('verdict: inconclusive: no reset observed after arming; '
                 .. 'no Main_RunFrameServices control after the init; '
                 .. 'silence here is not evidence'))
    assert(not t:has('negative evidence'))
    finished(t)
end

-- A reset without the init, and accesses seen anyway: still inconclusive.
do
    local t = capture()
    t:reset()
    for _ = 1, 30 do t:exec(CONTROL) end
    t:access('lbu', 0x80080000, BYTE, 0)
    t:frames(WINDOW)
    assert(t:has('qualified: no (File_SetPositionTable not entered after '
                 .. 'the reset; no Main_RunFrameServices control after the '
                 .. 'init)'))
    assert(t:has('verdict: inconclusive: File_SetPositionTable not entered '
                 .. 'after the reset'))
    assert(t:has('byte pre-init read pc=0x80080000'))
    finished(t)
end

-- The init ran but the control never fired after it: inconclusive.
do
    local t = capture()
    t:reset()
    t:exec(CONTROL)
    t:exec(INIT, 0x80012abc)
    t:frames(WINDOW)
    assert(t:has('verdict: inconclusive: no Main_RunFrameServices control '
                 .. 'after the init; silence here is not evidence'))
    finished(t)
end

-- Mismatched US signature words: inconclusive, even with silence.
do
    local t = booted()
    t:word(0x80013770, 0xa38001e0)
    t:frames(WINDOW)
    assert(t:has('signature File_SetPositionTable sb 0x80013770 expected '
                 .. '0xA38001D8 read A38001E0 MISMATCH'))
    assert(t:has('verdict: inconclusive: US signature mismatch; silence '
                 .. 'here is not evidence'))
    finished(t)
end

-- An early callback failure ends the trace and voids silence.
do
    local t = booted()
    PCSX.getRegisters = function() error('boom') end
    t:access('lw', 0x80060004, SPRITE + 8, 0)
    t:frames(1)
    assert(t:has('status: callback error'))
    assert(t:has('callback_error: '))
    assert(t:has('verdict: inconclusive: window did not complete; a callback '
                 .. 'failed; silence here is not evidence'))
    finished(t)
end

-- Nothing fired at all: the setup is blamed, not the game.
do
    local t = capture()
    t:frames(WINDOW)
    assert(t:has('verdict: nothing fired, not even the Main_RunFrameServices '
                 .. 'control: the setup failed'))
    assert(t:has('execution_breakpoint=NOT-observed'))
    finished(t)
end

-- No ShellReached after reset: the first vsync reinstalls, and hits from
-- before the reset are gone from the result.
do
    local t = capture()
    t:exec(CONTROL)
    t:access('lhu', 0x80050000, SPRITE + 4, 0)
    t:reset(false)
    assert(t:installed() == 0)
    t:frames(1)
    assert(t:has('6 breakpoints reinstalled at the first vsync after reset'))
    assert(t:installed() == 6)
    t:exec(INIT, 0x80012abc)
    t:exec(CONTROL)
    t:frames(WINDOW)
    assert(t:has('breakpoints: reinstalled at the first vsync after reset'))
    assert(t:has('control_hits: 1 resets: 1'))
    assert(t:has('accesses: reads=0 writes=0'))
    assert(not t:has('pc read pc=0x80050000'))
    assert(t:has('qualified: yes'))
    finished(t)
end

print = hostPrint
PCSX = nil
print('file_position_sprite_watch_test: all scenarios passed')
