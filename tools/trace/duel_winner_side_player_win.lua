-- duel_winner_side_player_win.lua
--
-- WHAT THIS ANSWERS
--   gDuel_bWinnerSide is structurally the winning-side index used by the
--   end-of-duel credit path. The consumed CPU-win trace recorded 1, but that
--   value was already present when the script started after a previous loss.
--
--   This control run must observe a PLAYER win after a CPU win, so a change
--   from 1 to 0 or a latched value of 0 establishes the remaining polarity.
--
-- HOW TO RUN
--   1. First lose a one-player duel so the previous result is a CPU win.
--   2. Start another one-player duel and arrange to win it.
--   3. Before the finishing action, open Debug -> Lua editor, paste this
--      file, and let it auto-run.
--   4. Wait a few seconds after the duel ends.
--   5. Copy the whole document into
--      tools/trace/result/duel_winner_side_player_win.txt and fill in the
--      context.
--
--   No breakpoint, debugger pause, or interpreter CPU is required.
--   The script accepts only a fresh low-to-high end-credit latch transition
--   after observing duel mode; a latch already set when it starts is ignored.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm that the immediately previous duel was a loss, that this duel was
--   a player win, how it ended, and that the script began before the final
--   action.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_winner_side_player_win'
local MAIN_MODE = 0x8009b26c
local DUEL_MODE = 3
local WINNER_SIDE = 0x8009b165
local DUEL_STATE = 0x8009b23a
local END_CREDIT_LATCH = 0x2000
local OPPONENT_ID = 0x8009b361
local PLAYER_LP = 0x800ea004
local OPPONENT_LP = 0x800ea024
local POST_LATCH_FRAMES = 180
local TIMEOUT_FRAMES = 72000
local MAX_SAMPLES = 12

local mem = PCSX.getMemPtr()

local function phys(addr) return addr - 0x80000000 end
local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end
local function u16(addr)
    return tonumber(ffi.cast('uint16_t*', mem + phys(addr))[0])
end
local function s8(addr)
    local value = u8(addr)
    if value >= 0x80 then
        return value - 0x100
    end
    return value
end
local function hasBit(value, mask)
    return value % (mask * 2) >= mask
end

local lines = {}
local samples = 0
local frames = 0
local enteredDuel = false
local latchSeen = false
local lastLatchSet = false
local postLatchFrames = 0
local lastWinner = nil
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function capture(reason)
    if samples >= MAX_SAMPLES then
        return
    end

    samples = samples + 1
    emit('')
    emit(string.format('--- sample %d: %s ---', samples, reason))
    emit(string.format(
        '  mode=0x%02X winner_side=%d duel_state=0x%04X',
        u8(MAIN_MODE), u8(WINNER_SIDE), u16(DUEL_STATE)
    ))
    emit(string.format(
        '  opponent_id=%d player_lp=%d opponent_lp=%d',
        s8(OPPONENT_ID), u16(PLAYER_LP), u16(OPPONENT_LP)
    ))
end

local function finish(reason)
    if done then
        return
    end
    done = true

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm the previous duel was a loss, this duel was a player win,')
    print(' the ending method, and that the script ran before the final action>')
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

    if not enteredDuel then
        if mode == DUEL_MODE then
            enteredDuel = true
            lastWinner = u8(WINNER_SIDE)
            lastLatchSet = hasBit(u16(DUEL_STATE), END_CREDIT_LATCH)
            capture('entered duel mode')
        elseif frames >= TIMEOUT_FRAMES then
            finish('timed out before duel mode was observed')
        end
        return
    end

    local winner = u8(WINNER_SIDE)
    if winner ~= lastWinner then
        lastWinner = winner
        capture('winner-side byte changed')
    end

    local latchSet = hasBit(u16(DUEL_STATE), END_CREDIT_LATCH)
    if not latchSeen and latchSet and not lastLatchSet then
        latchSeen = true
        capture('fresh end-credit latch transition')
    elseif latchSeen then
        postLatchFrames = postLatchFrames + 1
        if postLatchFrames >= POST_LATCH_FRAMES then
            capture('three seconds after end-credit latch')
            finish('captured player-win control')
        end
    elseif mode ~= DUEL_MODE then
        capture('left duel mode before end-credit latch was observed')
        finish('duel ended without observing the end-credit latch')
    elseif frames >= TIMEOUT_FRAMES then
        capture('timeout snapshot')
        finish('timed out before the end-credit latch was observed')
    end
    lastLatchSet = latchSet
end

listener_duel_winner_side_player_win = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('duel_winner_side_player_win: win after a prior loss to confirm polarity')
