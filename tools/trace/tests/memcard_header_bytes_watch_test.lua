-- Run from the repository root:
--   luajit tools/trace/tests/memcard_header_bytes_watch_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/memcard_header_bytes_watch.lua'

local RAM_BASE = 0x80000000
local CONTROL, DIALOG = 0x8001306c, 0x8003f454
local REQ_LOAD, REQ_WRITE = 0x8003f7d4, 0x8003f87c
local INIT_OP, UPDATE_OP = 0x8003d300, 0x8003dc1c
local READ_SECTOR, WRITE_SECTOR = 0x80044278, 0x80044380
local FRAME = 0x80210000
local STATE = 0x8009b3cf
local WINDOW = 240 * 60
local WORDS = {
    [CONTROL] = 0x27bdffe0, [INIT_OP] = 0x3c02800f, [UPDATE_OP] = 0x27bdffc8,
    [READ_SECTOR] = 0x27bdffe0, [WRITE_SECTOR] = 0x27bdffe0,
    [DIALOG] = 0x978404f2, [REQ_LOAD] = 0x27bdffe8, [REQ_WRITE] = 0x27bdffe8,
    [0x8003e108] = 0x0c01109e, [0x8003e31c] = 0xa022007e,
    [0x8003e340] = 0xa022007a, [0x8003e380] = 0xa024007f,
    [0x8003e394] = 0x0c0110e0,
}

local function capture(eightMegs)
    -- PCSX-Redux backs 8 MB whatever the setting.
    local memory = ffi.new('uint8_t[?]', 0x800000)
    local r = {}
    for i = 0, 31 do r[i] = 0 end
    local names = {a0 = 4, a1 = 5, a2 = 6, ra = 31}
    local registers = {pc = 0, GPR = {r = r, n = setmetatable({}, {
        __index = function(_, k) return names[k] and r[names[k]] end})}}
    local events, breakpoints, output = {}, {}, {}
    local function word(address, value)
        ffi.cast('uint32_t*', memory + (address % 0x20000000))[0] = value
    end
    for address, value in pairs(WORDS) do word(address, value) end
    print = function(text) output[#output + 1] = tostring(text) end
    local emulator = {Debug = {Debug = true}, Dynarec = false}
    if eightMegs == 'throw' then
        setmetatable(emulator, {__index = function(_, k)
            if k == '8Megs' then error('setting unreadable') end
        end})
        eightMegs = false
    elseif eightMegs == 'missing' then
        eightMegs = false
    else
        emulator['8Megs'] = eightMegs
    end
    PCSX = {
        settings = {emulator = emulator},
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, width, label, callback)
            assert(kind == 'Exec' or kind == 'Read' or kind == 'Write')
            local key = kind .. string.format('%08X', address)
            assert(breakpoints[key] == nil, 'duplicate ' .. key)
            if kind ~= 'Exec' then assert(width == 6, 'footer width') end
            local handle = {enabled = true, callback = callback}
            function handle:disable() self.enabled = false end
            breakpoints[key] = handle
            return handle
        end,
        Events = {createEventListener = function(name, callback)
            events[name] = callback
            return {}
        end},
    }
    breakpoints_memcard_header_bytes_watch = nil
    listeners_memcard_header_bytes_watch = nil
    dofile(scriptPath)

    -- Physical offset the CPU lands on for a frame address.
    local function frameOffset(address)
        local off = address - RAM_BASE
        if not eightMegs then off = off % 0x200000 end
        return off
    end
    local t = {output = output, memory = memory}
    function t:countOf(text)
        local n = 0
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then n = n + 1 end
        end
        return n
    end
    function t:has(text) return self:countOf(text) > 0 end
    function t:installed()
        local n = 0
        for _ in pairs(breakpoints) do n = n + 1 end
        return n
    end
    function t:hasBreakpoint(key) return breakpoints[key] ~= nil end
    function t:frames(n) for _ = 1, n do events['GPU::Vsync']() end end
    function t:reset(shell)
        breakpoints = {}
        events['ExecutionFlow::Reset']()
        if shell ~= false then events['ExecutionFlow::ShellReached']() end
    end
    function t:fire(kind, address, ...)
        local bp = breakpoints[kind .. string.format('%08X', address)]
        assert(bp, 'no ' .. kind .. ' breakpoint at ' .. address)
        if bp.enabled then bp.callback(...) end
    end
    function t:exec(address, ra, a0, a1, a2)
        r[31], r[4], r[5], r[6] = ra or 0, a0 or 0, a1 or 0, a2 or 0
        registers.pc = address
        self:fire('Exec', address, address, 4, 'exec')
    end
    function t:setFrame(index, value)
        memory[frameOffset(FRAME) + index] = value
    end
    function t:frameByte(index) return memory[frameOffset(FRAME) + index] end
    -- A retail-style footer store `sb v0, imm(at)` with at = 0x80210000, or
    -- a load through register 8. The breakpoint fires at `bpBase`.
    function t:footer(kind, pc, index, bpBase, value, ra, base)
        local address = (base or FRAME) + index
        if WORDS[pc] == nil and pc % 0x20000000 < 0x800000 then
            local op = kind == 'write' and 0x28 or 0x24
            word(pc, op * 0x4000000 + 8 * 0x200000 + 2 * 0x10000)
            r[8] = address
        end
        r[1], r[2], r[4], r[31] = FRAME, value or 0, value or 0, ra or 0
        registers.pc = pc
        self:fire(kind == 'write' and 'Write' or 'Read', bpBase, address, 1,
                  'mem')
        if kind == 'write' then
            memory[frameOffset(address)] = value or 0
        end
    end
    function t:word(address, value) word(address, value) end
    function t:byte(address, value) memory[address - RAM_BASE] = value end
    function t:rowLines()
        local n = 0
        for _, line in ipairs(output) do
            if line:sub(1, 3) == 'vs=' then n = n + 1 end
        end
        return n
    end
    function t:allDisabled()
        for _, bp in pairs(breakpoints) do
            if bp.enabled then return false end
        end
        return true
    end
    return t
