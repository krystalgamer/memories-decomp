-- Run from the repository root:
--   luajit tools/trace/tests/duel_effect_followups_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/duel_effect_followups.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local REQUESTS = 0x800ead88
local DISPATCH, CONTROL = 0x801462b0, 0x8002c6c8
local ALLOCATE, RETURN = 0x8002c604, 0x8002c784
local ATLAS, SIBLING, LAYERED = 0x80148f98, 0x80148e88, 0x80147e08
local BASE = 0x80100000
local SLOT0, SLOT1 = BASE + 0x2800, BASE + 0x2880
local HANDLER = 0x8014efc4
local RESIDENT = { [CONTROL] = 0x27bdffd8, [ALLOCATE] = 0x27bdffe8,
                   [RETURN] = 0x00002021 }
local MODULE = { [0x801462b0] = 0x27bdffc8, [0x801462b4] = 0xafb7002c,
                 [0x80148f98] = 0x27bdff98, [0x8015013c] = 0x30c60003 }

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local gpr = {}
    for i = 0, 33 do gpr[i] = 0 end
    local registers = { pc = 0, GPR = { r = gpr,
        n = { a0 = 0, a1 = 0, a2 = 0, a3 = 0, ra = 0 } } }
    local events, breakpoints, output = {}, {}, {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u16at(a) return ffi.cast('uint16_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end
    for addr, word in pairs(RESIDENT) do
        u32at(addr)[0] = options.corrupt == addr and 0 or word
    end
    if options.base ~= false then u32at(0x80010000)[0] = BASE end

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
    breakpoints_duel_effect_followups = nil
    listener_duel_effect_followups = nil
    dofile(scriptPath)

    local r = { output = output, breakpoints = breakpoints }
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
    function r:u16(addr, v) u16at(addr)[0] = v % 0x10000 end
    function r:u32(addr, v) u32at(addr)[0] = v end
    function r:call(addr, regs)
        local n = registers.GPR.n
        n.a0, n.a1, n.a2, n.a3, n.ra = regs.a0 or 0, regs.a1 or 0,
            regs.a2 or 0, regs.a3 or 0, regs.ra or 0
        if breakpoints[addr].enabled then breakpoints[addr].callback() end
    end
    -- A store at pc of register rt to base register + imm, as the CPU
    -- would execute it, firing the write watch that covers the address.
    function r:store(watch, pc, word, regs)
        registers.pc = pc
        u32at(pc)[0] = word
        for i, v in pairs(regs) do gpr[i] = v end
        if breakpoints[watch].enabled then breakpoints[watch].callback() end
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

-- The full route: a damage spell, Dark Hole (left to #7872), the jar and the
-- swords, ending once all three parts are covered.
local duel = capture()
assert(duel:count() == 9, 'nine breakpoints')
assert(duel:has('9 breakpoints installed, 0 signature mismatches'))
assert(duel.breakpoints[SLOT0 + 0x50].kind == 'Write')
assert(duel.breakpoints[SLOT1 + 0x50].size == 20)
duel:load(true)
duel:u16(SLOT0 + 0x50 + 2 * 3, 7)
duel:u16(SLOT1 + 0x6a, 1)
duel:mode(3)
duel:frames(1)
assert(duel:has('-- t=   0.0s duel started; effects are numbered from here'))
assert(duel:has('-- t=   0.0s swords at duel start slot0@0x80102800 enabled=0 '
                .. 'count=0 tiles=0,0,0,7,0,0,0,0,0,0'))
assert(duel:has('swords at duel start slot1@0x80102880 enabled=1'))
duel:call(CONTROL, {})
assert(duel:has('execution breakpoint observed; setup confirmed'))

-- Part A: Sparks runs effect 6 and its sibling control.
duel:call(ALLOCATE, {})
duel:call(DISPATCH, { a0 = 6, a1 = 0, a3 = duel:request(0, 6) })
assert(duel:has('#01 t=   0.0s START effect=6 phase=0'))
duel:u16(BASE, 0xfff0)
duel:call(SIBLING, { a0 = 0x800f0000, a1 = 48, a2 = BASE, ra = 0x80155b90 })
assert(duel:has('#01 t=   0.0s A sibling ra=0x80155B90 size=48 index=nil '
                .. 'offset=(-16,0,0)'))
duel:call(SIBLING, { a1 = 48, ra = 0x80155b90 })
assert(duel:countOf('A sibling') == 1, 'a repeated caller prints no row')
duel:call(RETURN, {})
duel:release(0)
duel:frames(1)
assert(duel:has('#01 t=   0.0s END   effect=6 frames=1 calls=1 '
                .. 'helpers=siblingx2 reason=finished'))

-- Part B: Dark Hole (phase 0) null calls are left to
-- duel_effect_null_offset.lua; only the jar (phase 1) is reported.
duel:u16(RAM_BASE, 3)
duel:u16(RAM_BASE + 2, 0xfffe)
duel:call(DISPATCH, { a0 = 17, a1 = 0, a3 = duel:request(1, 17) })
duel:call(LAYERED, { a1 = 0x40, a2 = 5, a3 = 0, ra = 0x80158f00 })
duel:call(LAYERED, { a1 = 0x40, a2 = 5, a3 = 0, ra = 0x80158f00 })
assert(duel:countOf('B null offset') == 0, 'phase 0 belongs to #7872')
duel:call(LAYERED, { a1 = 0x40, a3 = 0x80100100, ra = 0x80158f80 })
duel:release(1)
duel:frames(1)
assert(duel:has('helpers=layeredx1 reason=finished'))
duel:call(DISPATCH, { a0 = 17, a1 = 1, a3 = duel:request(1, 17) })
duel:u16(RAM_BASE + 4, 9)
duel:call(LAYERED, { a1 = 0x20, a3 = 0, ra = 0x80158f00 })
assert(duel:has('#03 t=   0.1s B null offset ra=0x80158F00 phase=1 size=32 '
                .. 'depth=0 reads ram0=(3,-2,9)'))
duel:call(LAYERED, { a1 = 0x20, a3 = 0, ra = 0x80158f00 })
assert(duel:countOf('B null offset') == 1, 'same RAM, same effect: no row')
duel:release(1)
duel:frames(1)

-- Part C: a foreign store into slot 0's tiles before the swords run, then
-- the handler's own stores, then init and the first draw.
-- sh $v0, 0x52($a0): opcode 0x29, base 4, rt 2.
local SH = 0xa4820052
duel:store(SLOT0 + 0x50, 0x80030000, SH, { [4] = SLOT0, [2] = 0x1234 })
assert(duel:has('-- t=   0.1s C tile write pc=0x80030000 addr=0x80102852 '
                .. 'width=2 value=0x1234'))
duel:store(SLOT0 + 0x50, 0x80030000, SH, { [4] = SLOT0, [2] = 1 })
assert(duel:countOf('C tile write') == 1, 'same writer and episode: no row')
duel:call(DISPATCH, { a0 = 21, a1 = 2, a3 = duel:request(2, 21) })
assert(duel:has('#04 t=   0.1s swords before init phase=2 slot0@0x80102800 enabled=0'))
duel:store(SLOT0 + 0x50, HANDLER + 0x40, SH, { [4] = SLOT0, [2] = 5 })
assert(duel:countOf('C tile write') == 1, 'the handler is not a foreign writer')
-- Phase 2 init enables the slot with count 10; a repeat disables it.
duel:u16(SLOT0 + 0x6a, 1)
duel:u16(SLOT0 + 0x64, 10)
duel:call(DISPATCH, { a0 = 21, a1 = 2, a3 = duel:request(2, 21) })
assert(duel:countOf('swords before init') == 1)
assert(duel:has('#04 t=   0.1s swords disable (slot was enabled) slot0'))
duel:call(DISPATCH, { a0 = 21, a1 = -1, a3 = duel:request(2, 21) })
assert(duel:has('#04 t=   0.1s swords first draw slot0@0x80102800 enabled=1'))
duel:call(DISPATCH, { a0 = 21, a1 = -1, a3 = duel:request(2, 21) })
assert(duel:countOf('swords first draw') == 1)
-- A 3-D battle leaves mode 3; that does not end the trace.
duel:mode(0x23 + 1)
duel:frames(1)
assert(not duel:has('status:'))
duel:release(2)
duel:mode(3)
duel:frames(1)
assert(duel:has('-- t=   0.1s back on the duel board'))
assert(duel:has('status: all three parts covered'))
assert(duel:has('effects_seen: 6,17,21'))
assert(duel:has('A verdict: DuelEffect_DrawAtlasColumnQuad never executed '
                .. 'while its sibling control ran 2 times'))
assert(duel:has('B: effect17_phases=0,1 null_offset_calls=2 non_null_calls=1 '
                .. 'phase0_null_calls_left_to_duel_effect_null_offset=2'))
assert(duel:has('C: before_init_rows=1 first_draw_rows=1 '
                .. 'handler_tile_writes=1 other_tile_writers=1'))
assert(duel:has('A caller sibling DuelEffect_DrawFixedTextureQuad '
                .. 'ra=0x80155B90 calls=2 sizes=48 indices=n/a effects=#01/e6'))
assert(duel:has('B null caller ra=0x80158F00 calls=2 effects=#03/e17 '
                .. 'ram0=(3,-2,9)'))
assert(duel:has('C tile writer pc=0x80030000 writes=2 effects=outside'))
assert(duel:allDisabled())
assert(duel:has('copy everything above into '
                .. 'tools/trace/result/duel_effect_followups.txt'))

-- The atlas helper itself, with an index.
local atlas = capture()
atlas:load(true)
atlas:mode(3)
atlas:frames(1)
atlas:call(DISPATCH, { a0 = 9, a3 = atlas:request(0, 9) })
atlas:call(ATLAS, { a0 = 0x10040, a1 = 3, a2 = 0x80100001, ra = 0x80158100 })
assert(atlas:has('#01 t=   0.0s A atlas ra=0x80158100 size=64 index=3 '
                 .. 'offset=unreadable'))
atlas:call(ATLAS, { a0 = 64, a1 = 4, ra = 0x80158100 })
assert(atlas:countOf('A atlas') == 2, 'a new index prints a row')
atlas:frames(360 * 60)
assert(atlas:has('status: six-minute window after the first effect ended'))
assert(atlas:has('A verdict: DuelEffect_DrawAtlasColumnQuad executed 2 times'))
assert(atlas:has('A caller atlas DuelEffect_DrawAtlasColumnQuad ra=0x80158100 '
                 .. 'calls=2 sizes=64 indices=3,4 effects=#01/e9'))
assert(atlas:has('#01 t= 360.0s END   effect=9 frames=21600 calls=1 '
                 .. 'helpers=atlasx2 reason=still running'))

-- Without the sibling the atlas negative is not claimed.
local weak = capture()
weak:load(true)
weak:mode(3)
weak:frames(1)
weak:call(DISPATCH, { a0 = 2, a3 = weak:request(0, 2) })
weak:frames(360 * 60)
assert(weak:has('A verdict: inconclusive; the sibling control never ran'))

-- Another module at the bank addresses: nothing counts.
local foreign = capture()
foreign:load(false)
foreign:mode(3)
foreign:frames(1)
foreign:call(ATLAS, { a0 = 1 })
foreign:call(LAYERED, { a3 = 0 })
foreign:call(DISPATCH, { a0 = 1 })
foreign:mode(0)
foreign:frames(1200 * 60)
assert(foreign:has('foreign_module_hits: 3'))
assert(foreign:has('status: timed out'))
assert(not foreign:has('B null offset'))
-- A store at an address inside the handler range while another module is
-- loaded is still a foreign writer.
local other = capture()
other:load(false)
other:store(SLOT1 + 0x50, HANDLER + 8, 0xa4820052, { [4] = SLOT1, [2] = 1 })
assert(other:has('C tile write pc=0x8014EFCC addr=0x801028D2'))
-- A non-store word at pc is reported as unreadable rather than guessed.
other:store(SLOT1 + 0x50, 0x80031000, 0, {})
assert(other:has('C tile write pc=0x80031000 addr=unreadable width=unreadable '
                 .. 'value=unreadable'))

-- Before boot the base pointer is 0: the retail base is watched, and a
-- different base at duel start is reported.
local early = capture({ base = false })
assert(early.breakpoints[SLOT0 + 0x50] ~= nil)
early:u32(0x80010000, 0x80120000)
early:mode(3)
early:frames(1)
assert(early:has('shared work base is 0x80120000, not the watched 0x80100000'))

-- Implausible request pointers, row caps, silent setup, signature mismatch
-- and callback error.
local bad = capture()
bad:load(true)
bad:mode(3)
bad:frames(1)
bad:call(DISPATCH, { a0 = 1, a3 = 0x7ffffff0 })
assert(bad:has('IMPLAUSIBLE request=0x7FFFFFF0'))
assert(not bad:has('START'))
local busy = capture()
busy:load(true)
busy:mode(3)
busy:frames(1)
for i = 1, 40 do
    busy:call(DISPATCH, { a0 = 17, a1 = 1, a3 = busy:request(i % 8, 17) })
    busy:u16(RAM_BASE, i)
    busy:call(LAYERED, { a3 = 0, ra = 0x80158f00 })
end
assert(busy:countOf('B null offset') == 24)
local silent = capture()
silent:mode(3)
silent:frames(600 * 60)
assert(silent:has('ten minutes in a duel without a control hit'))
assert(silent:has('execution_breakpoint=NOT-observed'))
local wrong = capture({ corrupt = CONTROL })
assert(wrong:has('1 signature mismatches'))
local broken = capture()
broken:load(true)
broken:mode(3)
broken:frames(1)
PCSX.getRegisters = function() error('registers unavailable') end
broken:call(ATLAS, {})
broken:frames(1)
assert(broken:has('status: callback error'))
assert(broken:has('callback_error: '))

io.stdout:write('duel_effect_followups_test: ok\n')

-- Phases 0/1 start count at 0: the first negative call draws nothing, so
-- the snapshot waits for count > 0. A later init after disabling is a
-- real reinit and is logged again.
local low = capture()
low:load(true)
low:mode(3)
low:frames(1)
low:call(DISPATCH, { a0 = 21, a1 = 1, a3 = low:request(0, 21) })
assert(low:has('#01 t=   0.0s swords before init phase=1 slot1@0x80102880 enabled=0'))
low:u16(SLOT1 + 0x6a, 1)
low:call(DISPATCH, { a0 = 21, a1 = -2, a3 = low:request(0, 21) })
assert(low:countOf('swords first draw') == 0, 'count 0 draws nothing')
low:u16(SLOT1 + 0x64, 1)
low:call(DISPATCH, { a0 = 21, a1 = -2, a3 = low:request(0, 21) })
assert(low:has('#01 t=   0.0s swords first draw slot1@0x80102880 enabled=1 count=1'))
low:u16(SLOT1 + 0x64, 2)
low:call(DISPATCH, { a0 = 21, a1 = -2, a3 = low:request(0, 21) })
assert(low:countOf('swords first draw') == 1)
low:call(DISPATCH, { a0 = 21, a1 = 1, a3 = low:request(0, 21) })
assert(low:has('swords disable (slot was enabled) slot1'))
assert(low:countOf('swords before init') == 1, 'disabling is not an init')
low:u16(SLOT1 + 0x6a, 0)
low:call(DISPATCH, { a0 = 21, a1 = 1, a3 = low:request(0, 21) })
assert(low:countOf('swords before init') == 2, 'a reinit is logged')
