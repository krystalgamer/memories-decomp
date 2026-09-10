-- Run from the repository root:
--   luajit tools/trace/tests/campaign_opening_scene_ids_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
--
-- This verifies collection and bounded diagnostic behavior. It does not map
-- any numeric ID to visible campaign-opening text.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/campaign_opening_scene_ids.lua'

local RAM_BASE = 0x80000000
local TEXT_LOOKUP = 0x8003b744
local TEXT_LOOKUP_ENTRY_WORD = 0x00a03021
local MAIN_MODE = 0x8009b26c

local function capture(validSignature, mode)
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a1 = 0, ra = 0 } } }
    local listeners = {}
    local breakpoints = {}
    local output = {}

    local function u8at(addr)
        return ffi.cast('uint8_t*', memory + (addr - RAM_BASE))
    end

    local function u32at(addr)
        return ffi.cast('uint32_t*', memory + (addr - RAM_BASE))
    end

    if validSignature ~= false then
        u32at(TEXT_LOOKUP)[0] = TEXT_LOOKUP_ENTRY_WORD
    end
    u8at(MAIN_MODE)[0] = mode or 9

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
    breakpoint_campaign_opening_scene_ids = nil
    listener_campaign_opening_scene_ids_reset = nil
    listener_campaign_opening_scene_ids = nil
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

local normal = capture(true, 0x29)
normal:lookup(0x4ff, 0x80011110)
normal:lookup(0x507, 0x80011110)
for id = 0x500, 0x503 do
    normal:lookup(id, 0x80022220)
end
normal:lookup(0x500, 0x80022220)
normal:frames(300)
assert(normal:has('status: captured four distinct opening-scene IDs'),
       'four IDs must finish after the 300-frame quiet window')
assert(normal:has(
    'summary: hits=4 unique_ids=4 observed_ids=0x500,0x501,0x502,0x503'
), 'the summary must sort and deduplicate IDs')
assert(normal:has(
    'id=0x500 callsite=0x80022220 mode_raw=0x29 mode=09'
), 'each row must preserve caller and normalized mode')
assert(not normal:has('id=0x4FF'), 'IDs below the range are ignored')
assert(not normal:has('id=0x507'), 'IDs above the range are ignored')
assert(normal:disabled())

local limited = capture(true, 9)
for index = 1, 48 do
    limited:lookup(0x500, 0x80030000 + index * 4)
end
limited:lookup(0x500, 0x80040000)
limited:frames(1)
assert(limited:has(
    'status: maximum context count reached; partial trace follows'
), 'the context capture must stop at 48 accepted rows')
assert(limited:has(
    'summary: hits=48 unique_ids=1 observed_ids=0x500'
), 'the hit bound must not inflate the unique-ID count')
assert(limited:disabled())

local silent = capture(true, 9)
silent:frames(600)
assert(silent:has(
    'no opening-scene lookups observed; confirm interpreter CPU'
), 'a no-hit run must print the likely setup diagnosis')
silent:frames(215400)
assert(silent:has('status: timed out without an opening-scene lookup'),
       'a silent run must finish with an explicit no-hit status')
assert(silent:disabled())

local broken = capture(true, 9)
broken:breakRegisters()
broken:lookup(0x500, 0x80055550)
broken:frames(1)
assert(broken:has('status: breakpoint callback error:'),
       'breakpoint failures must be surfaced on the next frame')
assert(broken:disabled())

local reset = capture(true, 9)
reset:lookup(0x502, 0x80066660)
reset:reset()
assert(reset:has('status: reset observed; rerun the script after reset'),
       'a reset must end the current capture explicitly')
assert(reset:has(
    'summary: hits=1 unique_ids=1 observed_ids=0x502'
), 'a reset must retain the partial capture')

local mismatch = capture(false, 9)
assert(mismatch:has(
    'Text_LookupString signature mismatch at 0x8003B744: 0x00000000'
), 'a wrong entry instruction must reject the trace')
assert(not mismatch:installed(),
       'signature rejection must happen before breakpoint installation')

print = hostPrint
print('campaign_opening_scene_ids: all six callback-replay cases passed')
