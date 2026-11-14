-- Run from the repository root:
--   luajit tools/trace/tests/main_menu_handle_blink_test.lua
-- Replays callbacks with mock PCSX events, Exec and Write breakpoints and
-- real LuaJIT FFI memory. Game stores are emulated as real store
-- instructions so the script's decoder sees what the interpreter would.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local originalPrint = print
local scriptPath = 'tools/trace/main_menu_handle_blink.lua'

local RAM_BASE, POOL, RECORD = 0x80000000, 0x800EFE48, 0x70
local CONTROL, PROBE, PROBE_STORE = 0x8001306C, 0x8009AF08, 0x8001310C
local ACQUIRE, RELEASE, MOVE, RESET_POOL =
    0x800400AC, 0x8004020C, 0x800402A0, 0x80040390
local HANDLES = {0x801845A0, 0x801845DC}
local WORDS = {
    [0x8018001C] = 0x3C022E8B, [0x80180088] = 0xAE024558,
    [0x80180104] = 0xAE02455C, [0x80180FD8] = 0x27BDFFC0,
    [0x80181018] = 0xAE0245A0, [0x80181084] = 0xAE0245A4,
    [0x80181F68] = 0x27BDFFC0, [0x80181FA0] = 0xAE0245DC,
}
local RESIDENT = {
    [CONTROL] = 0x27BDFFE0, [ACQUIRE] = 0x00803821, [RELEASE] = 0x97820508,
    [MOVE] = 0x27BDFFE0, [RESET_POOL] = 0x00001821,
    [PROBE_STORE] = 0xAF820000,
}
local REG = {zero = 0, v0 = 2, a0 = 4, a1 = 5, t0 = 8, t1 = 9, gp = 28, ra = 31}
local GAME_PC = 0x80041000 -- a store site outside every lifecycle function
local HIDE, GAP = 180, 120

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local events, breakpoints, output = {}, {}, {}
    local function w32(address, value)
        ffi.cast('uint32_t*', memory + (address - RAM_BASE))[0] = value
    end
    local function w16(address, value)
        ffi.cast('uint16_t*', memory + (address - RAM_BASE))[0] = value
    end
    local function r16(address)
        return tonumber(ffi.cast('uint16_t*', memory + (address - RAM_BASE))[0])
    end
    if options.signature ~= false then
        for address, value in pairs(WORDS) do w32(address, value) end
    end
    if options.resident ~= false then
        for address, value in pairs(RESIDENT) do w32(address, value) end
    end
    w32(PROBE, 0x3C)
    local gpr = {r = {}, n = {}}
    for i = 0, 31 do gpr.r[i] = 0 end
    local registers = {pc = 0, GPR = gpr}
    local function set(name, value)
        gpr.r[REG[name]] = value
        gpr.n[name] = value
    end
    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        settings = {emulator = {Debug = {Debug = true}, Dynarec = false}},
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, width, label, callback)
            if kind == 'Write' and options.noWrite then return nil end
            local handle = {address = address, kind = kind, width = width,
                enabled = true, callback = callback}
            function handle:disable() self.enabled = false end
            function handle:remove() self.removed = true end
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
    probe_main_menu_handle_blink = nil
    flagwatch_main_menu_handle_blink = nil
    dofile(scriptPath)

    local r = {output = output, w32 = w32, w16 = w16, r16 = r16,
        breakpoints = breakpoints}
    function r:object(slot, flags)
        local address = POOL + slot * RECORD
        w16(address + 0x08, flags or 0xE8)
        memory[address + 0x0A - RAM_BASE] = slot
        w32(address + 0x04, 0x01000000 + slot)
        w32(address + 0x30, 0x00080000 + slot)
        return address
    end
    function r:snapshot() return ffi.string(memory, 0x200000) end
    function r:countOf(text)
        local count = 0
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then count = count + 1 end
        end
        return count
    end
    function r:has(text) return self:countOf(text) > 0 end
    function r:exec(address, values)
        for name, value in pairs(values or {}) do set(name, value) end
        for _, bp in ipairs(breakpoints) do
            if bp.kind == 'Exec' and bp.address == address and bp.enabled
                and not bp.removed then
                bp.callback(address, 4, 'exec')
            end
        end
    end
    -- One store instruction at `site`: fires overlapping Write breakpoints
    -- with pc = site + 4 (or site with pcAtStore), then writes memory.
    function r:store(site, size, address, value, opts)
        opts = opts or {}
        local op = ({[1] = 0x28, [2] = 0x29, [4] = 0x2B})[size]
        if not opts.keepWord then
            w32(site, op * 0x4000000 + REG.t0 * 0x200000 + REG.t1 * 0x10000)
        end
        set('t0', address)
        set('t1', value)
        registers.pc = opts.pcAtStore and site or site + 4
        for _, bp in ipairs(breakpoints) do
            if bp.kind == 'Write' and bp.enabled and not bp.removed
                and bp.address < address + size and address < bp.address + bp.width then
                bp.callback(address, size, 'write')
            end
        end
        if size == 1 then
            memory[address - RAM_BASE] = value % 0x100
        elseif size == 2 then
            w16(address, value % 0x10000)
        else
            w32(address, value)
        end
    end
    function r:control() self:exec(CONTROL) end
    function r:probe()
        set('gp', PROBE)
        set('v0', 0x3C)
        registers.pc = PROBE_STORE + 4
        for _, bp in ipairs(breakpoints) do
            if bp.kind == 'Write' and bp.enabled and not bp.removed
                and bp.address == PROBE then
                bp.callback(PROBE, 4, 'write')
            end
        end
        w32(PROBE, 0x3C)
    end
    function r:vsync(count)
        for _ = 1, count or 1 do events['GPU::Vsync']() end
    end
    -- One game frame: frame services entry, its per-frame store, vsync.
    function r:frames(count)
        for _ = 1, count do
            self:control()
            self:probe()
            self:vsync()
        end
    end
    function r:release(object, ra)
        self:exec(RELEASE, {a0 = object, ra = ra or 0x80012345})
        self:store(RELEASE + 0x88, 2, object + 8, 0, {keepWord = false})
    end
    function r:acquire(object, flags)
        self:exec(ACQUIRE, {a0 = (object - POOL) / RECORD, a1 = 2})
        self:store(ACQUIRE + 0x90, 2, object + 8, 0xC0)
        self:store(GAME_PC, 2, object + 8, flags or 0xE8)
    end
    function r:reset() events['ExecutionFlow::Reset']() end
    function r:flagWatches()
        local count = 0
        for _, bp in ipairs(breakpoints) do
            if bp.kind == 'Write' and bp.address ~= PROBE and bp.enabled
                and not bp.removed then count = count + 1 end
        end
        return count
    end
    function r:finishedOnce()
        assert(self:countOf('script: main_menu_handle_blink') == 1)
        assert(self:countOf('==== USER CONTEXT ====') == 1)
        assert(self:countOf('==== TRACE RESULT =====') == 1)
        assert(self:countOf('--- end of trace') == 1)
        for _, bp in ipairs(breakpoints) do
            assert(not bp.enabled or bp.removed, 'every breakpoint is off')
        end
    end
    return r
