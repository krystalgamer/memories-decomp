-- main_menu_handle_blink.lua
--
-- WHAT THIS ANSWERS
--   The main_menu overlay (SLUS-014.11, loaded at 0x80180000) keeps five
--   stored DisplayObject handles whose on-screen graphic nobody has
--   identified, so they still carry address names:
--     D_80184558  0x80184558  MainMenu_InitFrontendMenu: sprite from
--                             gDisplay_abResourceBank1, resource 5/0
--                             variant 0, at (0,0), depth offset 0
--     D_8018455C  0x8018455C  same function: resource 5/0 variant 2 at
--                             (0,8), depth offset 1. Its sibling, variant 1,
--                             is the already named gMainMenu_pStartPrompt
--                             (0x80184560), which this script leaves alone.
--     D_801845A0  0x801845A0  MainMenu_StartValueSetup: default resource
--                             0/4 variant 0xB at (0,0), depth offset -2
--     D_801845A4  0x801845A4  same function: bank 1 resource 6/0 variant 0
--                             at (0,0xA), depth offset -1
--     D_801845DC  0x801845DC  MainMenu_InitTradeScreen: the same 0/4
--                             variant 0xB sprite as D_801845A0, depth offset
--                             -2 (paired with gTrade_pStatsModeDisplay at
--                             0x801845E0)
--   MainMenu_StartValueSetup is called only by Main_RunTwoPlayerDuelSetup,
--   so its two handles live on the 2P Duel life-point setup screen.
--   Static reading gives texture cells and depths, not what the player sees.
--   The neighbours D_80184564, D_801845A8 and D_801845AC are not referenced
--   by any instruction of the module, so they are not probed.
--
-- METHOD
--   Every frame the script polls the five handle globals while the main_menu
--   overlay signature matches. A handle is a candidate only when it is
--   non-null, 4-byte aligned, inside RAM (0x80000000-0x801FFFFF) and points
--   at an allocated entry of gDisplayObject_aPool (0x800EFE48, 96 entries of
--   0x70 bytes, pool_index at +0x0A agreeing). One handle at a time, in the
--   order above, it clears only DISPLAY_OBJECT_FLAG_RENDERABLE (0x40) of the
--   u16 flags at +0x08 for three seconds, re-clearing it every frame if the
--   game sets it again, then restores the original bit and waits two
--   seconds. Each handle blinks twice. The renderers draw an entry only when
--   (flags & 0xC0) == 0xC0 and still run its update callback first, so the
--   only effect is that the graphic is not drawn. Nothing else is written.
--   If a handle changes or goes null mid-blink the blink is abandoned and
--   the bit is restored only if the same object is still allocated.
--
-- HOW TO RUN (about three minutes; the window ends itself after four)
--   1. Enable the debugger, select the interpreter CPU (restart the emulator
--      if you switch), Debug -> Lua editor, paste this file.
--   2. Go to the main menu (title screen with the menu) and leave it idle
--      until both D_80184558 and D_8018455C have blinked twice (~20 s).
--   3. Choose 2P Duel. It loads saves from both memory cards; once the
--      life-point setup screen is up, wait until D_801845A0 and D_801845A4
--      have blinked twice (~20 s), then back out.
--   4. Choose Trade (also loads both cards) and wait until D_801845DC has
--      blinked twice (~10 s).
--   5. Copy the whole document into
--      tools/trace/result/main_menu_handle_blink.txt.
--
-- WHAT TO WRITE IN THE CONTEXT
--   One line per handle: what disappeared during its HIDE rows (or
--   "nothing visible", or "screen not reached").

local ffi = require('ffi')

local SCRIPT_NAME = 'main_menu_handle_blink'
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

local FPS = 60
local HIDE_FRAMES = 3 * FPS
local GAP_FRAMES = 2 * FPS
local BLINKS = 2
local WINDOW_FRAMES = 240 * FPS
local MAX_ROWS = 120
local RAM_BASE, RAM_END = 0x80000000, 0x80200000
local POOL, POOL_COUNT, RECORD = 0x800EFE48, 96, 0x70
local FLAG_RENDERABLE, FLAG_ALLOCATED = 0x40, 0x80
local CONTROL, CONTROL_WORD = 0x8001306C, 0x27BDFFE0

