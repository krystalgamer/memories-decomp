-- Run from the repository root:
--   luajit tools/trace/tests/fade_band_tail_usage_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
--
-- This verifies call-site guards, segment accounting and bounded finish paths.
-- It does not establish whether the combined tail is visible in-game.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/fade_band_tail_usage.lua'

local RAM_BASE = 0x80000000
local BAND_SUBMIT = 0x800155a0
local TAIL_SUBMIT = 0x80015690
local SORT_BOX_FILL_JAL = 0x0c021090
local FADE_STATE = 0x800e9ec8
local MAIN_MODE = 0x8009b26c
local TAIL_DEPTH = 0x8009b140
local DRAW_CONTROL = 0x8009b141
local COLOR_OVERRIDE = 0x8009b145

local function capture(validSignature)
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local listeners = {}
    local breakpoints = {}
    local output = {}

    local function u8at(addr)
        return ffi.cast('uint8_t*', memory + (addr - RAM_BASE))
    end

    local function u16at(addr)
        return ffi.cast('uint16_t*', memory + (addr - RAM_BASE))
    end

    local function u32at(addr)
        return ffi.cast('uint32_t*', memory + (addr - RAM_BASE))
    end

    if validSignature ~= false then
        u32at(BAND_SUBMIT)[0] = SORT_BOX_FILL_JAL
        u32at(TAIL_SUBMIT)[0] = SORT_BOX_FILL_JAL
    end

    print = function(...)
        local values = {}
        for index = 1, select('#', ...) do
            values[index] = tostring(select(index, ...))
        end
        output[#output + 1] = table.concat(values, '\t')
    end

    PCSX = {
        getMemPtr = function()
            return memory
        end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = { enabled = true, callback = callback }
            function handle:disable()
                self.enabled = false
            end
            breakpoints[address] = handle
            return handle
        end,
        Events = {
            createEventListener = function(name, callback)
                listeners[name] = callback
                return {}
            end,
        },
    }
    breakpoint_fade_band_tail_band = nil
    breakpoint_fade_band_tail_tail = nil
    listener_fade_band_tail_reset = nil
    listener_fade_band_tail_usage = nil
    dofile(scriptPath)

    local result = {}

    function result:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then
                return true
            end
        end
        return false
    end

    function result:snapshot(
        mode, flags, level, target, step, head, depth, draw, color
    )
        u8at(MAIN_MODE)[0] = mode
        u8at(FADE_STATE + 4)[0] = level
        u8at(FADE_STATE + 5)[0] = target
        u8at(FADE_STATE + 6)[0] = flags
        u8at(FADE_STATE + 7)[0] = step
        u16at(FADE_STATE + 8)[0] = head % 0x10000
        u8at(TAIL_DEPTH)[0] = depth
        u8at(DRAW_CONTROL)[0] = draw
        u8at(COLOR_OVERRIDE)[0] = color
    end

    function result:band(count)
        local breakpoint = breakpoints[BAND_SUBMIT]
        assert(breakpoint and breakpoint.enabled)
        for _ = 1, count or 1 do
            breakpoint.callback()
        end
    end

    function result:tail(count)
        local breakpoint = breakpoints[TAIL_SUBMIT]
        assert(breakpoint and breakpoint.enabled)
        for _ = 1, count or 1 do
            breakpoint.callback()
        end
    end

    function result:frames(count)
        for _ = 1, count do
            listeners['GPU::Vsync']()
        end
    end

    function result:reset()
        listeners['ExecutionFlow::Reset']()
    end

    function result:installed()
        return breakpoints[BAND_SUBMIT] ~= nil
            and breakpoints[TAIL_SUBMIT] ~= nil
    end

    function result:disabled()
        return not breakpoints[BAND_SUBMIT].enabled
            and not breakpoints[TAIL_SUBMIT].enabled
    end

    return result
end

local combined = capture(true)
combined:snapshot(0x23, 0x03, 100, 200, 12, -5, 7, 2, 9)
combined:band(2)
combined:tail(1)
combined:frames(1)
combined:frames(1)
combined:frames(119)
assert(combined:has('status: captured a combined band-plus-tail segment'),
       'a combined segment must finish after 120 quiet frames')
assert(combined:has(
    'summary: segments=1 combined=1 band_only=0 tail_only=0 '
        .. 'hit_frames=1 output_truncated=false'
), 'the combined summary must classify one hit frame')
assert(combined:has(
    'kind=combined_band_tail mode_raw=0x23 mode=03 flags=0x03 '
        .. 'bands=true tail=true level=100 target=200 step=12 '
        .. 'head=-5 depth=7 draw_control=2 color_override=9'
), 'the segment snapshot must preserve every fade-state field')
assert(combined:has(
    'duration=1 kind=combined_band_tail band_calls=2 tail_calls=1'
), 'same-frame submission counts must be retained')
assert(combined:disabled())

local ordinary = capture(true)
for segment = 1, 6 do
    if segment % 2 == 1 then
        ordinary:band(1)
    else
        ordinary:tail(1)
    end
    ordinary:frames(1)
    ordinary:frames(1)
end
ordinary:frames(119)
assert(ordinary:has(
    'status: captured six fade segments without waiting for more'
), 'six ordinary segments must satisfy the alternate finish bound')
assert(ordinary:has(
    'summary: segments=6 combined=0 band_only=3 tail_only=3 hit_frames=6'
), 'band-only and tail-only segments must be counted separately')
assert(ordinary:disabled())

local truncated = capture(true)
for segment = 1, 60 do
    if segment % 2 == 1 then
        truncated:band(1)
    else
        truncated:tail(1)
    end
    truncated:frames(1)
end
truncated:frames(1)
truncated:frames(119)
assert(truncated:has('segments=60 combined=0 band_only=30 tail_only=30'),
       'kind changes must start distinct segments')
assert(truncated:has('output_truncated=true'),
       'the line buffer must report its 96-line bound')
assert(truncated:disabled())

local silent = capture(true)
silent:frames(600)
assert(silent:has(
    'no fade submissions observed; confirm interpreter CPU'
), 'a no-hit run must print the likely setup diagnosis')
silent:frames(107400)
assert(silent:has(
    'status: timed out without a fade submission; use interpreter CPU'
), 'a silent run must finish with an explicit no-hit status')
assert(silent:disabled())

local reset = capture(true)
reset:band(1)
reset:frames(1)
reset:reset()
assert(reset:has('status: reset observed; rerun the script after reset'),
       'a reset must end the current trace explicitly')
assert(reset:has('reason=capture finished'),
       'a reset must close any active segment')

local mismatch = capture(false)
assert(mismatch:has('call-site signature mismatch band=0x00000000'),
       'wrong call-site instructions must reject the trace')
assert(not mismatch:installed(),
       'signature rejection must happen before breakpoints are installed')

print = hostPrint
print('fade_band_tail_usage: all six callback-replay cases passed')
