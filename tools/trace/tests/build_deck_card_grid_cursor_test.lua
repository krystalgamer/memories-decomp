-- Run from the repository root:
--   luajit tools/trace/tests/build_deck_card_grid_cursor_test.lua
-- Replays Build Deck cursor samples with mock PCSX events and real FFI RAM.
local ffi = require('ffi')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/build_deck_card_grid_cursor.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local BUILD_DECK_ROOT_POINTER = 0x8009b2fc
local VIEWPORT_X = 0x8009b146
local LIBRARY_COLUMN = 0x8009b258
local LIBRARY_ROW = 0x8009b259
local LIBRARY_SELECTED_CARD = 0x8009b338
local PAD1_REPEAT = 0x8009b394
local PAD1_PRESSED = 0x8009b398
local PAD1_HELD = 0x8009b3a4
local ROOT = 0x80100000
local ROOT_STATE_OFFSET = 0x633e
local ROOT_NEXT_STATE_OFFSET = 0x6340
local ROOT_PANE_OFFSET = 0x6342
local PANE_STRIDE = 0x2d4c
local FIRST_LIST_OFFSET = 4
local LIST_FIRST_OFFSET = 0x2d3c
local LIST_FIRST_TARGET_OFFSET = 0x2d3e
local LIST_ROW_COUNT_OFFSET = 0x2d40
local LIST_CURSOR_OFFSET = 0x2d48
local ENTRY_SIZE = 0x10

local function capture(mode, root)
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local events = {}
    local output = {}

    local function byte(addr)
        return ffi.cast('uint8_t*', memory + (addr - RAM_BASE))
    end

    local function set8(addr, value)
        byte(addr)[0] = value % 0x100
    end

    local function set16(addr, value)
        set8(addr, value)
        set8(addr + 1, math.floor(value / 0x100))
    end

    local function set32(addr, value)
        set16(addr, value % 0x10000)
        set16(addr + 2, math.floor(value / 0x10000))
    end

    local function listAddress(pane)
        return ROOT + FIRST_LIST_OFFSET + pane * PANE_STRIDE
    end

    local function entryAddress(pane, index)
        return listAddress(pane) + index * ENTRY_SIZE
    end

    local function initializeList(pane, baseCard)
        local list = listAddress(pane)
        set16(list + LIST_FIRST_OFFSET, 0)
        set16(list + LIST_FIRST_TARGET_OFFSET, 0)
        set16(list + LIST_ROW_COUNT_OFFSET, 20)
        set8(list + LIST_CURSOR_OFFSET, 0)
        for index = 0, 19 do
            local entry = entryAddress(pane, index)
            set16(entry + 4, baseCard + index)
            set8(entry + 0x0d, 1)
        end
    end

    set8(MAIN_MODE, mode or 7)
    set32(BUILD_DECK_ROOT_POINTER, root == nil and ROOT or root)
    if root == nil or root == ROOT then
        set16(ROOT + ROOT_STATE_OFFSET, 0)
        set16(ROOT + ROOT_NEXT_STATE_OFFSET, 0)
        set8(ROOT + ROOT_PANE_OFFSET, 0)
        initializeList(0, 1)
        initializeList(1, 101)
    end
    set16(VIEWPORT_X, 0)
    set8(LIBRARY_COLUMN, 3)
    set8(LIBRARY_ROW, 4)
    set16(LIBRARY_SELECTED_CARD, 55)

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
        Events = {
            createEventListener = function(name, callback)
                events[name] = callback
                return {}
            end,
        },
    }
    listener_build_deck_card_grid_cursor_reset = nil
    listener_build_deck_card_grid_cursor = nil
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

    function result:set8(addr, value)
        set8(addr, value)
    end

    function result:set16(addr, value)
        set16(addr, value)
    end

    function result:mode(value)
        set8(MAIN_MODE, value)
    end

    function result:pane(value)
        set8(ROOT + ROOT_PANE_OFFSET, value)
    end

    function result:list(pane, first, target, cursor)
        local list = listAddress(pane)
        set16(list + LIST_FIRST_OFFSET, first)
        set16(list + LIST_FIRST_TARGET_OFFSET, target)
        set8(list + LIST_CURSOR_OFFSET, cursor)
    end

    function result:input(held, repeatMask, pressed)
        set16(PAD1_HELD, held or 0)
        set16(PAD1_REPEAT, repeatMask or 0)
        set16(PAD1_PRESSED, pressed or 0)
    end

    function result:frames(count)
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:sample(held, repeatMask, pressed, mutate)
        self:input(held, repeatMask, pressed)
        self:frames(1)
        if mutate ~= nil then
            mutate(self)
        end
        self:input(0, 0, 0)
        self:frames(40)
    end

    function result:reset()
        events['ExecutionFlow::Reset']()
    end

    return result
