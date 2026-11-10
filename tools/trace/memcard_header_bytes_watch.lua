-- memcard_header_bytes_watch.lua
--
-- WHAT THIS ANSWERS
--   The legacy memory-card controller MemCardWork_UpdateOperation
--   (0x8003DC1C, src/game/mem_card.c) reads one 128-byte card sector into
--   gMemCardWork_abFrame (0x80210000) with MemCard_ReqReadSector, and before
--   writing it back with MemCard_ReqWriteSector stores 1 in D_8021007E and
--   0xFF in D_8021007D..D_8021007A, then XORs bytes 0..0x7E into
--   gMemCardWork_bFrameChecksum (+0x7F). The sector index is the save
--   file's DIRENTRY.head / 64, so the frame looks like the file's directory
--   frame, whose standard layout leaves +0x20..+0x7E unused; the meaning of
--   the five bytes is unknown (notes/memory-card-work-controller.md,
--   notes/mem-card-work-state.md).
--   Two further doubts: no resident jal or code pointer reaches
--   MemCardWork_InitializeOperation or MemCardWork_UpdateOperation, nor any
--   of the 3586 catalogued overlay modules, and 0x80210000 lies beyond 2 MB
--   of RAM, so on a retail console the frame mirrors 0x80010000. The retail
--   save and load menus go through MemCardDialog_Update (LIBMCRD) instead.
--   So: during a real SAVE and LOAD, does the legacy controller run at all,
--   who reads or writes the footer bytes 0x7A..0x7F, with which values, and
--   what does the whole frame hold when a sector is read or written?
--   Each answer names something:
--     * a qualified run in which the save and the load both ran and the
--       controller never did records MemCardWork_* and the footer bytes as
--       unreachable development code in the notes;
--     * if it runs, the frame dumped after the read shows whether the
--       sector is the directory frame and what the card holds at
--       +0x7A..+0x7E, so the five bytes can be named as directory-frame
--       fields (or a game marker) and the state values can be tied to the
--       save and load steps;
--     * any reader of the footer other than the checksum loop gives the
--       bytes their consumer.
--
-- WHAT IS WATCHED
--   * Read and Write breakpoints on the footer bytes +0x7A..+0x7F, placed
--     where the CPU actually lands: 0x8021007A with the 8 MB RAM setting,
--     its 2 MB mirror 0x8001007A otherwise (PCSX-Redux folds addresses
--     above 2 MB the same way the console does). Rows say "controller"
--     for the 5 stores, the checksum loop and the checksum store inside
--     MemCardWork_UpdateOperation, "kernel" for BIOS card-driver copies,
--     "rodata" for 2 MB-mode reads that really target the resident rodata
--     at 0x8001007A (ignored by the verdict), and "OTHER" for everything
--     else: the answer.
--   * Exec breakpoints on MemCardWork_InitializeOperation (a0 = mode),
--     MemCardWork_UpdateOperation (each change of operation state, flags,
--     mode, result and gDialog_bCallbackState is logged), MemCard_ReqReadSector
--     and MemCard_ReqWriteSector (a0..a2). Each sector request dumps the
--     whole 128-byte frame with its XOR check; a read dumps it again once
--     the bytes have changed and settled for half a second, or after 3 s.
--   * Route witnesses: SaveData_RequestLoad, SaveData_RequestWrite (with
--     the caller module) and MemCardDialog_Update, which drives the retail
--     card dialog. Control: Main_RunFrameServices (0x8001306C).
--   Output is bounded: 4 rows per pc/kind, 60 state rows, 8 frame dumps,
--   200 rows in total (live and in the document), 64 pc/kind pairs in the
--   final table; everything beyond is still counted. Silence counts as
--   negative evidence only for a qualified capture: the control fired, the
--   save and the load requests and the card dialog all ran, every US
--   signature word matched, no callback failed and the full window ran.
--   Anything less is reported as inconclusive. A console reset keeps the
--   capture (the route may span one); breakpoints are reinstalled when the
--   BIOS shell is reached.
--
-- HOW TO RUN (about four minutes)
--   1. Open PCSX-Redux with the game, memory card 1 inserted holding a
--      campaign save, and the debugger enabled. Select the interpreter CPU
--      (restart the emulator if you switch). Leave the RAM size as it is.
--   2. Boot to the main menu. Debug -> Lua editor, paste this file. It
--      prints "armed".
--   3. Choose LOAD and load the save from card 1.
--   4. In the menu that follows choose SAVE and save to card 1 (overwrite).
--   5. If time is left, soft-reset, boot to the main menu and LOAD again.
--   6. The document prints by itself four minutes after arming. Copy it
--      into tools/trace/result/memcard_header_bytes_watch.txt and fill in
--      the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Did the load and the save both succeed (which messages appeared)? Did
--   you soft-reset and load again? Which RAM size is configured
--   (Configuration -> Emulation -> 8MB RAM)?

