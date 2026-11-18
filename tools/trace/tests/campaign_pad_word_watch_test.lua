-- Run from the repository root:
--   luajit tools/trace/tests/campaign_pad_word_watch_test.lua
-- Replays Vsync callbacks with a mock PCSX table and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local originalPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/campaign_pad_word_watch.lua'

local RAM_BASE = 0x80000000
local REPEAT, PRESSED, HELD = 0x8009b394, 0x8009b398, 0x8009b3a4
local WORD, NEIGHBOUR = 0x800c4e68, 0x800c4e74
local WINDOW = 7200

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
    listener_campaign_pad_word_watch = nil
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
    function r:u16(address, value)
        ffi.cast('uint16_t*', memory + (address - RAM_BASE))[0] = value
    end
    return r
end

local function finished(r, status)
    assert(r:has('status: ' .. status), status)
    assert(r:countOf('==== USER CONTEXT') == 1)
    assert(r:countOf('==== TRACE RESULT') == 1)
    assert(r:countOf('script: campaign_pad_word_watch') == 1)
    r:frames(120)
    assert(r:countOf('--- end of trace') == 1,
        'later frames must not print a second document')
end

-- Cross is pressed for one frame and held for three; the word follows the
-- pressed edge, then once matches only the repeat word, then none of them.
do
    local r = capture()
    assert(r:has('watching now; stopping by itself in 120 seconds'))
    r:frames(1)
    assert(r:has('D_800C4E68=0000 pressed=0000 held=0000 repeat=0000 '
        .. 'D_800C4E74=0000 | changed: start'))
    r:u16(PRESSED, 0x40)
    r:u16(HELD, 0x40)
    r:u16(WORD, 0x40)
    r:frames(1)
    assert(r:has('D_800C4E68=0040 pressed=0040 held=0040 repeat=0000 '
        .. 'D_800C4E74=0000 | changed: word,pressed,held'))
    r:u16(PRESSED, 0)
    r:u16(WORD, 0)
    r:frames(2)
    r:u16(REPEAT, 0x1000)
    r:u16(WORD, 0x1000)
    r:frames(1)
    r:u16(WORD, 0x20)
    r:u16(NEIGHBOUR, 3)
    r:frames(1)
    assert(r:has('changed: word,neighbour'))
    r:u16(WORD, 0)
    r:frames(WINDOW)
    finished(r, 'window ended')
    assert(r:has('nonzero frames: 3 equal to pressed: 1 held: 1 repeat: 1 '
        .. 'none of them: 1'))
end

-- Exact counts over a controlled run.
do
    local r = capture()
    r:u16(WORD, 0x40)
    r:u16(PRESSED, 0x40)
    r:frames(1)
    r:u16(PRESSED, 0)
    r:u16(HELD, 0x40)
    r:frames(2)
    r:u16(HELD, 0)
    r:u16(REPEAT, 0x40)
    r:frames(1)
    r:u16(REPEAT, 0)
    r:frames(1)
    r:u16(WORD, 0)
    r:frames(WINDOW)
    finished(r, 'window ended')
    assert(r:has('nonzero frames: 5 equal to pressed: 1 held: 2 repeat: 1 '
        .. 'none of them: 1'))
    assert(not r:has('note: D_800C4E68 stayed zero'))
end

-- The word never moves: the document says so.
do
    local r = capture()
    r:frames(WINDOW)
    finished(r, 'window ended')
    assert(r:has('nonzero frames: 0'))
    assert(r:has('note: D_800C4E68 stayed zero'))
end

-- A word that changes every frame is capped.
do
    local r = capture()
    for i = 1, 300 do
        r:u16(WORD, i)
        r:frames(1)
    end
    r:frames(WINDOW)
    finished(r, 'window ended')
    assert(r:has('rows: 200 dropped: 100'))
end

print = originalPrint
PCSX = nil
print('campaign_pad_word_watch_test: ok')
