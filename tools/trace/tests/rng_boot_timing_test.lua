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

local completed = 'status: captured boot through three seconds of main mode 8'

local normal = capture()
normal:reset()
normal:srand(0x56, 0x80013338)
normal:frames(5)
normal:srand(BOOT_SEED, 0x80012c48)
normal:rand()
normal:mode(8)
normal:frames(180)
assert(not normal:has(completed), 'capture ended before 180 post-entry frames')
normal:frames(1)
assert(normal:has(completed), 'capture must finish without a post-boot 0x56 seed')
assert(normal:has('srand_calls=1 startup_seed_seen=false'))
assert(normal:has('callsite=0x80012C48'))
assert(normal:disabled(), 'completed capture must disable both breakpoints')

local reseeded = capture()
reseeded:srand(BOOT_SEED, 0x80012c48)
reseeded:srand(0x56, 0x80013338)
reseeded:mode(8)
reseeded:frames(181)
assert(reseeded:has(completed), 'optional later reseeds must remain supported')
assert(reseeded:has('srand_calls=2 startup_seed_seen=true'))

local unarmed = capture()
unarmed:reset()
unarmed:srand(0x56, 0x80013338)
unarmed:mode(8)
unarmed:frames(36000)
assert(not unarmed:has(completed), 'mode 8 alone must not arm capture')
assert(unarmed:has('status: timed out before srand(0x55555555)'))

local reset = capture()
reset:srand(BOOT_SEED, 0x80012c48)
reset:mode(8)
reset:frames(10)
reset:reset()
reset:frames(181)
assert(not reset:has(completed), 'reset must discard the old capture')
reset:srand(BOOT_SEED, 0x80012c48)
reset:frames(181)
assert(reset:has(completed), 'capture must work after breakpoint reinstallation')
assert(reset:has('srand_calls=1 startup_seed_seen=false'))

local limited = capture()
limited:srand(BOOT_SEED, 0x80012c48)
for _ = 1, 768 do limited:rand() end
limited:mode(8)
limited:frames(1)
assert(limited:has('status: maximum event count reached; partial boot trace follows'))
assert(limited:disabled(), 'event-limit completion must disable breakpoints')

print = hostPrint
print('rng_boot_timing: all five callback-replay cases passed')
