-- Run from the repository root:
--   luajit tools/trace/tests/rng_runtime_calls_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
--
-- This verifies trace accounting and known PRNG arithmetic. It does not turn
-- the simulated modes or frame cadence into runtime gameplay evidence.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/rng_runtime_calls.lua'

local RAM_BASE = 0x80000000
local RAND = 0x8008e590
local SRAND = 0x8008e5c0
local SEED = 0x800fe6f8
local MAIN_MODE = 0x8009b26c

local function capture(mode, seed)
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a0 = 0, ra = 0 } } }
    local listeners = {}
    local breakpoints = {}
    local output = {}

    local function u8at(addr)
        return ffi.cast('uint8_t*', memory + (addr - RAM_BASE))
    end

    local function u32at(addr)
        return ffi.cast('uint32_t*', memory + (addr - RAM_BASE))
    end

    u8at(MAIN_MODE)[0] = (mode or 8) % 0x100
    u32at(SEED)[0] = seed or 0x12345678

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
    breakpoint_rng_runtime_rand = nil
    breakpoint_rng_runtime_srand = nil
    listener_rng_runtime_reset = nil
    listener_rng_runtime_calls = nil
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
            listeners['GPU::Vsync']()
        end
    end

    function result:mode(value)
        u8at(MAIN_MODE)[0] = value % 0x100
    end

    function result:seed(value)
        u32at(SEED)[0] = value
    end

    function result:rand(callsite)
        registers.GPR.n.ra = callsite + 8
        local breakpoint = breakpoints[RAND]
        assert(breakpoint and breakpoint.enabled)
        breakpoint.callback()
    end

    function result:srand(value, callsite)
        registers.GPR.n.a0 = value
        registers.GPR.n.ra = callsite + 8
        local breakpoint = breakpoints[SRAND]
        assert(breakpoint and breakpoint.enabled)
        breakpoint.callback()
    end

    function result:breakRegisters()
        PCSX.getRegisters = function()
            error('mock register failure')
        end
    end

    function result:reset()
        listeners['ExecutionFlow::Reset']()
    end

    function result:disabled()
        return not breakpoints[RAND].enabled
            and not breakpoints[SRAND].enabled
    end

    return result
end

local normal = capture(0x28, 0x12345678)
normal:rand(0x80011110)
normal:seed(0x0b719151)
normal:frames(300)
normal:mode(0x23)
normal:frames(1)
normal:srand(0xdeadbeef, 0x80022220)
normal:seed(0xdeadbeef)
normal:rand(0x80033330)
normal:seed(0x1c014dfc)
normal:frames(1499)
assert(normal:has('status: captured thirty seconds of runtime RNG activity'),
       'the normal capture must stop after 1,800 frames')
assert(normal:has(
    'summary: frames=1800 events=3 rand_calls=2 srand_calls=1 '
        .. 'start_seed=0x12345678 end_seed=0x1C014DFC'
), 'the final summary must retain seeds and call totals')
assert(normal:has(
    'event=0001 frame=0000 mode=08 kind=rand callsite=0x80011110 '
        .. 'old=0x12345678 next=0x0B719151 return=0x0B71'
), 'the first known recurrence must be printed exactly')
assert(normal:has(
    'event=0002 frame=0301 mode=03 kind=srand callsite=0x80022220 '
        .. 'old=0x0B719151 requested=0xDEADBEEF next=0xDEADBEEF'
), 'srand must preserve the requested 32-bit seed')
assert(normal:has(
    'event=0003 frame=0301 mode=03 kind=rand callsite=0x80033330 '
        .. 'old=0xDEADBEEF next=0x1C014DFC return=0x1C01'
), 'the second known recurrence must be printed exactly')
assert(normal:has(
    'mode_change frame=0301 old=08 new=03 seed=0x0B719151'
), 'mode changes must retain the frame and current seed')
assert(normal:has(
    'mode_summary mode=03 rand_calls=1 srand_calls=1 '
        .. 'first_frame=0301 last_frame=0301'
), 'mode summaries must aggregate both call kinds')
assert(normal:has(
    'caller_summary mode=08 kind=rand callsite=0x80011110 count=1'
), 'caller summaries must preserve the normalized call site')
assert(normal:has(
    'checkpoint frame=0300 mode=08 seed=0x0B719151 events=1'
), 'five-second checkpoints must capture seed and event count')
assert(normal:disabled(), 'a completed capture must disable both breakpoints')

local silent = capture(6, 0x55555555)
silent:frames(600)
assert(silent:has(
    'no RNG calls observed; confirm interpreter CPU'
), 'a no-hit window must print the likely setup diagnosis')
silent:frames(1200)
assert(silent:has(
    'summary: frames=1800 events=0 rand_calls=0 srand_calls=0'
), 'a silent capture must still finish with explicit zero totals')
assert(silent:disabled())

local limited = capture(8, 0x56)
for _ = 1, 2048 do
    limited:rand(0x80044440)
end
limited:frames(1)
assert(limited:has('status: maximum event count reached; bounded trace follows'),
       'the trace must stop after its 2,048-event bound')
assert(limited:has(
    'summary: frames=1 events=2048 rand_calls=2048 srand_calls=0'
), 'the event-bound summary must retain all accepted calls')
assert(limited:has(
    'caller_summary mode=08 kind=rand callsite=0x80044440 count=2048'
), 'the bounded caller summary must aggregate every event')
assert(limited:has('event=2048 frame=0000'),
       'the final accepted event must be present')
assert(limited:disabled())

local broken = capture(8, 0x56)
broken:breakRegisters()
broken:rand(0x80055550)
broken:frames(1)
assert(broken:has('status: breakpoint callback error:'),
       'breakpoint errors must be surfaced on the next frame')
assert(broken:disabled())

local reset = capture(8, 0x56)
reset:srand(-1, 0x80066660)
reset:reset()
assert(reset:has('status: reset observed; partial trace follows'),
       'a reset must end the current capture explicitly')
assert(reset:has('requested=0xFFFFFFFF next=0xFFFFFFFF'),
       'negative register values must normalize to 32 bits')
assert(reset:disabled())

print = hostPrint
print('rng_runtime_calls: all five callback-replay cases passed')
