-- free_duel_unlock_tail.lua
--
-- WHAT THIS ANSWERS
--   The published "all Free Duel opponents" data cheat writes only
--   0x801D06F4-0x801D06F7. Static flag arithmetic places duelists 32-38 in
--   the next byte at 0x801D06F8, yet community lists describe the four-byte
--   write as unlocking everyone.
--
--   This trace compares all five unlock bytes with the Free Duel overlay's
--   40-byte availability grid and records visits to the last eight cells.
--   Human context establishes which portraits were visible and selectable.
--
-- HOW TO RUN
--   1. Load a save that has the secondary menu but is still missing at least
--      one campaign opponent with ID 32-38 in Free Duel.
--   2. Open PCSX-Redux's Lua editor, paste this file, and let it auto-run
--      before enabling any unlock cheat.
--   3. Enable only the published data cheat:
--        801D06F4 FFFF
--        801D06F6 FFFF
--   4. Enter Free Duel and move the cursor through the final two grid rows.
--      Note which portraits are present and whether IDs 32-38 can be selected.
--   5. Wait for the trace to finish, or leave the screen after checking.
--   6. Copy the whole document into
--      tools/trace/result/free_duel_unlock_tail.txt and fill in the context.
--
--   No breakpoint, debugger pause, or interpreter CPU is required.
--   The script verifies the Free Duel module ID and two function prologues
--   before reading the shared overlay data range.
--
-- WHAT TO WRITE IN THE CONTEXT
--   State which high-ID duelists were locked before the test, confirm the two
--   cheat lines above were the only unlock writes, list the visible/selectable
--   opponents in cells 32-38, and say whether Duel Master K remained present.

local ffi = require('ffi')

local SCRIPT_NAME = 'free_duel_unlock_tail'
local MAIN_MODE = 0x8009b26c
local FREE_DUEL_MODE = 6
local STORY_FLAGS = 0x801d0618
local UNLOCK_BYTES = 0x801d06f4
local GRID_AVAILABLE = 0x80169030
local SCREEN_FLAGS = 0x801690a4
local CURSOR_COLUMN = 0x8009b366
local CURSOR_ROW = 0x8009b367
local TARGET_COLUMN = 0x8009b36c
local TARGET_ROW = 0x8009b36d
local MODULE_BASE = 0x80168000
local UPDATE_SCREEN = 0x80168c7c
local MODULE_ENTRY = 0x80168fb4
local EXPECTED_MODULE_ID = 0x00000013
local EXPECTED_UPDATE_WORD = 0x27bdffd8
local EXPECTED_ENTRY_WORD = 0x27bdffe8
local GRID_SIZE = 40
local SETTLE_FRAMES = 120
local OBSERVE_FRAMES = 1200
local TIMEOUT_FRAMES = 72000
local MAX_SNAPSHOTS = 8
local MAX_CURSOR_SAMPLES = 16

local mem = PCSX.getMemPtr()

local function phys(addr)
    return addr - 0x80000000
end

local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end

local function u32(addr)
    return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0])
end

local function moduleSignature()
    return string.format(
        'id=0x%08X update=0x%08X entry=0x%08X',
        u32(MODULE_BASE), u32(UPDATE_SCREEN), u32(MODULE_ENTRY)
    )
end

local function isFreeDuelModule()
    return u32(MODULE_BASE) == EXPECTED_MODULE_ID
        and u32(UPDATE_SCREEN) == EXPECTED_UPDATE_WORD
        and u32(MODULE_ENTRY) == EXPECTED_ENTRY_WORD
end