local ffi = require('ffi')

local SCRIPT_NAME = 'memcard_header_bytes_watch'
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
-- PCSX-Redux always backs 8 MB; only the CPU's view is folded to 2 MB.
local WRAM_SIZE = 0x800000
local RESIDENT_START = 0x80010000
local RESIDENT_TEXT_END = 0x800906d4
local FPS = 60

local FRAME = 0x80210000
local FRAME_SIZE = 0x80
local FOOTER = 0x7a
local FOOTER_SIZE = 6
local FOOTER_NAMES = {[0x7a] = 'D_8021007A', [0x7b] = 'D_8021007B',
                      [0x7c] = 'D_8021007C', [0x7d] = 'D_8021007D',
                      [0x7e] = 'D_8021007E',
                      [0x7f] = 'gMemCardWork_bFrameChecksum'}

local OPERATION_STATE = 0x8009b3cf
local OPERATION_MODE = 0x8009b3c7
local WORK_FLAGS = 0x8009b3cc
local OPERATION_RESULT = 0x8009b3d5
local CALLBACK_STATE = 0x8009b3c1

-- Address, retail entry word, name.
local CONTROL = {0x8001306c, 0x27bdffe0, 'Main_RunFrameServices'}
local INIT_OP = {0x8003d300, 0x3c02800f, 'MemCardWork_InitializeOperation'}
local UPDATE_OP = {0x8003dc1c, 0x27bdffc8, 'MemCardWork_UpdateOperation'}
local UPDATE_OP_SIZE = 0x850
local READ_SECTOR = {0x80044278, 0x27bdffe0, 'MemCard_ReqReadSector'}
local WRITE_SECTOR = {0x80044380, 0x27bdffe0, 'MemCard_ReqWriteSector'}
local DIALOG = {0x8003f454, 0x978404f2, 'MemCardDialog_Update'}
local REQ_LOAD = {0x8003f7d4, 0x27bdffe8, 'SaveData_RequestLoad'}
local REQ_WRITE = {0x8003f87c, 0x27bdffe8, 'SaveData_RequestWrite'}

-- The controller's own footer accesses: the five stores, the checksum
-- loop's load and the checksum store.
local CONTROLLER_PCS = {
    [0x8003e31c] = 'store 0x7E', [0x8003e328] = 'store 0x7D',
    [0x8003e330] = 'store 0x7C', [0x8003e338] = 'store 0x7B',
    [0x8003e340] = 'store 0x7A', [0x8003e348] = 'checksum loop',
    [0x8003e380] = 'checksum store',
}

local SIGNATURES = {
    CONTROL, INIT_OP, UPDATE_OP, READ_SECTOR, WRITE_SECTOR, DIALOG,
    REQ_LOAD, REQ_WRITE,
    {0x8003e108, 0x0c01109e, 'UpdateOperation jal MemCard_ReqReadSector'},
    {0x8003e31c, 0xa022007e, 'UpdateOperation sb D_8021007E'},
    {0x8003e340, 0xa022007a, 'UpdateOperation sb D_8021007A'},
    {0x8003e380, 0xa024007f, 'UpdateOperation sb checksum'},
    {0x8003e394, 0x0c0110e0, 'UpdateOperation jal MemCard_ReqWriteSector'},
}

-- main_menu (US SU.MRG) calls both save requests.
local MAIN_MENU = {0x80180000, 0x8000,
                   {{0x0, 0x0f}, {0x4, 0x8018416c}, {0x8, 0x80183514}}}

