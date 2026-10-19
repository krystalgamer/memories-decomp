-- model_camera_gate_flags.lua
--
-- WHAT THIS ANSWERS
--   src/game/model_transfer_flags.h describes two independent flag bytes,
--   D_8009B07B and D_8009B07C, with address-named accessors: func_8005F174 /
--   func_8005F180 get and set the first, func_8005F18C / func_8005F198 the
--   second. Six callers share one guard,
--
--       state = func_8005F174();
--       if (state != 1 || func_8005F18C() != state) { ... }
--
--   so camera work such as Model_RequestCameraMoveToSlots and func_80059EBC
--   is skipped exactly while both bytes read 1. The writers are suggestive
--   but not conclusive: model_slot_control.c sets the second byte to 1 only
--   around calls into the model overlay's per-slot handlers and clears it
--   straight after; model_scene_controllers.c sets the first to 0, 1 or 2
--   from per-attack data bits 0x80 and 0x40. Neither the bytes nor their
--   accessors can be named until it is known which presentation they guard
--   and what the skipped camera move would have done.
--
--   This trace breaks on both setters and on the three camera-request entry
--   points. Each setter row records the value, the caller and the main mode;
--   each request row records its arguments, both flag bytes, and whether the
--   guard suppressed it. It also polls the neighbouring D_8009B079 flag and
--   D_8009B07A counter, which share the block and are not named either.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game, enable the debugger and select the
--      interpreter CPU. Breakpoints do not fire on the dynarec.
--   2. Debug -> Lua editor, paste this file and confirm it reports five
--      installed breakpoints and no signature mismatch.
--   3. In a duel, let several monster battles play out with their 3-D
--      animation: attack with your monster, let the opponent attack you,
--      and if you can make one fusion and one direct attack.
--   4. The trace prints after 64 rows, or three quiet minutes after the
--      first row.
--   5. Copy the whole document into
--      tools/trace/result/model_camera_gate_flags.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   List each battle in order: attacker and defender, and what the camera
--   did -- cut, pan, zoom, follow a monster, or stay still. Note any battle
--   where the camera visibly did not move while a monster was moving.

local ffi = require('ffi')

local SCRIPT_NAME = 'model_camera_gate_flags'

local RAM_BASE = 0x80000000
local RAM_END = 0x80200000

local MAIN_MODE = 0x8009b26c
local FLAG_A = 0x8009b07b
local FLAG_B = 0x8009b07c
local FLAG_079 = 0x8009b079
local COUNTER_07A = 0x8009b07a

local SET_A = 0x8005f180
local SET_B = 0x8005f198
local MOVE = 0x80052d2c
local REQUEST_SLOTS = 0x80059f18
local REQUEST_VALUE = 0x80059ebc

local TARGETS = {
    {SET_A, 0xa3840173, 'set first byte func_8005F180'},
    {SET_B, 0xa3840174, 'set second byte func_8005F198'},
    {REQUEST_SLOTS, 0x27bdffd8, 'Model_RequestCameraMoveToSlots'},
    {REQUEST_VALUE, 0x27bdffe0, 'func_80059EBC'},
    {MOVE, 0x27bdffc0, 'camera move func_80052D2C'},
}

local ROWS_PER_KEY = 2
local MAX_ROWS = 64
local QUIET_FRAMES = 10800
local NO_HIT_WARNING_FRAMES = 36000
local TIMEOUT_FRAMES = 216000

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function s8(addr)
    local v = u8(addr)
    if v ~= nil and v >= 0x80 then v = v - 0x100 end
    return v
end
local function u32(addr)
    if not inRam(addr, 4) then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local function show(v, fmt)
    if v == nil then return 'unreadable' end
    return string.format(fmt or '%d', v)
end
local function signed(v)
    if v >= 0x80000000 then return v - 0x100000000 end
    return v
end

local frames = 0
local lastRowFrame = nil
local rows = {}
local counts = {}
local logged = {}
local suppressed = 0
local passed = 0
local mismatches = {}
local callbackError = nil
local last079, last07A = nil, nil
local done = false

breakpoints_model_camera_gate_flags = {}

