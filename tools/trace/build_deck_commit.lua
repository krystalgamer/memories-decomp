-- build_deck_commit.lua
--
-- WHAT THIS ANSWERS
--   Build Deck edits change the working count at 0x801D560C while the
--   save-block deck at gDuel_awPlayerDeck appears unchanged. The remaining
--   question is whether a genuinely changed 40-card deck is committed to the
--   save block only when the player exits the screen.
--
--   This records the working count and all 40 save-block card IDs when Build
--   Deck is entered, whenever either value set changes, and for three seconds
--   after the screen exits.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game.
--   2. Debug -> Lua editor, paste this file, and let it auto-run.
--   3. Enter Build Deck with a valid 40-card deck.
--   4. Remove one card, add a different card, and exit with 40 cards.
--      Do not restore the original deck before exiting.
--   5. Copy the whole document into
--      tools/trace/result/build_deck_commit.txt and fill in the context.
--
--   No breakpoint, debugger pause, or interpreter CPU is required.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Identify the removed and added cards, state whether the working count
--   changed as expected, and confirm that Build Deck accepted the exit.

local ffi = require('ffi')

local SCRIPT_NAME = 'build_deck_commit'
local MAIN_MODE = 0x8009b26c
local BUILD_DECK_MODE = 7
local ACTIVE_BUFFER = 0x8009b0ac
local SELECTED_CARD = 0x8009b338
local WORKING_DECK_COUNT = 0x801d560c
local SAVED_DECK = 0x801d0200
local DECK_SIZE = 40
local POST_EXIT_FRAMES = 180
local TIMEOUT_FRAMES = 36000
local MAX_SAMPLES = 16

local mem = PCSX.getMemPtr()

local function phys(addr) return addr - 0x80000000 end
local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end
local function u16(addr)
    return tonumber(ffi.cast('uint16_t*', mem + phys(addr))[0])
end

local function readDeck()
    local cards = {}
    for i = 0, DECK_SIZE - 1 do
        cards[#cards + 1] = u16(SAVED_DECK + i * 2)
    end
    return cards
end

local function deckKey(cards)
    return table.concat(cards, ',')
end

local lines = {}
local samples = 0
local frames = 0
local enteredBuildDeck = false
local leftBuildDeck = false
local postExitFrames = 0
local lastKey = nil
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function capture(reason, count, cards)
    if samples >= MAX_SAMPLES then
        return
    end

    samples = samples + 1
    emit('')
    emit(string.format('--- sample %d: %s ---', samples, reason))
    emit(string.format(
        '  mode=0x%02X active_buffer=%d working_count=%d selected_card=%d',
        u8(MAIN_MODE), u8(ACTIVE_BUFFER), count, u16(SELECTED_CARD)
    ))
    emit('  gDuel_awPlayerDeck: ' .. deckKey(cards))
end

local function finish(reason)
    if done then
        return
    end
    done = true

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<identify the removed and added cards, describe the working-count')
    print(' changes, and confirm whether Build Deck accepted the exit>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function poll()
    if done then
        return
    end

    frames = frames + 1
    local mode = u8(MAIN_MODE) % 32

    if not enteredBuildDeck then
        if mode == BUILD_DECK_MODE then
            enteredBuildDeck = true
            local cards = readDeck()
            lastKey = u16(WORKING_DECK_COUNT) .. ':' .. deckKey(cards)
            capture(
                'entered Build Deck',
                u16(WORKING_DECK_COUNT),
                cards
            )
        elseif frames >= TIMEOUT_FRAMES then
            finish('timed out before Build Deck was observed')
        end
        return
    end

    local cards = readDeck()
    local count = u16(WORKING_DECK_COUNT)
    local key = count .. ':' .. deckKey(cards)
    if key ~= lastKey then
        lastKey = key
        capture('working count or save-block deck changed', count, cards)
        if samples >= MAX_SAMPLES then
            finish('maximum sample count reached')
            return
        end
    end

    if mode ~= BUILD_DECK_MODE then
        if not leftBuildDeck then
            leftBuildDeck = true
            capture('left Build Deck', count, cards)
        end
        postExitFrames = postExitFrames + 1
        if postExitFrames >= POST_EXIT_FRAMES then
            cards = readDeck()
            capture(
                'three seconds after leaving Build Deck',
                u16(WORKING_DECK_COUNT),
                cards
            )
            finish('captured Build Deck exit')
        end
    elseif frames >= TIMEOUT_FRAMES then
        capture('timeout snapshot', count, cards)
        finish('timed out before Build Deck exited')
    end
end

listener_build_deck_commit = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('build_deck_commit: waiting for Build Deck; make one lasting swap')
