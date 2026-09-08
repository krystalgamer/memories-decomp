-- duel_effect_request_payload.lua
--
-- WHAT THIS ANSWERS
--   src/game/duel_effect_request.h describes the record func_8002C68C hands a
--   duel-effect handler. Its layout came from func_8002C604, which fills every
--   field, so the record itself is settled. What is not settled is the three
--   payload halfwords at +0x00, +0x02 and +0x04, and the header says so: their
--   meaning belongs to the effect id, and the callers contradict each other.
--
--     func_80025EE0 (id 0x12) writes 0xA0 and 0x78 into +0x00 and +0x02,
--       which is the centre of a 320x240 screen, so for that effect they look
--       like an x/y pair.
--     func_8001825C (id 0x0B) and duel_trap_resolution.c (id 0x08) copy a card
--       record's +0x30/+0x32/+0x34 triple into all three. In a display object
--       that triple is x, z, y -- not x, y, z.
--     func_80026A3C (id 0x17) reads +0x04 back as a y coordinate, which agrees
--       with the card-copy reading and not with the first one.
--
--   Both readings cannot be right, and no amount of static reading separates
--   them, because each caller is self-consistent. What separates them is where
--   the effect actually appears on screen.
--
--   This trace records, for every request allocated during a duel, the effect
--   id, the three payload halfwords and +0x1A, and the caller that asked for
--   it. The human says which effect appeared and where. If id 0x12's effect
--   appears at the centre of the screen then +0x00 and +0x02 are x and y; if it
--   appears at the top-centre then +0x02 is a depth and +0x04 is y, and the
--   card-copy path is the one that reads correctly.
--
--   The by-product is worth as much: the id-to-effect mapping. Ten ids reach
--   func_8002C68C from matching C -- 5, 6, 7, 8, 9, 0x0B, 0x0F, 0x11, 0x12 and
--   0x17 -- and only some of them have a named effect in the source comments.
--
--   Naming the three fields is the point. Nothing else in the tree can do it.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger.
--   2. Select the interpreter CPU. Execution breakpoints do not fire on the
--      dynarec.
--   3. Debug -> Lua editor, paste this file, and let it run. Confirm that it
--      prints "breakpoint installed".
--   4. Start a duel and play until the trace prints. Deliberately provoke
--      effects rather than attacking normally: play a magic card, play a trap,
--      let a trap resolve against you, summon a ritual, and take direct damage.
--      The more distinct effects, the more ids get identified.
--   5. For every effect you see, note what it was and **where on the screen it
--      appeared** -- centre, top-centre, or over a particular field slot.
--   6. After the trace prints, copy the whole document into
--      tools/trace/result/duel_effect_request_payload.txt and fill in the
--      context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   Confirm the interpreter CPU. For each row in the trace, in order, say what
--   effect appeared and where on screen it was drawn. If two rows produced one
--   visible effect, say so. If an effect you provoked produced no row, say
--   that too -- a negative is evidence about which ids this path serves.
--   Screen coordinates are 320x240 with the origin at the top left, so
--   "centre" is roughly x=160 y=120.

local ffi = require('ffi')

local SCRIPT_NAME = 'duel_effect_request_payload'

-- The `jr $ra` of func_8002C68C. Chosen over the entry so that $v0 already
-- holds the record: the id is then read out of the record's own +0x18 rather
-- than out of $a0, which keeps the row self-consistent even if the argument
-- register is reused.
local ALLOC_RETURN = 0x8002c6c0
local MAIN_MODE = 0x8009b26c

local RAM_BASE = 0x80000000
local RAM_END = 0x80200000

local RECORD_FIELD_00 = 0x00
local RECORD_FIELD_02 = 0x02
local RECORD_FIELD_04 = 0x04
local RECORD_ID = 0x18
local RECORD_FIELD_1A = 0x1a
local RECORD_FLAGS = 0x1c
local RECORD_SIZE = 0x1e

local MAX_EVENTS = 64
local SETTLE_FRAMES = 1
local NO_HIT_WARNING_FRAMES = 5400
local TIMEOUT_FRAMES = 36000

local mem = PCSX.getMemPtr()

local hits = 0
local frames = 0
local firstHitFrame = nil
local done = false
local callbackError = nil
local lines = {}
local pending = {}
local idCounts = {}

local function inRam(addr, size)
    return addr >= RAM_BASE and addr + size <= RAM_END
end

local function phys(addr)
    return addr - RAM_BASE
end

-- Every read goes through these. A read outside RAM takes the emulator down
-- rather than raising, so the guard is the whole point.
local function u8(addr)
    if not inRam(addr, 1) then
        return nil
    end
    return tonumber(ffi.cast('uint8_t*', mem + phys(addr))[0])
end

local function u16(addr)
    if not inRam(addr, 2) then
        return nil
    end
    return tonumber(ffi.cast('uint16_t*', mem + phys(addr))[0])
end

local function signed16(value)
    if value == nil then
        return nil
    end
    if value >= 0x8000 then
        return value - 0x10000
    end
    return value
end

local function mainMode()
    local value = u8(MAIN_MODE)
    if value == nil then
        return -1
    end
    return value % 32
