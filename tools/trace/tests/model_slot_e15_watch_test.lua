-- Run from the repository root:
--   luajit tools/trace/tests/model_slot_e15_watch_test.lua
-- Replays Vsync callbacks with a mock PCSX table and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local originalPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/model_slot_e15_watch.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE, PHASE = 0x8009b26c, 0x8009af9a
local SLOTS, SLOT_SIZE = 0x800f2c40, 0xe20
local E15, ROW, ACTIVE = 0xe15, 0xe0f, 0xe1f
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
    listener_model_slot_e15_watch = nil
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
    function r:poke(address, value) memory[address - RAM_BASE] = value end
    function r:slot(n, field, value) self:poke(SLOTS + n * SLOT_SIZE + field, value) end
    return r
end

local function finished(r, status)
    assert(r:has('status: ' .. status), status)
    assert(r:countOf('==== USER CONTEXT') == 1)
    assert(r:countOf('==== TRACE RESULT') == 1)
    assert(r:countOf('script: model_slot_e15_watch') == 1)
    r:frames(120)
    assert(r:countOf('--- end of trace') == 1,
        'later frames must not print a second document')
end

-- A battle: the start row, then one row per frame in which anything changed,
-- naming what changed. The mode is the low five bits of the state byte.
do
    local r = capture()
    assert(r:has('watching now; stopping by itself in 180 seconds'))
    r:poke(MAIN_MODE, 0x86)
    r:frames(1)
    assert(r:has('t=   0.0s mode=06 phase=00 | slot0 e15=00 row=00 active=00 '
        .. '| slot1 e15=00 row=00 active=00 | changed: start'))
    r:frames(59)
    r:poke(PHASE, 5)
    r:slot(0, ACTIVE, 1)
    r:frames(1)
    assert(r:has('t=   1.0s mode=06 phase=05 | slot0 e15=00 row=00 active=01 '
        .. '| slot1 e15=00 row=00 active=00 | changed: phase,active_0'))
    r:slot(1, E15, 0xff)
    r:slot(1, ROW, 8)
    r:frames(1)
    assert(r:has('slot1 e15=FF row=08 active=00 | changed: e15_1,row_1'))
    r:frames(10)
    r:slot(1, E15, 0)
    r:frames(1)
    assert(r:has('changed: e15_1'))
    local before = #r.output
    r:frames(30)
    assert(#r.output == before, 'unchanged frames print nothing')
    r:frames(WINDOW)
    finished(r, 'window ended')
    assert(r:has('rows: 4 dropped: 0 e15_changes: 2'))
    assert(not r:has('note: field_E15 never changed'))
end

-- Nothing changes: the document says so instead of looking like a result.
do
    local r = capture()
    r:frames(WINDOW)
    finished(r, 'window ended')
    assert(r:has('rows: 1 dropped: 0 e15_changes: 0'))
    assert(r:has('note: field_E15 never changed'))
end

-- A value that flips every frame is capped rather than flooding the console.
do
    local r = capture()
    for i = 1, 400 do
        r:slot(0, E15, i % 2)
        r:frames(1)
    end
    r:frames(WINDOW)
    finished(r, 'window ended')
    assert(r:has('rows: 200 dropped: 200'))
end

print = originalPrint
PCSX = nil
print('model_slot_e15_watch_test: ok')
