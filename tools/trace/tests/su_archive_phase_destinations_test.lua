-- Run from the repository root:
--   luajit tools/trace/tests/su_archive_phase_destinations_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
--
-- This verifies callback pairing, record formatting, bounds, and reset
-- recovery. It does not treat mocked destinations as runtime observations.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/su_archive_phase_destinations.lua'

local RAM_BASE = 0x80000000
local LOAD_PACKAGE_STAGE = 0x8005b64c
local LOAD_PACKAGE_STAGE_EPILOGUE = 0x8005b84c
local MAIN_MODE = 0x8009b26c
local STREAM_FLAGS = 0x8009b0f4
local PHASE_VALUE = 0x8001002c

local function capture()
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a0 = 0, a1 = 0, ra = 0 } } }
    local events = {}
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

    u8at(MAIN_MODE)[0] = 0x28

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
            local handle = { enabled = true, callback = callback }
            function handle:disable()
                self.enabled = false
            end
            function handle:remove()
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
    breakpoint_su_archive_phase_entry = nil
    breakpoint_su_archive_phase_exit = nil
    listener_su_archive_phase_reset = nil
    listener_su_archive_phase_shell = nil
    listener_su_archive_phase_destinations = nil
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

    function result:mode(value)
        u8at(MAIN_MODE)[0] = value
    end

    function result:globals(flags, phaseValue)
        u32at(STREAM_FLAGS)[0] = flags
        u32at(PHASE_VALUE)[0] = phaseValue
    end

    function result:record(addr, values)
        u16at(addr + 0x04)[0] = values.field04a % 0x10000
        u16at(addr + 0x06)[0] = values.field04b % 0x10000
        u32at(addr + 0x08)[0] = values.buffer08
        u32at(addr + 0x0c)[0] = values.buffer0c
        u32at(addr + 0x1c)[0] = values.size1c
        u16at(addr + 0x30)[0] = values.field30a % 0x10000
        u16at(addr + 0x32)[0] = values.field30b % 0x10000
        u8at(addr + 0x46)[0] = values.status46
    end

    function result:entry(record, selector, callsite)
        registers.GPR.n.a0 = record
        registers.GPR.n.a1 = selector
        registers.GPR.n.ra = callsite + 8
        local breakpoint = breakpoints[LOAD_PACKAGE_STAGE]
        assert(breakpoint and breakpoint.enabled)
        breakpoint.callback()
    end

    function result:exit()
        local breakpoint = breakpoints[LOAD_PACKAGE_STAGE_EPILOGUE]
        assert(breakpoint and breakpoint.enabled)
        breakpoint.callback()
    end

    function result:frames(count)
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:breakRegisters()
        PCSX.getRegisters = function()
            error('mock register failure')
        end
    end

    function result:reset()
        for _, breakpoint in pairs(breakpoints) do
            breakpoint.enabled = false
        end
        events['ExecutionFlow::Reset']()
    end

    function result:shell()
        events['ExecutionFlow::ShellReached']()
    end

    function result:disabled()
        return not breakpoints[LOAD_PACKAGE_STAGE].enabled
            and not breakpoints[LOAD_PACKAGE_STAGE_EPILOGUE].enabled
    end

    return result
end

local normal = capture()
assert(normal:has('breakpoints installed'))
normal:exit()
for selector = 0, 4 do
    local record = 0x80002000 + selector * 0x80
    normal:record(record, {
        field04a = -1 - selector,
        field04b = 2 + selector,
        buffer08 = 0x80003000 + selector * 0x100,
        buffer0c = 0x80003100 + selector * 0x100,
        size1c = 0x1000 + selector,
        field30a = -3 - selector,
        field30b = 4 + selector,
        status46 = 0xa0 + selector,
    })
    normal:globals(0x11111111 + selector, 0x22222222 + selector)
    normal:entry(record, selector, 0x80050000 + selector * 4)
    normal:record(record, {
        field04a = 10 + selector,
        field04b = -20 - selector,
        buffer08 = 0x80004000 + selector * 0x100,
        buffer0c = 0x80004100 + selector * 0x100,
        size1c = 0x2000 + selector,
        field30a = 30 + selector,
        field30b = -40 - selector,
        status46 = 0xb0 + selector,
    })
    normal:globals(0x33333333 + selector, 0x44444444 + selector)
    normal:exit()