end

local function finished(t)
    assert(t:countOf('==== TRACE RESULT =====') == 1)
    assert(t:countOf('==== USER CONTEXT ====') == 1)
    assert(t:countOf('script: memcard_header_bytes_watch') == 1)
    assert(t:has('setup: debugger=on configured_cpu=interpreter'))
    assert(t:allDisabled())
    t:frames(WINDOW + 1)
    assert(t:countOf('==== TRACE RESULT =====') == 1, 'document printed once')
    assert(t:countOf('--- end of trace') == 1)
end

-- The retail route: load, then save, both through the card dialog.
local function route(t)
    for _ = 1, 3 do t:exec(CONTROL) end
    t:exec(REQ_LOAD, 0x80180400)
    for _ = 1, 10 do t:exec(DIALOG) end
    t:exec(REQ_WRITE, 0x80180500)
    t:exec(DIALOG)
end

-- 8 MB: the legacy controller runs, reads the sector, stamps the footer,
-- checksums and writes it back. Every step is logged with frame dumps.
do
    local t = capture(true)
    assert(t:has('ram 8MB: the frame is its own memory at 0x80210000'))
    assert(t:hasBreakpoint('Write8021007A') and t:hasBreakpoint('Read8021007A'))
    assert(t:installed() == 10)
    assert(t:has('signatures 13/13 US words ok'))
    assert(t:has('armed. Route: from the main menu LOAD'))
    route(t)
    t:exec(INIT_OP, 0x80181000, 0x101)
    assert(t:has('CALL MemCardWork_InitializeOperation mode=0x01 '
                 .. 'ra=0x80181000 in=unidentified'))
    t:byte(STATE, 0x85)
    t:exec(UPDATE_OP)
    t:exec(UPDATE_OP)
    assert(t:countOf('MemCardWork_UpdateOperation state=85') == 1,
           'unchanged state logs once')
    t:exec(READ_SECTOR, 0x8003e110, 0, FRAME, 3)
    assert(t:has('MemCard_ReqReadSector chan=0 addr=0x80210000 sector=3 '
                 .. 'ra=0x8003E110 in=resident(MemCardWork_UpdateOperation)'))
    assert(t:has('frame before-read footer=00 00 00 00 00 00 xor0..7E=00 '
                 .. 'stored7F=00 checksum=ok'))
    -- The card driver fills the frame over two frames.
    t:frames(1)
    t:setFrame(0, 0x51)
    t:frames(1)
    t:setFrame(0x7f, 0x51)
    t:frames(29)
    assert(not t:has('frame after-read'), 'waits until the frame settles')
    t:frames(2)
    assert(t:has('frame after-read footer=00 00 00 00 00 51 xor0..7E=51 '
                 .. 'stored7F=51 checksum=ok'))
    assert(t:has('frame +00 51 00 00'))
    -- State 8: five footer stores, one checksum-loop load, checksum store.
    t:footer('write', 0x8003e31c, 0x7e, 0x8021007a, 1)
    t:footer('write', 0x8003e340, 0x7a, 0x8021007a, 0xff)
    assert(t:has('controller write pc=0x8003E31C '
                 .. 'in=resident(MemCardWork_UpdateOperation)(store 0x7E) '
                 .. 'ra=0x00000000 at=D_8021007E(0x8021007E) width=1 00 -> 01 '
                 .. 'decode=ok state=85'))
    assert(t:has('at=D_8021007A(0x8021007A) width=1 00 -> FF decode=ok'))
    t:footer('read', 0x8003e348, 0x7a, 0x8021007a)
    assert(t:has('controller read pc=0x8003E348'))
    assert(t:has('(checksum loop)'))
    t:footer('write', 0x8003e380, 0x7f, 0x8021007a, 0xaf)
    assert(t:has('at=gMemCardWork_bFrameChecksum(0x8021007F) width=1 '
                 .. '51 -> AF decode=ok'))
    -- Somebody else reads the footer: the answer.
    t:footer('read', 0x80050000, 0x7b, 0x8021007a, nil, 0x80050100)
    assert(t:has('OTHER read pc=0x80050000 in=resident ra=0x80050100 '
                 .. 'at=D_8021007B(0x8021007B) width=1 value=00 decode=ok'))
    t:exec(WRITE_SECTOR, 0x8003e39c, 0, FRAME, 3)
    assert(t:has('frame write footer=FF 00 00 00 01 AF'))
    t:frames(WINDOW)
    assert(t:has('status: 240-second window ended'))
    assert(t:has('route: SaveData_RequestLoad=1 SaveData_RequestWrite=1 '
                 .. 'MemCardDialog_Update=11'))
    assert(t:has('legacy: MemCardWork_InitializeOperation=1 '
                 .. 'MemCardWork_UpdateOperation=2 MemCard_ReqReadSector=1 '
                 .. 'MemCard_ReqWriteSector=1'))
    assert(t:has('footer: reads=2 writes=3 controller=4 kernel=0 OTHER=1'))
    assert(t:has('qualified: yes'))
    assert(t:has('verdict: legacy controller or footer activity observed '
                 .. '(init=1 update=2 '
                 .. 'read_sector=1 write_sector=1); footer accesses '
                 .. 'controller=4 kernel=0 OTHER=1'))
    assert(t:has('milestone SaveData_RequestLoad ra=0x80180400 '
                 .. 'in=unidentified'))
    assert(t:has('pc read pc=0x80050000 OTHER count=1'))
    finished(t)
