-- duel_field_watch.lua
--
-- WHAT THIS ANSWERS
--   Which of the watched functions run on the duel board. The two zone
--   helpers turn a zone index (ten per side) into a pixel offset, 14 or 16
--   pixels apart, which looks like the duel field grid or its mini-map.
--
--   Every route of this family watches the same functions, all of which are
--   still address-named because nothing static shows who uses them:
--
--     func_8002C400 / func_8002C484  zone index -> pixel offset (no caller)
--     Model_GetSlotScale             model-slot scale quad (no caller)
--     func_800135FC                  orbit viewpoint offset (no caller)
--     func_8002C570                  "is this card in the chest" (no caller)
--     func_8003FFD8                  BGM step command 0xA000 (no caller)
--     func_80059B90                  rotation helper (no caller)
--     func_8005C374                  stores three movie bytes (no caller)
--     func_80058DC0                  returns D_8009AFA6 (no caller)
--     func_8005FC1C / func_8005FE44  the two model handler families, never
--                                    called in two 3-D battles
--     D_8009AFA6                     written by whom?
--
--   Each first call per caller prints a line at once, in the Lua console, so
--   you can say what was on screen at that moment. Main_RunFrameServices
--   proves the breakpoints are firing.
--
-- WHAT YOU DO (about two minutes)
--   1. PCSX-Redux with the game, debugger enabled, interpreter CPU.
--   2. At the main menu, Debug -> Lua editor, paste this file.
--   3. Start any Free Duel. The trace starts its two-minute window when the
--      duel board appears.
--   4. In your first turn, before playing a card, move the field cursor over
--      every zone on your side and then on the opponent's, one at a time.
--      Then just wait for the trace to print.
--   The trace prints by itself. Copy the whole document into
--   tools/trace/result/duel_field_watch.txt.
--
-- WHAT TO WRITE IN THE CONTEXT
--   The opponent you picked, and whether anything on screen moved in step
--   with the cursor besides the cursor itself (a small map, a highlight, a
--   panel). One or two lines is enough.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_field_watch'

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
local REGISTRY = 0x800f5918
local REGISTRY_COUNT = 80
local STATE_AFA6 = 0x8009afa6
local FAMILY = { [0x8005fc1c] = true, [0x8005fe44] = true }

local WATCH = {
    {0x8002c400, 0x2882000a, 'func_8002C400 zone offset'},
    {0x8002c484, 0x3c026666, 'func_8002C484 zone offset'},
    {0x8005949c, 0x000410c0, 'Model_GetSlotScale'},
    {0x800135fc, 0x27bdffd8, 'func_800135FC orbit offset'},
    {0x8002c570, 0x3c02801d, 'func_8002C570 chest test'},
    {0x8003ffd8, 0x27bdffe8, 'func_8003FFD8 BGM step'},
    {0x80059b90, 0x27bdff68, 'func_80059B90 rotation'},
    {0x8005c374, 0xa3840598, 'func_8005C374 movie bytes'},
    {0x80058dc0, 0x9382009e, 'func_80058DC0 get D_8009AFA6'},
    {0x8005fc1c, 0x3c02800f, 'func_8005FC1C handler family'},
    {0x8005fe44, 0x3c02800f, 'func_8005FE44 handler family'},
}
local CONTROL = {0x8001306c, 0x27bdffe0, 'control Main_RunFrameServices'}
-- The route starts at this function's first call; nil starts at once.
local START = {0x8002cee8, 0x93830364, 'start Main_RunDuel'}
local WINDOW_FRAMES = 7200
local START_TIMEOUT_FRAMES = 36000
local ROWS_PER_CALLER = 3
local MAX_ROWS = 60

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
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

local frames = 0
local startFrame = START == nil and 0 or nil
local rows = {}
local counts = {}
local perCaller = {}
local controlHits = 0
local mismatches = {}
local callbackError = nil
local done = false

breakpoints_duel_field_watch = {}

local function seconds()
    if startFrame == nil then return 'before-start' end
    return string.format('t=%ds', math.floor((frames - startFrame) / 60))
end

local function mode()
    local m = u8(MAIN_MODE)
    return m and string.format('0x%02X', m % 32) or 'unreadable'
