-- unreferenced_helper_callers.lua
--
-- WHAT THIS ANSWERS
--   Three matching helpers have no caller anywhere the static tools can see:
--   no C call, no jal and no stored pointer in the executable or any
--   extracted DATA file. They can only be reached through a computed
--   address, most likely from an overlay, and each is named by its user:
--
--   func_8002C400  zone index -> pixel offset: (v % 10) * 14 + 14, or + 174
--                  for v >= 10 (src/game/duel_grid_offsets.c).
--   func_8002C484  zone index -> pixel offset: (v / 10) * 178 +
--                  (v % 10) * 16 + 14. Which screen and axis are these --
--                  the duel field grid, the Library grid or Build Deck?
--   func_8005949C  returns &D_800F39F0 + index * MODEL_SLOT_SIZE, a four-word
--                  view inside a model slot (src/game/model_slot_properties.c).
--                  Who reads it?
--
--   This trace breaks on all three and logs, for the first calls from each
--   caller, the caller address, the argument, the value the helper returns
--   (computed from the same formula), the main mode and a signature of the
--   overlay resident at 0x80168000. Main_RunFrameServices is a control that
--   runs every frame, so silence here is real evidence only when the control
--   count is large.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file, and wait for "execution
--      breakpoint observed; setup confirmed".
--   3. Visit, for about a minute each and moving the cursor around: a duel
--      (open the hand and walk the field cursor over every zone), the
--      Library card grid, Build Deck, the Free Duel opponent grid, and one
--      monster's 3-D view in the Library.
--   4. The trace prints after 60 rows, or when you press Select twice in a
--      row on the title/main menu after visiting at least two screens.
--   5. Copy the whole document into
--      tools/trace/result/unreferenced_helper_callers.txt and fill in
--      context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   The screens you visited, in order. For every row, what was on screen and
--   what you were doing; for the grid helpers, which on-screen element sat at
--   the returned offset (a card slot, a list row, a cursor) and whether it
--   moved horizontally or vertically as you changed the zone.

local ffi = require('ffi')

local SCRIPT_NAME = 'unreferenced_helper_callers'

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

local MAIN_MODE = 0x8009b26c
local PAD1_PRESSED = 0x8009b398
local PAD_SELECT = 0x0100
local OVERLAY_BASE = 0x80168000
local ZONES = 10
local SLOT_BASE = 0x800f39f0
local SLOT_SIZE = 0xe20

local HELPERS = {
    {0x8002c400, 0x2882000a, 'zone-offset func_8002C400'},
    {0x8002c484, 0x3c026666, 'zone-offset func_8002C484'},
    {0x8005949c, 0x000410c0, 'slot-quad func_8005949C'},
}
local CONTROL = {0x8001306c, 0x27bdffe0, 'control Main_RunFrameServices'}

local ROWS_PER_CALLER = 4
local MAX_ROWS = 60
local NO_HIT_WARNING_FRAMES = 18000
local TIMEOUT_FRAMES = 216000

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function u16(addr)
    if not inRam(addr, 2) then return nil end
    return tonumber(ffi.cast('uint16_t*', mem + (addr - RAM_BASE))[0])
end
local function u32(addr)
    if not inRam(addr, 4) then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local function show(v, fmt)
    if v == nil then return 'unreadable' end
    return string.format(fmt or '%d', v)
end
local function signed(v)
    if v >= 0x80000000 then return v - 0x100000000 end
    return v
end

-- What each helper returns for this argument, from its matching C. MIPS
-- division truncates toward zero, as Lua's integer helpers below do.
local function quot(a, b)
    local q = math.floor(math.abs(a) / b)
    if a < 0 then q = -q end
    return q
end
local function result(addr, v)
    if addr == 0x8002c400 then
        local r = v - quot(v, ZONES) * ZONES
        return string.format('%d', r * 14 + (v >= ZONES and 174 or 14))
    elseif addr == 0x8002c484 then
        local q = quot(v, ZONES)
        return string.format('%d', q * 178 + (v - q * ZONES) * 16 + 14)
    end
    return string.format('0x%08X', (SLOT_BASE + v * SLOT_SIZE) % 0x100000000)
