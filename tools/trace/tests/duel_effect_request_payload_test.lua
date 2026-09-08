-- Run from the repository root:
--   luajit tools/trace/tests/duel_effect_request_payload_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
--
-- The point of this test is the guards. A trace script that dereferences a bad
-- pointer takes the emulator down and the user's session with it, so the cases
-- that matter most here are the ones where the record is 0, unaligned or out
-- of RAM, and the case where the pool hands the same record to someone else
-- before the payload is read back.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/duel_effect_request_payload.lua'

local ALLOC_RETURN = 0x8002c6c0
local MAIN_MODE = 0x8009b26c
local RAM_BASE = 0x80000000

local function capture()
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { v0 = 0, ra = 0 } } }
    local events = {}
    local breakpoints = {}
    local output = {}

    print = function(text)
        output[#output + 1] = tostring(text)
    end
    PCSX = {
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = { enabled = true, callback = callback }
            function handle:disable() self.enabled = false end
            function handle:remove() self.enabled = false end
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
    breakpoint_duel_effect_request_payload = nil
    listener_duel_effect_request_payload = nil
    dofile(scriptPath)

    local result = {}

    local function u16at(addr) return ffi.cast('uint16_t*', memory + (addr - RAM_BASE)) end
    local function u8at(addr) return ffi.cast('uint8_t*', memory + (addr - RAM_BASE)) end

    function result:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function result:frames(count)
        for _ = 1, count do events['GPU::Vsync']() end
    end
    -- Rows are printed once, at the end, so every case has to drive the
    -- capture to a stop before it can assert on them.
    function result:run()
        for _ = 1, 36000 do events['GPU::Vsync']() end
    end
    function result:mode(value)
        u8at(MAIN_MODE)[0] = value
    end
    -- Hand out a record the way func_8002C68C does: id and flags already
    -- written by func_8002C604, payload still zero.
    function result:allocate(record, id, caller)
        if record ~= 0 and record >= RAM_BASE and record < RAM_BASE + 0x200000 then
            u16at(record + 0x18)[0] = id
            u8at(record + 0x1c)[0] = 0x80
        end
        registers.GPR.n.v0 = record
        registers.GPR.n.ra = caller
        assert(breakpoints[ALLOC_RETURN].enabled)
        breakpoints[ALLOC_RETURN].callback()
    end
    -- What a handler does after the call returns.
    function result:fill(record, a, b, c)
        u16at(record + 0x00)[0] = a % 0x10000
        u16at(record + 0x02)[0] = b % 0x10000
        u16at(record + 0x04)[0] = c % 0x10000
    end
    function result:reassign(record, id)
        u16at(record + 0x18)[0] = id
    end
    function result:disabled()
        return not breakpoints[ALLOC_RETURN].enabled
    end
    return result
end

-- The case the script exists for: id 0x12 writes a pair, the card ids write a
-- triple, and both must come out as readable rows.
local normal = capture()
normal:mode(0x0a)
normal:allocate(0x800eb400, 0x12, 0x80025f10)
normal:fill(0x800eb400, 0xa0, 0x78, 0)
normal:allocate(0x800eb420, 0x0b, 0x800182f0)
normal:fill(0x800eb420, 100, -24, 80)
normal:run()
assert(normal:has('id=0x12'), 'the pair-writing id must be reported')
assert(normal:has('+0x00=160 +0x02=120 +0x04=0'), 'payload must be read back')
assert(normal:has('id=0x0B'), 'the triple-writing id must be reported')
assert(normal:has('+0x00=100 +0x02=-24 +0x04=80'), 'negatives must be signed')
assert(normal:has('caller=0x80025F10'), 'the caller identifies the effect')
assert(not normal:has('RECYCLED'), 'a settled record is not recycled')

-- The settle window is one frame, which assumes the handler fills the record
-- before the next vsync -- true for every caller in matching C, since each
-- writes the payload on the statements after the call. Pin the boundary so
-- that assumption is visible rather than implied: a record filled only after
-- its settle frame reads back as zeros, which is what a wrong assumption would
-- look like in a result.
local late = capture()
late:allocate(0x800eb400, 0x08, 0x8003a000)
late:frames(2)
late:fill(0x800eb400, 1, 2, 3)
late:run()
assert(late:has('+0x00=0 +0x02=0 +0x04=0'),
       'a fill after the settle frame is missed, and must read as zeros')
assert(not late:has('+0x00=1'), 'the row is not re-read later')

-- func_8002C68C returns 0 when the pool is full. Real outcome, no dereference.
local full = capture()
full:allocate(0, 0, 0x80025f10)
full:run()
assert(full:has('POOL_FULL'), 'a full pool must be reported, not dereferenced')

-- Guards. None of these may read through the pointer.
local bad = capture()
bad:allocate(0x00001234, 0, 0x80025f10)
bad:allocate(0x800eb401, 0, 0x80025f10)
bad:allocate(0x9fffffff, 0, 0x80025f10)
bad:run()
assert(bad:has('IMPLAUSIBLE record=0x00001234'), 'below RAM must be rejected')
assert(bad:has('IMPLAUSIBLE record=0x800EB401'), 'unaligned must be rejected')
assert(bad:has('IMPLAUSIBLE record=0x9FFFFFFF'), 'above RAM must be rejected')

-- The pool can hand the record to someone else before we look at it. Such a
-- row is not evidence and has to say so.
local recycled = capture()
recycled:allocate(0x800eb400, 0x12, 0x80025f10)
recycled:reassign(0x800eb400, 0x17)
recycled:run()
assert(recycled:has('RECYCLED'), 'a reused record must be flagged')

-- Bounded output, and the breakpoint goes away when it is.
local limited = capture()
for i = 1, 64 do
    limited:allocate(0x800eb400 + i * 0x20, 0x05, 0x80025f10)
end
limited:run()
assert(limited:has('status: captured 64 requests'), 'capture must be bounded')
assert(limited:disabled(), 'a finished capture must disable its breakpoint')

-- Nothing at all is a setup failure, and has to say which one.
local silent = capture()
silent:frames(5400)
assert(silent:has('status: no request seen'), 'silence must be explained')
assert(silent:disabled())

print = hostPrint
print('duel_effect_request_payload: all seven callback-replay cases passed')