end

-- 2 MB (the default): the CPU's 0x80210000 lands on 0x80010000, so the
-- breakpoint sits on the mirror and the frame is read through it; a kernel
-- copy is labelled; main_menu callers are named by signature.
do
    local t = capture(false)
    assert(t:has('ram 2MB: the frame folds onto 0x80010000'))
    assert(t:hasBreakpoint('Write8001007A'))
    assert(not t:hasBreakpoint('Write8021007A'))
    t:word(0x80180000, 0x0f)
    t:word(0x80180004, 0x8018416c)
    t:word(0x80180008, 0x80183514)
    route(t)
    assert(t:has('milestone SaveData_RequestWrite ra=0x80180500 '
                 .. 'in=main_menu+0x500'))
    t:footer('write', 0x00001234, 0x7c, 0x8001007a, 0x42)
    assert(t:has('kernel write pc=0x00001234 in=kernel'))
    assert(t:has('at=D_8021007C(0x8021007C) width=1 00 -> 42 decode=ok'))
    assert(t:frameByte(0x7c) == 0x42)
    -- A pc in the BIOS ROM cannot be decoded and must not be dereferenced.
    t:footer('read', 0xbfc01000, 0x7c, 0x8001007a)
    assert(t:has('kernel read pc=0xBFC01000 in=kernel ra=0x00000000 '
                 .. 'at=D_8021007C(0x8021007C) width=1 value=42 decode=none'))
    -- Resident code reading its own rodata at 0x8001007B is not a frame
    -- access, and does not spoil a negative result.
    t:footer('read', 0x80020000, 0x7b, 0x8001007a, nil, 0, 0x80010000)
    assert(t:has('rodata read pc=0x80020000 in=resident ra=0x00000000 '
                 .. 'at=D_8021007B(0x8001007B)'))
    t:frames(WINDOW)
    assert(t:has('OTHER=0 rodata_mirror=1'))
    assert(t:has('qualified: yes'))
    assert(t:has('verdict: legacy controller or footer activity observed '
                 .. '(init=0 update=0 read_sector=0 write_sector=0); footer '
                 .. 'accesses controller=0 kernel=2 OTHER=0'),
           'kernel footer accesses are not silence')
    assert(t:has('ram: 2MB: the frame folds onto 0x80010000 (resident '
                 .. 'rodata); footer breakpoint 0x8001007A'))
    assert(t:has('final: state=00 mode=00 flags=0000 result=00 callback=00 '
                 .. 'footer=00 00 42 00 00 00'))
    finished(t)