local function row(key, text)
    counts[key] = (counts[key] or 0) + 1
    if #rows >= MAX_ROWS or (logged[key] or 0) >= ROWS_PER_KEY then return end
    logged[key] = (logged[key] or 0) + 1
    rows[#rows + 1] = string.format('row=%02d frame=%06d ', #rows + 1, frames)
        .. text
    lastRowFrame = frames
end

local function flags()
    local mode = u8(MAIN_MODE)
    return string.format('first=%s second=%s mode=%s', show(u8(FLAG_A)),
                         show(u8(FLAG_B)),
                         mode and string.format('0x%02X', mode % 32)
                         or 'unreadable')
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_model_camera_gate_flags) do bp:disable() end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<each battle in order: attacker, defender, and what the camera')
    print(' did -- cut, pan, zoom, follow, or stay still while a monster moved>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format('frames: %d rows: %d gated_requests_passed: %d '
                        .. 'gated_requests_suppressed: %d',
                        frames, #rows, passed, suppressed))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    local keys = {}
    for key in pairs(counts) do keys[#keys + 1] = key end
    table.sort(keys)
    for _, key in ipairs(keys) do
        print(string.format('count %s %d', key, counts[key]))
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function onHit(target)
    if done then return end
    local n = PCSX.getRegisters().GPR.n
    local addr = target[1]
    if addr == SET_A or addr == SET_B then
        local value = signed(n.a0)
        local key = string.format('%s value=%d caller=0x%08X', target[3],
                                  value, n.ra)
        row(key, string.format('%s value=%d caller=0x%08X before: %s',
                               target[3], value, n.ra, flags()))
    elseif addr == MOVE then
        local key = string.format('%s caller=0x%08X', target[3], n.ra)
        row(key, string.format('%s args=%d,%d,%d,%d caller=0x%08X %s',
                               target[3], signed(n.a0), signed(n.a1),
                               signed(n.a2), signed(n.a3), n.ra, flags()))
    else
        local blocked = u8(FLAG_A) == 1 and u8(FLAG_B) == 1
        if blocked then suppressed = suppressed + 1 else passed = passed + 1 end
        local verdict = blocked and 'SUPPRESSED' or 'passed'
        local key = string.format('%s caller=0x%08X %s', target[3], n.ra,
                                  verdict)
        row(key, string.format('%s args=%d,%d,%d,%d caller=0x%08X %s %s',
                               target[3], signed(n.a0), signed(n.a1),
                               signed(n.a2), signed(n.a3), n.ra, flags(),
                               verdict))
    end
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end

    local f079, c07A = u8(FLAG_079), s8(COUNTER_07A)
    if last079 ~= nil and (f079 ~= last079 or c07A ~= last07A) then
        row(string.format('poll D_8009B079=%s D_8009B07A=%s', show(f079),
                          show(c07A)),
            string.format('poll D_8009B079 %s->%s D_8009B07A %s->%s %s',
                          show(last079), show(f079), show(last07A),
                          show(c07A), flags()))
    end
    last079, last07A = f079, c07A

    if #rows >= MAX_ROWS then
        finish('captured ' .. MAX_ROWS .. ' rows')
    elseif lastRowFrame ~= nil and frames - lastRowFrame >= QUIET_FRAMES then
        finish('three quiet minutes without a new row')
    elseif lastRowFrame == nil and frames >= NO_HIT_WARNING_FRAMES then
        finish('nothing in ten minutes; select the interpreter CPU and let '
               .. 'a monster battle play')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

for _, target in ipairs(TARGETS) do
    local word = u32(target[1])
    if word ~= target[2] then
        mismatches[#mismatches + 1] = string.format(
            '0x%08X expected 0x%08X read %s', target[1], target[2],
            show(word, '0x%08X'))
    end
    local t = target
    breakpoints_model_camera_gate_flags[#breakpoints_model_camera_gate_flags + 1] =
        PCSX.addBreakpoint(t[1], 'Exec', 4, SCRIPT_NAME .. ': ' .. t[3],
            function()
                local ok, err = pcall(onHit, t)
                if not ok then callbackError = tostring(err) end
            end)
end

listener_model_camera_gate_flags = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: %d breakpoints installed, %d signature mismatches',
                    SCRIPT_NAME, #breakpoints_model_camera_gate_flags,
                    #mismatches))