end

-- A handle that is live and blinkable after the witnesses arm (frame 1
-- arms, frame 2 hides).
local function blinking(slot, handle)
    local r = capture()
    local object = r:object(slot)
    r.w32(HANDLES[handle or 1], object)
    r:frames(2)
    assert(r.r16(object + 8) == 0xA8, 'hidden on the second frame')
    return r, object
end

-- Signature mismatch: valid handles, but nothing is written, ever.
do
    local r = capture({signature = false})
    for i, address in ipairs(HANDLES) do r.w32(address, r:object(i)) end
    local before = r:snapshot()
    r:frames(2000)
    assert(r:snapshot() == before, 'a mismatched overlay must not be written')
    assert(r:has('signature mismatch'))
    assert(not r:has('HIDE '))
    r:frames(10800)
    r:finishedOnce()
    assert(r:has('flag_writes: 0 '))
    assert(r:snapshot() == before)
end

-- Witnesses not armed: read-only, no writes at all, and the result says so.
for _, case in ipairs({
    {options = {noWrite = true}, reason = 'Write breakpoints unavailable'},
    {options = {resident = false}, reason = 'resident lifecycle code words'},
    {options = {}, reason = 'no execution breakpoint has fired yet',
        noControl = true, noProbe = true},
    {options = {}, reason = 'Main_RunFrameServices has not fired yet',
        noControl = true},
    {options = {}, reason = 'write probe has not fired yet', noProbe = true},
}) do
    local r = capture(case.options)
    for i, address in ipairs(HANDLES) do r.w32(address, r:object(i)) end
    local before = r:snapshot()
    for _ = 1, 600 do
        if not case.noControl then r:control() end
        if not case.noProbe then r:probe() end
        r:vsync()
    end
    assert(r:snapshot() == before, case.reason)
    assert(r:has('SEEN D_801845A0'), 'handles are still logged')
    assert(not r:has('HIDE '))
    r:vsync(10800)
    r:finishedOnce()
    assert(r:has('mode: READ-ONLY (' .. case.reason), case.reason)
    assert(r:has('flag_writes: 0 '))
