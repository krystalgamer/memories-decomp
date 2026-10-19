-- display_effect_steps.lua
--
-- WHAT THIS ANSWERS
--   DisplayEffect_ProcessMenuRecords walks the three 0x4C-byte records at
--   D_800EB010 every frame and, for each live record (+0x30 >= 0) with a
--   nonzero step byte (+0x33), calls D_80090F68[step & 0x1F]. Every handler
--   in that thirteen-entry table is still address-named:
--
--     1  func_80039FF8          7  func_8003AAE4  fade-in/resource
--     2  func_8003A560  VRAM    8  func_8003DA40
--     3  func_8003A990  cosine  9  func_8003D74C
--     4  func_8003AD6C  rows   10  func_8003D614
--     5  func_8003B054  portr. 11  func_8003D518
--     6  func_8003AC48  fade-out/resource       12  func_8003DA40
--
--   The short labels are the inventory's static readings (page swap, cosine
--   position easing, four-row transition, portrait transition, fades); none
--   has been tied to what the player sees, and steps 1 and 8-12 have no
--   reading at all. Text_HandleDisplayEffectCommand (text control byte F6)
--   and Script_OpShowMenu are the writers, so the steps run during dialogue
--   and menus.
--
--   This trace breaks on the eleven distinct handler entries and logs one
--   row per episode -- a record running one step on consecutive frames --
--   with the record slot, the step byte, the neighbouring control fields
--   and how many frames it ran.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game, enable the debugger and select the
--      interpreter CPU. Breakpoints do not fire on the dynarec.
--   2. Debug -> Lua editor, paste this file and confirm it reports eleven
--      installed breakpoints and a passed table check.
--   3. Go through campaign dialogue with portraits (the opening with Simon
--      is enough), one choice prompt, a card shop menu, and the start and
--      end of a duel.
--   4. The trace prints after 64 rows, or three quiet minutes after the
--      first row.
--   5. Copy the whole document into
--      tools/trace/result/display_effect_steps.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For each row, say what was on screen and what moved or changed during
--   it: a portrait sliding in or out, a picture fading, a panel or window
--   opening, a background swap, or nothing you could see.

local ffi = require('ffi')

local SCRIPT_NAME = 'display_effect_steps'

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
local RECORDS = 0x800eb010
local RECORD_SIZE = 0x4c
local RECORD_COUNT = 3
local TABLE = 0x80090f68
local TABLE_WORDS = {
    0x80039ff8, 0x80039ff8, 0x8003a560, 0x8003a990, 0x8003ad6c, 0x8003b054,
    0x8003ac48, 0x8003aae4, 0x8003da40, 0x8003d74c, 0x8003d614, 0x8003d518,
    0x8003da40,
}

local MAX_ROWS = 64
-- A handler can skip a vsync when the game runs at half rate.
local EPISODE_GAP_FRAMES = 4
local QUIET_FRAMES = 10800
local NO_HIT_WARNING_FRAMES = 36000
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
local lastRowFrame = nil
local rows = {}
local episodes = {}
local stepCounts = {}
local tableProblems = {}
local callbackError = nil
local done = false

breakpoints_display_effect_steps = {}

local function snapshot(record)
    return string.format(
        'live=%s f31=%s f32=%s f34=%s f36=%s f3A=%s f3B=%s f3C=%s',
        show(s8(record + 0x30)), show(u8(record + 0x31), '0x%02X'),
        show(u8(record + 0x32), '0x%02X'), show(u16(record + 0x34), '0x%04X'),
        show(u16(record + 0x36), '0x%04X'), show(u8(record + 0x3a), '0x%02X'),
        show(u8(record + 0x3b), '0x%02X'), show(u8(record + 0x3c), '0x%02X'))
end

