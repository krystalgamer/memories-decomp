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
--
-- METHOD
--   Every frame the script polls the five handle globals while the main_menu
--   overlay signature matches. A handle is a candidate only when it is
--   non-null, 4-byte aligned, inside RAM (0x80000000-0x801FFFFF) and points
--   at an allocated entry of gDisplayObject_aPool (0x800EFE48, 96 entries of
--   0x70 bytes). One handle at a time, in the order above, it clears only
--   DISPLAY_OBJECT_FLAG_RENDERABLE (0x40) of the u16 flags at +0x08 for three
--   seconds, then sets that bit back and waits two seconds; each handle
--   blinks twice. The renderers run an entry's update callback and then draw
--   it only when (flags & 0xC0) == 0xC0, so the only effect is that the
--   graphic is not drawn.
--
-- LIFETIME WITNESS (why a write can never land on a different object)
--   No DisplayObject field is an allocation generation: DisplayObject_Release
--   zeroes flags and DisplayObject_AcquireSlot may hand the same address,
--   index and flags to a new owner between two polls. So the blinked
--   allocation is followed by execution, not by sampled values:
--     * Exec breakpoints on every resident slot lifecycle entry
--       (DisplayObject_AcquireSlot 0x800400AC, a0 = index; DisplayObject_
--       Release 0x8004020C, a0 = slot, also reached from ReleaseIfPresent
--       and FadeBrightnessAndRelease; DisplayObject_MoveToListHead
--       0x800402A0, which releases and restores the same allocation;
--       DisplayObject_ResetPool 0x80040390, the bulk free behind
--       DisplayObject_Reset). A hit on the blinked slot (any ResetPool hit,
--       any AcquireSlot of its index, a Release not made by a matching
--       MoveToListHead) ends the lifetime before the game writes.
--     * A Write breakpoint on the blinked object's flags for the whole blink.
--       Each store is decoded from the instruction at the reported PC (base
--       register + offset must equal the reported address); a store that
--       clears ALLOCATED (0x80), or one that cannot be decoded, ends the
--       lifetime. Stores that keep 0x80 are the same allocation; one that
--       sets 0x40 again is re-hidden on the next frame.
--   Writes start only after the setup is proven in this run: the
--   Main_RunFrameServices Exec breakpoint fired, every lifecycle breakpoint
--   was installed on verified entry words, and a Write probe on
--   gMain_nServiceResyncCountdown (0x8009AF08, stored every frame by
--   `sw v0,0(gp)` at 0x8001310C) fired and decoded. Until then, or if any
--   witness cannot be installed, the script is READ-ONLY: it logs the
--   handles and blinks nothing, and the result says why.
--   All writes happen at the entry of Main_RunFrameServices, before that
--   frame's render. Hide/re-hide also need the overlay signature and the
--   watched handle to still name the object. While the lifetime is intact
--   the end of a blink (or a lost handle/overlay, or the window ending) sets
--   0x40 back by read-modify-write, keeping any other bits the game changed
--   (SHOW for a full blink, STOP otherwise). Once the lifetime has changed
--   the blink is ABANDONED and nothing is written, so a replacement object
--   is never touched; a reset abandons the same way.
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
local FINISH_GRACE = 30
local MAX_ROWS = 120
local RAM_BASE, RAM_END = 0x80000000, 0x80200000
local POOL, POOL_COUNT, RECORD = 0x800EFE48, 96, 0x70
local FLAG_RENDERABLE, FLAG_ALLOCATED = 0x40, 0x80
local CONTROL = 0x8001306C
local PROBE, PROBE_STORE = 0x8009AF08, 0x8001310C
local ACQUIRE, RELEASE, MOVE, RESET_POOL =
    0x800400AC, 0x8004020C, 0x800402A0, 0x80040390
local RELEASE_END, MOVE_END = RELEASE + 0x94, MOVE + 0xCC

