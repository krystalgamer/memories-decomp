-- Run from the repository root:
--   luajit tools/trace/tests/sound_sequence_07f8_watch_test.lua
-- Replays Vsync callbacks with a mock PCSX table and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local originalPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/sound_sequence_07f8_watch.lua'

local RAM_BASE = 0x80000000
local POINTER, STATE = 0x8009b458, 0x80120000
local F7F8, TRACKS_N, TIMEBASE, TOKEN, TICKS = 0x7f8, 0x7fa, 0x7fc, 0x7e0, 0x7f0
local TRACKS, TRACK_SIZE, ENDED = 0x518, 0x2c, 0x24
local WINDOW = 10800

local function capture()
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local events, output = {}, {}
    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        getMemPtr = function() return memory end,
        addBreakpoint = function() error('this trace installs no breakpoints') end,
        Events = {
            createEventListener = function(name, callback)
                events[name] = callback
                return {}
            end,
        },
    }
    listener_sound_sequence_07f8_watch = nil
    dofile(scriptPath)

    local r = {output = output}
    function r:countOf(text)
        local count = 0
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then count = count + 1 end
        end
        return count
    end
    function r:has(text) return self:countOf(text) > 0 end
    function r:frames(count)
        for _ = 1, count do events['GPU::Vsync']() end
    end
    function r:u32(address, value)
        ffi.cast('uint32_t*', memory + (address - RAM_BASE))[0] = value
    end
    function r:u16(address, value)
        ffi.cast('uint16_t*', memory + (address - RAM_BASE))[0] = value
    end
    function r:u8(address, value) memory[address - RAM_BASE] = value end
    return r
end

local function finished(r, status)
    assert(r:has('status: ' .. status), status)
    assert(r:countOf('==== USER CONTEXT') == 1)
    assert(r:countOf('==== TRACE RESULT') == 1)
    assert(r:countOf('script: sound_sequence_07f8_watch') == 1)
    r:frames(120)
    assert(r:countOf('--- end of trace') == 1,
        'later frames must not print a second document')
end

-- A null pointer, then one outside RAM, then a valid state: none of the bad
-- pointers is dereferenced, and each is named in its row.
do
    local r = capture()
    r:frames(1)
    assert(r:has('mode=00 state=pointer null | changed: start'))
    r:u32(POINTER, 0x1f800000)
    r:frames(1)
    assert(r:has('state=pointer 0x1F800000 outside RAM | changed: state'))
    r:u32(POINTER, STATE + 2)
    r:frames(1)
    assert(r:has('state=pointer 0x80120002 outside RAM'))
    r:u32(POINTER, 0x801ffff0)
    r:frames(1)
    assert(r:has('state=pointer 0x801FFFF0 outside RAM'),
        'a state that would run past the end of RAM is refused')
    r:frames(WINDOW)
    finished(r, 'window ended')
    assert(r:has('field_07F8 values seen: none (state pointer never valid)'))
end

-- A sequence opens with three tracks, one ends, field_07F8 changes; the tick
-- count rides along without making rows of its own.
do
    local r = capture()
    r:u32(POINTER, STATE)
    r:u16(STATE + TRACKS_N, 3)
    r:u16(STATE + TIMEBASE, 0x60)
    r:u16(STATE + TOKEN, 7)
    r:frames(1)
    assert(r:has('field_07F8=0000 tracks=0003 live=3 timebase=0060 token=0007 '
        .. 'seq_state=0000 source=00000000 ticks=0 | changed: start'))
    local before = #r.output
    for i = 1, 30 do
        r:u32(STATE + TICKS, i)
        r:frames(1)
    end
    assert(#r.output == before, 'tick count alone prints nothing')
    r:u8(STATE + TRACKS + 1 * TRACK_SIZE + ENDED, 1)
    r:frames(1)
    assert(r:has('live=2') and r:has('changed: live'))
    r:u16(STATE + F7F8, 1)
    r:frames(1)
    assert(r:has('field_07F8=0001') and r:has('ticks=30 | changed: f7f8'))
    r:frames(WINDOW)
    finished(r, 'window ended')
    assert(r:has('field_07F8 values seen: 0000, 0001'))
end

-- A corrupt track count is clamped to the sixteen tracks the state holds.
do
    local r = capture()
    r:u32(POINTER, STATE)
    r:u16(STATE + TRACKS_N, 0xffff)
    r:frames(1)
    assert(r:has('tracks=FFFF live=16'))
    r:frames(WINDOW)
    finished(r, 'window ended')
end

print = originalPrint
PCSX = nil
print('sound_sequence_07f8_watch_test: ok')