end
normal:frames(180)
assert(normal:has(
    'status: captured selectors 0 through 4 followed by quiet time'
), 'five selectors must finish after the quiet window')
assert(normal:has(
    'summary: hits=5 completed=5 selectors=0=true 1=true 2=true 3=true 4=true'
), 'the summary must retain every selector')
assert(normal:has('orphan_exit frame=0 mode=0x08'),
       'an exit without an entry must be recorded')
assert(normal:has(
    'entry_frame=0 exit_frame=0 mode=0x08 callsite=0x80050000 '
        .. 'record=0x80002000'
), 'paired callbacks must retain timing and caller context')
assert(normal:has(
    'before flags=0x11111111 phase_value=0x22222222 '
        .. 'field04=(-1,2) buffers08=(0x80003000,0x80003100) '
        .. 'size1C=0x00001000 field30=(-3,4) status46=0xA0'
), 'the entry snapshot must preserve signed and pointer fields')
assert(normal:has(
    'after  flags=0x33333333 phase_value=0x44444444 '
        .. 'field04=(10,-20) buffers08=(0x80004000,0x80004100) '
        .. 'size1C=0x00002000 field30=(30,-40) status46=0xB0'
), 'the exit snapshot must preserve record mutations')
assert(normal:disabled())

local ordering = capture()
ordering:entry(0x80005000, 5, 0x80051000)
ordering:entry(0, 6, 0x80051004)
ordering:exit()
ordering:entry(0x7ffffff0, 7, 0x80051008)
ordering:frames(72000)
assert(ordering:has('entry_before_previous_exit old_hit=1 old_selector=5'),
       'a new entry must report the overwritten pending callback')
assert(ordering:has('--- hit 2 selector 6 ---'))
assert(ordering:has('invalid_record(0x00000000)'),
       'an invalid record pointer must not be dereferenced')
assert(ordering:has(
    'pending_without_exit hit=3 selector=7 record=0x7FFFFFF0'
), 'timeout must retain an unmatched entry')
assert(ordering:has('status: timed out after a partial SU callback trace'))
assert(ordering:has(
    'summary: hits=3 completed=1 selectors=0=false 1=false 2=false '
        .. '3=false 4=false'
), 'out-of-range selectors must not count as completed stages')
assert(ordering:disabled())

local limited = capture()
limited:record(0x80006000, {
    field04a = 0,
    field04b = 0,
    buffer08 = 0,
    buffer0c = 0,
    size1c = 0,
    field30a = 0,
    field30b = 0,
    status46 = 0,
})
for index = 1, 12 do
    limited:entry(0x80006000, 0, 0x80052000 + index * 4)
    limited:exit()
end
limited:frames(1)
assert(limited:has('status: maximum hit count reached; partial trace follows'))
assert(limited:has(
    'summary: hits=12 completed=12 selectors=0=true 1=false 2=false '
        .. '3=false 4=false'
), 'the hit limit must preserve completed callback counts')
assert(limited:disabled())

local broken = capture()
broken:breakRegisters()
broken:entry(0x80007000, 0, 0x80053000)
broken:frames(1)
assert(broken:has('status: breakpoint callback error:'),
       'callback failures must surface on the next frame')
assert(broken:has('mock register failure'))
assert(broken:disabled())

local reset = capture()
reset:reset()
assert(reset:has('reset observed; waiting for BIOS shell'))
reset:shell()
assert(reset:has('breakpoints reinstalled at BIOS shell'))
reset:frames(600)
assert(reset:has(
    'no callback hits after reset; confirm the BIOS-shell reinstall message'
), 'post-reset silence must print the reinstall diagnosis')
reset:frames(71400)
assert(reset:has(
    'status: timed out without a callback hit; reload SU with interpreter CPU'
), 'post-reset silence must finish explicitly')
assert(reset:disabled())

local silent = capture()
silent:frames(600)
assert(silent:has(
    'OPTION return did not reload SU; leave the script running and reset once'
), 'pre-reset silence must recommend the boot fallback')
silent:frames(71400)
assert(silent:has(
    'status: timed out without a callback hit; reload SU with interpreter CPU'
), 'pre-reset silence must finish explicitly')
assert(silent:disabled())

print = hostPrint
print('su_archive_phase_destinations: all six callback-replay cases passed')
