-- Run from the repository root:
--   luajit tools/trace/tests/sound_sequence_sync_track_watch_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local originalPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/sound_sequence_sync_track_watch.lua'

local RAM_BASE = 0x80000000
local CONTROL, OPEN, PLAY, FAST_FORWARD = 0x8001306c, 0x80049a64, 0x80049af4,
    0x80049bac
local HEADER, DISPATCH, PROCESS = 0x8004bce8, 0x8004c114, 0x8004c8c8
local STATE_PTR, STATE = 0x8009b458, 0x801e0000
local FIELD = STATE + 0x7f8
local STEP_SITE = 0x80045800
local HEADER_STORE = 0x8004bd40
local SEQ_DATA = 0x801a0000

local WORDS = {
    [CONTROL] = 0x27bdffe0, [OPEN] = 0x00803021, [PLAY] = 0x3c02800a,
    [FAST_FORWARD] = 0x3c02800a, [HEADER] = 0x3c03800a,
    [DISPATCH] = 0x27bdffe8, [PROCESS] = 0x3c02800a,
    -- sh $v1, 0x7f8($v0)
    [HEADER_STORE] = 0xa44307f8,
}

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local gpr = {n = {}, r = {}}
    local registers = {pc = 0, GPR = gpr}
    local events, breakpoints, output = {}, {}, {}
    local function word(address, value)
        ffi.cast('uint32_t*', memory + (address - RAM_BASE))[0] = value
    end
    local function half(address, value)
        ffi.cast('uint16_t*', memory + (address - RAM_BASE))[0] = value
    end
    for address, value in pairs(WORDS) do word(address, value) end
    if options.badControl then word(CONTROL, 0) end
    ffi.copy(memory + (SEQ_DATA - RAM_BASE), 'pQES', 4)
    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        settings = {emulator = {Debug = {Debug = true}, Dynarec = false}},
        getMemPtr = function() return memory end,
        getRegisters = function()
            if options.brokenRegisters then return nil end
            return registers
        end,
        addBreakpoint = function(address, kind, size, label, callback)
            assert(breakpoints[address] == nil)
            if address == FIELD then
                assert(kind == 'Write' and size == 2)
            else
                assert(kind == 'Exec' and size == 4)
            end
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
    breakpoints_sound_sequence_sync_track_watch = nil
    listeners_sound_sequence_sync_track_watch = nil
    dofile(scriptPath)

    local r = {output = output}
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
    function r:pointer(value) word(STATE_PTR, value) end
    function r:field(value) half(FIELD, value) end
    function r:tracks(value) half(STATE + 0x7fa, value) end
    function r:timebase(value) half(STATE + 0x7fc, value) end
    function r:call(address, values, force)
        for _, name in ipairs({'a0', 'a1', 'a2', 'a3', 'ra'}) do
            gpr.n[name] = values[name] or 0
        end
        local bp = assert(breakpoints[address])
        if bp.enabled or force then bp.callback() end
    end
    function r:store(pc, value, force)
        registers.pc = pc
        gpr.r[2] = STATE
        gpr.r[3] = value
        local bp = assert(breakpoints[FIELD], 'field watch installed')
        if bp.enabled or force then bp.callback() end
    end
    function r:track(index) return STATE + 0x518 + index * 0x2c end
    function r:loopCount(index, value)
        memory[STATE + 0x518 + index * 0x2c + 0x26 - RAM_BASE] = value
    end
    function r:mode(channel, value)
        memory[STATE + channel * 0x18 + 0x12 - RAM_BASE] = value
    end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do
            if bp.enabled then return false end
        end
        return true
    end
    function r:watching() return breakpoints[FIELD] ~= nil end
    local count = 0
    for _ in pairs(breakpoints) do count = count + 1 end
    assert(count == 7, 'seven exec breakpoints before the state exists')
    return r
end

local function stopped(r, status)
    assert(r:has('status: ' .. status), status)
    assert(r:countOf('script: sound_sequence_sync_track_watch') == 1)
    assert(r:has('==== USER CONTEXT'))
    assert(r:has('setup: '))
    assert(r:allDisabled())
    r:frames(60)
    r:reset()
    r:call(CONTROL, {}, true)
    r:call(HEADER, {}, true)
    assert(r:countOf('==== TRACE RESULT') == 1)
    assert(r:countOf('--- end of trace') == 1,
        'termination emits one final document, even after late callbacks')
end