-- main_menu overlay words (retail module, sha1 5a6aa8ab...): function
-- entries and the `sw v0, %lo(handle)(s0)` that stores each handle.
local SIGNATURE = {
    {0x8018001C, 0x3C022E8B}, {0x80180088, 0xAE024558},
    {0x80180104, 0xAE02455C}, {0x80180FD8, 0x27BDFFC0},
    {0x80181018, 0xAE0245A0}, {0x80181084, 0xAE0245A4},
    {0x80181F68, 0x27BDFFC0}, {0x80181FA0, 0xAE0245DC},
}
-- Resident words the witnesses rely on (SLUS_014.11): the lifecycle
-- entries, MoveToListHead's and Reset's calls, the probe store.
local RESIDENT = {
    {CONTROL, 0x27BDFFE0}, {ACQUIRE, 0x00803821}, {RELEASE, 0x97820508},
    {MOVE, 0x27BDFFE0}, {RESET_POOL, 0x00001821},
    {PROBE_STORE, 0xAF820000},
}

local handles = {
    {name = 'D_80184558', address = 0x80184558, screen = 'main menu'},
    {name = 'D_8018455C', address = 0x8018455C, screen = 'main menu'},
    {name = 'D_801845A0', address = 0x801845A0, screen = '2P Duel setup'},
    {name = 'D_801845A4', address = 0x801845A4, screen = '2P Duel setup'},
    {name = 'D_801845DC', address = 0x801845DC, screen = 'Trade'},
}
for _, h in ipairs(handles) do
    h.blinks, h.stopped, h.abandoned, h.refused, h.rewrites, h.last =
        0, 0, 0, 0, 0, 0
end

local mem = PCSX.getMemPtr()
local rows, dropped = {}, 0
local frames, event, controlHits, writes = 0, 0, 0, 0
local resident, mismatches, abandonedTotal = nil, 0, 0
local active, gapUntil, allDoneAt = nil, 0, nil
local finishing, finishingAt = nil, nil
local done = false
local witness = {installed = false, probe = false, failure = nil}
local announcedArmed = false
breakpoints_main_menu_handle_blink = {}
probe_main_menu_handle_blink = nil
flagwatch_main_menu_handle_blink = nil

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

local function matches(words)
    for _, pair in ipairs(words) do
        if read32(pair[1]) ~= pair[2] then return false end
    end
    return true
end
local function signature() return matches(SIGNATURE) end

-- Why a pointer is refused, or nil when it is an allocated pool entry.
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
    return nil
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

-- Why writes are not allowed yet, or nil once every witness is proven.
local function readOnlyReason()
    if witness.failure ~= nil then return witness.failure end
    if not witness.installed then return 'lifecycle breakpoints not installed' end
    if traceExecutionStatus() ~= 'execution_breakpoint=observed' then
        return 'no execution breakpoint has fired yet'
    end
    if controlHits == 0 then return 'Main_RunFrameServices has not fired yet' end
    if not witness.probe then return 'write probe has not fired yet' end
    return nil
end

local function u32(value)
    value = tonumber(value) or 0
    if value < 0 then value = value + 0x100000000 end
    return value % 0x100000000
end

-- Decode the store behind a Write breakpoint hit. Returns the instruction
-- address, the store size, the effective address and the stored register
-- value, or nil when no store at pc or pc-4 explains the reported access.
local STORES = {[0x28] = 1, [0x29] = 2, [0x2B] = 4}
local function decodeStore(address, width)
    local registers = PCSX.getRegisters()
    local pc = u32(registers.pc)
    for _, at in ipairs({pc, pc - 4}) do
        local word = read32(at)
        if word ~= nil then
            local size = STORES[bit.rshift(word, 26)]
            if size ~= nil and (width == nil or width == size) then
                local base = bit.band(bit.rshift(word, 21), 31)
                local rt = bit.band(bit.rshift(word, 16), 31)
                local offset = signed16(bit.band(word, 0xFFFF))
                local ea = u32(u32(registers.GPR.r[base]) + offset)
                if bit.band(ea, 0x1FFFFFFF)
                    == bit.band(u32(address), 0x1FFFFFFF) then
                    return at, size, ea, u32(registers.GPR.r[rt])
                end
            end
        end
    end
    return nil
end

