-- Run from the repository root:
--   luajit tools/trace/tests/duel_result_rewards_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/duel_result_rewards.lua'

local RAM_BASE = 0x80000000
local MAIN_MODE = 0x8009b26c
local RECENT_DROPS = 0x801d07bc
local CARD_CHEST = 0x801d0250
local RESULT_STAGE, REWARD_STAGE = 0x80020be4, 0x80032184
local REQUEST, COMPACT, AWARD = 0x80032328, 0x80032370, 0x80021894
local BUILD_DECK, OUTRO = 0x800323f8, 0x80020f4c
local WORDS = {
    [RESULT_STAGE] = 0x27bdffe0, [REWARD_STAGE] = 0x27bdffe8,
    [REQUEST] = 0x27bdffd8, [COMPACT] = 0x3c02801d, [AWARD] = 0x3c02801d,
    [BUILD_DECK] = 0x27bdffb8, [OUTRO] = 0x27bdffb8,
}

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = { GPR = { n = { a0 = 0, a1 = 0, ra = 0 } } }
    local events, breakpoints, output = {}, {}, {}
    local function u8at(a) return ffi.cast('uint8_t*', memory + (a - RAM_BASE)) end
    local function u16at(a) return ffi.cast('uint16_t*', memory + (a - RAM_BASE)) end
    local function u32at(a) return ffi.cast('uint32_t*', memory + (a - RAM_BASE)) end
    for addr, word in pairs(WORDS) do
        u32at(addr)[0] = options.corrupt == addr and 0 or word
    end
    u8at(MAIN_MODE)[0] = 3

    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        getMemPtr = function() return memory end,
        getRegisters = function() return registers end,
        addBreakpoint = function(address, kind, size, label, callback)
            local handle = { enabled = true, callback = callback }
            function handle:disable() self.enabled = false end
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
    breakpoints_duel_result_rewards = nil
    listener_duel_result_rewards = nil
    dofile(scriptPath)

    local r = {}
    function r:has(text)
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then return true end
        end
        return false
    end
    function r:frames(n) for _ = 1, n do events['GPU::Vsync']() end end
    function r:mode(v) u8at(MAIN_MODE)[0] = v end
    function r:call(addr, a0, a1, ra)
        local n = registers.GPR.n
        n.a0, n.a1, n.ra = a0 or 0, a1 or 0, ra or 0
        if breakpoints[addr].enabled then breakpoints[addr].callback() end
    end
    function r:drops(list)
        for i = 0, 15 do u16at(RECENT_DROPS + i * 2)[0] = list[i + 1] or 0 end
    end
    function r:chest(id, n) u8at(CARD_CHEST + id - 1)[0] = n end
    function r:descriptor(p, fields)
        for offset, v in pairs(fields.u16 or {}) do u16at(p + offset)[0] = v end
        for offset, v in pairs(fields.u32 or {}) do u32at(p + offset)[0] = v end
    end
    function r:hasBoth(a, b)
        for _, line in ipairs(output) do
            if line:find(a, 1, true) and line:find(b, 1, true) then return true end
        end
        return false
    end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do if bp.enabled then return false end end
        return true
    end
    return r
end

-- The route the script exists for: result stages, an award, then two Build
-- Deck visits that request file 0x2189 and compact the table.
local route = capture()
assert(route:has('7 breakpoints installed, 0 signature mismatches'))
route:call(OUTRO)
route:descriptor(0x800f1000, { u32 = { [0x24] = 0x1a2b, [0x10] = 0x9800 } })
route:call(RESULT_STAGE, 0x800f1000, 0, 0x8002f000)
route:descriptor(0x800f1000, { u16 = { [0x04] = 0x40, [0x06] = 0x10,
    [0x30] = 0, [0x32] = 0x100 }, u32 = { [0x1c] = 0x10000 } })
route:frames(1)
route:drops({ 5, 9 })
route:chest(5, 1)
route:chest(9, 0)
route:call(AWARD, 77, 0, 0x80021a00)
route:drops({ 77, 5, 9 })
route:chest(77, 1)
route:frames(1)
route:mode(7)
route:call(BUILD_DECK)
route:call(REQUEST, 0, 0, 0x80032400)
route:call(REWARD_STAGE, 0x800f2000, 2, 0x8003c000)
route:frames(1)
route:call(COMPACT, 0, 0, 0x80032410)
route:drops({ 77, 5 })
route:frames(1)
route:call(BUILD_DECK)
route:frames(7201)
assert(route:has('status: two Build Deck visits, then two quiet minutes'))
assert(route:has('execution_breakpoint=observed'))
assert(route:has('result-stage func_80020BE4 mode=0 descriptor=0x800F1000 '
                 .. 'caller=0x8002F000 main=0x03'))
assert(route:has('before: rect=(0,0,0,0)'))
assert(route:has('after: rect=(0,0,64,16) vram=(0x0,0x100)'))
assert(route:has('phase_size=0x10000 total=0x9800 lba=0x1A2B'))
assert(route:has('Duel_AwardCard card=77 caller=0x80021A00 main=0x03 '
                 .. 'before=[5:1 9:0 0 0'))
assert(route:has('after=[77:1 5:1 9:0 0'))
assert(route:has('reward-request func_80032328 caller=0x80032400 main=0x07'))
assert(route:has('reward-stage func_80032184 mode=2 descriptor=0x800F2000'))
assert(route:has('compact func_80032370 caller=0x80032410 main=0x07 '
                 .. 'before=[77:1 5:1 9:0 0'))
assert(route:has('after=[77:1 5:1 0 0'))
assert(route:has('build_deck_visits: 2'))
assert(route:allDisabled())

-- Guards: a bad descriptor is reported, not read.
local bad = capture()
bad:call(RESULT_STAGE, 0, 1, 0)
bad:call(RESULT_STAGE, 0x800f1001, 1, 0)
bad:call(RESULT_STAGE, 0x801ffff0, 1, 0)
bad:frames(54000)
assert(bad:has('IMPLAUSIBLE result-stage func_80020BE4 descriptor=0x00000000 mode=1'))
assert(bad:has('IMPLAUSIBLE result-stage func_80020BE4 descriptor=0x800F1001'))
assert(bad:has('IMPLAUSIBLE result-stage func_80020BE4 descriptor=0x801FFFF0'))

-- Stage rows are capped inside the callback; calls keep counting.
local stages = capture()
for i = 1, 40 do stages:call(REWARD_STAGE, 0x800f2000, i % 4, 0) end
stages:frames(54000)
assert(stages:has('row=24 '))
assert(not stages:has('row=25 '))
assert(stages:hasBoth('count reward-stage func_80032184', ' 40'))

-- Overall bound.
local flood = capture()
for _ = 1, 90 do flood:call(REQUEST, 0, 0, 0) end
flood:frames(1)
assert(flood:has('status: captured 80 rows'))
assert(flood:has('rows: 80'))

-- A wrong entry word is named.
local corrupt = capture({ corrupt = COMPACT })
assert(corrupt:has('1 signature mismatches'))

-- No control hit for fifteen minutes is a setup failure.
local silent = capture()
silent:frames(54000)
assert(silent:has('status: fifteen minutes without a control hit'))
assert(silent:has('execution_breakpoint=NOT-observed'))

print = hostPrint
print('duel_result_rewards: all six callback-replay cases passed')