end

-- A qualified retail route where the controller never runs is negative
-- evidence.
do
    local t = capture(true)
    route(t)
    t:frames(WINDOW)
    assert(t:has('qualified: yes'))
    assert(t:has('verdict: the retail save (1) and load (1) requests ran '
                 .. 'through MemCardDialog_Update (11 calls) and neither the '
                 .. 'legacy MemCardWork controller nor its sector requests '
                 .. 'ran'))
    finished(t)
end

-- A read whose frame never changes is still dumped after three seconds.
do
    local t = capture(true)
    route(t)
    t:exec(READ_SECTOR, 0x8003e110, 0, FRAME, 3)
    t:frames(179)
    assert(not t:has('after-read'))
    t:frames(1)
    assert(t:has('frame after-read(unchanged after 3s)'))
    t:exec(READ_SECTOR, 0x8003e110, 0, 0x80200000, 3)
    assert(t:has('note: buffer 0x80200000 is not the frame'))
    t:frames(WINDOW)
    finished(t)
end

-- Bounded output: frame dumps stop at eight, footer repeats at four per pc,
-- rows at 200, and the counts stay exact.
do
    local t = capture(true)
    route(t)
    for _ = 1, 12 do t:exec(WRITE_SECTOR, 0x8003e39c, 0, FRAME, 3) end
    assert(t:countOf('frame write footer=') == 8)
    for _ = 1, 20 do t:footer('read', 0x80060000, 0x7d, 0x8021007a) end
    assert(t:countOf('OTHER read pc=0x80060000') == 4)
    for _ = 1, 4 do
        for i = 1, 200 do
            t:footer('read', 0x80070000 + 4 * i, 0x7d, 0x8021007a)
        end
    end
    assert(t:countOf('row budget of 200 reached') == 1)
    t:frames(WINDOW)
    assert(t:rowLines() == 400, 'two copies of 200 rows')
    assert(t:has('footer: reads=820 writes=0 controller=0 kernel=0 OTHER=820'))
    assert(t:has('frame_dumps=8/8'))
    assert(t:has('tracked_pcs=64/64 untracked_pc_accesses=548'))
    finished(t)