-- The watch arrives with the state; a sequence opens, loops per track, and
-- a later nonzero value switches loops to the all-tracks branch.
do
    local r = capture()
    r:call(CONTROL, {})
    r:frames(2)
    assert(not r:watching(), 'no watch while the pointer is null')
    r:pointer(STATE)
    r:frames(1)
    assert(r:watching())
    assert(r:has('#001 t=   0.1s Q00 WATCH field_07F8 at 0x801E07F8 value=0'))
    r:call(OPEN, {a0 = SEQ_DATA, a1 = 3, ra = STEP_SITE})
    assert(r:has('#002 t=   0.1s Q00 OPEN source=0x801A0000 tag=pQES vab=3 '
        .. 'caller=SD_StepActiveCommand(0x80045800) mode=0'))
    r:call(PLAY, {a0 = 0x101, ra = STEP_SITE + 8})
    assert(r:has('#003 t=   0.1s Q00 PLAY SD_PlaySequence arg=0x101 '
        .. 'caller=SD_StepActiveCommand(0x80045808)'))
    r:call(HEADER, {ra = 0x8004c7a0})
    assert(r:has('#004 t=   0.1s Q01 HEADER source=0x00000000 '
        .. 'caller=SD_StartSequenceTracks(0x8004C7A0)'))
    r:store(HEADER_STORE, 0)
    assert(r:has('#005 t=   0.1s Q01 STORE field_07F8 0 -> 0 width=2 by '
        .. 'SD_ReadSequenceHeader pc=0x8004BD40'))
    r:tracks(3)
    r:timebase(480)
    r:frames(1)
    assert(r:has('#006 t=   0.1s Q01 SEQ from #004 tracks=3 timebase=480 '
        .. 'field_07F8=0 state=0'))
    for _ = 1, 3 do
        r:call(DISPATCH, {a0 = r:track(1), a1 = 0xb2, a2 = 0x63, a3 = 0x14})
    end
    r:call(DISPATCH, {a0 = r:track(1), a1 = 0x92, a2 = 0x63, a3 = 0x14})
    r:call(DISPATCH, {a0 = r:track(1), a1 = 0xb2, a2 = 0x07, a3 = 0x64})
    assert(r:countOf('LOOP start') == 1, 'repeats and non-loop events hidden')
    assert(r:has('#007 t=   0.1s Q01 LOOP start track=1 ch=2 value=20 '
        .. 'field_07F8=0 loop_count=0 mode=0x00 branch=own-track ticks=0'))
    -- Controller 6 outside loop mode is an ordinary controller.
    r:call(DISPATCH, {a0 = r:track(1), a1 = 0xb2, a2 = 0x06, a3 = 0x40})
    assert(r:has('Q01 LOOP data_entry track=1 ch=2 value=64 field_07F8=0 '
        .. 'loop_count=0 mode=0x00 branch=not-taken:mode=0x00'))
    r:mode(2, 0x14)
    r:call(DISPATCH, {a0 = r:track(1), a1 = 0xb2, a2 = 0x06, a3 = 0x7f})
    assert(r:has('Q01 LOOP data_entry track=1 ch=2 value=127 field_07F8=0 '
        .. 'loop_count=0 mode=0x14 branch=own-track'))
    -- A loop end with loop_count 0 restores nothing.
    r:call(DISPATCH, {a0 = r:track(1), a1 = 0xb2, a2 = 0x63, a3 = 0x1e})
    assert(r:has('Q01 LOOP end track=1 ch=2 value=30 field_07F8=0 '
        .. 'loop_count=0 mode=0x14 branch=not-taken:loop_count=0'))
    r:loopCount(1, 0x7f)
    r:call(DISPATCH, {a0 = r:track(1), a1 = 0xb2, a2 = 0x63, a3 = 0x1e})
    assert(r:has('Q01 LOOP end track=1 ch=2 value=30 field_07F8=0 '
        .. 'loop_count=127 mode=0x14 branch=own-track'))
    r:store(0x80045a00, 0x10002)
    assert(r:has('Q01 STORE undecoded by SD_StepActiveCommand '
        .. 'pc=0x80045A00'), 'a non-store word is reported, not decoded')
    r:field(2)
    r:frames(1)
    assert(r:has('Q01 FIELD field_07F8 0 -> 2'))
    r:loopCount(0, 2)
    r:call(DISPATCH, {a0 = r:track(0), a1 = 0xb0, a2 = 0x63, a3 = 0x1e})
    assert(r:has('Q01 LOOP end track=0 ch=0 value=30 field_07F8=2 '
        .. 'loop_count=2 mode=0x00 branch=all-tracks'))
    r:call(DISPATCH, {a0 = r:track(0), a1 = 0xb0, a2 = 0x06, a3 = 0x05})
    assert(r:has('Q01 LOOP data_entry track=0 ch=0 value=5 field_07F8=2 '
        .. 'loop_count=2 mode=0x00 branch=not-taken:mode=0x00'),
        'a nonzero field does not make an ordinary controller a loop')
    r:call(DISPATCH, {a0 = STATE + 0x100, a1 = 0xb0, a2 = 0x63, a3 = 0x1e})
    assert(r:has('Q01 LOOP end track=? ch=0'), 'pointer outside track array')
    r:call(DISPATCH, {a0 = 0x00000010, a1 = 0xb0, a2 = 0x63, a3 = 0x1e})
    assert(r:has('loop_count=unreadable mode=0x00 branch=unknown'))
    r:tracks(4)
    r:frames(1)
    assert(r:has('Q01 TRACKS track_count 3 -> 4'))
    r:call(PROCESS, {})
    r:reset()
    stopped(r, 'reset; reload the script and start again')
    assert(r:has('frames: 6 control_hits: 1 headers: 1 process_calls: 1 '
        .. 'field_stores: 2 nonzero_stores: 0 undecoded_stores: 1 '
        .. 'nonzero_polls: 2 dropped_rows: 0'))
    assert(r:has('state: 0x801E0000 field_07F8=2 track_count=4 '
        .. 'watch=0x801E07F8'))
    assert(r:has('loop Q01 start track=1 ch=2 value=20 branch=own-track '
        .. 'count=3'))
    assert(r:has('loop Q01 end track=0 ch=0 value=30 branch=all-tracks '
        .. 'count=1'))
    assert(r:has('loop Q01 end track=1 ch=2 value=30 '
        .. 'branch=not-taken:loop_count=0 count=1'))
    print = originalPrint