local function storyFlag(id)
    local byte = u8(STORY_FLAGS + math.floor(id / 8))
    local mask = math.floor(0x80 / (2 ^ (id % 8)))
    return math.floor(byte / mask) % 2
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function unlockKey()
    local values = {}
    for i = 0, 4 do
        values[#values + 1] = string.format('%02X', u8(UNLOCK_BYTES + i))
    end
    return table.concat(values, ' ')
end

local function gridKey()
    local values = {}
    for i = 0, GRID_SIZE - 1 do
        values[#values + 1] = string.format('%02X', u8(GRID_AVAILABLE + i))
    end
    return table.concat(values)
end

local lines = {}
local frames = 0
local freeDuelFrames = 0
local snapshots = 0
local cursorSamples = 0
local enteredFreeDuel = false
local snapshotTaken = false
local lastUnlockKey = unlockKey()
local lastSnapshotKey = nil
local lastCursor = nil
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function capture(reason)
    if snapshots >= MAX_SNAPSHOTS then
        return
    end

    snapshots = snapshots + 1
    lastSnapshotKey = unlockKey() .. ':' .. gridKey()

    emit('')
    emit(string.format('--- snapshot %d: %s ---', snapshots, reason))
    emit(string.format(
        '  frame=%d mode=%d screen_flags=0x%02X',
        frames, mainMode(), u8(SCREEN_FLAGS)
    ))
    emit('  module_signature ' .. moduleSignature())
    emit('  unlock_bytes_F4_F8=' .. unlockKey())

    for row = 0, 7 do
        local values = {}
        for column = 0, 4 do
            local index = row * 5 + column
            values[#values + 1] = tostring(u8(GRID_AVAILABLE + index))
        end
        emit(string.format(
            '  grid_row_%d_ids_%02d_%02d=%s',
            row, row * 5, row * 5 + 4, table.concat(values, ' ')
        ))
    end

    local mismatches = {}
    for id = 1, 38 do
        local flag = storyFlag(0x6e0 + id)
        local available = u8(GRID_AVAILABLE + id)
        if available ~= flag then
            mismatches[#mismatches + 1] = string.format(
                '%d(flag=%d grid=%d)', id, flag, available
            )
        end
    end
    if #mismatches == 0 then
        emit('  flag_grid_mismatches=none')
    else
        emit('  flag_grid_mismatches=' .. table.concat(mismatches, ','))
    end

    for id = 32, 38 do
        emit(string.format(
            '  tail_id=%d cell=(%d,%d) flag=%d grid=%d',
            id, id % 5, math.floor(id / 5),
            storyFlag(0x6e0 + id), u8(GRID_AVAILABLE + id)
        ))
    end
    emit(string.format(
        '  tail_id=39 cell=(4,7) flag=not-tested grid=%d',
        u8(GRID_AVAILABLE + 39)
    ))
end

local function captureCursor()
    if cursorSamples >= MAX_CURSOR_SAMPLES then
        return
    end

    local column = u8(CURSOR_COLUMN)
    local row = u8(CURSOR_ROW)
    local index = row * 5 + column
    local key = string.format('%d,%d', column, row)

    if key == lastCursor or index < 32 or index >= GRID_SIZE then
        return
    end

    lastCursor = key
    cursorSamples = cursorSamples + 1
    emit(string.format(
        'cursor_sample=%d frame=%d id=%d cell=(%d,%d) grid=%d '
        .. 'target=(%d,%d)',
        cursorSamples, frames, index, column, row,
        u8(GRID_AVAILABLE + index), u8(TARGET_COLUMN), u8(TARGET_ROW)
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
    print('<state which IDs 32-38 were locked before the test; confirm only')
    print(' 801D06F4 FFFF / 801D06F6 FFFF were enabled; list which final-row')
    print(' portraits were visible/selectable; say whether Duel Master K appeared>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: snapshots=%d cursor_samples=%d',
        snapshots, cursorSamples
    ))
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

    local currentUnlockKey = unlockKey()
    if currentUnlockKey ~= lastUnlockKey then
        emit(string.format(
            'unlock_change frame=%d old=%s new=%s',
            frames, lastUnlockKey, currentUnlockKey
        ))
        lastUnlockKey = currentUnlockKey
    end

    if mainMode() ~= FREE_DUEL_MODE then
        if enteredFreeDuel and snapshotTaken then
            finish('left Free Duel after capturing the availability grid')
        elseif frames >= TIMEOUT_FRAMES then
            finish('timed out before Free Duel mode was observed')
        end
        return
    end

    if not enteredFreeDuel then
        enteredFreeDuel = true
        freeDuelFrames = 0
        emit(string.format(
            'entered_free_duel frame=%d unlock_bytes_F4_F8=%s',
            frames, currentUnlockKey
        ))
    end

    freeDuelFrames = freeDuelFrames + 1
    if freeDuelFrames < SETTLE_FRAMES then
        return
    end

    if not isFreeDuelModule() then
        emit('module_signature_mismatch ' .. moduleSignature())
        finish('Free Duel module signature mismatch after settle')
        return
    end

    if not snapshotTaken then
        capture('overlay settled after entering Free Duel')
        snapshotTaken = true
    else
        local key = currentUnlockKey .. ':' .. gridKey()
        if key ~= lastSnapshotKey then
            capture('unlock bytes or availability grid changed')
        end
    end

    captureCursor()

    if snapshots >= MAX_SNAPSHOTS then
        finish('maximum snapshot count reached')
    elseif freeDuelFrames >= SETTLE_FRAMES + OBSERVE_FRAMES then
        finish('observation window completed')
    end
end

emit('baseline_unlock_bytes_F4_F8=' .. lastUnlockKey)

listener_free_duel_unlock_tail = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print('free_duel_unlock_tail: baseline captured; enable only the two data codes')
