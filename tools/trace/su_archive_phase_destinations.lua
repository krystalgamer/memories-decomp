-- su_archive_phase_destinations.lua
--
-- WHAT THIS ANSWERS
--   func_8005B64C retains the inherited name select_reverb_preset, but its
--   static body never touches the SPU. It is registered only as the callback
--   for the 115-sector SU.MRG request and uses selector values 0 through 4 to
--   configure transfer rectangles, destination buffers, sizes, and flags.
--
--   This trace captures the callback before and after each selector case
--   during an actual main-menu archive reload. The selector sequence and
--   resulting transfer records can decide whether a specific
--   MainMenu_ConfigureArchivePhase name or a broader Cd_SelectTransferPreset
--   name is justified.
--
--   Returning from OPTION may not reload SU.MRG. PCSX-Redux also removes Lua
--   breakpoints during reset, so this script reinstalls both callbacks at the
--   BIOS shell before using the boot-time load as its fallback.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec.
--   3. From the initial main menu, enter OPTION.
--   4. Open Debug -> Lua editor, paste this file, and let it auto-run.
--   5. Back out of OPTION once to return to the initial main menu. Do not
--      enter another screen until the trace finishes.
--   6. Wait for all five selectors or the bounded timeout.
--   7. Copy the whole document into
--      tools/trace/result/su_archive_phase_destinations.txt and fill in the
--      context.
--
--   If returning from OPTION produces no callback hits, leave the script
--   running and reset once. It should print "reset observed" followed by
--   "breakpoints reinstalled at BIOS shell" before the boot-time SU load.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm interpreter CPU, whether the script began in OPTION or before a
--   reset, the exact transition that caused the hits, whether the initial
--   main menu appeared normally, and whether any other screen was entered.

local ffi = require('ffi')

local SCRIPT_NAME = 'su_archive_phase_destinations'
local CONFIGURE_PHASE = 0x8005b64c
local CONFIGURE_PHASE_EPILOGUE = 0x8005b84c
local MAIN_MODE = 0x8009b26c
local STREAM_FLAGS = 0x8009b0f4
local PHASE_VALUE = 0x8001002c
local SELECTOR_COUNT = 5
local RECORD_SIZE = 0x47
local MAX_HITS = 12
local NO_HIT_WARNING_FRAMES = 600
local QUIET_FRAMES = 180
local TIMEOUT_FRAMES = 72000

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

local function u16(addr)
    return tonumber(ffi.cast('uint16_t*', mem + phys(addr))[0])
end

local function u32(addr)
    return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0])
end

local function s16(addr)
    local value = u16(addr)
    if value >= 0x8000 then
        return value - 0x10000
    end
    return value
end

local function isRam(addr, size)
    addr = normalize32(addr)
    return addr >= 0x80000000
        and addr < 0x80200000
        and size >= 0
        and addr + size <= 0x80200000
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function callSite(regs)
    return normalize32(tonumber(regs.GPR.n.ra) - 8)
end

local function recordText(addr)
    addr = normalize32(addr)
    if not isRam(addr, RECORD_SIZE) then
        return string.format('invalid_record(0x%08X)', addr)
    end

    return string.format(
        'field04=(%d,%d) buffers08=(0x%08X,0x%08X) '
        .. 'size1C=0x%08X field30=(%d,%d) status46=0x%02X',
        s16(addr + 0x04),
        s16(addr + 0x06),
        u32(addr + 0x08),
        u32(addr + 0x0C),
        u32(addr + 0x1C),
        s16(addr + 0x30),
        s16(addr + 0x32),
        u8(addr + 0x46)
    )
end

local lines = {}
local seen = {}
local frames = 0
local hits = 0
local completed = 0
local quietFrames = 0
local pending = nil
local callbackError = nil
local hitLimitReached = false
local resetSeen = false
local noHitWarningPrinted = false
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function resetCaptureState()
    lines = {}
    seen = {}
    frames = 0
    hits = 0
    completed = 0
    quietFrames = 0
    pending = nil
    callbackError = nil
    hitLimitReached = false
    resetSeen = true
    noHitWarningPrinted = false
    done = false
end

local function selectorSummary()
    local values = {}
    for selector = 0, SELECTOR_COUNT - 1 do
        values[#values + 1] = string.format(
            '%d=%s', selector, tostring(seen[selector] == true)
        )
    end
    return table.concat(values, ' ')
end

local function seenCount()
    local count = 0
    for selector = 0, SELECTOR_COUNT - 1 do
        if seen[selector] then
            count = count + 1
        end
    end
    return count
end

local function allSelectorsSeen()
    for selector = 0, SELECTOR_COUNT - 1 do
        if not seen[selector] then
            return false
        end
    end
    return true
end

