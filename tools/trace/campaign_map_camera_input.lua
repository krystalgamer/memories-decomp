-- campaign_map_camera_input.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md documents the exact static input contract of
--   CampaignMap_MoveCameraDpad at 0x80168388, but leaves its live dispatch
--   unproved. CampaignMap_UpdateLocation does not call it, and static scans
--   found no direct call or stored pointer in the resident image or verified
--   overlays.
--
--   This trace breaks on both CampaignMap_MoveCameraDpad and the known active
--   map tick CampaignMap_UpdateLocation. A hit on the first proves the camera
--   helper is live; repeated hits on only the second provide controlled
--   negative evidence that the debugger and map tick were active while the
--   helper was not observed. Camera snapshots cover the five fields the
--   helper can change, but do not attribute a change unless its breakpoint
--   also fired.
--
-- HOW TO RUN
--   1. Enter the visible campaign location map, then open the PCSX-Redux
--      debugger and select the interpreter CPU.
--   2. Paste this script into Debug -> Lua editor and confirm that it reports
--      two installed breakpoints.
--   3. Try isolated holds of Left, Right, Up, Down, L1 and R1. Also try a
--      direction with L2 or R2, and a direction while holding Cross. Release
--      each combination before trying the next.
--   4. Stop when the trace prints its result, or wait about thirty seconds for
--      the bounded negative result.
--   5. Copy the whole document into
--      tools/trace/result/campaign_map_camera_input.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Name the visible map/location and campaign progress. List every button
--   combination tried and whether the view visibly moved, rotated or zoomed.
--   Confirm that the interpreter CPU was selected and note any cheat, patch
--   or debug option that could alter map input.

local bit = require('bit')
local ffi = require('ffi')

local SCRIPT_NAME = 'campaign_map_camera_input'
local MOVE_CAMERA = 0x80168388
local MOVE_CAMERA_SIGNATURE = 0x27bdffe8
local UPDATE_LOCATION = 0x80168fcc
local UPDATE_LOCATION_SIGNATURE = 0x27bdffe0
local MAIN_MODE = 0x8009b26c
local PAD1_HELD = 0x8009b3a4
local CAMERA = 0x800f2848
local CAMPAIGN_MAP_MODE = 5
local INPUT_MASK = 0xf04f
local MAX_INPUT_SAMPLES = 16
local MAX_MOVE_ROWS = 16
local MAX_MOVE_HITS = 120
local NO_HIT_WARNING_FRAMES = 300
local POST_HIT_FRAMES = 60
local TIMEOUT_FRAMES = 1800

local mem = PCSX.getMemPtr()

local function phys(addr)
    return addr - 0x80000000
end

local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end

local function u16(addr)
    return tonumber(ffi.cast('uint16_t*', mem + phys(addr))[0])
end

local function u32(addr)
    return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0])
end

local function s16(addr)
    local value = u16(addr)
    if value >= 0x8000 then
        return value - 0x10000
    end
    return value
end

local function s32(addr)
    local value = u32(addr)
    if value >= 0x80000000 then
        return value - 0x100000000
    end
    return value
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function signaturesMatch()
    return u32(MOVE_CAMERA) == MOVE_CAMERA_SIGNATURE
        and u32(UPDATE_LOCATION) == UPDATE_LOCATION_SIGNATURE
end

local function cameraSnapshot()
    return {
        mode = mainMode(),
        held = u16(PAD1_HELD),
        field00 = s16(CAMERA),
        angle = s16(CAMERA + 0x02),
        field04 = s16(CAMERA + 0x04),
        vrx = s32(CAMERA + 0x1c),
        vrz = s32(CAMERA + 0x24),
    }
end

local function snapshotText(prefix, frame, value)
    return string.format(
        '%s frame=%06d mode=%d held=0x%04X '
            .. 'field_00=%d angle=%d field_04=%d vrx=%d vrz=%d',
        prefix,
        frame,
        value.mode,
        value.held,
        value.field00,
        value.angle,
        value.field04,
        value.vrx,
        value.vrz
    )
end