end

local function show(value)
    if value == nil then
        return 'unreadable'
    end
    return string.format('%d', value)
end

local function finish(reason)
    if done then
        return
    end
    done = true

    if breakpoint_duel_effect_request_payload ~= nil then
        breakpoint_duel_effect_request_payload:disable()
    end

    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<for every row below, in order: which effect appeared, and where on')
    print(' the screen it was drawn -- centre, top-centre, or over which field')
    print(' slot. Say if two rows made one visible effect, and name any effect')
    print(' you provoked that produced no row at all.>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('status: ' .. reason)
    print(string.format('summary: requests=%d rows=%d frames=%d',
                        hits, #lines, frames))

    local ids = {}
    for id in pairs(idCounts) do
        ids[#ids + 1] = id
    end
    table.sort(ids)
    local seen = {}
    for _, id in ipairs(ids) do
        seen[#seen + 1] = string.format('0x%02X x%d', id, idCounts[id])
    end
    if #seen > 0 then
        print('ids: ' .. table.concat(seen, ', '))
    else
        print('ids: none')
    end

    if callbackError ~= nil then
        print('callback_error: ' .. callbackError)
    end
    print('')
    for _, line in ipairs(lines) do
        print(line)
    end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

-- Emit one row for a request whose caller has had a frame to fill it in.
local function emit(entry)
    local id = u16(entry.record + RECORD_ID)
    local recycled = ''
    if id ~= entry.id then
        -- The pool handed this record to something else before we looked.
        -- Report the row anyway and say so; a recycled row is not evidence.
        recycled = ' RECYCLED'
    end

    local field00 = signed16(u16(entry.record + RECORD_FIELD_00))
    local field02 = signed16(u16(entry.record + RECORD_FIELD_02))
    local field04 = signed16(u16(entry.record + RECORD_FIELD_04))
    local field1A = signed16(u16(entry.record + RECORD_FIELD_1A))

    lines[#lines + 1] = string.format(
        'request=%02d frame=%06d mode=0x%02X id=0x%02X record=0x%08X '
        .. 'caller=0x%08X +0x00=%s +0x02=%s +0x04=%s +0x1A=%s%s',
        entry.index,
        entry.frame,
        entry.mode,
        entry.id,
        entry.record,
        entry.caller,
        show(field00),
        show(field02),
        show(field04),
        show(field1A),
        recycled
    )
end

local function onAllocate()
    if done then
        return
    end

    local regs = PCSX.getRegisters()
    local record = regs.GPR.n.v0
    local caller = regs.GPR.n.ra

    -- func_8002C68C returns 0 when the pool is full. That is a real outcome
    -- and worth a row, but there is nothing to read.
    if record == 0 then
        hits = hits + 1
        lines[#lines + 1] = string.format(
            'request=%02d frame=%06d mode=0x%02X POOL_FULL caller=0x%08X',
            hits, frames, mainMode(), caller
        )
        return
    end

    if not inRam(record, RECORD_SIZE) or record % 4 ~= 0 then
        hits = hits + 1
        lines[#lines + 1] = string.format(
            'request=%02d frame=%06d IMPLAUSIBLE record=0x%08X caller=0x%08X',
            hits, frames, record, caller
        )
        return
    end

    local id = u16(record + RECORD_ID)
    local flags = u8(record + RECORD_FLAGS)
    if id == nil or flags == nil then
        return
    end

    hits = hits + 1
    if firstHitFrame == nil then
        firstHitFrame = frames
    end
    idCounts[id] = (idCounts[id] or 0) + 1

    pending[#pending + 1] = {
        index = hits,
        frame = frames,
        mode = mainMode(),
        id = id,
        flags = flags,
        record = record,
        caller = caller,
    }
end

local function poll()
    if done then
        return
    end
    frames = frames + 1

    local keep = {}
    for _, entry in ipairs(pending) do
        if frames - entry.frame >= SETTLE_FRAMES then
            emit(entry)
        else
            keep[#keep + 1] = entry
        end
    end
    pending = keep

    if callbackError ~= nil then
        finish('callback error: ' .. callbackError)
        return
    end
    if #lines >= MAX_EVENTS then
        finish('captured ' .. MAX_EVENTS .. ' requests')
        return
    end
    if firstHitFrame == nil and frames >= NO_HIT_WARNING_FRAMES then
        finish('no request seen; start a duel and use interpreter CPU')
        return
    end
    if frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

breakpoint_duel_effect_request_payload = PCSX.addBreakpoint(
    ALLOC_RETURN,
    'Exec',
    4,
    'Capture each duel-effect request as it is handed out',
    function()
        local ok, err = pcall(onAllocate)
        if not ok then
            callbackError = tostring(err)
        end
    end
)

listener_duel_effect_request_payload = PCSX.Events.createEventListener(
    'GPU::Vsync',
    function()
        local ok, err = pcall(poll)
        if not ok then
            finish('script error: ' .. tostring(err))
        end
    end
)

print(SCRIPT_NAME .. ': breakpoint installed; start a duel and provoke effects')
