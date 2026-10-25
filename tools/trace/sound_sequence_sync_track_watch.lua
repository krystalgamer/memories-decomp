-- sound_sequence_sync_track_watch.lua
--
-- WHAT THIS ANSWERS
--   SDSecondaryState.field_07F8 (*gSD_pSecondaryState + 0x7F8, u16) is read
--   in two places, and SD_ReadSequenceHeader is the only static writer,
--   storing 0:
--     * SD_ProcessSequenceTracks advances sequence_tick_count (+0x7F0) only
--       on ticks of the track whose index equals field_07F8;
--     * SD_DispatchSequenceChannelEvent, on loop-start/loop-end controls
--       (control 0x63, values 0x14/0x1E) and the following data entry,
--       saves and restores every track together while the field is nonzero,
--       and only the event's own track while it is zero.
--   Either the field never leaves zero (track 0 keeps time, loops are per
--   track), or something such as a store whose address is computed changes
--   it. This trace does not assume the nonzero path is reached. It reports
--   every store that hits the field, every change seen by polling, and
--   which loop branch each loop control actually took.
--
--   Every row is printed the moment it happens with a running #NNN marker and
--   a time stamp, so it can be matched to what was playing:
--     * OPEN/PLAY rows: SD_OpenSequence (data tag and source),
--       SD_PlaySequence and SD_PlaySequenceFastForward, with callers,
--     * HEADER rows: each SD_ReadSequenceHeader, then a SEQ row with
--       track_count, timebase and field_07F8 once the header is read,
--     * STORE rows: every store that hits field_07F8 (writer, old, new),
--     * FIELD/TRACKS rows: any polled change of field_07F8 or track_count,
--     * LOOP rows: the first loop control per sequence for each kind, track
--       and branch; repeats are counted in the final document. The branch
--       is read from the same guards the dispatcher tests: a loop end with
--       loop_count 0, or controller 6 while the channel's mode is not 0x14,
--       is reported as not-taken rather than as a loop branch.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file before the title screen if you
--      can, or at the main menu.
--   3. Stay on the main menu for about two minutes, so its music reaches
--      its loop point at least once. Then start a Free Duel and wait about
--      two more minutes in the duel without leaving.
--   4. It prints its document after 8 sequences, five minutes after the
--      first sequence header, after 12 minutes in total, or on reset.
--   5. Copy the whole document into
--      tools/trace/result/sound_sequence_sync_track_watch.txt and fill in the
--      context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   One line per SEQ row: which screen you were on and whether the music
--   restarted or looped audibly (roughly when).

local ffi = require('ffi')

local SCRIPT_NAME = 'sound_sequence_sync_track_watch'
-- trace_setup_block.lua
--
-- The canonical TRACE_SETUP block. Every trace that installs a breakpoint
-- copies everything below this header verbatim, directly below its
-- SCRIPT_NAME line. trace_setup_preflight_test.lua checks each script's copy
-- against this file and exercises the block on its own, so it stays tested
-- even while no trace script is waiting to be run.

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
local MAIN_MODE = 0x8009b26c
local STATE_PTR = 0x8009b458
local STATE_SIZE = 0x804
local TRACKS = 0x518
local TRACK_SIZE = 0x2c
local TRACK_SLOTS = 16
local SEQUENCE_STATE = 0x7e2
local SOURCE = 0x7e8
local TICKS = 0x7f0
local FIELD = 0x7f8
local TRACK_COUNT = 0x7fa
local TIMEBASE = 0x7fc
local CONTROL_MODE, LOOP_START, LOOP_END = 0x63, 0x14, 0x1e
local DATA_ENTRY = 6
local TRACK_LOOP_COUNT = 0x26
local CHANNEL_SIZE = 0x18
local CHANNEL_CONTROL_MODE = 0x12

-- {address, first instruction word, name}
local CONTROL = {0x8001306c, 0x27bdffe0, 'control Main_RunFrameServices'}
local OPEN = {0x80049a64, 0x00803021, 'SD_OpenSequence'}
local PLAY = {0x80049af4, 0x3c02800a, 'SD_PlaySequence'}
local FAST_FORWARD = {0x80049bac, 0x3c02800a, 'SD_PlaySequenceFastForward'}
local HEADER = {0x8004bce8, 0x3c03800a, 'SD_ReadSequenceHeader'}
local DISPATCH = {0x8004c114, 0x27bdffe8, 'SD_DispatchSequenceChannelEvent'}
local PROCESS = {0x8004c8c8, 0x3c02800a, 'SD_ProcessSequenceTracks'}

