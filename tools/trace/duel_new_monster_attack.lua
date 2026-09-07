-- duel_new_monster_attack.lua
--
-- WHAT THIS ANSWERS
--   The gameplay notes say that a monster played this turn may attack this
--   turn, but that rule has only been observed during play. Duel-card records
--   use flag 0x4000 to exclude a card from another action in the same turn.
--
--   This trace watches the player's 15 duel-card records, captures the first
--   empty-to-occupied monster placement, and records whether flag 0x4000 is
--   clear on placement and then becomes set before the active side changes.
--   It also records opponent LP so an immediate direct attack can be
--   distinguished from a position change or another card action.
--
-- HOW TO RUN
--   1. Start an ordinary one-player duel and run this script before playing
--      a card on the first player turn. The player's field should be empty.
--   2. Play exactly one monster face-up in attack position. Do not fuse,
--      equip it, change its position, or play another monster.
--   3. Immediately use that same monster for a direct attack while the
--      opponent's field is empty. Choose an attacker that will remain in play.
--      If the opponent already has a monster, restart from a clean first turn.
--   4. Wait for the script to finish, then copy the whole document into
--      tools/trace/result/duel_new_monster_attack.txt and fill in the context.
--
--   If the game refuses the attack, leave the turn open until the observation
--   window finishes and describe the exact prompt or disabled action.
--   No breakpoint, debugger pause, or interpreter CPU is required.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Name the monster and its card ID, confirm the script was armed before it
--   was played onto an empty field, say whether the direct attack was offered
--   and resolved, report the opponent LP change, and list any accidental
--   position changes, fusions, equips, or other player card actions.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_new_monster_attack'
local MAIN_MODE = 0x8009b26c
local DUEL_MODE = 3
local ACTIVE_SIDE = 0x8009b1d5
local OPPONENT_ID = 0x8009b361
local OPPONENT_LP = 0x800ea024
local CARD_RECORDS = 0x801a7ad8
local CARD_RECORD_SIZE = 0x1c
local PLAYER_RECORD_COUNT = 15
local CARD_ID_OFFSET = 0x0c
local FLAGS_OFFSET = 0x16
local PLAYER_SIDE = 0
local FLAG_USED_THIS_TURN = 0x4000
local FLAG_OCCUPIED = 0x8000
local POST_ATTACK_FRAMES = 120
local PLACEMENT_OBSERVE_FRAMES = 3600
local TIMEOUT_FRAMES = 72000
local MAX_CHANGES = 64

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

local function u16(addr)
    return tonumber(ffi.cast('uint16_t*', mem + phys(addr))[0])
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function hasFlag(value, flag)
    return math.floor(value / flag) % 2 == 1
end

local function recordAddress(index)
    return CARD_RECORDS + index * CARD_RECORD_SIZE
end

local function readRecord(index)
    local base = recordAddress(index)
    return {
        card_id = u16(base + CARD_ID_OFFSET),
        flags = u16(base + FLAGS_OFFSET),
    }
end

local function recordText(index, record)
    return string.format(
        'record=%d address=0x%08X card_id=%d flags=0x%04X '
            .. 'occupied=%s used_0x4000=%s',
        index,
        recordAddress(index),
        record.card_id,
        record.flags,
        tostring(hasFlag(record.flags, FLAG_OCCUPIED)),
        tostring(hasFlag(record.flags, FLAG_USED_THIS_TURN))
    )
end

local lines = {}
local records = {}
local frames = 0
local duelFrames = 0
local changes = 0
local quietFrames = 0
local enteredDuel = false
local watchedIndex = nil
local watchedCardID = nil
local placementFrame = nil
local placementUsed = nil
local placementOpponentLP = nil
local lastOpponentLP = nil
local usedTransition = false
local opponentLPDrop = false
local turnChanged = false
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function capture(reason)
    emit('')
    emit(string.format('--- snapshot: %s ---', reason))
    emit(string.format(
        '  frame=%d duel_frame=%d mode=0x%02X active_side=%d '
            .. 'opponent_id=%d opponent_lp=%d',
        frames,
        duelFrames,
        u8(MAIN_MODE),
        u8(ACTIVE_SIDE),
        s8(OPPONENT_ID),
        u16(OPPONENT_LP)
    ))
    if watchedIndex ~= nil then
        emit('  watched ' .. recordText(watchedIndex, readRecord(watchedIndex)))
    end
end

local function finish(reason)
    if done then
        return
    end
    done = true

    if enteredDuel then
        capture('final')
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<name the monster and card ID; confirm the script was armed before')
    print(' placement onto an empty field; say whether its immediate direct')
    print(' attack was offered and resolved; report the LP change and any')
    print(' accidental position change, fusion, equip, or other card action>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: watched_index=%s card_id=%s placement_used=%s '
            .. 'used_transition=%s opponent_lp_drop=%s turn_changed=%s',
        tostring(watchedIndex),
        tostring(watchedCardID),
        tostring(placementUsed),
        tostring(usedTransition),
        tostring(opponentLPDrop),
        tostring(turnChanged)
    ))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function enterDuel()
    enteredDuel = true
    duelFrames = 0
    if s8(OPPONENT_ID) < 0 then
        finish('entered a two-player duel; use an ordinary CPU duel')
        return
    end
    for index = 0, PLAYER_RECORD_COUNT - 1 do
        records[index] = readRecord(index)
    end
    lastOpponentLP = u16(OPPONENT_LP)
    capture('entered one-player duel')
    print(SCRIPT_NAME
        .. ': armed; play one attack-position monster and attack directly')
