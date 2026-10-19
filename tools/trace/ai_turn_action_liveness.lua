-- ai_turn_action_liveness.lua
--
-- WHAT THIS ANSWERS
--   src/game/ai_turn_action.c holds six contiguous, matching functions that
--   choose a turn action with rand(): func_8002712C (spell search),
--   func_80027228 (fusion search), func_80027508 (random action pick),
--   func_8002778C and func_800278A0 (field-target scans) and func_800279BC
--   (the per-turn play decision that writes gAi_Selection bytes 0x09-0x0B).
--   notes/ai-scripts.md shows the retail opponent decides through the
--   AiScript_Run bytecode instead, and no static caller of func_80027508 or
--   func_800279BC exists: the executable and every extracted DATA file hold no
--   jal to either and no stored pointer to either.
--
--   So the unit is either dead code beside the script AI, or it is reached
--   through a computed pointer in some mode nobody has watched. Which one
--   decides what it is named: a legacy/unused AI, or the AI of a specific
--   mode. This trace breaks on all six entries and on two known-live
--   controls -- AiScript_Run and the board-snapshot exporter func_80027DF8 --
--   so a silent unit can be told apart from a breakpoint that never armed.
--   Every hit records the main mode, duel scene state, opponent id, active
--   side and caller; func_800279BC also records the selection it wrote.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game, enable the debugger and select the
--      interpreter CPU. Breakpoints do not fire on the dynarec.
--   2. Debug -> Lua editor, paste this file and confirm it reports eight
--      installed breakpoints and no signature mismatch.
--   3. Play, in order and as far as you can: a few opponent turns of a
--      campaign or Free Duel, a few turns of a two-player duel, and a
--      minute idling on the title screen in case it starts a demo duel.
--   4. The trace prints after 48 rows, or when you have been in at least
--      one duel and press Select twice in a row on pad 1 outside a duel.
--   5. Copy the whole document into
--      tools/trace/result/ai_turn_action_liveness.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   List each mode you played, in order, with the opponent names. For every
--   row naming one of the six functions, say what was on screen and what the
--   side that was acting visibly did next. If no such row appears, say how
--   many opponent turns and two-player turns you watched.

local ffi = require('ffi')

local SCRIPT_NAME = 'ai_turn_action_liveness'

-- Breakpoints fire only while the debugger setting is on and the interpreter
-- CPU is running. PCSX-Redux reads the debugger flag live, so a script can
-- switch it on itself; the dynarec only changes after a restart, so it can
-- only be reported. Runs before any breakpoint is installed.
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
    local dynarec = 'unknown'
    local okDynarec, dynarecOn = pcall(function() return emulator.Dynarec end)
    if okDynarec and dynarecOn == false then
        dynarec = 'off'
    elseif okDynarec and dynarecOn == true then
        dynarec = 'ON: breakpoints will not fire; select the interpreter CPU '
            .. 'and restart the emulator'
    end
    return 'debugger=' .. debugger .. ' dynarec=' .. dynarec
end)()
print(SCRIPT_NAME .. ': setup ' .. TRACE_SETUP)

local RAM_BASE = 0x80000000
local RAM_END = 0x80200000

local MAIN_MODE = 0x8009b26c
local SCENE_STATE = 0x8009b23a
local OPPONENT_ID = 0x8009b361
local ACTIVE_SIDE = 0x8009b1d5
local SELECTION = 0x800eae88
local PAD1_PRESSED = 0x8009b398
local PAD_SELECT = 0x0100
local DUEL_MODE = 3

-- Entry address, retail first word, label, and whether it is a control.
local TARGETS = {
    {0x8002712c, 0x27bdffc8, 'func_8002712C spell search', false},
    {0x80027228, 0x27bdffa8, 'func_80027228 fusion search', false},
    {0x80027508, 0x27bdffe0, 'func_80027508 random action pick', false},
    {0x8002778c, 0x27bdffd0, 'func_8002778C field-target scan', false},
    {0x800278a0, 0x27bdffd0, 'func_800278A0 face-up target scan', false},
    {0x800279bc, 0x27bdff90, 'func_800279BC play decision', false},
    {0x80070650, 0x27bdffd8, 'AiScript_Run (control)', true},
    {0x80027df8, 0x27bdffc8, 'func_80027DF8 board snapshot (control)', true},
}
local PLAY_DECISION = 0x800279bc

local MAX_ROWS = 48
local ROWS_PER_TARGET = 8
local CONTROL_ROWS = 2
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

local function s8(addr)
    local v = u8(addr)
    if v ~= nil and v >= 0x80 then v = v - 0x100 end
    return v
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

local frames = 0
local rows = {}
local counts = {}
local logged = {}
local pendingDecision = nil
local mismatches = {}
local callbackError = nil
local sawDuel = false
local selectPresses = 0
local lastPressed = 0
local done = false

