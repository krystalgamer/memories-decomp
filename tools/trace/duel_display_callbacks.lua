-- duel_display_callbacks.lua
--
-- WHAT THIS ANSWERS
--   Six per-frame duel display callbacks and one sprite constructor are still
--   address-named because their code shows the mechanics but not what the
--   player sees:
--
--   func_8001EC70  cosine-eases an object from its saved position to the
--                  target at +0x28/+0x2A over +0x2C frames, then clears
--                  update. Installed by the ritual controller
--                  (duel_field_effects.c), hand actions, draw resolution and
--                  the result banners. Is it a card sliding into a slot?
--   func_8001ED20  the same ease, but it also ramps tint byte +0x22 and
--                  finally clears the clip-test flag. Installed from
--                  duel_scene_battle.c. What does +0x22 fade?
--   func_80019BD0  ramps byte +0x21 by 8 a frame up to 0x40 (0xC0 if it was
--                  already set), then runs a second phase and on completion
--                  clears DUEL_CARD_FLAG_DISPLAY_MARKER on the card record.
--                  Installed by DuelScene_UpdateCardPlacement. Is it the
--                  "just placed" highlight or a selection marker?
--   func_800229F4 / func_80022674  the flip and turn-back card steps that
--                  Duel_UpdateDisplayTransition installs on occupied field
--                  cards for transition bits 0x04 and 0x01. Which gameplay
--                  moment raises which bit?
--   func_80020EE8  a result-outro confetti callback: sets clip-test, lowers
--                  +0x21 by 2 a frame and releases below 0xC0. Does it run
--                  after DuelResult_UpdateOrbitSprite, as the fly-off?
--   Duel_CreateFieldCardPanel  acquires a fixed 0x1F/0x100 sprite at (a0, a1) on
--                  ordering table 1 in screen space. Its only caller is
--                  Duel_UpdateDisplayTransition with transition bit 0x02,
--                  at (0, screen height). Hand tray or info bar?
--
--   Each callback call is grouped into one episode per object: the start and
--   target positions, frame count, tint bytes before and after, the card
--   record and its flags before and after, and the duel scene state. The
--   transition word is polled, and DuelScene_Update is a control that proves
--   breakpoints fire.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file, and wait for "execution
--      breakpoint observed; setup confirmed" once a duel is running.
--   3. Play one duel to a WIN. On the way: open the hand, place a monster
--      face-up and another face-down, move the field cursor over both sides,
--      attack, end the turn, and if you can, summon a ritual. Let the result
--      screen play fully, including the confetti.
--   4. The trace prints when you leave the duel, or after 96 rows.
--   5. Copy the whole document into
--      tools/trace/result/duel_display_callbacks.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For each "transition" and "sprite" row: what you had just done and what
--   appeared or moved, especially at the screen bottom. For each "episode"
--   row: which card or sprite it was, where it moved from and to, and what
--   faded, glowed, flipped or flew off. Say when the confetti appeared and
--   when it left.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_display_callbacks'

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
local DUEL_MODE = 3
local SCENE_STATE = 0x8009b23a
local RESULT_STATE = 0x8009b174
local TRANSITION = 0x8009b162
local CARD_RECORDS = 0x801a7ad8
local CARD_RECORD_SIZE = 0x1c
local CARD_RECORD_COUNT = 30
local OBJECT_SIZE = 0x70

-- Entry, retail first word, label, rows allowed for this callback.
local CALLBACKS = {
    {0x8001ec70, 0x27bdffe8, 'ease func_8001EC70', 16},
    {0x8001ed20, 0x27bdffe8, 'ease+tint func_8001ED20', 12},
    {0x80019bd0, 0x27bdffe8, 'marker func_80019BD0', 12},
    {0x800229f4, 0x27bdffe0, 'flip func_800229F4', 12},
    {0x80022674, 0x27bdffe0, 'turn-back func_80022674', 12},
    {0x80020ee8, 0x27bdffe8, 'fly-off func_80020EE8', 8},
    {0x80020d4c, 0x27bdffc8, 'orbit DuelResult_UpdateOrbitSprite', 4},
}
local SPRITE = {0x80018150, 0x27bdffd0, 'sprite Duel_CreateFieldCardPanel'}
local CONTROL = {0x80024200, 0x9782025a, 'control DuelScene_Update'}