local function latch(reason)
    if active ~= nil and active.changed == nil then
        active.changed = reason
    end
end

local function removeFlagWatch()
    local bp = flagwatch_main_menu_handle_blink
    flagwatch_main_menu_handle_blink = nil
    if bp ~= nil then
        pcall(function() bp:disable() end)
        pcall(function() bp:remove() end)
    end
end

local function onFlagsWrite(address, width)
    local blink = active
    if blink == nil or blink.changed ~= nil then return end
    local at, size, ea, value = decodeStore(address, width)
    if at == nil then
        latch('a write to its flags could not be decoded')
        return
    end
    local inRelease = at >= RELEASE and at < RELEASE_END
    local inMove = at >= MOVE and at < MOVE_END
    if blink.moving and (inRelease or inMove) then
        if inMove then blink.moving = false end
        return
    end
    local flags = blink.object + 0x08
    if ea > flags + 1 or ea + size <= flags then return end
    local low = ea == flags and value or nil
    if size == 1 and ea == flags + 1 then return end
    if low == nil or bit.band(low, FLAG_ALLOCATED) == 0 then
        latch(string.format('store at 0x%08X cleared ALLOCATED', at))
        return
    end
    if bit.band(low, FLAG_RENDERABLE) ~= 0 then
        blink.rewrites = blink.rewrites + 1
        blink.handle.rewrites = blink.handle.rewrites + 1
        if blink.rewrites == 1 then
            live(string.format('REWRITE %s: store at 0x%08X set the bit '
                .. 'again; re-hidden next frame', blink.handle.name, at))
        end
    end
end

local function abandon(reason)
    local blink = active
    if blink == nil then return end
    active = nil
    removeFlagWatch()
    local h = blink.handle
    h.abandoned = h.abandoned + 1
    abandonedTotal = abandonedTotal + 1
    live(string.format('ABANDON %s (obj=0x%08X): %s; nothing written, the '
        .. 'bit was NOT restored', h.name, blink.object, reason))
    gapUntil = frames + GAP_FRAMES
end

-- Set the bit back on the same allocation, keeping other game changes.
local function restore(reason)
    local blink = active
    if blink == nil then return end
    if blink.changed ~= nil then
        abandon('lifetime changed: ' .. blink.changed)
        return
    end
    local flags = read16(blink.object + 0x08)
    if flags == nil or bit.band(flags, FLAG_ALLOCATED) == 0 then
        abandon('slot unallocated without an observed release')
        return
    end
    active = nil
    removeFlagWatch()
    writeFlags(blink.object, bit.bor(
        bit.band(flags, bit.bnot(FLAG_RENDERABLE)), blink.original))
    local h = blink.handle
    if reason == nil then
        h.blinks = h.blinks + 1
        live(string.format('SHOW %s (obj=0x%08X) blink %d/%d done; '
            .. 'game re-set the bit %d time(s)', h.name, blink.object,
            h.blinks, BLINKS, blink.rewrites))
    else
        h.stopped = h.stopped + 1
        live(string.format('STOP %s (obj=0x%08X): %s; bit restored',
            h.name, blink.object, reason))
    end
    gapUntil = frames + GAP_FRAMES
end

local function start(h, pointer)
    local index = (pointer - POOL) / RECORD
    local blink = {
        handle = h, object = pointer, index = index, started = frames,
        rewrites = 0, moving = false, changed = nil,
        original = bit.band(read16(pointer + 0x08), FLAG_RENDERABLE),
    }
    local ok, bp = pcall(traceAddBreakpoint, pointer + 0x08, 'Write', 2,
        SCRIPT_NAME .. ': flags of ' .. h.name, function(address, width)
            if done then return end
            local fine, err = pcall(onFlagsWrite, address, width)
            if not fine then latch('write witness error: ' .. tostring(err)) end
        end)
    if not ok or bp == nil then
        witness.failure = 'Write breakpoint unavailable: ' .. tostring(bp)
        live('READ-ONLY: ' .. witness.failure)
        return
    end
    flagwatch_main_menu_handle_blink = bp
    active = blink
    local flags = read16(pointer + 0x08)
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

