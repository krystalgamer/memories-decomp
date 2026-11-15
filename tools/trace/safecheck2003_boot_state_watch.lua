-- safecheck2003_boot_state_watch.lua
--
-- WHAT THIS ANSWERS
--   The safechk boot module (Sony's anti-modchip check, notes/overlays/safecheck2003.md)
--   is WA_MRG module 0x16 at 0x80168000. Its 20 controller states are
--   described statically in that note. Main_RunBootSequence
--   (src/game/main_boot_frontend.c) calls SafetyInit once and then
--   `while (SafetySync(1) != 0) {}`: a busy loop, not one call per frame;
--   the controller's waits (states 9, 14, 19) poll VSync(-1). So: on a real
--   boot, which states does SafetySync walk through in which order and
--   for how long, which CD commands does SafetySendCommand send with which
--   parameters, what does SafetyPollResult read back, which region and message
--   set does SafetyInit select, and does the warning SafetyCheck2 ever
--   run? Each answer checks the static reading: the state order and the
--   command per state check the note's state table and the jump table at
--   0x80168004; the command bytes and results check SafetySendCommand as the
--   command sender, SafetyPollResult as the result poll and SafetyInit as
--   the region select;
--   a warning or helper hit (or its absence) is recorded for SafetyCheck2,
--   SafetyCheck3, SafetyCheck4, SafetyGetGlyphRange and SafetyCheck5.
--   The emulator answers like an unmodified console, so only the clean path
--   is observable here; the warning branches stay static evidence.
--
-- WHERE THE VALUES LIVE (from src/overlays/safecheck2003/safechk.c)
--   * State: the s32 safechk_state. SafetySync loads it at 0x80168178, bounds
--     it with sltiu 0x14 and indexes the jump table at 0x80168004; every case
--     stores the next state back there. SafetyInit writes it (1 or 0) and
--     the message index safechk_message_set (0, 1 or 2).
--   * SafetyInit lbu's the BIOS region byte 0xBFC7FF52 into $v1 and never
--     overwrites it, so $v1 at its only jr ra (0x80168158) is that byte; the
--     ROM itself is not read.
--   * SafetySync(a0): its return value is $s0, copied to $v0 in the delay
--     slot at 0x80168A78; every path reaches the jr ra at 0x80168A90, where
--     $v0 and safechk_state are read.
--   * SafetySendCommand(a0 = command index n): the command is entry n of
--     safechk_commands {cmd, nparams, nresults, intsum} (14 entries); the
--     parameter bytes are safechk_params[0..nparams), filled by the
--     controller before the call. All are read at the entry breakpoint.
--   * SafetyPollResult(a0 = n): a0 is overwritten on its error path, so n is
--     kept from the entry breakpoint. Its only jr ra is 0x80168C64 (nop delay
--     slot); there $v0 is the result (0 nothing yet, 1 done, -1 error, -2
--     error with status bit 4) and $a3 the interrupt cause (low three bits
--     of 0x1F801803). The result bytes are safechk_results[0..nresults), two
--     bytes for cause 5. Zero returns are only counted: the busy loop calls
--     it hundreds of thousands of times.
--   * SafetyCheck2 reads message safechk_messages[safechk_message_set]
--     (12-byte entries; text pointer at +8). The pointer is printed only
--     when it lies inside the module.
--
-- WHAT IS WATCHED
--   Exec breakpoints: Main_RunFrameServices (0x8001306C, control);
--   SafetyInit entry and exit; SafetySync entry and exit;
--   SafetyPollResult entry and exit; SafetySendCommand, SafetyCheck2,
--   SafetyCheck3, SafetyCheck4, SafetyGetGlyphRange and SafetyCheck5 entries.
--   Several modules share 0x80168000, so every hit in the module first checks
--   12 words of the North American image (module word, jump table, function
--   entries, exits, and the US VSync/ResetGraph jal words that differ in the
--   Japanese and PAL images). A mismatching hit is refused and only counted.
--   A controller row prints whenever (entry state, exit state, return value)
--   changes; repeats are counted. Output is bounded: 200 rows in total,
--   80 controller rows, 60 command rows, 6 non-zero poll rows per command,
--   8 refused-hit rows, 64 state-path entries.
--   A reset clears the per-boot capture (an earlier boot keeps a one-line
--   summary) and breakpoints are reinstalled at the BIOS shell or the first
--   vsync after the reset. The document prints 5 s after the controller
--   returns 0 or after the warning, or 120 s after the last reset (or
--   arming).
--
-- HOW TO RUN (about one minute)
--   1. PCSX-Redux with the North American disc, debugger enabled,
--      interpreter CPU (restart the emulator if you switch).
--   2. At the main menu, Debug -> Lua editor, paste this file. It prints
--      "armed".
--   3. Reset the console (soft or hard reset) and let the game boot by
--      itself to the title screen. The boot may run slower than usual.
--   4. The document prints by itself. Copy it into
--      tools/trace/result/safecheck2003_boot_state_watch.txt and fill in the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Soft or hard reset? Did a "SOFTWARE TERMINATED" warning appear, and did
--   the game reach the title screen?

local ffi = require('ffi')

local SCRIPT_NAME = 'safecheck2003_boot_state_watch'
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
-- The game uses 2 MB; reads fold the way the console mirrors them.
local RAM_SIZE = 0x200000
local FPS = 60

local MODULE = 0x80168000
local MODULE_END = 0x80169800

local STATE = 0x80169474        -- safechk_state, controller state
local MESSAGE_INDEX = 0x801694D0 -- safechk_message_set, set by SafetyInit
local COMMANDS = 0x8016941C     -- safechk_commands {cmd, nparams, ...}
local COMMAND_COUNT = 14
local PARAMS = 0x80169464       -- safechk_params, parameter bytes
local RESULTS = 0x8016946C      -- safechk_results, result bytes
local BUFFER_SIZE = 8
local MESSAGES = 0x8016947C     -- safechk_messages, 12-byte entries
local MESSAGE_COUNT = 2

-- Address, retail entry word, name.
local CONTROL = {0x8001306c, 0x27bdffe0, 'Main_RunFrameServices'}
local SELECT = {0x801680f4, 0x3c03bfc8, 'SafetyInit'}
local SELECT_EXIT = {0x80168158, 0x03e00008, 'SafetyInit jr ra'}
local CONTROLLER = {0x80168160, 0x27bdffe0, 'SafetySync'}
local CONTROLLER_EXIT = {0x80168a90, 0x03e00008, 'SafetySync jr ra'}
local POLL = {0x80168a98, 0x00041080, 'SafetyPollResult'}
local POLL_EXIT = {0x80168c64, 0x03e00008, 'SafetyPollResult jr ra'}
local SEND = {0x80168c6c, 0x00002821, 'SafetySendCommand'}
local WARNING = {0x80168d5c, 0x27bdff08, 'SafetyCheck2'}
local HELPERS = {
    {0x80168ec8, 0x27bdffd0, 'SafetyCheck3'},
    {0x8016903c, 0x27bdff90, 'SafetyCheck4'},
    {0x801691f0, 0x00003821, 'SafetyGetGlyphRange'},
    {0x80169284, 0x27bdffb0, 'SafetyCheck5'},
}

-- The North American image: entry words, exits, the jump table's first
-- two entries and two US-only resident jal targets.
local MODULE_SIGNATURE = {
    {0x80168000, 0x00000016, 'module word'},
    {0x80168004, 0x80168a70, 'jump table state 0'},
    {0x80168008, 0x801681a8, 'jump table state 1'},
    SELECT, SELECT_EXIT, CONTROLLER, CONTROLLER_EXIT, POLL, POLL_EXIT, SEND,
    {0x80168a30, 0x0c01d05c, 'US jal VSync'},
    {0x80168d6c, 0x0c01fcd4, 'US jal ResetGraph'},
}

-- Public CD-ROM command numbering, as a reading aid only.
local CD_HINT = {[0x01] = 'Getstat', [0x02] = 'Setloc', [0x03] = 'Play',
                 [0x09] = 'Pause', [0x0a] = 'Init', [0x0b] = 'Mute',
                 [0x0e] = 'Setmode', [0x13] = 'GetTN', [0x14] = 'GetTD',
                 [0x16] = 'SeekP', [0x19] = 'Test', [0x1a] = 'GetID',
                 [0x1e] = 'ReadTOC'}

local WINDOW_FRAMES = 120 * FPS
local GRACE_FRAMES = 5 * FPS
local MAX_ROWS = 200
local MAX_CONTROLLER_ROWS = 80
local MAX_SEND_ROWS = 60
local MAX_POLL_ROWS_PER_COMMAND = 6
local MAX_REFUSED_ROWS = 8
local MAX_PATH = 64
local MAX_EARLIER_BOOTS = 4

local mem = PCSX.getMemPtr()

-- Offset of a KUSEG/KSEG0/KSEG1 RAM address, or nil for anything else.
local function phys(addr, size)
    if type(addr) ~= 'number' or addr ~= addr then return nil end
    if addr < 0 or addr >= 0x100000000 or addr % 1 ~= 0 then return nil end
    local seg = addr - addr % 0x20000000
    if seg ~= 0 and seg ~= RAM_BASE and seg ~= 0xa0000000 then return nil end
    local off = addr % 0x20000000
    if off >= 0x800000 then return nil end
    off = off % RAM_SIZE
    if off % size ~= 0 or off + size > RAM_SIZE then return nil end
    return off
end
local function u8(addr)
    local off = phys(addr, 1)
    if mem == nil or off == nil then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + off)[0])
