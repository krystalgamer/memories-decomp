-- duel_winner_side.lua
--
-- WHAT THIS ANSWERS
--   Static code uses D_8009B165 as a side index while awarding end-of-duel
--   credit. A player win has shown value 0, but value 1 has not been observed
--   during a CPU win. This captures the byte when the one-shot end-credit
--   latch is set and includes the life points and selected duelist's W/L
--   record so the result can support or reject gDuel_bWinnerSide.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game.
--   2. Debug -> Lua editor, paste this file, and let it auto-run.
--   3. Enter a one-player duel and start the script before the final action.
--   4. For the missing evidence, lose the duel and wait a few seconds after
--      the duel ends. A player win is also useful as a control run.
--   5. Copy the whole document into
--      tools/trace/result/duel_winner_side.txt and fill in the context.
--
--   No breakpoint, debugger pause, or interpreter CPU is required.
--
-- WHAT TO WRITE IN THE CONTEXT
--   State whether the player won or lost, how the duel ended (LP, Exodia,
--   deck-out, or surrender), which opponent was selected, and whether the
--   script was started before the final action.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_winner_side'
local MAIN_MODE = 0x8009b26c
local DUEL_MODE = 3
local WINNER_SIDE = 0x8009b165
local DUEL_STATE = 0x8009b23a
local END_CREDIT_LATCH = 0x2000
local OPPONENT_ID = 0x8009b361
local PLAYER_LP = 0x800ea004
local OPPONENT_LP = 0x800ea024
local DUELIST_RECORDS = 0x801d071c
local DUELIST_COUNT = 39
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
    local opponent = s8(OPPONENT_ID)
    emit('')
    emit(string.format('--- sample %d: %s ---', samples, reason))
    emit(string.format(
        '  mode=0x%02X winner_side=%d duel_state=0x%04X',
        u8(MAIN_MODE), u8(WINNER_SIDE), u16(DUEL_STATE)
    ))
    emit(string.format(
        '  opponent_id=%d player_lp=%d opponent_lp=%d',
        opponent, u16(PLAYER_LP), u16(OPPONENT_LP)
    ))

    if opponent >= 0 and opponent < DUELIST_COUNT then
        local record = DUELIST_RECORDS + opponent * 4
        emit(string.format(
            '  duelist_record=0x%08X wins=%d losses=%d',
            record, u16(record), u16(record + 2)
        ))
    else
        emit('  duelist_record=<not applicable for this opponent id>')
    end
end

local function finish(reason)
    if done then
        return
    end
    done = true

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<state win/loss, ending method, opponent, and whether the script')
    print(' was running before the final action>')
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

    if not latchSeen and hasBit(u16(DUEL_STATE), END_CREDIT_LATCH) then
        latchSeen = true
        capture('end-credit latch set')
    elseif latchSeen then
        postLatchFrames = postLatchFrames + 1
        if postLatchFrames >= POST_LATCH_FRAMES then
            capture('three seconds after end-credit latch')
            finish('captured end-credit state')
        end
    elseif mode ~= DUEL_MODE then
        capture('left duel mode before end-credit latch was observed')
        finish('duel ended without observing the end-credit latch')
    elseif frames >= TIMEOUT_FRAMES then
        capture('timeout snapshot')
        finish('timed out before the end-credit latch was observed')
    end
end

listener_duel_winner_side = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('duel_winner_side: waiting for duel mode; finish a one-player duel')