local WINDOW_FRAMES = 240 * FPS
local READ_SETTLE_FRAMES = 3 * FPS
local READ_STABLE_FRAMES = FPS / 2
local MAX_ROWS = 200
local MAX_ROWS_PER_PC = 4
local MAX_STATE_ROWS = 60
local MAX_DUMPS = 8
local MAX_PCS = 64

local LOADS = {[0x20] = 1, [0x21] = 2, [0x22] = 4, [0x23] = 4, [0x24] = 1,
               [0x25] = 2, [0x26] = 4, [0x32] = 4}
local STORES = {[0x28] = 1, [0x29] = 2, [0x2a] = 4, [0x2b] = 4, [0x2e] = 4,
                [0x3a] = 4}
local EXACT_STORES = {[0x28] = true, [0x29] = true, [0x2b] = true}

local mem = PCSX.getMemPtr()

local ramNote, eightMegs = (function()
    local ok, value = pcall(function() return PCSX.settings.emulator['8Megs'] end)
    if ok and value == true then
        return '8MB: the frame is its own memory at 0x80210000', true
    elseif ok and value == false then
        return '2MB: the frame folds onto 0x80010000 (resident rodata)', false
    end
    return 'unknown (assumed 2MB): the frame folds onto 0x80010000', false
end)()

-- Physical offset of a KUSEG/KSEG0/KSEG1 RAM address as the CPU sees it.
local function phys(addr)
    if type(addr) ~= 'number' then return nil end
    local seg = addr - addr % 0x20000000
    if seg ~= 0 and seg ~= RAM_BASE and seg ~= 0xa0000000 then return nil end
    local off = addr % 0x20000000
    if off >= WRAM_SIZE then return nil end
    if not eightMegs then off = off % 0x200000 end
    return off
end
local function u8(addr)
    local off = phys(addr)
    if mem == nil or off == nil then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + off)[0])
end
local function u32(addr)
    local off = phys(addr)
    if mem == nil or off == nil or off % 4 ~= 0 or off + 4 > WRAM_SIZE then
        return nil
    end
    return tonumber(ffi.cast('uint32_t*', mem + off)[0])
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

-- The footer breakpoint address the CPU's accesses normalise to.
local FOOTER_BP = eightMegs and FRAME + FOOTER
    or (FRAME + FOOTER - 0x200000)

-- Capture state; kept across resets because the route may span one.
local vsyncs, controlHits = 0, 0
local rows, dropped, suppressed, stateRows = {}, 0, 0, 0
local perPc, pcOrder, untracked = {}, {}, 0
local counts = {read = 0, write = 0, controller = 0, kernel = 0, other = 0,
                rodata = 0, decode_mismatch = 0, no_address = 0}
local hits = {init = 0, update = 0, read_sector = 0, write_sector = 0,
              dialog = 0, req_load = 0, req_write = 0}
local dumps, pendingRead = 0, nil
local lastState = nil
local callbackError = nil

local done = false
local resets = 0
local reinstalled = true
local installNote = 'not installed'
local generation = 0

breakpoints_memcard_header_bytes_watch = {}

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

local function mainMenuResident()
    for _, sig in ipairs(MAIN_MENU[3]) do
        if u32(MAIN_MENU[1] + sig[1]) ~= sig[2] then return false end
    end
    return true
end

local function moduleAt(pc)
    if pc < RESIDENT_START or pc >= 0xa0000000 then return 'kernel' end
    if pc >= UPDATE_OP[1] and pc < UPDATE_OP[1] + UPDATE_OP_SIZE then
        return 'resident(' .. UPDATE_OP[3] .. ')'
    end
    if pc < RESIDENT_TEXT_END then return 'resident' end
    if pc >= MAIN_MENU[1] and pc < MAIN_MENU[1] + MAIN_MENU[2]
        and mainMenuResident() then
        return string.format('main_menu+0x%X', pc - MAIN_MENU[1])
    end
    return 'unidentified'
end

-- With 2 MB the breakpoint also sees the resident rodata that really lives
-- at 0x8001007A; an access made through that address is not a frame access.
local function classOf(pc, addr)
    if CONTROLLER_PCS[pc] then return 'controller' end
    if pc < RESIDENT_START or pc >= 0xa0000000 then return 'kernel' end
    if addr ~= nil and addr % 0x20000000 < 0x200000 then return 'rodata' end
    return 'OTHER'
