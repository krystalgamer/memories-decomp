-- Run from the repository root:
--   luajit tools/trace/tests/duel_effect_ids_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/duel_effect_ids.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local REQUESTS = 0x800ead88
local DISPATCH, CONTROL = 0x801462b0, 0x8002c6c8
local CREATE, ALLOCATE = 0x8002c68c, 0x8002c604
local RESIDENT = { [CONTROL] = 0x27bdffd8, [CREATE] = 0x27bdffe8,
                   [ALLOCATE] = 0x27bdffe8 }
local MODULE = { [0x801462b0] = 0x27bdffc8, [0x801462b4] = 0xafb7002c,
                 [0x80148f98] = 0x27bdff98, [0x8015013c] = 0x30c60003 }

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a0 = 0, a1 = 0, a2 = 0, a3 = 0, ra = 0 } } }
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
    breakpoints_duel_effect_ids = nil
    listener_duel_effect_ids = nil
    dofile(scriptPath)

    local r = { output = output }
    function r:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function r:index(text)
        for i, line in ipairs(output) do
            if line:find(text, 1, true) then return i end
        end
        return nil
    end
    function r:frames(n) for _ = 1, n do events['GPU::Vsync']() end end
    function r:mode(v) u8at(MAIN_MODE)[0] = v end
    function r:load(on)
        for addr, word in pairs(MODULE) do u32at(addr)[0] = on and word or 0x1234 end
    end
    function r:call(addr, regs)
        local n = registers.GPR.n
        n.a0, n.a1, n.a2, n.a3, n.ra = regs.a0 or 0, regs.a1 or 0,
            regs.a2 or 0, regs.a3 or 0, regs.ra or 0
        if breakpoints[addr].enabled then breakpoints[addr].callback() end
    end
    -- What the resident allocator and update loop do to one request entry.
    function r:request(slot, id, fields)
        local req = REQUESTS + slot * 0x20
        u8at(req + 0x1c)[0] = 0x80
        u16at(req + 0x18)[0] = id
        for offset, value in pairs(fields or {}) do
            u16at(req + offset)[0] = value % 0x10000
        end
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

-- The case the script exists for: a starter creates effect 8 through
-- DuelEffect_CreateRequest, it runs for a few frames and finishes, and a
-- second effect started directly through the allocator follows in the same
-- request entry.
local duel = capture()
assert(duel:count() == 4, 'four breakpoints')
assert(duel:has('4 breakpoints installed, 0 signature mismatches'))
duel:mode(3)
duel:load(true)
duel:frames(1)
assert(duel:has('duel started; effects are numbered from here'))
duel:call(CONTROL, {})
assert(duel:has('execution breakpoint observed; setup confirmed'))
duel:call(CREATE, { a0 = 8, ra = 0x80026c00 })
duel:call(ALLOCATE, { a0 = 8, ra = 0x8002c6a0 })
local req = duel:request(0, 8, { [0] = 3, [2] = 0xfffe, [4] = 7, [0x12] = 1 })
duel:call(DISPATCH, { a0 = 8, a1 = 0, a3 = req })
for _ = 1, 3 do
    duel:frames(1)
    duel:call(DISPATCH, { a0 = 8, a1 = 0xffffffff, a3 = req })
end
assert(duel:has('#01 t=   0.0s START effect=8 handler=0x801499DC func_80147B18 '
                .. 'phase=0 request=0 payload=(3,-2,7) field_10=0x0000 '
                .. 'variant=1 field_1D=0 flags=0x80 created_by=0x80026C00'))
duel:release(0)
duel:frames(1)
assert(duel:has('#01 t=   0.1s END   effect=8 frames=4 calls=4 phases=0,-1 '
                .. 'reason=finished'))
duel:call(ALLOCATE, { a0 = 0x17, ra = 0x80031000 })
req = duel:request(0, 0x17)
duel:call(DISPATCH, { a0 = 0x17, a1 = 0, a3 = req })
assert(duel:has('#02 t=   0.1s START effect=23 handler=0x80152C20'))
assert(duel:has('created_by=0x80031000'))
-- A row is printed when it happens, not only in the final document.
assert(duel:index('#02 t=') ~= nil and not duel:has('status:'))
duel:mode(1)
duel:frames(1)
assert(duel:has('#02 t=   0.1s END   effect=23 frames=1 calls=1 phases=0 '
                .. 'reason=still running'))
