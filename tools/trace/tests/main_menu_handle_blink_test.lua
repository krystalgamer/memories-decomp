-- Run from the repository root:
--   luajit tools/trace/tests/main_menu_handle_blink_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local originalPrint = print
local scriptPath = 'tools/trace/main_menu_handle_blink.lua'

local RAM_BASE, POOL, RECORD = 0x80000000, 0x800EFE48, 0x70
local CONTROL = 0x8001306C
local HANDLES = {0x80184558, 0x8018455C, 0x801845A0, 0x801845A4, 0x801845DC}
local SIGNATURE = {
    [0x8018001C] = 0x3C022E8B, [0x80180088] = 0xAE024558,
    [0x80180104] = 0xAE02455C, [0x80180FD8] = 0x27BDFFC0,
    [0x80181018] = 0xAE0245A0, [0x80181084] = 0xAE0245A4,
    [0x80181F68] = 0x27BDFFC0, [0x80181FA0] = 0xAE0245DC,
}
local HIDE, GAP = 180, 120

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local events, breakpoints, output = {}, {}, {}
    local function w32(address, value)
        ffi.cast('uint32_t*', memory + (address - RAM_BASE))[0] = value
    end
    local function r32(address)
        return tonumber(ffi.cast('uint32_t*', memory + (address - RAM_BASE))[0])
    end
    local function w16(address, value)
        ffi.cast('uint16_t*', memory + (address - RAM_BASE))[0] = value
    end
    local function r16(address)
        return tonumber(ffi.cast('uint16_t*', memory + (address - RAM_BASE))[0])
    end
    if options.signature ~= false then
        for address, value in pairs(SIGNATURE) do w32(address, value) end
    end
    w32(CONTROL, 0x27BDFFE0)
    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        settings = {emulator = {Debug = {Debug = true}, Dynarec = false}},
        getMemPtr = function() return memory end,
        getRegisters = function() return {pc = 0, GPR = {n = {}}} end,
        addBreakpoint = function(address, kind, size, label, callback)
            assert(kind == 'Exec' and size == 4 and address == CONTROL)
            local handle = {enabled = true, callback = callback}
            function handle:disable() self.enabled = false end
            breakpoints[#breakpoints + 1] = handle
            return handle
        end,
        Events = {
            createEventListener = function(name, callback)
                events[name] = callback
                return {}
            end,
        },
    }
    breakpoints_main_menu_handle_blink = nil
    listeners_main_menu_handle_blink = nil
    dofile(scriptPath)
    assert(#breakpoints == 1)

    local r = {output = output, w32 = w32, r32 = r32, w16 = w16, r16 = r16}
    -- An allocated pool entry: flags 0xE8, pool_index, some fields.
    function r:object(slot, flags)
        local address = POOL + slot * RECORD
        w16(address + 0x08, flags or 0xE8)
        memory[address + 0x0A - RAM_BASE] = slot
        w32(address + 0x04, 0x01000000 + slot)
        w32(address + 0x30, 0x00080000 + slot)
        w32(address + 0x3C, 0x00100020)
        w32(address + 0x5C, 0x1234)
        return address
    end
    function r:release(address) w16(address + 0x08, 0) end
    function r:snapshot() return ffi.string(memory, 0x200000) end
    function r:countOf(text)
        local count = 0
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then count = count + 1 end
        end
        return count
    end
    function r:has(text) return self:countOf(text) > 0 end
    function r:frames(count)
        for _ = 1, count do events['GPU::Vsync']() end
    end
    function r:control()
        local bp = breakpoints[1]
        if bp.enabled then bp.callback(CONTROL, 4, 'control') end
    end
    function r:reset() events['ExecutionFlow::Reset']() end
    function r:finishedOnce()
        assert(self:countOf('script: main_menu_handle_blink') == 1)
        assert(self:countOf('==== USER CONTEXT ====') == 1)
        assert(self:countOf('==== TRACE RESULT =====') == 1)
        assert(self:countOf('--- end of trace') == 1)
        assert(not breakpoints[1].enabled)
    end
    return r
end

-- Signature mismatch: valid handles, but nothing is written, ever.
do
    local r = capture({signature = false})
    for i, address in ipairs(HANDLES) do r.w32(address, r:object(i)) end
    local before = r:snapshot()
    r:frames(2000)
    r:control()
    assert(r:snapshot() == before, 'a mismatched overlay must not be written')
    assert(r:has('signature mismatch'))
    assert(not r:has('HIDE '))
    r:frames(14400)
    r:finishedOnce()
    assert(r:has('flag_writes: 0 '))
    assert(r:snapshot() == before)
end

-- Null, unaligned, outside-RAM and non-pool handles are refused untouched.
do
    local r = capture()
    r.w32(HANDLES[1], POOL + 2)
    r.w32(HANDLES[2], 0x80200000)
    r.w32(HANDLES[3], 0x80100000)
    r.w32(HANDLES[4], 0)
    r.w32(HANDLES[5], POOL + 7 * RECORD) -- in the pool, but not allocated
    local before = r:snapshot()
    r:frames(600)
    assert(r:has('REFUSED D_80184558 = 0x800EFE4A: unaligned'))
    assert(r:has('REFUSED D_8018455C = 0x80200000: outside RAM'))
    assert(r:has('REFUSED D_801845A0 = 0x80100000: not a DisplayObject'))
    assert(r:has('REFUSED D_801845DC = 0x800F0158: pool slot not allocated'))
    assert(not r:has('D_801845A4 cleared'), 'a handle that was always null is silent')
    assert(not r:has('HIDE '))
    assert(r:snapshot() == before)
end

-- One blink clears exactly bit 0x40 and restores it; nothing else changes.
do
    local r = capture()
    local object = r:object(3, 0xE8)
    r.w32(HANDLES[1], object)
    local before = r:snapshot()
    r:frames(1)
    assert(r:has('SEEN D_80184558 obj=0x800EFF98 slot=3 x=3 y=8 flags=0x00E8'))
    assert(r:has('HIDE D_80184558 (obj=0x800EFF98) for 3 s'))
    assert(r.r16(object + 8) == 0xA8)
    r:frames(HIDE - 1)
    assert(r.r16(object + 8) == 0xA8, 'still hidden before three seconds')
    r:frames(1)
    assert(r:has('SHOW D_80184558 (obj=0x800EFF98) blink 1/2 done'))
    assert(r:snapshot() == before, 'only the one bit was ever touched')
    r:frames(GAP - 1)
    assert(r:countOf('HIDE D_80184558') == 1, 'a two-second gap between blinks')
    r:frames(1)
    assert(r:countOf('HIDE D_80184558') == 2)
    r:frames(HIDE)
    assert(r:snapshot() == before)
    r:frames(GAP * 3)
    assert(r:countOf('HIDE D_80184558') == 2, 'two blinks per handle')
end

-- The game setting only the bit again mid-blink is re-hidden from either
-- hook, then the original bit comes back.
do
    local r = capture()
    local object = r:object(4, 0xE8)
    r.w32(HANDLES[2], object)
    r:frames(10)
    assert(r.r16(object + 8) == 0xA8)
    r.w16(object + 8, 0xE8) -- game: set renderable again
    r:control()
    assert(r.r16(object + 8) == 0xA8, 'the frame-services hook re-hides')
    r.w16(object + 8, 0xE8)
    r:frames(1)
    assert(r.r16(object + 8) == 0xA8, 'the frame tick re-hides')
    assert(r:countOf('REWRITE D_8018455C') == 1)
    r:frames(HIDE)
    assert(r.r16(object + 8) == 0xE8, 'original bit back')
    assert(r:has('SHOW D_8018455C (obj=0x800F0008) blink 1/2 done; game '
        .. 're-set the bit 2 time(s)'))
end

-- Any other flag change is a foreign write: abandoned with no write.
do
    local r = capture()
    local object = r:object(4, 0xE8)
    r.w32(HANDLES[2], object)
    r:frames(10)
    r.w16(object + 8, 0xAC) -- game: clip-test on, renderable still clear
    local before = r:snapshot()
    r:control()
    r:frames(HIDE + GAP - 1)
    assert(r:snapshot() == before, 'nothing written after a foreign change')
    assert(r:countOf('ABANDON D_8018455C (obj=0x800F0008): flags changed '
        .. '0x00A8 -> 0x00AC') == 1)
    assert(not r:has('SHOW D_8018455C'))
end

-- The reviewer's interleaving: slot A is released and reallocated to a new
-- renderable object, the handle is cleared, and the frame-services hook runs
-- before the next vsync. The replacement must never be written.
for _, handleAfter in ipairs({0, POOL + 30 * RECORD}) do
    local r = capture()
    local a = r:object(5)
    r.w32(HANDLES[3], a)
    r:frames(30)
    assert(r.r16(a + 8) == 0xA8)
    r:release(a)
    r:object(5, 0xE8) -- same address and pool_index, new owner
    r.w32(HANDLES[3], handleAfter)
    local before = r:snapshot()
    r:control()
    assert(r:snapshot() == before, 'the control hook left the replacement alone')
    assert(r:has(string.format('ABANDON D_801845A0 (obj=0x800F0078): handle '
        .. 'changed to 0x%08X; nothing written', handleAfter)))
    r:frames(1)
    assert(r.r16(a + 8) == 0xE8, 'the vsync tick wrote nothing either')
    assert(not r:has('SHOW D_801845A0'))
    r:frames(HIDE)
    assert(r:countOf('ABANDON D_801845A0') == 1)
end

-- Released and reallocated with the handle unchanged but different flags:
-- the flags no longer match our write, so it is a new lifetime.
do
    local r = capture()
    local a = r:object(5)
    r.w32(HANDLES[3], a)
    r:frames(30)
    r:release(a)
    r:object(5, 0xC8)
    local before = r:snapshot()
    r:control()
    r:frames(1)
    assert(r:snapshot() == before)
    assert(r:has('ABANDON D_801845A0 (obj=0x800F0078): flags changed 0x00A8 '
        .. '-> 0x00C8'))
end

-- The overlay changing before the control hook: abandoned, nothing written.
do
    local r = capture()
    local object = r:object(9)
    r.w32(HANDLES[5], object)
    r:frames(5)
    r.w32(0x8018001C, 0)
    local before = r:snapshot()
    r:control()
    assert(r:snapshot() == before)
    assert(r:has('ABANDON D_801845DC (obj=0x800F0238): main_menu overlay '
        .. 'left; nothing written'))
    r:frames(HIDE)
    assert(r:snapshot() == before)
    assert(r:has('signature mismatch'))
end

-- Restoration refused at the end of the blink: no SHOW, no blink counted,
-- and the result says the bit was not restored.
do
    local r = capture()
    local object = r:object(12)
    r.w32(HANDLES[1], object)
    r:frames(HIDE)
    r:release(object)
    local before = r:snapshot()
    r:frames(1)
    assert(r:snapshot() == before)
    assert(r:has('ABANDON D_80184558 (obj=0x800F0388): object no longer live: '
        .. 'pool slot not allocated; nothing written, the original bit was '
        .. 'NOT restored'))
    assert(not r:has('SHOW D_80184558'))
    r:frames(14400)
    r:finishedOnce()
    assert(r:has('D_80184558 0x80184558 blinks=0/2 stopped=0 abandoned=1'))
    assert(r:has('abandoned_blinks: 1 '))
end

-- Bounded output: a handle churning through refused values.
do
    local r = capture()
    for i = 1, 400 do
        r.w32(HANDLES[4], 0x80100001 + i * 4)
        r:frames(1)
    end
    r:frames(14400)
    r:finishedOnce()
    local rows = r:countOf('REFUSED D_801845A4')
    assert(rows == 2 * 119, 'each row prints once live and once in the '
        .. 'document; got ' .. rows)
    assert(r:has('dropped_rows: 281'))
    assert(r:has('status: four-minute window ended'))
    assert(r:has('execution_breakpoint=NOT-observed'))
end

-- Every handle live: all blink in order, then the document prints once and
-- memory is back to the original.
do
    local r = capture()
    for i, address in ipairs(HANDLES) do r.w32(address, r:object(10 + i)) end
    local before = r:snapshot()
    r:control()
    local order = {}
    r:frames(5 * 2 * (HIDE + GAP) + 1)
    for _, line in ipairs(r.output) do
        if line:find('==== USER CONTEXT', 1, true) then break end
        local name = line:match('^#%d+ t=[%d.]+s HIDE (D_%x+)')
        if name then order[#order + 1] = name end
    end
    assert(table.concat(order, ',') == 'D_80184558,D_80184558,D_8018455C,'
        .. 'D_8018455C,D_801845A0,D_801845A0,D_801845A4,D_801845A4,'
        .. 'D_801845DC,D_801845DC', table.concat(order, ','))
    r:finishedOnce()
    assert(r:has('status: every handle blinked twice'))
    assert(r:has('execution_breakpoint=observed'))
    assert(r:has('D_801845DC (Trade): what disappeared?'))
    assert(r:snapshot() == before)
    r:frames(100)
    r:reset()
    r:finishedOnce()
end

-- Window end or reset mid-blink restores the bit before printing once.
do
    local r = capture()
    r:frames(14400 - 10)
    local object = r:object(20)
    r.w32(HANDLES[1], object)
    local before = r:snapshot()
    r:frames(5)
    assert(r.r16(object + 8) == 0xA8)
    r:frames(5)
    r:finishedOnce()
    assert(r:has('STOP D_80184558 (obj=0x800F0708): trace ended; bit restored'))
    assert(r:snapshot() == before)
    r:frames(10)
    assert(r:snapshot() == before, 'nothing is written after the end')

    local q = capture()
    local other = q:object(21)
    q.w32(HANDLES[2], other)
    local start = q:snapshot()
    q:frames(3)
    assert(q.r16(other + 8) == 0xA8)
    q:reset()
    q:finishedOnce()
    assert(q:has('status: reset;'))
    assert(q:has('STOP D_8018455C (obj=0x800F0778): trace ended; bit restored'))
    assert(q:snapshot() == start)
end

print, PCSX = originalPrint, nil
breakpoints_main_menu_handle_blink = nil
listeners_main_menu_handle_blink = nil
io.stdout:write('main_menu_handle_blink_test: ok\n')
