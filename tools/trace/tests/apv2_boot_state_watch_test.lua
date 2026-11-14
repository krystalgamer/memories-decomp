-- Run from the repository root:
--   luajit tools/trace/tests/apv2_boot_state_watch_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/apv2_boot_state_watch.lua'

local RAM_BASE = 0x80000000
local CONTROL = 0x8001306c
local SELECT, SELECT_EXIT = 0x801680f4, 0x80168158
local CONTROLLER, CONTROLLER_EXIT = 0x80168160, 0x80168a90
local POLL, POLL_EXIT = 0x80168a98, 0x80168c64
local SEND, WARNING = 0x80168c6c, 0x80168d5c
local STATE, MESSAGE_INDEX = 0x80169474, 0x801694d0
local PARAMS, RESULTS = 0x80169464, 0x8016946c
local WINDOW, GRACE = 120 * 60, 5 * 60
local BREAKPOINTS = 13

-- The retail US image's signature words.
local MODULE_WORDS = {
    [0x80168000] = 0x16, [0x80168004] = 0x80168a70,
    [0x80168008] = 0x801681a8, [SELECT] = 0x3c03bfc8,
    [SELECT_EXIT] = 0x03e00008, [CONTROLLER] = 0x27bdffe0,
    [CONTROLLER_EXIT] = 0x03e00008, [POLL] = 0x00041080,
    [POLL_EXIT] = 0x03e00008, [SEND] = 0x00002821,
    [0x80168a30] = 0x0c01d05c, [0x80168d6c] = 0x0c01fcd4,
}
-- D_8016941C, as in tmp/overlays/apv2/module.bin.
local COMMAND_TABLE = {
    {0x01, 0, 1, 3}, {0x13, 0, 3, 3}, {0x14, 1, 3, 3}, {0x02, 3, 1, 3},
    {0x16, 0, 1, 5}, {0x0e, 1, 1, 3}, {0x0a, 0, 1, 5}, {0x0b, 0, 1, 3},
    {0x03, 0, 1, 3}, {0x19, 1, 1, 3}, {0x19, 1, 2, 3}, {0x09, 0, 1, 5},
    {0x1e, 0, 1, 5}, {0x1a, 0, 1, 5},
}
local ENGLISH = '     SOFTWARE TERMINATED\nCONSOLE MAY HAVE BEEN MODIFIED\n'

local function capture()
    local memory = ffi.new('uint8_t[?]', 0x800000)
    local r = {}
    for i = 0, 31 do r[i] = 0 end
    local names = {v0 = 2, v1 = 3, a0 = 4, a3 = 7, ra = 31}
    local registers = {pc = 0, GPR = {r = r, n = setmetatable({}, {
        __index = function(_, k) return names[k] and r[names[k]] end})}}
    local events, breakpoints, output = {}, {}, {}
    local function word(address, value)
        ffi.cast('uint32_t*', memory + (address % 0x20000000))[0] = value
    end
    local function byte(address, value)
        memory[address % 0x20000000] = value
    end
    word(CONTROL, 0x27bdffe0)
    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        settings = {emulator = {Debug = {Debug = true}, Dynarec = false}},
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, width, label, callback)
            assert(kind == 'Exec' and width == 4)
            local key = string.format('%08X', address)
            assert(breakpoints[key] == nil, 'duplicate ' .. key)
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
    breakpoints_apv2_boot_state_watch = nil
    listeners_apv2_boot_state_watch = nil
    dofile(scriptPath)

    local t = {output = output}
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
    function t:exec(address, set)
        for i = 0, 31 do r[i] = 0 end
        for k, v in pairs(set or {}) do r[names[k]] = v end
        registers.pc = address
        local bp = breakpoints[string.format('%08X', address)]
        assert(bp, string.format('no breakpoint at 0x%08X', address))
        if bp.enabled then bp.callback(address, 4, 'exec') end
    end
    function t:word(address, value) word(address, value) end
    function t:byte(address, value) byte(address, value) end
    -- Load the US image: signature words, command table, messages.
    function t:loadUs()
        for address, value in pairs(MODULE_WORDS) do word(address, value) end
        for i, c in ipairs(COMMAND_TABLE) do
            for k = 1, 4 do byte(0x8016941c + 4 * (i - 1) + k - 1, c[k]) end
        end
        for k = 1, #ENGLISH do
            byte(0x80168054 + k - 1, ENGLISH:byte(k))
        end
        word(0x8016947c + 8, 0x801680a8)
        word(0x80169488 + 8, 0x80168054)
    end
    -- Another module at 0x80168000.
    function t:loadOther()
        for address in pairs(MODULE_WORDS) do word(address, 0) end
        word(0x80168000, 0x2a)
    end
    function t:setState(s) word(STATE, s % 0x100000000) end
    -- One controller call: entry in state `from`, exit in state `to`.
    function t:controller(from, to, ret)
        self:setState(from)
        self:exec(CONTROLLER, {a0 = 1, ra = 0x80043b00})
        self:setState(to)
        self:exec(CONTROLLER_EXIT, {v0 = ret % 0x100000000})
    end
    function t:send(n, params)
        for k, v in ipairs(params or {}) do byte(PARAMS + k - 1, v) end
        self:exec(SEND, {a0 = n, ra = 0x801681b0})
    end
    function t:poll(n, ret, cause, results)
        for k, v in ipairs(results or {}) do byte(RESULTS + k - 1, v) end
        self:exec(POLL, {a0 = n, ra = 0x801681cc})
        self:exec(POLL_EXIT, {v0 = ret % 0x100000000, a3 = cause or 0,
                              a0 = 0x8016946c})
    end
    function t:rowLines()
        local n = 0
        for _, line in ipairs(output) do
            if line:sub(1, 2) == 'b=' then n = n + 1 end
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
    assert(t:countOf('script: apv2_boot_state_watch') == 1)
    assert(t:has('setup: debugger=on configured_cpu=interpreter'))
    assert(t:allDisabled())
    t:frames(WINDOW + 1)
    assert(t:countOf('==== TRACE RESULT =====') == 1, 'document printed once')
    assert(t:countOf('--- end of trace') == 1)