assert(duel:has('status: left the duel'))
assert(duel:has('effect 08 handler=0x801499DC func_80147B18 episodes=#01 '
                .. 'created_by=0x80026C00'))
assert(duel:has('effect 23 handler=0x80152C20 func_80152048 episodes=#02'))
assert(duel:has('execution_breakpoint=observed'))
assert(duel:has('control_hits: 1 dispatch_hits: 5 foreign_module_hits: 0'))
assert(duel:allDisabled())
assert(duel:has('copy everything above into tools/trace/result/duel_effect_ids.txt'))

-- Another module at 0x80146000 must not produce rows, and the verdict says so.
local foreign = capture()
foreign:mode(3)
foreign:load(false)
foreign:frames(1)
foreign:call(CONTROL, {})
foreign:call(DISPATCH, { a0 = 2, a3 = REQUESTS })
foreign:mode(0)
foreign:frames(1)
assert(not foreign:has('START'))
assert(foreign:has('foreign_module_hits: 1'))
assert(foreign:has('verdict: requests ran but the NA duel_effects dispatcher'))

-- A request pointer outside RAM, or unaligned, is refused before any read.
local bad = capture()
bad:mode(3)
bad:load(true)
bad:frames(1)
bad:call(DISPATCH, { a0 = 1, a3 = 0 })
bad:call(DISPATCH, { a0 = 1, a3 = REQUESTS + 2 })
bad:call(DISPATCH, { a0 = 1, a3 = 0x801ffff0 })
assert(bad:has('IMPLAUSIBLE request=0x00000000'))
assert(bad:has('IMPLAUSIBLE request=0x800EAD8A'))
assert(bad:has('IMPLAUSIBLE request=0x801FFFF0'))
assert(not bad:has('START'))

-- A full pool is reported live.
local full = capture()
full:mode(3)
full:frames(1)
for slot = 0, 7 do full:request(slot, 1) end
full:call(ALLOCATE, { a0 = 5, ra = 0x80029000 })
assert(full:has('request pool full, effect=5 caller=0x80029000'))

-- Effects stop being numbered at 40 and the trace ends once they finish.
local many = capture()
many:mode(3)
many:load(true)
many:frames(1)
for i = 1, 41 do
    many:call(ALLOCATE, { a0 = i % 25, ra = 0x80030000 })
    local r = many:request(1, i % 25)
    many:call(DISPATCH, { a0 = i % 25, a3 = r })
    many:release(1)
    many:frames(1)
end
assert(many:has('status: captured 40 effects'))
assert(many:has('#40 '))
assert(not many:has('#41 '))

-- The fixed window: three minutes after the first effect.
local window = capture()
window:mode(3)
window:load(true)
window:frames(1)
local wr = window:request(2, 4)
window:call(DISPATCH, { a0 = 4, a3 = wr })
window:frames(180 * 60 - 1)
assert(not window:has('status:'))
window:frames(1)
assert(window:has('status: three-minute window after the first effect ended'))

-- No control hit after ten minutes in a duel names the setup.
local silent = capture()
silent:mode(3)
silent:frames(600 * 60)
assert(silent:has('ten minutes in a duel without a control hit'))
assert(silent:has('execution_breakpoint=NOT-observed'))

-- A resident signature mismatch is reported in the document.
local wrong = capture({ corrupt = ALLOCATE })
assert(wrong:has('1 signature mismatches'))
wrong:mode(3)
wrong:frames(1)
wrong:mode(0)
wrong:frames(1)
assert(wrong:has('signature_mismatch: 0x8002C604 expected 0x27BDFFE8 read 0x00000000'))

-- A callback error ends the trace with the error instead of failing silently.
local broken = capture()
broken:mode(3)
broken:load(true)
broken:frames(1)
PCSX.getRegisters = function() error('registers unavailable') end
broken:call(DISPATCH, {})
broken:frames(1)
assert(broken:has('status: callback error'))
assert(broken:has('callback_error: '))

io.stdout:write('duel_effect_ids_test: ok\n')
