-- Run from the repository root:
--   luajit tools/trace/tests/display_effect_steps_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/display_effect_steps.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local RECORDS = 0x800eb010
local TABLE = 0x80090f68
local TABLE_WORDS = {
    0x80039ff8, 0x80039ff8, 0x8003a560, 0x8003a990, 0x8003ad6c, 0x8003b054,
    0x8003ac48, 0x8003aae4, 0x8003da40, 0x8003d74c, 0x8003d614, 0x8003d518,
    0x8003da40,
}

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a0 = 0, ra = 0 } } }
    local events, breakpoints, output = {}, {}, {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u16at(a) return ffi.cast('uint16_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end
    for i, w in ipairs(TABLE_WORDS) do u32at(TABLE + (i - 1) * 4)[0] = w end
    if options.patch then u32at(TABLE + 4 * 5)[0] = 0 end
    u8at(MAIN_MODE)[0] = 0x05

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
    breakpoints_display_effect_steps = nil
    listener_display_effect_steps = nil
    dofile(scriptPath)

    local r = {}
    function r:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function r:frames(n) for _ = 1, n do events['GPU::Vsync']() end end
    function r:setRecord(slot, step, f31)
        local rec = RECORDS + slot * 0x4c
        u8at(rec + 0x30)[0] = 0
        u8at(rec + 0x31)[0] = f31 or 0
        u8at(rec + 0x33)[0] = step
        u16at(rec + 0x34)[0] = 0xd8
    end
    -- What DisplayEffect_ProcessMenuRecords does for one live record.
    function r:dispatch(slot)
        local rec = RECORDS + slot * 0x4c
        local step = u8at(rec + 0x33)[0] % 32
        local handler = TABLE_WORDS[step + 1]
        registers.GPR.n.a0 = rec
        if breakpoints[handler].enabled then breakpoints[handler].callback() end
    end
    function r:direct(handler, argument, caller)
        registers.GPR.n.a0 = argument
        registers.GPR.n.ra = caller
        if breakpoints[handler].enabled then breakpoints[handler].callback() end
    end
    function r:count() local n = 0 for _ in pairs(breakpoints) do n = n + 1 end return n end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do if bp.enabled then return false end end
        return true
    end
    return r
end

-- Normal: slot 1 runs the portrait step at half rate, then changes to the
-- fade-out step; each is one episode row.
local normal = capture()
assert(normal:count() == 11, 'eleven distinct handlers')
assert(normal:has('11 breakpoints installed; table check passed'))
normal:setRecord(1, 5, 0x21)
for _ = 1, 6 do normal:dispatch(1); normal:frames(2) end
normal:setRecord(1, 6, 0x21)
for _ = 1, 3 do normal:dispatch(1); normal:frames(1) end
normal:frames(10810)
assert(normal:has('status: three quiet minutes without a new row'))
assert(normal:has('slot=1 step=5 handler=0x8003B054 frames=6 mode=0x05 '
                  .. 'start: live=0 f31=0x21'))
assert(normal:has('slot=1 step=6 handler=0x8003AC48 frames=3'))
assert(normal:has('count step=5 handler=0x8003B054 calls=6'))
assert(normal:allDisabled())

-- Steps 1 and 12 share handlers with 0 and 8; the step byte, not the
-- handler, identifies the row.
local shared = capture()
shared:setRecord(0, 12)
shared:dispatch(0)
shared:frames(10810)
assert(shared:has('slot=0 step=12 handler=0x8003DA40'))

-- A call from outside the table is reported, not read as a record.
local foreign = capture()
foreign:direct(0x8003a990, 0x800f0000, 0x80039e00)
foreign:direct(0x8003a990, 0x800f0000, 0x80039e00)
foreign:direct(0x8003a990, 0x800eb012, 0x80039e00)
foreign:frames(10810)
assert(foreign:has('FOREIGN handler=0x8003A990 argument=0x800F0000 '
                   .. 'caller=0x80039E00'))
assert(not foreign:has('row=02'), 'one foreign row per handler episode')

-- A patched table is reported.
local patched = capture({ patch = true })
assert(patched:has('table check FAILED (1)'))

-- Bounded output.
local flood = capture()
for i = 1, 70 do
    flood:setRecord(i % 3, (i % 12) + 1)
    flood:dispatch(i % 3)
    flood:frames(6)
end
assert(flood:has('status: captured 64 rows'))

-- Silence names the likely setup mistake.
local silent = capture()
silent:frames(36000)
assert(silent:has('status: no step ran in ten minutes'))

print = hostPrint
print('display_effect_steps: all six callback-replay cases passed')
