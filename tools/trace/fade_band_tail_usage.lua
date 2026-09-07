-- fade_band_tail_usage.lua
--
-- WHAT THIS ANSWERS
--   notes/fade-transition-state.md and notes/research/the-game.md establish
--   that Fade_DrawOverlay can submit either thirty 320x8 bands, one 320x240
--   box, or the bands followed by an extra 320x8 box at y=240 when flag bits
--   0x01 and 0x02 are combined. Static code does not establish which visible
--   transitions use that combined path or whether the SDK position offset
--   makes the extra bottom box visible.
--
--   This trace hooks the two exact GsSortBoxFill call sites inside
--   Fade_DrawOverlay. It groups frames into band-only, full-screen-tail, and
--   combined band-plus-tail segments, recording the main mode and fade state
--   at each segment boundary. Human context identifies the visible screen and
--   whether a strip, flash, or shifted line appears at the bottom edge.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec.
--   3. Start at the initial or loaded menu, open Debug -> Lua editor, paste
--      this file, and confirm that it prints "breakpoints installed".
--   4. Exercise several isolated transitions, pausing for one second on each
--      destination. Suggested routes are OPTION and back, BUILD DECK and
--      back, FREE DUEL and back, and LIBRARY or PASSWORD and back.
--   5. Each new render segment prints its number and kind. Note the screen
--      visible during that segment. For "combined_band_tail", inspect the
--      bottom edge for a strip, flash, shifted line, or no visible effect.
--   6. Do not reset while capturing. After a combined segment or six ordinary
--      segments, wait for the trace to finish and copy the whole document into
--      tools/trace/result/fade_band_tail_usage.txt.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm a retail executable and interpreter CPU. For every numbered
--   segment, state the action, source screen, destination screen, and what was
--   visible. For each combined segment, explicitly describe the bottom edge.

local ffi = require('ffi')

local SCRIPT_NAME = 'fade_band_tail_usage'
local BAND_SUBMIT = 0x800155a0
local TAIL_SUBMIT = 0x80015690
local SORT_BOX_FILL_JAL = 0x0c021090
local FADE_STATE = 0x800e9ec8
local MAIN_MODE = 0x8009b26c
local TAIL_DEPTH = 0x8009b140
local DRAW_CONTROL = 0x8009b141
local COLOR_OVERRIDE = 0x8009b145
local FLAG_BANDS = 0x01
local FLAG_TAIL = 0x02
local TARGET_SEGMENTS = 6
local QUIET_FRAMES = 120
local NO_HIT_WARNING_FRAMES = 600
local TIMEOUT_FRAMES = 108000
local MAX_LINES = 96

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

local function hasFlag(value, flag)
    return math.floor(value / flag) % 2 == 1
end

local lines = {}
local frames = 0
local bandCalls = 0
local tailCalls = 0
local hitFrames = 0
local lastActivityFrame = nil
local segmentCount = 0
local combinedSegments = 0
local segmentKinds = {
    band_only = 0,
    tail_only = 0,
    combined_band_tail = 0,
}
local currentSegment = nil
local callbackError = nil
local noHitWarningPrinted = false
local outputTruncated = false
local done = false

