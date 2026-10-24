-- Run from the repository root:
--   luajit tools/trace/tests/duel_effect_textures_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/duel_effect_textures.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local REQUESTS = 0x800ead88
local DISPATCH, CONTROL = 0x801462b0, 0x8002c6c8
local ALLOCATE, RETURN = 0x8002c604, 0x8002c784
local IMAGES, MODES, WORDS = 0x8015a270, 0x8015a4bc, 0x8015b7e0
local VARIANTS = MODES + 0x28
local RESIDENT = { [CONTROL] = 0x27bdffd8, [ALLOCATE] = 0x27bdffe8,
                   [RETURN] = 0x00002021 }
local MODULE = { [0x801462b0] = 0x27bdffc8, [0x801462b4] = 0xafb7002c,
                 [0x80148f98] = 0x27bdff98, [0x8015013c] = 0x30c60003 }

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { pc = 0, GPR = { n = { a0 = 0, a1 = 0, a2 = 0, a3 = 0, ra = 0 } } }
    local events, breakpoints, output = {}, {}, {}
    local function u16at(a) return ffi.cast('uint16_t*', memory + (a - RAM_BASE)) end
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end
    for addr, word in pairs(RESIDENT) do
        u32at(addr)[0] = options.corrupt == addr and 0 or word
    end

    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = { enabled = true, callback = callback, kind = kind,
                             size = size }
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
    breakpoints_duel_effect_textures = nil
    listener_duel_effect_textures = nil
    dofile(scriptPath)

    local r = { output = output, breakpoints = breakpoints }
    function r:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function r:frames(n) for _ = 1, n do events['GPU::Vsync']() end end
    function r:mode(v) u8at(MAIN_MODE)[0] = v end
    function r:load(on)
        for addr, word in pairs(MODULE) do u32at(addr)[0] = on and word or 0x1234 end
        if not on then return end
        -- Slot 3 as the retail data has it, and its built word pair.
        local rec = IMAGES + 3 * 28
        u32at(rec)[0] = 8
        u16at(rec + 4)[0], u16at(rec + 6)[0] = 552, 256
        u16at(rec + 8)[0], u16at(rec + 10)[0] = 16, 64
        u16at(rec + 16)[0], u16at(rec + 18)[0] = 16, 241
        u16at(rec + 20)[0], u16at(rec + 22)[0] = 16, 1
        u16at(MODES + 6)[0] = 1
        local v = VARIANTS + 2 * 28
        u32at(v)[0] = 9
        u16at(v + 4)[0], u16at(v + 6)[0] = 56, 192
        u16at(v + 8)[0], u16at(v + 10)[0] = 24, 28
        u16at(v + 16)[0], u16at(v + 18)[0] = 256, 242
        u16at(v + 20)[0] = 256
    end
    function r:build()
        u16at(WORDS + 12)[0] = 0x0038
        u16at(WORDS + 14)[0] = 0x3c41
    end
    function r:call(addr, regs, access)
        local n = registers.GPR.n
        n.a0, n.a1, n.a2, n.a3, n.ra = regs.a0 or 0, regs.a1 or 0,
            regs.a2 or 0, regs.a3 or 0, regs.ra or 0
        registers.pc = regs.pc or addr
        local bp = breakpoints[addr]
        if bp.enabled then bp.callback(access or addr, 2, 'test') end
    end
    function r:read(base, address, pc, ra)
        self:call(base, { pc = pc, ra = ra }, address)
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

-- The case the script exists for: effect 11 builds the table, reads slot 3
-- and variant 2 while drawing, and finishes; the slot table is dumped once.
local duel = capture()
assert(duel:count() == 7, 'seven breakpoints')
assert(duel.breakpoints[WORDS].kind == 'Read' and duel.breakpoints[WORDS].size == 84)
assert(duel.breakpoints[VARIANTS].size == 140)
assert(duel.breakpoints[IMAGES].size == 588)
assert(duel:has('7 breakpoints installed, 0 signature mismatches'))
duel:mode(3)
duel:load(true)
duel:frames(1)
duel:call(CONTROL, {})
assert(duel:has('execution breakpoint observed; setup confirmed'))
duel:call(ALLOCATE, { a0 = 11 })
local req = duel:request(0, 11)
duel:call(DISPATCH, { a0 = 11, a1 = 2, a3 = req })
-- The init conversion reads every image; those reads are not the effect's.
duel:read(IMAGES, IMAGES + 3 * 28 + 4, 0x80150150, 0x801462f0)
duel:build()
duel:read(VARIANTS, VARIANTS + 2 * 28 + 6, 0x80150200, 0x80146900)
duel:read(WORDS, WORDS + 12, 0x80147000, 0x80146a00)
duel:read(WORDS, WORDS + 14, 0x80147004, 0x80146a00)
duel:call(RETURN, {})
assert(duel:has('#01 t=   0.0s START effect=11 phase=2 variant=0'))
assert(not duel:has('reads image03'), 'init reads are not attributed')
assert(duel:has('#01 t=   0.0s effect=11 reads variant2 pc=0x80150200 '
                .. 'ra=0x80146900 image vram=(56,192) 24x28 halfwords = '
                .. '48x28 texels 8bit, clut vram=(256,242) 256 colors'))