-- main_menu overlay words (retail module, sha1 5a6aa8ab...): function
-- entries and the `sw v0, %lo(handle)(s0)` that stores each handle.
local SIGNATURE = {
    {0x8018001C, 0x3C022E8B}, {0x80180088, 0xAE024558},
    {0x80180104, 0xAE02455C}, {0x80180FD8, 0x27BDFFC0},
    {0x80181018, 0xAE0245A0}, {0x80181084, 0xAE0245A4},
    {0x80181F68, 0x27BDFFC0}, {0x80181FA0, 0xAE0245DC},
}

local handles = {
    {name = 'D_80184558', address = 0x80184558, screen = 'main menu'},
    {name = 'D_8018455C', address = 0x8018455C, screen = 'main menu'},
    {name = 'D_801845A0', address = 0x801845A0, screen = '2P Duel setup'},
    {name = 'D_801845A4', address = 0x801845A4, screen = '2P Duel setup'},
    {name = 'D_801845DC', address = 0x801845DC, screen = 'Trade'},
}
for _, h in ipairs(handles) do
    h.blinks, h.aborts, h.refused, h.rewrites, h.last = 0, 0, 0, 0, 0
end

local mem = PCSX.getMemPtr()
local rows, dropped = {}, 0
local frames, event, controlHits, writes = 0, 0, 0, 0
local resident, mismatches = nil, 0
local active, gapUntil, allDoneAt = nil, 0, nil
local done = false
breakpoints_main_menu_handle_blink = {}

local function ramOk(address, size)
    return mem ~= nil and address ~= nil and address >= RAM_BASE
        and address + size <= RAM_END and address % size == 0
end
local function read32(address)
    if not ramOk(address, 4) then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (address - RAM_BASE))[0])
end
local function read16(address)
    if not ramOk(address, 2) then return nil end
    return tonumber(ffi.cast('uint16_t*', mem + (address - RAM_BASE))[0])
end
local function read8(address)
    if not ramOk(address, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (address - RAM_BASE))[0])
end
local function signed16(value)
    if value >= 0x8000 then return value - 0x10000 end
    return value
end

local function signature()
    for _, pair in ipairs(SIGNATURE) do
        if read32(pair[1]) ~= pair[2] then return false end
    end
    return true
end

-- Why a pointer is refused, or nil when it is a live pool entry.
local function refusal(pointer)
    if pointer == nil or pointer == 0 then return 'null' end
    if pointer % 4 ~= 0 then return 'unaligned' end
    if pointer < RAM_BASE or pointer + RECORD > RAM_END then
        return 'outside RAM'
    end
    local offset = pointer - POOL
    if offset < 0 or offset >= POOL_COUNT * RECORD or offset % RECORD ~= 0 then
        return 'not a DisplayObject pool entry'
    end
    local flags = read16(pointer + 0x08)
    if flags == nil or bit.band(flags, FLAG_ALLOCATED) == 0 then
        return 'pool slot not allocated'
    end
    if read8(pointer + 0x0A) ~= offset / RECORD then
        return 'pool_index disagrees with slot'
    end
    return nil
end

-- Fields that change only if the slot is released and reused.
local function identity(pointer)
    return string.format('%08X/%08X/%04X/%08X/%08X', read32(pointer + 0x04),
        read32(pointer + 0x24), read16(pointer + 0x1E),
        read32(pointer + 0x3C), read32(pointer + 0x5C))
end

local function writeFlags(pointer, value)
    if refusal(pointer) ~= nil then return false end
    ffi.cast('uint16_t*', mem + (pointer + 0x08 - RAM_BASE))[0] = value
    writes = writes + 1
    return true
end

local function stamp() return string.format('t=%.2fs', frames / FPS) end