local function emit(text)
    if #lines < MAX_LINES then
        lines[#lines + 1] = text
    else
        outputTruncated = true
    end
end

local function mode()
    return u8(MAIN_MODE) % 32
end

local function snapshotText()
    local flags = u8(FADE_STATE + 6)

    return string.format(
        'mode_raw=0x%02X mode=%02d flags=0x%02X bands=%s tail=%s '
            .. 'level=%d target=%d step=%d head=%d depth=%d '
            .. 'draw_control=%d color_override=%d',
        u8(MAIN_MODE),
        mode(),
        flags,
        tostring(hasFlag(flags, FLAG_BANDS)),
        tostring(hasFlag(flags, FLAG_TAIL)),
        u8(FADE_STATE + 4),
        u8(FADE_STATE + 5),
        u8(FADE_STATE + 7),
        s16(FADE_STATE + 8),
        u8(TAIL_DEPTH),
        u8(DRAW_CONTROL),
        u8(COLOR_OVERRIDE)
    )
end

local function closeSegment(reason)
    if currentSegment == nil then
        return
    end

    emit(string.format(
        'segment_end=%02d frame=%06d duration=%d kind=%s '
            .. 'band_calls=%d tail_calls=%d reason=%s',
        currentSegment.number,
        frames,
        currentSegment.frames,
        currentSegment.kind,
        currentSegment.band_calls,
        currentSegment.tail_calls,
        reason
    ))
    currentSegment = nil
end

local function startSegment(kind)
    segmentCount = segmentCount + 1
    segmentKinds[kind] = segmentKinds[kind] + 1
    if kind == 'combined_band_tail' then
        combinedSegments = combinedSegments + 1
    end

    currentSegment = {
        number = segmentCount,
        kind = kind,
        frames = 0,
        band_calls = 0,
        tail_calls = 0,
    }

    emit(string.format(
        'segment_start=%02d frame=%06d kind=%s %s',
        segmentCount,
        frames,
        kind,
        snapshotText()
    ))

    if kind == 'combined_band_tail' then
        print(string.format(
            '%s: segment %d combined_band_tail; note the visible screen '
                .. 'and bottom edge',
            SCRIPT_NAME,
            segmentCount
        ))
    else
        print(string.format(
            '%s: segment %d %s; note the visible transition',
            SCRIPT_NAME,
            segmentCount,
            kind
        ))
    end
end

local function disableBreakpoint(handle)
    if handle ~= nil then
        pcall(function()
            handle:disable()
        end)
    end
end

local function finish(reason)
    if done then
        return
    end
    done = true

    closeSegment('capture finished')
    disableBreakpoint(breakpoint_fade_band_tail_band)
    disableBreakpoint(breakpoint_fade_band_tail_tail)

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<confirm retail executable and interpreter CPU; for every segment')
    print(' identify the action, source screen, destination screen, and visible')
    print(' transition; for each combined_band_tail segment explicitly describe')
    print(' any bottom-edge strip, flash, shifted line, or absence of an effect>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format(
        'summary: segments=%d combined=%d band_only=%d tail_only=%d '
            .. 'hit_frames=%d output_truncated=%s',
        segmentCount,
        combinedSegments,
        segmentKinds.band_only,
        segmentKinds.tail_only,
        hitFrames,
        tostring(outputTruncated)
    ))
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function renderKind()
    if bandCalls > 0 and tailCalls > 0 then
        return 'combined_band_tail'
    end
    if bandCalls > 0 then
        return 'band_only'
    end
    if tailCalls > 0 then
        return 'tail_only'
    end
    return nil
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

    local kind = renderKind()
    if kind ~= nil then
        hitFrames = hitFrames + 1
        lastActivityFrame = frames

        if currentSegment == nil then
            startSegment(kind)
        elseif currentSegment.kind ~= kind then
            closeSegment('render kind changed')
            startSegment(kind)
        end

        currentSegment.frames = currentSegment.frames + 1
        currentSegment.band_calls =
            currentSegment.band_calls + bandCalls
        currentSegment.tail_calls =
            currentSegment.tail_calls + tailCalls
    elseif currentSegment ~= nil then
        closeSegment('draw submissions stopped')
    end

    bandCalls = 0
    tailCalls = 0

    if not noHitWarningPrinted
        and hitFrames == 0
        and frames >= NO_HIT_WARNING_FRAMES then
        noHitWarningPrinted = true
        print(SCRIPT_NAME
            .. ': no fade submissions observed; confirm interpreter CPU '
            .. 'and trigger a menu transition')
    end

    if currentSegment == nil and lastActivityFrame ~= nil then
        local quiet = frames - lastActivityFrame
        if combinedSegments > 0 and quiet >= QUIET_FRAMES then
            finish('captured a combined band-plus-tail segment')
        elseif segmentCount >= TARGET_SEGMENTS and quiet >= QUIET_FRAMES then
            finish('captured six fade segments without waiting for more')
        end
    end

    if frames >= TIMEOUT_FRAMES then
        if hitFrames > 0 then
            finish('timed out after a partial fade trace')
        else
            finish('timed out without a fade submission; use interpreter CPU')
        end
    end
end

if u32(BAND_SUBMIT) ~= SORT_BOX_FILL_JAL
    or u32(TAIL_SUBMIT) ~= SORT_BOX_FILL_JAL then
    print(string.format(
        '%s: call-site signature mismatch band=0x%08X tail=0x%08X',
        SCRIPT_NAME,
        u32(BAND_SUBMIT),
        u32(TAIL_SUBMIT)
    ))
    return
end

breakpoint_fade_band_tail_band = PCSX.addBreakpoint(
    BAND_SUBMIT,
    'Exec',
    4,
    'Trace fade band box submission',
    function()
        local ok, err = pcall(function()
            bandCalls = bandCalls + 1
        end)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

breakpoint_fade_band_tail_tail = PCSX.addBreakpoint(
    TAIL_SUBMIT,
    'Exec',
    4,
    'Trace fade tail box submission',
    function()
        local ok, err = pcall(function()
            tailCalls = tailCalls + 1
        end)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_fade_band_tail_reset = PCSX.Events.createEventListener(
    'ExecutionFlow::Reset',
    function()
        local ok, err = pcall(function()
            breakpoint_fade_band_tail_band = nil
            breakpoint_fade_band_tail_tail = nil
            finish('reset observed; rerun the script after reset')
        end)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_fade_band_tail_usage = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print(SCRIPT_NAME .. ': breakpoints installed')
print(SCRIPT_NAME
    .. ': armed; exercise menu transitions without resetting the console')