end

-- Null, unaligned, outside-RAM and non-pool handles are refused untouched.
for _, case in ipairs({
    {POOL + 2, 0x80200000, 'REFUSED D_801845A0 = 0x800EFE4A: unaligned',
        'REFUSED D_801845DC = 0x80200000: outside RAM'},
    {0x80100000, POOL + 7 * RECORD,
        'REFUSED D_801845A0 = 0x80100000: not a DisplayObject',
        'REFUSED D_801845DC = 0x800F0158: pool slot not allocated'},
}) do
    local r = capture()
    r.w32(HANDLES[1], case[1])
    r.w32(HANDLES[2], case[2])
    local before = r:snapshot()
    r:frames(600)
    assert(r:has(case[3]), case[3])
    assert(r:has(case[4]), case[4])
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
    assert(not r:has('witnesses confirmed'))
    assert(r:has('SEEN D_801845A0 obj=0x800EFF98 slot=3 x=3 y=8 flags=0x00E8'))
    assert(r.r16(object + 8) == 0xE8, 'no write before the probe has fired')
    r:frames(1)
    assert(r:has('witnesses confirmed; blinking enabled'))
    assert(r:has('HIDE D_801845A0 (obj=0x800EFF98) for 3 s'))
    assert(r.r16(object + 8) == 0xA8)
    assert(r:flagWatches() == 1, 'a Write witness for the blink')
    r:frames(HIDE - 1)
    assert(r.r16(object + 8) == 0xA8, 'still hidden before three seconds')
    r:frames(1)
    assert(r:has('SHOW D_801845A0 (obj=0x800EFF98) blink 1/2 done'))
    assert(r:flagWatches() == 0, 'the witness goes with the blink')
    assert(r:snapshot() == before, 'only the one bit was ever touched')
    r:frames(GAP - 1)
    assert(r:countOf('HIDE D_801845A0') == 1, 'a two-second gap')
    r:frames(1)
    assert(r:countOf('HIDE D_801845A0') == 2)
    r:frames(HIDE + 1)
    assert(r:snapshot() == before)
    r:frames(GAP * 3)
    assert(r:countOf('HIDE D_801845A0') == 2, 'two blinks per handle')
end

-- The game setting the bit again is re-hidden at the next frame services.
do
    local r, object = blinking(4, 2)
    r:store(GAME_PC, 2, object + 8, 0xE8)
    assert(r:countOf('REWRITE D_801845DC: store at 0x80041000') == 1)
    r:control()
    assert(r.r16(object + 8) == 0xA8, 're-hidden')
    r:vsync()
    r:frames(HIDE)
    assert(r.r16(object + 8) == 0xE8, 'original bit back')
    assert(r:has('SHOW D_801845DC (obj=0x800F0008) blink 1/2 done; game '
        .. 're-set the bit 1 time(s)'))
end

-- A benign change of another bit, with no lifecycle event, is the same
-- allocation: re-hide keeps the bit, restore sets 0x40 back next to it.
for _, pcAtStore in ipairs({false, true}) do
    local r, object = blinking(4, 2)
    r:store(GAME_PC, 2, object + 8, 0xEC, {pcAtStore = pcAtStore})
    r:frames(1)
    assert(r.r16(object + 8) == 0xAC)
    r:store(GAME_PC, 1, object + 8, 0xEC, {pcAtStore = pcAtStore})
    r:frames(1)
    assert(r.r16(object + 8) == 0xAC, 'byte store setting the bit, re-hidden')
    r:frames(HIDE)
    assert(r.r16(object + 8) == 0xEC, 'restored beside the game bit')
    assert(r:has('SHOW D_801845DC (obj=0x800F0008) blink 1/2 done'))
    assert(not r:has('ABANDON'))
