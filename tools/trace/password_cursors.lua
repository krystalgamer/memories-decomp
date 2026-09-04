-- password_cursors.lua
--
-- WHAT THIS ANSWERS
--   The password screen has two cursor-like values and neither is named:
--
--     D_8016D428  compared against 0 and 7, and placed at x = value * 16 + 0xA3
--     D_8016D42C  clamped to 0..5,        and placed at x = value * 16 + 0x6B
--
--   Eight positions would be the eight password digits described in
--   notes/research/the-game.md section 4.5. Six is unexplained, and the two
--   use different x origins, so they are not the same control.
--
--   This samples both, the objects they move, and the eight entered digits,
--   so the values can be matched against what is actually on screen.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game.
--   2. Debug -> Lua editor, paste this file, let it auto-run.
--   3. Reach the password screen: from the post-load main menu choose
--      Password. It does not matter whether you arrive before or after
--      pasting this.
--   4. Move each cursor a few times and type part of a password. The script
--      prints a sample whenever anything it watches changes.
--   5. Copy the whole document into tools/trace/result/password_cursors.txt
--      and fill in the context.
--
--   No breakpoint, so no debugger and no interpreter CPU needed.
--
-- WHAT TO WRITE IN THE CONTEXT
--   What the password screen shows, and for each of the two cursors, what you
--   were pressing when the value changed and what visibly moved. If one of
--   them is not a cursor at all, say what else changed on screen. Six
--   positions is the part that needs explaining.

local ffi = require('ffi')

local SCRIPT_NAME = 'password_cursors'
local MODULE_BASE = 0x80168000
local MODULE_ID   = 0x15          -- password; free_duel is 0x13, overworld 0x14
local DIGITS      = 0x8016d410    -- gPassword_abDigits, eight bytes
local D_400       = 0x8016d400
local D_420       = 0x8016d420
local D_428       = 0x8016d428
local D_42C       = 0x8016d42c
local D_43C       = 0x8016d43c
local MAX_SAMPLES = 12
local SETTLE_FRAMES = 600         -- about ten seconds with no change ends it

local mem = PCSX.getMemPtr()

local function phys(addr) return addr - 0x80000000 end
local function u8(a) return tonumber(ffi.cast('uint8_t*', mem + phys(a))[0]) end
local function u16(a) return tonumber(ffi.cast('uint16_t*', mem + phys(a))[0]) end
local function u32(a) return tonumber(ffi.cast('uint32_t*', mem + phys(a))[0]) end
local function s8(a) local v = u8(a) if v >= 0x80 then return v - 0x100 end return v end
local function s16(a) local v = u16(a) if v >= 0x8000 then return v - 0x10000 end return v end

local function isRam(p) return p >= 0x80000000 and p < 0x80200000 end

local lines = {}
local function emit(t) lines[#lines + 1] = t end

local samples, done, lastKey, quiet = 0, false, nil, 0

local function describe(label, pointer, offsets)
    if not isRam(pointer) then
        emit(string.format('  %-12s 0x%08x  <not a RAM pointer>', label, pointer))
        return
    end
    local parts = {}
    for _, off in ipairs(offsets) do
        parts[#parts + 1] = string.format('+0x%02X=%d', off, s16(pointer + off))
    end
    emit(string.format('  %-12s 0x%08x  %s', label, pointer, table.concat(parts, '  ')))
end

local function capture()
    samples = samples + 1
    emit('')
    emit(string.format('--- sample %d of %d ---', samples, MAX_SAMPLES))
    emit(string.format('  D_8016D400 = 0x%02X   (bit 0x10 = %s)',
                       u8(D_400), (u8(D_400) % 32 >= 16) and 'set' or 'clear'))
    emit(string.format('  D_8016D428 = %d      -> x would be %d',
                       u32(D_428), u32(D_428) * 16 + 0xA3))
    emit(string.format('  D_8016D42C = %d      -> x would be %d',
                       s8(D_42C), s8(D_42C) * 16 + 0x6B))
    describe('D_8016D420', u32(D_420), {0x18, 0x1A, 0x30, 0x32})
    describe('D_8016D43C', u32(D_43C), {0x44, 0x46, 0x30, 0x32})
    local digits = {}
    for i = 0, 7 do digits[#digits + 1] = string.format('%02X', u8(DIGITS + i)) end
    emit('  gPassword_abDigits: ' .. table.concat(digits, ' '))
end

local function finish()
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<what the password screen showed, and for each cursor what you were')
    print(' pressing when it changed and what moved on screen>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    for _, l in ipairs(lines) do print(l) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function key()
    local parts = { u8(D_400), u32(D_428), u8(D_42C), u32(D_420), u32(D_43C) }
    for i = 0, 7 do parts[#parts + 1] = u8(DIGITS + i) end
    return table.concat(parts, ',')
end

local function poll()
    if done then return end
    -- Four overlays share 0x80168000, so check which one is resident before
    -- reading anything else. Reading these addresses under free_duel or the
    -- overworld would produce confident nonsense.
    if u32(MODULE_BASE) ~= MODULE_ID then return end

    local k = key()
    if k ~= lastKey then
        lastKey = k
        quiet = 0
        capture()
        if samples >= MAX_SAMPLES then
            done = true
            finish()
        end
        return
    end

    if samples > 0 then
        quiet = quiet + 1
        if quiet >= SETTLE_FRAMES then
            done = true
            emit('')
            emit(string.format('(no further change for %d frames)', SETTLE_FRAMES))
            finish()
        end
    end
end

listener_password_cursors = PCSX.Events.createEventListener('GPU::Vsync', function()
    local ok, err = pcall(poll)
    if not ok then
        done = true
        print('password_cursors: ' .. tostring(err))
    end
end)

print('password_cursors: sampling every vertical blank while the password')
print('module (id 0x15) is resident. Move the cursors and type some digits.')