-- The only place that writes guest memory: the entry of
-- Main_RunFrameServices, before the frame's render.
local function onFrameServices()
    if active ~= nil then
        local blink = active
        local now = signature()
        if blink.changed ~= nil then
            abandon('lifetime changed: ' .. blink.changed)
        elseif blink.moving then
            return
        elseif finishing ~= nil then
            restore(finishing)
        elseif not now then
            restore('main_menu overlay left')
        elseif read32(blink.handle.address) ~= blink.object then
            restore(string.format('handle changed to 0x%08X',
                read32(blink.handle.address)))
        elseif frames - blink.started >= HIDE_FRAMES then
            restore(nil)
        else
            local flags = read16(blink.object + 0x08)
            if bit.band(flags, FLAG_ALLOCATED) == 0 then
                abandon('slot unallocated without an observed release')
            elseif bit.band(flags, FLAG_RENDERABLE) ~= 0 then
                writeFlags(blink.object,
                    bit.band(flags, bit.bnot(FLAG_RENDERABLE)))
            end
        end
    end
    if finishing ~= nil then
        if active == nil then finish(finishing) end
        return
    end
    local why = readOnlyReason()
    if why == nil and not announcedArmed then
        announcedArmed = true
        live('witnesses confirmed; blinking enabled')
    end
    if active == nil and why == nil and frames >= gapUntil and signature() then
        for _, h in ipairs(handles) do
            local pointer = read32(h.address)
            if h.blinks < BLINKS and pointer == h.last
                and refusal(pointer) == nil
                and bit.band(read16(pointer + 0x08), FLAG_RENDERABLE) ~= 0 then
                start(h, pointer)
                break
            end
        end
    end
end

local function tick()
    frames = frames + 1
    local now = signature()
    if now ~= resident then
        resident = now
        if now then
            live('main_menu overlay signature matches')
        else
            mismatches = mismatches + 1
            live('signature mismatch: main_menu is not resident; no blink '
                .. 'starts until it returns')
        end
    end
    if now then poll() end
    if allDoneAt == nil then
        local all = true
        for _, h in ipairs(handles) do
            if h.blinks < BLINKS then all = false end
        end
        if all then allDoneAt = frames end
    end
    if finishing == nil then
        if allDoneAt ~= nil and frames - allDoneAt >= GAP_FRAMES then
            finishing, finishingAt = 'every handle blinked twice', frames
        elseif frames >= WINDOW_FRAMES then
            finishing, finishingAt = 'four-minute window ended', frames
        end
    end
    if finishing ~= nil then
        if active == nil then
            finish(finishing)
        elseif frames - finishingAt >= FINISH_GRACE then
            abandon('Main_RunFrameServices did not run to restore it')
            finish(finishing)
        end
    end
end

