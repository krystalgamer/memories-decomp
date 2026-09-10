-- rng_boot_timing.lua
--
-- WHAT THIS ANSWERS
--   notes/rng.md and notes/research/the-game.md retain a community timing
--   claim that boot seeds the PRNG during the Konami logo and begins consuming
--   it during the intro. Static code proves an early srand(0x56), followed by
--   srand(0x55555555), and the rand recurrence, but not which screen is
--   visible at each call.
--
--   This trace records every rand and srand entry from the BIOS shell through
--   the first three seconds of main mode 8. Each row includes the VSync frame,
--   mode, call site, old seed, and the state/result implied by the confirmed
--   runtime implementation. It arms after the requested hard reset, before
--   either game seed call, so both the initial 0x56 seed and later
--   0x55555555 seed are in scope. Human context supplies the visible-screen
--   labels.
--
--   PCSX-Redux removes Lua breakpoints during a console reset. The script
--   therefore listens for ExecutionFlow::Reset and reinstalls both execution
--   breakpoints when ExecutionFlow::ShellReached fires before the game boots.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec.
--   3. Debug -> Lua editor, paste this file, and let it run. Confirm that it
--      prints "breakpoints installed".
--   4. Hard-reset the emulated console. The script should print both
--      "reset observed" and "breakpoints reinstalled at BIOS shell".
--      Do not skip the intro or press buttons.
--   5. Note what is visible at the initial 0x56 seed, the later 0x55555555
--      seed, first rand call, main mode 8 entry, and any further reseed.
--   6. After the trace prints, copy the whole document into
--      tools/trace/result/rng_boot_timing.txt and fill in the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm the interpreter CPU and hard reset, whether any input or intro
--   skip was used, what was visible at both seed calls and each later live
--   milestone, and when the title screen appeared.

local ffi = require('ffi')

local SCRIPT_NAME = 'rng_boot_timing'
local RAND = 0x8008e590
local SRAND = 0x8008e5c0
local SEED = 0x800fe6f8
local MAIN_MODE = 0x8009b26c
local MENU_MODE = 8
local BOOT_SEED = 0x55555555
local STARTUP_SEED = 0x00000056
local POST_MENU_FRAMES = 180
local NO_BOOT_SEED_WARNING_FRAMES = 600
local MAX_EVENTS = 768
local TIMEOUT_FRAMES = 36000

local mem = PCSX.getMemPtr()

local function phys(addr)
    return addr - 0x80000000
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

local lines = {}
local vsyncFrames = 0
local captureFrame = nil
local menuFrame = nil
local lastMode = nil
local eventCount = 0
local randCalls = 0
local srandCalls = 0
local sawStartupSeed = false
local sawBootSeed = false
local callbackError = nil
local eventLimitReached = false
local armed = false
local resetSeen = false
local noBootSeedWarningPrinted = false
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function resetCaptureState()
    lines = {}
    vsyncFrames = 0
    captureFrame = nil
    menuFrame = nil
    lastMode = nil
    eventCount = 0
    randCalls = 0
    srandCalls = 0
    sawStartupSeed = false
    sawBootSeed = false
    callbackError = nil
    eventLimitReached = false
    armed = false
    resetSeen = true
    noBootSeedWarningPrinted = false
    done = false
end

local function relativeFrame()
    if captureFrame == nil then
        return 0
    end
    return vsyncFrames - captureFrame
end

local function callSite()
    local regs = PCSX.getRegisters()
    local returnAddress = tonumber(regs.GPR.n.ra)
    return (returnAddress - 8) % 0x100000000
end

local function record(text)
    if eventCount >= MAX_EVENTS then
        eventLimitReached = true
        return
    end

    eventCount = eventCount + 1
    emit(string.format(
        'event=%03d frame=%05d mode=%02d %s',
        eventCount, relativeFrame(), mainMode(), text
    ))
end

local function armAtShell()
    lines = {}
    captureFrame = vsyncFrames
    menuFrame = nil
    lastMode = mainMode()
    eventCount = 0
    randCalls = 0
    srandCalls = 0
    sawStartupSeed = false
    sawBootSeed = false
    callbackError = nil
    eventLimitReached = false
    armed = true
    resetSeen = true
    noBootSeedWarningPrinted = false
    done = false

    emit(string.format(
        'milestone frame=%05d bios_shell mode=%02d seed=0x%08X',
        relativeFrame(), mainMode(), u32(SEED)
    ))
    print('rng_boot_timing: capture armed at BIOS shell')
end

local function onSrand()
    local regs = PCSX.getRegisters()
    local argument = tonumber(regs.GPR.n.a0)

    if not armed or done then
        return
    end

    local oldSeed = u32(SEED)
    srandCalls = srandCalls + 1
    record(string.format(
        'kind=srand callsite=0x%08X old=0x%08X requested=0x%08X',
        callSite(), oldSeed, argument
    ))

    if argument == STARTUP_SEED then
        if not sawBootSeed and not sawStartupSeed then
            print('rng_boot_timing: initial srand(0x56) observed; '
                .. 'note the visible screen')
            sawStartupSeed = true
        else
            print('rng_boot_timing: additional srand(0x56) observed; '
                .. 'note the visible screen')
        end
    elseif argument == BOOT_SEED and not sawBootSeed then
        sawBootSeed = true
        print('rng_boot_timing: srand(0x55555555) observed; '
            .. 'note the visible screen')
    end
