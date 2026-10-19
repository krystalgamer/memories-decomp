-- duel_scene_state8.lua
--
-- WHAT THIS ANSWERS
--   gDuel_apfnSceneStateHandler (src/game/duel_scene_callbacks.c) has fifteen
--   duel scene states, and state 8, func_8001B170, is the only one without a
--   name. DuelScene_UpdateCardPlacement enters it after a card has been
--   placed (scene word 0x4008 or 8). Its own sub-state byte D_8009B174 then
--   runs six phases:
--
--     1-2  slide the placed card's object to (0x86, 0x2A); a card whose
--          object type +0x68 is 0x14 or more skips straight to phase 4
--     3    open text box 0x21 with a choice and read it; the answer sets or
--          clears DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2 (0x200) on the record
--     4    put a card from the deck onto the field (Duel_SetupFieldCardFromDeck
--          with D_8009B19C) and swap the placed object for the new one
--     5    if D_8009B154 is nonzero and Duel_SelectTrapByCardId finds
--          Reverse Trap (689) in the trap row it searches, go to 6;
--          otherwise return to scene state 5
--     6    run the trap presentation, request duel effect 0x0D carrying
--          -D_8009B154, play sound 0x21, then lower the card's stat modifier
--          by D_8009B154 * 2
--
--   DuelScene_UpdateCardPlacement adds 500 to D_8009B154 for each equip it
--   commits (1000 for card 0x291) and zeroes it for a fusion, so phase 6
--   reads like Reverse Trap turning an equip bonus into a penalty, and phase
--   3 like the guardian-star choice. Neither reading has been checked against
--   the screen, nor has which placements reach phase 4's deck-to-field
--   setup or which side's Reverse Trap answers. Naming the state, the
--   sub-state byte, D_8009B154 and D_8009B19C depends on that.
--
--   This trace polls the scene word. While state 8 runs it logs each change
--   of the sub-state byte, D_8009B154, D_8009B19C, D_8009B218, the text box's
--   string id and the placed card's id, flags and stat modifier, so each
--   episode reads as a phase sequence the human can match to the screen.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game. No breakpoint or interpreter CPU is
--      needed.
--   2. Debug -> Lua editor, paste this file and confirm the waiting message.
--   3. In a duel, place cards in as many ways as you can: a monster face-up
--      and face-down, an equip onto a monster, a magic card, a trap, and a
--      fusion from the hand. Answer any question the game asks. If you can,
--      equip a monster while the other side has a Reverse Trap set.
--   4. The trace prints after six state-8 episodes, 64 rows, or when you
--      leave the duel after at least one episode.
--   5. Copy the whole document into
--      tools/trace/result/duel_scene_state8.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For each episode, say exactly what you placed and how, the full text of
--   any question shown and what you answered, which card appeared or
--   changed afterwards, and any ATK/DEF change you saw.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_scene_state8'

local RAM_BASE = 0x80000000
local RAM_END = 0x80200000

local MAIN_MODE = 0x8009b26c
local DUEL_MODE = 3
local SCENE = 0x8009b23a
local STATE8 = 8
local SUBSTATE = 0x8009b174
local AMOUNT = 0x8009b154
local DECK_RECORD = 0x8009b19c
local FLAG_218 = 0x8009b218
local COUNTER_210 = 0x8009b210
local SELECTED_CARD = 0x8009b338
local OBJECT_SLOT = 0x800e9ef0
local TEXT_BOX = 0x800eb0f8
local CARD_RECORDS = 0x801a7ad8
local CARD_RECORD_SIZE = 0x1c
local CARD_RECORD_COUNT = 30
local OBJECT_SIZE = 0x70

local MAX_EPISODES = 6
local MAX_ROWS = 64
local HISTORY = 4
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
local rows = {}
local history = {}
local episodes = 0
local inEpisode = false
local lastScene = nil
local lastState = nil
local sawDuel = false
local done = false

local function row(text)
    if #rows < MAX_ROWS then
        rows[#rows + 1] = string.format('frame=%06d ', frames) .. text
    end
end

local function placedCard()
    local object = u32(OBJECT_SLOT)
    if object == nil or object % 4 ~= 0 or not inRam(object, OBJECT_SIZE) then
        return string.format('object=%s', show(object, '0x%08X'))
    end
    local index = u8(object + 0x6a)
    local text = string.format('object=0x%08X type68=%s record=%s',
                               object, show(u8(object + 0x68), '0x%02X'),
                               show(index))
    if index == nil or index >= CARD_RECORD_COUNT then return text end
    local rec = CARD_RECORDS + index * CARD_RECORD_SIZE
    return text .. string.format(' card=%s flags=%s stat_modifier=%s',
                                 show(s16(rec + 0x0c)),
                                 show(u16(rec + 0x16), '0x%04X'),
                                 show(s16(rec + 0x12)))
end

local function state()
    return string.format(
        'sub=%s amount=%s deck_record=%s f218=%s c210=%s text=%s selected=%s',
        show(u8(SUBSTATE), '0x%02X'), show(s16(AMOUNT)), show(u8(DECK_RECORD)),
        show(s8(FLAG_218)), show(u16(COUNTER_210)),
        show(u16(TEXT_BOX + 0x36), '0x%03X'), show(s16(SELECTED_CARD)))
end

local function finish(reason)
    if done then return end
    done = true
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<for each episode: what you placed and how, the full text of any')
    print(' question and your answer, which card appeared or changed, and any')
    print(' ATK/DEF change you saw>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format('frames: %d episodes: %d rows: %d', frames, episodes,
                        #rows))
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function poll()
    if done then return end
    frames = frames + 1

    local mode = u8(MAIN_MODE)
    if mode == nil or mode % 32 ~= DUEL_MODE then
        if sawDuel then
            finish(episodes > 0 and 'left the duel'
                   or 'left the duel without ever entering state 8')
        end
        if frames >= TIMEOUT_FRAMES then finish('timed out') end
        return
    end
    sawDuel = true

    local scene = u16(SCENE)
    if scene ~= lastScene then
        local isState8 = scene % 16 == STATE8
        if isState8 and not inEpisode then
            episodes = episodes + 1
            inEpisode = true
            row(string.format('=== episode %d enters state 8 ===', episodes))
            for _, h in ipairs(history) do row('  before: ' .. h) end
            row(string.format('  scene 0x%04X -> 0x%04X %s', lastScene or 0,
                              scene, state()))
            row('  ' .. placedCard())
            lastState = state()
        elseif inEpisode and not isState8 then
            inEpisode = false
            row(string.format('  leaves to scene 0x%04X %s', scene, state()))
            row('  ' .. placedCard())
            if episodes >= MAX_EPISODES then
                finish('captured ' .. MAX_EPISODES .. ' episodes')
                return
            end
        elseif inEpisode then
            row(string.format('  scene 0x%04X -> 0x%04X', lastScene or 0, scene))
        end
        history[#history + 1] = string.format('scene 0x%04X -> 0x%04X',
                                              lastScene or 0, scene)
        if #history > HISTORY then table.remove(history, 1) end
        lastScene = scene
    end

    if inEpisode then
        local now = state()
        if now ~= lastState then
            row('  ' .. now)
            lastState = now
        end
    end

    if #rows >= MAX_ROWS then
        finish('captured ' .. MAX_ROWS .. ' rows')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

listener_duel_scene_state8 = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(SCRIPT_NAME .. ': waiting for duel scene state 8; place cards in a duel')
