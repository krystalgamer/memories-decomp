-- Run from the repository root:
--   luajit tools/trace/tests/model_slot_dc0_watch_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local originalPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/model_slot_dc0_watch.lua'

local RAM_BASE = 0x80000000
local CONTROL, TINT_TARGET, DRAW_SLOT = 0x8001306c, 0x80059590, 0x800540b4
local SLOTS, SLOT_SIZE, DC0, ACTIVE = 0x800f2c40, 0xe20, 0xdc0, 0xe1f
local FILL_STORE, INIT_SLOT = 0x8005b608, 0x8004cb0c
local BATTLE, CALLER_SP = 0x8004eb00, 0x801ff000

local function storeWord(op, base, rt, imm)
    return op * 0x4000000 + base * 0x200000 + rt * 0x10000 + imm % 0x10000
end

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local gpr = {n = {}, r = {}}
    local registers = {pc = 0, GPR = gpr}
    local events, breakpoints, output = {}, {}, {}
    local function word(address, value)
        ffi.cast('uint32_t*', memory + (address - RAM_BASE))[0] = value
    end
    local function byte(address, value)
        memory[address - RAM_BASE] = value
    end
    word(CONTROL, options.badControl and 0 or 0x27bdffe0)
    word(TINT_TARGET, 0x000410c0)
    word(DRAW_SLOT, 0x27bdf738)
    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        settings = {emulator = {Debug = {Debug = true}, Dynarec = false}},
        getMemPtr = function() return memory end,
        getRegisters = function()
            if options.brokenRegisters then return nil end
            return registers
        end,
        addBreakpoint = function(address, kind, size, label, callback)
            assert(breakpoints[address] == nil)
            assert((kind == 'Exec' and size == 4)
                or (kind == 'Write' and size == 8))
            local handle = {enabled = true, callback = callback, kind = kind}
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
    breakpoints_model_slot_dc0_watch = nil
    listeners_model_slot_dc0_watch = nil
    dofile(scriptPath)

    local r = {output = output, byte = function(_, ...) byte(...) end}
    function r:has(text) return self:countOf(text) > 0 end
    function r:countOf(text)
        local count = 0
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then count = count + 1 end
        end
        return count
    end
    function r:frames(count)
        for _ = 1, count do events['GPU::Vsync']() end
    end
    function r:reset() events['ExecutionFlow::Reset']() end
    function r:active(slot, on)
        byte(SLOTS + slot * SLOT_SIZE + ACTIVE, on and 1 or 0)
    end
    function r:dc0(slot, k, value)
        byte(SLOTS + slot * SLOT_SIZE + DC0 + k, value)
    end
    function r:call(address, values, force)
        for _, name in ipairs({'a0', 'a1', 'a2', 'a3', 'ra', 'sp'}) do
            gpr.n[name] = values[name] or 0
        end
        local bp = assert(breakpoints[address])
        if bp.enabled or force then bp.callback() end
    end
    -- Fires the slot's write breakpoint for one store instruction at pc,
    -- then performs the store, as the emulator would.
    function r:store(slot, pc, op, k, value, ra, force)
        local base = SLOTS + slot * SLOT_SIZE
        word(pc, op and storeWord(op, 4, 5, DC0 + k) or 0)
        registers.pc = pc
        gpr.r[4], gpr.r[5], gpr.n.ra = base, value, ra or 0
        local bp = assert(breakpoints[base + DC0])
        if bp.enabled or force then bp.callback() end
        local width = ({[0x28] = 1, [0x29] = 2, [0x2b] = 4})[op or 0] or 0
        for b = 0, width - 1 do
            byte(base + DC0 + k + b, math.floor(value / 2 ^ (8 * b)) % 0x100)
        end
    end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do
            if bp.enabled then return false end
        end
        return true
    end
    local count, writes = 0, 0
    for _, bp in pairs(breakpoints) do
        count = count + 1
        if bp.kind == 'Write' then writes = writes + 1 end
    end
    assert(count == 5 and writes == 3, 'two exec and three write breakpoints')
    return r
end

local function stopped(r, status)
    assert(r:has('status: ' .. status), status)
    assert(r:countOf('script: model_slot_dc0_watch') == 1)
    assert(r:has('==== USER CONTEXT'))
    assert(r:allDisabled())
    r:frames(60)
    r:reset()
    r:call(CONTROL, {}, true)
    r:call(TINT_TARGET, {}, true)
    r:store(0, DRAW_SLOT + 0x100, 0x28, 3, 4, 0, true)
    assert(r:countOf('==== TRACE RESULT') == 1)
    assert(r:countOf('--- end of trace') == 1,
        'termination emits one final document, even after late callbacks')
end

