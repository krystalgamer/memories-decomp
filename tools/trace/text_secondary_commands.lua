-- text_secondary_commands.lua
--
-- WHAT THIS ANSWERS
--   notes/text-control-bytecode.md documents the primary text control bytes,
--   and its F8 entry escapes into a second table: Text_DispatchSecondaryCommand
--   (0x80038B4C) reads one opcode byte and calls D_80090EAC[opcode], whose 47
--   entries "cover text formatting, string insertion, sound, fades,
--   campaign-duel setup, display objects, and other effect commands".
--   Nineteen of those opcodes still reach address-named handlers:
--
--     00 func_80037DA4  01 func_800380D4  02 func_80038110  03 func_80038148
--     04 func_800382A8  05 func_80038334  08 func_80038070  09 func_800383DC
--     0B func_80038094  12 func_80038888  13 func_80038890  14 func_80038898
--     16 func_800389C4  17 func_800389D8  18 func_80038A44  2B func_80039BE0
--     2C func_80039AFC  2D func_80039BE0  2E func_80039C94
--
--   Reading each handler gives its operand count and the fields it writes,
--   not what the player sees. This trace breaks at the dispatcher entry and,
--   for each opcode, records where it ran (text string id, caller, main mode)
--   and the bytes that follow it in the stream. Named opcodes are logged once
--   as context; unnamed ones up to four times each.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game, enable the debugger and select the
--      interpreter CPU. Breakpoints do not fire on the dynarec.
--   2. Debug -> Lua editor, paste this file and confirm that it reports the
--      breakpoint installed and the table check passed.
--   3. Go through as much varied text as you can: a campaign conversation
--      with a portrait, a choice prompt, a card shop, a duel's opening and
--      result texts, the Library and the Password screen.
--   4. The trace prints after 64 rows, once every unnamed opcode has been
--      seen, or after five quiet minutes without a new row.
--   5. Copy the whole document into
--      tools/trace/result/text_secondary_commands.txt and fill in context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   For each row of an unnamed opcode, say which text was on screen and
--   what happened at that point of the text: a pause, a sound, a colour or
--   size change, a portrait or picture appearing or moving, a screen shake,
--   inserted text such as a name or number, or nothing visible.

local ffi = require('ffi')

local SCRIPT_NAME = 'text_secondary_commands'

-- Breakpoints fire only while the debugger setting is on and the interpreter
-- CPU is running. PCSX-Redux reads the debugger flag live, so a script can
-- switch it on itself; the dynarec only changes after a restart, so it can
-- only be reported. Runs before any breakpoint is installed.
local TRACE_SETUP = (function()
    local okSettings, emulator = pcall(function()
        return PCSX.settings.emulator
    end)
    if not okSettings or emulator == nil then
        return 'unknown: PCSX.settings unavailable; enable the debugger and '
            .. 'select the interpreter CPU by hand'
    end
    local debugger = 'unknown'
    local okDebug, debugOn = pcall(function() return emulator.Debug.Debug end)
    if okDebug and debugOn == true then
        debugger = 'on'
    elseif okDebug and debugOn == false then
        pcall(function() emulator.Debug.Debug = true end)
        local _, now = pcall(function() return emulator.Debug.Debug end)
        debugger = now == true and 'enabled-by-script'
            or 'OFF: enable Configuration -> Emulation -> Debugger'
    end
    local dynarec = 'unknown'
    local okDynarec, dynarecOn = pcall(function() return emulator.Dynarec end)
    if okDynarec and dynarecOn == false then
        dynarec = 'off'
    elseif okDynarec and dynarecOn == true then
        dynarec = 'ON: breakpoints will not fire; select the interpreter CPU '
            .. 'and restart the emulator'
    end
    return 'debugger=' .. debugger .. ' dynarec=' .. dynarec
end)()
print(SCRIPT_NAME .. ': setup ' .. TRACE_SETUP)

local RAM_BASE = 0x80000000
local RAM_END = 0x80200000

local DISPATCH = 0x80038b4c
local DISPATCH_WORD = 0x27bdffe8
local TABLE = 0x80090eac
local MAIN_MODE = 0x8009b26c
local STREAM_SLOTS = 22
local STREAM_INDEX = 0x58
local STRING_ID = 0x36
local OPERAND_BYTES = 5

