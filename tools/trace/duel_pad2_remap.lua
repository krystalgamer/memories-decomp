-- duel_pad2_remap.lua
--
-- WHAT THIS ANSWERS
--   Input_BackupPad1AndUsePad2 temporarily publishes pad 2's held, pressed,
--   and repeat masks through the pad 1 globals before func_80024200 runs.
--   Input_RestorePad1FromBackup restores the original pad 1 masks afterward.
--
--   The caller enables that remap in two-player duels according to two
--   still-unnamed bytes, D_8009B1D5 and D_8009B238. This trace records both
--   selectors and both controllers' masks at the entry to each helper so the
--   active-player meaning of those bytes can be identified.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game, enable the debugger, and select the
--      interpreter CPU. Breakpoints do not fire on the dynarec.
--   2. Debug -> Lua editor, paste this file, and let it auto-run.
--   3. Start a two-player duel. On several prompts, press distinct buttons
--      on pad 1 and pad 2, including a prompt controlled by each player.
--   4. Leave the duel or wait until the script finishes.
--   5. Copy the whole document into
--      tools/trace/result/duel_pad2_remap.txt and fill in the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For each sample, state whose turn or prompt was visible, which controller
--   you used, and which buttons were held or newly pressed.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_pad2_remap'
local BACKUP_AND_REMAP_FN = 0x8003cdf8
local RESTORE_FN = 0x8003ce48
local MAIN_MODE = 0x8009b26c
local DUEL_MODE = 3
local OPPONENT_ID = 0x8009b361
local SIDE_DEFAULT = 0x8009b1d5
local SIDE_OVERRIDE = 0x8009b238
local PAD1_REPEAT = 0x8009b394
local PAD2_REPEAT = 0x8009b396
local PAD1_PRESSED = 0x8009b398
local PAD2_PRESSED = 0x8009b39a
local PAD1_REPEAT_BACKUP = 0x8009b39e
local PAD1_PRESSED_BACKUP = 0x8009b3a0
local PAD1_HELD = 0x8009b3a4
local PAD2_HELD = 0x8009b3a6
local PAD1_HELD_BACKUP = 0x8009b3ac
local MIN_SAMPLES = 6
local MAX_SAMPLES = 24
local SETTLE_FRAMES = 1200
local TIMEOUT_FRAMES = 72000

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

local lines = {}
local samples = 0
local frames = 0
local quiet = 0
local enteredDuel = false
local sawRemap = false
local done = false
local lastKeys = {}

local function emit(text)
    lines[#lines + 1] = text
end

local function stateKey(stage)
    return string.format(
        '%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d',
        stage,
        u8(SIDE_DEFAULT),
        s8(SIDE_OVERRIDE),
        u16(PAD1_HELD),
        u16(PAD2_HELD),
        u16(PAD1_PRESSED),
        u16(PAD2_PRESSED),
        u16(PAD1_REPEAT),
        u16(PAD2_REPEAT),
        u16(PAD1_HELD_BACKUP),
        u16(PAD1_PRESSED_BACKUP)
    ) .. ',' .. u16(PAD1_REPEAT_BACKUP)
end

local function capture(stage)
    local key = stateKey(stage)
    if lastKeys[stage] == key or samples >= MAX_SAMPLES then
        return
    end

    lastKeys[stage] = key
    samples = samples + 1
    quiet = 0
    sawRemap = true

    emit('')
    emit(string.format('--- sample %d of %d: %s entry ---',
                       samples, MAX_SAMPLES, stage))
    emit(string.format(
        '  mode=0x%02X opponent_id=%d side_default=%d side_override=%d',
        u8(MAIN_MODE), s8(OPPONENT_ID),
        u8(SIDE_DEFAULT), s8(SIDE_OVERRIDE)
    ))
    emit(string.format(
        '  pad1: held=0x%04X pressed=0x%04X repeat=0x%04X',
        u16(PAD1_HELD), u16(PAD1_PRESSED), u16(PAD1_REPEAT)
    ))
    emit(string.format(
        '  pad2: held=0x%04X pressed=0x%04X repeat=0x%04X',
        u16(PAD2_HELD), u16(PAD2_PRESSED), u16(PAD2_REPEAT)
    ))
    emit(string.format(
        '  backup: held=0x%04X pressed=0x%04X repeat=0x%04X',
        u16(PAD1_HELD_BACKUP),
        u16(PAD1_PRESSED_BACKUP),
        u16(PAD1_REPEAT_BACKUP)
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
    print(' controller you used, and which buttons were held or pressed>')
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

local function breakpointCallback(stage)
    return function()
        if done then
            return false
        end

        local ok, err = pcall(function()
            if u8(MAIN_MODE) % 32 == DUEL_MODE and s8(OPPONENT_ID) < 0 then
                capture(stage)
                if samples >= MAX_SAMPLES then
                    finish('maximum sample count reached')
                end
            end
        end)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
        if done then
            return false
        end
    end
end

breakpoint_duel_pad2_remap = PCSX.addBreakpoint(
    BACKUP_AND_REMAP_FN, 'Exec', 4, 'pad 1 backup and pad 2 remap',
    breakpointCallback('backup/remap')
)

breakpoint_duel_pad2_restore = PCSX.addBreakpoint(
    RESTORE_FN, 'Exec', 4, 'pad 1 restore',
    breakpointCallback('restore')
)

listener_duel_pad2_remap = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        if done then
            return
        end

        local ok, err = pcall(function()
            frames = frames + 1
            local mode = u8(MAIN_MODE) % 32
            if mode == DUEL_MODE then
                enteredDuel = true
            elseif enteredDuel and sawRemap then
                finish('left duel mode after observing the pad 2 remap')
                return
            end

            if sawRemap then
                quiet = quiet + 1
                if samples >= MIN_SAMPLES and quiet >= SETTLE_FRAMES then
                    finish('no new input state for twenty seconds')
                end
            elseif frames >= TIMEOUT_FRAMES then
                finish('timed out before a two-player pad remap was observed')
            end
        end)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('duel_pad2_remap: armed; start a two-player duel and use both pads')