end

-- The clean boot, cut short: select, a few states with commands and
-- results, the wait state, then return 0.
local function cleanBoot(t)
    for _ = 1, 3 do t:exec(CONTROL) end
    t:loadUs()
    t:exec(SELECT, {ra = 0x80043a10})
    t:setState(1)
    t:word(MESSAGE_INDEX, 1)
    t:exec(SELECT_EXIT, {v1 = 0x41})
    t:send(1)
    t:controller(1, 2, 1)
    t:poll(1, 0)
    t:poll(1, 0)
    t:controller(2, 2, 2)
    t:controller(2, 2, 2)
    t:poll(1, 1, 3, {0x02, 0x01, 0x14})
    t:send(6)
    t:controller(2, 3, 2)
    t:send(3, {0x00, 0x02, 0x00})
    t:frames(30)
    for _ = 1, 1000 do t:controller(14, 14, 14) end
    t:send(10, {5})
    t:controller(14, 15, 14)
    t:poll(10, 1, 3, {0x02, 0x00})
    t:controller(18, 0, 18)
    t:controller(0, 0, 0)
end

-- A clean boot after a reset: every step logged, document printed once.
do
    local t = capture()
    assert(t:installed() == BREAKPOINTS)
    assert(t:has('control Main_RunFrameServices entry word ok'))
    assert(t:has('0x80168000 now holds another module'))
    assert(t:has('armed. Route: reset the console now'))
    t:exec(CONTROL)
    assert(t:has('execution breakpoint observed; setup confirmed'))
    t:frames(10)
    t:reset()
    assert(t:has('reset observed; per-boot capture cleared'))
    assert(t:has('13 breakpoints reinstalled at BIOS shell'))
    cleanBoot(t)
    assert(t:has('func_801680F4 entry ra=0x80043A10'))
    assert(t:has('func_801680F4 exit region_byte=0x41(A) state=1 '
                 .. 'message_index=1'))
    assert(t:has('func_80168C6C n=1 cmd=0x13(GetTN) nparams=0 nresults=3 '
                 .. 'wait=3 params=- ra=0x801681B0'))
    assert(t:has('func_80168C6C n=3 cmd=0x02(Setloc) nparams=3 nresults=1 '
                 .. 'wait=3 params=00 02 00'))
    assert(t:has('func_80168C6C n=10 cmd=0x19(Test) nparams=1 nresults=2 '
                 .. 'wait=3 params=05'))
    assert(t:has('func_80168A98 n=1 cmd=0x13(GetTN) nparams=0 nresults=3 '
                 .. 'wait=3 ret=1 cause=3 results=02 01 14 zero_polls=2'))
    assert(t:has('func_80168A98 n=10 cmd=0x19(Test) nparams=1 nresults=2 '
                 .. 'wait=3 ret=1 cause=3 results=02 00'))
    assert(t:has('func_80168160 a0=1 state 1 -> 2 ret=1'))
    assert(t:countOf('func_80168160 a0=1 state 2 -> 2 ret=2') == 1,
           'repeats print once')
    assert(t:countOf('state 14 -> 14 ret=14') == 1)
    assert(t:has('state 14 -> 15 ret=14 calls=1005 repeats_before=999'))
    assert(t:has('returned 0 after 1007 calls: APv2 finished'))
    t:frames(GRACE - 1)
    assert(not t:has('==== TRACE RESULT ====='), 'grace period pinned')
    t:exec(CONTROL)
    t:frames(1)
    assert(t:has('status: func_80168160 returned 0 and 5 s passed'))
    assert(t:has('resets: 1'))
    assert(t:has('verdict: APv2 completed on the unmodified path: '
                 .. 'func_801680F4 ran, func_80168160 returned 0 after 1007 '
                 .. 'calls at bt=0.5s, 4 CD commands sent, the warning never '
                 .. 'ran; the game ran 1 frame services afterwards'))
    assert(t:has('func_801680F4: calls=1 ra=0x80043A10 region_byte=0x41(A) '
                 .. 'state=1 message_index=1'))
    assert(t:has('func_80168160: calls=1007 first_state=1 final_state=0 '
                 .. 'returned_zero=yes at bt=0.5s'))
    assert(t:has('state_path: 1@0.0s > 2@0.0s > 3@0.0s > 14@0.5s > 15@0.5s '
                 .. '> 18@0.5s > 0@0.5s'))
    assert(t:has('state_entry_calls: 0:1 1:1 2:3 14:1001 18:1'))
    assert(t:has('func_80168C6C: sends=4 1(0x13 GetTN)x1 3(0x02 Setloc)x1 '
                 .. '6(0x0A Init)x1 10(0x19 Test)x1'))
    assert(t:has('func_80168A98: n=1 zero=2 ok=1 err=0 err_status=0'))
    assert(t:has('func_80168D5C: never ran'))
    assert(t:has('helpers: func_80168EC8=0 func_8016903C=0 '
                 .. 'func_801691F0=0 func_80169284=0'))
    assert(t:has('signature: verified_hits_this_boot='))
    assert(t:has('refused_hits=0'))
    finished(t)