end

local function onRand()
    if not armed or done then
        return
    end

    local oldSeed = u32(SEED)
    local newSeed = nextSeed(oldSeed)
    local result = math.floor(newSeed / 0x10000) % 0x8000

    randCalls = randCalls + 1
    record(string.format(
        'kind=rand callsite=0x%08X old=0x%08X next=0x%08X return=0x%04X',
        callSite(), oldSeed, newSeed, result
    ))

    if randCalls == 1 then
        print('rng_boot_timing: first rand call observed; note the visible screen')
    end
end

local function finish(reason)
    if done then
        return
    end
    done = true

    if breakpoint_rng_boot_rand ~= nil then
        breakpoint_rng_boot_rand:disable()
    end
    if breakpoint_rng_boot_srand ~= nil then
        breakpoint_rng_boot_srand:disable()
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm interpreter CPU and hard reset; state whether input or an')
    print(' intro skip was used; identify the visible screen at the initial')
    print(' 0x56 seed, the 0x55555555 seed, first rand call, mode 8 entry,')
    print(' any later reseed, and title appearance>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: events=%d rand_calls=%d srand_calls=%d '
            .. 'startup_seed_seen=%s boot_seed_seen=%s',
        eventCount,
        randCalls,
        srandCalls,
        tostring(sawStartupSeed),
        tostring(sawBootSeed)
    ))
    for _, line in ipairs(lines) do
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

    vsyncFrames = vsyncFrames + 1

    if callbackError ~= nil then
        finish('breakpoint callback error: ' .. callbackError)
        return
    end

    if not armed then
        if resetSeen
            and not noBootSeedWarningPrinted
            and vsyncFrames >= NO_BOOT_SEED_WARNING_FRAMES then
            noBootSeedWarningPrinted = true
            print('rng_boot_timing: BIOS shell capture not armed; confirm '
                .. 'the shell/reinstall message')
        end
        if vsyncFrames >= TIMEOUT_FRAMES then
            finish('timed out before BIOS shell capture; hard-reset and '
                   .. 'confirm breakpoint reinstallation')
        end
        return
    end

    if eventLimitReached then
        finish('maximum event count reached; partial boot trace follows')
        return
    end

    if not sawBootSeed
        and not noBootSeedWarningPrinted
        and relativeFrame() >= NO_BOOT_SEED_WARNING_FRAMES then
        noBootSeedWarningPrinted = true
        print('rng_boot_timing: no srand(0x55555555) hit after BIOS shell; '
            .. 'confirm interpreter CPU')
    end

    local mode = mainMode()
    if mode ~= lastMode then
        lastMode = mode
        emit(string.format(
            'mode_change frame=%05d mode=%02d seed=0x%08X',
            relativeFrame(), mode, u32(SEED)
        ))
    end

    if sawBootSeed and mode == MENU_MODE and menuFrame == nil then
        menuFrame = relativeFrame()
        emit(string.format(
            'milestone frame=%05d main_mode_8 seed=0x%08X',
            menuFrame, u32(SEED)
        ))
        print('rng_boot_timing: main mode 8 observed; note the visible screen')
    elseif menuFrame ~= nil
        and relativeFrame() - menuFrame >= POST_MENU_FRAMES then
        if sawStartupSeed then
            finish('captured startup and boot seeds through three seconds '
                   .. 'of main mode 8')
        else
            finish('captured boot seed but not initial 0x56 seed; '
                   .. 'partial trace follows')
        end
    elseif relativeFrame() >= TIMEOUT_FRAMES then
        if sawBootSeed then
            finish('timed out after boot seed; partial trace follows')
        else
            finish('timed out before srand(0x55555555); partial trace follows')
        end
    end
end

local function installBreakpoints(reason)
    if breakpoint_rng_boot_rand ~= nil then
        breakpoint_rng_boot_rand:remove()
    end
    if breakpoint_rng_boot_srand ~= nil then
        breakpoint_rng_boot_srand:remove()
    end

    breakpoint_rng_boot_rand = PCSX.addBreakpoint(
        RAND,
        'Exec',
        4,
        'Trace rand during boot',
        function()
            local ok, err = pcall(onRand)
            if not ok then
                callbackError = tostring(err)
            end
        end
    )

    breakpoint_rng_boot_srand = PCSX.addBreakpoint(
        SRAND,
        'Exec',
        4,
        'Trace srand during boot',
        function()
            local ok, err = pcall(onSrand)
            if not ok then
                callbackError = tostring(err)
            end
        end
    )

    print('rng_boot_timing: breakpoints ' .. reason)
end

listener_rng_boot_reset = PCSX.Events.createEventListener(
    'ExecutionFlow::Reset',
    function()
        local ok, err = pcall(function()
            breakpoint_rng_boot_rand = nil
            breakpoint_rng_boot_srand = nil
            resetCaptureState()
            print('rng_boot_timing: reset observed; waiting for BIOS shell')
        end)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_rng_boot_shell = PCSX.Events.createEventListener(
    'ExecutionFlow::ShellReached',
    function()
        local ok, err = pcall(function()
            installBreakpoints('reinstalled at BIOS shell')
            armAtShell()
        end)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_rng_boot_timing = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

installBreakpoints('installed')
print('rng_boot_timing: ready; hard-reset without skipping the intro')