local HANDLERS = {
    [0x00] = {0x80037da4, 'func_80037DA4'},
    [0x01] = {0x800380d4, 'func_800380D4'},
    [0x02] = {0x80038110, 'func_80038110'},
    [0x03] = {0x80038148, 'func_80038148'},
    [0x04] = {0x800382a8, 'func_800382A8'},
    [0x05] = {0x80038334, 'func_80038334'},
    [0x06] = {0x80038388, 'Text_SetPenX'},
    [0x07] = {0x800383b0, 'Text_SetOutputLimit'},
    [0x08] = {0x80038070, 'func_80038070'},
    [0x09] = {0x800383dc, 'func_800383DC'},
    [0x0a] = {0x80038498, 'Text_SetColorIndex'},
    [0x0b] = {0x80038094, 'func_80038094'},
    [0x0c] = {0x800384e4, 'Text_SetFastPrintMode'},
    [0x0d] = {0x80038530, 'Text_StartCampaignDuel'},
    [0x0e] = {0x80038690, 'DuelEffect_PlayBgmCommand'},
    [0x0f] = {0x800386b8, 'DuelEffect_ProcessBgmCommand'},
    [0x10] = {0x80038798, 'DuelEffect_PlaySoundCommand'},
    [0x11] = {0x80038800, 'DuelEffect_FadeOutBgmCommand'},
    [0x12] = {0x80038888, 'func_80038888'},
    [0x13] = {0x80038890, 'func_80038890'},
    [0x14] = {0x80038898, 'func_80038898'},
    [0x15] = {0x800388d8, 'Text_ApplyFadeCommand'},
    [0x16] = {0x800389c4, 'func_800389C4'},
    [0x17] = {0x800389d8, 'func_800389D8'},
    [0x18] = {0x80038a44, 'func_80038A44'},
    [0x19] = {0x80038ab0, 'Text_UnlockDuelist'},
    [0x1a] = {0x80038b08, 'Text_CloseChoice'},
    [0x21] = {0x80038eb0, 'Text_HandleDisplayEffectCommand'},
    [0x22] = {0x80037d6c, 'Text_SetStateFromStream'},
    [0x23] = {0x80038b4c, 'Text_DispatchSecondaryCommand'},
    [0x24] = {0x80038d2c, 'Text_HandleCampaignFlagCommand'},
    [0x25] = {0x80038d14, 'Text_StartPageWait'},
    [0x26] = {0x80038bf0, 'Text_HandleChoiceCommand'},
    [0x27] = {0x80038db8, 'Text_PushStreamOffset'},
    [0x28] = {0x80038ba8, 'Text_SetCursorOffset'},
    [0x29] = {0x80038e1c, 'Text_NewLine'},
    [0x2a] = {0x80038e7c, 'Text_EndStream'},
    [0x2b] = {0x80039be0, 'func_80039BE0'},
    [0x2c] = {0x80039afc, 'func_80039AFC'},
    [0x2d] = {0x80039be0, 'func_80039BE0'},
    [0x2e] = {0x80039c94, 'func_80039C94'},
}
for op = 0x1b, 0x20 do HANDLERS[op] = {0x80037d2c, 'Text_ExtendGlyphCode'} end
local OPCODE_COUNT = 0x2f

local NAMED_ROWS = 1
local UNNAMED_ROWS = 4
local MAX_ROWS = 64
local QUIET_FRAMES = 18000
local NO_HIT_WARNING_FRAMES = 36000
local TIMEOUT_FRAMES = 432000

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function u16(addr)
    if not inRam(addr, 2) then return nil end
    return tonumber(ffi.cast('uint16_t*', mem + (addr - RAM_BASE))[0])
end
local function u32(addr)
    if not inRam(addr, 4) then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local function show(v, fmt)
    if v == nil then return 'unreadable' end
    return string.format(fmt or '%d', v)
end

local function unnamed(op)
    local h = HANDLERS[op]
    return h ~= nil and h[2]:sub(1, 5) == 'func_'
end

local UNNAMED_TOTAL = 0
for op = 0, OPCODE_COUNT - 1 do
    if unnamed(op) then UNNAMED_TOTAL = UNNAMED_TOTAL + 1 end
end

local frames = 0
local lastRowFrame = nil
local rows = {}
local counts = {}
local logged = {}
local seenUnnamed = 0
local implausible = 0
local tableProblems = {}
local callbackError = nil
local done = false