assert(duel:has('#01 t=   0.0s effect=11 reads slot03 pc=0x80147000 '
                .. 'ra=0x80146A00 tpage=0x0038 (x=512 y=256 4bit abr=1) '
                .. 'clut=0x3C41 (x=16 y=241)'))
duel:frames(1)
assert(duel:has('slot table (VRAM coordinates in halfwords)'))
assert(duel:has('slot 03 tpage=0x0038 (x=512 y=256 4bit abr=1) clut=0x3C41 '
                .. '(x=16 y=241) mode=1 image vram=(552,256) 16x64 halfwords '
                .. '= 64x64 texels 4bit, clut vram=(16,241) 16 colors'))
assert(duel:has('variant 2 image vram=(56,192)'))
-- After the dispatcher returned, a read belongs to no effect.
duel:read(WORDS, WORDS + 12, 0x80160000, 0x80160100)
assert(duel:has('-- t=   0.0s effect=none reads slot03 pc=0x80160000'))
duel:call(DISPATCH, { a0 = 11, a1 = 0xffffffff, a3 = req })
duel:read(WORDS, WORDS + 12, 0x80147000, 0x80146a00)
duel:release(0)
duel:frames(1)
assert(duel:has('#01 t=   0.1s END   effect=11 frames=2 calls=2 '
                .. 'reads=variant2x1,slot03x3 reason=finished'))
duel:mode(1)
duel:frames(1)
assert(duel:has('status: left the duel'))
assert(duel:has('slot03 used_by=#01/e11,outside pcs=0x80147000,0x80160000'))
assert(duel:has('variant2 used_by=#01/e11 pcs=0x80150200'))
assert(duel:has('table_reads: 5'))
assert(not duel:has('verdict: the slot table was never built'))
assert(duel:allDisabled())
assert(duel:has('copy everything above into tools/trace/result/duel_effect_textures.txt'))

-- Reads while another module occupies the bank addresses are ignored.
local foreign = capture()
foreign:mode(3)
foreign:load(false)
foreign:frames(1)
foreign:call(DISPATCH, { a0 = 1, a3 = REQUESTS })
foreign:read(WORDS, WORDS, 0x80150000, 0)
foreign:mode(0)
foreign:frames(1)
assert(foreign:has('foreign_module_hits: 1'))
assert(foreign:has('table_reads: 0'))
assert(foreign:has('verdict: the slot table was never built'))

-- Implausible request pointers are refused before any read.
local bad = capture()
bad:mode(3)
bad:load(true)
bad:frames(1)
bad:call(DISPATCH, { a0 = 1, a3 = 0 })
bad:call(DISPATCH, { a0 = 1, a3 = 0x801ffff0 })
assert(bad:has('IMPLAUSIBLE request=0x00000000'))
assert(bad:has('IMPLAUSIBLE request=0x801FFFF0'))
assert(not bad:has('START'))

-- Live read rows stop at 160 while counts continue.
local many = capture()
many:mode(3)
many:load(true)
many:frames(1)
for i = 1, 40 do
    many:call(ALLOCATE, { a0 = i % 25 })
    local r = many:request(1, i % 25)
    many:call(DISPATCH, { a0 = i % 25, a3 = r })
    for slot = 0, 4 do many:read(WORDS, WORDS + slot * 4, 0x80147000, 0) end
    many:release(1)
    many:frames(1)
end
local readRows = 0
for _, line in ipairs(many.output) do
    if line:find(' reads slot', 1, true) then readRows = readRows + 1 end
end
assert(readRows == 160 * 2, 'live rows are capped and repeated in the document')
assert(many:has('status: captured 40 effects'))
assert(many:has('table_reads: 200'))

-- The fixed window and the silent-setup diagnosis.
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

-- Signature mismatches and callback errors reach the document.
local wrong = capture({ corrupt = RETURN })
assert(wrong:has('1 signature mismatches'))
local broken = capture()
broken:mode(3)
broken:load(true)
broken:frames(1)
PCSX.getRegisters = function() error('registers unavailable') end
broken:call(DISPATCH, {})
broken:frames(1)
assert(broken:has('status: callback error'))
assert(broken:has('callback_error: '))

io.stdout:write('duel_effect_textures_test: ok\n')