finish = function(reason)
    if done then return end
    if active ~= nil then
        abandon(active.changed ~= nil and 'lifetime changed: ' .. active.changed
            or 'trace ended outside Main_RunFrameServices')
    end
    done = true
    removeFlagWatch()
    for _, bp in ipairs(breakpoints_main_menu_handle_blink) do
        pcall(function() bp:disable() end)
    end
    if probe_main_menu_handle_blink ~= nil then
        pcall(function() probe_main_menu_handle_blink:disable() end)
    end
    print('\n==== USER CONTEXT ====\n')
    for _, h in ipairs(handles) do
        print(h.name .. ' (' .. h.screen .. '): what disappeared?')
    end
    print('\n==== TRACE RESULT =====\n')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    local why = readOnlyReason()
    if why ~= nil then
        print('mode: READ-ONLY (' .. why .. '); handles were logged, '
            .. 'nothing was blinked unless rows above say otherwise')
    else
        print('mode: blinking (lifecycle and write witnesses confirmed)')
    end
    print(string.format('frames: %d control_hits: %d flag_writes: %d '
        .. 'signature_mismatches: %d abandoned_blinks: %d dropped_rows: %d',
        frames, controlHits, writes, mismatches, abandonedTotal, dropped))
    for _, h in ipairs(handles) do
        print(string.format('%s 0x%08X blinks=%d/%d stopped=%d '
            .. 'abandoned=%d refused=%d game_rewrites=%d', h.name, h.address,
            h.blinks, BLINKS, h.stopped, h.abandoned, h.refused, h.rewrites))
    end
    print('')
    for _, row in ipairs(rows) do print(row) end
    print('\n--- end of trace, copy everything above into '
        .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function guarded(label, body)
    return function(...)
        if done then return end
        local ok, err = pcall(body, ...)
        if not ok then
            latch(label .. ' error: ' .. tostring(err))
            finish(label .. ' error: ' .. tostring(err))
        end
    end
end

local function argument(name)
    return u32(PCSX.getRegisters().GPR.n[name])
end

local function install(address, label, body)
    local ok, bp = pcall(traceAddBreakpoint, address, 'Exec', 4,
        SCRIPT_NAME .. ': ' .. label, guarded(label, body))
    if not ok or bp == nil then return false end
    breakpoints_main_menu_handle_blink[
        #breakpoints_main_menu_handle_blink + 1] = bp
    return true
end

local control = install(CONTROL, 'Main_RunFrameServices', function()
    if read32(CONTROL) == 0x27BDFFE0 then controlHits = controlHits + 1 end
    onFrameServices()
end)

if not matches(RESIDENT) then
    witness.failure = 'resident lifecycle code words do not match SLUS_014.11'
elseif not control then
    witness.failure = 'Main_RunFrameServices breakpoint not installed'
else
    local all = install(ACQUIRE, 'DisplayObject_AcquireSlot', function()
        if active ~= nil and argument('a0') == active.index then
            latch('DisplayObject_AcquireSlot(index ' .. active.index .. ')')
        end
    end)
    all = install(RELEASE, 'DisplayObject_Release', function()
        if active ~= nil and argument('a0') == active.object then
            local ra = argument('ra')
            if not (active.moving and ra >= MOVE and ra < MOVE_END) then
                latch('DisplayObject_Release of the slot')
            end
        end
    end) and all
    all = install(MOVE, 'DisplayObject_MoveToListHead', function()
        if active ~= nil and argument('a0') == active.object then
            if active.moving then latch('nested list move') end
            active.moving = true
        end
    end) and all
    all = install(RESET_POOL, 'DisplayObject_ResetPool', function()
        latch('DisplayObject_ResetPool freed every slot')
    end) and all
    if all then
        local ok, bp = pcall(traceAddBreakpoint, PROBE, 'Write', 4,
            SCRIPT_NAME .. ': write probe', guarded('probe', function(a, w)
                local at = decodeStore(a, w)
                if at == PROBE_STORE and not witness.probe then
                    witness.probe = true
                    pcall(function() probe_main_menu_handle_blink:disable() end)
                end
            end))
        if ok and bp ~= nil then
            probe_main_menu_handle_blink = bp
            witness.installed = true
        else
            witness.failure = 'Write breakpoints unavailable: ' .. tostring(bp)
        end
    else
        witness.failure = 'a lifecycle breakpoint could not be installed'
    end
end

listeners_main_menu_handle_blink = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        if done then return end
        local ok, err = pcall(tick)
        if not ok then
            latch('frame error')
            finish('frame error: ' .. tostring(err))
        end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        if done then return end
        latch('console reset')
        finish('reset; reload the script after returning to the main menu')
    end),
}

print(SCRIPT_NAME .. ': armed. Route: 1) main menu, stay idle until '
    .. 'D_80184558 and D_8018455C blinked twice; 2) 2P Duel, wait on the '
    .. 'life-point setup until D_801845A0/D_801845A4 blinked twice, back '
    .. 'out; 3) Trade, wait until D_801845DC blinked twice. Ends by itself '
    .. 'within four minutes.')
print(SCRIPT_NAME .. ': main_menu signature '
    .. (signature() and 'matches' or 'mismatch (not resident yet)')
    .. '; ' .. (readOnlyReason() == nil and 'witnesses ready'
        or 'READ-ONLY until: ' .. readOnlyReason()))
