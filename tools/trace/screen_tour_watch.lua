-- screen_tour_watch.lua
--
-- WHAT THIS ANSWERS
--   Two doubts that need the screens no trace has visited yet.
--
--   DisplayObject +0x2C/+0x2E, +0x60 and +0x6C. The pooled record's tail is
--     reused by many update callbacks: Library_DrawFadingCardOutline counts
--     +0x60 down by 0x10, Duel_UpdateDrawCardSlide sweeps it from -0x400 to
--     0 with a step kept at +0x2C, dialog transitions sweep it from +-0x400,
--     and +0x6C is cleared when some tweens end. Does each callback give
--     these fields one consistent meaning, or several?
--     Every frame the 96-entry pool at 0x800EFE48 is scanned. Objects are
--     grouped by their update callback (+0x24), render kind (+0x10) and
--     draw callback (+0x4C, when it holds a code address), and each group
--     keeps its range and per-frame steps for every field.
--   gGraphics_FrameStepControl bytes 1 and 2 (0x8009AFA5, D_8009AFA6), and
--     func_80058DC0, the uncalled getter of byte 2. Four routes never wrote
--     either byte or called the getter; this tour visits the screens they
--     skipped.
--
--   Each change of the main mode byte is a live #NN screen marker. A row
--   prints the first time a group changes one of the fields, and on every
--   byte write or getter call.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file, from the title screen.
--   3. Visit, in any order: Password, Trade, Library (scroll and open a
--      card), Campaign (load a save, walk the map, talk to someone), the
--      save/load screen, and a Free Duel (draw cards, fuse two monsters,
--      and finish the duel if you can, so the result screen shows).
--   4. It prints its document after eight minutes, or after 40 screens.
--   5. Copy the whole document into tools/trace/result/screen_tour_watch.txt
--      and fill in the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   One line per #NN: the screen you were on and what moved or faded there.

local ffi = require('ffi')

local SCRIPT_NAME = 'screen_tour_watch'
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
local CODE_BASE = 0x80010000
local CODE_END = 0x801f0000
local MAIN_MODE = 0x8009b26c
local FPS = 60
local POOL = 0x800efe48
local RECORD = 0x70
local CAPACITY = 96
local FLAG_ALLOCATED = 0x80
local STEP_CONTROL = 0x8009afa4

local CONTROL = {0x8001306c, 0x27bdffe0, 'control Main_RunFrameServices'}
local GETTER = {0x80058dc0, 0x9382009e, 'func_80058DC0'}

-- name, offset, reader, signed
local FIELDS = {
    {'f2C', 0x2c, 'u16'},
    {'f2E', 0x2e, 's16'},
    {'f60', 0x60, 's16'},
    {'f6C', 0x6c, 'u8'},
}
local MAX_STEPS = 6

local MAX_SCREENS = 40
local MAX_ROWS = 400
local MAX_TWEEN_ROWS = 160
local MAX_BYTE_ROWS = 40
local WINDOW_FRAMES = 480 * FPS
local NO_CONTROL_FRAMES = 120 * FPS

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr ~= nil and addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function u16(addr)
    if not inRam(addr, 2) or addr % 2 ~= 0 then return nil end
    return tonumber(ffi.cast('uint16_t*', mem + (addr - RAM_BASE))[0])
end
local function s16(addr)
    local v = u16(addr)
    if v ~= nil and v >= 0x8000 then v = v - 0x10000 end
    return v
end
local function u32(addr)
    if not inRam(addr, 4) or addr % 4 ~= 0 then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local READ = {u8 = u8, u16 = u16, s16 = s16}
local function show(v, fmt)
    if v == nil then return 'unreadable' end
    return string.format(fmt or '%d', v)
end

local frames = 0
local rows = {}
local tweenRows = 0
local byteRows = 0
local screenCount = 0
local screen = nil
local lastMode = nil
local controlHits = 0
local getterCalls = 0
local byteWrites = {[1] = 0, [2] = 0}
local otherStores = 0
local seen = {}
local seenOrder = {}
local groups = {}
local groupOrder = {}
local tracked = {}
local mismatches = {}
local callbackError = nil
local done = false

breakpoints_screen_tour_watch = {}

local function stamp()
    return string.format('t=%6.1fs', frames / FPS)
end

local function live(text)
    print(text)
    if #rows < MAX_ROWS then rows[#rows + 1] = text end
end