end

-- The reviewer's ABA: release A, reacquire the same address/index, game
-- sets flags back to 0xE8, the handle never changes. The control hook and
-- the vsync tick must write nothing, and no SHOW appears.
do
    local r, object = blinking(5, 1)
    r:release(object)
    r:acquire(object, 0xE8)
    assert(r.r16(object + 8) == 0xE8)
    local before = r:snapshot()
    r:control()
    assert(r:snapshot() == before, 'the control hook wrote nothing')
    r:vsync()
    r:frames(GAP - 2)
    assert(r:snapshot() == before, 'nothing written on the replacement')
    assert(r:countOf('ABANDON D_801845A0 (obj=0x800F0078): lifetime changed: '
        .. 'DisplayObject_Release of the slot; nothing written') == 1)
    assert(not r:has('SHOW D_801845A0'))
    assert(not r:has('STOP D_801845A0'))
    -- After the gap the replacement is a new lifetime and may blink anew.
    r:frames(2)
    assert(r:countOf('HIDE D_801845A0') == 2)
end

-- The same ABA seen only through the Write witness (a direct free from an
-- unknown site, then a direct reallocation): still abandoned, no write.
do
    local r, object = blinking(5, 1)
    r:store(GAME_PC, 2, object + 8, 0)
    r:store(GAME_PC + 8, 2, object + 8, 0xE8)
    local before = r:snapshot()
    r:frames(GAP - 1)
    assert(r:snapshot() == before)
    assert(r:has('ABANDON D_801845A0 (obj=0x800F0078): lifetime changed: '
        .. 'store at 0x80041000 cleared ALLOCATED'))
end

-- Reallocation plus a cleared handle before the next vsync, and an
-- AcquireSlot of the index alone: abandoned, the replacement untouched.
for _, variant in ipairs({'clear', 'acquire'}) do
    local r, object = blinking(5, 1)
    if variant == 'clear' then
        r:release(object)
        r:acquire(object, 0xE8)
        r.w32(HANDLES[1], 0)
    else
        r:exec(ACQUIRE, {a0 = 5, a1 = 2})
    end
    local before = r:snapshot()
    r:control()
    r:vsync()
    assert(r:snapshot() == before, variant)
    assert(r:countOf('ABANDON D_801845A0') == 1, variant)
end

-- A bulk ResetPool frees every slot.
do
    local r, object = blinking(9, 2)
    r:exec(RESET_POOL)
    local before = r:snapshot()
    r:frames(GAP - 1)
    assert(r:snapshot() == before)
    assert(r:has('ABANDON D_801845DC (obj=0x800F0238): lifetime changed: '
        .. 'DisplayObject_ResetPool freed every slot'))
end

-- A write the decoder cannot explain is treated as a new lifetime.
do
    local r, object = blinking(9, 2)
    r.w32(GAME_PC, 0) -- nop at the reported pc
    r:store(GAME_PC, 2, object + 8, 0xA8, {keepWord = true})
    local before = r:snapshot()
    r:frames(2)
    assert(r:snapshot() == before)
    assert(r:has('could not be decoded'))
end

-- MoveToListHead releases and restores the same allocation: not a new
-- lifetime, so the blink completes and is restored.
do
    local r, object = blinking(9, 2)
    r:exec(MOVE, {a0 = object, a1 = 3})
    r:exec(RELEASE, {a0 = object, ra = MOVE + 0x18})
    r:store(RELEASE + 0x88, 2, object + 8, 0)
    r:store(MOVE + 0xB0, 2, object + 8, 0xA8)
    r:frames(HIDE)
    assert(r.r16(object + 8) == 0xE8)
    assert(r:has('SHOW D_801845DC'))
    assert(not r:has('ABANDON'))
end