local FUNCTIONS = {
    {0x80045514, 0x6d4, 'SD_StepActiveCommand'},
    {0x80048f14, 0xfc, 'SD_InitSecondaryRuntime'},
    {0x80049a64, 0x90, 'SD_OpenSequence'},
    {0x80049af4, 0xb8, 'SD_PlaySequence'},
    {0x80049bac, 0x94, 'SD_PlaySequenceFastForward'},
    {0x8004bce8, 0x184, 'SD_ReadSequenceHeader'},
    {0x8004be88, 0x224, 'SD_HandleSequenceMetaEvent'},
    {0x8004c114, 0x30c, 'SD_DispatchSequenceChannelEvent'},
    {0x8004c420, 0x140, 'SD_ReadSequenceEvent'},
    {0x8004c77c, 0xd0, 'SD_StartSequenceTracks'},
    {0x8004c8c8, 0x198, 'SD_ProcessSequenceTracks'},
}

local MAX_SEQUENCES = 8
local MAX_ROWS = 400
local MAX_STORE_ROWS = 80
local MAX_LOOP_ROWS = 120
local WINDOW_FRAMES = 300 * FPS
local NO_CONTROL_FRAMES = 300 * FPS
local TIMEOUT_FRAMES = 720 * FPS
local STORE_WIDTH = {[0x28] = 1, [0x29] = 2, [0x2b] = 4}

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr ~= nil and addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if mem == nil or not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function u16(addr)
    if mem == nil or not inRam(addr, 2) or addr % 2 ~= 0 then return nil end
    return tonumber(ffi.cast('uint16_t*', mem + (addr - RAM_BASE))[0])
end
local function u32(addr)
    if mem == nil or not inRam(addr, 4) or addr % 4 ~= 0 then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local function show(v, fmt)
    if v == nil then return 'unreadable' end
    return string.format(fmt or '%d', v)
end

local frames = 0
local marker = 0
local rows, dropped = {}, 0
local storeRows, loopRows = 0, 0
local sequence = 0
local seen = {}
local firstHeaderFrame = nil
local pendingHeader = nil
local controlHits, processCalls, headers, storeCount = 0, 0, 0, 0
local undecoded, nonzeroStores, nonzeroPolls = 0, 0, 0
local lastField, lastTracks = nil, nil
local watched = nil
local loops, loopOrder = {}, {}
local mismatches = {}
local callbackError = nil
local done = false

breakpoints_sound_sequence_sync_track_watch = {}

local function stamp()
    return string.format('t=%6.1fs', frames / FPS)
end

local function live(text)
    marker = marker + 1
    local line = string.format('#%03d %s Q%02d %s', marker, stamp(), sequence,
                               text)
    print(line)
    if #rows < MAX_ROWS then rows[#rows + 1] = line else dropped = dropped + 1 end
    return marker
end

local function functionName(pc)
    if pc == nil then return 'unknown' end
    for _, f in ipairs(FUNCTIONS) do
        if pc >= f[1] and pc < f[1] + f[2] then return f[3] end
    end
    if pc >= 0x800906e0 and pc < RAM_END then return 'overlay' end
    return 'unknown'
end

local function caller(ra)
    return string.format('%s(0x%08X)', functionName(ra), ra)
end

local function state()
    local p = u32(STATE_PTR)
    if p == nil or p % 4 ~= 0 or not inRam(p, STATE_SIZE) then return nil end
    return p
end

local function tagText(addr)
    local parts = {}
    for k = 0, 3 do
        local c = u8(addr + k)
        if c == nil then return 'unreadable' end
        parts[#parts + 1] = (c >= 0x20 and c < 0x7f) and string.char(c)
            or string.format('\\x%02X', c)
    end
    return table.concat(parts)
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_sound_sequence_sync_track_watch) do
        pcall(function() bp:disable() end)
    end
    local base = state()
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<one line per SEQ row: the screen, and whether the music restarted')
    print(' or looped audibly, roughly when>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d control_hits: %d headers: %d '
                        .. 'process_calls: %d field_stores: %d '
                        .. 'nonzero_stores: %d undecoded_stores: %d '
                        .. 'nonzero_polls: %d dropped_rows: %d', frames,
                        controlHits, headers, processCalls, storeCount,
                        nonzeroStores, undecoded, nonzeroPolls, dropped))
    print(string.format('state: 0x%s field_07F8=%s track_count=%s '
                        .. 'watch=%s', show(base, '%08X'),
                        show(base and u16(base + FIELD)),
                        show(base and u16(base + TRACK_COUNT)),
                        watched and string.format('0x%08X', watched)
                            or 'not installed'))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    print('')
    for _, key in ipairs(loopOrder) do
        print(string.format('loop %s count=%d', key, loops[key]))
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