end

-- Logged rows are also printed at once, so the console shows them while
-- the screen they belong to is still visible.
local function row(key, text)
    counts[key] = (counts[key] or 0) + 1
    if (perCaller[text.key] or 0) >= ROWS_PER_CALLER or #rows >= MAX_ROWS then
        return
    end
    perCaller[text.key] = (perCaller[text.key] or 0) + 1
    local line = string.format('row=%02d %s main=%s %s', #rows + 1, seconds(),
                               mode(), text.body)
    rows[#rows + 1] = line
    pcall(print, SCRIPT_NAME .. ': ' .. line)
end

local function registryKey(value)
    for i = 0, REGISTRY_COUNT - 1 do
        local entry = REGISTRY + i * 8
        if u32(entry) == value then
            return string.format(' key=0x%08X', u32(entry + 4))
        end
    end
    return ' key=none'
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_duel_field_watch) do bp:disable() end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<the opponent, and whether anything besides the cursor moved')
    print(' with it -- a small map, a highlight, a panel>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d control_hits: %d rows: %d', frames,
                        controlHits, #rows))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    local silent = {}
    for _, w in ipairs(WATCH) do
        if (counts[w[1]] or 0) == 0 then
            silent[#silent + 1] = w[3]
        else
            print(string.format('called %s x%d', w[3], counts[w[1]]))
        end
    end
    print(string.format('written D_8009AFA6 x%d', counts[STATE_AFA6] or 0))
    print('not called: ' .. (#silent > 0 and table.concat(silent, ', ')
                             or 'none'))
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onWatch(w)
    if done then return end
    local n = PCSX.getRegisters().GPR.n
    local extra = FAMILY[w[1]] and registryKey(n.a0) or ''
    row(w[1], {
        key = string.format('%08X:%08X', w[1], n.ra),
        body = string.format('%s caller=0x%08X a0=%d a1=%d a2=%d%s', w[3],
                             n.ra, signed(n.a0), signed(n.a1), signed(n.a2),
                             extra),
    })
end

local function onWrite()
    if done then return end
    local pc = PCSX.getRegisters().pc
    row(STATE_AFA6, {
        key = string.format('AFA6:%08X', pc),
        body = string.format('write D_8009AFA6 pc=0x%08X before=%s', pc,
                             show(u8(STATE_AFA6))),
    })
end

local function onStart()
    if done or startFrame ~= nil then return end
    startFrame = frames
    print(string.format('%s: route started; stopping by itself in %d seconds',
                        SCRIPT_NAME, WINDOW_FRAMES / 60))
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
    elseif startFrame == nil and frames >= START_TIMEOUT_FRAMES then
        finish('the route never started within ten minutes')
    elseif startFrame ~= nil and frames - startFrame >= WINDOW_FRAMES then
        if controlHits == 0 then
            finish('window ended without a control hit; the setup is not '
                   .. 'working, check the setup line')
        else
            finish('window ended')
        end
    end
end

local function install(target, kind, handler)
    if kind == 'Exec' then
        local word = u32(target[1])
        if word ~= target[2] then
            mismatches[#mismatches + 1] = string.format(
                '0x%08X expected 0x%08X read %s', target[1], target[2],
                show(word, '0x%08X'))
        end
    end
    breakpoints_duel_field_watch[#breakpoints_duel_field_watch + 1] =
        traceAddBreakpoint(target[1], kind, kind == 'Exec' and 4 or 1,
            SCRIPT_NAME .. ': ' .. target[3], function()
                local ok, err = pcall(handler)
                if not ok then callbackError = tostring(err) end
            end)
end

for _, w in ipairs(WATCH) do
    local watched = w
    install(watched, 'Exec', function() onWatch(watched) end)
end
install({STATE_AFA6, 0, 'write D_8009AFA6'}, 'Write', onWrite)
install(CONTROL, 'Exec', function() controlHits = controlHits + 1 end)
if START ~= nil then install(START, 'Exec', onStart) end

listener_duel_field_watch = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches; %s',
                    SCRIPT_NAME, #breakpoints_duel_field_watch, #mismatches,
                    START == nil and 'watching now' or 'waiting for the route'))