end

local frames = 0
local rows = {}
local counts = {}
local perCaller = {}
local modesSeen = {}
local modeCount = 0
local controlHits = 0
local selectPresses = 0
local lastPressed = 0
local mismatches = {}
local callbackError = nil
local done = false

breakpoints_unreferenced_helper_callers = {}

local function overlay()
    return string.format('%s %s', show(u32(OVERLAY_BASE), '%08X'),
                         show(u32(OVERLAY_BASE + 4), '%08X'))
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_unreferenced_helper_callers) do
        bp:disable()
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<the screens you visited, in order; for every row, what was on')
    print(' screen and what you were doing, and for the grid helpers which')
    print(' element sat at the returned offset and which way it moved>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    local modes = {}
    for m in pairs(modesSeen) do modes[#modes + 1] = string.format('0x%02X', m) end
    table.sort(modes)
    print(string.format('frames: %d rows: %d control_hits: %d modes_seen: %s',
                        frames, #rows, controlHits,
                        #modes > 0 and table.concat(modes, ',') or 'none'))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    for _, h in ipairs(HELPERS) do
        print(string.format('count %-28s %d', h[3], counts[h[1]] or 0))
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onHelper(helper)
    if done then return end
    counts[helper[1]] = (counts[helper[1]] or 0) + 1
    local n = PCSX.getRegisters().GPR.n
    local key = string.format('%08X:%08X', helper[1], n.ra)
    if (perCaller[key] or 0) >= ROWS_PER_CALLER or #rows >= MAX_ROWS then
        return
    end
    perCaller[key] = (perCaller[key] or 0) + 1
    local v = signed(n.a0)
    local mode = u8(MAIN_MODE)
    rows[#rows + 1] = string.format(
        'row=%02d frame=%06d %s caller=0x%08X arg=%d returns=%s main=%s '
            .. 'overlay=%s', #rows + 1, frames, helper[3], n.ra, v,
        result(helper[1], v),
        mode and string.format('0x%02X', mode % 32) or 'unreadable',
        overlay())
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    local mode = u8(MAIN_MODE)
    if mode ~= nil and not modesSeen[mode % 32] then
        modesSeen[mode % 32] = true
        modeCount = modeCount + 1
    end

    local pressed = u16(PAD1_PRESSED) or 0
    if pressed ~= 0 and pressed ~= lastPressed then
        selectPresses = pressed == PAD_SELECT and selectPresses + 1 or 0
    end
    lastPressed = pressed

    if #rows >= MAX_ROWS then
        finish('captured ' .. MAX_ROWS .. ' rows')
    elseif selectPresses >= 2 and modeCount >= 3 then
        finish('stopped by Select twice after visiting several screens')
    elseif controlHits == 0 and frames >= NO_HIT_WARNING_FRAMES then
        finish('five minutes without a control hit; the setup is not '
               .. 'working, check the setup line')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

local function install(target, handler)
    local word = u32(target[1])
    if word ~= target[2] then
        mismatches[#mismatches + 1] = string.format(
            '0x%08X expected 0x%08X read %s', target[1], target[2],
            show(word, '0x%08X'))
    end
    breakpoints_unreferenced_helper_callers[#breakpoints_unreferenced_helper_callers + 1] =
        traceAddBreakpoint(target[1], 'Exec', 4,
            SCRIPT_NAME .. ': ' .. target[3], function()
                local ok, err = pcall(handler)
                if not ok then callbackError = tostring(err) end
            end)
end

for _, helper in ipairs(HELPERS) do
    local h = helper
    install(h, function() onHelper(h) end)
end
install(CONTROL, function() controlHits = controlHits + 1 end)

listener_unreferenced_helper_callers = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches',
                    SCRIPT_NAME, #breakpoints_unreferenced_helper_callers,
                    #mismatches))
