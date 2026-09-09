-- campaign_duel_home_terrain.lua
--
-- WHAT THIS ANSWERS
--   notes/research/the-game.md leaves the home terrain of the five shrine
--   pairs and the late finale duels unverified, except for Guardian Sebek and
--   Guardian Neku. Matching Text_StartCampaignDuel reads the opponent, two
--   follow-up values and terrain from the story stream before changing the
--   main mode from campaign (2) to duel (3).
--
--   This trace captures the first unresolved shrine or finale opponent at
--   that exact mode transition. It records the campaign scene, opponent,
--   terrain, adjacent follow-up values, selected BGM and remaining setup
--   value. Human context identifies the duelist and visible field, which can
--   confirm one terrain assignment without inferring it from a guide.
--
-- HOW TO RUN
--   1. Start the script immediately before one campaign duel against a shrine
--      opponent (IDs 21-30) or late-finale opponent (IDs 35-38).
--   2. Enter the duel normally. No interpreter CPU or debugger breakpoint is
--      required.
--   3. Confirm the script reports the target opponent, then wait one second
--      for the result document.
--   4. Copy the whole document into
--      tools/trace/result/campaign_duel_home_terrain.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Name the duelist and campaign location, state whether this was the gate
--   mage, high mage or finale sequence, describe the visible field terrain,
--   and note any cheat, debug option or card effect that changed the terrain.

local ffi = require('ffi')

local SCRIPT_NAME = 'campaign_duel_home_terrain'
local MAIN_MODE = 0x8009b26c
local CAMPAIGN_SCENE = 0x8009b27a
local OPPONENT_ID = 0x8009b361
local TERRAIN = 0x8009b364
local DUEL_BGM = 0x8009b36a
local FOLLOWUP_A = 0x8009b370
local FOLLOWUP_B = 0x8009b372
local VALUE_374 = 0x8009b374
local CAMPAIGN_MODE = 2
local DUEL_MODE = 3
local STABLE_FRAMES = 60
local TIMEOUT_FRAMES = 36000
local MAX_SKIPPED_ROWS = 16

local TERRAIN_NAMES = {
    [0] = 'Normal',
    [1] = 'Forest',
    [2] = 'Wasteland',
    [3] = 'Mountain',
    [4] = 'Sogen',
    [5] = 'Umi',
    [6] = 'Yami',
}

local mem = PCSX.getMemPtr()

local function phys(addr)
    return addr - 0x80000000
end

local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end

local function u16(addr)
    return u8(addr) + u8(addr + 1) * 0x100
end

local function mainMode()
    return u8(MAIN_MODE) % 32
end

local function terrainName(value)
    return TERRAIN_NAMES[value] or string.format('unknown(%d)', value)
end

local function isTargetOpponent(value)
    return (value >= 21 and value <= 30)
        or (value >= 35 and value <= 38)
end

local function readSnapshot()
    return {
        scene = u8(CAMPAIGN_SCENE),
        opponent = u8(OPPONENT_ID),
        terrain = u8(TERRAIN),
        bgm = u16(DUEL_BGM),
        followupA = u16(FOLLOWUP_A),
        followupB = u16(FOLLOWUP_B),
        value374 = u16(VALUE_374),
    }
end

local function snapshotText(prefix, frame, snapshot)
    return string.format(
        '%s frame=%06d scene_index=0x%02X opponent_id=%d '
            .. 'terrain=%d(%s) D_8009B36A=0x%04X '
            .. 'D_8009B370=0x%04X D_8009B372=0x%04X '
            .. 'D_8009B374=0x%04X',
        prefix,
        frame,
        snapshot.scene,
        snapshot.opponent,
        snapshot.terrain,
        terrainName(snapshot.terrain),
        snapshot.bgm,
        snapshot.followupA,
        snapshot.followupB,
        snapshot.value374
    )
end

local function snapshotsEqual(left, right)
    return left.scene == right.scene
        and left.opponent == right.opponent
        and left.terrain == right.terrain
        and left.bgm == right.bgm
        and left.followupA == right.followupA
        and left.followupB == right.followupB
        and left.value374 == right.value374
end

local lines = {}
local frames = 0
local previousMode = mainMode()
local entry = nil
local latest = nil
local stableFrames = 0
local skippedCampaignDuels = 0
local ignoredNonCampaignDuels = 0
local changedAfterEntry = false
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
    print('<name the duelist and campaign location; state whether this was a')
    print(' gate mage, high mage or finale duel; describe the visible field')
    print(' terrain; list any cheat, debug option or card effect that changed it>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: target_found=%s skipped_campaign_duels=%d '
            .. 'ignored_non_campaign_duels=%d changed_after_entry=%s',
        tostring(entry ~= nil),
        skippedCampaignDuels,
        ignoredNonCampaignDuels,
        tostring(changedAfterEntry)
    ))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function captureTransition()
    local snapshot = readSnapshot()

    if not isTargetOpponent(snapshot.opponent) then
        skippedCampaignDuels = skippedCampaignDuels + 1
        if skippedCampaignDuels <= MAX_SKIPPED_ROWS then
            emit(snapshotText('skipped_campaign_duel', frames, snapshot))
        end
        print(string.format(
            '%s: campaign opponent %d is outside target IDs 21-30 and 35-38',
            SCRIPT_NAME,
            snapshot.opponent
        ))
        return
    end

    entry = snapshot
    latest = snapshot
    stableFrames = 0
    emit(snapshotText('entry', frames, entry))
    print(string.format(
        '%s: captured opponent %d on terrain %d (%s); note the visible field',
        SCRIPT_NAME,
        entry.opponent,
        entry.terrain,
        terrainName(entry.terrain)
    ))
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

    local mode = mainMode()

    if entry == nil then
        if mode == DUEL_MODE and previousMode ~= DUEL_MODE then
            if previousMode == CAMPAIGN_MODE then
                captureTransition()
            else
                ignoredNonCampaignDuels = ignoredNonCampaignDuels + 1
            end
        end
        previousMode = mode

        if frames >= TIMEOUT_FRAMES then
            finish('timed out before a target campaign duel; arm in mode 2')
        end
        return
    end

    if mode ~= DUEL_MODE then
        finish('left duel mode before the stability window completed')
        return
    end

    stableFrames = stableFrames + 1
    latest = readSnapshot()
    if not snapshotsEqual(entry, latest) and not changedAfterEntry then
        changedAfterEntry = true
        emit(snapshotText('first_change', frames, latest))
    end

    if stableFrames >= STABLE_FRAMES then
        emit(snapshotText('stable_window_end', frames, latest))
        if changedAfterEntry then
            finish('captured target duel; values changed during stability window')
        else
            finish('captured target duel through sixty stable frames')
        end
    end
end

listener_campaign_duel_home_terrain_reset =
    PCSX.Events.createEventListener(
        'ExecutionFlow::Reset',
        function()
            local ok, err = pcall(function()
                finish('reset observed; rerun immediately before the target duel')
            end)
            if not ok then
                callbackError = tostring(err)
            end
        end
    )

listener_campaign_duel_home_terrain =
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

print(SCRIPT_NAME
    .. ': armed; waiting for campaign mode 2 -> target duel mode 3')
