-- Run from the repository root:
--   luajit tools/trace/tests/model_slot_state_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/model_slot_state.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local SLOTS, SLOT_SIZE = 0x800f2c40, 0xe20
local GATE_FIRST, GATE_SECOND, AFA6 = 0x8009b07b, 0x8009b07c, 0x8009afa6
local A = {
    choice = 0x80050f24, anim = 0x80057af4, angle = 0x8005a618,
    angleRet = 0x8005a6a0, setFirst = 0x8005f180, setSecond = 0x8005f198,
    toSlots = 0x80059f18, request = 0x80059ebc, start = 0x80052d2c,
    status = 0x80058dd8, mode = 0x80059284, getter = 0x80058dc0,
    search = 0x8005a3d0, searchRet = 0x8005a460, frame = 0x8001306c,
    battle = 0x8004eb00,
}
local WORDS = {
    [A.choice] = 0x27bdffd8, [A.anim] = 0x27bdffd0, [A.angle] = 0x27bdffe8,
    [A.angleRet] = 0x03e00008, [A.setFirst] = 0xa3840173,
    [A.setSecond] = 0xa3840174, [A.toSlots] = 0x27bdffd8,
    [A.request] = 0x27bdffe0, [A.start] = 0x27bdffc0, [A.status] = 0x000410c0,
    [A.mode] = 0x3c03800f, [A.getter] = 0x9382009e, [A.search] = 0x90830e17,
    [A.searchRet] = 0x03e00008, [A.frame] = 0x27bdffe0, [A.battle] = 0x27bdffc0,
}

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local n = { a0 = 0, a1 = 0, a2 = 0, a3 = 0, v0 = 0, ra = 0 }
    local registers = { GPR = { n = n }, pc = 0 }
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
    breakpoints_model_slot_state = nil
    listener_model_slot_state = nil
    dofile(scriptPath)

    local r = { u8at = u8at, u32at = u32at, breakpoints = breakpoints }
    function r:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function r:frames(count) for _ = 1, count do events['GPU::Vsync']() end end
    function r:call(name, regs)
        for k, v in pairs(regs or {}) do n[k] = v % 0x100000000 end
        local address = A[name] or name
        if breakpoints[address].enabled then breakpoints[address].callback() end
    end
    function r:write(pc)
        registers.pc = pc
        if breakpoints[AFA6].enabled then breakpoints[AFA6].callback() end
    end
    function r:slot(index, fields)
        for offset, value in pairs(fields) do
            u8at(SLOTS + index * SLOT_SIZE + offset)[0] = value
        end
    end
    function r:count() local c = 0 for _ in pairs(breakpoints) do c = c + 1 end return c end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do if bp.enabled then return false end end
        return true
    end
    return r
end

-- A battle: func_80050F24 picks an animation, asks for a camera move and sets
-- the first gate; a later request is suppressed with both gates at 1.
local battle = capture()
assert(battle:count() == 17, 'sixteen execution breakpoints and one write')
assert(battle.breakpoints[AFA6].kind == 'Write')
assert(battle:has('17 breakpoints installed, 0 signature mismatches'))
battle:call('frame')
battle:slot(1, { [0xdfe] = 2, [0xe0e] = 7 })
battle:call('choice', { a0 = 1, ra = 0x8004f150 })
battle:call('anim', { a0 = 1, a1 = 5, a2 = 1, ra = 0x80050f70 })
battle:call('toSlots', { a0 = 1, a1 = -1, a2 = 0, a3 = 90, ra = 0x80050fd0 })
battle:call('setFirst', { a0 = 2, ra = 0x80051000 })
battle.u8at(GATE_FIRST)[0] = 1
battle.u8at(GATE_SECOND)[0] = 1
battle:call('request', { a0 = 3, ra = 0x8004f300 })
battle:call('angle', { a0 = 1, ra = 0x8004f148 })
battle:call('angleRet', { v0 = 0xc00 })
battle:frames(10801)
assert(battle:has('status: three quiet minutes without a new row'))
assert(battle:has('execution_breakpoint=observed'))
assert(battle:has('=== func_80050F24 slot=1 DFE=2 E0E=7 E11=0 E14=0x00 E1F=0 '
                  .. 'caller=0x8004F150 gates=0,0 main=0x03'))
assert(battle:has('  from func_80050F24: Model_ControlSlotAnimation slot=1 '
                  .. 'animation=5 arg=1'))
assert(battle:has('  from func_80050F24: Model_RequestCameraMoveToSlots '
                  .. 'args=1,-1,0,90 caller=0x80050FD0 gates=0,0 main=0x03 passed'))