end

-- Missing save, load or dialog: inconclusive, and named.
do
    local t = capture(true)
    for _ = 1, 3 do t:exec(CONTROL) end
    t:exec(REQ_LOAD, 0x80180400)
    t:frames(WINDOW)
    assert(t:has('qualified: no (no SaveData_RequestWrite (the save was not '
                 .. 'captured); MemCardDialog_Update never ran)'))
    assert(t:has('silence here is not evidence'))
    assert(not t:has('negative evidence'))
    finished(t)
end

-- A US signature mismatch voids silence.
do
    local t = capture(true)
    t:word(0x8003e31c, 0xa022007d)
    route(t)
    t:frames(WINDOW)
    assert(t:has('signatures: 12/13 US words ok'))
    assert(t:has('verdict: inconclusive: US signature mismatch'))
    finished(t)
end

-- A callback failure ends the trace.
do
    local t = capture(true)
    route(t)
    PCSX.getRegisters = function() error('boom') end
    t:exec(READ_SECTOR)
    t:frames(1)
    assert(t:has('status: callback error'))
    assert(t:has('callback_error: '))
    assert(t:has('verdict: inconclusive: window did not complete; a callback '
                 .. 'failed'))
    finished(t)
end

-- Nothing fired: the setup is blamed.
do
    local t = capture(true)
    t:frames(WINDOW)
    assert(t:has('verdict: nothing fired, not even the Main_RunFrameServices '
                 .. 'control: the setup failed'))
    assert(t:has('execution_breakpoint=NOT-observed'))
    finished(t)
end

-- A reset keeps the capture and reinstalls; without ShellReached the first
-- vsync reinstalls.
do
    local t = capture(true)
    for _ = 1, 3 do t:exec(CONTROL) end
    t:exec(REQ_WRITE, 0x80180500)
    t:exec(DIALOG)
    t:reset()
    assert(t:has('reset observed; capture kept'))
    assert(t:has('10 breakpoints reinstalled at BIOS shell'))
    t:reset(false)
    assert(t:installed() == 0)
    t:frames(1)
    assert(t:has('10 breakpoints reinstalled at the first vsync after reset'))
    t:exec(REQ_LOAD, 0x80180400)
    t:frames(WINDOW)
    assert(t:has('resets: 2'))
    assert(t:has('route: SaveData_RequestLoad=1 SaveData_RequestWrite=1 '
                 .. 'MemCardDialog_Update=1'))
    assert(t:has('qualified: yes'))
    finished(t)
end

-- An absent or unreadable 8Megs setting is never qualified negative
-- evidence, even over a full witnessed route.
for _, mode in ipairs({'missing', 'throw'}) do
    local t = capture(mode)
    route(t)
    t:frames(WINDOW)
    assert(t:has('ram: unknown (assumed 2MB)'))
    assert(t:has('qualified: no'), mode)
    assert(t:has('RAM size unknown (8Megs setting unreadable)'), mode)
    assert(not t:has('negative evidence'), mode)
    finished(t)
end

print = hostPrint
PCSX = nil
print('memcard_header_bytes_watch_test: all scenarios passed')
