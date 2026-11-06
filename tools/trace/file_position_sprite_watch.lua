-- file_position_sprite_watch.lua
--
-- WHAT THIS ANSWERS
--   File_SetPositionTable (0x800136E4, resident, runs once at boot while the
--   CD file table is built) fills a GsSPRITE-shaped SpritePrim D_800E9DF0
--   (0x800E9DF0..0x800E9E13, 0x24 bytes) as a 24x24 sprite at (0x120, 0xD0),
--   tpage 0xB, uv (0x00, 0xA0), clut (0x230, 0xFC), neutral grey, attribute
--   0x08000000, and clears the byte D_8009B0E0 (0x8009B0E0, written as
--   `sb zero, 0x1D8(gp)`). No tracked C reads either symbol afterwards. The
--   sprite sits in the bottom-right corner and looks like a disc-access /
--   "loading" indicator with D_8009B0E0 as its state byte, but that is a
--   guess. Does any code read or write the sprite or the byte after boot,
--   from which pc and ra, on which frame, and does D_8009B0E0 ever change?
--   The answer lets both get evidence-backed names (for example a file-access
--   indicator sprite and its state byte) or be recorded as unused.
--
-- WHAT IS WATCHED
--   * Read and Write breakpoints over the 0x24-byte sprite and over the byte.
--     Each access is printed at once: vsync count, game frame (control hits),
--     time, phase, pc, ra, kind, field/offset and, for writes, old -> new.
--     Phase "init" is a pc inside File_SetPositionTable, "pre-init" is an
--     access after a reset but before File_SetPositionTable ran (the startup
--     bss clear), "post-init" is everything else: the accesses that matter.
--     At most 4 rows per pc and kind and 200 rows in total are printed; every
--     access is still counted per pc in the final table.
--   * An Exec breakpoint on File_SetPositionTable marks the boot init.
--   * Control: an Exec breakpoint on Main_RunFrameServices (0x8001306C), so
--     silence counts as negative evidence only if the control fired.
--   * Every vsync the sprite and the byte are re-read and every change is
--     printed field by field.
--   The script survives a console reset: it clears its state on reset and
--   reinstalls its breakpoints when the BIOS shell is reached.
--
-- HOW TO RUN (about three minutes)
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file. It prints "armed".
--   3. Soft-reset the console (Emulation -> Soft reset). It prints
--      "reset observed" and then "breakpoints reinstalled".
--   4. Let it boot to the title screen, press Start, open the Library (it
--      loads from disc) and go back, then open the Free Duel menu.
--   5. During every disc load watch the bottom-right corner of the screen:
--      does a small (24x24) grey or icon sprite appear there?
--   6. The document prints by itself three minutes after the reset. Copy it
--      into tools/trace/result/file_position_sprite_watch.txt and fill in the
--      context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Did a small icon appear bottom-right during any load (which screen,
--   roughly when)? Did you soft-reset after arming?

local ffi = require('ffi')

local SCRIPT_NAME = 'file_position_sprite_watch'
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
local FPS = 60

local SPRITE = 0x800e9df0
local SPRITE_SIZE = 0x24
local STATE_BYTE = 0x8009b0e0

local CONTROL = {0x8001306c, 0x27bdffe0, 'Main_RunFrameServices'}
local INIT = {0x800136e4, 0x27bdffd8, 'File_SetPositionTable'}
local INIT_SIZE = 0x100
-- `sb zero, 0x1D8(gp)` in File_SetPositionTable: the store of D_8009B0E0.
local INIT_BYTE_STORE = {0x80013770, 0xa38001d8, 'File_SetPositionTable sb'}

local WINDOW_FRAMES = 180 * FPS
local MAX_ROWS = 200
local MAX_ROWS_PER_PC = 4
local MAX_CHANGE_ROWS = 100

-- SpritePrim (src/ygo_types.h), GsSPRITE layout.
local FIELDS = {
    {0x00, 4, 'attribute'}, {0x04, 2, 'x'}, {0x06, 2, 'y'},
    {0x08, 2, 'w'}, {0x0a, 2, 'h'}, {0x0c, 2, 'tpage'},
    {0x0e, 1, 'u'}, {0x0f, 1, 'v'}, {0x10, 2, 'cx'}, {0x12, 2, 'cy'},
    {0x14, 1, 'r'}, {0x15, 1, 'g'}, {0x16, 1, 'b'}, {0x17, 1, 'rgb_pad'},
    {0x18, 2, 'mx'}, {0x1a, 2, 'my'}, {0x1c, 4, 'scale'},
    {0x20, 4, 'rotate'},
}