-- A write breakpoint fires before the store, so memory still holds the old
-- value. The store is decoded from the instruction at pc.
local function onStore()
    local regs = PCSX.getRegisters()
    local pc = tonumber(regs.pc)
    local w = u32(pc)
    storeCount = storeCount + 1
    local width = w and STORE_WIDTH[math.floor(w / 0x4000000)]
    local fn = functionName(pc)
    if width == nil then
        undecoded = undecoded + 1
        if storeRows < MAX_STORE_ROWS then
            storeRows = storeRows + 1
            live(string.format('STORE undecoded by %s pc=0x%08X', fn, pc))
        end
        return
    end
    local base = tonumber(regs.GPR.r[math.floor(w / 0x200000) % 32])
    local value = tonumber(regs.GPR.r[math.floor(w / 0x10000) % 32])
    local imm = w % 0x10000
    if imm >= 0x8000 then imm = imm - 0x10000 end
    local addr = (base + imm) % 0x100000000
    local old = u16(watched)
    local new = old or 0
    for b = 0, width - 1 do
        local k = addr + b - watched
        if k == 0 or k == 1 then
            local byte = math.floor(value / 2 ^ (8 * b)) % 0x100
            new = k == 0 and (new - new % 0x100 + byte)
                or (new % 0x100 + byte * 0x100)
        end
    end
    if new ~= 0 then nonzeroStores = nonzeroStores + 1 end
    if storeRows < MAX_STORE_ROWS then
        storeRows = storeRows + 1
        live(string.format('STORE field_07F8 %s -> %d width=%d by %s '
                           .. 'pc=0x%08X', show(old), new, width, fn, pc))
    end
end

