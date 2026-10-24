-- Run from the repository root:
--   luajit tools/trace/tests/model_slot_requests_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/model_slot_requests.lua'

local RAM_BASE = 0x80000000
local SLOTS, SLOT_SIZE = 0x800f2c40, 0xe20
local CONTROL, SET, QUEUE = 0x8001306c, 0x80059284, 0x80058938
local PARTS, ALL = 0x80058838, 0x80059af8
local WORDS = { [CONTROL] = 0x27bdffe0, [SET] = 0x3c03800f,
                [QUEUE] = 0x27bdfff8, [PARTS] = 0x27bdffd8,
                [ALL] = 0x27bdffe0 }
local SP = 0x801ffe00

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local gpr = {}
    for i = 0, 33 do gpr[i] = 0 end
    local registers = { pc = 0, GPR = { r = gpr, n = {} } }
    local events, breakpoints, output = {}, {}, {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end
    for addr, word in pairs(WORDS) do
        u32at(addr)[0] = options.corrupt == addr and 0 or word
    end
    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = { enabled = true, callback = callback, kind = kind }
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
    breakpoints_model_slot_requests = nil
    listener_model_slot_requests = nil
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
    function r:active(slot, on) u8at(SLOTS + slot * SLOT_SIZE + 0xe1f)[0] = on and 1 or 0 end
    function r:request(slot, v) u8at(SLOTS + slot * SLOT_SIZE + 0xe11)[0] = v end
    function r:u8(addr, v) u8at(addr)[0] = v end
    function r:u32(addr, v) u32at(addr)[0] = v end
    function r:call(addr, regs)
        local n = {}
        for k, v in pairs(regs) do n[k] = v end
        n.sp = n.sp or SP
        for _, k in ipairs({'a0', 'a1', 'a2', 'a3', 'ra'}) do n[k] = n[k] or 0 end
        registers.GPR.n = n
        if breakpoints[addr].enabled then breakpoints[addr].callback() end
    end
    -- sb rt, 0xE11(base) at pc, firing the slot's write watch.
    function r:store(slot, pc, value, word)
        registers.pc = pc
        u32at(pc)[0] = word or 0xa0820e11
        gpr[2] = value
        local addr = SLOTS + slot * SLOT_SIZE + 0xe11
        if breakpoints[addr].enabled then breakpoints[addr].callback() end
    end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do if bp.enabled then return false end end
        return true
    end
    function r:count() local n = 0 for _ in pairs(breakpoints) do n = n + 1 end return n end
    return r
end

local run = capture()
assert(run:count() == 8, 'eight breakpoints')
assert(run:has('8 breakpoints installed, 0 signature mismatches'))
assert(run.breakpoints[SLOTS + 0xe11].kind == 'Write')
run:frames(1)
run:call(CONTROL, {})
assert(run:has('execution breakpoint observed; setup confirmed'))
-- A call before any scene is tagged --.
run:call(SET, { a0 = 0, a1 = 3, ra = 0x8004f000 })
assert(run:has('-- t=   0.0s set_draw_mode ra=0x8004F000 slot=0 value=3 '
               .. 'active=--- request_now=0/0/0'))
run:active(0, true)
run:active(2, true)
run:request(2, 4)
run:frames(1)
assert(run:has('#01 t=   0.0s START active=0-2 mode=0 request_now=0/0/4'))
run:call(SET, { a0 = 2, a1 = 3, ra = 0x8004f100 })
assert(run:has('#01 t=   0.0s set_draw_mode ra=0x8004F100 slot=2 value=3'))
-- A direct overlay tint with the selection bit and a part mask.
run:u32(SP + 0x10, 15)
run:u32(SP + 0x14, 0x80100000)
for i = 0, 7 do run:u8(0x80100000 + i, i) end
run:call(QUEUE, { a0 = 0, a1 = 1, a2 = 0x07ff8000, a3 = 0x00102030,
                  ra = 0x80168400 })
assert(run:has('#01 t=   0.0s tint caller=0x80168400 via=direct slot=0 '
               .. 'selection=0x1 start=0080FF:07 end=302010:00 duration=15 '
               .. 'mask=00 01 02 03 04 05 06 07 active=0-2'))
-- Through ForAllParts: the overlay's return address is the caller.
run:call(ALL, { a0 = 2, a1 = 2, ra = 0x80168800 })
run:u32(SP + 0x14, 0)
run:call(QUEUE, { a0 = 2, a1 = 0, ra = 0x80059b40 })
assert(run:has('tint caller=0x80168800 via=ForAllParts(a1=0x2) slot=2 '
               .. 'selection=0x0 start=000000:00 end=000000:00 duration=15 '
               .. 'mask=all'))
-- The front end is consumed; the next direct call is direct again.
run:u32(SP + 0x14, 0x7ffffff0)
run:call(QUEUE, { a0 = 1, ra = 0x80168900 })
assert(run:has('tint caller=0x80168900 via=direct slot=1 selection=0x0'))
assert(run:has('mask=unreadable'))
-- Stores: the first (writer, slot, value) prints; repeats are counted.
run:store(2, 0x80054200, 0)
assert(run:has('#01 t=   0.0s store pc=0x80054200 Model_DrawSlot slot=2 value=0'))
run:store(2, 0x80054200, 0)
assert(run:countOf(' store pc=0x80054200') == 1)
run:store(0, 0x8004cc00, 0x83)
assert(run:has('store pc=0x8004CC00 Model_InitSlot slot=0 value=131'))
run:store(1, 0x80070000, 1, 0xac820e10)
assert(run:has('store pc=0x80070000 unknown slot=1 value=unreadable'))
run:active(0, false)
run:active(2, false)
run:frames(1)
assert(run:has('#01 t=   0.1s END   frames=1 draw_mode_calls=1 tints=3 '
               .. 'reason=no slot active'))
run:active(1, true)
run:frames(300 * 60)
assert(run:has('#02 t=   0.1s START active=-1-'))
assert(run:has('status: five-minute window after the first scene ended'))
assert(run:has('#02 t= 300.0s END'))
assert(run:has('reason=still running'))
assert(run:has('set_draw_mode_calls: 2 tint_requests: 3 selection_bit_set: 1 '
               .. 'draw_mode_stores: 4'))
assert(run:has('set_draw_mode ra=0x8004F000 slot=0 value=3 calls=1'))
assert(run:has('tint caller=0x80168800 via=ForAllParts(a1=0x2) slot=2 calls=1'))
assert(run:has('store pc=0x80054200 Model_DrawSlot slot=2 value=0 count=2'))
assert(run:allDisabled())
assert(run:has('copy everything above into '
               .. 'tools/trace/result/model_slot_requests.txt'))

-- Scene cap.
local many = capture()
many:call(CONTROL, {})
for _ = 1, 12 do
    many:active(0, true)
    many:frames(1)
    many:active(0, false)
    many:frames(1)
end
assert(many:has('status: captured 12 scenes'))
-- Row cap on calls.
local busy = capture()
for i = 1, 150 do busy:call(SET, { a0 = 0, a1 = 3, ra = i * 4 }) end
assert(busy:countOf(' set_draw_mode ra=') == 120)
-- Silent setup, timeout, signature mismatch and callback error.
local silent = capture()
silent:frames(300 * 60)
assert(silent:has('five minutes without a control hit'))
assert(silent:has('execution_breakpoint=NOT-observed'))
local idle = capture()
idle:call(CONTROL, {})
idle:frames(1200 * 60)
assert(idle:has('status: timed out'))
local wrong = capture({ corrupt = QUEUE })
assert(wrong:has('1 signature mismatches'))
local broken = capture()
PCSX.getRegisters = function() error('registers unavailable') end
broken:call(SET, {})
broken:frames(1)
assert(broken:has('status: callback error'))
assert(broken:has('callback_error: '))

io.stdout:write('model_slot_requests_test: ok\n')