-- A full scene: flag changes, colour stores, sw decoding, ramps, tint calls.
do
    local r = capture()
    r:call(CONTROL, {})
    r:frames(3)
    assert(not r:has('#01 '))
    r:active(0, true)
    r:active(2, true)
    r:frames(1)
    assert(r:has('#01 t=   0.1s START active=0-2 mode=0 '
        .. 'dc0=000000:00:000000:00|'))

    r:store(0, DRAW_SLOT + 0x100, 0x28, 3, 2)
    assert(r:has('#01 t=   0.1s byte3 slot=0 00 -> 02 by Model_DrawSlot '
        .. 'pc=0x800541B4'))
    r:store(0, DRAW_SLOT + 0x100, 0x28, 3, 2)
    assert(r:countOf('byte3 slot=0') == 1, 'unchanged stores print no row')

    r:store(2, DRAW_SLOT + 0x200, 0x2b, 4, 0x05112233)
    assert(r:has('#01 t=   0.1s colour slot=2 bytes=4..6 by Model_DrawSlot '
        .. 'pc=0x800542B4 store=00->33 before=000000:00:000000:00'))
    assert(r:has('byte7 slot=2 00 -> 05 by Model_DrawSlot'))
    assert(r:countOf('colour slot=2') == 1, 'one colour row per scene writer')

    r:store(1, FILL_STORE, 0x2b, 0, 0, INIT_SLOT + 0x40)
    assert(r:has('store pc=0x8005B608 Model_FillWords<Model_InitSlot ')
        == false, 'summary rows only appear in the final document')
    r:store(0, BATTLE + 0x40, 0x29, 0, 0x4040)
    r:store(0, 0x80100000, nil, 0, 0)

    -- The stores and three stepped frames form one change episode; two
    -- stable frames close it.
    r:frames(1)
    for v = 2, 6, 2 do
        r:dc0(0, 4, v)
        r:frames(1)
    end
    r:frames(2)
    assert(r:has('#01 t=   0.1s ramp slot=2 000000:00:000000:00 -> '
        .. '000000:00:332211:05 frames=1'))
    assert(r:has('#01 t=   0.2s ramp slot=0 000000:00:000000:00 -> '
        .. '404000:02:060000:00 frames=4'))

    local args = {a0 = 1, a1 = 5, a2 = 0x80, a3 = 0x80, ra = BATTLE + 0x80,
                  sp = CALLER_SP}
    r:byte(CALLER_SP + 0x10, 0x80)
    r:call(TINT_TARGET, args)
    r:call(TINT_TARGET, args)
    assert(r:countOf('tint_target caller=Model_UpdateAnimatedBattle'
        .. '(0x8004EB80) slot=1 mode=5 rgb=128/128/128') == 1)

    r:active(0, false)
    r:active(2, false)
    r:frames(1)
    assert(r:has('#01 t=   0.2s END   frames=7 tint_target_calls=2 '
        .. 'flag_changes=2 reason=no slot active'))

    r:reset()
    stopped(r, 'reset; reload the script and start again')
    assert(r:has('tint_target_calls: 2 dc0_stores: 6 undecoded_stores: 1'))
    assert(r:has('tint_target caller=Model_UpdateAnimatedBattle(0x8004EB80) '
        .. 'slot=1 mode=5 rgb=128/128/128 calls=2'))
    assert(r:has('store pc=0x8005B608 Model_FillWords<Model_InitSlot slot=1 '
        .. 'byte=0 count=1 changed=0 values=00'))
    assert(r:has('store pc=0x800541B4 Model_DrawSlot slot=0 byte=3 count=2 '
        .. 'changed=1 values=02'))
    assert(r:has('store pc=0x8004EB40 Model_UpdateAnimatedBattle slot=0 '
        .. 'byte=1 count=1 changed=1 values=40'))
    assert(r:has('store pc=0x80100000 unknown slot=0 byte=? width=? count=1 '
        .. 'changed=0 values=undecoded'))
    assert(r:has('setup: '))
end

-- Eight scenes stop the run; an open ramp closes with the scene.
do
    local r = capture()
    r:call(CONTROL, {})
    for n = 1, 8 do
        r:active(1, true)
        r:frames(1)
        if n == 8 then
            r:dc0(1, 0, 9)
            r:frames(1)
        end
        r:active(1, false)
        r:frames(1)
        assert(r:has(string.format('#%02d ', n)))
    end
    assert(r:has('#08 t=   0.3s ramp slot=1 000000:00:000000:00 -> '
        .. '090000:00:000000:00 frames=1 open'))
    stopped(r, 'captured 8 scenes')
end

-- Window, missing control, timeout, callback error and signature mismatch.
do
    local r = capture()
    r:call(CONTROL, {})
    r:active(0, true)
    r:frames(240 * 60)
    assert(not r:has('script: model_slot_dc0_watch'))
    r:frames(1)
    stopped(r, 'four-minute window after the first scene ended')
    assert(r:has('#01 ') and r:has('END   ') and r:has('reason=still running'))
end
do
    local r = capture()
    r:frames(300 * 60)
    stopped(r, 'five minutes without a control hit')
end
do
    local r = capture()
    r:call(CONTROL, {})
    r:frames(900 * 60)
    stopped(r, 'timed out')
end
do
    local r = capture({brokenRegisters = true})
    r:call(TINT_TARGET, {})
    r:frames(1)
    stopped(r, 'callback error')
    assert(r:has('callback_error: '))
end
do
    local r = capture({badControl = true})
    assert(r:has('1 signature mismatches'))
    r:reset()
    stopped(r, 'reset')
    assert(r:has('signature_mismatch: control Main_RunFrameServices '
        .. '0x8001306C expected 0x27BDFFE0 read 0x00000000'))
end

print = originalPrint
print('model_slot_dc0_watch: all cases passed')
