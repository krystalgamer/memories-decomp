-- Run from the repository root:
--   luajit tools/trace/tests/screen_tour_watch_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/screen_tour_watch.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local POOL, RECORD = 0x800efe48, 0x70
local CONTROL, GETTER, STEP = 0x8001306c, 0x80058dc0, 0x8009afa4
local WORDS = { [CONTROL] = 0x27bdffe0, [GETTER] = 0x9382009e }

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local gpr = {}
    for i = 0, 33 do gpr[i] = 0 end
    local registers = { pc = 0, GPR = { r = gpr, n = { ra = 0 } } }
    local events, breakpoints, output = {}, {}, {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u16at(a) return ffi.cast('uint16_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end
    for addr, word in pairs(WORDS) do
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
    breakpoints_screen_tour_watch = nil
    listener_screen_tour_watch = nil
    dofile(scriptPath)

    local r = { breakpoints = breakpoints, output = output }
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
    function r:object(i, o)
        local b = POOL + i * RECORD
        if o == nil then u16at(b + 8)[0] = 0 return end
        u16at(b + 8)[0] = 0xc0
        u32at(b + 0x24)[0] = o.update or 0
        u32at(b + 0x10)[0] = o.kind or 0
        u32at(b + 0x4c)[0] = o.draw or 0
        u16at(b + 0x2c)[0] = (o.f2C or 0) % 0x10000
        u16at(b + 0x2e)[0] = (o.f2E or 0) % 0x10000
        u16at(b + 0x60)[0] = (o.f60 or 0) % 0x10000
        u8at(b + 0x6c)[0] = o.f6C or 0
    end
    function r:call(addr, ra)
        registers.GPR.n = { ra = ra or 0 }
        if breakpoints[addr].enabled then breakpoints[addr].callback() end
    end
    -- A store word at pc with base register 4 and source register 2.
    function r:store(pc, word, base, value)
        registers.pc = pc
        u32at(pc)[0] = word
        gpr[4], gpr[2] = base, value
        if breakpoints[STEP].enabled then breakpoints[STEP].callback() end
    end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do if bp.enabled then return false end end
        return true
    end
    function r:count() local n = 0 for _ in pairs(breakpoints) do n = n + 1 end return n end
    return r
end

local run = capture()
assert(run:count() == 3, 'three breakpoints')
assert(run:has('3 breakpoints installed, 0 signature mismatches'))
assert(run.breakpoints[STEP].kind == 'Write' and run.breakpoints[STEP].size == 4)
run:mode(1)
run:frames(1)
assert(run:has('#01 t=   0.0s SCREEN main_mode=0x01'))
run:call(CONTROL)
assert(run:has('execution breakpoint observed; setup confirmed'))
-- A draw-card slide: f60 sweeps -0x400 -> 0 by its f2C step, then clears
-- update and f6C.
run:object(20, { update = 0x80018c34, kind = 1, draw = 0x00a0a0a0,
                 f2C = 0x100, f60 = -0x400, f6C = 1 })
run:frames(1)
run:object(20, { update = 0x80018c34, kind = 1, draw = 0x00a0a0a0,
                 f2C = 0x100, f60 = -0x300, f6C = 1 })
run:frames(1)
assert(run:has('#01 t=   0.1s tween slot=20 upd=0x80018C34 kind=1 draw=- '
               .. 'f60 -1024->-768'))
run:object(20, { update = 0x80018c34, kind = 1, draw = 0x00a0a0a0,
                 f2C = 0x100, f60 = -0x200, f6C = 0 })
run:frames(1)
assert(run:countOf('f60 ') == 1, 'one row per group and field')
assert(run:has('f6C 1->0'))
run:object(20, { update = 0, kind = 1, f60 = 0 })
run:frames(1)
run:object(20, nil)
-- A fading outline on another screen: f60 counts down by 0x10.
run:mode(5)
run:object(30, { update = 0, kind = 1, draw = 0x8002a9c0, f60 = 0x100 })
run:frames(1)
assert(run:has('#02 t=   0.1s SCREEN main_mode=0x05'))
run:object(30, { update = 0, kind = 1, draw = 0x8002a9c0, f60 = 0xf0 })
run:frames(1)
assert(run:has('#02 t=   0.1s tween slot=30 upd=0x00000000 kind=1 '
               .. 'draw=0x8002A9C0 f60 256->240'))
-- Frame-step bytes: an sb to byte 2, an sw over all four, and an sb to
-- byte 0 that is not counted as a byte 1/2 write.
run:call(GETTER, 0x80070010)
assert(run:has('#02 t=   0.1s getter func_80058DC0 ra=0x80070010 returns=0'))
run:call(GETTER, 0x80070010)
assert(run:countOf('getter func_80058DC0 ra=') == 1)
run:store(0x80071000, 0xa0820002, STEP, 7)       -- sb $v0, 2($a0)
assert(run:has('#02 t=   0.1s write byte2 (0x8009AFA6) pc=0x80071000 '
               .. 'value=7 before=0'))
run:store(0x80071100, 0xac820000, STEP, 0x00030201) -- sw $v0, 0($a0)
assert(run:has('write byte1 (0x8009AFA5) pc=0x80071100 value=2'))
assert(run:has('write byte2 (0x8009AFA6) pc=0x80071100 value=3'))
run:store(0x80071200, 0xa0820000, STEP, 1)       -- sb $v0, 0($a0)
run:frames(480 * 60)
assert(run:has('status: eight-minute tour ended'))
assert(run:has('getter_calls: 2 byte1_writes: 1 byte2_writes: 2 '
               .. 'other_step_control_stores: 1 object_groups: 3'))
assert(run:has('group upd=0x80018C34 kind=1 draw=- objects=1 life=3..3 '
               .. 'screens=#01 f2C=256..256 steps=none f2E=0..0 steps=none '
               .. 'f60=-1024..-512 steps=256 f6C=0..1 steps=-1'))
assert(run:has('group upd=0x00000000 kind=1 draw=0x8002A9C0 objects=1'))
assert(run:has('f60=240..256 steps=-16'))
assert(run:has('write byte2 pc=0x80071000 value=7 screen=#02 count=1'))
assert(run:has('getter func_80058DC0 ra=0x80070010 screen=#02 count=2'))
assert(run:allDisabled())
assert(run:has('copy everything above into '
               .. 'tools/trace/result/screen_tour_watch.txt'))

-- Screen cap, step cap, silent setup, mismatch and callback error.
local hop = capture()
hop:call(CONTROL)
for i = 1, 41 do hop:mode(i) hop:frames(1) end
assert(hop:has('status: visited 40 screens'))
local steps = capture()
steps:call(CONTROL)
for i = 1, 9 do
    steps:object(0, { update = 0x80020000, f60 = i * i })
    steps:frames(1)
end
steps:frames(480 * 60)
assert(steps:has('f60=1..81 steps=3,5,7,9,11,13+'))
local silent = capture()
silent:frames(120 * 60)
assert(silent:has('two minutes without a control hit'))
assert(silent:has('execution_breakpoint=NOT-observed'))
local wrong = capture({ corrupt = GETTER })
assert(wrong:has('1 signature mismatches'))
local broken = capture()
PCSX.getRegisters = function() error('registers unavailable') end
broken:call(GETTER)
broken:frames(1)
assert(broken:has('status: callback error'))
assert(broken:has('callback_error: '))

io.stdout:write('screen_tour_watch_test: ok\n')