end

-- A decoded nonzero store is counted and shows the merged value.
do
    local r = capture()
    r:pointer(STATE)
    r:frames(1)
    r:store(HEADER_STORE, 0x10005)
    assert(r:has('STORE field_07F8 0 -> 5 width=2 by SD_ReadSequenceHeader'))
    r:reset()
    assert(r:has('field_stores: 1 nonzero_stores: 1 undecoded_stores: 0'))
    print = originalPrint
end

-- Misaligned or out-of-range state pointers never install the watch.
do
    local r = capture()
    r:call(CONTROL, {})
    r:pointer(0x801e0002)
    r:frames(2)
    r:pointer(0x801fff00)
    r:frames(2)
    assert(not r:watching())
    r:reset()
    assert(r:has('state: 0xunreadable field_07F8=unreadable '
        .. 'track_count=unreadable watch=not installed'))
    print = originalPrint
end

-- Eight sequence headers end the trace once the last one is described.
do
    local r = capture()
    r:call(CONTROL, {})
    r:pointer(STATE)
    r:frames(1)
    for _ = 1, 8 do
        r:call(HEADER, {ra = 0x8004c7a0})
        r:frames(1)
    end
    stopped(r, 'captured 8 sequences')
    assert(r:has('Q08 SEQ from #'))
    print = originalPrint
end

do
    local r = capture()
    r:call(CONTROL, {})
    r:pointer(STATE)
    r:frames(1)
    r:call(HEADER, {})
    r:frames(300 * 60 - 1)
    assert(not r:has('==== TRACE RESULT'))
    r:frames(1)
    stopped(r, 'five-minute window after the first sequence header ended')
    print = originalPrint
end

do
    local r = capture()
    r:frames(300 * 60)
    stopped(r, 'five minutes without a control hit')
    assert(r:has('execution_breakpoint=NOT-observed'))
    print = originalPrint
end

do
    local r = capture()
    r:call(CONTROL, {})
    r:frames(720 * 60)
    stopped(r, 'timed out')
    assert(r:has('execution_breakpoint=observed'))
    print = originalPrint
end

do
    local r = capture({brokenRegisters = true})
    r:call(OPEN, {a0 = SEQ_DATA})
    r:frames(1)
    stopped(r, 'callback error')
    assert(r:has('callback_error: '))
    print = originalPrint
end

do
    local r = capture({badControl = true})
    r:reset()
    assert(r:has('signature_mismatch: control Main_RunFrameServices '
        .. '0x8001306C expected 0x27BDFFE0 read 0x00000000'))
    assert(r:has('1 signature mismatches'))
    print = originalPrint
end

print = originalPrint
print('sound_sequence_sync_track_watch: all cases passed')