end

local function recordPlayerChanges()
    local changed = false

    for index = 0, PLAYER_RECORD_COUNT - 1 do
        local old = records[index]
        local current = readRecord(index)
        local wasOccupied = hasFlag(old.flags, FLAG_OCCUPIED)
        local isOccupied = hasFlag(current.flags, FLAG_OCCUPIED)

        if old.card_id ~= current.card_id or old.flags ~= current.flags then
            changed = true
            if changes < MAX_CHANGES then
                changes = changes + 1
                emit(string.format(
                    'change=%02d frame=%d duel_frame=%d active_side=%d '
                        .. 'record=%d card_id=%d->%d flags=0x%04X->0x%04X',
                    changes,
                    frames,
                    duelFrames,
                    u8(ACTIVE_SIDE),
                    index,
                    old.card_id,
                    current.card_id,
                    old.flags,
                    current.flags
                ))
            end
        end

        if watchedIndex == nil
            and u8(ACTIVE_SIDE) == PLAYER_SIDE
            and not wasOccupied
            and isOccupied
            and current.card_id ~= 0 then
            watchedIndex = index
            watchedCardID = current.card_id
            placementFrame = frames
            placementUsed =
                hasFlag(current.flags, FLAG_USED_THIS_TURN)
            placementOpponentLP = u16(OPPONENT_LP)
            emit(string.format(
                'placement frame=%d duel_frame=%d %s opponent_lp=%d',
                frames,
                duelFrames,
                recordText(index, current),
                placementOpponentLP
            ))
            capture('new player field record')
            print(string.format(
                '%s: watching record %d card %d; attack directly now',
                SCRIPT_NAME,
                watchedIndex,
                watchedCardID
            ))
        elseif watchedIndex == index
            and frames ~= placementFrame
            and not hasFlag(old.flags, FLAG_USED_THIS_TURN)
            and hasFlag(current.flags, FLAG_USED_THIS_TURN) then
            usedTransition = true
            emit(string.format(
                'used_transition frame=%d duel_frame=%d active_side=%d '
                    .. 'record=%d flags=0x%04X->0x%04X',
                frames,
                duelFrames,
                u8(ACTIVE_SIDE),
                index,
                old.flags,
                current.flags
            ))
            print(SCRIPT_NAME
                .. ': watched card flag 0x4000 became set')
        end

        records[index] = current
    end

    return changed
end

local function poll()
    if done then
        return
    end

    frames = frames + 1

    if not enteredDuel then
        if mainMode() == DUEL_MODE then
            enterDuel()
        elseif frames >= TIMEOUT_FRAMES then
            finish('timed out before one-player duel mode was observed')
        end
        return
    end

    duelFrames = duelFrames + 1
    if mainMode() ~= DUEL_MODE then
        finish('left duel mode after a partial capture')
        return
    end

    local activeSide = u8(ACTIVE_SIDE)
    local changed = recordPlayerChanges()
    local opponentLP = u16(OPPONENT_LP)

    if opponentLP ~= lastOpponentLP then
        changed = true
        emit(string.format(
            'opponent_lp_change frame=%d duel_frame=%d active_side=%d '
                .. 'old=%d new=%d',
            frames,
            duelFrames,
            activeSide,
            lastOpponentLP,
            opponentLP
        ))
        lastOpponentLP = opponentLP
    end

    if watchedIndex ~= nil then
        local watched = readRecord(watchedIndex)

        if activeSide ~= PLAYER_SIDE then
            turnChanged = true
            finish('active side changed before complete direct-attack evidence')
            return
        end
        if watched.card_id ~= watchedCardID
            or not hasFlag(watched.flags, FLAG_OCCUPIED) then
            finish('watched card left its field record before completion')
            return
        end
        if opponentLP < placementOpponentLP then
            opponentLPDrop = true
        end

        if usedTransition and opponentLPDrop then
            if changed then
                quietFrames = 0
            else
                quietFrames = quietFrames + 1
            end
            if quietFrames >= POST_ATTACK_FRAMES then
                finish('same-turn used-bit transition and opponent LP drop captured')
                return
            end
        elseif frames - placementFrame >= PLACEMENT_OBSERVE_FRAMES then
            finish('placement observation window completed without full evidence')
            return
        end
    end

    if changes >= MAX_CHANGES then
        finish('maximum player-record change count reached')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out after a partial capture')
    end
end

listener_duel_new_monster_attack = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('duel_new_monster_attack: waiting for a one-player duel')
