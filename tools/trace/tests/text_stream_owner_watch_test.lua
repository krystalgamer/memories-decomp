-- Run from the repository root:
--   luajit tools/trace/tests/text_stream_owner_watch_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/text_stream_owner_watch.lua'

local CONTROL = 0x8001306c
local BUILD, CARD, DECIMAL = 0x800393b0, 0x80037da4, 0x80038148
local STAGED, OFFSET, ENDSTREAM = 0x800383dc, 0x80038db8, 0x80038e7c
local READ_U32, READ_BYTE = 0x80036d70, 0x8003b7e0
local CHANNEL = 0x800eb0f8 + 0x64
local WINDOW = 180 * 60
local WORDS = {
    [CONTROL] = {0x27bdffe0, 0xafbf0018},
    [BUILD] = {0x27bdffe0, 0xafb00010},
    [CARD] = {0x27bdffe8, 0xafb00010},
    [DECIMAL] = {0x27bdffd8, 0xafb1001c},
    [STAGED] = {0x97860426, 0x3402cfff},
    [OFFSET] = {0x27bdffe8, 0xafb00010},
    [ENDSTREAM] = {0x90820058, 0x00000000},
    [READ_U32] = {0x80820058, 0x00000000},
    [READ_BYTE] = {0x80820058, 0x00000000},
}

local function capture(prepare)
    local memory = ffi.new('uint8_t[?]', 0x800000)
    local r = {}
    for i = 0, 31 do r[i] = 0 end
    local names = {a0 = 4, ra = 31}
    local registers = {pc = 0, GPR = {r = r, n = setmetatable({}, {
        __index = function(_, k) return names[k] and r[names[k]] end})}}
    local events, breakpoints, output = {}, {}, {}
    local function word(address, value)
        ffi.cast('uint32_t*', memory + (address % 0x20000000))[0] = value
    end
    for address, pair in pairs(WORDS) do
        word(address, pair[1])
        word(address + 4, pair[2])
    end
    if prepare then prepare(word) end
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
    breakpoints_text_stream_owner_watch = nil
    listeners_text_stream_owner_watch = nil
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
    function t:reset() events['ExecutionFlow::Reset']() end
    function t:exec(address, a0, ra)
        r[4], r[31] = a0 or 0, ra or 0
        registers.pc = address
        local bp = breakpoints[string.format('%08X', address)]
        assert(bp, 'no breakpoint at ' .. address)
        if bp.enabled then bp.callback(address, 4, 'exec') end
    end
    function t:word(address, value) word(address, value) end
    function t:half(address, value)
        ffi.cast('uint16_t*', memory + (address % 0x20000000))[0] = value
    end
    function t:bytes(address, list)
        for i, b in ipairs(list) do
            memory[address % 0x20000000 + i - 1] = b
        end
    end
    function t:index(value, owner)
        memory[(owner or CHANNEL) % 0x20000000 + 0x58] = value % 0x100
    end
    function t:slot(k, value, owner) word((owner or CHANNEL) + 4 * k, value) end
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
    assert(t:countOf('script: text_stream_owner_watch') == 1)
    assert(t:has('setup: debugger=on configured_cpu=interpreter'))
    assert(t:allDisabled())
    t:frames(WINDOW + 1)
    assert(t:countOf('==== TRACE RESULT =====') == 1, 'document printed once')
    assert(t:countOf('--- end of trace') == 1)
end