assert(battle:has('  from func_80050F24: set-first func_8005F180 value=2'))
assert(battle:has('Model_RequestCameraMove args=3,-1,0,90 caller=0x8004F300 '
                  .. 'gates=1,1 main=0x03 SUPPRESSED'))
assert(battle:has('func_8005A618 caller=0x8004F148 slot=1 DFE=2'))
assert(battle:has('returns=3072 (0xC00 of 0x1000)'))
assert(battle:has('camera_requests_passed: 1 suppressed: 1'))
assert(battle:allDisabled())

-- Animation calls from anywhere else are not attributed.
local other = capture()
other:call('frame')
other:call('anim', { a0 = 0, a1 = 3, a2 = 1, ra = 0x8004f000 })
other:frames(18000)
assert(not other:has('Model_ControlSlotAnimation slot=0'))

-- Status, E11, AFA6 and the hierarchy search.
local slots = capture()
slots:call('frame')
slots:frames(1)
slots:slot(0, { [0xe14] = 0xff, [0xe1f] = 1 })
slots:call('status', { a0 = 0, ra = 0x8002a000 })
slots:slot(0, { [0xe14] = 0 })
slots:call('status', { a0 = 0, ra = 0x8002a000 })
slots:call('status', { a0 = 7, ra = 0x8002a000 })
slots:call('mode', { a0 = 2, a1 = 3, ra = 0x8004f400 })
slots:slot(2, { [0xe11] = 3 })
slots:frames(1)
slots:write(0x80055000)
slots.u8at(AFA6)[0] = 4
slots:frames(1)
slots:call('getter', { ra = 0x80056000 })
local model = SLOTS
local entries = 0x80100000
slots.u32at(model + 0xd14)[0] = entries
slots.u8at(model + 0xe17)[0] = 4
slots.u32at(entries + 2 * 0x50 + 0x4c)[0] = entries + 0x50
slots:call('search', { a0 = model, a1 = entries + 0x50, ra = 0x8004cc00 })
slots:call('searchRet', { v0 = 2 })
slots:call('search', { a0 = 0x00001234, a1 = 0, ra = 0x8004cc00 })
slots:call('searchRet', { v0 = 0 })
slots:frames(10801)
assert(slots:has('func_80058DD8 caller=0x8002A000 slot=0 DFE=0 E0E=0 E11=0 '
                 .. 'E14=0xFF E1F=1 returns=1'))
assert(slots:has('E14=0x00 E1F=1 returns=2'))
assert(slots:has('slot=7 (out of range) returns=out-of-range'))
assert(slots:has('func_80059284 caller=0x8004F400 value=3 before: slot=2'))
assert(slots:has('poll slot=2 E11 0->3'))
assert(slots:has('write D_8009AFA6 pc=0x80055000 before=0'))
assert(slots:has('poll D_8009AFA6 0->4'))
assert(slots:has('func_80058DC0 caller=0x80056000 returns=4'))
assert(slots:has('func_8005A3D0 caller=0x8004CC00 model=0x800F2C40 count=4 '
                 .. 'parent=1 (super not-a-unit) returns=2 (super 1)'))
assert(slots:has('IMPLAUSIBLE model=0x00001234'))

-- A return with no entry is ignored.
local orphan = capture()
orphan:call('frame')
orphan:call('angleRet', { v0 = 5 })
orphan:frames(18000)
assert(not orphan:has('func_8005A618 caller'))

-- Per-key caps hold inside the callback and every hit is counted.
local busy = capture()
busy:call('frame')
for _ = 1, 200 do
    busy:call('setSecond', { a0 = 1, ra = 0x80056000 })
    busy:call('setSecond', { a0 = 0, ra = 0x80056010 })
end
busy:frames(10801)
assert(busy:has('count set-second func_8005F198 value=1 caller=0x80056000 200'))
assert(busy:has('row=006 '))
assert(not busy:has('row=007 '))

-- Bounded output.
local flood = capture()
for i = 1, 130 do flood:call('start', { a0 = i, ra = 0x80050000 + i * 4 }) end
flood:frames(1)
assert(flood:has('status: captured 110 rows'))
assert(flood:has('rows: 110'))

-- A wrong word is named; no control is a setup failure.
local corrupt = capture({ corrupt = A.searchRet })
assert(corrupt:has('1 signature mismatches'))
local silent = capture()
silent:frames(18000)
assert(silent:has('status: five minutes without a control hit'))
assert(silent:has('execution_breakpoint=NOT-observed'))

print = hostPrint
print('model_slot_state: all seven callback-replay cases passed')
