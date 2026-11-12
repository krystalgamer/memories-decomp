-- resident_dead_globals_watch.lua
--
-- WHAT THIS ANSWERS
--   A static scan of SLUS_014.11 (every gp-relative and lui-paired load and
--   store, plus every jal and stored code pointer) leaves these resident
--   symbols without a meaningful reader or writer, so none can be named:
--     D_8009B0A4, D_8009B0D4 (s32)  read only by Main_RunFrameServices,
--                                   never written.
--     D_8009B0B0, D_8009B0BC (s32)  written only by Main_RunFrameServices,
--                                   copied from the two above.
--     D_8009B0D1 (u8)    write-only: Main_Init, Main_ApplyMenuSelection and
--                        SaveData_RequestLoad store 0.
--     D_8009B2B2 (u8)    write-only: DebugMenu_Init stores 0x80,
--                        DebugMenu_EnterMappedMode stores 0.
--     D_8009B2DE (u16)   DebugMenu_UpdateBustUpEntry zeroes it, then tests
--                        it; nothing sets it non-zero.
--     D_8009B268 (u8)    stored 0/1 beside menu requests; its one reader,
--                        Main_RunMenu, passes it to MainMenu_InitFrontendMenu
--                        (main_menu overlay), which ignores it.
--     D_8009B4A0..A2 (u8 x3)  written only by func_8005C374, never read.
--     func_8005C374 (movie: copies its three arguments into D_8009B4A0..A2)
--     and func_8003D32C (empty) have no static caller at all.
--   Does any code, including overlays and code that builds the address in
--   a register, read or write these at runtime, with non-zero or changing
--   values, or call the two functions? Each answer names something:
--     * a writer of D_8009B0A4/D_8009B0D4 makes the Main_RunFrameServices
--       pairs real progress counters (and names all four);
--     * a reader of D_8009B0D1, D_8009B2B2 or D_8009B4A0..A2, or a non-zero
--       writer of D_8009B2DE, gives each a role;
--     * a caller of func_8005C374 or func_8003D32C names the function and,
--       for the first, its three bytes;
--     * a qualified silent run records them as vestigial (debug leftovers)
--       in the notes instead.
--
-- WHAT IS WATCHED
--   * Read and Write breakpoints over every symbol above at its declared
--     width (src/game/main.c, debug_menu.c, main_mode_runners.c,
--     movie_pipeline.c). Every access is classified "static" when its pc is
--     one of the 27 load/store sites the static scan found for that symbol,
--     and "UNEXPECTED" otherwise; the UNEXPECTED rows are the answer. Each
--     row has vsync count, game frame (control hits), time, pc, ra, the
--     module at pc (resident, a named overlay matched by signature words, or
--     unidentified plus the code words around pc), the accessed symbol and
--     width PCSX-Redux hands the callback, and the value: current memory for
--     a read, old -> new for a decoded store ("decode=ok" when the
--     instruction at pc agrees with the callback's address and width).
--   * Exec breakpoints on func_8005C374 (a0..a2, ra, caller module) and
--     func_8003D32C (ra, caller module).
--   * Every vsync all symbols are re-read; each change is printed.
--   * Exec on Main_Init marks the boot; control: Exec on
--     Main_RunFrameServices (0x8001306C).
--   Output is bounded: 2 rows per static pc, 4 per unexpected pc/kind, 8
--   per called function, 200 rows in total (live and in the document), 64
--   pc/kind pairs in the final table; everything beyond is still counted.
--   Silence counts as negative evidence only for a qualified capture: a
--   reset was observed, the control fired after it, every US signature word
--   matched, no callback failed and the full window ran. Anything less is
--   reported as inconclusive. The script survives a console reset: it
--   clears its state on reset and reinstalls its breakpoints when the BIOS
--   shell is reached.
--
-- HOW TO RUN (about five minutes)
--   1. Open PCSX-Redux with the game, a memory card holding a campaign save
--      in slot 1, and the debugger enabled. Select the interpreter CPU
--      (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file. It prints "armed".
--   3. Soft-reset the console (Emulation -> Soft reset). It prints
--      "reset observed" and then "breakpoints reinstalled".
--   4. Boot through the intro to the title and the main menu. Choose LOAD
--      (or NEW GAME), reach the campaign map and start a duel; play a turn
--      or two. Do not reset again: a second reset starts a new capture.
--   5. Optional, if you have time left: back on the main menu enable the
--      documented cheat `80184594 000B` (notes/research/gameshark-codes.md)
--      to open the Debug Menu, disable it again, highlight BustUp and
--      confirm, and wait ten seconds. If you can get back to the Debug
--      Menu, confirm MOVIE and let a movie play.
--   6. The document prints by itself five minutes after the reset. Copy it
--      into tools/trace/result/resident_dead_globals_watch.txt and fill in
--      the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Which screens you went through, in order (load or new game, which duel,
--   whether you reached the Debug Menu and which entries you confirmed).
--   For any UNEXPECTED or call row, what was on screen at that time.

local ffi = require('ffi')

local SCRIPT_NAME = 'resident_dead_globals_watch'
-- Breakpoints fire only while the debugger setting is on and the interpreter
-- CPU is the engine actually running. PCSX-Redux reads the debugger flag live,
-- so a script can switch it on itself. The CPU engine is chosen at emulator
-- start and Lua cannot query it: the Dynarec setting only says what the next
-- start will use. The setup therefore counts as confirmed only once one of
-- this script's execution breakpoints has fired. Runs before any breakpoint
-- is installed.
local TRACE_SETUP = (function()
    local okSettings, emulator = pcall(function()
        return PCSX.settings.emulator
    end)
    if not okSettings or emulator == nil then
        return 'unknown: PCSX.settings unavailable; enable the debugger and '
            .. 'select the interpreter CPU by hand'
    end
    local debugger = 'unknown'
    local okDebug, debugOn = pcall(function() return emulator.Debug.Debug end)
    if okDebug and debugOn == true then
        debugger = 'on'
    elseif okDebug and debugOn == false then
        pcall(function() emulator.Debug.Debug = true end)
        local _, now = pcall(function() return emulator.Debug.Debug end)
        debugger = now == true and 'enabled-by-script'
            or 'OFF: enable Configuration -> Emulation -> Debugger'
    end
    local configured = 'unknown'
    local okDynarec, dynarecOn = pcall(function() return emulator.Dynarec end)
    if okDynarec and dynarecOn == false then
        configured = 'interpreter'
    elseif okDynarec and dynarecOn == true then
        configured = 'DYNAREC: breakpoints will not fire; select the '
            .. 'interpreter CPU and restart the emulator'
    end
    return 'debugger=' .. debugger .. ' configured_cpu=' .. configured
        .. ' active_cpu=unverified'
end)()
print(SCRIPT_NAME .. ': setup ' .. TRACE_SETUP)

-- Every breakpoint goes through here so the first real hit can confirm the
-- running engine. The wrapped invoker's return value is passed through.
local traceExecutionObserved = false
local function traceAddBreakpoint(address, kind, width, cause, invoker)
    if invoker == nil then
        return PCSX.addBreakpoint(address, kind, width, cause)
    end
    return PCSX.addBreakpoint(address, kind, width, cause, function(...)
        if not traceExecutionObserved then
            traceExecutionObserved = true
            pcall(print, SCRIPT_NAME .. ': execution breakpoint observed; '
                .. 'setup confirmed')
        end
        return invoker(...)
    end)
end
local function traceExecutionStatus()
    if traceExecutionObserved then
        return 'execution_breakpoint=observed'
    end
    return 'execution_breakpoint=NOT-observed: the setup is unverified and '
        .. 'silence is not evidence'
end

local RAM_BASE = 0x80000000
local RAM_END = 0x80200000
local RESIDENT_TEXT_END = 0x800906d4
local FPS = 60

-- Symbol, address, width (declared C type).
local TARGETS = {
    {'D_8009B0A4', 0x8009b0a4, 4},
    {'D_8009B0B0', 0x8009b0b0, 4},
    {'D_8009B0BC', 0x8009b0bc, 4},
    {'D_8009B0D1', 0x8009b0d1, 1},
    {'D_8009B0D4', 0x8009b0d4, 4},
    {'D_8009B268', 0x8009b268, 1},
    {'D_8009B2B2', 0x8009b2b2, 1},
    {'D_8009B2DE', 0x8009b2de, 2},
    {'D_8009B4A0', 0x8009b4a0, 3},
}

-- Every static load/store of a target in SLUS_014.11: pc, retail word,
-- symbol, kind, containing function.
local KNOWN = {
    {0x80012bd4, 0xa38001c9, 'D_8009B0D1', 'write', 'Main_Init'},
    {0x800130d0, 0x8f8201a8, 'D_8009B0B0', 'read', 'Main_RunFrameServices'},
    {0x800130d4, 0x8f83019c, 'D_8009B0A4', 'read', 'Main_RunFrameServices'},
    {0x800130e8, 0x8f8201b4, 'D_8009B0BC', 'read', 'Main_RunFrameServices'},
    {0x800130ec, 0x8f8301cc, 'D_8009B0D4', 'read', 'Main_RunFrameServices'},
    {0x80013118, 0x8f82019c, 'D_8009B0A4', 'read', 'Main_RunFrameServices'},
    {0x8001311c, 0x8f8401cc, 'D_8009B0D4', 'read', 'Main_RunFrameServices'},
    {0x80013128, 0xaf8201a8, 'D_8009B0B0', 'write', 'Main_RunFrameServices'},
    {0x8001312c, 0xaf8401b4, 'D_8009B0BC', 'write', 'Main_RunFrameServices'},
    {0x8002d460, 0xa3820360, 'D_8009B268', 'write', 'Main_ApplyMenuSelection'},
    {0x8002d4cc, 0xa020b0d1, 'D_8009B0D1', 'write', 'Main_ApplyMenuSelection'},
    {0x8002d54c, 0xa3800360, 'D_8009B268', 'write', 'Main_ApplyMenuSelection'},
    {0x8002d568, 0xa3800360, 'D_8009B268', 'write', 'Main_ApplyMenuSelection'},
    {0x8002d5c0, 0x93840360, 'D_8009B268', 'read', 'Main_RunMenu'},
    {0x8002d794, 0xa3820360, 'D_8009B268', 'write', 'Main_RunGameOver'},
    {0x8002f158, 0xa022b268, 'D_8009B268', 'write', 'Script_OpSavePrompt'},
    {0x8002f218, 0xa022b268, 'D_8009B268', 'write', 'Script_OpSavePrompt'},
    {0x8002fa30, 0xa022b268, 'D_8009B268', 'write', 'Script_OpReturnToMenu'},
    {0x800301ac, 0xa38203aa, 'D_8009B2B2', 'write', 'DebugMenu_Init'},
    {0x80030d34, 0xa022b268, 'D_8009B268', 'write',
     'DebugMenu_UpdateTitleEntry'},
    {0x80030fbc, 0xa38003aa, 'D_8009B2B2', 'write',
     'DebugMenu_EnterMappedMode'},
    {0x80031370, 0xa78003d6, 'D_8009B2DE', 'write',
     'DebugMenu_UpdateBustUpEntry'},
    {0x80031384, 0x978203d6, 'D_8009B2DE', 'read',
     'DebugMenu_UpdateBustUpEntry'},
    {0x8003f7f4, 0xa020b0d1, 'D_8009B0D1', 'write', 'SaveData_RequestLoad'},
    {0x8005c374, 0xa3840598, 'D_8009B4A0', 'write', 'func_8005C374'},
    {0x8005c378, 0xa3850599, 'D_8009B4A0', 'write', 'func_8005C374'},
    {0x8005c37c, 0xa386059a, 'D_8009B4A0', 'write', 'func_8005C374'},
}

local CONTROL = {0x8001306c, 0x27bdffe0, 'Main_RunFrameServices'}
local BOOT = {0x80012b50, 0x27bdffe8, 'Main_Init'}
local CALLS = {
    {0x8005c374, 0xa3840598, 'func_8005C374', true},
    {0x8003d32c, 0x03e00008, 'func_8003D32C', false},
}

-- Overlays that can hold code calling into the resident image (US). Words
-- are from the retail archives (config/slus_01411/overlays.json). The two
-- overworld modules share their code; only their data at +0x22C8 differs.
local OVERLAYS = {
    {'free_duel', 0x80168000, 0x2800,
     {{0x0, 0x13}, {0x4, 0x3c028017}, {0x8, 0x3c06800a}}},
    {'password', 0x80168000, 0x7800,
     {{0x0, 0x15}, {0x4, 0x8149}, {0x8, 0x8168}}},
    {'overworld_before_coup', 0x80168000, 0x3000,
     {{0x0, 0x14}, {0x4, 0x27bdffe0}, {0x22c8, 0x4829}}},
    {'overworld_after_coup', 0x80168000, 0x3000,
     {{0x0, 0x14}, {0x4, 0x27bdffe0}, {0x22c8, 0x1111}}},
    {'overworld', 0x80168000, 0x3000,
     {{0x0, 0x14}, {0x4, 0x27bdffe0}, {0x8, 0xafb10014}}},
    {'main_menu', 0x80180000, 0x8000,
     {{0x0, 0x0f}, {0x4, 0x8018416c}, {0x8, 0x80183514}}},
    {'credits', 0x80180000, 0x8000,
     {{0x0, 0x10}, {0x4, 0x5f5f5f5f}, {0x8, 0x5f5f5f5f}}},
    {'duel_effects', 0x80146000, 0x16000,
     {{0x0, 0x18}, {0x4, 0x1000}, {0x8, 0x1000}}},
}

local WINDOW_FRAMES = 300 * FPS
local MAX_ROWS = 200
local MAX_ROWS_STATIC_PC = 2
local MAX_ROWS_UNEXPECTED_PC = 4
local MAX_ROWS_PER_CALL = 8
local MAX_CHANGE_ROWS = 60
local MAX_PCS = 64

local LOADS = {[0x20] = 1, [0x21] = 2, [0x22] = 4, [0x23] = 4, [0x24] = 1,
               [0x25] = 2, [0x26] = 4, [0x32] = 4}
local STORES = {[0x28] = 1, [0x29] = 2, [0x2a] = 4, [0x2b] = 4, [0x2e] = 4,
                [0x3a] = 4}
local EXACT_STORES = {[0x28] = true, [0x29] = true, [0x2b] = true}

local knownByPc = {}
for _, k in ipairs(KNOWN) do knownByPc[k[1]] = k end

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return type(addr) == 'number' and addr >= RAM_BASE
        and addr + size <= RAM_END
end
local function u8(addr)
    if mem == nil or not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function u32(addr)
    if mem == nil or not inRam(addr, 4) or addr % 4 ~= 0 then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local function readLE(addr, width)
    local value = 0
    for k = width - 1, 0, -1 do
        local b = u8(addr + k)
        if b == nil then return nil end
        value = value * 0x100 + b
    end
    return value
end
local function hexw(v, width)
    if v == nil then return '??' end
    return string.format('%0' .. (width * 2) .. 'X', v)
end

-- Capture state, cleared on reset.
local vsyncs, controlHits, bootEntries = 0, 0, 0
local rows, dropped, suppressed, changeRows = {}, 0, 0, 0
local perPc, pcOrder, untracked = {}, {}, 0
local perTarget = {}
local calls = {}
local counts = {}
local resetSeen, controlAfterReset = false, 0
local snapshot = nil
local callbackError = nil

-- Run state, kept across resets.
local done = false
local resets = 0
local reinstalled = false
local installNote = 'not installed'
local generation = 0

breakpoints_resident_dead_globals_watch = {}

local function resetCaptureState()
    vsyncs, controlHits, bootEntries = 0, 0, 0
    rows, dropped, suppressed, changeRows = {}, 0, 0, 0
    perPc, pcOrder, untracked = {}, {}, 0
    perTarget = {}
    for _, t in ipairs(TARGETS) do
        perTarget[t[1]] = {static_read = 0, static_write = 0,
                           unexpected_read = 0, unexpected_write = 0,
                           nonzero_writes = 0}
    end
    calls = {}
    for _, c in ipairs(CALLS) do calls[c[3]] = 0 end
    counts = {read = 0, write = 0, static = 0, unexpected = 0,
              decode_mismatch = 0, no_address = 0}
    controlAfterReset = 0
    snapshot = nil
    callbackError = nil
end
resetCaptureState()

local function stamp()
    return string.format('vs=%d f=%d t=%6.1fs', vsyncs, controlHits,
                         vsyncs / FPS)
end

local function live(text)
    if #rows < MAX_ROWS then
        rows[#rows + 1] = text
        print(text)
        if #rows == MAX_ROWS then
            print(SCRIPT_NAME .. ': row budget of ' .. MAX_ROWS .. ' reached; '
                  .. 'further rows are only counted')
        end
    else
        dropped = dropped + 1
    end
end

local function overlayMatches(o)
    for _, sig in ipairs(o[4]) do
        if u32(o[2] + sig[1]) ~= sig[2] then return false end
    end
    return true
end

local function residentOverlays()
    local names = {}
    for _, o in ipairs(OVERLAYS) do
        if overlayMatches(o) then
            -- The generic overworld entry only fills in for an unknown
            -- data variant.
            if o[1] ~= 'overworld' or (names[#names] or ''):sub(1, 9)
                ~= 'overworld' then
                names[#names + 1] = o[1]
            end
        end
    end
    return names
end

local function codeWords(pc)
    local parts = {}
    for k = -1, 2 do
        parts[#parts + 1] = hexw(u32(pc + 4 * k), 4)
    end
    return table.concat(parts, ' ')
end

-- The module at pc: resident, a signature-matched overlay, or unidentified
-- with the code words around pc so the row can be matched offline.
local function moduleAt(pc)
    if pc >= RAM_BASE and pc < RESIDENT_TEXT_END then
        local k = knownByPc[pc]
        if k ~= nil then return 'resident(' .. k[5] .. ')' end
        if pc >= CONTROL[1] and pc < CONTROL[1] + 0xe8 then
            return 'resident(' .. CONTROL[3] .. ')'
        end
        return 'resident'
    end
    for _, o in ipairs(OVERLAYS) do
        if pc >= o[2] and pc < o[2] + o[3] and overlayMatches(o) then
            return string.format('%s+0x%X', o[1], pc - o[2])
        end
    end
    return 'unidentified[code ' .. codeWords(pc) .. ']'
end

local function targetAt(addr)
    for _, t in ipairs(TARGETS) do
        if addr >= t[2] and addr < t[2] + t[3] then return t end
    end
    return nil
end

local function describe(addr)
    local t = targetAt(addr)
    if t == nil then return string.format('0x%08X', addr) end
    if addr == t[2] then return t[1] end
    return string.format('%s+%d', t[1], addr - t[2])
end

-- A memory breakpoint fires before the access with pc on the accessing
-- instruction: memory still holds the old bytes and a store's new value is
-- still in its source register.
local function decode(regs, pc)
    local w = u32(pc)
    if w == nil then return nil end
    local op = math.floor(w / 0x4000000)
    local width = LOADS[op] or STORES[op]
    if width == nil then return nil end
    local r = regs.GPR and regs.GPR.r
    if r == nil then return nil end
    local base = tonumber(r[math.floor(w / 0x200000) % 32])
    if base == nil then return nil end
    local imm = w % 0x10000
    if imm >= 0x8000 then imm = imm - 0x10000 end
    local d = {addr = (base + imm) % 0x100000000, width = width}
    if EXACT_STORES[op] then
        local v = tonumber(r[math.floor(w / 0x10000) % 32])
        if v ~= nil then d.value = v % (2 ^ (8 * width)) end
    end
    return d
end

local function onAccess(kind, target, bpAddress, bpWidth)
    local regs = PCSX.getRegisters()
    local pc = tonumber(regs.pc) or 0
    local ra = tonumber(regs.GPR and regs.GPR.n and regs.GPR.n.ra) or 0
    local addr = type(bpAddress) == 'number' and bpAddress or nil
    local width = tonumber(bpWidth)
    local d = decode(regs, pc)
    local decodeNote
    if d == nil then
        decodeNote = 'decode=none'
    elseif addr ~= nil and d.addr == addr and d.width == width then
        decodeNote = 'decode=ok'
    else
        decodeNote = string.format('decode=mismatch(0x%08X/%d)', d.addr,
                                   d.width)
        counts.decode_mismatch = counts.decode_mismatch + 1
        d = nil
    end
    if addr == nil then counts.no_address = counts.no_address + 1 end

    local k = knownByPc[pc]
    local class = (k ~= nil and k[3] == target[1] and k[4] == kind)
        and 'static' or 'UNEXPECTED'
    counts[kind] = counts[kind] + 1
    counts[class == 'static' and 'static' or 'unexpected'] =
        counts[class == 'static' and 'static' or 'unexpected'] + 1
    local tally = perTarget[target[1]]
    local slot = (class == 'static' and 'static_' or 'unexpected_') .. kind
    tally[slot] = tally[slot] + 1

    local detail
    local nonzero = false
    if addr == nil then
        detail = 'at=unknown(callback gave no address) ' .. decodeNote
    else
        width = width or 1
        detail = string.format('at=%s width=%d', describe(addr), width)
        if kind == 'write' then
            local old = readLE(addr, width)
            local new = d and d.value
            detail = detail .. string.format(' %s -> %s', hexw(old, width),
                                             new and hexw(new, width) or '??')
            nonzero = new ~= nil and new ~= 0
        else
            detail = detail .. ' value=' .. hexw(readLE(addr, width), width)
        end
        detail = detail .. ' ' .. decodeNote
    end
    if kind == 'write' and nonzero then
        tally.nonzero_writes = tally.nonzero_writes + 1
    end

    local key = string.format('%s pc=0x%08X %s', kind, pc, target[1])
    local entry = perPc[key]
    if entry == nil then
        if #pcOrder >= MAX_PCS then
            untracked = untracked + 1
            return
        end
        entry = {count = 0, firstVs = vsyncs, firstFrame = controlHits,
                 ra = ra, class = class, module = moduleAt(pc), nonzero = 0}
        perPc[key] = entry
        pcOrder[#pcOrder + 1] = key
    end
    entry.count = entry.count + 1
    entry.lastVs = vsyncs
    if nonzero then entry.nonzero = entry.nonzero + 1 end
    local limit = class == 'static' and MAX_ROWS_STATIC_PC
        or MAX_ROWS_UNEXPECTED_PC
    if entry.count > limit then
        suppressed = suppressed + 1
        return
    end
    live(string.format('%s %s %s pc=0x%08X in=%s ra=0x%08X ra_in=%s %s',
                       stamp(), class, kind, pc, entry.module, ra,
                       moduleAt(ra), detail))
end

local function onCall(c)
    local n = PCSX.getRegisters().GPR.n
    local ra = tonumber(n.ra) or 0
    calls[c[3]] = calls[c[3]] + 1
    if calls[c[3]] > MAX_ROWS_PER_CALL then
        suppressed = suppressed + 1
        return
    end
    local args = ''
    if c[4] then
        args = string.format(' a0=0x%08X a1=0x%08X a2=0x%08X',
                             tonumber(n.a0) or 0, tonumber(n.a1) or 0,
                             tonumber(n.a2) or 0)
    end
    live(string.format('%s CALL %s ra=0x%08X ra_in=%s%s', stamp(), c[3], ra,
                       moduleAt(ra), args))
end

local function takeSnapshot()
    local s = {}
    for _, t in ipairs(TARGETS) do
        local v = readLE(t[2], t[3])
        if v == nil then return nil end
        s[t[1]] = v
    end
    return s
end

local function formatSnapshot(s)
    if s == nil then return 'unreadable' end
    local parts = {}
    for _, t in ipairs(TARGETS) do
        parts[#parts + 1] = t[1] .. '=' .. hexw(s[t[1]], t[3])
    end
    return table.concat(parts, ' ')
end

local function pollChanges()
    local now = takeSnapshot()
    if now == nil then return end
    if snapshot ~= nil then
        local changes = {}
        for _, t in ipairs(TARGETS) do
            local a, b = snapshot[t[1]], now[t[1]]
            if a ~= b then
                changes[#changes + 1] = t[1] .. ' ' .. hexw(a, t[3]) .. '->'
                    .. hexw(b, t[3])
            end
        end
        if #changes > 0 then
            if changeRows < MAX_CHANGE_ROWS then
                changeRows = changeRows + 1
                live(string.format('%s change %s', stamp(),
                                   table.concat(changes, ' ')))
            else
                dropped = dropped + 1
            end
        end
    end
    snapshot = now
end

local function signatureCheck()
    local bad = {}
    local total = 0
    local function check(addr, expected, label)
        total = total + 1
        local word = u32(addr)
        if word ~= expected then
            bad[#bad + 1] = string.format('signature %s 0x%08X expected '
                .. '0x%08X read %s MISMATCH', label, addr, expected,
                hexw(word, 4))
        end
    end
    check(CONTROL[1], CONTROL[2], CONTROL[3])
    check(BOOT[1], BOOT[2], BOOT[3])
    for _, c in ipairs(CALLS) do check(c[1], c[2], c[3]) end
    for _, k in ipairs(KNOWN) do check(k[1], k[2], k[5]) end
    return bad, total
end

local function qualificationGaps(windowDone)
    local bad = signatureCheck()
    local gaps = {}
    if not windowDone then gaps[#gaps + 1] = 'window did not complete' end
    if callbackError ~= nil then gaps[#gaps + 1] = 'a callback failed' end
    if #bad > 0 then gaps[#gaps + 1] = 'US signature mismatch' end
    if not resetSeen then
        gaps[#gaps + 1] = 'no reset observed after arming'
    end
    if controlAfterReset == 0 then
        gaps[#gaps + 1] = 'no Main_RunFrameServices control after the reset'
    end
    return gaps
end

local function callTotal()
    local n = 0
    for _, c in ipairs(CALLS) do n = n + calls[c[3]] end
    return n
end

local function verdict(gaps)
    local found = counts.unexpected + callTotal()
    if controlHits == 0 and counts.read + counts.write == 0
        and callTotal() == 0 then
        return 'nothing fired, not even the Main_RunFrameServices control: '
            .. 'the setup failed (debugger off, dynarec still running, or '
            .. 'breakpoints lost after a reset without a reinstall line); '
            .. 'silence is not evidence'
    elseif #gaps > 0 then
        local text = 'inconclusive: ' .. table.concat(gaps, '; ')
        if found > 0 then
            text = text .. string.format('; unexpected accesses=%d calls=%d '
                .. 'were still seen but the capture is not qualified',
                counts.unexpected, callTotal())
        else
            text = text .. '; silence here is not evidence'
        end
        return text
    elseif found == 0 then
        return string.format('only the statically known accesses and no '
            .. 'call of func_8005C374 or func_8003D32C during %d control '
            .. 'frames of a qualified capture: negative evidence that these '
            .. 'symbols are vestigial on this route', controlAfterReset)
    end
    return string.format('UNEXPECTED accesses=%d and calls=%d '
        .. '(func_8005C374=%d func_8003D32C=%d) observed; see the rows',
        counts.unexpected, callTotal(), calls.func_8005C374,
        calls.func_8003D32C)
end

local function finish(reason, windowDone)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_resident_dead_globals_watch) do
        pcall(function() bp:disable() end)
    end
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<screens in order (load/new game, which duel, debug menu entries')
    print(' confirmed?); what was on screen at any UNEXPECTED or CALL row;')
    print(' did you soft-reset after arming?>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print('breakpoints: ' .. installNote)
    print(string.format('vsyncs: %d control_hits: %d resets: %d '
                        .. 'main_init_entries: %d', vsyncs, controlHits,
                        resets, bootEntries))
    print(string.format('accesses: reads=%d writes=%d static=%d '
                        .. 'unexpected=%d dropped_rows=%d '
                        .. 'repeat_rows_suppressed=%d', counts.read,
                        counts.write, counts.static, counts.unexpected,
                        dropped, suppressed))
    print(string.format('calls: func_8005C374=%d func_8003D32C=%d',
                        calls.func_8005C374, calls.func_8003D32C))
    print(string.format('bounds: rows=%d/%d tracked_pcs=%d/%d '
                        .. 'untracked_pc_accesses=%d decode_mismatch=%d '
                        .. 'no_callback_address=%d', #rows, MAX_ROWS,
                        #pcOrder, MAX_PCS, untracked, counts.decode_mismatch,
                        counts.no_address))
    local gaps = qualificationGaps(windowDone == true)
    print('qualified: ' .. (#gaps == 0 and 'yes'
                            or ('no (' .. table.concat(gaps, '; ') .. ')')))
    print('verdict: ' .. verdict(gaps))
    local bad, total = signatureCheck()
    print(string.format('signatures: %d/%d US words ok', total - #bad, total))
    for _, line in ipairs(bad) do print(line) end
    local overlays = residentOverlays()
    print('overlays_at_end: ' .. (#overlays > 0
        and table.concat(overlays, ' ') or 'none matched'))
    for _, t in ipairs(TARGETS) do
        local p = perTarget[t[1]]
        print(string.format('symbol %s static_reads=%d static_writes=%d '
                            .. 'UNEXPECTED_reads=%d UNEXPECTED_writes=%d '
                            .. 'nonzero_writes=%d', t[1], p.static_read,
                            p.static_write, p.unexpected_read,
                            p.unexpected_write, p.nonzero_writes))
    end
    print('final: ' .. formatSnapshot(takeSnapshot()))
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    print('')
    for _, key in ipairs(pcOrder) do
        local e = perPc[key]
        print(string.format('pc %s %s count=%d nonzero=%d first_vs=%d '
                            .. 'first_f=%d last_vs=%d ra=0x%08X in=%s', key,
                            e.class, e.count, e.nonzero, e.firstVs,
                            e.firstFrame, e.lastVs, e.ra, e.module))
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function guarded(handler)
    return function(...)
        if done then return end
        local ok, err = pcall(handler, ...)
        if not ok and callbackError == nil then callbackError = tostring(err) end
    end
end

local function add(address, kind, width, label, handler)
    local list = breakpoints_resident_dead_globals_watch
    local mine = generation
    list[#list + 1] = traceAddBreakpoint(address, kind, width,
                                         SCRIPT_NAME .. ': ' .. label,
                                         guarded(function(...)
        if mine == generation then handler(...) end
    end))
end

local function installBreakpoints(note)
    generation = generation + 1
    breakpoints_resident_dead_globals_watch = {}
    add(CONTROL[1], 'Exec', 4, 'control ' .. CONTROL[3], function()
        controlHits = controlHits + 1
        if resetSeen then controlAfterReset = controlAfterReset + 1 end
    end)
    add(BOOT[1], 'Exec', 4, BOOT[3], function()
        bootEntries = bootEntries + 1
        live(string.format('%s milestone Main_Init entered; %s', stamp(),
                           formatSnapshot(takeSnapshot())))
    end)
    for _, c in ipairs(CALLS) do
        add(c[1], 'Exec', 4, 'call ' .. c[3], function() onCall(c) end)
    end
    for _, t in ipairs(TARGETS) do
        add(t[2], 'Read', t[3], t[1] .. ' read',
            function(a, w) onAccess('read', t, a, w) end)
        add(t[2], 'Write', t[3], t[1] .. ' write',
            function(a, w) onAccess('write', t, a, w) end)
    end
    installNote = note
    print(string.format('%s: %d breakpoints %s', SCRIPT_NAME,
                        #breakpoints_resident_dead_globals_watch, note))
end

local function poll()
    if done then return end
    if resetSeen and not reinstalled then
        -- No ShellReached arrived (fast boot); the reset is over by now.
        reinstalled = true
        installBreakpoints('reinstalled at the first vsync after reset')
    end
    vsyncs = vsyncs + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    pollChanges()
    if vsyncs >= WINDOW_FRAMES then
        finish(string.format('%d-second window ended', WINDOW_FRAMES / FPS),
               true)
    end
end

listeners_resident_dead_globals_watch = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        local ok, err = pcall(function()
            if done then return end
            -- PCSX-Redux drops Lua breakpoints on reset.
            breakpoints_resident_dead_globals_watch = {}
            generation = generation + 1
            resetCaptureState()
            resets = resets + 1
            resetSeen = true
            reinstalled = false
            installNote = 'lost at reset, not reinstalled'
            print(SCRIPT_NAME .. ': reset observed; state cleared, waiting '
                  .. 'for the BIOS shell')
        end)
        if not ok then callbackError = tostring(err) end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::ShellReached', function()
        local ok, err = pcall(function()
            if done or reinstalled then return end
            reinstalled = true
            installBreakpoints('reinstalled at BIOS shell')
        end)
        if not ok then callbackError = tostring(err) end
    end),
}

installBreakpoints('installed')
snapshot = takeSnapshot()
print(SCRIPT_NAME .. ': now ' .. formatSnapshot(snapshot))
do
    local bad, total = signatureCheck()
    print(string.format('%s: signatures %d/%d US words ok', SCRIPT_NAME,
                        total - #bad, total))
    for _, line in ipairs(bad) do print(SCRIPT_NAME .. ': ' .. line) end
end
print(SCRIPT_NAME .. ': armed. Route: soft-reset the console now; boot to '
      .. 'the main menu, LOAD (or NEW GAME), reach the campaign map and '
      .. 'start a duel, play a turn or two; do not reset again. Optionally '
      .. 'open the Debug Menu (cheat 80184594 000B), confirm BustUp, then '
      .. 'MOVIE. The document '
      .. 'prints by itself ' .. (WINDOW_FRAMES / FPS)
      .. ' seconds after the reset.')