end

-- Another module at 0x80168000 (or another region's image): hits are
-- refused and never counted; refused rows are bounded.
do
    local t = capture()
    t:reset()
    t:exec(CONTROL)
    t:loadOther()
    t:exec(SELECT)
    t:controller(1, 2, 1)
    t:send(1)
    for _ = 1, 20 do t:poll(1, 1, 3, {1}) end
    assert(t:countOf('refused func_') == 8)
    assert(t:has('refused func_801680F4: 0x80168000 does not hold the US '
                 .. 'APv2 image (12/12 words differ; module word 0x80168000 '
                 .. 'expected 0x00000016 read 0000002A)'))
    -- The Japanese image differs only in its jal words.
    t:loadUs()
    t:word(0x80168a30, 0x0c01d000)
    t:exec(SEND, {a0 = 1})
    t:frames(WINDOW)
    assert(t:has('status: 120-second window since the last reset ended'))
    assert(t:has('refused_hits=45'))
    assert(t:has('verified_hits_this_boot=0'))
    assert(t:has('func_80168160: calls=0'))
    assert(t:has('func_80168C6C: sends=0 -'))
    assert(t:has('verdict: inconclusive: only code other than the US APv2 '
                 .. 'image ran at the watched addresses (45 refused hits)'))
    finished(t)
end

-- Reset reinstall at the shell and at the first vsync; the per-boot
-- capture is cleared and an earlier boot keeps one summary line; the
-- warning path and the message text.
do
    local t = capture()
    t:exec(CONTROL)
    t:reset(false)
    assert(t:installed() == 0)
    t:frames(1)
    assert(t:has('13 breakpoints reinstalled at the first vsync after reset'))
    t:loadUs()
    t:setState(1)
    t:word(MESSAGE_INDEX, 1)
    t:exec(SELECT_EXIT, {v1 = 0x41})
    t:controller(1, 2, 1)
    t:send(1)
    t:reset()
    assert(t:countOf('reset observed') == 2)
    assert(t:installed() == BREAKPOINTS)
    t:setState(6)
    t:word(MESSAGE_INDEX, 1)
    t:poll(13, -1, 3, {0x02})
    t:exec(WARNING, {ra = 0x80168474})
    assert(t:has('WARNING func_80168D5C entry ra=0x80168474 state=6 '
                 .. 'message_index=1 text@0x80168054 "     SOFTWARE '
                 .. 'TERMINATED / CONSOLE MAY HAVE BEEN MODIFIED / "'))
    t:exec(0x80168ec8, {ra = 0x80168e40})
    t:exec(0x80168ec8, {ra = 0x80168e40})
    assert(t:countOf('func_80168EC8 first call ra=0x80168E40') == 1)
    t:frames(GRACE)
    assert(t:has('status: the warning ran and 5 s passed'))
    assert(t:has('resets: 2'))
    assert(t:has('earlier_boot 1: select=region 0x41 state 1 message 1 '
                 .. 'controller_calls=1 sends=1 path=1@0.0s>2@0.0s '
                 .. 'returned_zero=no warning=no'))
    assert(t:has('func_80168160: calls=0'), 'per-boot capture cleared')
    assert(t:has('func_80168A98: n=13 zero=0 ok=0 err=1'))
    assert(t:has('verdict: the warning func_80168D5C RAN'))
    assert(t:has('helpers: func_80168EC8=2'))
    finished(t)
end

-- Bad indexes and pointers are refused, never dereferenced.
do
    local t = capture()
    t:reset()
    t:loadUs()
    t:setState(3)
    t:exec(SEND, {a0 = 14})
    t:exec(SEND, {a0 = 0xffffffff})
    assert(t:has('func_80168C6C n=14 index refused (outside 0..13) params=-'))
    assert(t:has('func_80168C6C n=-1 index refused'))
    t:poll(200, 1, 3)
    assert(t:has('func_80168A98 n=200 index refused (outside 0..13) ret=1 '
                 .. 'cause=3 results=-'))
    -- Message index outside the table, and text pointers outside the module.
    t:word(MESSAGE_INDEX, 7)
    t:exec(WARNING)
    assert(t:has('message_index=7 message index 7 refused'))
    for _, ptr in ipairs({0, 0x80167000, 0x1f801800,
                          0x80169800, 0xbfc00000}) do
        t:word(MESSAGE_INDEX, 1)
        t:word(0x80169488 + 8, ptr)
        t:exec(WARNING)
        assert(t:has(string.format('text pointer 0x%08X refused', ptr)))
    end
    t:frames(GRACE)
    assert(not t:has('callback_error'))
    finished(t)
end

-- Bounded output: controller rows stop at 80, sends at 60, polls at 6 per
-- command, all rows at 200; counts stay exact.
do
    local t = capture()
    t:reset()
    t:loadUs()
    for i = 1, 100 do t:controller(i % 2 + 1, i % 2 + 2, 1) end
    for _ = 1, 70 do t:send(0) end
    for _ = 1, 10 do t:poll(0, 1, 3, {2}) end
    for n = 1, 13 do for _ = 1, 8 do t:poll(n, -2, 5, {0x12, 0x80}) end end
    assert(t:countOf('func_80168160 a0=1 state') == 80)
    assert(t:countOf('func_80168C6C n=0 cmd=0x01(Getstat)') == 60)
    assert(t:countOf('func_80168A98 n=0') == 6)
    assert(t:has('func_80168A98 n=1 cmd=0x13(GetTN) nparams=0 nresults=3 '
                 .. 'wait=3 ret=-2 cause=5 results=12 80'))
    assert(t:countOf('row budget of 200 reached') == 1)
    t:frames(WINDOW)
    assert(t:rowLines() == 400, 'two copies of 200 rows')
    assert(t:has('func_80168C6C: sends=70 0(0x01 Getstat)x70'))
    assert(t:has('func_80168A98: n=1 zero=0 ok=0 err=0 err_status=8'))
    assert(t:has('controller_rows=80/80 send_rows=60/60'))
    assert(t:has('verdict: inconclusive: APv2 ran but func_80168160 never '
                 .. 'returned 0 within the window (last state 2)'))
    finished(t)
end

-- No reset: APv2 cannot have run; the human is told to reset.
do
    local t = capture()
    t:exec(CONTROL)
    t:frames(WINDOW)
    assert(t:has('status: 120-second window since the arming ended'))
    assert(t:has('verdict: inconclusive: no reset observed after arming'))
    finished(t)
end

-- Reset, control fired again, APv2 silent.
do
    local t = capture()
    t:reset()
    t:exec(CONTROL)
    t:frames(WINDOW)
    assert(t:has('verdict: inconclusive: no APv2 breakpoint fired after the '
                 .. 'reset (control hits this boot=1)'))
    finished(t)
end

-- Nothing fired: the setup is blamed.
do
    local t = capture()
    t:reset()
    t:frames(WINDOW)
    assert(t:has('verdict: nothing fired, not even the Main_RunFrameServices '
                 .. 'control: the setup failed'))
    assert(t:has('execution_breakpoint=NOT-observed'))
    finished(t)
end

-- A callback failure ends the trace.
do
    local t = capture()
    t:reset()
    t:loadUs()
    PCSX.getRegisters = function() error('boom') end
    t:exec(SEND)
    t:frames(1)
    assert(t:has('status: callback error'))
    assert(t:has('callback_error: '))
    assert(t:has('verdict: inconclusive: a callback failed'))
    finished(t)
end

print = hostPrint
PCSX = nil
print('apv2_boot_state_watch_test: all scenarios passed')
