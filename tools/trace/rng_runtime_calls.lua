-- rng_runtime_calls.lua
--
-- WHAT THIS ANSWERS
--   notes/rng.md retains a community claim that the PRNG usually advances
--   once per frame, with menu and duel exceptions. Static code proves the
--   recurrence and many callers, while rng_boot_timing.lua covers only boot
--   through the first three seconds of main mode 8.
--
--   This trace captures one bounded post-boot window from any screen. It
--   records every captured rand and srand entry with the VSync frame, main
--   mode, call site, old seed, next/requested seed, and rand result. Caller
--   and mode summaries make repeated per-frame calls visible without assigning
--   a screen or gameplay meaning that only human context can establish.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the retail game and enable the debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec.
--   3. Reach the screen or duel state whose RNG cadence you want to measure.
--      Do not reset during the capture.
--   4. Debug -> Lua editor, paste this file, and let it auto-run. Confirm that
--      it prints "breakpoints installed".
--   5. Stay idle for five seconds, perform one clearly identified input or
--      action, then stay idle again. The script prints five-second checkpoints;
--      note the visible screen and action at each one.
--   6. The trace stops after thirty seconds or 2,048 RNG calls. Copy the whole
--      document into tools/trace/result/rng_runtime_calls.txt and fill in the
--      context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm the retail executable and interpreter CPU. State the starting
--   screen/state, whether it was idle, every input or action and its nearest
--   checkpoint/frame, visible transitions, and any unexpected interruption.

local ffi = require('ffi')

local SCRIPT_NAME = 'rng_runtime_calls'
local RAND = 0x8008e590
local SRAND = 0x8008e5c0
local SEED = 0x800fe6f8
local MAIN_MODE = 0x8009b26c
local CAPTURE_FRAMES = 1800
local CHECKPOINT_FRAMES = 300
local NO_HIT_WARNING_FRAMES = 600
local MAX_EVENTS = 2048

local mem = PCSX.getMemPtr()

local function phys(addr)
    return addr - 0x80000000
end

local function normalize32(value)
    return value % 0x100000000
end

local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end

local function u32(addr)
    return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0])
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

-- Keep each multiply below 2^53 so Lua numbers preserve the low 32 bits.
local function nextSeed(seed)
    local seedLo = seed % 0x10000
    local seedHi = math.floor(seed / 0x10000)
    local productLo = seedLo * 0x4e6d
    local crossLo = (
        seedLo * 0x41c6 + seedHi * 0x4e6d
    ) % 0x10000

    return (
        productLo + crossLo * 0x10000 + 0x3039
    ) % 0x100000000
end

local events = {}
local markers = {}
local modeStats = {}
local modeOrder = {}
local callerStats = {}
local callerOrder = {}
local frames = 0
local eventCount = 0
local randCalls = 0
local srandCalls = 0
local callbackError = nil
local eventLimitReached = false
local noHitWarningPrinted = false
local done = false
local startSeed = u32(SEED)
local lastMode = mainMode()

