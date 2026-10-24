-- Run from the repository root:
--   luajit tools/trace/tests/duel_effect_atlas_quad_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/duel_effect_atlas_quad.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local REQUESTS = 0x800ead88
local DISPATCH, CONTROL = 0x801462b0, 0x8002c6c8
local ALLOCATE, RETURN = 0x8002c604, 0x8002c784
local ATLAS, SIBLING = 0x80148f98, 0x80148e88
local RESIDENT = { [CONTROL] = 0x27bdffd8, [ALLOCATE] = 0x27bdffe8,
                   [RETURN] = 0x00002021 }
local MODULE = { [0x801462b0] = 0x27bdffc8, [0x801462b4] = 0xafb7002c,
                 [0x80148f98] = 0x27bdff98, [0x8015013c] = 0x30c60003 }

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { pc = 0, GPR = { n = { a0 = 0, a1 = 0, a2 = 0, a3 = 0, ra = 0 } } }
    local events, breakpoints, output = {}, {}, {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u16at(a) return ffi.cast('uint16_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end
    for addr, word in pairs(RESIDENT) do
        u32at(addr)[0] = options.corrupt == addr and 0 or word
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
    breakpoints_duel_effect_atlas_quad = nil
    listener_duel_effect_atlas_quad = nil
    dofile(scriptPath)

    local r = { output = output }
    function r:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function r:countOf(text)
        local n = 0
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then n = n + 1 end
        end
        return n
    end
    function r:frames(n) for _ = 1, n do events['GPU::Vsync']() end end
    function r:mode(v) u8at(MAIN_MODE)[0] = v end
    function r:load(on)
        for addr, word in pairs(MODULE) do u32at(addr)[0] = on and word or 0x1234 end
    end
    function r:vector(addr, x, y, z)
        u16at(addr)[0], u16at(addr + 2)[0], u16at(addr + 4)[0] =
            x % 0x10000, y % 0x10000, z % 0x10000
    end
    function r:call(addr, regs)
        local n = registers.GPR.n
        n.a0, n.a1, n.a2, n.a3, n.ra = regs.a0 or 0, regs.a1 or 0,
            regs.a2 or 0, regs.a3 or 0, regs.ra or 0
        if breakpoints[addr].enabled then breakpoints[addr].callback() end
    end
    function r:request(slot, id)
        local req = REQUESTS + slot * 0x20
        u8at(req + 0x1c)[0] = 0x80
        u16at(req + 0x18)[0] = id
        return req
    end
    function r:release(slot) u8at(REQUESTS + slot * 0x20 + 0x1c)[0] = 0 end
    function r:count() local n = 0 for _ in pairs(breakpoints) do n = n + 1 end return n end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do if bp.enabled then return false end end
        return true
    end
    return r
end

-- The case the script exists for: the helper runs inside effect 9 from one
-- caller with two indices, the sibling runs inside effect 6, and a stray
-- call after the dispatcher returned is labelled outside.
local duel = capture()
assert(duel:count() == 6, 'six breakpoints')
assert(duel:has('6 breakpoints installed, 0 signature mismatches'))
duel:mode(3)
duel:load(true)
duel:frames(1)
duel:call(CONTROL, {})
assert(duel:has('execution breakpoint observed; setup confirmed'))
duel:vector(0x80100000, -16, 32, 0)
duel:call(ALLOCATE, {})
local req = duel:request(0, 9)
duel:call(DISPATCH, { a0 = 9, a1 = 0, a3 = req })
duel:call(ATLAS, { a0 = 0x10040, a1 = 3, a2 = 0x80100000, ra = 0x80158100 })
assert(duel:has('#01 t=   0.0s atlas func_80156D50 ra=0x80158100 size=64 '
                .. 'index=3 offset=(-16,32,0)'))
duel:call(ATLAS, { a0 = 64, a1 = 3, a2 = 0x80100000, ra = 0x80158100 })
duel:call(ATLAS, { a0 = 64, a1 = 4, a2 = 0x80100001, ra = 0x80158100 })
assert(duel:countOf('atlas func_80156D50 ra=0x80158100') == 2,
       'a repeated index prints no row')
assert(duel:has('index=4 offset=unreadable'), 'an odd vector address is not read')
duel:call(RETURN, {})
duel:call(ATLAS, { a0 = 32, a1 = 1, a2 = 0, ra = 0x80170000 })
assert(duel:has('-- t=   0.0s atlas func_80156D50 ra=0x80170000 size=32 index=1 '
                .. 'offset=unreadable'))
duel:call(ALLOCATE, {})
local req2 = duel:request(1, 6)
duel:call(DISPATCH, { a0 = 6, a1 = 0, a3 = req2 })
duel:vector(0x800f0000, 0x2010, 0x30, 0)
duel:call(SIBLING, { a0 = 0x800f0000, a1 = 48, a2 = 0x80100000, ra = 0x80155b90 })
assert(duel:has('#02 t=   0.0s sibling func_80156C40 ra=0x80155B90 size=48 '
                .. 'rgb=(16,32,48) offset=(-16,32,0)'))
duel:call(SIBLING, { a0 = 0, a1 = 48, ra = 0x80155b90 })
duel:release(0)
duel:frames(1)
assert(duel:has('#01 t=   0.0s END   effect=9 frames=1 calls=1 '
                .. 'helpers=atlasx3 reason=finished'))
duel:mode(1)
duel:frames(1)
assert(duel:has('#02 t=   0.1s END   effect=6 frames=2 calls=1 '
                .. 'helpers=siblingx2 reason=still running'))
assert(duel:has('status: left the duel'))
assert(duel:has('atlas_calls: 4 sibling_calls: 2'))
assert(duel:has('caller sibling func_80156C40 ra=0x80155B90 calls=2 sizes=48 '
                .. 'indices=n/a effects=#02/e6'))
assert(duel:has('verdict: func_80156D50 executed'))
assert(duel:has('caller atlas func_80156D50 ra=0x80158100 calls=3 sizes=64 '
                .. 'indices=3,4 effects=#01/e9'))
assert(duel:has('caller atlas func_80156D50 ra=0x80170000 calls=1 sizes=32 '
                .. 'indices=1 effects=outside'))
assert(duel:allDisabled())
assert(duel:has('copy everything above into tools/trace/result/duel_effect_atlas_quad.txt'))

-- The negative result the doubt expects: the sibling control runs, the
-- helper never does.
local dead = capture()
dead:mode(3)
dead:load(true)
dead:frames(1)
dead:call(DISPATCH, { a0 = 7, a3 = dead:request(0, 7) })
dead:call(SIBLING, { a1 = 64, ra = 0x80157a54 })
assert(dead:has('size=64 rgb=unreadable offset=unreadable'))
dead:call(DISPATCH, { a0 = 2, a3 = dead:request(1, 2) })
dead:mode(0)
dead:frames(1)
assert(dead:has('verdict: func_80156D50 never executed while the bank ran 2 '
                .. 'dispatcher calls and the sibling control ran 1 times '
                .. '(effects seen: 2,7)'))

-- Without the control the negative count is not claimed.
local weak = capture()
weak:mode(3)
weak:load(true)
weak:frames(1)
weak:call(DISPATCH, { a0 = 2, a3 = weak:request(0, 2) })
weak:mode(0)
weak:frames(1)
assert(weak:has('neither did the sibling control (effects seen: 2)'))

-- Another module at the bank addresses: nothing counts.
local foreign = capture()
foreign:mode(3)
foreign:load(false)
foreign:frames(1)
foreign:call(ATLAS, { a0 = 1 })
foreign:call(DISPATCH, { a0 = 1 })
foreign:mode(0)
foreign:frames(1)
assert(foreign:has('foreign_module_hits: 2'))
assert(foreign:has('atlas_calls: 0'))
assert(foreign:has('verdict: inconclusive; the NA duel_effects bank never ran'))

-- The bank is replaced between the dispatch and a helper hit: the hit is
-- foreign, and once the words return the effect is no longer current.
local swap = capture()
swap:mode(3)
swap:load(true)
swap:frames(1)
swap:call(DISPATCH, { a0 = 9, a3 = swap:request(0, 9) })
swap:load(false)
swap:call(ATLAS, { a0 = 8, a1 = 2, ra = 0x80158100 })
swap:load(true)
swap:call(ATLAS, { a0 = 8, a1 = 2, ra = 0x80158100 })
assert(swap:has('-- t=   0.0s atlas func_80156D50 ra=0x80158100 size=8 index=2'))
assert(not swap:has('#01 t=   0.0s atlas'))
swap:mode(0)
swap:frames(1)
assert(swap:has('foreign_module_hits: 1 atlas_calls: 1'))
assert(swap:has('effects=outside'))

-- Implausible request pointers are refused.
local bad = capture()
bad:mode(3)
bad:load(true)
bad:frames(1)
bad:call(DISPATCH, { a0 = 1, a3 = 0x7ffffff0 })
assert(bad:has('IMPLAUSIBLE request=0x7FFFFFF0'))
assert(not bad:has('START'))

-- Live call rows are capped at 80.
local busy = capture()
busy:mode(3)
busy:load(true)
busy:frames(1)
busy:call(DISPATCH, { a0 = 9, a3 = busy:request(0, 9) })
for i = 1, 120 do busy:call(ATLAS, { a0 = 8, a1 = i % 32, ra = 0x80158000 + i * 4 }) end
assert(busy:countOf(' atlas func_80156D50 ra=') == 80)
busy:mode(0)
busy:frames(1)
assert(busy:has('atlas_calls: 120'))

-- More than 32 distinct indices from one caller are marked, not listed.
local wide = capture()
wide:mode(3)
wide:load(true)
wide:frames(1)
for i = 0, 40 do wide:call(ATLAS, { a0 = 8, a1 = i, ra = 0x80158000 }) end
wide:mode(0)
wide:frames(1)
assert(wide:has('indices=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,'
                .. '20,21,22,23,24,25,26,27,28,29,30,31+ effects=outside'))

-- Window, silent setup, signature mismatch and callback error.
local window = capture()
window:mode(3)
window:load(true)
window:frames(1)
window:call(DISPATCH, { a0 = 4, a3 = window:request(2, 4) })
window:frames(180 * 60)
assert(window:has('status: three-minute window after the first effect ended'))
local silent = capture()
silent:mode(3)
silent:frames(600 * 60)
assert(silent:has('ten minutes in a duel without a control hit'))
assert(silent:has('execution_breakpoint=NOT-observed'))
local wrong = capture({ corrupt = CONTROL })
assert(wrong:has('1 signature mismatches'))
local broken = capture()
broken:mode(3)
broken:load(true)
broken:frames(1)
PCSX.getRegisters = function() error('registers unavailable') end
broken:call(ATLAS, {})
broken:frames(1)
assert(broken:has('status: callback error'))
assert(broken:has('callback_error: '))

io.stdout:write('duel_effect_atlas_quad_test: ok\n')
