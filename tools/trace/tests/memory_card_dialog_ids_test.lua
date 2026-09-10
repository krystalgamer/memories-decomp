-- Run from the repository root:
--   luajit tools/trace/tests/memory_card_dialog_ids_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
--
-- This verifies collection and bounded diagnostic behavior. It does not map
-- any numeric ID to a visible memory-card message.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/memory_card_dialog_ids.lua'

local RAM_BASE = 0x80000000
local TEXT_LOOKUP = 0x8003b744
local TEXT_LOOKUP_ENTRY_WORD = 0x00a03021
local MAIN_MODE = 0x8009b26c
local DIALOG_RESULT_ID = 0x8009b3c6
local DIALOG_OPERATION = 0x8009b3de
local DIALOG_SLOT = 0x8009b3ee
local DIALOG_STATUS = 0x8009b3ef
local DIALOG_FLAGS = 0x8009b3fa
local IO_RESULT = 0x8009b450

local function capture(validSignature)
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a1 = 0, ra = 0 } } }
    local listeners = {}
    local breakpoints = {}
    local output = {}

    local function u8at(addr)
        return ffi.cast('uint8_t*', memory + (addr - RAM_BASE))
    end

    local function u16at(addr)
        return ffi.cast('uint16_t*', memory + (addr - RAM_BASE))
    end

    local function u32at(addr)
        return ffi.cast('uint32_t*', memory + (addr - RAM_BASE))
    end

    if validSignature ~= false then
        u32at(TEXT_LOOKUP)[0] = TEXT_LOOKUP_ENTRY_WORD
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
        getRegisters = function()
            return registers
        end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = { enabled = true, callback = callback }
            function handle:disable()
                self.enabled = false
            end
            breakpoints[address] = handle
            return handle
        end,
        Events = {
            createEventListener = function(name, callback)
                listeners[name] = callback
                return {}
            end,
        },
    }
    breakpoint_memory_card_dialog_ids = nil
    listener_memory_card_dialog_ids_reset = nil
    listener_memory_card_dialog_ids = nil
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

    function result:context(mode, flags, operation, resultID, slot, status, io)
        u8at(MAIN_MODE)[0] = mode
        u16at(DIALOG_FLAGS)[0] = flags
        u8at(DIALOG_OPERATION)[0] = operation
        u8at(DIALOG_RESULT_ID)[0] = resultID
        u8at(DIALOG_SLOT)[0] = slot
        u8at(DIALOG_STATUS)[0] = status
        u32at(IO_RESULT)[0] = io % 0x100000000
    end

    function result:lookup(id, callsite)
        registers.GPR.n.a1 = id
        registers.GPR.n.ra = callsite + 8
        local breakpoint = breakpoints[TEXT_LOOKUP]
        assert(breakpoint and breakpoint.enabled)
        breakpoint.callback()
    end

    function result:frames(count)
        for _ = 1, count do
            listeners['GPU::Vsync']()
        end
    end

    function result:breakRegisters()
        PCSX.getRegisters = function()
            error('mock register failure')
        end
    end

    function result:reset()
        listeners['ExecutionFlow::Reset']()
    end

    function result:installed()
        return breakpoints[TEXT_LOOKUP] ~= nil
    end

    function result:disabled()
        return not breakpoints[TEXT_LOOKUP].enabled
    end

    return result
end

local normal = capture(true)
normal:context(0x28, 0x1234, 2, 0x44, 1, 3, -5)
normal:lookup(0x0b9, 0x80011110)
normal:lookup(0x0e1, 0x80011110)
for id = 0x0ba, 0x0bf do
    normal:lookup(id, 0x80022220)
end
normal:lookup(0x0ba, 0x80022220)
normal:frames(300)
assert(normal:has('status: captured six distinct memory-card dialogue IDs'),
       'six IDs must finish after the 300-frame quiet window')
assert(normal:has(
    'summary: hits=6 unique_ids=6 '
        .. 'observed_ids=0x0BA,0x0BB,0x0BC,0x0BD,0x0BE,0x0BF'
), 'the summary must sort and deduplicate IDs')
assert(normal:has(
    'mode_raw=0x28 mode=08 flags=0x1234 operation=2 '
        .. 'result_id=0x44 slot=1 status=3 io_result=-5'
), 'each row must preserve the signed and packed dialogue context')
assert(not normal:has('id=0x0B9'), 'IDs below the union range are ignored')
assert(not normal:has('id=0x0E1'), 'IDs above the union range are ignored')
assert(normal:disabled())

local limited = capture(true)
limited:context(3, 0, 0, 0, 0, 0, 0)
for index = 1, 64 do
    limited:lookup(0x0ba, 0x80030000 + index * 4)
end
limited:lookup(0x0ba, 0x80040000)
limited:frames(1)
assert(limited:has(
    'status: maximum context count reached; partial trace follows'
), 'the context capture must stop at 64 accepted rows')
assert(limited:has(
    'summary: hits=64 unique_ids=1 observed_ids=0x0BA'
), 'the hit bound must not inflate the unique-ID count')
assert(limited:disabled())

local silent = capture(true)
silent:frames(600)
assert(silent:has(
    'no matching lookups observed; confirm interpreter CPU'
), 'a no-hit run must print the likely setup diagnosis')
silent:frames(71400)
assert(silent:has(
    'status: timed out without a matching lookup; use interpreter CPU'
), 'a silent run must finish with an explicit no-hit status')
assert(silent:disabled())

local broken = capture(true)
broken:breakRegisters()
broken:lookup(0x0ba, 0x80055550)
broken:frames(1)
assert(broken:has('status: breakpoint callback error:'),
       'breakpoint failures must be surfaced on the next frame')
assert(broken:disabled())

local reset = capture(true)
reset:context(3, 0, 1, 2, 0, 4, 0)
reset:lookup(0x0bc, 0x80066660)
reset:reset()
assert(reset:has('status: reset observed; rerun the script after reset'),
       'a reset must end the current capture explicitly')
assert(reset:has(
    'summary: hits=1 unique_ids=1 observed_ids=0x0BC'
), 'a reset must retain the partial capture')

local mismatch = capture(false)
assert(mismatch:has(
    'Text_LookupString signature mismatch at 0x8003B744: 0x00000000'
), 'a wrong entry instruction must reject the trace')
assert(not mismatch:installed(),
       'signature rejection must happen before breakpoint installation')

print = hostPrint
print('memory_card_dialog_ids: all six callback-replay cases passed')
