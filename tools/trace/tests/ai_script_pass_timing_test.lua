-- Run from the repository root:
--   luajit tools/trace/tests/ai_script_pass_timing_test.lua
-- Replays the VM breakpoints and VSync listener with mock PCSX callbacks and
-- real LuaJIT FFI RAM.
local ffi = require('ffi')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/ai_script_pass_timing.lua'

local RAM_BASE = 0x80000000
local VM_ENTRY = 0x80070650
local VM_DISPATCH = 0x800706a8
local VM_TIMING_RETURN = 0x800706e0
local VM_EXIT = 0x800706f0
local PREVIOUS_CURSOR = 0x800f5bf4
local MAIN_MODE = 0x8009b26c
local OPPONENT_ID = 0x8009b361
local SCRIPT_BASE = 0x801a8000

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = {GPR = {n = {ra = 0, v0 = 0}}}
    local events = {}
    local breakpoints = {}
    local output = {}
    local scriptOffset = 0

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
        value = value % 0x100000000
        set16(addr, value % 0x10000)
        set16(addr + 2, math.floor(value / 0x10000))
    end

    set32(VM_ENTRY, options.entrySignature or 0x27bdffd8)
    set32(VM_DISPATCH, options.dispatchSignature or 0x0040f809)
    set32(VM_TIMING_RETURN, options.timingSignature or 0x284200f0)
    set32(VM_EXIT, options.exitSignature or 0x8fbf0024)
    set8(MAIN_MODE, 3)
    set8(OPPONENT_ID, 21)

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
        getRegisters = function()
            return registers
        end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = {
                enabled = true,
                callback = callback,
                kind = kind,
                size = size,
                label = label,
            }
            function handle:disable()
                self.enabled = false
            end
            breakpoints[address] = handle
            return handle
        end,
        Events = {
            createEventListener = function(name, callback)
                events[name] = callback
                return {}
            end,
        },
    }
    breakpoint_ai_script_pass_entry = nil
    breakpoint_ai_script_pass_dispatch = nil
    breakpoint_ai_script_pass_timing = nil
    breakpoint_ai_script_pass_exit = nil
    listener_ai_script_pass_timing_reset = nil
    listener_ai_script_pass_timing = nil
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

    function result:call(address)
        assert(breakpoints[address] ~= nil, 'breakpoint must be installed')
        assert(breakpoints[address].enabled, 'breakpoint must be enabled')
        breakpoints[address].callback()
    end

    function result:enter(site)
        registers.GPR.n.ra = (site or 0x80071400) + 8
        self:call(VM_ENTRY)
    end

    function result:command(opcode, handler, cursor)
        if cursor == nil then
            cursor = SCRIPT_BASE + scriptOffset
            scriptOffset = scriptOffset + 1
        end
        if cursor >= RAM_BASE and cursor < RAM_BASE + 0x200000 then
            set8(cursor, opcode or 0)
        end
        set32(PREVIOUS_CURSOR, cursor)
        registers.GPR.n.v0 = handler or 0x80070b00
        self:call(VM_DISPATCH)
    end

    function result:timing(value)
        registers.GPR.n.v0 = value
        self:call(VM_TIMING_RETURN)
    end

    function result:exit(value)
        registers.GPR.n.v0 = value
        self:call(VM_EXIT)
    end

    function result:frames(count)
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:reset()
        events['ExecutionFlow::Reset']()
    end

    function result:entrySignature(value)
        set32(VM_ENTRY, value)
    end

    function result:disabled()
        for _, address in ipairs({
            VM_ENTRY, VM_DISPATCH, VM_TIMING_RETURN, VM_EXIT
        }) do
            if breakpoints[address] == nil
                or breakpoints[address].enabled then
                return false
            end
        end
        return true
    end

    return result
end

local normal = capture()
assert(normal:has('four breakpoints installed'))
normal:enter(0x80071400)
normal:command(0x01, 0x80070b00)
normal:timing(10)
normal:command(0x02, 0x80070b40)
normal:timing(240)
normal:exit(0)
normal:enter()
normal:command(0x03, 0x80070bf8)
normal:exit(1)
normal:enter()
normal:command(0x04, 0x80071000)
normal:exit(2)
normal:enter()
normal:command(0x05, 0x80071020)
normal:exit(3)
for opcode = 6, 8 do
    normal:enter()
    normal:command(opcode, 0x80071000 + opcode * 4)
    normal:timing(240)
    normal:exit(0)
end
assert(not normal:has('status: captured eight AI interpreter passes'))
normal:enter()
normal:command(0x09, 0x80071024)
normal:timing(240)
normal:exit(0)
assert(normal:has('status: captured eight AI interpreter passes'))
assert(normal:has(
    'pass=01 status=complete entry_frame=000000 exit_frame=000000'
))
assert(normal:has('result=0(yield) commands=2 timing_queries=2'))
assert(normal:has('vsync_min=10 vsync_max=240 vsync_last=240'))
assert(normal:has('yield_after_command=2'))
assert(normal:has('opcodes=01@80070B00,02@80070B40'))
assert(normal:has('result=1(end_hand) commands=1 timing_queries=0'))
assert(normal:has('result=2(play_field_card)'))
assert(normal:has('result=3(end_field)'))
assert(normal:has('passes_started=8 passes_completed=8'))
assert(normal:disabled())

local invalidCursor = capture()
invalidCursor:enter()
invalidCursor:command(0x10, 0x80071234, 0)
invalidCursor:command(0x11, 0x80071238, 0x7fffffff)
invalidCursor:command(0x12, 0x8007123c, 0x80200000)
invalidCursor:command(0x13, 0x80071240, SCRIPT_BASE + 1)
invalidCursor:timing(240)
invalidCursor:exit(0)
invalidCursor:reset()
assert(invalidCursor:has('status: reset observed; rerun during an opponent turn'))
assert(invalidCursor:has('invalid_cursors=3'))
assert(invalidCursor:has('opcodes=invalid(0x00000000)@80071234'))
assert(invalidCursor:has('13@80071240'))
assert(invalidCursor:disabled())

local orphaned = capture()
orphaned:command(1, 0x80070b00)
orphaned:timing(3)
orphaned:exit(0)
orphaned:reset()
assert(orphaned:has('orphan_dispatches=1'))
assert(orphaned:has('orphan_timing_queries=1'))
assert(orphaned:has('orphan_exits=1'))

local silent = capture()
silent:frames(599)
assert(not silent:has('no AI pass seen'))
silent:frames(1)
assert(silent:has('no AI pass seen'))
silent:frames(35400)
assert(silent:has(
    'status: timed out before eight completed AI interpreter passes'
))
assert(silent:disabled())

local wrongSignature = capture({entrySignature = 0x12345678})
assert(wrongSignature:has(
    'status: resident AI VM signatures do not match retail'
))

local replaced = capture()
replaced:enter()
replaced:command(1, 0x80070b00)
replaced:entrySignature(0)
replaced:frames(1)
assert(replaced:has(
    'status: resident AI VM signature changed during capture'
))
assert(replaced:has('pass=01 status=partial'))
assert(replaced:disabled())

local limited = capture()
limited:enter()
for opcode = 1, 512 do
    limited:command(opcode % 0x44, 0x80070b00)
end
limited:frames(1)
assert(limited:has(
    'status: maximum command count reached; partial trace follows'
))
assert(limited:has('commands=512'))
assert(limited:has('opcodes='))
assert(limited:has(',...'))
assert(limited:disabled())

print = hostPrint
print('ai_script_pass_timing: all seven callback-replay cases passed')
