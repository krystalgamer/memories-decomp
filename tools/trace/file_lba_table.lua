-- file_lba_table.lua
--
-- WHAT THIS ANSWERS
--   File_SetPositionTable fills seven words at gFile_anLba (0x800E9EA8)
--   from the filenames in gFile_apszName (0x8009078C). Static analysis
--   establishes the table shape, but the retail executable initializes the
--   words at runtime, so their values cannot be verified from the EXE alone.
--
--   This waits for all seven entries to become positive, then prints each
--   filename pointer, filename, and resolved disc LBA.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game.
--   2. Debug -> Lua editor, paste this file, and let it auto-run.
--   3. If the game is already past boot, the table should print on the next
--      frame. Otherwise let the boot sequence continue.
--   4. Copy the whole document into
--      tools/trace/result/file_lba_table.txt and fill in the context.
--
--   No breakpoint, debugger pause, or interpreter CPU is required.
--
-- WHAT TO WRITE IN THE CONTEXT
--   State whether the script was started before or after the title screen and
--   whether the game was running from a normal retail disc image.

local ffi = require('ffi')

local SCRIPT_NAME = 'file_lba_table'
local FILE_NAMES = 0x8009078c
local FILE_LBAS = 0x800e9ea8
local ENTRY_COUNT = 7
local MAX_NAME_BYTES = 64
local TIMEOUT_FRAMES = 1200

local mem = PCSX.getMemPtr()

local function phys(addr) return addr - 0x80000000 end
local function u8(addr)
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end
local function u32(addr)
    return tonumber(ffi.cast('uint32_t*', mem + phys(addr))[0])
end
local function s32(addr)
    local value = u32(addr)
    if value >= 0x80000000 then
        return value - 0x100000000
    end
    return value
end
local function isRam(addr)
    return addr >= 0x80000000 and addr < 0x80200000
end

local function readCString(addr)
    if not isRam(addr) then
        return '<not a RAM pointer>'
    end

    local chars = {}
    for i = 0, MAX_NAME_BYTES - 1 do
        local value = u8(addr + i)
        if value == 0 then
            return table.concat(chars)
        end
        if value >= 0x20 and value <= 0x7e then
            chars[#chars + 1] = string.char(value)
        else
            chars[#chars + 1] = string.format('<%02X>', value)
        end
    end
    return table.concat(chars) .. '<unterminated>'
end

local function tableReady()
    for i = 0, ENTRY_COUNT - 1 do
        if s32(FILE_LBAS + i * 4) <= 0 then
            return false
        end
    end
    return true
end

local function printResult(reason)
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<state whether this was run before or after the title screen, and')
    print(' whether the game used an unmodified retail disc image>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    for i = 0, ENTRY_COUNT - 1 do
        local namePointer = u32(FILE_NAMES + i * 4)
        local lba = s32(FILE_LBAS + i * 4)
        print(string.format(
            '  [%d] name_ptr=0x%08X name=%s lba=%d (0x%08X)',
            i, namePointer, readCString(namePointer), lba, lba % 0x100000000
        ))
    end
    print(string.format(
        '  terminator name_ptr=0x%08X', u32(FILE_NAMES + ENTRY_COUNT * 4)
    ))
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local frames = 0
local done = false

local function poll()
    if done then
        return
    end

    frames = frames + 1
    if tableReady() then
        done = true
        printResult('all seven LBA entries initialized')
    elseif frames >= TIMEOUT_FRAMES then
        done = true
        printResult('timed out before every LBA became positive')
    end
end

listener_file_lba_table = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            done = true
            print('file_lba_table: ' .. tostring(err))
        end
    end
)

print('file_lba_table: waiting for the seven runtime file LBAs')