local function tag()
    return screen and string.format('#%02d', screen) or '--'
end

local function noteSeen(key)
    if seen[key] ~= nil then
        seen[key] = seen[key] + 1
        return false
    end
    seen[key] = 1
    seenOrder[#seenOrder + 1] = key
    return true
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_screen_tour_watch) do bp:disable() end
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<one line per #NN: the screen and what moved or faded there>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d screens: %d control_hits: %d '
                        .. 'getter_calls: %d byte1_writes: %d '
                        .. 'byte2_writes: %d other_step_control_stores: %d '
                        .. 'object_groups: %d', frames, screenCount,
                        controlHits, getterCalls, byteWrites[1],
                        byteWrites[2], otherStores, #groupOrder))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    print('')
    for _, key in ipairs(seenOrder) do
        print(string.format('%s count=%d', key, seen[key]))
    end
    for _, key in ipairs(groupOrder) do
        local g = groups[key]
        local parts = {}
        for _, f in ipairs(FIELDS) do
            local r = g.fields[f[1]]
            if r.min ~= nil then
                local steps = {}
                for _, s in ipairs(r.steps) do steps[#steps + 1] = tostring(s) end
                parts[#parts + 1] = string.format('%s=%d..%d steps=%s%s',
                    f[1], r.min, r.max,
                    #steps > 0 and table.concat(steps, ',') or 'none',
                    r.moreSteps and '+' or '')
            end
        end
        print(string.format('group %s objects=%d life=%s..%s screens=%s %s',
                            key, g.objects, show(g.minLife), show(g.maxLife),
                            table.concat(g.screens, ','),
                            table.concat(parts, ' ')))
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onGetter()
    getterCalls = getterCalls + 1
    local ra = tonumber(PCSX.getRegisters().GPR.n.ra)
    local key = string.format('getter func_80058DC0 ra=0x%08X screen=%s', ra,
                              tag())
    if noteSeen(key) and byteRows < MAX_BYTE_ROWS then
        byteRows = byteRows + 1
        live(string.format('%s %s getter func_80058DC0 ra=0x%08X '
                           .. 'returns=%s', tag(), stamp(), ra,
                           show(u8(STEP_CONTROL + 2))))
    end
end

-- A write breakpoint fires before the store, so the bytes it covers and
-- the value are decoded from the store instruction (sb/sh/sw only).
local STORE_WIDTH = {[0x28] = 1, [0x29] = 2, [0x2b] = 4}
local function onStepStore()
    local regs = PCSX.getRegisters()
    local pc = tonumber(regs.pc)
    local w = u32(pc)
    local width = w and STORE_WIDTH[math.floor(w / 0x4000000)]
    local first, value = nil, nil
    if width ~= nil then
        local imm = w % 0x10000
        if imm >= 0x8000 then imm = imm - 0x10000 end
        first = (tonumber(regs.GPR.r[math.floor(w / 0x200000) % 32]) + imm)
            % 0x100000000
        value = tonumber(regs.GPR.r[math.floor(w / 0x10000) % 32])
            % (2 ^ (8 * width))
    end
    local hit = false
    for byte = 1, 2 do
        local addr = STEP_CONTROL + byte
        local covered = first == nil
            or (addr >= first and addr < first + width)
        if covered then
            hit = true
            byteWrites[byte] = byteWrites[byte] + 1
            local byteValue = 'unreadable'
            if first ~= nil then
                byteValue = tostring(math.floor(value / 2 ^ (8 * (addr - first)))
                                     % 0x100)
            end
            local key = string.format('write byte%d pc=0x%08X value=%s '
                                      .. 'screen=%s', byte, pc, byteValue,
                                      tag())
            if noteSeen(key) and byteRows < MAX_BYTE_ROWS then
                byteRows = byteRows + 1
                live(string.format('%s %s write byte%d (0x%08X) pc=0x%08X '
                                   .. 'value=%s before=%s', tag(), stamp(),
                                   byte, addr, pc, byteValue,
                                   show(u8(addr))))
            end
        end
    end
    if not hit then otherStores = otherStores + 1 end
end

local function codeAddress(v)
    return v ~= nil and v >= CODE_BASE and v < CODE_END and v % 4 == 0
end

local function groupKey(base)
    local update = u32(base + 0x24)
    local draw = u32(base + 0x4c)
    return string.format('upd=0x%08X kind=%s draw=%s', update or 0,
                         show(u32(base + 0x10)),
                         codeAddress(draw) and string.format('0x%08X', draw)
                             or '-')
end

local function group(key)
    local g = groups[key]
    if g == nil then
        g = {objects = 0, screens = {}, screenSeen = {}, fields = {}}
        for _, f in ipairs(FIELDS) do g.fields[f[1]] = {steps = {}, stepSeen = {}} end
        groups[key] = g
        groupOrder[#groupOrder + 1] = key
    end
    local s = tag()
    if not g.screenSeen[s] then
        g.screenSeen[s] = true
        g.screens[#g.screens + 1] = s
    end
    return g
end

local function closeObject(t)
    local g = groups[t.key]
    local life = frames - t.born
    if g.minLife == nil or life < g.minLife then g.minLife = life end
    if g.maxLife == nil or life > g.maxLife then g.maxLife = life end
end

local function scanPool()
    for i = 0, CAPACITY - 1 do
        local base = POOL + i * RECORD
        local flags = u16(base + 0x08)
        local t = tracked[i]
        if flags == nil or flags % 0x100 < FLAG_ALLOCATED then
            if t ~= nil then
                closeObject(t)
                tracked[i] = nil
            end
        else
            local key = groupKey(base)
            if t ~= nil and t.key ~= key then
                closeObject(t)
                t = nil
            end
            local g = group(key)
            local values = {}
            for _, f in ipairs(FIELDS) do values[f[1]] = READ[f[3]](base + f[2]) end
            if t == nil then
                g.objects = g.objects + 1
                t = {key = key, born = frames, values = values}
                tracked[i] = t
            end
            for _, f in ipairs(FIELDS) do
                local name = f[1]
                local r = g.fields[name]
                local v, old = values[name], t.values[name]
                if v ~= nil then
                    if r.min == nil or v < r.min then r.min = v end
                    if r.max == nil or v > r.max then r.max = v end
                    if old ~= nil and v ~= old then
                        local step = v - old
                        if not r.stepSeen[step] then
                            if #r.steps < MAX_STEPS then
                                r.stepSeen[step] = true
                                r.steps[#r.steps + 1] = step
                            else
                                r.moreSteps = true
                            end
                        end
                        if not r.changed and tweenRows < MAX_TWEEN_ROWS then
                            tweenRows = tweenRows + 1
                            live(string.format('%s %s tween slot=%d %s %s '
                                               .. '%d->%d', tag(), stamp(), i,
                                               key, name, old, v))
                        end
                        r.changed = true
                    end
                end
            end
            t.values = values
        end
    end
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    local mode = u8(MAIN_MODE)
    if mode ~= lastMode then
        lastMode = mode
        if screenCount >= MAX_SCREENS then
            finish('visited ' .. MAX_SCREENS .. ' screens')
            return
        end
        screenCount = screenCount + 1
        screen = screenCount
        live(string.format('#%02d %s SCREEN main_mode=%s', screen, stamp(),
                           show(mode, '0x%02X')))
    end
    scanPool()
    if frames >= WINDOW_FRAMES then
        finish('eight-minute tour ended')
    elseif frames >= NO_CONTROL_FRAMES and controlHits == 0 then
        finish('two minutes without a control hit; the setup is not '
               .. 'working, check the setup line')
    end
end

local function guarded(handler)
    return function()
        if done then return end
        local ok, err = pcall(handler)
        if not ok and callbackError == nil then callbackError = tostring(err) end
    end
end

local function add(address, kind, width, label, handler)
    breakpoints_screen_tour_watch[#breakpoints_screen_tour_watch + 1] =
        traceAddBreakpoint(address, kind, width, SCRIPT_NAME .. ': ' .. label,
                           guarded(handler))
end

local function install(target, handler)
    local word = u32(target[1])
    if word ~= target[2] then
        mismatches[#mismatches + 1] = string.format(
            '0x%08X expected 0x%08X read %s', target[1], target[2],
            show(word, '0x%08X'))
    end
    add(target[1], 'Exec', 4, target[3], handler)
end

install(CONTROL, function() controlHits = controlHits + 1 end)
install(GETTER, onGetter)
add(STEP_CONTROL, 'Write', 4, 'gGraphics_FrameStepControl', onStepStore)

listener_screen_tour_watch = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches; '
                    .. 'start the tour', SCRIPT_NAME,
                    #breakpoints_screen_tour_watch, #mismatches))
