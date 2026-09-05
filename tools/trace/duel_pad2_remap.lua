-- duel_pad2_remap.lua
--
-- WHAT THIS ANSWERS
--   func_80024388 chooses whether duel updates should temporarily expose pad
--   2 through the pad 1 input globals. Static code derives that decision from
--   D_8009B1D5 and, when nonnegative, D_8009B238.
--
--   The first trace used execution breakpoints on the remap helpers, but a
--   complete two-player run produced no hits. This version samples the two
--   selector bytes and both controllers' published masks every vertical blank.
--   It therefore works on either CPU core and can establish which selector
--   follows the player whose turn or prompt is currently active.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game.
--   2. Debug -> Lua editor, paste this file, and let it auto-run.
--   3. Start a two-player duel.
--   4. On several prompts during each player's turn, press distinct buttons
--      on pad 1 and pad 2. Include the result screen if possible.
--   5. Copy the whole document into
--      tools/trace/result/duel_pad2_remap.txt and fill in the context.
--
--   No breakpoint, debugger pause, or interpreter CPU is required.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For each sample, state whose turn or prompt was visible, which controller
--   was accepted by the game, and which buttons were held or newly pressed.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_pad2_remap'
local MAIN_MODE = 0x8009b26c
local DUEL_MODE = 3
local OPPONENT_ID = 0x8009b361
local PLAYING_SIDE = 0x8009b1d5
local INPUT_SIDE_OVERRIDE = 0x8009b238
local SAVE_POINTER_1 = 0x8009b1d8
local SAVE_POINTER_2 = 0x8009b1dc
local DUEL_STATE = 0x8009b23a
local PAD1_REPEAT = 0x8009b394
local PAD2_REPEAT = 0x8009b396
local PAD1_PRESSED = 0x8009b398
local PAD2_PRESSED = 0x8009b39a
local PAD1_HELD = 0x8009b3a4
local PAD2_HELD = 0x8009b3a6
local MIN_SAMPLES = 12
local MAX_SAMPLES = 64
local SETTLE_FRAMES = 1800
local TIMEOUT_FRAMES = 216000

local mem = PCSX.getMemPtr()

local function phys(addr) return addr - 0x80000000 end
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
local function u32(addr)
    return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0])
end
local function isRam(addr)
    return addr >= 0x80000000 and addr < 0x80200000
end

local lines = {}
local samples = 0
local frames = 0
local quiet = 0
local enteredTwoPlayerDuel = false
local seenPlayingSides = {}
local lastKey = nil
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function twoPlayerDuelActive()
    return u8(MAIN_MODE) % 32 == DUEL_MODE
        and isRam(u32(SAVE_POINTER_1))
        and isRam(u32(SAVE_POINTER_2))
end

local function selectedInputSide()
    if s8(OPPONENT_ID) >= 0 then
        return 0
    end

    local side = u8(PLAYING_SIDE)
    local override = s8(INPUT_SIDE_OVERRIDE)
    if override >= 0 then
        side = override
    end
    return side
end

local function key()
    return string.format(
        '%d,%d,%d,%d,%d,%d,%d,%d,%d,%d',
        u8(PLAYING_SIDE),
        s8(INPUT_SIDE_OVERRIDE),
        u16(DUEL_STATE),
        u16(PAD1_HELD),
        u16(PAD2_HELD),
        u16(PAD1_PRESSED),
        u16(PAD2_PRESSED),
        u16(PAD1_REPEAT),
        u16(PAD2_REPEAT),
        selectedInputSide()
    )
end

local function capture(reason)
    samples = samples + 1
    quiet = 0

    local playingSide = u8(PLAYING_SIDE)
    local selectedSide = selectedInputSide()
    seenPlayingSides[playingSide] = true

    emit('')
    emit(string.format('--- sample %d of %d: %s ---',
                       samples, MAX_SAMPLES, reason))
    emit(string.format(
        '  mode=0x%02X opponent_id=%d duel_state=0x%04X',
        u8(MAIN_MODE), s8(OPPONENT_ID), u16(DUEL_STATE)
    ))
    emit(string.format(
        '  playing_side=%d input_side_override=%d selected_input_side=%d',
        playingSide, s8(INPUT_SIDE_OVERRIDE), selectedSide
    ))
    emit(string.format(
        '  pad1: held=0x%04X pressed=0x%04X repeat=0x%04X',
        u16(PAD1_HELD), u16(PAD1_PRESSED), u16(PAD1_REPEAT)
    ))
    emit(string.format(
        '  pad2: held=0x%04X pressed=0x%04X repeat=0x%04X',
        u16(PAD2_HELD), u16(PAD2_PRESSED), u16(PAD2_REPEAT)
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
    print('<for each sample, state whose turn or prompt was visible, which')
    print(' controller was accepted, and which buttons were held or pressed>')
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
    if not twoPlayerDuelActive() then
        if enteredTwoPlayerDuel then
            finish('left the two-player duel')
        elseif frames >= TIMEOUT_FRAMES then
            finish('timed out before a two-player duel was observed')
        end
        return
    end

    enteredTwoPlayerDuel = true
    local currentKey = key()
    if currentKey ~= lastKey then
        local reason = lastKey == nil and 'entered two-player duel'
            or 'selector or input mask changed'
        lastKey = currentKey
        capture(reason)
        if samples >= MAX_SAMPLES then
            finish('maximum sample count reached')
        end
        return
    end

    quiet = quiet + 1
    if samples >= MIN_SAMPLES
        and seenPlayingSides[0]
        and seenPlayingSides[1]
        and quiet >= SETTLE_FRAMES then
        finish('observed both playing-side values and then settled')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out; partial two-player input data follows')
    end
end

listener_duel_pad2_remap = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('duel_pad2_remap: waiting for a two-player duel; use both pads')
