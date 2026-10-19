-- Run from the repository root:
--   luajit tools/trace/tests/ai_turn_action_liveness_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/ai_turn_action_liveness.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local PAD1_PRESSED = 0x8009b398
local SELECTION = 0x800eae88
local WORDS = {
    [0x8002712c] = 0x27bdffc8, [0x80027228] = 0x27bdffa8,
    [0x80027508] = 0x27bdffe0, [0x8002778c] = 0x27bdffd0,
    [0x800278a0] = 0x27bdffd0, [0x800279bc] = 0x27bdff90,
    [0x80070650] = 0x27bdffd8, [0x80027df8] = 0x27bdffc8,
}

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { ra = 0 } } }
    local events, breakpoints, output = {}, {}, {}
    for addr, word in pairs(WORDS) do
        if not (options.corrupt == addr) then
            ffi.cast('uint32_t*', memory + (addr - RAM_BASE))[0] = word
        end
    end

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
    breakpoints_ai_turn_action_liveness = nil
    listener_ai_turn_action_liveness = nil
    dofile(scriptPath)

    local r = {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u16at(a) return ffi.cast('uint16_t*', memory + (a - RAM_BASE)) end
    function r:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function r:frames(n) for _ = 1, n do events['GPU::Vsync']() end end
    function r:mode(v) u8at(MAIN_MODE)[0] = v end
    function r:hit(addr, ra)
        registers.GPR.n.ra = ra or 0x80012345
        assert(breakpoints[addr] ~= nil, string.format('no bp at %08X', addr))
        if breakpoints[addr].enabled then breakpoints[addr].callback() end
    end
    function r:select(n)
        u8at(SELECTION + n)[0] = 0
    end
    function r:writeSelection(action, argument, flag)
        u8at(SELECTION + 9)[0] = action
        u8at(SELECTION + 10)[0] = argument
        u8at(SELECTION + 11)[0] = flag
    end
    function r:press(mask)
        u16at(PAD1_PRESSED)[0] = mask
        r:frames(1)
        u16at(PAD1_PRESSED)[0] = 0
        r:frames(1)
    end
    function r:hasBoth(a, b)
        for _, line in ipairs(output) do
            if line:find(a, 1, true) and line:find(b, 1, true) then
                return true
            end
        end
        return false
    end
    function r:count() local n = 0 for _ in pairs(breakpoints) do n = n + 1 end return n end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do if bp.enabled then return false end end
        return true
    end
    return r
end

-- All eight entries are armed, and the retail words are checked.
local armed = capture()
assert(armed:count() == 8, 'eight breakpoints must be installed')
assert(armed:has('8 breakpoints installed, 0 signature mismatches'))

-- A mismatched word is reported rather than silently trusted.
local corrupt = capture({ corrupt = 0x800279bc })
assert(corrupt:has('1 signature mismatches'))
corrupt:frames(18000)
assert(corrupt:has('signature_mismatch: 0x800279BC expected 0x27BDFF90'))

-- The case the script exists for: the decision runs, and the selection it
-- wrote is read back on the following frame.
local live = capture()
live:mode(3)
live:hit(0x80070650)
live:hit(0x800279bc, 0x80019abc)
live:writeSelection(2, -1 % 256, 1)
live:frames(1)
live:mode(1)
live:press(0x100)
live:press(0x100)
assert(live:has('status: stopped by Select twice outside a duel'))
assert(live:has('hit=func_800279BC play decision caller=0x80019ABC'))
assert(live:has('action=2 argument=-1 flag=1'), 'selection must be read back')
assert(live:has('mode=0x03'))
assert(live:allDisabled(), 'finished trace must disable every breakpoint')

-- Select inside a duel, or before any duel, must not stop the capture.
local early = capture()
early:mode(1)
early:press(0x100)
early:press(0x100)
assert(not early:has('status:'), 'Select before any duel must not stop')
early:mode(3)
early:press(0x100)
early:press(0x100)
assert(not early:has('status:'), 'Select inside a duel must not stop')

-- Controls are capped low so a long session cannot crowd out the targets.
local busy = capture()
busy:mode(3)
busy:frames(1)
for _ = 1, 500 do busy:hit(0x80070650) end
busy:hit(0x80027508)
busy:mode(1)
busy:press(0x100)
busy:press(0x100)
assert(busy:hasBoth('AiScript_Run (control)', ' 500'),
       'every control hit must be counted')
assert(busy:has('hit=func_80027508 random action pick'))

-- Bounded output.
local flood = capture()
for _ = 1, 10 do
    for addr in pairs(WORDS) do flood:hit(addr) end
end
flood:frames(1)
assert(flood:has('status: captured 48 rows'))

-- Silence names the likely setup mistake.
local silent = capture()
silent:frames(18000)
assert(silent:has('status: no hit at all, not even the controls'))

print = hostPrint
print('ai_turn_action_liveness: all seven callback-replay cases passed')