breakpoints_ai_turn_action_liveness = {}

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_ai_turn_action_liveness) do
        bp:disable()
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<modes played in order, with opponent names; for each row naming')
    print(' one of the six functions, what was on screen and what the acting')
    print(' side did next; if none appeared, how many turns you watched>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP)
    print('status: ' .. reason)
    print(string.format('frames: %d rows: %d', frames, #rows))
    for _, m in ipairs(mismatches) do
        print('signature_mismatch: ' .. m)
    end
    if callbackError ~= nil then
        print('callback_error: ' .. callbackError)
    end
    for _, t in ipairs(TARGETS) do
        print(string.format('count 0x%08X %-40s %d', t[1], t[3],
                            counts[t[1]] or 0))
    end
    print('')
    for _, line in ipairs(rows) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function context()
    local mode = u8(MAIN_MODE)
    return string.format(
        'mode=%s scene=%s opponent=%s side=%s',
        mode and string.format('0x%02X', mode % 32) or 'unreadable',
        show(u16(SCENE_STATE), '0x%04X'),
        show(s8(OPPONENT_ID)),
        show(u8(ACTIVE_SIDE))
    )
end

local function onHit(target)
    if done then return end
    local addr = target[1]
    counts[addr] = (counts[addr] or 0) + 1
    local limit = target[4] and CONTROL_ROWS or ROWS_PER_TARGET
    -- Hits keep counting after the cap; only the rows stop, here rather
    -- than at the next vsync, because several hits can land in one frame.
    if (logged[addr] or 0) >= limit or #rows >= MAX_ROWS then return end
    logged[addr] = (logged[addr] or 0) + 1

    local ra = PCSX.getRegisters().GPR.n.ra
    rows[#rows + 1] = string.format(
        'row=%02d frame=%06d hit=%s caller=0x%08X %s',
        #rows + 1, frames, target[3], ra, context()
    )
    if addr == PLAY_DECISION then
        pendingDecision = {row = #rows, frame = frames}
    end
end

local function reportDecision()
    local s = SELECTION
    rows[pendingDecision.row] = rows[pendingDecision.row] .. string.format(
        ' -> selection result=%s field1=%s value=%s zero=%s random=%s '
            .. 'action=%s argument=%s flag=%s',
        show(s8(s + 0x00)), show(s8(s + 0x01)), show(s8(s + 0x06)),
        show(s8(s + 0x07)), show(s8(s + 0x08)), show(s8(s + 0x09)),
        show(s8(s + 0x0a)), show(s8(s + 0x0b))
    )
    pendingDecision = nil
end

local function poll()
    if done then return end
    frames = frames + 1

    if pendingDecision ~= nil and frames > pendingDecision.frame then
        reportDecision()
    end
    if callbackError ~= nil then
        finish('callback error')
        return
    end

    local mode = u8(MAIN_MODE)
    local inDuel = mode ~= nil and mode % 32 == DUEL_MODE
    if inDuel then
        sawDuel = true
        selectPresses = 0
    end

    local pressed = u16(PAD1_PRESSED) or 0
    local fresh = pressed ~= 0 and pressed ~= lastPressed
    lastPressed = pressed
    if fresh and not inDuel then
        if pressed == PAD_SELECT then
            selectPresses = selectPresses + 1
        else
            selectPresses = 0
        end
    end
    if sawDuel and selectPresses >= 2 then
        finish('stopped by Select twice outside a duel')
    elseif #rows >= MAX_ROWS then
        finish('captured ' .. MAX_ROWS .. ' rows')
    elseif frames >= NO_HIT_WARNING_FRAMES
        and (counts[0x80070650] or 0) == 0
        and (counts[0x80027df8] or 0) == 0
        and #rows == 0 then
        finish('no hit at all, not even the controls; select the '
               .. 'interpreter CPU and play an opponent turn')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

local installed = 0
for _, target in ipairs(TARGETS) do
    local word = u32(target[1])
    if word ~= target[2] then
        mismatches[#mismatches + 1] = string.format(
            '0x%08X expected 0x%08X read %s', target[1], target[2],
            show(word, '0x%08X'))
    end
    local t = target
    breakpoints_ai_turn_action_liveness[#breakpoints_ai_turn_action_liveness + 1] =
        PCSX.addBreakpoint(t[1], 'Exec', 4, SCRIPT_NAME .. ': ' .. t[3],
            function()
                local ok, err = pcall(onHit, t)
                if not ok then callbackError = tostring(err) end
            end)
    installed = installed + 1
end

listener_ai_turn_action_liveness = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches',
                    SCRIPT_NAME, installed, #mismatches))