local function live(text)
    event = event + 1
    local row = string.format('#%03d %s %s', event, stamp(), text)
    if #rows < MAX_ROWS then
        rows[#rows + 1] = row
        print(row)
    else
        dropped = dropped + 1
    end
end

local function describe(pointer)
    local bytes = {}
    for offset = 0x64, 0x6A do
        bytes[#bytes + 1] = string.format('%02X', read8(pointer + offset))
    end
    return string.format('obj=0x%08X slot=%d x=%d y=%d flags=0x%04X '
        .. 'attribute=0x%08X extent=0x%08X uv=0x%04X/0x%04X b64..6A=%s '
        .. 'ot=%d depth=%d update=0x%08X resource=0x%08X', pointer,
        (pointer - POOL) / RECORD, signed16(read16(pointer + 0x30)),
        signed16(read16(pointer + 0x32)), read16(pointer + 0x08),
        read32(pointer + 0x04), read32(pointer + 0x3C),
        read16(pointer + 0x5C), read16(pointer + 0x5E),
        table.concat(bytes, ' '), read8(pointer + 0x17),
        read16(pointer + 0x14), read32(pointer + 0x24),
        read32(pointer + 0x54))
end

-- Keep the bit clear while a blink runs; counts the game setting it again.
local function rehide()
    if active == nil or refusal(active.object) ~= nil then return end
    local flags = read16(active.object + 0x08)
    if bit.band(flags, FLAG_RENDERABLE) ~= 0 then
        active.handle.rewrites = active.handle.rewrites + 1
        active.rewrites = active.rewrites + 1
        writeFlags(active.object,
            bit.band(flags, bit.bnot(FLAG_RENDERABLE)))
        if active.rewrites == 1 then
            live(string.format('REWRITE %s: the game set the bit again; '
                .. 're-hidden', active.handle.name))
        end
    end
end

-- Put the original bit back, and only on the same live object.
local function restore(reason)
    local blink = active
    if blink == nil then return end
    active = nil
    local h = blink.handle
    local restored = false
    if refusal(blink.object) == nil
        and identity(blink.object) == blink.identity then
        local flags = read16(blink.object + 0x08)
        restored = writeFlags(blink.object, bit.bor(
            bit.band(flags, bit.bnot(FLAG_RENDERABLE)), blink.original))
    end
    if reason == nil then
        h.blinks = h.blinks + 1
        live(string.format('SHOW %s (obj=0x%08X) blink %d/%d done; '
            .. 'game re-set the bit %d time(s)', h.name, blink.object,
            h.blinks, BLINKS, blink.rewrites))
    else
        h.aborts = h.aborts + 1
        live(string.format('ABORT %s (obj=0x%08X): %s; %s', h.name,
            blink.object, reason, restored and 'bit restored'
                or 'bit not restored (object released or reused)'))
    end
    gapUntil = frames + GAP_FRAMES
end

local function start(h, pointer)
    local flags = read16(pointer + 0x08)
    active = {
        handle = h, object = pointer, started = frames, rewrites = 0,
        original = bit.band(flags, FLAG_RENDERABLE),
        identity = identity(pointer),
    }
    writeFlags(pointer, bit.band(flags, bit.bnot(FLAG_RENDERABLE)))
    live(string.format('HIDE %s (obj=0x%08X) for 3 s -- note what '
        .. 'disappears', h.name, pointer))
end

local function poll()
    for _, h in ipairs(handles) do
        local pointer = read32(h.address)
        if pointer ~= h.last then
            h.last = pointer
            local why = refusal(pointer)
            if pointer == 0 then
                live(h.name .. ' cleared to null')
            elseif why ~= nil then
                h.refused = h.refused + 1
                live(string.format('REFUSED %s = 0x%08X: %s', h.name,
                    pointer, why))
            else
                live(string.format('SEEN %s %s', h.name, describe(pointer)))
            end
        end
    end
end

local finish

local function tick()
    frames = frames + 1
    local now = signature()
    if now ~= resident then
        resident = now
        if now then
            live('main_menu overlay signature matches')
        else
            mismatches = mismatches + 1
            live('signature mismatch: main_menu is not resident; no memory '
                .. 'is touched until it returns')
        end
    end
    if active ~= nil then
        local current = now and read32(active.handle.address) or nil
        if not now then
            restore('main_menu overlay left')
        elseif current ~= active.object then
            restore(string.format('handle changed to 0x%08X', current))
        elseif refusal(active.object) ~= nil then
            restore('object no longer live: ' .. refusal(active.object))
        else
            rehide()
            if frames - active.started >= HIDE_FRAMES then restore(nil) end
        end
    end
    if now then poll() end
    if now and active == nil and frames >= gapUntil then
        for _, h in ipairs(handles) do
            local pointer = h.last
            if h.blinks < BLINKS and refusal(pointer) == nil then
                local flags = read16(pointer + 0x08)
                if bit.band(flags, FLAG_RENDERABLE) ~= 0 then
                    start(h, pointer)
                    break
                end
            end
        end
    end
    if allDoneAt == nil then
        local all = true
        for _, h in ipairs(handles) do
            if h.blinks < BLINKS then all = false end
        end
        if all then allDoneAt = frames end
    end
    if allDoneAt ~= nil and frames - allDoneAt >= GAP_FRAMES then
        finish('every handle blinked twice')
    elseif frames >= WINDOW_FRAMES then
        finish('four-minute window ended')
    end
end

finish = function(reason)
    if done then return end
    if active ~= nil then pcall(restore, 'trace ended') end
    done = true
    for _, bp in ipairs(breakpoints_main_menu_handle_blink) do
        pcall(function() bp:disable() end)
    end
    print('\n==== USER CONTEXT ====\n')
    for _, h in ipairs(handles) do
        print(h.name .. ' (' .. h.screen .. '): what disappeared?')
    end
    print('\n==== TRACE RESULT =====\n')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d control_hits: %d flag_writes: %d '
        .. 'signature_mismatches: %d dropped_rows: %d', frames, controlHits,
        writes, mismatches, dropped))
    for _, h in ipairs(handles) do
        print(string.format('%s 0x%08X blinks=%d/%d aborts=%d refused=%d '
            .. 'game_rewrites=%d', h.name, h.address, h.blinks, BLINKS,
            h.aborts, h.refused, h.rewrites))
    end
    if controlHits == 0 then
        print('note: Main_RunFrameServices never fired; the blinks still '
            .. 'ran from GPU::Vsync, but check the debugger/interpreter setup')
    end
    print('')
    for _, row in ipairs(rows) do print(row) end
    print('\n--- end of trace, copy everything above into '
        .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

breakpoints_main_menu_handle_blink[1] = traceAddBreakpoint(CONTROL, 'Exec', 4,
    SCRIPT_NAME .. ': Main_RunFrameServices', function()
        if done then return end
        local ok, err = pcall(function()
            if read32(CONTROL) == CONTROL_WORD then
                controlHits = controlHits + 1
            end
            if resident then rehide() end
        end)
        if not ok then finish('callback error: ' .. tostring(err)) end
    end)

listeners_main_menu_handle_blink = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        if done then return end
        local ok, err = pcall(tick)
        if not ok then finish('frame error: ' .. tostring(err)) end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        if done then return end
        finish('reset; reload the script after returning to the main menu')
    end),
}

print(SCRIPT_NAME .. ': armed. Route: 1) main menu, stay idle until '
    .. 'D_80184558 and D_8018455C blinked twice; 2) 2P Duel, wait on the '
    .. 'life-point setup until D_801845A0/D_801845A4 blinked twice, back '
    .. 'out; 3) Trade, wait until D_801845DC blinked twice. Ends by itself '
    .. 'within four minutes.')
print(SCRIPT_NAME .. ': main_menu signature '
    .. (signature() and 'matches' or 'mismatch (not resident yet)'))