-- Load/store opcodes: width, kind.
local LOADS = {[0x20] = 1, [0x21] = 2, [0x22] = 4, [0x23] = 4, [0x24] = 1,
               [0x25] = 2, [0x26] = 4, [0x32] = 4}
local STORES = {[0x28] = 1, [0x29] = 2, [0x2a] = 4, [0x2b] = 4, [0x2e] = 4,
                [0x3a] = 4}
local EXACT_STORES = {[0x28] = true, [0x29] = true, [0x2b] = true}

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
local vsyncs, controlHits = 0, 0
local rows, dropped, suppressed, changeRows = {}, 0, 0, 0
local perPc, pcOrder = {}, {}
local counts = {}
local initEntries, initEntryRa = 0, nil
local resetSeen = false
local snapshot = nil
local callbackError = nil

-- Run state, kept across resets.
local done = false
local resets = 0
local reinstalled = false
local installNote = 'not installed'
-- Handlers from an earlier install ignore hits, in case a reset left the
-- old breakpoints alive beside the reinstalled ones.
local generation = 0

breakpoints_file_position_sprite_watch = {}

local function resetCaptureState()
    vsyncs, controlHits = 0, 0
    rows, dropped, suppressed, changeRows = {}, 0, 0, 0
    perPc, pcOrder = {}, {}
    counts = {['pre-init'] = 0, init = 0, ['post-init'] = 0,
              read = 0, write = 0, post_read = 0, post_write = 0}
    initEntries, initEntryRa = 0, nil
    snapshot = nil
    callbackError = nil
end
resetCaptureState()

local function stamp()
    return string.format('vs=%d f=%d t=%6.1fs', vsyncs, controlHits,
                         vsyncs / FPS)
end

local function live(text)
    print(text)
    if #rows < MAX_ROWS then rows[#rows + 1] = text else dropped = dropped + 1 end
end

local function functionName(pc)
    if pc >= INIT[1] and pc < INIT[1] + INIT_SIZE then return INIT[3] end
    if pc >= CONTROL[1] and pc < CONTROL[1] + 0xe8 then return CONTROL[3] end
    return '?'
end

local function describe(addr)
    if addr >= STATE_BYTE and addr < STATE_BYTE + 1 then
        return 'D_8009B0E0'
    end
    local off = addr - SPRITE
    if off >= 0 and off < SPRITE_SIZE then
        for _, f in ipairs(FIELDS) do
            if off >= f[1] and off < f[1] + f[2] then
                return string.format('sprite+0x%02X(%s)', off, f[3])
            end
        end
    end
    return string.format('0x%08X', addr)
end

local function phaseOf(pc)
    if pc >= INIT[1] and pc < INIT[1] + INIT_SIZE then return 'init' end
    if resetSeen and initEntries == 0 then return 'pre-init' end
    return 'post-init'
end

