-- duel_card_turn_animations.lua
--
-- WHAT THIS ANSWERS
--   src/game/duel_card_turn_animations.c matches two per-frame card display
--   callbacks that are still address-named: func_800229F4, a "flip/turn"
--   step, and func_80022674, its mirror, a "turn-back" step. Both choose a
--   mode from the card record on their first frame -- 0 face-down, 1 face-up
--   attack, 2 face-up defense -- and mode 0 of the turn-back step also sets
--   record flag 0x400.
--
--   Duel_UpdateDisplayTransition (0x800235C0) is their only installer. When
--   gDuel_wDisplayTransitionFlags carries bit 0x04 it installs the flip step
--   on every occupied field card; when a view tween finishes with bit 0x01
--   it installs the turn-back step instead. notes/duel-cursor-motion.md
--   documents the flag word as a request/status pattern (0x3, 0xC, 0x10, ...)
--   but nothing says what the player sees during either step, or which
--   gameplay moment raises bit 0x01 versus 0x04. That is what both names and
--   the two bits are waiting for.
--
--   This trace polls the transition word and logs every change with the duel
--   scene state. It also breaks on both callbacks and logs one row per card
--   episode: the card, its record flags before and after, the chosen mode,
--   and how many frames the callback ran.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game, enable the debugger and select the
--      interpreter CPU. Breakpoints do not fire on the dynarec.
--   2. Debug -> Lua editor, paste this file and confirm it reports two
--      installed breakpoints and no signature mismatch.
--   3. Start any duel. Put at least one card face-down and one face-up on
--      your field, then do ordinary things: open the hand, place a card,
--      move the field cursor over your cards and the opponent's, attack,
--      and end the turn.
--   4. The trace prints when you leave the duel, or after 64 rows.
--   5. Copy the whole document into
--      tools/trace/result/duel_card_turn_animations.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For each "transition" row, say what you had just done and how the view
--   changed. For each "episode" row, say which card it was (your side or the
--   opponent's, face-up or face-down) and what that card visibly did: turned
--   over, rotated, tilted, lifted, faded, or nothing you could see.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_card_turn_animations'

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
local DUEL_MODE = 3
local SCENE_STATE = 0x8009b23a
local TRANSITION = 0x8009b162
local CARD_RECORDS = 0x801a7ad8
local CARD_RECORD_SIZE = 0x1c
local CARD_RECORD_COUNT = 30
local OBJECT_SIZE = 0x70

local TARGETS = {
    {0x800229f4, 0x27bdffe0, 'flip func_800229F4'},
    {0x80022674, 0x27bdffe0, 'turn-back func_80022674'},
}

local MAX_ROWS = 64
-- The game may update at half the vsync rate, so a callback can skip a vsync
-- without its episode having ended.
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
local mismatches = {}
local callbackError = nil
local lastTransition = nil
local sawDuel = false
local done = false

breakpoints_duel_card_turn_animations = {}

local function row(text)
    if #rows < MAX_ROWS then rows[#rows + 1] = text end
end

local function recordAddress(index)
    if index == nil or index >= CARD_RECORD_COUNT then return nil end
    return CARD_RECORDS + index * CARD_RECORD_SIZE
end

local function closeEpisode(key, ep, reason)
    local rec = recordAddress(ep.index)
    row(string.format(
        'episode=%02d %s object=0x%08X record=%s card=%s side=%s '
            .. 'flags_before=%s flags_after=%s mode=%s frames=%d '
            .. 'scene=%s transition=0x%04X %s',
        ep.number, ep.label, ep.object, show(ep.index),
        show(ep.card), ep.index and tostring(ep.index >= 15 and 1 or 0)
            or 'unknown',
        show(ep.flagsBefore, '0x%04X'),
        rec and show(u16(rec + 0x16), '0x%04X') or 'unreadable',
        show(ep.mode), ep.calls, show(ep.scene, '0x%04X'),
        ep.transition or 0, reason
    ))
    episodes[key] = nil
end

local function finish(reason)
    if done then return end
    for key, ep in pairs(episodes) do closeEpisode(key, ep, 'open') end
    done = true
    for _, bp in ipairs(breakpoints_duel_card_turn_animations) do
        bp:disable()
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<for each transition row: what you had just done and how the view')
    print(' changed; for each episode row: which card (yours or the')
    print(" opponent's, face-up or face-down) and what it visibly did>")
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP)
    print('status: ' .. reason)
    print(string.format('frames: %d duel_frames: %d episodes: %d rows: %d',
                        frames, duelFrames, episodeCount, #rows))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onHit(target)
    if done then return end
    local object = PCSX.getRegisters().GPR.n.a0
    if object % 4 ~= 0 or not inRam(object, OBJECT_SIZE) then
        row(string.format('IMPLAUSIBLE %s object=0x%08X', target[3], object))
        return
    end
    local key = string.format('%08X:%08X', target[1], object)
    local ep = episodes[key]
    if ep == nil then
        episodeCount = episodeCount + 1
        local index = u8(object + 0x6a)
        local rec = recordAddress(index)
        ep = {
            number = episodeCount,
            label = target[3],
            object = object,
            index = index,
            card = rec and s16(rec + 0x0c) or nil,
            flagsBefore = rec and u16(rec + 0x16) or nil,
            scene = u16(SCENE_STATE),
            transition = u16(TRANSITION),
            calls = 0,
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
        -- The mode is chosen inside the first call, so read it afterwards.
        if ep.mode == nil then ep.mode = s16(ep.object + 0x2e) end
        if frames - ep.lastFrame > EPISODE_GAP_FRAMES then closeEpisode(key, ep, 'ended') end
    end

    local mode = u8(MAIN_MODE)
    local inDuel = mode ~= nil and mode % 32 == DUEL_MODE
    if inDuel then
        sawDuel = true
        duelFrames = duelFrames + 1
        local transition = u16(TRANSITION)
        if transition ~= lastTransition then
            row(string.format('transition frame=%06d 0x%04X -> 0x%04X scene=%s',
                              frames, lastTransition or 0, transition or 0,
                              show(u16(SCENE_STATE), '0x%04X')))
            lastTransition = transition
        end
    elseif sawDuel then
        finish('left the duel')
        return
    end

    if #rows >= MAX_ROWS then
        finish('captured ' .. MAX_ROWS .. ' rows')
    elseif duelFrames >= NO_HIT_WARNING_FRAMES and episodeCount == 0 then
        finish('no callback hit in ten minutes of duel; select the '
               .. 'interpreter CPU')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

for _, target in ipairs(TARGETS) do
    local word = u32(target[1])
    if word ~= target[2] then
        mismatches[#mismatches + 1] = string.format(
            '0x%08X expected 0x%08X read %s', target[1], target[2],
            show(word, '0x%08X'))
    end
    local t = target
    breakpoints_duel_card_turn_animations[#breakpoints_duel_card_turn_animations + 1] =
        PCSX.addBreakpoint(t[1], 'Exec', 4, SCRIPT_NAME .. ': ' .. t[3],
            function()
                local ok, err = pcall(onHit, t)
                if not ok then callbackError = tostring(err) end
            end)
end

listener_duel_card_turn_animations = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches',
                    SCRIPT_NAME, #breakpoints_duel_card_turn_animations,
                    #mismatches))