-- Handle changed or overlay left with the lifetime intact: the same
-- allocation gets its bit back (STOP), nothing else is written.
for _, how in ipairs({'handle', 'overlay'}) do
    local r = capture()
    local object, other = r:object(9), r:object(10)
    r.w32(HANDLES[2], object)
    local before = r:snapshot()
    r:frames(5)
    assert(r.r16(object + 8) == 0xA8)
    if how == 'handle' then
        r.w32(HANDLES[2], other)
        before = r:snapshot()
        r.w16(object + 8, 0xA8)
    else
        r.w32(0x8018001C, 0)
        before = r:snapshot()
    end
    r:control()
    assert(r.r16(object + 8) == 0xE8, how)
    assert(r:has('STOP D_801845DC (obj=0x800F0238): '
        .. (how == 'handle' and 'handle changed to 0x800F02A8'
            or 'main_menu overlay left') .. '; bit restored'))
    r.w16(object + 8, 0xA8)
    assert(r:snapshot() == before, how .. ': only that bit was written')
end

-- Bounded output: a handle churning through refused values.
do
    local r = capture()
    for i = 1, 400 do
        r.w32(HANDLES[1], 0x80100001 + i * 4)
        r:frames(1)
    end
    r:frames(10800)
    r:finishedOnce()
    local rows = r:countOf('REFUSED D_801845A0')
    assert(rows == 2 * 118, 'each row prints once live and once in the '
        .. 'document; got ' .. rows)
    assert(r:has('dropped_rows: 282'))
    assert(r:has('status: three-minute window ended'))
    assert(r:has('execution_breakpoint=observed'))
end

-- Every handle live: all blink in order, then the document prints once and
-- memory is back to the original.
do
    local r = capture()
    for i, address in ipairs(HANDLES) do r.w32(address, r:object(10 + i)) end
    local before = r:snapshot()
    r:frames(#HANDLES * 2 * (HIDE + 1 + GAP) + 10)
    local order = {}
    for _, line in ipairs(r.output) do
        if line:find('==== USER CONTEXT', 1, true) then break end
        local name = line:match('^#%d+ t=[%d.]+s HIDE (D_%x+)')
        if name then order[#order + 1] = name end
    end
    assert(table.concat(order, ',') == 'D_801845A0,D_801845A0,'
        .. 'D_801845DC,D_801845DC', table.concat(order, ','))
    r:finishedOnce()
    assert(r:has('status: every handle blinked twice'))
    assert(r:has('mode: blinking (lifecycle and write witnesses confirmed)'))
    assert(r:has('D_801845DC (Trade): what disappeared?'))
    assert(r:snapshot() == before)
    r:frames(100)
    r:reset()
    r:finishedOnce()
end

-- Window end mid-blink: restored at the next frame services, printed once.
do
    local r = capture()
    r:frames(10800 - 10)
    local object = r:object(20)
    r.w32(HANDLES[1], object)
    local before = r:snapshot()
    r:frames(5)
    assert(r.r16(object + 8) == 0xA8)
    r:frames(6)
    r:finishedOnce()
    assert(r:has('STOP D_801845A0 (obj=0x800F0708): three-minute window ended; '
        .. 'bit restored'))
    assert(r:snapshot() == before)
    r:frames(10)
    assert(r:snapshot() == before, 'nothing is written after the end')
end

-- Window end with frame services stalled: abandoned after the grace, with
-- no write from the vsync context.
do
    local r = capture()
    r:frames(10800 - 10)
    local object = r:object(20)
    r.w32(HANDLES[1], object)
    r:frames(5)
    local before = r:snapshot()
    r:vsync(40)
    r:finishedOnce()
    assert(r:snapshot() == before)
    assert(r:has('ABANDON D_801845A0 (obj=0x800F0708): Main_RunFrameServices '
        .. 'did not run to restore it'))
end

-- Reset mid-blink: a lifetime change, so nothing is written.
do
    local r, object = blinking(21, 2)
    local before = r:snapshot()
    r:reset()
    r:finishedOnce()
    assert(r:has('status: reset;'))
    assert(r:has('ABANDON D_801845DC (obj=0x800F0778): lifetime changed: '
        .. 'console reset'))
    assert(r:snapshot() == before)
end

print, PCSX = originalPrint, nil
breakpoints_main_menu_handle_blink = nil
listeners_main_menu_handle_blink = nil
probe_main_menu_handle_blink = nil
flagwatch_main_menu_handle_blink = nil
io.stdout:write('main_menu_handle_blink_test: ok\n')
