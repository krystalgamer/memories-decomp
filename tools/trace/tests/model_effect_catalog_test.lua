-- Run from the repository root:
--   luajit tools/trace/tests/model_effect_catalog_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local originalPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/model_effect_catalog.lua'

local RAM_BASE = 0x80000000
local SLOTS, SLOT_SIZE = 0x800f2c40, 0xe20
local VARIANT_DATA, MODEL_ID, STAR = 0xdec, 0xdf8, 0xdfe
local BASES = {[0] = 0x8013b000, [1] = 0x8017b000}
local WORK = {[0] = 0x80136000, [1] = 0x80140000}

-- The generated tables, evaluated on their own.
local handle = assert(io.open(scriptPath))
local source = handle:read('*a')
handle:close()
local block = assert(source:match('\n(%-%- BEGIN GENERATED CATALOG\n.-'
                                  .. '%-%- END GENERATED CATALOG\n)'))
local env = {CATALOG = {}}
setfenv(assert(loadstring(block)), env)()
local CATALOG, CARDS = env.CATALOG, env.CARDS

do
    local images, functions, named = 0, 0, 0
    for slot = 0, 1 do
        for header, entry in pairs(CATALOG[slot]) do
            images = images + 1
            assert(header > 0 and header < 0x1000)
            assert(entry[1] == 'h' .. header or entry[1]:match('^e%d%d%d$'))
            if entry[1]:sub(1, 1) == 'e' then named = named + 1 end
            for item in entry[2]:gmatch('%S+') do
                assert(item:match('^%d+/[01]$'), item)
            end
            local last = 0
            for _, f in ipairs(entry[3]) do
                assert(f[1] > last and f[1] % 4 == 0 and f[1] < 0x5000)
                assert(f[2] >= 0 and f[2] < 0x100000000 and #f[3] > 0)
                last = f[1]
                functions = functions + 1
            end
            assert(#entry[3] >= 1 and #entry[3] <= 10)
        end
    end
    assert(images == 650 and named == 332 and functions == 1370,
           images .. ' ' .. named .. ' ' .. functions)
    assert(#CARDS == 722 and CARDS[708] == 'Cosmo Queen/Venus/Uranus')
    -- notes/model-effects.csv: e006 is NA header 59 in slot 0.
    assert(CATALOG[0][59][1] == 'e006')
    assert(CATALOG[0][356][2] == '707/0' and #CATALOG[0][356][3] == 8)
end

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local registers = {pc = 0, GPR = {n = {ra = 0x8013c000}, r = {}}}
    local events, breakpoints, output = {}, {}, {}
    local added = 0
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
            assert(breakpoints[address] == nil, 'one breakpoint per address')
            assert(kind == 'Exec' and size == 4 and callback ~= nil)
            added = added + 1
            local bp = {enabled = true, callback = callback}
            function bp:disable() self.enabled = false end
            function bp:enable() self.enabled = true end
            breakpoints[address] = bp
            return bp
        end,
        Events = {
            createEventListener = function(name, callback)
                events[name] = callback
                return {}
            end,
        },
    }
    breakpoints_model_effect_catalog = nil
    listeners_model_effect_catalog = nil
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
    function r:added() return added end
    function r:frames(count)
        for _ = 1, count do events['GPU::Vsync']() end
    end
    function r:reset() events['ExecutionFlow::Reset']() end
    -- Loads an image into a slot. With code=false only the header lands
    -- (the rest of the transfer has not arrived yet).
    function r:load(slot, header, model, star, work, code)
        local base = BASES[slot]
        word(base, header)
        local entry = CATALOG[slot][header]
        if entry ~= nil and code ~= false then
            for _, f in ipairs(entry[3]) do word(base + f[1], f[2]) end
        end
        local s = SLOTS + slot * SLOT_SIZE
        word(s + VARIANT_DATA, work or WORK[slot])
        ffi.cast('uint16_t*', memory + (s + MODEL_ID - RAM_BASE))[0] = model
        memory[s + STAR - RAM_BASE] = star
    end
    function r:code(slot, header)
        for _, f in ipairs(CATALOG[slot][header][3]) do
            word(BASES[slot] + f[1], f[2])
        end
    end
    function r:poke(address, value) word(address, value) end
    function r:unload(slot) word(BASES[slot], 0) end
    function r:bp(slot, offset) return breakpoints[BASES[slot] + offset] end
    -- Fires the breakpoint as the emulator would: only while enabled,
    -- unless forced.
    function r:hit(slot, offset, force)
        local address = BASES[slot] + offset
        local bp = breakpoints[address]
        if bp == nil then return false end
        if bp.enabled or force then
            local result = bp.callback(address, 4, 'x')
            assert(result == nil, 'a false return would remove the breakpoint')
            return true
        end
        return false
    end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do
            if bp.enabled then return false end
        end
        return true
    end
    assert(added == 0, 'nothing is armed before an image loads')
    return r
end

local function stopped(r, status)
    assert(r:has('status: ' .. status), status)
    assert(r:countOf('script: model_effect_catalog') == 1)
    assert(r:has('==== USER CONTEXT'))
    assert(r:has('setup: '))
    assert(r:allDisabled())
    r:frames(60)
    r:reset()
    for slot = 0, 1 do
        for offset = 0, 0x5000, 4 do r:hit(slot, offset, true) end
    end
    assert(r:countOf('==== TRACE RESULT') == 1)
    assert(r:countOf('--- end of trace') == 1,
        'termination emits one final document, even after late callbacks')
end

-- One battle: Cosmo Queen (Venus) attacks in slot 0 and a header-59 (e006)
-- monster defends in slot 1 under a model the catalog does not expect
-- there. Hits are counted, first hits are printed with the caller, and the
-- image's breakpoints are disabled when it is cleared.
do
    local r = capture()
    assert(r:has('model_effect_catalog: armed;'))
    r:frames(10)
    r:load(0, 356, 707, 0)
    r:frames(1)
    assert(r:has('#01 t=   0.2s START slot=0 header=356 h356 model=707 '
        .. 'card=708 "Cosmo Queen" star=Venus selection=0 expected=yes '
        .. 'work=0x80136000 functions=8'))
    assert(r:added() == 8)
    assert(r:hit(0, 0x4))
    assert(r:has('model_effect_catalog: execution breakpoint observed; '
        .. 'setup confirmed'))
    assert(r:has('#01 t=   0.2s first +0x0004 '
        .. 'FrenchModelVariant373_HandleRequest slot=0 ra=0x8013C000'))
    r:frames(60)
    r:hit(0, 0x4)
    r:hit(0, 0x10a8)
    r:hit(0, 0x10a8)
    assert(r:countOf('first +0x10A8') == 1)
    -- A defender whose image is not where the catalog puts that model.
    r:load(1, 506, 57, 1)
    r:frames(1)
    assert(r:has('#02 t=   1.2s START slot=1 header=506 h506 model=57 '
        .. 'card=58 "Kuriboh" star=Saturn selection=1 expected=NO '
        .. '(catalog: 707/0)'))
    r:unload(0)
    r:frames(1)
    assert(r:has('#01 t=   1.2s END   slot=0 reason=image unloaded'))
    assert(not r:bp(0, 0x4).enabled and r:bp(1, 0x4).enabled)
    assert(not r:hit(0, 0x10a8), 'a closed image no longer counts')
    r:hit(0, 0x10a8, true)
    r:reset()
    stopped(r, 'reset; reload the script and start again')
    assert(r:has('summary #01 slot=0 header=356 h356 model=707 card=708 '
        .. '"Cosmo Queen" star=Venus selection=0 expected=yes frames=62'))
    assert(r:has('summary #01   +0x0004 FrenchModelVariant373_HandleRequest '
        .. 'hits=2 first=+0.0s'))
    assert(r:has('summary #01   +0x10A8 FrenchModelVariant373_DrawRisingRings '
        .. 'hits=2 first=+1.0s'))
    assert(r:has('summary #01   +0x33D8 FrenchModelVariant373_DrawRays hits=0'))
    assert(r:has('summary #02 slot=1 header=506'))
    assert(r:has('#02 t=   1.2s END   slot=1 reason=still running'))
    assert(r:has('stray_hits: 1 foreign_hits: 0 signature_mismatches: 0'))
    assert(r:has('execution_breakpoint=observed'))
end

-- The header lands before the code: functions are armed as their first
-- words arrive, and a word that never matches is reported, not armed.
-- An image with the same header but a new work area is a new episode that
-- re-enables the existing breakpoints instead of adding more.
do
    local r = capture()
    r:load(0, 356, 707, 0, nil, false)
    r:frames(1)
    assert(r:has('START slot=0 header=356'))
    assert(r:added() == 0)
    r:code(0, 356)
    r:poke(BASES[0] + 0x33d8, 0)
    r:frames(1)
    assert(r:added() == 7)
    r:frames(119)
    r:poke(BASES[0] + 0x33d8, 0x27bdfec8)
    r:frames(1)
    assert(r:added() == 7, 'the deadline has passed')
    r:hit(0, 0x1770)
    r:load(0, 356, 707, 0, WORK[0] + 0x100)
    r:frames(1)
    assert(r:has('#01 t=   2.0s END   slot=0 reason=image unloaded or work '
        .. 'area changed'))
    assert(r:has('#02 t=   2.0s START slot=0 header=356'))
    assert(r:added() == 8 and r:bp(0, 0x1770).enabled)
    r:hit(0, 0x1770)
    assert(r:countOf('first +0x1770') == 2, 'one first row per episode')
    -- Another module overwrote the image's code: the hit is not counted.
    r:poke(BASES[0] + 0x2240, 0)
    r:hit(0, 0x2240)
    -- The slot is reused for something else under a live breakpoint.
    r:poke(BASES[0], 412)
    r:hit(0, 0x2784)
    r:reset()
    stopped(r, 'reset')
    assert(r:has('summary #01   +0x33D8 FrenchModelVariant373_DrawRays not '
        .. 'armed (signature mismatch)'))
    assert(r:has('summary #01   +0x1770 FrenchModelVariant373_DrawSphere '
        .. 'hits=1 first=+2.0s'))
    assert(r:has('summary #02   +0x2240 FrenchModelVariant373_DrawStrip hits=0'))
    assert(r:has('foreign_hits: 1 signature_mismatches: 2'))
end

-- Guards: no work pointer, an unaligned or out-of-RAM one, and headers the
-- catalog does not know open nothing; unknown small headers are named once.
do
    local r = capture()
    r:load(0, 356, 707, 0, 0)
    r:frames(1)
    r:load(0, 356, 707, 0, 0x1000)
    r:frames(1)
    r:load(0, 356, 707, 0, 0x80136002)
    r:frames(1)
    r:load(0, 356, 707, 0, 0x80200000)
    r:frames(1)
    assert(not r:has('START'))
    r:load(1, 999, 1, 0)
    r:frames(5)
    assert(r:countOf('UNKNOWN header=999 slot=1') == 1)
    r:load(1, 0x27bd0000, 1, 0)
    r:frames(1)
    assert(not r:has('UNKNOWN header=666697728'))
    -- A model id past the card list and a slot read before load.
    r:load(0, 356, 4000, 7)
    r:frames(1)
    assert(r:has('START slot=0 header=356 h356 model=4000 card=unknown '
        .. 'selection=7 expected=NO (catalog: 707/0)'))
    r:reset()
    stopped(r, 'reset')
end

-- Eight episodes (four battles) end the run; headers are per slot, so
-- slot 0's header 59 in slot 1 is unknown there.
do
    local r = capture()
    for n = 1, 4 do
        r:load(0, 356, 707, 0, WORK[0] + n * 0x10)
        r:load(1, 506, 57, 0, WORK[1] + n * 0x10)
        r:frames(1)
        assert(not r:has('status:'))
        r:unload(0)
        r:unload(1)
        r:frames(1)
    end
    stopped(r, 'captured 8 episodes')
    assert(r:countOf(' START ') == 16, 'eight rows, live and in the document')
    assert(r:has('breakpoints: 16 '), 'reused across the four battles')
end
do
    local r = capture()
    r:load(1, 59, 57, 0)
    r:frames(1)
    assert(r:countOf('UNKNOWN header=59 slot=1') == 1)
    assert(not r:has('START'))
    r:reset()
    stopped(r, 'reset')
end
-- A ninth image is not opened while the eighth is still running, and the
-- breakpoint count stays bounded by the distinct addresses.
do
    local r = capture()
    for n = 1, 8 do
        local slot = n % 2
        local header = slot == 0 and 59 or 506
        r:load(slot, header, 57, 0, WORK[slot] + n * 0x10)
        r:frames(1)
        if n < 8 then
            r:unload(slot)
            r:frames(1)
        end
    end
    r:load(1, 506, 57, 0, WORK[1] + 0x200)
    r:frames(1)
    assert(not r:has('status:'))
    r:unload(0)
    r:frames(1)
    stopped(r, 'captured 8 episodes')
    assert(r:countOf(' START ') == 16, 'eight rows, live and in the document')
    assert(r:has('breakpoints: 9 '), 'header 59 has one entry, 506 eight')
end
do
    local r = capture({brokenRegisters = true})
    r:load(0, 59, 57, 0)
    r:frames(1)
    assert(r:has('START slot=0 header=59 e006 model=57 card=58 '
        .. '"Kuriboh" star=Mercury selection=0 expected=yes'))
    r:hit(0, 0x4)
    assert(r:has('first +0x0004 FrenchModelVariant76_HandleRequest slot=0 '
        .. 'ra=unreadable'))
    r:reset()
    stopped(r, 'reset')
end

-- Resident at load: an image already in place on the first Vsync is opened
-- and labelled.
do
    local r = capture()
    r:load(1, 506, 707, 0)
    r:frames(1)
    assert(r:has('START slot=1 header=506 h506 model=707 card=708 '
        .. '"Cosmo Queen" star=Venus selection=0 expected=yes '
        .. 'work=0x80140000 functions=8 (resident at load)'))
    r:reset()
    stopped(r, 'reset')
end

-- Nothing at all: the run ends by itself inside the Vsync budget that old
-- PCSX-Redux builds overflow at (two stack slots per event, 65,500 slots),
-- and says no battle was seen and the setup is unverified.
do
    local r = capture()
    r:frames(300 * 60 - 1)
    assert(not r:has('status:'))
    r:frames(1)
    stopped(r, 'five minutes elapsed')
    assert(300 * 60 * 2 < 65500)
    assert(r:has('no variant image loaded'))
    assert(r:has('execution_breakpoint=NOT-observed'))
end

-- A failing print (the console erroring) does not break the callbacks.
do
    local r = capture()
    local captured = print
    print = function() error('console full') end
    r:load(0, 356, 707, 0)
    r:frames(1)
    r:hit(0, 0x1770)
    print = captured
    r:reset()
    stopped(r, 'reset')
    assert(not r:has('callback_error'))
    assert(r:has('#01 t=   0.0s START slot=0'), 'rows are kept for the document')
    assert(r:has('first +0x1770'))
end

print = originalPrint
print('model_effect_catalog: all cases passed')
