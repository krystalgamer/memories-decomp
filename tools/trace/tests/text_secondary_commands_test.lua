-- Run from the repository root:
--   luajit tools/trace/tests/text_secondary_commands_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/text_secondary_commands.lua'

local RAM_BASE = 0x80000000
local DISPATCH = 0x80038b4c
local TABLE = 0x80090eac
local MAIN_MODE = 0x8009b26c

-- The retail table, as dumped from SLUS_014.11.
local RETAIL = {
    0x80037da4, 0x800380d4, 0x80038110, 0x80038148, 0x800382a8, 0x80038334,
    0x80038388, 0x800383b0, 0x80038070, 0x800383dc, 0x80038498, 0x80038094,
    0x800384e4, 0x80038530, 0x80038690, 0x800386b8, 0x80038798, 0x80038800,
    0x80038888, 0x80038890, 0x80038898, 0x800388d8, 0x800389c4, 0x800389d8,
    0x80038a44, 0x80038ab0, 0x80038b08, 0x80037d2c, 0x80037d2c, 0x80037d2c,
    0x80037d2c, 0x80037d2c, 0x80037d2c, 0x80038eb0, 0x80037d6c, 0x80038b4c,
    0x80038d2c, 0x80038d14, 0x80038bf0, 0x80038db8, 0x80038ba8, 0x80038e1c,
    0x80038e7c, 0x80039be0, 0x80039afc, 0x80039be0, 0x80039c94,
}
local UNNAMED = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x08, 0x09, 0x0b, 0x0c,
                 0x12, 0x13, 0x14, 0x16, 0x17, 0x18, 0x2b, 0x2c, 0x2d, 0x2e}

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a0 = 0, ra = 0 } } }
    local events, breakpoints, output = {}, {}, {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u16at(a) return ffi.cast('uint16_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end
    for i, word in ipairs(RETAIL) do u32at(TABLE + (i - 1) * 4)[0] = word end
    u32at(DISPATCH)[0] = 0x27bdffe8
    if options.patchTable then u32at(TABLE + 4 * 0x12)[0] = 0 end

    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = { enabled = true, callback = callback }
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
    breakpoint_text_secondary_commands = nil
    listener_text_secondary_commands = nil
    dofile(scriptPath)

    local r = {}
    function r:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function r:frames(n) for _ = 1, n do events['GPU::Vsync']() end end
    -- Point the text object's current stream at `bytes` and dispatch.
    function r:dispatch(object, stream, bytes, stringId, caller)
        local buffer = 0x80100000
        for i, b in ipairs(bytes) do u8at(buffer + i - 1)[0] = b end
        u8at(object + 0x58)[0] = stream % 256
        u32at(object + (stream % 256 < 0x80 and stream or 0) * 4)[0] = buffer
        u16at(object + 0x36)[0] = stringId or 0
        registers.GPR.n.a0 = object
        registers.GPR.n.ra = caller or 0x800393f0
        if breakpoints[DISPATCH].enabled then breakpoints[DISPATCH].callback() end
    end
    -- Dispatch with an object whose stream slot 0 holds `cursor` verbatim.
    function r:dispatchCursor(object, cursor)
        u8at(object + 0x58)[0] = 0
        u32at(object)[0] = cursor
        registers.GPR.n.a0 = object
        if breakpoints[DISPATCH].enabled then breakpoints[DISPATCH].callback() end
    end
    function r:disabled() return not breakpoints[DISPATCH].enabled end
    u8at(MAIN_MODE)[0] = 0x05
    return r
end

-- Normal: an unnamed and a named opcode in one conversation.
local normal = capture()
assert(normal:has('breakpoint installed; table check passed'))
normal:dispatch(0x800eb100, 1, {0x16, 0x07, 0x00, 0xff, 0xfe, 0xfd}, 0x501)
normal:dispatch(0x800eb100, 1, {0x10, 0x03, 0x80, 0x87, 0x00, 0x00}, 0x501)
normal:frames(18001)
assert(normal:has('status: five quiet minutes without a new row'))
assert(normal:has('op=0x16 UNNAMED handler=func_800389C4 next=07 00 FF FE FD '
                  .. 'string=0x501'))
assert(normal:has('op=0x10 named handler=DuelEffect_PlaySoundCommand'))
assert(normal:has('mode=0x05'))
assert(normal:has('unnamed_seen: 1/20'))
assert(normal:has('unnamed_not_seen: 0x00 0x01'))
assert(normal:disabled())

-- Row limits: one row per named opcode, four per unnamed one, every call
-- counted.
local limits = capture()
for _ = 1, 9 do
    limits:dispatch(0x800eb100, 0, {0x10}, 1)
    limits:dispatch(0x800eb100, 0, {0x2c}, 1)
end
limits:frames(18001)
assert(limits:has('row=05'), 'one named plus four unnamed rows')
assert(not limits:has('row=06'), 'no row beyond the limits')
assert(limits:has('count op=0x2C func_80039AFC'))

-- Every unnamed opcode seen ends the capture early.
local all = capture()
for _, op in ipairs(UNNAMED) do all:dispatch(0x800eb100, 0, {op}, 2) end
all:frames(1)
assert(all:has('status: every unnamed opcode seen'))
assert(all:has('unnamed_not_seen: none'))

-- Guards: a bad object, a bad stream index and a cursor outside RAM.
local bad = capture()
bad:dispatch(0x800eb102, 0, {0x00}, 0)
bad:dispatch(0x800eb200, -1, {0x00}, 0)
bad:dispatch(0x800eb300, 30, {0x00}, 0)
bad:dispatchCursor(0x800eb400, 0x00001000)
bad:frames(18001)
assert(bad:has('IMPLAUSIBLE object=0x800EB102'))
assert(bad:has('IMPLAUSIBLE object=0x800EB200 stream=-1'))
assert(bad:has('IMPLAUSIBLE object=0x800EB300 stream=30'))
assert(bad:has('IMPLAUSIBLE object=0x800EB400 cursor=0x00001000'))

-- A patched table entry is reported at install time.
local patched = capture({ patchTable = true })
assert(patched:has('table check FAILED (1)'))
patched:frames(36000)
assert(patched:has('table_mismatch: op=0x12 expected 0x80038888 read 0x00000000'))

-- Bounded output.
local flood = capture()
-- All of this lands before one vsync: 22 named rows plus 4 x 11 unnamed
-- rows would be 66, so the cap must hold inside the callback.
for _, op in ipairs({0x06, 0x07, 0x0a, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
                     0x15, 0x19, 0x1a, 0x1b, 0x21, 0x22, 0x23, 0x24, 0x25,
                     0x26, 0x27, 0x28, 0x29, 0x2a}) do
    flood:dispatch(0x800eb100, 0, {op}, 3)
end
for _ = 1, 4 do
    for i = 1, 11 do flood:dispatch(0x800eb100, 0, {UNNAMED[i]}, 3) end
end
flood:frames(1)
assert(flood:has('status: captured 64 rows'))
assert(flood:has('rows: 64 implausible: 0'), 'summary must report the cap')
assert(flood:has('row=64 '))
assert(not flood:has('row=65 '), 'no row beyond the cap')

-- IMPLAUSIBLE rows obey the same cap and are still counted.
local junk = capture()
for _ = 1, 70 do junk:dispatch(0x800eb102, 0, {0x00}, 0) end
junk:frames(1)
assert(junk:has('status: captured 64 rows'))
assert(junk:has('rows: 64 implausible: 70'))

-- Silence names the likely setup mistake.
local silent = capture()
silent:frames(36000)
assert(silent:has('status: no F8 command in ten minutes'))

print = hostPrint
print('text_secondary_commands: all eight callback-replay cases passed')