local function finish(reason)
    if done then return end
    done = true
    if breakpoint_text_secondary_commands ~= nil then
        breakpoint_text_secondary_commands:disable()
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<for each row of an unnamed (func_) opcode: which text was on')
    print(' screen and what happened at that point -- pause, sound, colour or')
    print(' size change, portrait or picture moving, shake, inserted name or')
    print(' number, or nothing visible>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP)
    print('status: ' .. reason)
    print(string.format('frames: %d rows: %d implausible: %d '
                        .. 'unnamed_seen: %d/%d', frames, #rows,
                        implausible, seenUnnamed, UNNAMED_TOTAL))
    for _, p in ipairs(tableProblems) do print('table_mismatch: ' .. p) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    local ops = {}
    for op in pairs(counts) do ops[#ops + 1] = op end
    table.sort(ops)
    for _, op in ipairs(ops) do
        local h = HANDLERS[op]
        print(string.format('count op=0x%02X %-32s %d', op,
                            h and h[2] or 'OUT_OF_TABLE', counts[op]))
    end
    local missing = {}
    for op = 0, OPCODE_COUNT - 1 do
        if unnamed(op) and counts[op] == nil then
            missing[#missing + 1] = string.format('0x%02X', op)
        end
    end
    print('unnamed_not_seen: ' .. (#missing > 0 and table.concat(missing, ' ')
                                   or 'none'))
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

-- Every append goes through here, so the cap holds even when several
-- dispatches land between two vsyncs.
local function addRow(text)
    if #rows >= MAX_ROWS then return false end
    rows[#rows + 1] = text
    lastRowFrame = frames
    return true
end

local function onDispatch()
    if done then return end
    local regs = PCSX.getRegisters()
    local object = regs.GPR.n.a0
    local caller = regs.GPR.n.ra
    if object % 4 ~= 0 or not inRam(object, STREAM_INDEX + 1) then
        implausible = implausible + 1
        addRow(string.format('IMPLAUSIBLE object=0x%08X', object))
        return
    end
    local index = u8(object + STREAM_INDEX)
    if index >= 0x80 then index = index - 0x100 end
    if index < 0 or index >= STREAM_SLOTS then
        implausible = implausible + 1
        addRow(string.format('IMPLAUSIBLE object=0x%08X stream=%d', object,
                             index))
        return
    end
    local cursor = u32(object + index * 4)
    local op = cursor and u8(cursor)
    if op == nil then
        implausible = implausible + 1
        addRow(string.format('IMPLAUSIBLE object=0x%08X cursor=%s', object,
                             show(cursor, '0x%08X')))
        return
    end

    counts[op] = (counts[op] or 0) + 1
    local isUnnamed = unnamed(op)
    if isUnnamed and counts[op] == 1 then seenUnnamed = seenUnnamed + 1 end
    local limit = isUnnamed and UNNAMED_ROWS or NAMED_ROWS
    if (logged[op] or 0) >= limit or #rows >= MAX_ROWS then return end
    logged[op] = (logged[op] or 0) + 1

    local bytes = {}
    for i = 1, OPERAND_BYTES do
        bytes[#bytes + 1] = show(u8(cursor + i), '%02X')
    end
    local mode = u8(MAIN_MODE)
    local h = HANDLERS[op]
    addRow(string.format(
        'row=%02d frame=%06d op=0x%02X %s handler=%s next=%s string=%s '
            .. 'object=0x%08X caller=0x%08X mode=%s',
        #rows + 1, frames, op, isUnnamed and 'UNNAMED' or 'named',
        h and h[2] or 'OUT_OF_TABLE', table.concat(bytes, ' '),
        show(u16(object + STRING_ID), '0x%03X'), object, caller,
        mode and string.format('0x%02X', mode % 32) or 'unreadable'
    ))
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
    elseif #rows >= MAX_ROWS then
        finish('captured ' .. MAX_ROWS .. ' rows')
    elseif seenUnnamed >= UNNAMED_TOTAL then
        finish('every unnamed opcode seen')
    elseif lastRowFrame ~= nil and frames - lastRowFrame >= QUIET_FRAMES then
        finish('five quiet minutes without a new row')
    elseif lastRowFrame == nil and frames >= NO_HIT_WARNING_FRAMES then
        finish('no F8 command in ten minutes; select the interpreter CPU '
               .. 'and open a conversation')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

-- The table is retail data; check all 47 entries before trusting the labels
-- printed beside each row.
for op = 0, OPCODE_COUNT - 1 do
    local word = u32(TABLE + op * 4)
    local h = HANDLERS[op]
    if h ~= nil and word ~= h[1] then
        tableProblems[#tableProblems + 1] = string.format(
            'op=0x%02X expected 0x%08X read %s', op, h[1], show(word, '0x%08X'))
    end
end
local dispatchWord = u32(DISPATCH)
if dispatchWord ~= DISPATCH_WORD then
    tableProblems[#tableProblems + 1] = string.format(
        'dispatcher 0x%08X expected 0x%08X read %s', DISPATCH, DISPATCH_WORD,
        show(dispatchWord, '0x%08X'))
end

breakpoint_text_secondary_commands = PCSX.addBreakpoint(
    DISPATCH, 'Exec', 4, SCRIPT_NAME .. ': F8 secondary dispatch',
    function()
        local ok, err = pcall(onDispatch)
        if not ok then callbackError = tostring(err) end
    end
)

listener_text_secondary_commands = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end
)

print(string.format('%s: breakpoint installed; table check %s',
                    SCRIPT_NAME, #tableProblems == 0 and 'passed'
                    or ('FAILED (' .. #tableProblems .. ')')))
