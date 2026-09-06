-- duel_rank_fusion_equip_counters.lua
--
-- WHAT THIS ANSWERS
--   The duel-rank table reads player statistics bytes +0x08 and +0x09 with
--   identical score thresholds. Static evidence establishes that one counts
--   INITIATE FUSION and the other EQUIP MAGIC, but not which byte is which.
--
--   This trace records every score-counter change while a human performs one
--   successful fusion from the hand followed by one valid equip. The first
--   candidate byte to change belongs to fusion; the second belongs to equip.
--
-- HOW TO RUN
--   1. Start an ordinary one-player duel with a deck that can perform one
--      hand fusion and then equip the resulting or another monster.
--   2. Before performing either action, open Debug -> Lua editor, paste this
--      file, and let it auto-run.
--   3. Perform exactly one successful fusion initiated from the hand. Do not
--      equip yet. Wait until the console reports a player candidate change.
--   4. Perform exactly one valid equip and wait at least three seconds.
--   5. Copy the whole document into
--      tools/trace/result/duel_rank_fusion_equip_counters.txt and fill in the
--      context.
--
--   Do not perform any other player fusion or equip during the capture.
--   Opponent actions are safe because its statistics use the second record.
--   No breakpoint, debugger pause, or interpreter CPU is required.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm the script started before both controlled actions. Name the cards
--   used in the successful hand fusion and the valid equip, state their order,
--   and mention any accidental additional player fusion or equip.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_rank_fusion_equip_counters'
local MAIN_MODE = 0x8009b26c
local DUEL_MODE = 3
local ACTIVE_SIDE = 0x8009b1d5
local OPPONENT_ID = 0x8009b361
local SIDE_RECORDS = 0x800e9ff0
local SIDE_RECORD_SIZE = 0x20
local PLAYER_SIDE = 0
local CPU_SIDE = 1
local FUSION_OR_EQUIP_A = 0x08
local FUSION_OR_EQUIP_B = 0x09
local POST_BOTH_FRAMES = 180
local TIMEOUT_FRAMES = 72000
local MAX_CHANGES = 96

local TRACKED_OFFSETS = {
    0x01, 0x02, 0x03, 0x04, 0x05,
    0x06, 0x07, 0x08, 0x09, 0x18,
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

local function recordAddress(side)
    return SIDE_RECORDS + side * SIDE_RECORD_SIZE
end

local function readCounters(side)
    local counters = {}
    local base = recordAddress(side)
    for _, offset in ipairs(TRACKED_OFFSETS) do
        counters[offset] = u8(base + offset)
    end
    return counters
end

local function countersText(counters)
    local values = {}
    for _, offset in ipairs(TRACKED_OFFSETS) do
        values[#values + 1] = string.format(
            '+%02X=%d', offset, counters[offset]
        )
    end
    return table.concat(values, ' ')
end

local lines = {}
local frames = 0
local duelFrames = 0
local changes = 0
local quietFrames = 0
local enteredDuel = false
local playerCounters = nil
local cpuCounters = nil
local playerCandidateSeen = {
    [FUSION_OR_EQUIP_A] = false,
    [FUSION_OR_EQUIP_B] = false,
}
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function captureSnapshot(reason)
    emit('')
    emit(string.format('--- snapshot: %s ---', reason))
    emit(string.format(
        '  frame=%d duel_frame=%d mode=%d active_side=%d opponent_id=%d',
        frames, duelFrames, mainMode(), u8(ACTIVE_SIDE), s8(OPPONENT_ID)
    ))
    emit('  player ' .. countersText(readCounters(PLAYER_SIDE)))
    emit('  cpu    ' .. countersText(readCounters(CPU_SIDE)))
end

local function finish(reason)
    if done then
        return
    end
    done = true

    if enteredDuel then
        captureSnapshot('final')
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm the script started before both actions; name the cards in')
    print(' the successful hand fusion and valid equip; state their order and')
    print(' whether any additional player fusion or equip occurred>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: changes=%d player_candidate_08=%s player_candidate_09=%s',
        changes,
        tostring(playerCandidateSeen[FUSION_OR_EQUIP_A]),
        tostring(playerCandidateSeen[FUSION_OR_EQUIP_B])
    ))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function recordChanges(side, oldCounters, newCounters)
    local sideName = side == PLAYER_SIDE and 'player' or 'cpu'
    local changed = false

    for _, offset in ipairs(TRACKED_OFFSETS) do
        if changes >= MAX_CHANGES then
            return changed
        end

        local oldValue = oldCounters[offset]
        local newValue = newCounters[offset]
        if oldValue ~= newValue then
            changes = changes + 1
            changed = true
            emit(string.format(
                'change=%02d frame=%d duel_frame=%d side=%s '
                .. 'offset=+0x%02X old=%d new=%d active_side=%d',
                changes,
                frames,
                duelFrames,
                sideName,
                offset,
                oldValue,
                newValue,
                u8(ACTIVE_SIDE)
            ))

            if side == PLAYER_SIDE
                and playerCandidateSeen[offset] ~= nil then
                playerCandidateSeen[offset] = true
                print(string.format(
                    '%s: player candidate +0x%02X changed from %d to %d',
                    SCRIPT_NAME, offset, oldValue, newValue
                ))
            end
        end
    end

    return changed
end

local function bothPlayerCandidatesSeen()
    return playerCandidateSeen[FUSION_OR_EQUIP_A]
        and playerCandidateSeen[FUSION_OR_EQUIP_B]
end

local function poll()
    if done then
        return
    end

    frames = frames + 1

    if not enteredDuel then
        if mainMode() == DUEL_MODE then
            enteredDuel = true
            duelFrames = 0
            if s8(OPPONENT_ID) < 0 then
                finish('entered a two-player duel; use an ordinary CPU duel')
                return
            end
            playerCounters = readCounters(PLAYER_SIDE)
            cpuCounters = readCounters(CPU_SIDE)
            captureSnapshot('entered one-player duel')
            print(SCRIPT_NAME
                .. ': armed; perform one hand fusion, wait, then one equip')
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

    local currentPlayer = readCounters(PLAYER_SIDE)
    local currentCpu = readCounters(CPU_SIDE)
    local playerChanged = recordChanges(
        PLAYER_SIDE, playerCounters, currentPlayer
    )
    local cpuChanged = recordChanges(CPU_SIDE, cpuCounters, currentCpu)
    playerCounters = currentPlayer
    cpuCounters = currentCpu

    if playerChanged or cpuChanged then
        quietFrames = 0
    elseif bothPlayerCandidatesSeen() then
        quietFrames = quietFrames + 1
    end

    if changes >= MAX_CHANGES then
        finish('maximum counter-change count reached; partial trace follows')
    elseif bothPlayerCandidatesSeen() and quietFrames >= POST_BOTH_FRAMES then
        finish('both player candidate counters changed, followed by quiet time')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out after a partial capture')
    end
end

listener_duel_rank_fusion_equip_counters =
    PCSX.Events.createEventListener(
        'GPU::Vsync',
        function()
            local ok, err = pcall(poll)
            if not ok then
                finish('script error: ' .. tostring(err))
            end
        end
    )

print('duel_rank_fusion_equip_counters: waiting for a one-player duel')