local function installWatch(base)
    if watched ~= nil then return end
    watched = base + FIELD
    breakpoints_sound_sequence_sync_track_watch[
        #breakpoints_sound_sequence_sync_track_watch + 1] =
        traceAddBreakpoint(watched, 'Write', 2,
                           SCRIPT_NAME .. ': field_07F8', function()
            if done then return end
            local ok, err = pcall(onStore)
            if not ok and callbackError == nil then
                callbackError = tostring(err)
            end
        end)
    live(string.format('WATCH field_07F8 at 0x%08X value=%s', watched,
                       show(u16(watched))))
end

local function onOpen()
    local n = PCSX.getRegisters().GPR.n
    local input = tonumber(n.a0)
    live(string.format('OPEN source=0x%08X tag=%s vab=%d caller=%s mode=%s',
                       input, tagText(input), tonumber(n.a1) % 0x10000,
                       caller(tonumber(n.ra)), show(u8(MAIN_MODE))))
end

local function onPlay(name)
    local n = PCSX.getRegisters().GPR.n
    live(string.format('PLAY %s arg=0x%X caller=%s', name, tonumber(n.a0),
                       caller(tonumber(n.ra))))
end

local function onHeader()
    headers = headers + 1
    firstHeaderFrame = firstHeaderFrame or frames
    sequence = sequence + 1
    seen = {}
    local base = state()
    pendingHeader = live(string.format('HEADER source=%s caller=%s',
                         show(base and u32(base + SOURCE), '0x%08X'),
                         caller(tonumber(PCSX.getRegisters().GPR.n.ra))))
end

local function onDispatch()
    local n = PCSX.getRegisters().GPR.n
    local status = tonumber(n.a1) % 0x100
    local d1, d2 = tonumber(n.a2) % 0x100, tonumber(n.a3) % 0x100
    if status - status % 0x10 ~= 0xb0 then return end
    local kind
    if d1 == CONTROL_MODE and d2 == LOOP_START then
        kind = 'start'
    elseif d1 == CONTROL_MODE and d2 == LOOP_END then
        kind = 'end'
    elseif d1 == DATA_ENTRY then
        kind = 'data_entry'
    else
        return
    end
    local base = state()
    local field = base and u16(base + FIELD)
    local track = '?'
    local p = tonumber(n.a0)
    if base ~= nil and p >= base + TRACKS
        and p < base + TRACKS + TRACK_SLOTS * TRACK_SIZE
        and (p - base - TRACKS) % TRACK_SIZE == 0 then
        track = tostring((p - base - TRACKS) / TRACK_SIZE)
    end
    -- The breakpoint runs before the dispatcher, so these are the values its
    -- guards test: a loop end restores only with a nonzero loop_count, and
    -- controller 6 is loop data only while the channel's mode is 0x14.
    local ch = status % 0x10
    local loopCount = u8(p + TRACK_LOOP_COUNT)
    local mode = base and u8(base + ch * CHANNEL_SIZE + CHANNEL_CONTROL_MODE)
    local branch
    if kind == 'end' and loopCount == nil
        or kind == 'data_entry' and mode == nil then
        branch = 'unknown'
    elseif kind == 'end' and loopCount == 0 then
        branch = 'not-taken:loop_count=0'
    elseif kind == 'data_entry' and mode ~= LOOP_START then
        branch = string.format('not-taken:mode=0x%02X', mode)
    elseif field == nil then
        branch = 'unknown'
    else
        branch = field ~= 0 and 'all-tracks' or 'own-track'
    end
    local key = string.format('Q%02d %s track=%s ch=%d value=%d branch=%s',
                              sequence, kind, track, ch, d2, branch)
    if loops[key] == nil then
        loops[key] = 0
        loopOrder[#loopOrder + 1] = key
    end
    loops[key] = loops[key] + 1
    local seenKey = kind .. track .. branch
    if seen[seenKey] or loopRows >= MAX_LOOP_ROWS then return end
    seen[seenKey] = true
    loopRows = loopRows + 1
    live(string.format('LOOP %s track=%s ch=%d value=%d field_07F8=%s '
                       .. 'loop_count=%s mode=%s branch=%s ticks=%s', kind,
                       track, ch, d2, show(field), show(loopCount),
                       show(mode, '0x%02X'), branch,
                       show(base and u32(base + TICKS))))
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    local base = state()
    if base ~= nil then
        installWatch(base)
        local field = u16(base + FIELD)
        local tracks = u16(base + TRACK_COUNT)
        if pendingHeader ~= nil then
            live(string.format('SEQ from #%03d tracks=%s timebase=%s '
                               .. 'field_07F8=%s state=%s', pendingHeader,
                               show(tracks), show(u16(base + TIMEBASE)),
                               show(field),
                               show(u16(base + SEQUENCE_STATE))))
            pendingHeader = nil
        else
            if lastField ~= nil and field ~= lastField then
                live(string.format('FIELD field_07F8 %s -> %s', show(lastField),
                                   show(field)))
            end
            if lastTracks ~= nil and tracks ~= lastTracks then
                live(string.format('TRACKS track_count %s -> %s',
                                   show(lastTracks), show(tracks)))
            end
        end
        if field ~= nil and field ~= 0 then nonzeroPolls = nonzeroPolls + 1 end
        lastField, lastTracks = field, tracks
    end
    if headers >= MAX_SEQUENCES and pendingHeader == nil then
        finish('captured ' .. MAX_SEQUENCES .. ' sequences')
    elseif firstHeaderFrame ~= nil
        and frames - firstHeaderFrame >= WINDOW_FRAMES then
        finish('five-minute window after the first sequence header ended')
    elseif frames >= NO_CONTROL_FRAMES and controlHits == 0 then
        finish('five minutes without a control hit; the setup is not '
               .. 'working, check the setup line')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

local function guarded(handler)
    return function()
        if done then return end
        local ok, err = pcall(handler)
        if not ok and callbackError == nil then callbackError = tostring(err) end
    end
end

local function add(target, handler)
    breakpoints_sound_sequence_sync_track_watch[
        #breakpoints_sound_sequence_sync_track_watch + 1] =
        traceAddBreakpoint(target[1], 'Exec', 4, SCRIPT_NAME .. ': ' .. target[3],
                           guarded(handler))
end

local function check(target)
    local word = u32(target[1])
    if word ~= target[2] then
        mismatches[#mismatches + 1] = string.format(
            '%s 0x%08X expected 0x%08X read %s', target[3], target[1],
            target[2], show(word, '0x%08X'))
    end
end

for _, target in ipairs({CONTROL, OPEN, PLAY, FAST_FORWARD, HEADER, DISPATCH,
                         PROCESS}) do
    check(target)
end

add(CONTROL, function() controlHits = controlHits + 1 end)
add(OPEN, onOpen)
add(PLAY, function() onPlay('SD_PlaySequence') end)
add(FAST_FORWARD, function() onPlay('SD_PlaySequenceFastForward') end)
add(HEADER, onHeader)
add(DISPATCH, onDispatch)
add(PROCESS, function() processCalls = processCalls + 1 end)

listeners_sound_sequence_sync_track_watch = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        finish('reset; reload the script and start again')
    end),
}

print(string.format('%s: %d breakpoints installed, %d signature mismatches; '
                    .. 'stay on the main menu about two minutes, then start '
                    .. 'a Free Duel', SCRIPT_NAME,
                    #breakpoints_sound_sequence_sync_track_watch,
                    #mismatches))