local function finish(reason)
    if done then
        return
    end
    done = true

    if breakpoint_su_archive_phase_entry ~= nil then
        breakpoint_su_archive_phase_entry:disable()
    end
    if breakpoint_su_archive_phase_exit ~= nil then
        breakpoint_su_archive_phase_exit:disable()
    end

    if pending ~= nil then
        emit(string.format(
            'pending_without_exit hit=%d selector=%d record=0x%08X',
            pending.hit, pending.selector, pending.record
        ))
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm interpreter CPU; say whether the script began in OPTION or')
    print(' before a reset; name the transition that caused the hits; confirm')
    print(' whether the initial menu appeared and no other screen was entered>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: hits=%d completed=%d selectors=%s',
        hits, completed, selectorSummary()
    ))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onEntry()
    if done or hitLimitReached or callbackError ~= nil then
        return
    end
    if hits >= MAX_HITS then
        hitLimitReached = true
        return
    end

    local regs = PCSX.getRegisters()
    local record = normalize32(tonumber(regs.GPR.n.a0))
    local selector = normalize32(tonumber(regs.GPR.n.a1))

    if pending ~= nil then
        emit(string.format(
            'entry_before_previous_exit old_hit=%d old_selector=%d',
            pending.hit, pending.selector
        ))
    end

    hits = hits + 1
    quietFrames = 0
    pending = {
        hit = hits,
        frame = frames,
        record = record,
        selector = selector,
        callsite = callSite(regs),
        flags = u32(STREAM_FLAGS),
        phaseValue = u32(PHASE_VALUE),
        before = recordText(record),
    }
end

local function onExit()
    if done or callbackError ~= nil then
        return
    end
    if pending == nil then
        emit(string.format(
            'orphan_exit frame=%d mode=0x%02X', frames, mainMode()
        ))
        return
    end

    local sample = pending
    pending = nil
    completed = completed + 1
    quietFrames = 0

    if sample.selector < SELECTOR_COUNT then
        seen[sample.selector] = true
    end

    emit('')
    emit(string.format(
        '--- hit %d selector %d ---',
        sample.hit, sample.selector
    ))
    emit(string.format(
        '  entry_frame=%d exit_frame=%d mode=0x%02X '
        .. 'callsite=0x%08X record=0x%08X',
        sample.frame,
        frames,
        mainMode(),
        sample.callsite,
        sample.record
    ))
    emit(string.format(
        '  before flags=0x%08X phase_value=0x%08X %s',
        sample.flags, sample.phaseValue, sample.before
    ))
    emit(string.format(
        '  after  flags=0x%08X phase_value=0x%08X %s',
        u32(STREAM_FLAGS), u32(PHASE_VALUE), recordText(sample.record)
    ))

    print(string.format(
        '%s: captured selector %d (unique %d/%d)',
        SCRIPT_NAME, sample.selector, seenCount(), SELECTOR_COUNT
    ))

    if hits >= MAX_HITS and not allSelectorsSeen() then
        hitLimitReached = true
    end
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
    if hitLimitReached then
        finish('maximum hit count reached; partial trace follows')
        return
    end

    if not noHitWarningPrinted
        and completed == 0
        and frames >= NO_HIT_WARNING_FRAMES then
        noHitWarningPrinted = true
        if resetSeen then
            print(SCRIPT_NAME
                .. ': no callback hits after reset; confirm the BIOS-shell '
                .. 'reinstall message and interpreter CPU')
        else
            print(SCRIPT_NAME
                .. ': OPTION return did not reload SU; leave the script '
                .. 'running and reset once')
        end
    end

    if completed > 0 then
        quietFrames = quietFrames + 1
    end

    if allSelectorsSeen() and quietFrames >= QUIET_FRAMES then
        finish('captured selectors 0 through 4 followed by quiet time')
    elseif frames >= TIMEOUT_FRAMES then
        if completed > 0 then
            finish('timed out after a partial SU callback trace')
        else
            finish('timed out without a callback hit; reload SU with interpreter CPU')
        end
    end
end

local function installBreakpoints(reason)
    if breakpoint_su_archive_phase_entry ~= nil then
        breakpoint_su_archive_phase_entry:remove()
    end
    if breakpoint_su_archive_phase_exit ~= nil then
        breakpoint_su_archive_phase_exit:remove()
    end

    breakpoint_su_archive_phase_entry = PCSX.addBreakpoint(
        CONFIGURE_PHASE,
        'Exec',
        4,
        'Trace SU archive phase entry',
        function()
            local ok, err = pcall(onEntry)
            if not ok then
                callbackError = tostring(err)
            end
        end
    )

    breakpoint_su_archive_phase_exit = PCSX.addBreakpoint(
        CONFIGURE_PHASE_EPILOGUE,
        'Exec',
        4,
        'Trace SU archive phase exit',
        function()
            local ok, err = pcall(onExit)
            if not ok then
                callbackError = tostring(err)
            end
        end
    )

    print(SCRIPT_NAME .. ': breakpoints ' .. reason)
end

listener_su_archive_phase_reset = PCSX.Events.createEventListener(
    'ExecutionFlow::Reset',
    function()
        local ok, err = pcall(function()
            breakpoint_su_archive_phase_entry = nil
            breakpoint_su_archive_phase_exit = nil
            resetCaptureState()
            print(SCRIPT_NAME .. ': reset observed; waiting for BIOS shell')
        end)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_su_archive_phase_shell = PCSX.Events.createEventListener(
    'ExecutionFlow::ShellReached',
    function()
        local ok, err = pcall(function()
            installBreakpoints('reinstalled at BIOS shell')
        end)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_su_archive_phase_destinations = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

installBreakpoints('installed')
print('su_archive_phase_destinations: armed; back out of OPTION once')