end

local function frameChecksum()
    local x = 0
    for k = 0, FRAME_SIZE - 2 do
        local b = u8(FRAME + k)
        if b == nil then return nil end
        x = bit.bxor(x, b)
    end
    return x
end

local function frameBytes()
    local parts = {}
    for k = 0, FRAME_SIZE - 1 do
        parts[#parts + 1] = hexw(u8(FRAME + k), 1)
    end
    return parts
end

local function dumpFrame(why)
    if dumps >= MAX_DUMPS then
        dropped = dropped + 1
        return
    end
    dumps = dumps + 1
    local bytes = frameBytes()
    local x = frameChecksum()
    local stored = u8(FRAME + 0x7f)
    live(string.format('%s frame %s footer=%s xor0..7E=%s stored7F=%s %s',
                       stamp(), why, table.concat(bytes, ' ', 0x7b, 0x80),
                       hexw(x, 1), hexw(stored, 1),
                       (x ~= nil and x == stored) and 'checksum=ok'
                           or 'checksum=differs'))
    for line = 0, FRAME_SIZE / 16 - 1 do
        live(string.format('%s frame +%02X %s', stamp(), line * 16,
                           table.concat(bytes, ' ', line * 16 + 1,
                                        line * 16 + 16)))
    end
end

local function frameKey()
    return table.concat(frameBytes())
end

local function stateTuple()
    return string.format('state=%s mode=%s flags=%s result=%s callback=%s',
                         hexw(u8(OPERATION_STATE), 1),
                         hexw(u8(OPERATION_MODE), 1),
                         hexw(readLE(WORK_FLAGS, 2), 2),
                         hexw(u8(OPERATION_RESULT), 1),
                         hexw(u8(CALLBACK_STATE), 1))
end

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

-- The callback reports the address the CPU used (0x8021xxxx or its mirror);
-- both name the same footer byte.
local function footerName(addr)
    local off = (phys(addr) or 0) - phys(FRAME)
    if off >= FOOTER and off < FOOTER + FOOTER_SIZE then
        return FOOTER_NAMES[off]
    end
    return string.format('0x%08X', addr)
end

local function onAccess(kind, bpAddress, bpWidth)
    local regs = PCSX.getRegisters()
    local pc = tonumber(regs.pc) or 0
    local ra = tonumber(regs.GPR and regs.GPR.n and regs.GPR.n.ra) or 0
    local addr = type(bpAddress) == 'number' and bpAddress or nil
    local width = tonumber(bpWidth)
    local d = decode(regs, pc)
    local decodeNote
    if d == nil then
        decodeNote = 'decode=none'
    elseif addr ~= nil and phys(d.addr) == phys(addr) and d.width == width then
        decodeNote = 'decode=ok'
    else
        decodeNote = string.format('decode=mismatch(0x%08X/%d)', d.addr,
                                   d.width)
        counts.decode_mismatch = counts.decode_mismatch + 1
        d = nil
    end
    if addr == nil then counts.no_address = counts.no_address + 1 end
    local class = classOf(pc, addr)
    counts[kind] = counts[kind] + 1
    counts[class:lower()] = counts[class:lower()] + 1

    local detail
    if addr == nil then
        detail = 'at=unknown(callback gave no address) ' .. decodeNote
    else
        width = width or 1
        detail = string.format('at=%s(0x%08X) width=%d', footerName(addr),
                               addr, width)
        if kind == 'write' then
            local new = d and d.value
            detail = detail .. string.format(' %s -> %s',
                hexw(readLE(addr, width), width),
                new and hexw(new, width) or '??')
        else
            detail = detail .. ' value=' .. hexw(readLE(addr, width), width)
        end
        detail = detail .. ' ' .. decodeNote
    end

    local key = string.format('%s pc=0x%08X', kind, pc)
    local entry = perPc[key]
    if entry == nil then
        if #pcOrder >= MAX_PCS then
            untracked = untracked + 1
            return
        end
        entry = {count = 0, firstVs = vsyncs, ra = ra, class = class,
                 module = moduleAt(pc)}
        perPc[key] = entry
        pcOrder[#pcOrder + 1] = key
    end
    entry.count = entry.count + 1
    entry.lastVs = vsyncs
    if entry.count > MAX_ROWS_PER_PC then
        suppressed = suppressed + 1
        return
    end
    live(string.format('%s %s %s pc=0x%08X in=%s%s ra=0x%08X %s %s', stamp(),
                       class, kind, pc, entry.module,
                       CONTROLLER_PCS[pc] and ('(' .. CONTROLLER_PCS[pc] .. ')')
                           or '', ra, detail, stateTuple()))
end

local function args()
    local n = PCSX.getRegisters().GPR.n
    return tonumber(n.a0) or 0, tonumber(n.a1) or 0, tonumber(n.a2) or 0,
        tonumber(n.ra) or 0
end

local function onSector(which)
    local a0, a1, a2, ra = args()
    local key = which == 'read' and 'read_sector' or 'write_sector'
    hits[key] = hits[key] + 1
    live(string.format('%s %s chan=%d addr=0x%08X sector=%d ra=0x%08X in=%s '
                       .. '%s', stamp(), which == 'read' and READ_SECTOR[3]
                           or WRITE_SECTOR[3], a0, a1, a2, ra, moduleAt(ra),
                       stateTuple()))
    if a1 ~= FRAME then
        live(string.format('%s note: buffer 0x%08X is not the frame', stamp(),
                           a1))
    end
    if which == 'read' then
        dumpFrame('before-read')
        pendingRead = {key = frameKey(), since = vsyncs}
    else
        dumpFrame('write')
    end
end

local function onUpdate()
    hits.update = hits.update + 1
    local now = stateTuple()
    if now ~= lastState then
        lastState = now
        if stateRows < MAX_STATE_ROWS then
            stateRows = stateRows + 1
            live(string.format('%s %s %s', stamp(), UPDATE_OP[3], now))
        else
            dropped = dropped + 1
        end
    end
end

local function signatureCheck()
    local bad = {}
    for _, sig in ipairs(SIGNATURES) do
        local word = u32(sig[1])
        if word ~= sig[2] then
            bad[#bad + 1] = string.format('signature %s 0x%08X expected '
                .. '0x%08X read %s MISMATCH', sig[3], sig[1], sig[2],
                hexw(word, 4))
        end
    end
    return bad, #SIGNATURES
end

local function qualificationGaps(windowDone)
    local gaps = {}
    if not windowDone then gaps[#gaps + 1] = 'window did not complete' end
    if callbackError ~= nil then gaps[#gaps + 1] = 'a callback failed' end
    if #signatureCheck() > 0 then gaps[#gaps + 1] = 'US signature mismatch' end
    if controlHits == 0 then
        gaps[#gaps + 1] = 'no Main_RunFrameServices control'
    end
    if hits.req_load == 0 then
        gaps[#gaps + 1] = 'no SaveData_RequestLoad (the load was not captured)'
    end
    if hits.req_write == 0 then
        gaps[#gaps + 1] = 'no SaveData_RequestWrite (the save was not captured)'
    end
    if hits.dialog == 0 then gaps[#gaps + 1] = 'MemCardDialog_Update never ran' end
    return gaps
end

local function verdict(gaps)
    local legacy = hits.init + hits.update + hits.read_sector
        + hits.write_sector
    local footer = counts.read + counts.write - counts.rodata
    if controlHits == 0 and legacy + footer + hits.dialog == 0 then
        return 'nothing fired, not even the Main_RunFrameServices control: '
            .. 'the setup failed (debugger off, dynarec still running, or '
            .. 'breakpoints lost after a reset without a reinstall line); '
            .. 'silence is not evidence'
    elseif #gaps > 0 then
        local text = 'inconclusive: ' .. table.concat(gaps, '; ')
        if legacy + footer > 0 then
            text = text .. string.format('; legacy controller calls=%d and '
                .. 'footer accesses=%d were still seen', legacy, footer)
        else
            text = text .. '; silence here is not evidence'
        end
        return text
    elseif legacy == 0 and counts.other + counts.kernel == 0 then
        return string.format('the retail save (%d) and load (%d) requests ran '
            .. 'through MemCardDialog_Update (%d calls) and neither the '
            .. 'legacy MemCardWork controller nor its sector requests ran, '
            .. 'and no code outside it touched the footer, in a qualified '
            .. 'capture: negative evidence that the frame and its footer are '
            .. 'unused on the retail save/load path', hits.req_write,
            hits.req_load, hits.dialog)
    end
    return string.format('legacy controller or footer activity observed '
        .. '(init=%d update=%d '
        .. 'read_sector=%d write_sector=%d); footer accesses controller=%d '
        .. 'kernel=%d OTHER=%d; see the frame dumps and rows', hits.init,
        hits.update, hits.read_sector, hits.write_sector, counts.controller,
        counts.kernel, counts.other)
end

local function finish(reason, windowDone)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_memcard_header_bytes_watch) do
        pcall(function() bp:disable() end)
    end
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<did the load and the save both succeed (which messages)? did you')
    print(' soft-reset and load again? configured RAM size (8MB box)?>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('ram: ' .. ramNote .. string.format('; footer breakpoint 0x%08X',
                                               FOOTER_BP))
    print('status: ' .. reason)
    print('breakpoints: ' .. installNote)
    print(string.format('vsyncs: %d control_hits: %d resets: %d', vsyncs,
                        controlHits, resets))
    print(string.format('route: SaveData_RequestLoad=%d '
                        .. 'SaveData_RequestWrite=%d MemCardDialog_Update=%d',
                        hits.req_load, hits.req_write, hits.dialog))
    print(string.format('legacy: MemCardWork_InitializeOperation=%d '
                        .. 'MemCardWork_UpdateOperation=%d '
                        .. 'MemCard_ReqReadSector=%d MemCard_ReqWriteSector=%d',
                        hits.init, hits.update, hits.read_sector,
                        hits.write_sector))
    print(string.format('footer: reads=%d writes=%d controller=%d kernel=%d '
                        .. 'OTHER=%d rodata_mirror=%d dropped_rows=%d '
                        .. 'repeat_rows_suppressed=%d', counts.read,
                        counts.write, counts.controller, counts.kernel,
                        counts.other, counts.rodata, dropped, suppressed))
    print(string.format('bounds: rows=%d/%d tracked_pcs=%d/%d '
                        .. 'untracked_pc_accesses=%d frame_dumps=%d/%d '
                        .. 'state_rows=%d/%d decode_mismatch=%d '
                        .. 'no_callback_address=%d', #rows, MAX_ROWS, #pcOrder,
                        MAX_PCS, untracked, dumps, MAX_DUMPS, stateRows,
                        MAX_STATE_ROWS, counts.decode_mismatch,
                        counts.no_address))
    local gaps = qualificationGaps(windowDone == true)
    print('qualified: ' .. (#gaps == 0 and 'yes'
                            or ('no (' .. table.concat(gaps, '; ') .. ')')))
    print('verdict: ' .. verdict(gaps))
    local bad, total = signatureCheck()
    print(string.format('signatures: %d/%d US words ok', total - #bad, total))
    for _, line in ipairs(bad) do print(line) end
    print('final: ' .. stateTuple() .. ' footer='
          .. table.concat(frameBytes(), ' ', 0x7b, 0x80))
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    print('')
    for _, key in ipairs(pcOrder) do
        local e = perPc[key]
        print(string.format('pc %s %s count=%d first_vs=%d last_vs=%d '
                            .. 'ra=0x%08X in=%s', key, e.class, e.count,
                            e.firstVs, e.lastVs, e.ra, e.module))
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
    local list = breakpoints_memcard_header_bytes_watch
    local mine = generation
    list[#list + 1] = traceAddBreakpoint(address, kind, width,
                                         SCRIPT_NAME .. ': ' .. label,
                                         guarded(function(...)
        if mine == generation then handler(...) end
    end))
end

local function milestone(sig, key)
    return function()
        hits[key] = hits[key] + 1
        local _, _, _, ra = args()
        if hits[key] <= MAX_ROWS_PER_PC then
            live(string.format('%s milestone %s ra=0x%08X in=%s', stamp(),
                               sig[3], ra, moduleAt(ra)))
        end
    end
end

local function installBreakpoints(note)
    generation = generation + 1
    breakpoints_memcard_header_bytes_watch = {}
    add(CONTROL[1], 'Exec', 4, 'control ' .. CONTROL[3], function()
        controlHits = controlHits + 1
    end)
    add(DIALOG[1], 'Exec', 4, DIALOG[3], function()
        hits.dialog = hits.dialog + 1
        if hits.dialog == 1 then
            live(string.format('%s milestone %s first call', stamp(),
                               DIALOG[3]))
        end
    end)
    add(REQ_LOAD[1], 'Exec', 4, REQ_LOAD[3], milestone(REQ_LOAD, 'req_load'))
    add(REQ_WRITE[1], 'Exec', 4, REQ_WRITE[3],
        milestone(REQ_WRITE, 'req_write'))
    add(INIT_OP[1], 'Exec', 4, INIT_OP[3], function()
        hits.init = hits.init + 1
        local a0, _, _, ra = args()
        live(string.format('%s CALL %s mode=0x%02X ra=0x%08X in=%s', stamp(),
                           INIT_OP[3], a0 % 0x100, ra, moduleAt(ra)))
    end)
    add(UPDATE_OP[1], 'Exec', 4, UPDATE_OP[3], onUpdate)
    add(READ_SECTOR[1], 'Exec', 4, READ_SECTOR[3],
        function() onSector('read') end)
    add(WRITE_SECTOR[1], 'Exec', 4, WRITE_SECTOR[3],
        function() onSector('write') end)
    add(FOOTER_BP, 'Read', FOOTER_SIZE, 'frame footer read',
        function(a, w) onAccess('read', a, w) end)
    add(FOOTER_BP, 'Write', FOOTER_SIZE, 'frame footer write',
        function(a, w) onAccess('write', a, w) end)
    installNote = note
    print(string.format('%s: %d breakpoints %s', SCRIPT_NAME,
                        #breakpoints_memcard_header_bytes_watch, note))
end

local function poll()
    if done then return end
    if not reinstalled then
        -- No ShellReached arrived (fast boot); the reset is over by now.
        reinstalled = true
        installBreakpoints('reinstalled at the first vsync after reset')
    end
    vsyncs = vsyncs + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    if pendingRead ~= nil then
        -- The card driver fills the frame over several frames; dump once it
        -- has changed and then held still for half a second.
        local key = frameKey()
        if key ~= pendingRead.key then
            pendingRead.key = key
            pendingRead.changedAt = vsyncs
        end
        local changedAt = pendingRead.changedAt
        if changedAt ~= nil and vsyncs - changedAt >= READ_STABLE_FRAMES then
            pendingRead = nil
            dumpFrame('after-read')
        elseif vsyncs - pendingRead.since >= READ_SETTLE_FRAMES then
            pendingRead = nil
            dumpFrame(changedAt and 'after-read(still changing after 3s)'
                      or 'after-read(unchanged after 3s)')
        end
    end
    if vsyncs >= WINDOW_FRAMES then
        finish(string.format('%d-second window ended', WINDOW_FRAMES / FPS),
               true)
    end
end

listeners_memcard_header_bytes_watch = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        local ok, err = pcall(function()
            if done then return end
            -- PCSX-Redux drops Lua breakpoints on reset; the capture stays.
            breakpoints_memcard_header_bytes_watch = {}
            generation = generation + 1
            resets = resets + 1
            reinstalled = false
            pendingRead = nil
            lastState = nil
            installNote = 'lost at reset, not reinstalled'
            live(string.format('%s reset observed; capture kept, waiting for '
                               .. 'the BIOS shell', stamp()))
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

print(SCRIPT_NAME .. ': ram ' .. ramNote)
installBreakpoints('installed')
do
    local bad, total = signatureCheck()
    print(string.format('%s: signatures %d/%d US words ok', SCRIPT_NAME,
                        total - #bad, total))
    for _, line in ipairs(bad) do print(SCRIPT_NAME .. ': ' .. line) end
end
print(SCRIPT_NAME .. ': now ' .. stateTuple() .. ' footer='
      .. table.concat(frameBytes(), ' ', 0x7b, 0x80))
print(SCRIPT_NAME .. ': armed. Route: from the main menu LOAD the save on '
      .. 'card 1, then choose SAVE and overwrite it on card 1; if time is '
      .. 'left, soft-reset and LOAD again. The document prints by itself '
      .. (WINDOW_FRAMES / FPS) .. ' seconds after arming.')
