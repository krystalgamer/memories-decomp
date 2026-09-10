-- Run from the repository root:
--   luajit tools/trace/tests/opponent_card_resource_indices_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
--
-- This verifies patching, collection, bounds, and restoration. It does not
-- identify what resource indices 0 or 1 display in the emulator.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/opponent_card_resource_indices.lua'

local RAM_BASE = 0x80000000
local FUNCTION_ENTRY = 0x80018004
local FUNCTION_RETURN = 0x8001806c
local PATCH_INSTRUCTION = 0x80018064
local RETAIL_WORD = 0x240200ff
local PRECEDING_WORD = 0x04410002
local FOLLOWING_WORD = 0xa0820067
local EPILOGUE_WORD = 0x8fbf0014
local STATE_POINTER = 0x8009b1c8
local MAIN_MODE = 0x8009b26c
local PLAYER_LP = 0x800ea004
local OPPONENT_LP = 0x800ea024
local STATE = 0x80001000

local function capture(validSignature)
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a0 = 0 } } }
    local events = {}
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

    u32at(PATCH_INSTRUCTION - 4)[0] = PRECEDING_WORD
    u32at(PATCH_INSTRUCTION)[0] =
        validSignature == false and 0x240200fe or RETAIL_WORD
    u32at(PATCH_INSTRUCTION + 4)[0] = FOLLOWING_WORD
    u32at(PATCH_INSTRUCTION + 8)[0] = EPILOGUE_WORD
    u32at(STATE_POINTER)[0] = STATE
    u8at(STATE + 0x1f)[0] = 0xff
    u8at(MAIN_MODE)[0] = 3
    u16at(PLAYER_LP)[0] = 8000
    u16at(OPPONENT_LP)[0] = 7000

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
                events[name] = callback
                return {}
            end,
        },
    }
    breakpoint_opponent_card_resource_entry = nil
    breakpoint_opponent_card_resource_return = nil
    listener_opponent_card_resource_indices = nil
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
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:statePointer(value)
        u32at(STATE_POINTER)[0] = value
    end

    function result:displayMode(value)
        u8at(STATE + 0x1f)[0] = value % 0x100
    end

    function result:resource(object, value)
        u8at(object + 0x67)[0] = value
    end

    function result:entry(object)
        registers.GPR.n.a0 = object
        local breakpoint = breakpoints[FUNCTION_ENTRY]
        assert(breakpoint and breakpoint.enabled)
        breakpoint.callback()
    end

    function result:returnObject(object)
        registers.GPR.n.a0 = object
        local breakpoint = breakpoints[FUNCTION_RETURN]
        assert(breakpoint and breakpoint.enabled)
        breakpoint.callback()
    end

    function result:byte(addr)
        return tonumber(u8at(addr)[0])
    end

    function result:word(addr)
        return tonumber(u32at(addr)[0])
    end

    function result:disabled()
        return not breakpoints[FUNCTION_ENTRY].enabled
            and not breakpoints[FUNCTION_RETURN].enabled
    end

    return result
end

local normal = capture(true)
normal:frames(1)
assert(normal:has('phase 0 armed; trigger an opponent-card display'))
assert(normal:byte(PATCH_INSTRUCTION) == 0)

normal:statePointer(0)
normal:entry(0x80002000)
normal:returnObject(0)
normal:statePointer(STATE)
normal:displayMode(0)
normal:entry(0x80002010)
normal:returnObject(0)

normal:displayMode(-1)
normal:resource(0x80003000, 0)
normal:entry(0x80002020)
normal:returnObject(0x80003000)

normal:resource(0x80003000, 7)
normal:frames(1)
normal:frames(599)
assert(normal:byte(PATCH_INSTRUCTION) == 1)

normal:resource(0x80003100, 1)
normal:entry(0x80002030)
normal:returnObject(0x80003100)
normal:frames(600)
assert(normal:has('status: captured both resource-index phases'))
assert(normal:has(
    'patch frame=000001 phase=0 instruction=0x24020000 immediate=0x00'
), 'phase 0 must patch only the immediate byte')
assert(normal:has(
    'phase=0 input_object=0x80002020 result_object=0x80003000 '
        .. 'display_mode=-1 resource=0 main_mode=0x03 '
        .. 'player_lp=8000 opponent_lp=7000'
), 'phase 0 must capture the negative-mode object context')
assert(normal:has(
    'resource_change frame=000002 phase=0 object=0x80003000 old=0 new=7'
), 'tracked resource changes must be recorded')
assert(normal:has('phase_end frame=000601 phase=0'))
assert(normal:has(
    'summary: hits=4 phase0=true phase1=true resource_changes=1 restored=true'
), 'the final summary must retain ignored hits and restoration state')
assert(normal:word(PATCH_INSTRUCTION) == RETAIL_WORD)
assert(normal:disabled())

local changes = capture(true)
changes:frames(1)
changes:resource(0x80004000, 0)
changes:entry(0x80004100)
changes:returnObject(0x80004000)
for value = 1, 33 do
    changes:resource(0x80004000, value)
    changes:frames(1)
end
changes:frames(567)
changes:resource(0x80004200, 1)
changes:entry(0x80004300)
changes:returnObject(0x80004200)
changes:frames(600)
assert(changes:has('resource_change output limit reached'),
       'resource changes beyond 32 must be summarized once')
assert(changes:has('resource_changes=32'),
       'the summary must preserve the resource-change output bound')
assert(changes:word(PATCH_INSTRUCTION) == RETAIL_WORD)

local broken = capture(true)
broken:frames(1)
broken:entry(0x80005000)
broken:returnObject(0)
broken:frames(1)
assert(broken:has('status: breakpoint callback error:'),
       'invalid negative-mode results must surface on the next frame')
assert(broken:has('invalid result object 0x00000000'),
       'the callback error must retain the rejected pointer')
assert(broken:has('restored=true'))
assert(broken:word(PATCH_INSTRUCTION) == RETAIL_WORD)
assert(broken:disabled())

local limited = capture(true)
limited:frames(1)
limited:displayMode(0)
for _ = 1, 16384 do
    limited:entry(0x80006000)
end
limited:frames(1)
assert(limited:has('status: maximum function-entry hit count reached'))
assert(limited:has(
    'summary: hits=16384 phase0=false phase1=false '
        .. 'resource_changes=0 restored=true'
), 'the entry bound must be explicit and restore the patch')
assert(limited:word(PATCH_INSTRUCTION) == RETAIL_WORD)
assert(limited:disabled())

local silent = capture(true)
silent:frames(1)
silent:frames(600)
assert(silent:has(
    'no function hits; select the interpreter CPU'
), 'a silent duel-mode run must print the likely setup diagnosis')
silent:frames(107399)
assert(silent:has(
    'status: timed out before both resource-index phases were captured'
), 'a silent run must end with an explicit timeout')
assert(silent:has('restored=true'))
assert(silent:word(PATCH_INSTRUCTION) == RETAIL_WORD)
assert(silent:disabled())

local mismatch = capture(false)
mismatch:frames(1)
assert(mismatch:has(
    'status: retail instruction signature not present: 0x240200FE'
), 'a non-retail immediate must reject the trace before patching')
assert(mismatch:has('restored=false'))
assert(mismatch:word(PATCH_INSTRUCTION) == 0x240200fe)
assert(mismatch:disabled())

print = hostPrint
print('opponent_card_resource_indices: all six callback-replay cases passed')
