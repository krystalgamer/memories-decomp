-- Run from the repository root: luajit tools/trace/tests/rng_boot_timing_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/rng_boot_timing.lua'

local RAND = 0x8008e590
local SRAND = 0x8008e5c0
local SEED = 0x800fe6f8
local MODE = 0x8009b26c
local BOOT_SEED = 0x55555555
local STARTUP_SEED = 0x56

local function capture()
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a0 = 0, ra = 0 } } }
    local events = {}
    local breakpoints = {}
    local output = {}

    print = function(text)
        output[#output + 1] = tostring(text)
    end
    PCSX = {
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = { enabled = true, callback = callback }
            function handle:disable() self.enabled = false end
            function handle:remove() self.enabled = false end
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
    breakpoint_rng_boot_rand = nil
    breakpoint_rng_boot_srand = nil
    dofile(scriptPath)

    local result = {}
    function result:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function result:position(text)
        for index, line in ipairs(output) do
            if line:find(text, 1, true) then return index end
        end
        return nil
    end
    function result:reset()
        for _, handle in pairs(breakpoints) do handle:remove() end
        breakpoints = {}
        events['ExecutionFlow::Reset']()
        events['ExecutionFlow::ShellReached']()
    end
    function result:mode(value)
        memory[MODE - 0x80000000] = value
    end
    function result:frames(count)
        for _ = 1, count do events['GPU::Vsync']() end
    end
    function result:srand(value, site)
        registers.GPR.n.a0 = value
        registers.GPR.n.ra = site + 8
        assert(breakpoints[SRAND].enabled)
        breakpoints[SRAND].callback()
        ffi.cast('uint32_t*', memory + (SEED - 0x80000000))[0] = value
    end
    function result:rand()
        registers.GPR.n.ra = 0x80020008
        assert(breakpoints[RAND].enabled)
        breakpoints[RAND].callback()
        local state = ffi.cast('uint32_t*', memory + (SEED - 0x80000000))
        local nextState = ffi.new('uint64_t', state[0]) * 1103515245 + 12345
        state[0] = ffi.cast('uint32_t', nextState)
    end
    function result:disabled()
        return not breakpoints[RAND].enabled and not breakpoints[SRAND].enabled
    end
    return result
end

local completed =
    'status: captured startup and boot seeds through three seconds'

local normal = capture()
normal:reset()
normal:srand(STARTUP_SEED, 0x80013338)
normal:frames(5)
normal:srand(BOOT_SEED, 0x80012c48)
normal:rand()
normal:mode(8)
normal:frames(180)
assert(not normal:has(completed), 'capture ended before 180 post-entry frames')
normal:frames(1)
assert(normal:has(completed), 'capture must include both startup seeds')
assert(normal:has(
    'srand_calls=2 startup_seed_seen=true boot_seed_seen=true'
))
assert(normal:has('callsite=0x80013338'))
assert(normal:has('callsite=0x80012C48'))
assert(normal:position('requested=0x00000056')
       < normal:position('requested=0x55555555'))
assert(normal:disabled(), 'completed capture must disable both breakpoints')

local missingStartup = capture()
missingStartup:reset()
missingStartup:srand(BOOT_SEED, 0x80012c48)
missingStartup:mode(8)
missingStartup:frames(181)
assert(missingStartup:has(
    'status: captured boot seed but not initial 0x56 seed'
))
assert(missingStartup:has(
    'srand_calls=1 startup_seed_seen=false boot_seed_seen=true'
))

local laterReseed = capture()
laterReseed:reset()
laterReseed:srand(BOOT_SEED, 0x80012c48)
laterReseed:srand(STARTUP_SEED, 0x80013338)
laterReseed:mode(8)
laterReseed:frames(181)
assert(laterReseed:has('additional srand(0x56) observed'))
assert(laterReseed:has(
    'status: captured boot seed but not initial 0x56 seed'
))
assert(laterReseed:has(
    'srand_calls=2 startup_seed_seen=false boot_seed_seen=true'
))

local missingBoot = capture()
missingBoot:reset()
missingBoot:srand(STARTUP_SEED, 0x80013338)
missingBoot:mode(8)
missingBoot:frames(36000)
assert(not missingBoot:has(completed), 'mode 8 alone must not finish capture')
assert(missingBoot:has('status: timed out before srand(0x55555555)'))
assert(missingBoot:has(
    'startup_seed_seen=true boot_seed_seen=false'
))

local unarmed = capture()
unarmed:frames(36000)
assert(unarmed:has('status: timed out before BIOS shell capture'))

local reset = capture()
reset:reset()
reset:srand(STARTUP_SEED, 0x80013338)
reset:srand(BOOT_SEED, 0x80012c48)
reset:mode(8)
reset:frames(10)
reset:reset()
reset:frames(181)
assert(not reset:has(completed), 'reset must discard the old capture')
reset:srand(STARTUP_SEED, 0x80013338)
reset:srand(BOOT_SEED, 0x80012c48)
reset:mode(8)
reset:frames(181)
assert(reset:has(completed), 'capture must work after breakpoint reinstallation')
assert(reset:has(
    'srand_calls=2 startup_seed_seen=true boot_seed_seen=true'
))

local limited = capture()
limited:reset()
limited:srand(STARTUP_SEED, 0x80013338)
limited:srand(BOOT_SEED, 0x80012c48)
for _ = 1, 768 do limited:rand() end
limited:mode(8)
limited:frames(1)
assert(limited:has('status: maximum event count reached; partial boot trace follows'))
assert(limited:disabled(), 'event-limit completion must disable breakpoints')

print = hostPrint
print('rng_boot_timing: all seven callback-replay cases passed')
