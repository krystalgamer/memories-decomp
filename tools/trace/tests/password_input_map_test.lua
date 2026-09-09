-- Run from the repository root:
--   luajit tools/trace/tests/password_input_map_test.lua
-- Replays Password input samples with mock PCSX events and real FFI RAM.
local ffi = require('ffi')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/password_input_map.lua'

local RAM_BASE = 0x80000000
local PASSWORD_UPDATE = 0x8016a37c
local PASSWORD_UPDATE_FIRST_WORD = 0x27bdffe0
local MAIN_MODE = 0x8009b26c
local PAD1_REPEAT = 0x8009b394
local PAD1_PRESSED = 0x8009b398
local PAD1_HELD = 0x8009b3a4
local PASSWORD_DIGITS = 0x8016d410
local PASSWORD_STATE = 0x8016d424
local PASSWORD_DIGIT_INDEX = 0x8016d428

local function capture(signature, mode)
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local events = {}
    local output = {}

    local function byte(addr)
        return ffi.cast('uint8_t*', memory + (addr - RAM_BASE))
    end

    local function set8(addr, value)
        byte(addr)[0] = value % 0x100
    end

    local function set16(addr, value)
        set8(addr, value)
        set8(addr + 1, math.floor(value / 0x100))
    end

    local function set32(addr, value)
        set16(addr, value)
        set16(addr + 2, math.floor(value / 0x10000))
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
                events[name] = callback
                return {}
            end,
        },
    }
    listener_password_input_map_reset = nil
    listener_password_input_map = nil
    set32(PASSWORD_UPDATE, signature or PASSWORD_UPDATE_FIRST_WORD)
    set8(MAIN_MODE, mode or 10)
    set16(PASSWORD_STATE, 0)
    set32(PASSWORD_DIGIT_INDEX, 0)
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

    function result:mode(value)
        set8(MAIN_MODE, value)
    end

    function result:index(value)
        set32(PASSWORD_DIGIT_INDEX, value)
    end

    function result:digit(index, value)
        set8(PASSWORD_DIGITS + index, value)
    end

    function result:state(value)
        set16(PASSWORD_STATE, value)
    end

    function result:input(held, repeatMask, pressed)
        set16(PAD1_HELD, held or 0)
        set16(PAD1_REPEAT, repeatMask or 0)
        set16(PAD1_PRESSED, pressed or 0)
    end

    function result:frames(count)
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:sample(held, repeatMask, pressed, mutate)
        self:input(held, repeatMask, pressed)
        self:frames(1)
        if mutate ~= nil then
            mutate(self)
        end
        self:input(0, 0, 0)
        self:frames(2)
    end

    function result:reset()
        events['ExecutionFlow::Reset']()
    end

    return result
end

local normal = capture()
normal:sample(0x2000, 0, 0, function(r) r:index(1) end)
normal:sample(0x8000, 0, 0, function(r) r:index(0) end)
normal:sample(0, 0x1000, 0, function(r) r:digit(0, 1) end)
normal:sample(0, 0x4000, 0, function(r) r:digit(0, 0) end)
normal:sample(0, 0, 0x10)
normal:sample(0, 0, 0x800)
normal:sample(0, 0, 0x40)
normal:input(0, 0, 0x20)
normal:frames(1)
normal:mode(8)
normal:input(0, 0, 0)
normal:frames(1)
assert(normal:has('status: captured input that left the Password screen'))
assert(normal:has('samples=8'))
assert(normal:has('held=0x2000'))
assert(normal:has('repeat=0x1000'))
assert(normal:has('pressed=0x0010'))
assert(normal:has('pressed=0x0800'))
assert(normal:has('pressed=0x0040'))
assert(normal:has('pressed=0x0020'))
assert(normal:has('sample_end=08'))
assert(normal:has('mode_changed=true'))

local badSignature = capture(0x12345678, 10)
assert(badSignature:has('Password overlay signature mismatch'))

local badMode = capture(PASSWORD_UPDATE_FIRST_WORD, 8)
assert(badMode:has('main mode 8 is not Password mode 10'))

local quiet = capture()
for _ = 1, 8 do
    quiet:sample(0, 0, 0x10)
end
quiet:frames(118)
assert(not quiet:has('status: captured eight inputs followed by quiet time'))
quiet:frames(1)
assert(quiet:has('status: captured eight inputs followed by quiet time'))

local latched = capture()
latched:input(0x2000, 0, 0)
latched:frames(1)
latched:input(0x8000, 0, 0)
latched:frames(3)
latched:input(0, 0, 0)
latched:frames(1)
latched:reset()
assert(latched:has('samples=1'))

local reset = capture()
reset:reset()
assert(reset:has('status: reset observed; rerun after entering Password'))

local silent = capture()
silent:frames(36000)
assert(silent:has('status: timed out after a partial Password input trace'))

print = hostPrint
print('password_input_map: all seven polling cases passed')