-- A memory breakpoint fires before the access, with pc on the accessing
-- instruction, so memory still holds the old bytes and a store's new value
-- is still in its source register.
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
    local d = {addr = (base + imm) % 0x100000000, width = width,
               store = STORES[op] ~= nil}
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
    local d = decode(regs, pc)
    local addr, width = nil, nil
    if d ~= nil then
        addr, width = d.addr, d.width
    elseif type(bpAddress) == 'number' then
        addr, width = bpAddress, tonumber(bpWidth) or 1
    end
    local phase = phaseOf(pc)
    counts[phase] = counts[phase] + 1
    counts[kind] = counts[kind] + 1
    if phase == 'post-init' then
        counts['post_' .. kind] = counts['post_' .. kind] + 1
    end

    local key = string.format('%s pc=0x%08X', kind, pc)
    local entry = perPc[key]
    if entry == nil then
        entry = {count = 0, firstVs = vsyncs, firstFrame = controlHits,
                 ra = ra, phase = phase, target = target,
                 where = addr and describe(addr) or target, changed = 0}
        perPc[key] = entry
        pcOrder[#pcOrder + 1] = key
    end
    entry.count = entry.count + 1
    entry.lastVs = vsyncs

    local detail
    if addr == nil then
        detail = 'at=undecoded'
    else
        detail = string.format('at=%s width=%d', describe(addr), width)
        if kind == 'write' then
            local old = readLE(addr, width)
            local new = d and d.value
            detail = detail .. string.format(' %s -> %s', hexw(old, width),
                                             new and hexw(new, width) or '??')
            if new ~= nil and old ~= nil and new ~= old then
                entry.changed = entry.changed + 1
            end
        else
            detail = detail .. ' value=' .. hexw(readLE(addr, width), width)
        end
    end
    if entry.count > MAX_ROWS_PER_PC then
        suppressed = suppressed + 1
        return
    end
    live(string.format('%s %s %s %s pc=0x%08X(%s) ra=0x%08X %s',
                       stamp(), target, phase, kind, pc,
                       functionName(pc), ra, detail))
end

local function takeSnapshot()
    local s = {}
    for k = 0, SPRITE_SIZE - 1 do
        local b = u8(SPRITE + k)
        if b == nil then return nil end
        s[k] = b
    end
    s.byte = u8(STATE_BYTE)
    return s
end

local function fieldValue(s, f)
    local v = 0
    for k = f[2] - 1, 0, -1 do v = v * 0x100 + s[f[1] + k] end
    return v
end

local function formatSnapshot(s)
    if s == nil then return 'unreadable' end
    local parts = {}
    for _, f in ipairs(FIELDS) do
        parts[#parts + 1] = f[3] .. '=' .. hexw(fieldValue(s, f), f[2])
    end
    return 'byte=' .. hexw(s.byte, 1) .. ' ' .. table.concat(parts, ' ')
end

local function pollChanges()
    local now = takeSnapshot()
    if now == nil then return end
    if snapshot ~= nil then
        local changes = {}
        if now.byte ~= snapshot.byte then
            changes[#changes + 1] = 'D_8009B0E0 ' .. hexw(snapshot.byte, 1)
                .. '->' .. hexw(now.byte, 1)
        end
        for _, f in ipairs(FIELDS) do
            local a, b = fieldValue(snapshot, f), fieldValue(now, f)
            if a ~= b then
                changes[#changes + 1] = f[3] .. ' ' .. hexw(a, f[2]) .. '->'
                    .. hexw(b, f[2])
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

local function signatureLines()
    local lines = {}
    for _, s in ipairs({CONTROL, INIT, INIT_BYTE_STORE}) do
        local word = u32(s[1])
        lines[#lines + 1] = string.format('signature %s 0x%08X expected '
            .. '0x%08X read %s %s', s[3], s[1], s[2], hexw(word, 4),
            word == s[2] and 'ok' or 'MISMATCH')
    end
    return lines
end

local function verdict()
    local later = counts.post_read + counts.post_write
    if controlHits == 0 and counts.read + counts.write == 0
        and initEntries == 0 then
        return 'nothing fired, not even the Main_RunFrameServices control: '
            .. 'the setup failed (debugger off, dynarec still running, or '
            .. 'breakpoints lost after a reset without a reinstall line); '
            .. 'silence is not evidence'
    elseif controlHits == 0 then
        return 'the control never fired although watch breakpoints did; '
            .. 'the run is incomplete, compare the signature lines'
    elseif later == 0 then
        return string.format('no CPU read or write of D_800E9DF0 or '
            .. 'D_8009B0E0 after File_SetPositionTable during %d control '
            .. 'frames: negative evidence that neither is used on this '
            .. 'route', controlHits)
    end
    local pcs = 0
    for _, key in ipairs(pcOrder) do
        if perPc[key].phase == 'post-init' then pcs = pcs + 1 end
    end
    return string.format('post-init accesses observed: reads=%d writes=%d '
        .. 'from %d pc/kind pairs', counts.post_read, counts.post_write, pcs)
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_file_position_sprite_watch) do
        pcall(function() bp:disable() end)
    end
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<did a small 24x24 grey/icon sprite appear bottom-right during any')
    print(' disc load? which screen, roughly when? did you soft-reset after')
    print(' arming?>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print('breakpoints: ' .. installNote)
    print(string.format('vsyncs: %d control_hits: %d resets: %d '
                        .. 'init_entries: %d init_ra: %s',
                        vsyncs, controlHits, resets, initEntries,
                        initEntryRa and string.format('0x%08X', initEntryRa)
                            or 'none'))
    print(string.format('accesses: reads=%d writes=%d pre-init=%d init=%d '
                        .. 'post-init=%d (post-init reads=%d writes=%d) '
                        .. 'dropped_rows=%d repeat_rows_suppressed=%d',
                        counts.read, counts.write, counts['pre-init'],
                        counts.init, counts['post-init'], counts.post_read,
                        counts.post_write, dropped, suppressed))
    if initEntries == 0 then
        print('boot_init: NOT observed; arm the script, then soft-reset to '
              .. 'capture File_SetPositionTable')
    else
        print('boot_init: observed')
    end
    print('verdict: ' .. verdict())
    for _, line in ipairs(signatureLines()) do print(line) end
    print('final: ' .. formatSnapshot(takeSnapshot()))
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    print('')
    for _, key in ipairs(pcOrder) do
        local e = perPc[key]
        print(string.format('pc %s %s %s count=%d changed=%d first_vs=%d '
                            .. 'first_f=%d last_vs=%d ra=0x%08X at=%s',
                            key, e.target, e.phase, e.count, e.changed,
                            e.firstVs, e.firstFrame, e.lastVs, e.ra, e.where))
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
    local list = breakpoints_file_position_sprite_watch
    local mine = generation
    list[#list + 1] = traceAddBreakpoint(address, kind, width,
                                         SCRIPT_NAME .. ': ' .. label,
                                         guarded(function(...)
        if mine == generation then handler(...) end
    end))
end

local function installBreakpoints(note)
    generation = generation + 1
    breakpoints_file_position_sprite_watch = {}
    add(CONTROL[1], 'Exec', 4, 'control ' .. CONTROL[3], function()
        controlHits = controlHits + 1
    end)
    add(INIT[1], 'Exec', 4, INIT[3], function()
        initEntries = initEntries + 1
        local n = PCSX.getRegisters().GPR.n
        initEntryRa = tonumber(n.ra)
        live(string.format('%s milestone File_SetPositionTable entered '
                           .. 'ra=0x%08X before: %s', stamp(), initEntryRa,
                           formatSnapshot(takeSnapshot())))
    end)
    add(SPRITE, 'Read', SPRITE_SIZE, 'D_800E9DF0 read',
        function(a, w) onAccess('read', 'sprite', a, w) end)
    add(SPRITE, 'Write', SPRITE_SIZE, 'D_800E9DF0 write',
        function(a, w) onAccess('write', 'sprite', a, w) end)
    add(STATE_BYTE, 'Read', 1, 'D_8009B0E0 read',
        function(a, w) onAccess('read', 'byte', a, w) end)
    add(STATE_BYTE, 'Write', 1, 'D_8009B0E0 write',
        function(a, w) onAccess('write', 'byte', a, w) end)
    installNote = note
    print(string.format('%s: %d breakpoints %s', SCRIPT_NAME,
                        #breakpoints_file_position_sprite_watch, note))
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
        finish(string.format('%d-second window ended', WINDOW_FRAMES / FPS))
    end
end

listeners_file_position_sprite_watch = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        local ok, err = pcall(function()
            if done then return end
            -- PCSX-Redux drops Lua breakpoints on reset.
            breakpoints_file_position_sprite_watch = {}
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
for _, line in ipairs(signatureLines()) do print(SCRIPT_NAME .. ': ' .. line) end
print(SCRIPT_NAME .. ': armed. Route: soft-reset the console now; let it '
      .. 'boot to the title, press Start, open the Library and go back, then '
      .. 'open the Free Duel menu. During every disc load watch the '
      .. 'bottom-right corner for a small 24x24 grey/icon sprite. The '
      .. 'document prints by itself ' .. (WINDOW_FRAMES / FPS)
      .. ' seconds after the reset.')
