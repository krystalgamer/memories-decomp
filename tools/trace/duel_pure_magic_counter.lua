-- duel_pure_magic_counter.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md identifies the player rank byte at +0x05 as
--   PURE MAGIC. The candidate writer increments it only for card type 0x14
--   before the later effect requests. That type filter is code-backed, but no
--   controlled runtime trace confirms the counter change for a visible card.
--
--   This trace baselines the player's rank counters, records every change, and
--   stops after the pure-magic byte changes. Human context identifies the card
--   and confirms that no fusion, equip, trap, or extra magic action occurred.
--
-- HOW TO RUN
--   1. Start an ordinary one-player duel and paste this script during a player
--      turn before activating the test card.
--   2. Activate exactly one ordinary Magic card whose effect resolves in that
--      turn. Do not fuse, equip, trigger a trap, or use another Magic card.
--   3. Leave the duel open until the script finishes. If the card is refused
--      or the counter does not change, describe the exact visible result.
--   4. Copy the whole document into
--      tools/trace/result/duel_pure_magic_counter.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Name the card and card ID, confirm it was ordinary Magic rather than equip,
--   ritual, or trap, describe its visible effect, list any accidental action,
--   and say whether the effect completed before the counter changed.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_pure_magic_counter'
local MAIN_MODE = 0x8009b26c
local ACTIVE_SIDE = 0x8009b1d5
local OPPONENT_ID = 0x8009b361
local PLAYER_STATS = 0x800e9ff0
local DUEL_MODE = 3
local PLAYER_SIDE = 0
local MAX_CHANGES = 32
local NO_PLAYER_TURN_WARNING_FRAMES = 600
local PLAYER_TURN_WAIT_TIMEOUT_FRAMES = 72000
local OBSERVATION_TIMEOUT_FRAMES = 36000
local POST_INCREMENT_FRAMES = 120

local COUNTERS = {
    {name = 'turns', offset = 0x01},
    {name = 'pure_magic', offset = 0x05},
    {name = 'traps', offset = 0x06},
    {name = 'fusions', offset = 0x08},
    {name = 'equips', offset = 0x09},
    {name = 'cards_used', offset = 0x18},
}

local mem = PCSX.getMemPtr()

local function phys(addr)
    return addr - 0x80000000
end

local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end

local function s8(addr)
    local value = u8(addr)
    if value >= 0x80 then
        return value - 0x100
    end
    return value
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function snapshotCounters()
    local values = {}
    for _, counter in ipairs(COUNTERS) do
        values[counter.name] = u8(PLAYER_STATS + counter.offset)
    end
    return values
end

local function delta(before, after)
    return (after - before) % 0x100
end

local function snapshotText(prefix, frame, values)
    return string.format(
        '%s frame=%06d mode=%d active_side=%d opponent_id=%d '
            .. 'turns=%d pure_magic=%d traps=%d fusions=%d equips=%d '
            .. 'cards_used=%d',
        prefix,
        frame,
        mainMode(),
        u8(ACTIVE_SIDE),
        s8(OPPONENT_ID),
        values.turns,
        values.pure_magic,
        values.traps,
        values.fusions,
        values.equips,
        values.cards_used
    )
end

local function changed(before, after)
    for _, counter in ipairs(COUNTERS) do
        if before[counter.name] ~= after[counter.name] then
            return true
        end
    end
    return false
end