local function deltaText(prefix, frame, before, after, reason)
    return string.format(
        '%s frame=%06d reason=%s field_00=%d angle=%d field_04=%d '
            .. 'vrx=%d vrz=%d',
        prefix,
        frame,
        reason,
        after.field00 - before.field00,
        after.angle - before.angle,
        after.field04 - before.field04,
        after.vrx - before.vrx,
        after.vrz - before.vrz
    )
end

local lines = {}
local frames = 0
local updateHits = 0
local moveHits = 0
local moveRows = 0
local inputSamples = 0
local firstMoveFrame = nil
local activeInput = nil
local pendingMoves = {}
local seenMoveMasks = {}
local callbackError = nil
local warningPrinted = false
local done = false

local function emit(text)
    lines[#lines + 1] = text
end

local function closeInput(reason)
    if activeInput == nil then
        return
    end

    local after = cameraSnapshot()
    emit(snapshotText(
        string.format('input_post=%02d', activeInput.number),
        frames,
        after
    ))
    emit(deltaText(
        string.format('input_delta=%02d', activeInput.number),
        frames,
        activeInput.before,
        after,
        reason
    ))
    activeInput = nil
end

local function disableBreakpoints()
    if breakpoint_campaign_map_camera_move ~= nil then
        breakpoint_campaign_map_camera_move:disable()
    end
    if breakpoint_campaign_map_camera_update ~= nil then
        breakpoint_campaign_map_camera_update:disable()
    end
end

local function finish(reason)
    if done then
        return
    end
    done = true
    closeInput('trace_end')
    disableBreakpoints()

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<name the visible map/location and campaign progress; list every')
    print(' button combination tried and whether the view visibly moved,')
    print(' rotated or zoomed; confirm interpreter CPU and list any patches>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: frames=%d move_hits=%d active_tick_hits=%d '
            .. 'input_samples=%d move_rows=%d routine_observed=%s '
            .. 'active_tick_observed=%s',
        frames,
        moveHits,
        updateHits,
        inputSamples,
        moveRows,
        tostring(moveHits > 0),
        tostring(updateHits > 0)
    ))
    print(string.format(
        'overlay_signatures: move=0x%08X expected=0x%08X '
            .. 'active_tick=0x%08X expected=0x%08X',
        u32(MOVE_CAMERA),
        MOVE_CAMERA_SIGNATURE,
        u32(UPDATE_LOCATION),
        UPDATE_LOCATION_SIGNATURE
    ))
    if callbackError ~= nil then
        print('callback_error: ' .. callbackError)
    end
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function startInput(mask)
    if inputSamples >= MAX_INPUT_SAMPLES then
        return
    end

    inputSamples = inputSamples + 1
    activeInput = {
        number = inputSamples,
        mask = mask,
        before = cameraSnapshot(),
    }
    emit(snapshotText(
        string.format('input_start=%02d', inputSamples),
        frames,
        activeInput.before
    ))
    print(string.format(
        '%s: input sample %d started with held mask 0x%04X',
        SCRIPT_NAME,
        inputSamples,
        mask
    ))
end

local function pollInput()
    local mask = bit.band(u16(PAD1_HELD), INPUT_MASK)

    if activeInput ~= nil and mask ~= activeInput.mask then
        if mask == 0 then
            closeInput('released')
        else
            closeInput('held_mask_changed')
        end
    end
    if mask ~= 0 and activeInput == nil then
        startInput(mask)
    end
end