-- A damage message: the root string pushes a formatted decimal into the
-- channel's own decimal_buffer, then a staged string, and both pop again.
do
    local t = capture()
    assert(t:installed() == 9)
    assert(t:has('signatures 9/9 US words ok'))
    assert(t:has('armed. Route: from the main menu start a Free Duel'))
    for _ = 1, 3 do t:exec(CONTROL) end
    t:slot(0, 0x801c1234)
    t:slot(8, 0x800f0000)
    t:bytes(0x801c1234, {0x00, 0xb4, 0x09, 0x80, 0x04, 0x11, 0x22, 0x33})
    t:word(0x8009b400, 1200)
    t:index(0)
    t:exec(BUILD, CHANNEL, 0x80039000)
    assert(t:has('OWNER 0x800EB15C gText_aChannels[1] first seen in '
                 .. 'TextBox_BuildStep ra=0x80039000 in=resident idx=0 '
                 .. 'live=[0=801C1234(tail-banks)] sel=00 B4 09 80 04 11 22 33'))
    t:exec(BUILD, CHANNEL, 0x80039000)
    assert(t:countOf('OWNER 0x800EB15C') == 1, 'unchanged owner logs once')
    t:exec(DECIMAL, CHANNEL, 0x800393f0)
    assert(t:has('CALL Text_PushFormattedDecimal owner=0x800EB15C '
                 .. 'gText_aChannels[1] ra=0x800393F0 in=resident idx=0 '
                 .. 'value_addr=8009B400(resident-bss) format=0x04 value=1200'))
    t:exec(READ_U32, CHANNEL, 0x80038160)
    -- The push: slot 1 now points at decimal_buffer.
    t:slot(1, CHANNEL + 0x44)
    t:index(1)
    t:exec(BUILD, CHANNEL, 0x80039000)
    assert(t:has('IDX 0x800EB15C gText_aChannels[1] 0 -> 1 '
                 .. 'seen-in=TextBox_BuildStep ra=0x80039000 '
                 .. 'live=[0=801C1234(tail-banks) 1=800EB1A0(self+0x44)]'))
    t:half(0x8009b32e, 0x8123)
    t:exec(STAGED, CHANNEL, 0x800393f0)
    assert(t:has('CALL Text_PushStagedString owner=0x800EB15C '
                 .. 'gText_aChannels[1] ra=0x800393F0 in=resident idx=1 '
                 .. 'staged_id=0x8123'))
    t:slot(2, 0x801d6000)
    t:index(2)
    t:exec(READ_BYTE, CHANNEL, 0x8003b000)
    assert(t:has('IDX 0x800EB15C gText_aChannels[1] 1 -> 2 '
                 .. 'seen-in=TextStream_ReadByte'))
    t:exec(ENDSTREAM, CHANNEL, 0x800393f0)
    assert(t:has('CALL Text_EndStream owner=0x800EB15C gText_aChannels[1] '
                 .. 'ra=0x800393F0 in=resident idx=2'))
    t:index(1)
    t:exec(BUILD, CHANNEL)
    t:index(0)
    t:exec(BUILD, CHANNEL)
    t:index(-1)
    t:exec(BUILD, CHANNEL)
    assert(t:has('0 -> -1 seen-in=TextBox_BuildStep ra=0x00000000 live=[none] '
                 .. 'sel=not-RAM'))
    -- A selected card's name pushed from an overlay caller.
    t:index(0)
    t:half(0x8009b338, 722)
    t:exec(CARD, CHANNEL, 0x80168100)
    assert(t:has('CALL Text_ExpandSelectedCardInfo owner=0x800EB15C '
                 .. 'gText_aChannels[1] ra=0x80168100 in=ovl-80168000 idx=0 '
                 .. 'op=0x00 card=722'))
    t:frames(WINDOW)
    assert(t:has('status: 180-second window ended'))
    assert(t:has('verdict: stream_index ranged -1..2 over 12 accepted hits; '
                 .. 'live slots used: 0,1,2; pushes=3 pops=1 transitions=6; '
                 .. 'out-of-range stream_index=1'))
    assert(t:has('fn Text_PushFormattedDecimal 0x80038148 hits=1 accepted=1'))
    assert(t:has('owner 0x800EB15C gText_aChannels[1] hits=12 idx=-1..2 '
                 .. 'last=0 transitions=6'))
    assert(t:has('slot 01 +0x04 live=5 {self(decimal_buffer)=5} '
                 .. 'examples=800EB1A0 | above=3 {self(decimal_buffer)=3}'))
    assert(t:has('slot 08 +0x20 live=0 {-} examples=- | above=12 '
                 .. '{resident-bss=12}'))
    assert(t:has('slot 02 +0x08 live=2 {tail-banks=2} examples=801D6000 | above=4'))
    assert(not t:has('slot 03 '), 'null slots are not listed')
    assert(not t:has('callback_error'))
    finished(t)
end

-- Bad owners are refused before any read; bad slot pointers are classified
-- and never dereferenced.
do
    local t = capture()
    t:exec(CONTROL)
    t:exec(BUILD, 0)
    t:exec(BUILD, CHANNEL + 2)
    t:exec(BUILD, 0x80300000)
    t:exec(BUILD, 0x801fffd0)
    t:exec(READ_BYTE, 0x1f800000)
    t:exec(BUILD, 0x00001000 + 0xbfc00000)
    assert(t:has('REFUSED TextBox_BuildStep owner=0x00000000 (null)'))
    assert(t:has('REFUSED TextBox_BuildStep owner=0x800EB15E (unaligned)'))
    assert(t:has('REFUSED TextBox_BuildStep owner=0x80300000 (outside RAM)'))
    assert(t:has('REFUSED TextBox_BuildStep owner=0x801FFFD0 (outside RAM)'),
           'a record crossing the end of RAM')
    assert(t:has('REFUSED TextStream_ReadByte owner=0x1F800000 (outside RAM)'))
    assert(t:has('REFUSED TextBox_BuildStep owner=0xBFC01000 (outside RAM)'))
    -- Slot pointers: outside RAM, scratchpad, unaligned value address.
    t:slot(0, 0x80300000)
    t:index(0)
    t:exec(DECIMAL, CHANNEL)
    assert(t:has('idx=0 operands=unreadable live=[0=80300000(other)] '
                 .. 'sel=not-RAM'))
    t:slot(0, 0x1f800010)
    t:exec(OFFSET, CHANNEL)
    assert(t:has('offset=unreadable live=[0=1F800010(scratchpad)] sel=not-RAM'))
    t:slot(0, 0x801c0000)
    t:bytes(0x801c0000, {0x01, 0x00, 0x30, 0x80, 0x02})
    t:exec(DECIMAL, CHANNEL)
    assert(t:has('value_addr=80300001(other) format=0x02 value=unreadable'))
    t:slot(0, 0x801ffffe)
    t:exec(BUILD, CHANNEL)
    t:exec(OFFSET, CHANNEL)
    assert(t:has('offset=unreadable live=[0=801FFFFE(ram-high)] sel=not-RAM'),
           'a cursor too close to the end of RAM is not read')
    -- A stream_index far outside the 22 slots selects nothing.
    t:index(100)
    t:exec(BUILD, CHANNEL)
    assert(t:has('0 -> 100 seen-in=TextBox_BuildStep'))
    t:frames(WINDOW)
    assert(t:has('fn TextBox_BuildStep 0x800393B0 hits=7 accepted=2 '
                 .. 'refused_owner=5 refused_signature=0'))
    assert(t:has('out-of-range stream_index=1'))
    assert(not t:has('callback_error'))
    finished(t)