local lines = {}
local frames = 0
local started = false
local startFrame = nil
local baseline = nil
local lastValues = nil
local pureMagicFrame = nil
local changeCount = 0
local warned = false
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function finish(reason)
    if done then
        return
    end
    done = true

    local final = snapshotCounters()
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<name the card/id and visible effect; confirm ordinary Magic type;')
    print(' list any fusion, equip, trap, or extra magic action; state whether')
    print(' the effect completed before the pure-magic counter changed>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: started=%s changes=%d pure_magic_observed=%s '
            .. 'mode=%d active_side=%d opponent_id=%d',
        tostring(started),
        changeCount,
        tostring(pureMagicFrame ~= nil),
        mainMode(),
        u8(ACTIVE_SIDE),
        s8(OPPONENT_ID)
    ))
    if baseline ~= nil then
        print(snapshotText('baseline', startFrame, baseline))
        print(snapshotText('final', frames, final))
        print(string.format(
            'delta turns=%d pure_magic=%d traps=%d fusions=%d '
                .. 'equips=%d cards_used=%d',
            delta(baseline.turns, final.turns),
            delta(baseline.pure_magic, final.pure_magic),
            delta(baseline.traps, final.traps),
            delta(baseline.fusions, final.fusions),
            delta(baseline.equips, final.equips),
            delta(baseline.cards_used, final.cards_used)
        ))
    end
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function startObservation()
    started = true
    startFrame = frames
    baseline = snapshotCounters()
    lastValues = baseline
    emit(snapshotText('observation_start', frames, baseline))
    print(SCRIPT_NAME
        .. ': player turn observed; activate exactly one ordinary Magic card')
end

local function recordChanges(current)
    if not changed(lastValues, current) then
        return
    end
    changeCount = changeCount + 1
    emit(snapshotText(
        string.format('change=%02d', changeCount),
        frames,
        current
    ))
    lastValues = current

    if pureMagicFrame == nil
        and current.pure_magic ~= baseline.pure_magic then
        pureMagicFrame = frames
        emit(string.format(
            'pure_magic_increment frame=%06d delta=%d',
            frames,
            delta(baseline.pure_magic, current.pure_magic)
        ))
        print(SCRIPT_NAME
            .. ': pure-magic counter changed; record the visible effect')
    end
    if changeCount >= MAX_CHANGES then
        finish('maximum counter change count reached; partial trace follows')
    end
end

local function poll()
    if done then
        return
    end
    frames = frames + 1

    if mainMode() ~= DUEL_MODE then
        if started then
            finish('left duel mode before observation completed')
        else
            finish('left duel mode before a player turn was observed')
        end
        return
    end

    if not started then
        if u8(ACTIVE_SIDE) == PLAYER_SIDE then
            startObservation()
        elseif not warned and frames >= NO_PLAYER_TURN_WARNING_FRAMES then
            warned = true
            print(SCRIPT_NAME
                .. ': waiting for player side 0; keep the duel running')
        end
        if frames >= PLAYER_TURN_WAIT_TIMEOUT_FRAMES then
            finish('timed out before a player turn was observed')
        end
        return
    end

    local current = snapshotCounters()
    recordChanges(current)
    if done then
        return
    end
    if u8(ACTIVE_SIDE) ~= PLAYER_SIDE then
        if pureMagicFrame == nil then
            finish('player turn ended without a pure-magic increment')
        else
            finish('player turn ended after the pure-magic increment')
        end
    elseif pureMagicFrame ~= nil
        and frames - pureMagicFrame >= POST_INCREMENT_FRAMES then
        finish('captured pure-magic increment and follow-up window')
    elseif frames - startFrame >= OBSERVATION_TIMEOUT_FRAMES then
        finish('timed out without a pure-magic increment')
    end
end

if mainMode() ~= DUEL_MODE then
    finish(string.format(
        'main mode %d is not duel mode %d',
        mainMode(),
        DUEL_MODE
    ))
else
    if u8(ACTIVE_SIDE) == PLAYER_SIDE then
        startObservation()
    end
    listener_duel_pure_magic_counter_reset =
        PCSX.Events.createEventListener(
            'ExecutionFlow::Reset',
            function()
                local ok, err = pcall(function()
                    finish('reset observed; rerun from the controlled duel')
                end)
                if not ok then
                    finish('reset callback error: ' .. tostring(err))
                end
            end
        )
    listener_duel_pure_magic_counter =
        PCSX.Events.createEventListener(
            'GPU::Vsync',
            function()
                local ok, err = pcall(poll)
                if not ok then
                    finish('script error: ' .. tostring(err))
                end
            end
        )
    if not started then
        print(SCRIPT_NAME .. ': armed; waiting for player side 0')
    end
end
