-- Run from the repository root:
--   luajit tools/trace/tests/debug_menu_entry_labels_test.lua
-- Replays Debug Menu selector/cursor capture with mock PCSX events and real
-- FFI RAM. It does not establish any visible menu label.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/debug_menu_entry_labels.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local DEBUG_ACTION = 0x8009b2eb
local DEBUG_SELECTOR = 0x8009b2f0
local DEBUG_CURSOR = 0x8009b2f1
local PAD1_REPEAT = 0x8009b394
local PAD1_PRESSED = 0x8009b398
local SELECTOR_1_TABLE = 0x80090d7c
local SELECTOR_0_TABLE = 0x80090d84

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local listeners = {}
    local output = {}

    local function ptr8(addr)
        return ffi.cast('uint8_t*', memory + (addr - RAM_BASE))
    end

    local function ptr32(addr)
        return ffi.cast('uint32_t*', memory + (addr - RAM_BASE))
    end

    local function set8(addr, value)
        ptr8(addr)[0] = value % 0x100
    end

    local function set16(addr, value)
        set8(addr, value)
        set8(addr + 1, math.floor(value / 0x100))
    end

    local function set32(addr, value)
        ptr32(addr)[0] = value
    end

    set8(MAIN_MODE, options.mode or 0)
    set8(DEBUG_ACTION, 0)
    set8(DEBUG_SELECTOR, 0)
    set8(DEBUG_CURSOR, 0)
    set16(PAD1_REPEAT, 0)
    set16(PAD1_PRESSED, 0)
    for index = 0, 22 do
        set32(SELECTOR_1_TABLE + index * 4, 0x80030000 + index * 4)
    end
    set32(SELECTOR_1_TABLE, 0x80031078)
    set32(SELECTOR_1_TABLE + 4, 0x80031000)
    set32(SELECTOR_0_TABLE, 0x80031078)
    if options.validSignature == false then
        set32(SELECTOR_1_TABLE, 0)
    end

    print = function(...)
        local values = {}
        for index = 1, select('#', ...) do
            values[index] = tostring(select(index, ...))
        end
        output[#output + 1] = table.concat(values, '\t')
    end

    PCSX = {
        getMemPtr = function()
            return memory
        end,
        Events = {
            createEventListener = function(name, callback)
                listeners[name] = callback
                return {}
            end,
        },
    }
    listener_debug_menu_entry_labels_reset = nil
    listener_debug_menu_entry_labels = nil
    dofile(scriptPath)

    local result = {}

    function result:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then
                return true
            end
        end
        return false
    end

    function result:frames(count)
        assert(listeners['GPU::Vsync'], 'trace did not install Vsync listener')
        for _ = 1, count do
            listeners['GPU::Vsync']()
        end
    end

    function result:context(selector, cursor, repeatMask, pressedMask)
        set8(DEBUG_SELECTOR, selector)
        set8(DEBUG_CURSOR, cursor)
        set16(PAD1_REPEAT, repeatMask or 0)
        set16(PAD1_PRESSED, pressedMask or 0)
        self:frames(1)
    end

    function result:action(value)
        set8(DEBUG_ACTION, value)
    end

    function result:reset()
        assert(listeners['ExecutionFlow::Reset'])
        listeners['ExecutionFlow::Reset']()
    end

    function result:installed()
        return listeners['GPU::Vsync'] ~= nil
    end

    return result
end

local normal = capture()
for cursor = 1, 19 do
    normal:context(0, cursor, cursor == 1 and 0x4000 or 0,
                   cursor == 1 and 0x0100 or 0)
end
for cursor = 0, 19 do
    normal:context(1, cursor)
end
assert(normal:has('status: captured all 40 selector/cursor contexts'))
assert(normal:has(
    'summary: contexts=40 selector_0=20 selector_1=20'
))
assert(normal:has(
    'selector=0 cursor=00 dispatch_index=01 table=0x80090D84 '
        .. 'target=0x8003000C'
))
assert(normal:has(
    'selector=1 cursor=02 dispatch_index=03 table=0x80090D7C '
        .. 'target=0x8003000C'
))
assert(normal:has('repeat=0x4000 pressed=0x0100'))

local action = capture()
action:action(3)
action:frames(1)
assert(action:has(
    'status: debug action 3 started; rerun without Circle/Cross/Square'
))
assert(action:has('summary: contexts=1 selector_0=1 selector_1=0'))

local invalidCursor = capture()
invalidCursor:context(0, 20)
assert(invalidCursor:has('status: cursor 20 outside expected range 0-19'))

local reset = capture()
reset:context(0, 1)
reset:reset()
assert(reset:has('status: reset observed; rerun from Debug Menu'))
assert(reset:has('summary: contexts=2 selector_0=2 selector_1=0'))

local silent = capture()
silent:frames(600)
assert(silent:has(
    'no new cursor position observed; use only d-pad and Select'
))
silent:frames(35400)
assert(silent:has(
    'status: timed out after a partial Debug Menu label capture'
))

local wrongMode = capture({mode = 8})
assert(wrongMode:has('status: main mode 8 is not Debug Menu mode 0'))
assert(not wrongMode:installed())

local badSignature = capture({validSignature = false})
assert(badSignature:has(
    'status: dispatch table signature mismatch at 0x80090D7C'
))
assert(not badSignature:installed())

print = hostPrint
print('debug_menu_entry_labels: all seven polling cases passed')