end

-- A signature mismatch refuses every hit on that entry.
do
    local t = capture(function(word) word(BUILD, 0x27bdffe8) end)
    assert(t:has('signatures 8/9 US words ok'))
    t:exec(CONTROL)
    t:slot(0, 0x801c0000)
    t:exec(BUILD, CHANNEL)
    t:exec(BUILD, CHANNEL)
    assert(t:countOf('REFUSED TextBox_BuildStep: entry words 27BDFFE8 '
                     .. 'AFB00010 are not the retail 27BDFFE0 AFB00010') == 1)
    assert(not t:has('OWNER'))
    t:frames(WINDOW)
    assert(t:has('refused_owner=0 refused_signature=2'))
    assert(t:has('verdict: inconclusive: US signature mismatch; every text '
                 .. 'hit was refused'))
    assert(t:has('signature TextBox_BuildStep 0x800393B0 expected 27BDFFE0 '
                 .. 'AFB00010 read 27BDFFE8 AFB00010 MISMATCH'))
    finished(t)
end

-- Bounded output: the row budget, per-function rows and transition repeats.
do
    local t = capture()
    t:exec(CONTROL)
    t:slot(0, 0x801c0000)
    for i = 1, 200 do
        t:index(i % 2)
        t:exec(BUILD, CHANNEL)
        t:exec(ENDSTREAM, CHANNEL)
    end
    assert(t:countOf('CALL Text_EndStream') == 40)
    assert(t:countOf(' 0 -> 1 seen-in=TextBox_BuildStep') == 6)
    for i = 1, 20 do
        t:exec(BUILD, 0x80100000 + 0x100 * i)
    end
    assert(t:countOf('OWNER ') == 16)
    for i = 1, 300 do
        t:exec(STAGED, 0x80120000 + 0x100 * (i % 4))
        t:exec(DECIMAL, CHANNEL)
        t:exec(OFFSET, CHANNEL)
        t:exec(CARD, CHANNEL)
    end
    -- Forty steps through distinct depths overflow the 240-row budget.
    for i = 1, 40 do
        t:index(i % 22)
        t:exec(BUILD, CHANNEL)
    end
    assert(t:countOf('row budget of 240 reached') == 1)
    t:frames(WINDOW)
    assert(t:rowLines() == 480, 'two copies of 240 rows')
    assert(t:has('owners=16/16 untracked_owner_hits=305'))
    assert(t:has('rows=240/240 dropped_rows=1226'))
    assert(t:has('transitions_suppressed=189'))
    finished(t)
end

-- Control fired but no text appeared.
do
    local t = capture()
    t:exec(CONTROL)
    t:frames(WINDOW)
    assert(t:has('verdict: inconclusive: the control fired but no text owner '
                 .. 'was accepted'))
    assert(t:has('execution_breakpoint=observed'))
    finished(t)
end

-- Text without any push names the missing route step.
do
    local t = capture()
    t:exec(CONTROL)
    t:slot(0, 0x801c0000)
    t:exec(BUILD, CHANNEL)
    t:frames(WINDOW)
    assert(t:has('pushes=0 pops=0'))
    assert(t:has('no nested string was pushed'))
    finished(t)
end

-- Nothing fired: the setup is blamed.
do
    local t = capture()
    t:frames(WINDOW - 1)
    assert(not t:has('==== TRACE RESULT ====='), 'window not over yet')
    t:frames(1)
    assert(t:has('verdict: nothing fired, not even the Main_RunFrameServices '
                 .. 'control: the setup failed'))
    assert(t:has('execution_breakpoint=NOT-observed'))
    finished(t)
end

-- A callback failure ends the trace at the next vsync.
do
    local t = capture()
    t:exec(CONTROL)
    PCSX.getRegisters = function() error('boom') end
    t:exec(BUILD, CHANNEL)
    t:frames(1)
    assert(t:has('status: callback error'))
    assert(t:has('callback_error: '))
    finished(t)
end

-- A reset ends the capture instead of leaving it silent.
do
    local t = capture()
    t:exec(CONTROL)
    t:reset()
    assert(t:has('reset observed; breakpoints are gone, ending'))
    assert(t:has('status: console reset; capture ended early'))
    assert(t:has('reset: yes'))
    finished(t)
end

print = hostPrint
PCSX = nil
print('text_stream_owner_watch_test: all scenarios passed')
