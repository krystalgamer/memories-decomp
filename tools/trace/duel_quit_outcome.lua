-- duel_quit_outcome.lua
--
-- WHAT THIS ANSWERS
--   The clean-room game description leaves the QUIT DUEL result unresolved.
--   Free Duel uses D_8009B362 after a duel to choose which half of the
--   selected duelist's W/L record to increment. This captures that byte and
--   the record before and after the player confirms QUIT DUEL? YES.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game.
--   2. Debug -> Lua editor, paste this file, and let it auto-run.
--   3. Start a one-player Free Duel and run the script before pressing START.
--   4. Press START, choose YES on QUIT DUEL?, and wait until the Free Duel
--      opponent-select screen returns.
--   5. Copy the whole document into
--      tools/trace/result/duel_quit_outcome.txt and fill in the context.
--
--   No breakpoint, debugger pause, or interpreter CPU is required.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Name the opponent, record the displayed W/L values before the duel,
--   confirm that YES was selected, note every screen shown while leaving,
--   and record the displayed W/L values after returning to Free Duel.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_quit_outcome'
local MAIN_MODE = 0x8009b26c
local DUEL_MODE = 3
local FREE_DUEL_MODE = 6
local DUEL_STATE = 0x8009b23a
local DUEL_OUTCOME = 0x8009b362
local OPPONENT_ID = 0x8009b361
local RETURN_FLAGS = 0x8009b365
local PLAYER_LP = 0x800ea004
local OPPONENT_LP = 0x800ea024
local PAD1_PRESSED = 0x8009b398
local DUELIST_RECORDS = 0x801d071c
local FIRST_DUELIST_ID = 1
local LAST_DUELIST_ID = 39
local RETURN_SETTLE_FRAMES = 180
local TIMEOUT_FRAMES = 72000
local MAX_SAMPLES = 24

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

local lines = {}
local samples = 0
local frames = 0
local quiet = 0
local enteredDuel = false
local returnedToFreeDuel = false
local opponent = nil
local recordAddress = nil
local initialWins = nil
local initialLosses = nil
local lastKey = nil
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function modeName(mode)
    if mode == DUEL_MODE then
        return 'Duel'
    end
    if mode == FREE_DUEL_MODE then
        return 'Free Duel'
    end
    return 'other'
end

local function recordValues()
    if recordAddress == nil then
        return nil, nil
    end
    return u16(recordAddress), u16(recordAddress + 2)
end

local function capture(reason)
    if samples >= MAX_SAMPLES then
        return
    end

    samples = samples + 1
    local mode = u8(MAIN_MODE) % 32
    local wins, losses = recordValues()

    emit('')
    emit(string.format(
        '--- sample %d of %d: %s ---',
        samples, MAX_SAMPLES, reason
    ))
    emit(string.format(
        '  screen=%s mode=0x%02X opponent_id=%d',
        modeName(mode), u8(MAIN_MODE), s8(OPPONENT_ID)
    ))
    emit(string.format(
        '  duel_outcome=%d return_flags=0x%02X duel_state=0x%04X',
        u8(DUEL_OUTCOME), u8(RETURN_FLAGS), u16(DUEL_STATE)
    ))
    emit(string.format(
        '  player_lp=%d opponent_lp=%d pad1_pressed=0x%04X',
        u16(PLAYER_LP), u16(OPPONENT_LP), u16(PAD1_PRESSED)
    ))

    if wins ~= nil then
        emit(string.format(
            '  duelist_record=0x%08X wins=%d losses=%d delta_wins=%d delta_losses=%d',
            recordAddress,
            wins,
            losses,
            wins - initialWins,
            losses - initialLosses
        ))
    else
        emit('  duelist_record=<opponent id is outside Free Duel slots 1-39>')
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
    print('<name the opponent; give W/L before and after; confirm QUIT DUEL?')
    print(' YES; list the screens shown while returning to Free Duel>')
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

local function key(mode)
    local wins, losses = recordValues()
    return string.format(
        '%d,%d,%d,%d,%d,%d',
        mode,
        s8(OPPONENT_ID),
        u8(DUEL_OUTCOME),
        u8(RETURN_FLAGS),
        wins or -1,
        losses or -1
    )
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
            opponent = s8(OPPONENT_ID)
            if opponent >= FIRST_DUELIST_ID and opponent <= LAST_DUELIST_ID then
                recordAddress = DUELIST_RECORDS + opponent * 4
                initialWins, initialLosses = recordValues()
            end
            lastKey = key(mode)
            capture('entered duel; baseline record captured')
        elseif frames >= TIMEOUT_FRAMES then
            finish('timed out before a one-player duel was observed')
        end
        return
    end

    local currentKey = key(mode)
    if currentKey ~= lastKey then
        lastKey = currentKey
        quiet = 0
        capture('outcome, return state, or selected record changed')
        if samples >= MAX_SAMPLES then
            finish('maximum sample count reached')
        end
        return
    end

    if mode == FREE_DUEL_MODE then
        returnedToFreeDuel = true
    end

    quiet = quiet + 1
    if returnedToFreeDuel and quiet >= RETURN_SETTLE_FRAMES then
        capture('three seconds after returning to Free Duel')
        finish('captured quit-duel return and final record')
    elseif frames >= TIMEOUT_FRAMES then
        capture('timeout snapshot')
        finish('timed out before the Free Duel screen returned')
    end
end

listener_duel_quit_outcome = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('duel_quit_outcome: start a Free Duel, then choose QUIT DUEL? YES')