local function emitMarker(text)
    markers[#markers + 1] = text
end

local function callSite(regs)
    return normalize32(tonumber(regs.GPR.n.ra) - 8)
end

local function rememberMode(mode, kind)
    local stat = modeStats[mode]
    if stat == nil then
        stat = {
            rand_calls = 0,
            srand_calls = 0,
            first_frame = frames,
            last_frame = frames,
        }
        modeStats[mode] = stat
        modeOrder[#modeOrder + 1] = mode
    end

    stat.last_frame = frames
    if kind == 'rand' then
        stat.rand_calls = stat.rand_calls + 1
    else
        stat.srand_calls = stat.srand_calls + 1
    end
end

local function rememberCaller(kind, mode, site)
    local key = string.format('%s:%02d:%08X', kind, mode, site)
    local stat = callerStats[key]
    if stat == nil then
        stat = {
            kind = kind,
            mode = mode,
            site = site,
            count = 0,
            first_frame = frames,
            last_frame = frames,
        }
        callerStats[key] = stat
        callerOrder[#callerOrder + 1] = key
    end

    stat.count = stat.count + 1
    stat.last_frame = frames
end

local function recordEvent(text)
    if eventCount >= MAX_EVENTS then
        eventLimitReached = true
        return false
    end

    eventCount = eventCount + 1
    events[#events + 1] = string.format(
        'event=%04d frame=%04d %s',
        eventCount,
        frames,
        text
    )
    if eventCount >= MAX_EVENTS then
        eventLimitReached = true
    end
    return true
end

local function onRand()
    if done or eventLimitReached or callbackError ~= nil then
        return
    end

    local regs = PCSX.getRegisters()
    local oldSeed = u32(SEED)
    local newSeed = nextSeed(oldSeed)
    local result = math.floor(newSeed / 0x10000) % 0x8000
    local mode = mainMode()
    local site = callSite(regs)

    randCalls = randCalls + 1
    rememberMode(mode, 'rand')
    rememberCaller('rand', mode, site)
    recordEvent(string.format(
        'mode=%02d kind=rand callsite=0x%08X '
            .. 'old=0x%08X next=0x%08X return=0x%04X',
        mode,
        site,
        oldSeed,
        newSeed,
        result
    ))
end

local function onSrand()
    if done or eventLimitReached or callbackError ~= nil then
        return
    end

    local regs = PCSX.getRegisters()
    local oldSeed = u32(SEED)
    local requested = normalize32(tonumber(regs.GPR.n.a0))
    local mode = mainMode()
    local site = callSite(regs)

    srandCalls = srandCalls + 1
    rememberMode(mode, 'srand')
    rememberCaller('srand', mode, site)
    recordEvent(string.format(
        'mode=%02d kind=srand callsite=0x%08X '
            .. 'old=0x%08X requested=0x%08X next=0x%08X',
        mode,
        site,
        oldSeed,
        requested,
        requested
    ))
end

local function disableBreakpoint(handle)
    if handle ~= nil then
        pcall(function()
            handle:disable()
        end)
    end
end

local function finish(reason)
    if done then
        return
    end
    done = true

    disableBreakpoint(breakpoint_rng_runtime_rand)
    disableBreakpoint(breakpoint_rng_runtime_srand)

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm the retail executable and interpreter CPU; identify the')
    print(' starting screen/state and whether it was idle; list every input or')
    print(' action with its nearest checkpoint/frame and describe visible')
    print(' transitions or interruptions>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: frames=%d events=%d rand_calls=%d srand_calls=%d '
            .. 'start_seed=0x%08X end_seed=0x%08X',
        frames,
        eventCount,
        randCalls,
        srandCalls,
        startSeed,
        u32(SEED)
    ))
    for _, line in ipairs(markers) do
        print(line)
    end
    for _, mode in ipairs(modeOrder) do
        local stat = modeStats[mode]
        print(string.format(
            'mode_summary mode=%02d rand_calls=%d srand_calls=%d '
                .. 'first_frame=%04d last_frame=%04d',
            mode,
            stat.rand_calls,
            stat.srand_calls,
            stat.first_frame,
            stat.last_frame
        ))
    end
    for _, key in ipairs(callerOrder) do
        local stat = callerStats[key]
        print(string.format(
            'caller_summary mode=%02d kind=%s callsite=0x%08X count=%d '
                .. 'first_frame=%04d last_frame=%04d',
            stat.mode,
            stat.kind,
            stat.site,
            stat.count,
            stat.first_frame,
            stat.last_frame
        ))
    end
    for _, line in ipairs(events) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function poll()
    if done then
        return
    end

    frames = frames + 1

    if callbackError ~= nil then
        finish('breakpoint callback error: ' .. callbackError)
        return
    end

    local mode = mainMode()
    if mode ~= lastMode then
        emitMarker(string.format(
            'mode_change frame=%04d old=%02d new=%02d seed=0x%08X',
            frames,
            lastMode,
            mode,
            u32(SEED)
        ))
        lastMode = mode
        print(string.format(
            '%s: frame %d mode changed to %d; note the visible screen',
            SCRIPT_NAME,
            frames,
            mode
        ))
    end

    if frames % CHECKPOINT_FRAMES == 0 then
        emitMarker(string.format(
            'checkpoint frame=%04d mode=%02d seed=0x%08X events=%d',
            frames,
            mode,
            u32(SEED),
            eventCount
        ))
        print(string.format(
            '%s: frame %d checkpoint; note the screen and recent action',
            SCRIPT_NAME,
            frames
        ))
    end

    if not noHitWarningPrinted
        and frames >= NO_HIT_WARNING_FRAMES
        and eventCount == 0 then
        noHitWarningPrinted = true
        print(SCRIPT_NAME
            .. ': no RNG calls observed; confirm interpreter CPU and '
            .. 'continue the selected idle/action window')
    end

    if eventLimitReached then
        finish('maximum event count reached; bounded trace follows')
        return
    end

    if frames >= CAPTURE_FRAMES then
        finish('captured thirty seconds of runtime RNG activity')
    end
end

emitMarker(string.format(
    'capture_start frame=0000 mode=%02d seed=0x%08X',
    lastMode,
    startSeed
))

breakpoint_rng_runtime_rand = PCSX.addBreakpoint(
    RAND,
    'Exec',
    4,
    'Trace rand during runtime',
    function()
        local ok, err = pcall(onRand)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

breakpoint_rng_runtime_srand = PCSX.addBreakpoint(
    SRAND,
    'Exec',
    4,
    'Trace srand during runtime',
    function()
        local ok, err = pcall(onSrand)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_rng_runtime_reset = PCSX.Events.createEventListener(
    'ExecutionFlow::Reset',
    function()
        local ok, err = pcall(function()
            finish('reset observed; partial trace follows')
        end)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_rng_runtime_calls = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print(SCRIPT_NAME .. ': breakpoints installed')
print(SCRIPT_NAME
    .. ': armed for thirty seconds; idle, perform one action, then idle again')