local function closeEpisode(key, ep, reason)
    if #rows < MAX_ROWS then
        rows[#rows + 1] = string.format(
            'row=%02d frame=%06d slot=%d step=%d handler=0x%08X frames=%d '
                .. 'mode=%s start: %s end: %s step_after=%s %s',
            #rows + 1, ep.frame, ep.slot, ep.step, ep.handler, ep.calls,
            ep.mode, ep.start, snapshot(ep.record),
            show(u8(ep.record + 0x33)), reason)
        lastRowFrame = frames
    end
    episodes[key] = nil
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_display_effect_steps) do bp:disable() end
    -- Foreign entries only throttle their single row; they hold no record.
    for key, ep in pairs(episodes) do
        if ep.foreign then
            episodes[key] = nil
        else
            local ok, err = pcall(closeEpisode, key, ep, 'open')
            if not ok and callbackError == nil then
                callbackError = 'closing episode: ' .. tostring(err)
            end
        end
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<for each row: what was on screen and what moved or changed --')
    print(' portrait sliding, picture fading, panel opening, background swap,')
    print(' or nothing visible>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP)
    print('status: ' .. reason)
    print(string.format('frames: %d rows: %d', frames, #rows))
    for _, p in ipairs(tableProblems) do print('table_mismatch: ' .. p) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    for step = 1, #TABLE_WORDS - 1 do
        if stepCounts[step] then
            print(string.format('count step=%d handler=0x%08X calls=%d', step,
                                TABLE_WORDS[step + 1], stepCounts[step]))
        end
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onHit(handler)
    if done then return end
    local record = PCSX.getRegisters().GPR.n.a0
    local offset = record - RECORDS
    if offset < 0 or offset % RECORD_SIZE ~= 0
        or offset >= RECORD_SIZE * RECORD_COUNT then
        -- These handlers are also reachable outside the table; say so
        -- rather than reading a record that may not be one.
        local key = string.format('foreign:%08X', handler)
        if episodes[key] == nil and #rows < MAX_ROWS then
            rows[#rows + 1] = string.format(
                'row=%02d frame=%06d FOREIGN handler=0x%08X argument=0x%08X '
                    .. 'caller=0x%08X', #rows + 1, frames, handler, record,
                PCSX.getRegisters().GPR.n.ra)
            lastRowFrame = frames
            episodes[key] = {foreign = true, lastFrame = frames}
        elseif episodes[key] ~= nil then
            episodes[key].lastFrame = frames
        end
        return
    end
    local slot = offset / RECORD_SIZE
    local step = (u8(record + 0x33) or 0) % 32
    stepCounts[step] = (stepCounts[step] or 0) + 1
    local key = string.format('%d:%d', slot, step)
    local ep = episodes[key]
    if ep == nil then
        local mode = u8(MAIN_MODE)
        ep = {
            slot = slot, step = step, handler = handler, record = record,
            frame = frames, calls = 0, start = snapshot(record),
            mode = mode and string.format('0x%02X', mode % 32) or 'unreadable',
        }
        episodes[key] = ep
    end
    ep.calls = ep.calls + 1
    ep.lastFrame = frames
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    for key, ep in pairs(episodes) do
        if frames - ep.lastFrame > EPISODE_GAP_FRAMES then
            if ep.foreign then episodes[key] = nil
            else closeEpisode(key, ep, 'ended') end
        end
    end
    if #rows >= MAX_ROWS then
        finish('captured ' .. MAX_ROWS .. ' rows')
    elseif lastRowFrame ~= nil and frames - lastRowFrame >= QUIET_FRAMES then
        finish('three quiet minutes without a new row')
    elseif lastRowFrame == nil and next(episodes) == nil
        and frames >= NO_HIT_WARNING_FRAMES then
        finish('no step ran in ten minutes; select the interpreter CPU and '
               .. 'open a conversation with a portrait')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

for i, word in ipairs(TABLE_WORDS) do
    local read = u32(TABLE + (i - 1) * 4)
    if read ~= word then
        tableProblems[#tableProblems + 1] = string.format(
            'step=%d expected 0x%08X read %s', i - 1, word, show(read, '0x%08X'))
    end
end

local seen = {}
for i = 2, #TABLE_WORDS do
    local handler = TABLE_WORDS[i]
    if not seen[handler] then
        seen[handler] = true
        breakpoints_display_effect_steps[#breakpoints_display_effect_steps + 1] =
            PCSX.addBreakpoint(handler, 'Exec', 4,
                string.format('%s: step handler 0x%08X', SCRIPT_NAME, handler),
                function()
                    local ok, err = pcall(onHit, handler)
                    if not ok then callbackError = tostring(err) end
                end)
    end
end

listener_display_effect_steps = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed; table check %s',
                    SCRIPT_NAME, #breakpoints_display_effect_steps,
                    #tableProblems == 0 and 'passed'
                    or ('FAILED (' .. #tableProblems .. ')')))