end

local normal = capture()
normal:sample(0, 0x4000, 0, function(r)
    r:list(0, 0, 0, 1)
end)
normal:sample(0, 0x1000, 0, function(r)
    r:list(0, 0, 0, 0)
end)
normal:sample(0x8, 0, 0, function(r)
    r:list(0, 8, 8, 0)
end)
normal:sample(0x4, 0, 0, function(r)
    r:list(0, 0, 0, 0)
end)
normal:sample(0x8000, 0, 0, function(r)
    r:pane(1)
    r:set16(VIEWPORT_X, 320)
end)
normal:sample(0x2000, 0, 0, function(r)
    r:pane(0)
    r:set16(VIEWPORT_X, 0)
end)
normal:frames(109)
assert(not normal:has('status: captured six inputs followed by quiet time'))
normal:frames(1)
assert(normal:has('status: captured six inputs followed by quiet time'))
assert(normal:has('samples=6 local_control_changed=true'))
assert(normal:has('grid_coordinates_changed=false'))
assert(normal:has('selected_card_global_changed=false'))
assert(normal:has('repeat=0x4000'))
assert(normal:has('held=0x0008'))
assert(normal:has('pane=0->1'))
assert(normal:has('active_card=1->2'))
assert(normal:has('library_column=3->3 library_row=4->4'))

local globalsChanged = capture()
globalsChanged:input(0, 0x4000, 0)
globalsChanged:frames(1)
globalsChanged:set8(LIBRARY_COLUMN, 4)
globalsChanged:set16(LIBRARY_SELECTED_CARD, 56)
globalsChanged:input(0, 0, 0)
globalsChanged:frames(40)
globalsChanged:mode(8)
globalsChanged:frames(1)
assert(globalsChanged:has('grid_coordinates_changed=true'))
assert(globalsChanged:has('selected_card_global_changed=true'))

local leftMode = capture()
leftMode:input(0, 0, 0x20)
leftMode:frames(1)
leftMode:mode(8)
leftMode:input(0, 0, 0)
leftMode:frames(1)
assert(leftMode:has('status: captured input that left Build Deck'))

local unsettled = capture()
unsettled:input(0, 0x4000, 0)
unsettled:frames(1)
unsettled:input(0, 0, 0)
for frame = 1, 240 do
    unsettled:set16(
        ROOT + FIRST_LIST_OFFSET + LIST_FIRST_TARGET_OFFSET,
        frame % 2
    )
    unsettled:frames(1)
end
unsettled:reset()
assert(unsettled:has('reason=settlement timeout'))

local invalidRoot = capture(7, 0)
assert(invalidRoot:has('invalid Build Deck root pointer 0x00000000'))

local badMode = capture(8)
assert(badMode:has('main mode 8 is not Build Deck mode 7'))

local reset = capture()
reset:reset()
assert(reset:has('status: reset observed; rerun from Build Deck'))

local silent = capture()
silent:frames(600)
assert(silent:has(
    'no input captured yet; perform the documented sequence'
))
silent:frames(35400)
assert(silent:has('status: timed out after a partial Build Deck cursor trace'))

print = hostPrint
print('build_deck_card_grid_cursor: all eight polling cases passed')
