-- main_menu_entry_slots.lua
--
-- WHAT THIS ANSWERS
--   The main menu overlay keeps eleven object pointers at D_80184568.
--   func_80180D2C parks them off screen, func_80180DD0 releases them, and
--   func_80181E30 tears the screen down. All three are decompiled and match,
--   but nothing in the code says WHAT the eleven slots are, so the functions
--   and the table are still address-named.
--
--   This trace dumps the slots and each object's screen position at the moment
--   they are parked. Correlating the positions with what is actually drawn on
--   the menu should settle whether the slots are the menu entries, and in which
--   order they are stored.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game.
--   2. Debug -> Lua editor, paste this file, let it auto-run.
--   3. Go to the main menu. It does not matter whether you are already there
--      when you paste this, or arrive afterwards.
--   4. The document prints itself once the menu is up. Copy all of it into
--      tools/trace/result/main_menu_entry_slots.txt and fill in the context.
--
--   No breakpoint and no debugger needed, so the interpreter CPU is not
--   required either.
--
-- WHY IT NO LONGER USES A BREAKPOINT
--   The first version broke on func_80180D2C. That never fired, because the
--   routine runs once while the menu is being built, which is over before the
--   menu is interactive and usually before the script has been pasted. This
--   version instead samples the table every vertical blank, so it does not
--   matter when it is armed.
--
-- WHAT TO WRITE IN THE CONTEXT
--   The menu entries you can see, top to bottom, and which side each one slid
--   in from. That is the part the trace cannot see.

local ffi = require('ffi')

local SCRIPT_NAME = 'main_menu_entry_slots'
local SLOT_TABLE  = 0x80184568   -- D_80184568, eleven object pointers
local SLOT_COUNT  = 11
local AXIS_FLAG   = 0x80184596   -- D_80184596, receives the argument
local READY_FLAG  = 0x80184599   -- D_80184599, set to 1 on the way out
local MODULE_BASE = 0x80180000   -- overlay load address, dumped as a signature
local MAX_SAMPLES = 3
local SETTLE_FRAMES = 180   -- about three seconds with no change ends the trace

local mem = PCSX.getMemPtr()

local function phys(addr) return addr - 0x80000000 end
local function u8(addr) return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0]) end
local function u16(addr) return tonumber(ffi.cast('uint16_t*', mem + phys(addr))[0]) end
local function u32(addr) return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0]) end

local function s16(addr)
    local value = u16(addr)
    if value >= 0x8000 then return value - 0x10000 end
    return value
end

local lines = {}
local function emit(text) lines[#lines + 1] = text end

local samples = 0
local done = false
local lastKey = nil
local quiet = 0

local function capture()
    samples = samples + 1

    emit('')
    emit(string.format('--- sample %d of %d ---', samples, MAX_SAMPLES))

    local signature = {}
    for i = 0, 15 do signature[#signature + 1] = string.format('%02x', u8(MODULE_BASE + i)) end
    emit('module signature at 0x80180000: ' .. table.concat(signature, ' '))

    emit(string.format('D_80184596 = 0x%02x   D_80184599 = 0x%02x', u8(AXIS_FLAG), u8(READY_FLAG)))
    emit('')
    emit('slot  pointer     +0x30 +0x32   +0x36 +0x38   +0x60  +0x6C  +0x08   +0x0C')
    emit('                  (x)   (y)     (dx)  (dy)    timer  state  flags   colour')

    for slot = 0, SLOT_COUNT - 1 do
        local object = u32(SLOT_TABLE + slot * 4)
        if object == 0 then
            emit(string.format('%4d  (null)', slot))
        elseif object < 0x80000000 or object >= 0x80200000 then
            emit(string.format('%4d  0x%08x  <not a RAM pointer, not dereferenced>', slot, object))
        else
            emit(string.format('%4d  0x%08x  %5d %5d   %5d %5d   0x%04x 0x%02x   0x%04x  0x%06x',
                slot, object,
                s16(object + 0x30), s16(object + 0x32),
                s16(object + 0x36), s16(object + 0x38),
                u16(object + 0x60), u8(object + 0x6c),
                u16(object + 0x08), u32(object + 0x0c) % 0x1000000))
        end
    end
end

local function finish()
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<which screen was on display, and the menu entries you could see,')
    print(' top to bottom, plus which side each one slid in from>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    for _, line in ipairs(lines) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

-- A key describing the populated slots. Sampling only when this changes keeps
-- the document short while still catching the entries arriving one by one.
local function slotKey()
    local parts = {}
    for slot = 0, SLOT_COUNT - 1 do
        parts[#parts + 1] = string.format('%08x', u32(SLOT_TABLE + slot * 4))
    end
    return table.concat(parts, ',')
end

local function poll()
    if done then return end

    -- func_80180D2C sets this on its way out, so it is the cheapest signal
    -- that the menu has finished being built and the slots are populated.
    if u8(READY_FLAG) ~= 1 then return end

    local key = slotKey()
    if key ~= lastKey then
        lastKey = key
        quiet = 0
        capture()
        if samples >= MAX_SAMPLES then
            done = true
            finish()
        end
        return
    end

    -- Stop waiting for changes that are not coming. Without this the document
    -- is never printed when the table settles in fewer than MAX_SAMPLES
    -- states, which is the normal case for a menu that is simply sitting there.
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

listener_main_menu_entry_slots = PCSX.Events.createEventListener('GPU::Vsync', function()
    local ok, err = pcall(poll)
    if not ok then
        done = true
        print('main_menu_entry_slots: ' .. tostring(err))
    end
end)

print('main_menu_entry_slots: sampling every vertical blank. Go to the main')
print('menu; the document prints itself once the slots are populated.')