end
local function u32(addr)
    local off = phys(addr, 4)
    if mem == nil or off == nil then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + off)[0])
end
local function s32(v)
    if v == nil then return nil end
    if v >= 0x80000000 then return v - 0x100000000 end
    return v
end
local function hex(v, digits)
    if v == nil then return string.rep('?', digits or 2) end
    return string.format('%0' .. (digits or 2) .. 'X', v)
end
local function dec(v)
    if v == nil then return '?' end
    return tostring(v)
end

-- Capture state that spans the whole run.
local vsyncs, resets, controlTotal = 0, 0, 0
local rows, dropped = {}, 0
local refused, refusedRows, firstRefused = 0, 0, nil
local earlierBoots = {}
local callbackError = nil
local done = false
local reinstalled = true
local installNote = 'not installed'
local generation = 0

-- Per-boot capture, cleared at each reset.
local boot
local function newBoot()
    boot = {
        startVs = vsyncs, control = 0, controlAfterDone = 0, verified = 0,
        select = nil, selectCalls = 0,
        controllerCalls = 0, controllerRows = 0, lastTuple = nil, repeats = 0,
        entryState = nil, path = {}, pathDropped = 0, lastPathState = nil,
        stateCalls = {}, firstState = nil, finalState = nil,
        doneAt = nil, sends = 0, sendRows = 0, sendCounts = {},
        pollN = nil, polls = {}, warning = nil, warningAt = nil, helpers = {},
    }
