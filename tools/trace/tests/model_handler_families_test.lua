-- Run from the repository root:
--   luajit tools/trace/tests/model_handler_families_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/model_handler_families.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local REGISTRY = 0x800f5918
local FC1C, FE44 = 0x8005fc1c, 0x8005fe44
local REGISTER, FIND = 0x80060170, 0x800601d0
local RUN_SLOT, DRAW_SLOT, CONTROL = 0x80059dd8, 0x800540b4, 0x8001306c
local WORDS = {
    [FC1C] = 0x3c02800f, [FE44] = 0x3c02800f, [REGISTER] = 0x3c02800f,
    [FIND] = 0x3c02800f, [RUN_SLOT] = 0x27bdffd0, [DRAW_SLOT] = 0x27bdf738,
    [CONTROL] = 0x27bdffe0,
}

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local n = { a0 = 0, a1 = 0, v0 = 0, ra = 0 }
    local registers = { GPR = { n = n } }
    local events, breakpoints, output = {}, {}, {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end
    for addr, word in pairs(WORDS) do
        u32at(addr)[0] = options.corrupt == addr and 0 or word
    end
    u8at(MAIN_MODE)[0] = 3

    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = { enabled = true, callback = callback, removed = false }
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
    breakpoints_model_handler_families = nil
    listener_model_handler_families = nil
    dofile(scriptPath)

    local r = { breakpoints = breakpoints }
    function r:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function r:frames(count) for _ = 1, count do events['GPU::Vsync']() end end
    function r:register(index, value, key)
        u32at(REGISTRY + index * 8)[0] = value
        u32at(REGISTRY + index * 8 + 4)[0] = key
    end
    function r:call(address, regs)
        for k, v in pairs(regs or {}) do n[k] = v % 0x100000000 end
        local bp = breakpoints[address]
        if bp ~= nil and bp.enabled and not bp.removed then
            -- PCSX-Redux removes a breakpoint whose invoker returns false.
            if bp.callback() == false then bp.removed = true end
        end
    end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do
            if bp.enabled and not bp.removed then return false end
        end
        return true
    end
    return r
end

-- The case the script exists for: a registration, then each family looks up
-- a registered value and its return value is read at the call site.
local live = capture()
assert(live:has('7 breakpoints installed, 0 signature mismatches'))
live:call(CONTROL)
live:call(REGISTER, { a0 = 0x00020015, a1 = 0x80089f00, ra = 0x80056a00 })
live:register(0, 0x80089f00, 0x00020015)
live:register(1, 0x80089f40, 0x00300001)
live:call(RUN_SLOT, { a0 = 1 })
live:call(FC1C, { a0 = 0x80089f00, ra = 0x8005a5a0 })
assert(live.breakpoints[0x8005a5a0] ~= nil, 'a return breakpoint is armed')
live:call(0x8005a5a0, { v0 = 0x80062058 })
assert(live.breakpoints[0x8005a5a0].removed, 'it removes itself')
live:call(DRAW_SLOT, { a0 = 0 })
live:call(FE44, { a0 = 0x80089f40, ra = 0x8005a5b0 })
live:call(0x8005a5b0, { v0 = 0x80067354 })
live:call(FE44, { a0 = 0x80089e20, ra = 0x8005a5b0 })
live:call(0x8005a5b0, { v0 = 0x80089e20 })
live:call(FIND, { a0 = 0x80089f40, ra = 0x8005f9a0 })
live:frames(10801)
assert(live:has('status: three quiet minutes without a new row'))
assert(live:has('execution_breakpoint=observed'))
assert(live:has('Model_RegisterHandlerKey key=0x00020015 group=0x0002 '
                .. 'kind=0x0015 value=0x80089F00 caller=0x80056A00'))
assert(live:has('family func_8005FC1C value=0x80089F00 key=0x00020015 '
                .. 'group=0x0002 kind=0x0015 caller=0x8005A5A0 slot=run(1) '
                .. 'main=0x03 returns=0x80062058'))
assert(live:has('family func_8005FE44 value=0x80089F40 key=0x00300001 '
                .. 'group=0x0030 kind=0x0001 caller=0x8005A5B0 slot=draw(0) '
                .. 'main=0x03 returns=0x80067354'))
assert(live:has('family func_8005FE44 value=0x80089E20 key=none(-1)'),
       'the GsU_00000000 sentinel is never looked up')
assert(live:has('Model_FindHandlerKey value=0x80089F40 key=0x00300001'))
assert(live:has('  [00] value=0x80089F00 key=0x00020015'))
assert(live:has('  [01] value=0x80089F40 key=0x00300001'))
assert(not live:has('  [02]'), 'empty registry entries are not printed')
assert(live:allDisabled())

-- Repeats of one value are counted, not re-logged.
local repeats = capture()
repeats:call(CONTROL)
for _ = 1, 20 do
    repeats:call(FC1C, { a0 = 0x80089f00, ra = 0x8005a5a0 })
    repeats:call(0x8005a5a0, { v0 = 1 })
end
repeats:frames(10801)
assert(not repeats:has('row=02 '))
assert(repeats:has('family func_8005FC1C') and repeats:has(' 20'))

-- A return that never comes is reported after the timeout.
local lost = capture()
lost:call(CONTROL)
lost:call(FC1C, { a0 = 0x80089f00, ra = 0x8005a5a0 })
lost:frames(61)
assert(not lost.breakpoints[0x8005a5a0].enabled)
lost:frames(10801)
assert(lost:has('returns=not-observed'))

-- A bad return address is not armed.
local bad = capture()
bad:call(CONTROL)
bad:call(FC1C, { a0 = 0x80089f00, ra = 0x00001000 })
bad:call(FC1C, { a0 = 0x80089f00, ra = 0x8005a5a2 })
assert(bad.breakpoints[0x00001000] == nil and bad.breakpoints[0x8005a5a2] == nil)

-- A return capture still open when the row cap ends the trace is disabled
-- with everything else.
local open = capture()
open:call(CONTROL)
for i = 1, 89 do
    open:call(FC1C, { a0 = 0x80100000 + i * 4, ra = 0x8005a000 + i * 4 })
    open:call(0x8005a000 + i * 4, { v0 = i })
end
open:call(FE44, { a0 = 0x80089f00, ra = 0x8005b000 })
open:call(FIND, { a0 = 0x80089f00, ra = 0x8005f9a0 })
open:frames(1)
assert(open:has('status: captured 90 rows'))
assert(not open.breakpoints[0x8005b000].enabled,
       'an open return capture is disabled at the end')
assert(open:allDisabled())

-- Bounded output.
local flood = capture()
flood:call(CONTROL)
for i = 1, 100 do
    flood:call(FC1C, { a0 = 0x80100000 + i * 4, ra = 0x8005a000 + i * 4 })
    flood:call(0x8005a000 + i * 4, { v0 = i })
end
flood:frames(1)
assert(flood:has('status: captured 90 rows'))
assert(flood:has('rows: 90'))

-- A wrong entry word is named; silence without the control is setup failure.
local corrupt = capture({ corrupt = FE44 })
assert(corrupt:has('1 signature mismatches'))
local silent = capture()
silent:frames(18000)
assert(silent:has('status: five minutes without a control hit'))
assert(silent:has('execution_breakpoint=NOT-observed'))

print = hostPrint
print('model_handler_families: all eight callback-replay cases passed')