local function flushPendingMoves()
    local keep = {}
    for _, entry in ipairs(pendingMoves) do
        if frames > entry.frame then
            local after = cameraSnapshot()
            emit(snapshotText(
                string.format('move_after=%02d', entry.number),
                frames,
                after
            ))
            emit(deltaText(
                string.format('move_delta=%02d', entry.number),
                frames,
                entry.before,
                after,
                'next_vsync'
            ))
        else
            keep[#keep + 1] = entry
        end
    end
    pendingMoves = keep
end

local function onMoveCamera()
    if done or callbackError ~= nil then
        return
    end

    moveHits = moveHits + 1
    if firstMoveFrame == nil then
        firstMoveFrame = frames
        print(SCRIPT_NAME
              .. ': CampaignMap_MoveCameraDpad executed; keep the current '
              .. 'input held briefly')
    end

    local held = u16(PAD1_HELD)
    if not seenMoveMasks[held] and moveRows < MAX_MOVE_ROWS then
        seenMoveMasks[held] = true
        moveRows = moveRows + 1
        local before = cameraSnapshot()
        emit(snapshotText(
            string.format('move_entry=%02d', moveRows),
            frames,
            before
        ))
        pendingMoves[#pendingMoves + 1] = {
            number = moveRows,
            frame = frames,
            before = before,
        }
    end

    if moveHits >= MAX_MOVE_HITS then
        finish('maximum free-camera hit count reached')
    end
end

local function onUpdateLocation()
    if done or callbackError ~= nil then
        return
    end

    updateHits = updateHits + 1
    if updateHits == 1 then
        emit(snapshotText('active_tick_first', frames, cameraSnapshot()))
        print(SCRIPT_NAME
              .. ': active CampaignMap_UpdateLocation control observed')
    end
end

local function poll()
    if done then
        return
    end

    frames = frames + 1

    if callbackError ~= nil then
        finish('breakpoint callback error: ' .. callbackError)
        return
    end
    if mainMode() ~= CAMPAIGN_MAP_MODE then
        finish('left Campaign Map mode during capture')
        return
    end
    if not signaturesMatch() then
        finish('overworld overlay signature changed during capture')
        return
    end

    pollInput()
    flushPendingMoves()

    if moveHits == 0
        and not warningPrinted
        and frames >= NO_HIT_WARNING_FRAMES then
        warningPrinted = true
        print(SCRIPT_NAME
              .. ': no free-camera hit yet; try the documented controls '
              .. 'and confirm interpreter CPU')
    end

    if firstMoveFrame ~= nil
        and frames - firstMoveFrame >= POST_HIT_FRAMES then
        finish('CampaignMap_MoveCameraDpad executed; follow-up captured')
        return
    end

    if frames >= TIMEOUT_FRAMES then
        if updateHits > 0 then
            finish('active map tick observed but free-camera routine was '
                   .. 'not observed during capture')
        else
            finish('no active-tick hit; select interpreter CPU and rerun '
                   .. 'on the visible campaign map')
        end
    end
end

listener_campaign_map_camera_input_reset =
    PCSX.Events.createEventListener(
        'ExecutionFlow::Reset',
        function()
            local ok, err = pcall(function()
                finish('reset observed; rerun from the visible campaign map')
            end)
            if not ok then
                callbackError = tostring(err)
            end
        end
    )

listener_campaign_map_camera_input =
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

if mainMode() ~= CAMPAIGN_MAP_MODE then
    finish(string.format(
        'main mode %d is not Campaign Map mode %d',
        mainMode(),
        CAMPAIGN_MAP_MODE
    ))
elseif not signaturesMatch() then
    finish('campaign overworld overlay signatures do not match retail')
else
    local ok, err = pcall(function()
        breakpoint_campaign_map_camera_move = PCSX.addBreakpoint(
            MOVE_CAMERA,
            'Exec',
            4,
            'Trace CampaignMap_MoveCameraDpad dispatch',
            function()
                local callbackOk, callbackErr = pcall(onMoveCamera)
                if not callbackOk then
                    callbackError = tostring(callbackErr)
                end
            end
        )
        breakpoint_campaign_map_camera_update = PCSX.addBreakpoint(
            UPDATE_LOCATION,
            'Exec',
            4,
            'Control: active CampaignMap_UpdateLocation tick',
            function()
                local callbackOk, callbackErr = pcall(onUpdateLocation)
                if not callbackOk then
                    callbackError = tostring(callbackErr)
                end
            end
        )
    end)
    if not ok then
        callbackError = tostring(err)
        finish('breakpoint installation error: ' .. callbackError)
    else
        print(SCRIPT_NAME
              .. ': breakpoints installed; try isolated camera controls')
    end
end