end
newBoot()

breakpoints_safecheck2003_boot_state_watch = {}

local function stamp()
    return string.format('b=%d vs=%d t=%6.1fs bt=%5.1fs', resets, vsyncs,
                         vsyncs / FPS, (vsyncs - boot.startVs) / FPS)
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

local function checkWords(list)
    local bad = {}
    for _, sig in ipairs(list) do
        local word = u32(sig[1])
        if word ~= sig[2] then
            bad[#bad + 1] = string.format('%s 0x%08X expected 0x%08X read %s',
                                          sig[3], sig[1], sig[2],
                                          hex(word, 8))
        end
    end
    return bad
end

-- Every hit inside 0x80168000 passes through here first.
local function moduleIsUs(label)
    local bad = checkWords(MODULE_SIGNATURE)
    if #bad == 0 then
        boot.verified = boot.verified + 1
        return true
    end
    refused = refused + 1
    local text = string.format('refused %s: 0x80168000 does not hold the US '
                               .. 'safechk image (%d/%d words differ; %s)', label,
                               #bad, #MODULE_SIGNATURE, bad[1])
    if firstRefused == nil then firstRefused = text end
    if refusedRows < MAX_REFUSED_ROWS then
        refusedRows = refusedRows + 1
        live(stamp() .. ' ' .. text)
    else
        dropped = dropped + 1
    end
    return false
end

local function regs()
    local r = PCSX.getRegisters()
    local n = r.GPR.n
    local function g(name) return tonumber(n[name]) end
    return g, tonumber(r.pc)
end

-- One safechk_commands entry, or nil when the index is out of range.
local function command(n)
    if type(n) ~= 'number' or n < 0 or n >= COMMAND_COUNT then return nil end
    local base = COMMANDS + 4 * n
    local c = {cmd = u8(base), nparams = u8(base + 1),
               nresults = u8(base + 2), intsum = u8(base + 3)}
    if c.cmd == nil then return nil end
    return c
end
local function bytes(addr, count)
    local parts = {}
    count = math.min(count or 0, BUFFER_SIZE)
    for k = 0, count - 1 do parts[#parts + 1] = hex(u8(addr + k)) end
    if #parts == 0 then return '-' end
    return table.concat(parts, ' ')
end
local function commandText(n, c)
    if c == nil then
        return string.format('n=%s index refused (outside 0..%d)', dec(n),
                             COMMAND_COUNT - 1)
    end
    return string.format('n=%d cmd=0x%s(%s) nparams=%s nresults=%s intsum=%s',
                         n, hex(c.cmd), CD_HINT[c.cmd] or '?', dec(c.nparams),
                         dec(c.nresults), dec(c.intsum))
end

local function noteState(s)
    if s == boot.lastPathState then return end
    boot.lastPathState = s
    if #boot.path < MAX_PATH then
        boot.path[#boot.path + 1] = string.format('%s@%.1fs', dec(s),
                                                  (vsyncs - boot.startVs) / FPS)
    else
        boot.pathDropped = boot.pathDropped + 1
    end
end

local function onControl()
    controlTotal = controlTotal + 1
    boot.control = boot.control + 1
    if boot.doneAt ~= nil then
        boot.controlAfterDone = boot.controlAfterDone + 1
    end
end

local function onSelect()
    if not moduleIsUs(SELECT[3]) then return end
    local g = regs()
    boot.selectCalls = boot.selectCalls + 1
    boot.selectRa = g('ra')
    live(string.format('%s %s entry ra=0x%s state=%s', stamp(), SELECT[3],
                       hex(boot.selectRa, 8), dec(s32(u32(STATE)))))
end

local function onSelectExit()
    if not moduleIsUs(SELECT_EXIT[3]) then return end
    local g = regs()
    local region = g('v1')
    if region ~= nil then region = region % 0x100 end
    local ch = (region ~= nil and region >= 0x20 and region < 0x7f)
        and string.char(region) or '?'
    boot.select = {region = region, ch = ch, state = s32(u32(STATE)),
                   mode = s32(u32(MESSAGE_INDEX))}
    noteState(boot.select.state)
    live(string.format('%s %s exit region_byte=0x%s(%s) state=%s '
                       .. 'message_index=%s', stamp(), SELECT[3], hex(region),
                       ch, dec(boot.select.state), dec(boot.select.mode)))
end

local function onControllerEntry()
    if not moduleIsUs(CONTROLLER[3]) then return end
    local g = regs()
    local s = s32(u32(STATE))
    boot.controllerCalls = boot.controllerCalls + 1
    boot.entryState = s
    boot.entryArg = g('a0')
    if boot.firstState == nil then boot.firstState = s end
    local key = dec(s)
    boot.stateCalls[key] = (boot.stateCalls[key] or 0) + 1
    noteState(s)
end

local function onControllerExit()
    if not moduleIsUs(CONTROLLER_EXIT[3]) then return end
    local g = regs()
    local ret = s32(g('v0'))
    local s = s32(u32(STATE))
    boot.finalState = s
    noteState(s)
    local tuple = string.format('%s>%s:%s', dec(boot.entryState), dec(s),
                                dec(ret))
    if tuple ~= boot.lastTuple then
        local text = string.format('%s %s a0=%s state %s -> %s ret=%s '
                                   .. 'calls=%d repeats_before=%d', stamp(),
                                   CONTROLLER[3], dec(boot.entryArg),
                                   dec(boot.entryState), dec(s), dec(ret),
                                   boot.controllerCalls, boot.repeats)
        boot.lastTuple = tuple
        boot.repeats = 0
        if boot.controllerRows < MAX_CONTROLLER_ROWS then
            boot.controllerRows = boot.controllerRows + 1
            live(text)
        else
            dropped = dropped + 1
        end
    else
        boot.repeats = boot.repeats + 1
    end
    if ret == 0 and boot.doneAt == nil then
        boot.doneAt = vsyncs
        live(string.format('%s %s returned 0 after %d calls: safechk finished; '
                           .. 'the document follows in %d s', stamp(),
                           CONTROLLER[3], boot.controllerCalls,
                           GRACE_FRAMES / FPS))
    end
end

local function onSend()
    if not moduleIsUs(SEND[3]) then return end
    local g = regs()
    local n = s32(g('a0'))
    local c = command(n)
    boot.sends = boot.sends + 1
    local key = dec(n)
    boot.sendCounts[key] = (boot.sendCounts[key] or 0) + 1
    if boot.sendRows < MAX_SEND_ROWS then
        boot.sendRows = boot.sendRows + 1
        live(string.format('%s %s %s params=%s ra=0x%s state=%s', stamp(),
                           SEND[3], commandText(n, c),
                           c and bytes(PARAMS, c.nparams) or '-',
                           hex(g('ra'), 8), dec(s32(u32(STATE)))))
    else
        dropped = dropped + 1
    end
end

local function onPoll()
    if not moduleIsUs(POLL[3]) then return end
    local g = regs()
    boot.pollN = s32(g('a0'))
end

local function onPollExit()
    if not moduleIsUs(POLL_EXIT[3]) then return end
    local g = regs()
    local n = boot.pollN
    local ret = s32(g('v0'))
    local cause = g('a3')
    if cause ~= nil then cause = cause % 8 end
    local key = dec(n)
    local p = boot.polls[key]
    if p == nil then
        p = {zero = 0, ok = 0, err = 0, err2 = 0, other = 0, rows = 0}
        boot.polls[key] = p
    end
    if ret == 0 then p.zero = p.zero + 1 return end
    if ret == 1 then p.ok = p.ok + 1
    elseif ret == -1 then p.err = p.err + 1
    elseif ret == -2 then p.err2 = p.err2 + 1
    else p.other = p.other + 1 end
    if p.rows >= MAX_POLL_ROWS_PER_COMMAND then
        dropped = dropped + 1
        return
    end
    p.rows = p.rows + 1
    local c = command(n)
    local count = cause == 5 and 2 or (c and c.nresults) or 0
    live(string.format('%s %s %s ret=%s cause=%s results=%s zero_polls=%d '
                       .. 'state=%s', stamp(), POLL[3], commandText(n, c),
                       dec(ret), dec(cause), bytes(RESULTS, count), p.zero,
                       dec(s32(u32(STATE)))))
end

-- The message text, only through a pointer inside the module.
local function messageText(index)
    if type(index) ~= 'number' or index < 0 or index >= MESSAGE_COUNT then
        return string.format('message index %s refused', dec(index))
    end
    local ptr = u32(MESSAGES + 12 * index + 8)
    if ptr == nil or ptr < MODULE or ptr >= MODULE_END then
        return string.format('text pointer 0x%s refused (outside the module)',
                             hex(ptr, 8))
    end
    local out = {}
    for k = 0, 95 do
        if ptr + k >= MODULE_END then break end
        local b = u8(ptr + k)
        if b == nil or b == 0 then break end
        if b >= 0x20 and b < 0x7f then
            out[#out + 1] = string.char(b)
        elseif b == 0x0a then
            out[#out + 1] = ' / '
        else
            out[#out + 1] = string.format('\\x%02X', b)
        end
    end
    return string.format('text@0x%08X "%s"', ptr, table.concat(out))
end

local function onWarning()
    if not moduleIsUs(WARNING[3]) then return end
    local g = regs()
    local index = s32(u32(MESSAGE_INDEX))
    boot.warning = string.format('ra=0x%s state=%s message_index=%s %s',
                                 hex(g('ra'), 8), dec(s32(u32(STATE))),
                                 dec(index), messageText(index))
    if boot.warningAt == nil then boot.warningAt = vsyncs end
    live(string.format('%s WARNING %s entry %s', stamp(), WARNING[3],
                       boot.warning))
end

local function onHelper(sig)
    return function()
        if not moduleIsUs(sig[3]) then return end
        local n = (boot.helpers[sig[3]] or 0) + 1
        boot.helpers[sig[3]] = n
        if n == 1 then
            local g = regs()
            live(string.format('%s %s first call ra=0x%s', stamp(), sig[3],
                               hex(g('ra'), 8)))
        end
    end
end

local function guarded(handler)
    return function(...)
        if done then return end
        local ok, err = pcall(handler, ...)
        if not ok and callbackError == nil then callbackError = tostring(err) end
    end
end

local function add(sig, handler)
    local list = breakpoints_safecheck2003_boot_state_watch
    local mine = generation
    list[#list + 1] = traceAddBreakpoint(sig[1], 'Exec', 4,
                                         SCRIPT_NAME .. ': ' .. sig[3],
                                         guarded(function(...)
        if mine == generation then handler(...) end
    end))
end

local function installBreakpoints(note)
    generation = generation + 1
    breakpoints_safecheck2003_boot_state_watch = {}
    add(CONTROL, onControl)
    add(SELECT, onSelect)
    add(SELECT_EXIT, onSelectExit)
    add(CONTROLLER, onControllerEntry)
    add(CONTROLLER_EXIT, onControllerExit)
    add(POLL, onPoll)
    add(POLL_EXIT, onPollExit)
    add(SEND, onSend)
    add(WARNING, onWarning)
    for _, sig in ipairs(HELPERS) do add(sig, onHelper(sig)) end
    installNote = note
    print(string.format('%s: %d breakpoints %s', SCRIPT_NAME,
                        #breakpoints_safecheck2003_boot_state_watch, note))
end

local function sortedKeys(t)
    local keys = {}
    for k in pairs(t) do keys[#keys + 1] = k end
    table.sort(keys, function(a, b)
        local x, y = tonumber(a), tonumber(b)
        if x and y then return x < y end
        return tostring(a) < tostring(b)
    end)
    return keys
end

local function safecheck2003Hits(b)
    return b.selectCalls + b.controllerCalls + b.sends
        + (b.warning and 1 or 0)
end

local function bootSummary(b)
    return string.format('select=%s controller_calls=%d sends=%d path=%s%s '
                         .. 'returned_zero=%s warning=%s',
                         b.select and string.format('region 0x%s state %s '
                             .. 'message %s', hex(b.select.region),
                             dec(b.select.state), dec(b.select.mode))
                             or 'not seen',
                         b.controllerCalls, b.sends,
                         #b.path > 0 and table.concat(b.path, '>') or '-',
                         b.pathDropped > 0 and ('>+' .. b.pathDropped) or '',
                         b.doneAt and 'yes' or 'no',
                         b.warning and 'RAN' or 'no')
end

local function verdict()
    local any = controlTotal + refused + safecheck2003Hits(boot)
    if any == 0 and not traceExecutionObserved then
        return 'nothing fired, not even the Main_RunFrameServices control: '
            .. 'the setup failed (debugger off, dynarec still running, or '
            .. 'breakpoints lost after a reset without a reinstall line); '
            .. 'silence is not evidence'
    end
    if callbackError ~= nil then
        return 'inconclusive: a callback failed'
    end
    if resets == 0 and safecheck2003Hits(boot) == 0 then
        return 'inconclusive: no reset observed after arming; safechk only runs '
            .. 'while the game boots, so reset the console after pasting'
    end
    if safecheck2003Hits(boot) == 0 then
        if refused > 0 then
            return string.format('inconclusive: only code other than the US '
                .. 'safechk image ran at the watched addresses (%d refused '
                .. 'hits); wrong disc or region?', refused)
        end
        return string.format('inconclusive: no safechk breakpoint fired after '
            .. 'the reset (control hits this boot=%d)', boot.control)
    end
    if boot.warning ~= nil then
        return 'the warning SafetyCheck2 RAN (state '
            .. dec(boot.finalState) .. '); see its row'
    end
    if boot.doneAt ~= nil then
        return string.format('safechk completed on the unmodified path: '
            .. 'SafetyInit %s, SafetySync returned 0 after %d calls at '
            .. 'bt=%.1fs, %d CD commands sent, the warning never ran; the '
            .. 'game ran %d frame services afterwards',
            boot.select and 'ran' or 'NOT seen', boot.controllerCalls,
            (boot.doneAt - boot.startVs) / FPS, boot.sends,
            boot.controlAfterDone)
    end
    return 'inconclusive: safechk ran but SafetySync never returned 0 '
        .. 'within the window (last state ' .. dec(boot.finalState) .. ')'
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_safecheck2003_boot_state_watch) do
        pcall(function() bp:disable() end)
    end
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<soft or hard reset? did a "SOFTWARE TERMINATED" warning appear,')
    print(' and did the game reach the title screen?>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print('breakpoints: ' .. installNote)
    print(string.format('vsyncs: %d resets: %d control_hits: %d '
                        .. 'control_hits_this_boot: %d', vsyncs, resets,
                        controlTotal, boot.control))
    print(string.format('signature: verified_hits_this_boot=%d '
                        .. 'refused_hits=%d', boot.verified, refused))
    if firstRefused ~= nil then print('first_refused: ' .. firstRefused) end
    print('verdict: ' .. verdict())
    if boot.select ~= nil then
        print(string.format('SafetyInit: calls=%d ra=0x%s '
                            .. 'region_byte=0x%s(%s) state=%s '
                            .. 'message_index=%s', boot.selectCalls,
                            hex(boot.selectRa, 8), hex(boot.select.region),
                            boot.select.ch, dec(boot.select.state),
                            dec(boot.select.mode)))
    else
        print(string.format('SafetyInit: exit not seen (entries=%d)',
                            boot.selectCalls))
    end
    print(string.format('SafetySync: calls=%d first_state=%s '
                        .. 'final_state=%s returned_zero=%s', boot.controllerCalls,
                        dec(boot.firstState), dec(boot.finalState),
                        boot.doneAt and string.format('yes at bt=%.1fs',
                            (boot.doneAt - boot.startVs) / FPS) or 'no'))
    print('state_path: ' .. (#boot.path > 0 and table.concat(boot.path, ' > ')
                             or '-')
          .. (boot.pathDropped > 0 and (' > +' .. boot.pathDropped
                                         .. ' more') or ''))
    local parts = {}
    for _, k in ipairs(sortedKeys(boot.stateCalls)) do
        parts[#parts + 1] = k .. ':' .. boot.stateCalls[k]
    end
    print('state_entry_calls: ' .. (#parts > 0 and table.concat(parts, ' ')
                                    or '-'))
    parts = {}
    for _, k in ipairs(sortedKeys(boot.sendCounts)) do
        local c = command(tonumber(k))
        parts[#parts + 1] = string.format('%s(0x%s %s)x%d', k,
            c and hex(c.cmd) or '??', c and (CD_HINT[c.cmd] or '?') or '?',
            boot.sendCounts[k])
    end
    print(string.format('SafetySendCommand: sends=%d %s', boot.sends,
                        #parts > 0 and table.concat(parts, ' ') or '-'))
    for _, k in ipairs(sortedKeys(boot.polls)) do
        local p = boot.polls[k]
        print(string.format('SafetyPollResult: n=%s zero=%d ok=%d err=%d '
                            .. 'err_status=%d other=%d', k, p.zero, p.ok,
                            p.err, p.err2, p.other))
    end
    print('SafetyCheck2: ' .. (boot.warning or 'never ran'))
    parts = {}
    for _, sig in ipairs(HELPERS) do
        parts[#parts + 1] = sig[3] .. '=' .. (boot.helpers[sig[3]] or 0)
    end
    print('helpers: ' .. table.concat(parts, ' '))
    for i, line in ipairs(earlierBoots) do
        print(string.format('earlier_boot %d: %s', i, line))
    end
    print(string.format('bounds: rows=%d/%d dropped_rows=%d '
                        .. 'controller_rows=%d/%d send_rows=%d/%d '
                        .. 'refused_rows=%d/%d', #rows, MAX_ROWS, dropped,
                        boot.controllerRows, MAX_CONTROLLER_ROWS,
                        boot.sendRows, MAX_SEND_ROWS, refusedRows,
                        MAX_REFUSED_ROWS))
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
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
    if boot.doneAt ~= nil and vsyncs - boot.doneAt >= GRACE_FRAMES then
        finish(string.format('SafetySync returned 0 and %d s passed',
                             GRACE_FRAMES / FPS))
    elseif boot.warningAt ~= nil and vsyncs - boot.warningAt >= GRACE_FRAMES then
        finish(string.format('the warning ran and %d s passed',
                             GRACE_FRAMES / FPS))
    elseif vsyncs - boot.startVs >= WINDOW_FRAMES then
        finish(string.format('%d-second window since the %s ended',
                             WINDOW_FRAMES / FPS,
                             resets > 0 and 'last reset' or 'arming'))
    end
end

listeners_safecheck2003_boot_state_watch = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        local ok, err = pcall(function()
            if done then return end
            -- PCSX-Redux drops Lua breakpoints on reset; each boot is its
            -- own capture.
            breakpoints_safecheck2003_boot_state_watch = {}
            generation = generation + 1
            if safecheck2003Hits(boot) > 0 and #earlierBoots < MAX_EARLIER_BOOTS then
                earlierBoots[#earlierBoots + 1] = bootSummary(boot)
            end
            resets = resets + 1
            newBoot()
            reinstalled = false
            installNote = 'lost at reset, not reinstalled'
            live(string.format('%s reset observed; per-boot capture cleared, '
                               .. 'waiting for the BIOS shell', stamp()))
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
do
    local bad = checkWords({CONTROL})
    print(string.format('%s: control %s entry word %s', SCRIPT_NAME,
                        CONTROL[3], #bad == 0 and 'ok' or ('MISMATCH: '
                                                           .. bad[1])))
    local moduleBad = checkWords(MODULE_SIGNATURE)
    print(string.format('%s: 0x80168000 now holds %s', SCRIPT_NAME,
                        #moduleBad == 0 and 'the US safechk image'
                            or 'another module (expected at the main menu)'))
end
print(SCRIPT_NAME .. ': armed. Route: reset the console now (soft or hard '
      .. 'reset) and let the game boot by itself to the title screen; the '
      .. 'boot may run slower than usual. The document prints by itself '
      .. (GRACE_FRAMES / FPS) .. ' s after the safechk check finishes, or '
      .. (WINDOW_FRAMES / FPS) .. ' s after the last reset.')
