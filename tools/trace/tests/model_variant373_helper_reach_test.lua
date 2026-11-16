-- Run from the repository root:
--   luajit tools/trace/tests/model_variant373_helper_reach_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local originalPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/model_variant373_helper_reach.lua'

local RAM_BASE = 0x80000000
local SLOTS, SLOT_SIZE = 0x800f2c40, 0xe20
local VARIANT_DATA, MODEL_ID, STAR, PHASE = 0xdec, 0xdf8, 0xdfe, 0x2484
local BASES = {[0] = 0x8013b000, [1] = 0x8017b000}
local HEADERS = {[0] = 356, [1] = 506}
local WORK = {[0] = 0x80100000, [1] = 0x80140000}
local HELPERS = {
    DrawLayers = {0x10a8, 0x27bdfec8}, DrawGrid = {0x1770, 0x27bdfee0},
    DrawPoints = {0x1ee4, 0x27bdfef0}, DrawStrip = {0x2240, 0x27bdff00},
    DrawRibbons = {0x2784, 0x27bdfee0}, DrawQuads = {0x2e50, 0x27bdfee0},
    DrawRays = {0x33d8, 0x27bdfec8},
}

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local gpr = {n = {}, r = {}}
    local registers = {pc = 0, GPR = gpr}
    local events, breakpoints, output = {}, {}, {}
    local function word(address, value)
        ffi.cast('uint32_t*', memory + (address - RAM_BASE))[0] =
            value % 0x100000000
    end
    print = function(text) output[#output + 1] = tostring(text) end
    PCSX = {
        settings = {emulator = {Debug = {Debug = true}, Dynarec = false}},
        getMemPtr = function() return memory end,
        getRegisters = function()
            if options.brokenRegisters then return nil end
            return registers
        end,
        addBreakpoint = function(address, kind, size, label, callback)
            assert(breakpoints[address] == nil)
            assert(kind == 'Exec' and size == 4)
            local handle = {enabled = true, callback = callback}
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
    breakpoints_model_variant373_helper_reach = nil
    listeners_model_variant373_helper_reach = nil
    dofile(scriptPath)

    local r = {output = output}
    function r:has(text) return self:countOf(text) > 0 end
    function r:countOf(text)
        local count = 0
        for _, line in ipairs(output) do
            if line:find(text, 1, true) then count = count + 1 end
        end
        return count
    end
    function r:frames(count)
        for _ = 1, count do events['GPU::Vsync']() end
    end
    function r:reset() events['ExecutionFlow::Reset']() end
    -- Loads Cosmo Queen's image (or another header) into a slot.
    function r:load(slot, header, model, work)
        local base = BASES[slot]
        word(base, header or HEADERS[slot])
        for _, h in pairs(HELPERS) do word(base + h[1], h[2]) end
        local s = SLOTS + slot * SLOT_SIZE
        word(s + VARIANT_DATA, work or WORK[slot])
        ffi.cast('uint16_t*', memory + (s + MODEL_ID - RAM_BASE))[0] =
            model or 707
        memory[s + STAR - RAM_BASE] = 0
    end
    function r:unload(slot) word(BASES[slot], 0) end
    function r:phase(slot, value) word(WORK[slot] + PHASE, value) end
    function r:corrupt(slot, name) word(BASES[slot] + HELPERS[name][1], 0) end
    function r:hit(slot, name, ra, force)
        gpr.n.ra, gpr.n.a0 = ra or 0x8013c000, WORK[slot]
        local bp = assert(breakpoints[BASES[slot] + HELPERS[name][1]])
        if bp.enabled or force then
            local result = bp.callback(BASES[slot] + HELPERS[name][1], 4, 'x')
            assert(result == nil, 'a false return would remove the breakpoint')
        end
    end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do
            if bp.enabled then return false end
        end
        return true
    end
    local count = 0
    for _ in pairs(breakpoints) do count = count + 1 end
    assert(count == 14, 'seven helpers in each of the two slot images')
    return r
end

local function stopped(r, status)
    assert(r:has('status: ' .. status), status)
    assert(r:countOf('script: model_variant373_helper_reach') == 1)
    assert(r:has('==== USER CONTEXT'))
    assert(r:has('setup: '))
    assert(r:allDisabled())
    r:frames(60)
    r:reset()
    r:hit(0, 'DrawStrip', nil, true)
    assert(r:countOf('==== TRACE RESULT') == 1)
    assert(r:countOf('--- end of trace') == 1,
        'termination emits one final document, even after late callbacks')
end

-- A slot-0 episode: controls fire, one candidate fires once, rows name the
-- caller and the phase; a repeat hit prints no second row.
do
    local r = capture()
    assert(r:has('14 breakpoints installed'))
    r:load(0)
    r:phase(0, 3)
    r:frames(1)
    assert(r:has('#01 t=   0.0s START slot=0 header=356 model=707 star=0 '
        .. 'work=0x80100000 phase=3'))
    r:hit(0, 'DrawLayers')
    r:hit(0, 'DrawLayers')
    r:hit(0, 'DrawGrid', 0x8013b5d0)
    assert(r:has('#01 t=   0.0s first DrawGrid slot=0 ra=0x8013B5D0 '
        .. 'a0=0x80100000 work=0x80100000 phase=3'))
    assert(r:countOf('first DrawLayers') == 1)
    r:hit(0, 'DrawRays', 0x8013e000)
    assert(r:has('first DrawRays slot=0 ra=0x8013E000 a0=0x80100000 '
        .. 'work=0x80100000 phase=3 CANDIDATE'))
    r:unload(0)
    r:frames(1)
    assert(r:has('#01 t=   0.0s END   slot=0 reason=image unloaded'))
    r:reset()
    stopped(r, 'reset; reload the script and start again')
    assert(r:has('summary #01 slot=0 frames=1 DrawLayers=2 DrawGrid=1 '
        .. 'DrawPoints=0 DrawStrip=0 DrawRibbons=0 DrawQuads=0 DrawRays=1'))
    assert(r:has('control_hits: 3 candidate_hits: 1 foreign_hits: 0 '
        .. 'signature_mismatches: 0'))
end

-- Other images at the same address and changed code are not counted; a
-- loaded image with silent controls is flagged.
do
    local r = capture()
    r:load(1, 412, 10)
    r:frames(1)
    r:hit(1, 'DrawStrip')
    assert(not r:has('START'), 'another header is not an episode')
    r:load(1)
    r:frames(1)
    assert(r:has('START slot=1 header=506 model=707'))
    r:corrupt(1, 'DrawQuads')
    r:hit(1, 'DrawQuads')
    assert(not r:has('first DrawQuads'))
    r:frames(240 * 60)
    stopped(r, 'four-minute window after the first episode ended')
    assert(r:has('foreign_hits: 1 signature_mismatches: 1'))
    assert(r:has('warning: a Cosmo Queen image was loaded but no control '
        .. 'helper fired'))
    assert(r:has('reason=still running'))
end

-- Guards: a work pointer outside RAM or unaligned does not open an episode;
-- a header-356 image under another model is labelled.
do
    local r = capture()
    r:load(0, nil, 707, 0x1000)
    r:frames(1)
    r:load(0, nil, 707, 0x80100002)
    r:frames(1)
    r:load(0, nil, 707, 0x801ffff0)
    r:frames(1)
    assert(not r:has('START'))
    r:load(0, nil, 12)
    r:frames(1)
    assert(r:has('START slot=0 header=356 model=12 (not 707)'))
    r:reset()
    stopped(r, 'reset')
end

-- Four episodes stop the run; a register failure is reported, not fatal.
do
    local r = capture()
    for n = 1, 4 do
        r:load(n % 2, nil, 707, WORK[n % 2] + n * 0x10)
        r:frames(1)
        r:unload(n % 2)
        r:frames(1)
    end
    stopped(r, 'captured 4 episodes')
end
do
    local r = capture({brokenRegisters = true})
    r:load(0)
    r:frames(1)
    r:hit(0, 'DrawGrid')
    assert(r:has('first DrawGrid slot=0 ra=unreadable'))
    r:reset()
    stopped(r, 'reset')
end
do
    local r = capture()
    r:frames(600 * 60)
    stopped(r, 'ten minutes without a Cosmo Queen image')
end
do
    local r = capture()
    r:load(0)
    r:frames(900 * 60)
    stopped(r, 'four-minute window')
end

print = originalPrint
print('model_variant373_helper_reach: all cases passed')