local MAX_ROWS = 96
local SPRITE_ROWS = 6
-- The game can update at half the vsync rate, so a callback may skip a
-- vsync without its episode having ended.
local EPISODE_GAP_FRAMES = 4
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
local function u16(addr)
    if not inRam(addr, 2) then return nil end
    return tonumber(ffi.cast('uint16_t*', mem + (addr - RAM_BASE))[0])
end
local function s16(addr)
    local v = u16(addr)
    if v ~= nil and v >= 0x8000 then v = v - 0x10000 end
    return v
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
local duelFrames = 0
local rows = {}
local episodes = {}
local episodeCount = 0
local logged = {}
local counts = {}
local controlHits = 0
local spriteRows = 0
local mismatches = {}
local callbackError = nil
local lastTransition = nil
local sawDuel = false
local done = false

breakpoints_duel_display_callbacks = {}

local function row(text)
    if #rows >= MAX_ROWS then return false end
    rows[#rows + 1] = text
    return true
end

local function context()
    local mode = u8(MAIN_MODE)
    return string.format('mode=%s scene=%s result_state=%s transition=%s',
                         mode and string.format('0x%02X', mode % 32)
                         or 'unreadable',
                         show(u16(SCENE_STATE), '0x%04X'),
                         show(u8(RESULT_STATE), '0x%02X'),
                         show(u16(TRANSITION), '0x%04X'))
end

local function tint(object)
    return string.format('%s,%s,%s,%s', show(u8(object + 0x20), '%02X'),
                         show(u8(object + 0x21), '%02X'),
                         show(u8(object + 0x22), '%02X'),
                         show(u8(object + 0x23), '%02X'))
end

-- The record byte at +0x6A only names a card for duel card objects; for
-- banners and confetti it is whatever the slot held, so it is labelled.
local function card(index)
    if index == nil or index >= CARD_RECORD_COUNT then
        return 'record=' .. show(index) .. ' (not a card record)', nil
    end
    local rec = CARD_RECORDS + index * CARD_RECORD_SIZE
    return string.format('record=%d side=%d card=%s', index,
                         index >= 15 and 1 or 0, show(s16(rec + 0x0c))),
        rec
end

local function closeEpisode(key, ep, reason)
    local recordFlags = ep.rec and show(u16(ep.rec + 0x16), '0x%04X')
        or 'n/a'
    row(string.format(
        'episode=%02d %s object=0x%08X %s calls=%d start=(%s,%s) '
            .. 'target=(%s,%s) frames_field=%s end=(%s,%s) mode2E=%s '
            .. 'tint=%s->%s flags=%s->%s card_flags=%s->%s update=%s %s '
            .. '[%s] %s',
        ep.number, ep.label, ep.object, ep.card, ep.calls, show(ep.x),
        show(ep.y), show(ep.tx), show(ep.ty), show(ep.span),
        show(s16(ep.object + 0x30)), show(s16(ep.object + 0x32)),
        show(ep.mode2E), ep.tintBefore, tint(ep.object),
        show(ep.flags, '0x%04X'), show(u16(ep.object + 0x08), '0x%04X'),
        show(ep.recFlags, '0x%04X'), recordFlags,
        show(u32(ep.object + 0x24), '0x%08X'), reason, ep.context,
        ep.firstFrame and string.format('frame=%06d', ep.firstFrame) or ''))
    episodes[key] = nil
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_duel_display_callbacks) do bp:disable() end
    for key, ep in pairs(episodes) do
        local ok, err = pcall(closeEpisode, key, ep, 'open')
        if not ok and callbackError == nil then
            callbackError = 'closing episode: ' .. tostring(err)
        end
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<for each transition and sprite row: what you had just done and')
    print(' what appeared or moved, especially at the screen bottom; for each')
    print(' episode row: which card or sprite, from where to where, and what')
    print(' faded, glowed, flipped or flew off; when the confetti came and went>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d duel_frames: %d episodes: %d rows: %d '
                        .. 'control_hits: %d', frames, duelFrames,
                        episodeCount, #rows, controlHits))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    for _, cb in ipairs(CALLBACKS) do
        print(string.format('count %-36s %d', cb[3], counts[cb[1]] or 0))
    end
    print(string.format('count %-36s %d', SPRITE[3], counts[SPRITE[1]] or 0))
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onCallback(cb)
    if done then return end
    counts[cb[1]] = (counts[cb[1]] or 0) + 1
    local object = PCSX.getRegisters().GPR.n.a0
    if object % 4 ~= 0 or not inRam(object, OBJECT_SIZE) then
        row(string.format('IMPLAUSIBLE %s object=0x%08X', cb[3], object))
        return
    end
    local key = string.format('%08X:%08X', cb[1], object)
    local ep = episodes[key]
    if ep == nil then
        if (logged[cb[1]] or 0) >= cb[4] or #rows >= MAX_ROWS then return end
        logged[cb[1]] = (logged[cb[1]] or 0) + 1
        episodeCount = episodeCount + 1
        local cardText, rec = card(u8(object + 0x6a))
        ep = {
            number = episodeCount, label = cb[3], object = object,
            card = cardText, rec = rec,
            recFlags = rec and u16(rec + 0x16) or nil,
            x = s16(object + 0x30), y = s16(object + 0x32),
            tx = s16(object + 0x28), ty = s16(object + 0x2a),
            span = u16(object + 0x2c), tintBefore = tint(object),
            flags = u16(object + 0x08), context = context(),
            firstFrame = frames, calls = 0,
        }
        episodes[key] = ep
    end
    ep.calls = ep.calls + 1
    ep.lastFrame = frames
end

local function onSprite()
    if done then return end
    counts[SPRITE[1]] = (counts[SPRITE[1]] or 0) + 1
    if spriteRows >= SPRITE_ROWS then return end
    local n = PCSX.getRegisters().GPR.n
    if row(string.format('sprite frame=%06d %s x=%d y=%d caller=0x%08X %s',
                         frames, SPRITE[3], n.a0, n.a1, n.ra, context())) then
        spriteRows = spriteRows + 1
    end
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    for key, ep in pairs(episodes) do
        -- Mode is chosen inside the first flip/turn-back call.
        if ep.mode2E == nil and ep.lastFrame < frames then
            ep.mode2E = s16(ep.object + 0x2e)
        end
        if frames - ep.lastFrame > EPISODE_GAP_FRAMES then
            closeEpisode(key, ep, 'ended')
        end
    end

    local mode = u8(MAIN_MODE)
    if mode ~= nil and mode % 32 == DUEL_MODE then
        sawDuel = true
        duelFrames = duelFrames + 1
        local transition = u16(TRANSITION)
        if transition ~= lastTransition then
            row(string.format('transition frame=%06d 0x%04X -> 0x%04X %s',
                              frames, lastTransition or 0, transition or 0,
                              context()))
            lastTransition = transition
        end
    elseif sawDuel then
        finish('left the duel')
        return
    end

    if #rows >= MAX_ROWS then
        finish('captured ' .. MAX_ROWS .. ' rows')
    elseif duelFrames >= NO_HIT_WARNING_FRAMES and controlHits == 0 then
        finish('ten minutes in a duel without a control hit; the setup is '
               .. 'not working, check the setup line')
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
    breakpoints_duel_display_callbacks[#breakpoints_duel_display_callbacks + 1] =
        traceAddBreakpoint(target[1], 'Exec', 4,
            SCRIPT_NAME .. ': ' .. target[3], function()
                local ok, err = pcall(handler)
                if not ok then callbackError = tostring(err) end
            end)
end

for _, cb in ipairs(CALLBACKS) do
    local c = cb
    install(c, function() onCallback(c) end)
end
install(SPRITE, onSprite)
install(CONTROL, function() controlHits = controlHits + 1 end)

listener_duel_display_callbacks = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches',
                    SCRIPT_NAME, #breakpoints_duel_display_callbacks,
                    #mismatches))
