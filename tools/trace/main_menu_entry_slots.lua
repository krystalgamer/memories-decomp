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
--   1. Open PCSX-Redux with the game, enable the debugger, and make sure the
--      interpreter CPU is selected. Breakpoints do not fire on the dynarec.
--   2. Debug -> Lua editor, paste this file, let it auto-run.
--   3. Boot to the title screen and enter the main menu so the overlay loads.
--   4. The script prints a finished document once it has seen the parking
--      routine a few times. Copy all of it into
--      tools/trace/result/main_menu_entry_slots.txt and fill in the context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Which screen you were on, and the menu entries you could see, top to
--   bottom, in the order they appear. If entries slid in from the sides, say
--   which side each one came from. That is the part the trace cannot see.

local ffi = require('ffi')

local SCRIPT_NAME = 'main_menu_entry_slots'
local PARK_FN     = 0x80180d2c   -- func_80180D2C, parks the slots off screen
local SLOT_TABLE  = 0x80184568   -- D_80184568, eleven object pointers
local SLOT_COUNT  = 11
local AXIS_FLAG   = 0x80184596   -- D_80184596, receives the argument
local READY_FLAG  = 0x80184599   -- D_80184599, set to 1 on the way out
local MODULE_BASE = 0x80180000   -- overlay load address, dumped as a signature
local MAX_HITS    = 3

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

local hits = 0
local done = false

local function capture()
    hits = hits + 1
    local regs = PCSX.getRegisters()

    emit('')
    emit(string.format('--- hit %d of %d, called from pc=0x%08x with a0=0x%08x ---',
                       hits, MAX_HITS, tonumber(regs.pc), tonumber(regs.GPR.n.a0)))
    emit(string.format('argument selects the moving axis: a0=%d', tonumber(regs.GPR.n.a0)))

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

breakpoint_main_menu_entry_slots = PCSX.addBreakpoint(
    PARK_FN, 'Exec', 4, 'main menu slot parking',
    function()
        if done then return false end
        local ok, err = pcall(function()
            capture()
            if hits >= MAX_HITS then
                done = true
                finish()
            end
        end)
        if not ok then print('main_menu_entry_slots: ' .. tostring(err)) end
        if done then return false end
    end)

print('main_menu_entry_slots: armed at 0x80180d2c. Enter the main menu; the')
print('document prints itself after ' .. MAX_HITS .. ' parking calls.')
