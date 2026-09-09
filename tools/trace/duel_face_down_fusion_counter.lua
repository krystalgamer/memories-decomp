-- duel_face_down_fusion_counter.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md proves that the normal single-card commitment
--   path increments the current side's rank statistic at +0x04 for a
--   face-down play. Numbered multi-card selections bypass that writer, so
--   static code does not establish whether a successful fusion placed
--   face-down is credited by another path.
--
--   This trace compares two complete player turns: one controlled single-card
--   face-down play, then one multi-card fusion whose result is placed
--   face-down. It records the face-down, fusion, equip, turn and draw-cursor
--   counters at each turn boundary and reports deltas even when a counter does
--   not change. Human context confirms the exact actions and whether the
--   fusion succeeded.
--
-- HOW TO RUN
--   1. Start an ordinary one-player duel with a deck and opening hand that can
--      make a simple fusion on the next player turn.
--   2. Paste this script after the first controlled player hand is drawn. No
--      interpreter CPU or debugger breakpoint is required.
--   3. On the first captured turn, play exactly one card face-down and end.
--      Do not fuse, equip, activate magic/traps, or change another card.
--   4. On the next captured player turn, select at least two hand cards,
--      complete a successful fusion, place the result face-down, and end.
--      Do not perform another fusion, equip, magic, trap, or position change.
--   5. Copy the whole document into
--      tools/trace/result/duel_face_down_fusion_counter.txt and fill in
--      context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Name the single card and fusion materials/result, confirm both placements
--   were face-down, list any accidental extra action, and state whether the
--   fusion completed before the turn ended.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_face_down_fusion_counter'
local MAIN_MODE = 0x8009b26c
local ACTIVE_SIDE = 0x8009b1d5
local OPPONENT_ID = 0x8009b361
local PLAYER_STATS = 0x800e9ff0
local DUEL_MODE = 3
local PLAYER_SIDE = 0
local MAX_CHANGES = 32
local NO_DUEL_WARNING_FRAMES = 600
local TURN_TIMEOUT_FRAMES = 36000
local TIMEOUT_FRAMES = 108000

local COUNTERS = {
    {name = 'turns', offset = 0x01},
    {name = 'face_down', offset = 0x04},
    {name = 'fusions', offset = 0x08},
    {name = 'equips', offset = 0x09},
    {name = 'draw_cursor', offset = 0x18},
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

local function snapshot()
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
        '%s frame=%06d active_side=%d turns=%d face_down=%d '
            .. 'fusions=%d equips=%d draw_cursor=%d',
        prefix,
        frame,
        u8(ACTIVE_SIDE),
        values.turns,
        values.face_down,
        values.fusions,
        values.equips,
        values.draw_cursor
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
local phase = 0
local turnNumber = 0
local completedTurns = 0
local turnStartFrame = nil
local turnStart = nil
local lastValues = nil
local changeCount = 0
local noDuelWarningPrinted = false
local callbackError = nil
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function finish(reason)
    if done then
        return
    end
    done = true

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<name the single card and fusion materials/result; confirm both')
    print(' placements were face-down; list accidental extra actions; state')
    print(' whether the fusion succeeded before the second turn ended>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: completed_turns=%d changes=%d mode=%d active_side=%d '
            .. 'opponent_id=%d',
        completedTurns,
        changeCount,
        mainMode(),
        u8(ACTIVE_SIDE),
        s8(OPPONENT_ID)
    ))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function turnLabel(number)
    if number == 1 then
        return 'single_face_down_control'
    end
    return 'multi_card_fusion_face_down'
end

local function startTurn(number)
    turnNumber = number
    turnStartFrame = frames
    turnStart = snapshot()
    lastValues = turnStart
    emit(snapshotText(
        string.format('turn_start=%d(%s)', number, turnLabel(number)),
        frames,
        turnStart
    ))

    if number == 1 then
        print(SCRIPT_NAME
            .. ': turn 1 started; play one card face-down, then end')
    else
        print(SCRIPT_NAME
            .. ': turn 2 started; fuse multiple cards face-down, then end')
    end
end

local function recordChanges()
    local current = snapshot()
    if not changed(lastValues, current) then
        return
    end

    changeCount = changeCount + 1
    if changeCount <= MAX_CHANGES then
        emit(snapshotText(
            string.format('change=%02d turn=%d', changeCount, turnNumber),
            frames,
            current
        ))
    end
    lastValues = current

    if changeCount >= MAX_CHANGES then
        finish('maximum counter change count reached; partial trace follows')
    end
end

local function endTurn()
    local final = snapshot()
    local label = turnLabel(turnNumber)

    emit(snapshotText(
        string.format('turn_end=%d(%s)', turnNumber, label),
        frames,
        final
    ))
    emit(string.format(
        'turn_delta=%d(%s) turns=%d face_down=%d fusions=%d '
            .. 'equips=%d draw_cursor=%d',
        turnNumber,
        label,
        delta(turnStart.turns, final.turns),
        delta(turnStart.face_down, final.face_down),
        delta(turnStart.fusions, final.fusions),
        delta(turnStart.equips, final.equips),
        delta(turnStart.draw_cursor, final.draw_cursor)
    ))
    completedTurns = completedTurns + 1

    if turnNumber == 1 then
        phase = 2
        print(SCRIPT_NAME
            .. ': control turn captured; wait for the next player turn')
    else
        finish('captured single-card and multi-card face-down turns')
    end
end

local function poll()
    if done then
        return
    end

    frames = frames + 1

    if callbackError ~= nil then
        finish('callback error: ' .. callbackError)
        return
    end

    if mainMode() ~= DUEL_MODE then
        if phase ~= 0 then
            finish('left duel mode before both controlled turns completed')
        elseif not noDuelWarningPrinted and frames >= NO_DUEL_WARNING_FRAMES then
            noDuelWarningPrinted = true
            print(SCRIPT_NAME
                .. ': duel mode not observed; start in an ordinary duel')
        end
        if frames >= TIMEOUT_FRAMES then
            finish('timed out before the first controlled player turn')
        end
        return
    end

    if s8(OPPONENT_ID) < 0 then
        finish('two-player duel detected; use an ordinary CPU duel')
        return
    end

    local side = u8(ACTIVE_SIDE)

    if phase == 0 then
        if side == PLAYER_SIDE then
            phase = 1
            startTurn(1)
        end
    elseif phase == 1 or phase == 3 then
        recordChanges()
        if done then
            return
        end
        if side ~= PLAYER_SIDE then
            endTurn()
        elseif frames - turnStartFrame >= TURN_TIMEOUT_FRAMES then
            finish('player turn timed out before it ended')
        end
    elseif phase == 2 and side == PLAYER_SIDE then
        phase = 3
        startTurn(2)
    end

    if frames >= TIMEOUT_FRAMES then
        finish('timed out after a partial controlled trace')
    end
end

listener_duel_face_down_fusion_counter_reset =
    PCSX.Events.createEventListener(
        'ExecutionFlow::Reset',
        function()
            local ok, err = pcall(function()
                finish('reset observed; rerun after the first player hand is drawn')
            end)
            if not ok then
                callbackError = tostring(err)
            end
        end
    )

listener_duel_face_down_fusion_counter =
    PCSX.Events.createEventListener(
        'GPU::Vsync',
        function()
            local ok, err = pcall(poll)
            if not ok then
                callbackError = tostring(err)
                finish('script error: ' .. callbackError)
            end
        end
    )

print(SCRIPT_NAME .. ': armed; waiting for the first controlled player turn')
