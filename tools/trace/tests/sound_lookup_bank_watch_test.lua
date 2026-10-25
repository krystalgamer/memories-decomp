-- Run from the repository root:
--   luajit tools/trace/tests/sound_lookup_bank_watch_test.lua
-- Replays callbacks with mock PCSX events and real LuaJIT FFI memory.
local ffi = require('ffi')
assert(PCSX == nil, 'run this standalone, not inside the emulator console')
local originalPrint = print
local scriptPath = SCRIPT_UNDER_TEST or 'tools/trace/sound_lookup_bank_watch.lua'

local RAM_BASE = 0x80000000
local CONTROL, LOAD, FULL, SLOT_SOUNDS = 0x8001306c, 0x80048d08, 0x8003fee0,
    0x8005106c
local SE_PLAY, SE_PAN, SE_STOP = 0x80048658, 0x80048768, 0x80047db0
local SD_VALUE_PTR, STATE = 0x8009b45c, 0x801e0000
local NOTE_TABLE = 0x801f0000
local SLOTS, SLOT_SIZE, ACTIVE = 0x800f2c40, 0xe20, 0xe1f
local ADVANCE_LOAD, LIBRARY_LOAD = 0x80056828, 0x8002bd0c
local SLOT_SOUND_SITE, BATTLE_SITE = 0x80051100, 0x8001f600

local WORDS = {
    [CONTROL] = 0x27bdffe0, [LOAD] = 0x27bdffd0, [FULL] = 0x27bdffe8,
    [SLOT_SOUNDS] = 0x27bdff70, [SE_PLAY] = 0x27bdffe0,
    [SE_PAN] = 0x27bdffc8, [0x80048920] = 0x27bdffd8,
    [0x80048a28] = 0x27bdffc0, [SE_STOP] = 0x27bdffd8,
}

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local gpr = {n = {}, r = {}}
    local registers = {pc = 0, GPR = gpr}
    local events, breakpoints, output = {}, {}, {}
    local function word(address, value)
        ffi.cast('uint32_t*', memory + (address - RAM_BASE))[0] = value
    end
    local function half(address, value)
        ffi.cast('uint16_t*', memory + (address - RAM_BASE))[0] = value
    end
    for address, value in pairs(WORDS) do word(address, value) end
    if options.badControl then word(CONTROL, 0) end
    if options.pointer ~= false then
        word(SD_VALUE_PTR, options.pointer or STATE)
        word(STATE + 0x43c, NOTE_TABLE)
        for b = 0, 1 do
            for i = 0, 31 do half(STATE + 0x44c + b * 0x40 + i * 2, 0xffff) end
        end
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
    breakpoints_sound_lookup_bank_watch = nil
    listeners_sound_lookup_bank_watch = nil
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
    function r:active(slot, on)
        memory[SLOTS + slot * SLOT_SIZE + ACTIVE - RAM_BASE] = on and 1 or 0
    end
    function r:key(bank, index, value)
        half(STATE + 0x44c + bank * 0x40 + index * 2, value)
    end
    function r:note(key, value) half(NOTE_TABLE + key * 2, value) end
    function r:call(address, values, force)
        for _, name in ipairs({'a0', 'a1', 'a2', 'a3', 'ra'}) do
            gpr.n[name] = values[name] or 0
        end
        local bp = assert(breakpoints[address])
        if bp.enabled or force then bp.callback() end
    end
    function r:allDisabled()
        for _, bp in pairs(breakpoints) do
            if bp.enabled then return false end
        end
        return true
    end
    local count = 0
    for _ in pairs(breakpoints) do count = count + 1 end
    assert(count == 9, 'nine exec breakpoints')
    return r
end

local function stopped(r, status)
    assert(r:has('status: ' .. status), status)
    assert(r:countOf('script: sound_lookup_bank_watch') == 1)
    assert(r:has('==== USER CONTEXT'))
    assert(r:has('setup: '))
    assert(r:allDisabled())
    r:frames(60)
    r:reset()
    r:call(CONTROL, {}, true)
    r:call(SE_PLAY, {a0 = 0x4000}, true)
    assert(r:countOf('==== TRACE RESULT') == 1)
    assert(r:countOf('--- end of trace') == 1,
        'termination emits one final document, even after late callbacks')
end

-- A load fills bank 1, a scene plays tagged and untagged sounds.
do
    local r = capture()
    r:call(CONTROL, {})
    r:frames(2)
    r:call(LOAD, {a0 = 1, a1 = 0x801a8800, ra = ADVANCE_LOAD + 0x40})
    assert(r:has('#001 t=   0.0s --- LOAD bank=1 src=0x801A8800 '
        .. 'caller=Model_AdvanceSlotLoad(0x80056868) mode=0'))
    r:key(1, 0, 0x0123)
    r:key(1, 3, 0x0456)
    r:frames(1)
    assert(r:has('#002 t=   0.1s --- BANK bank1 filled by LOAD #001: '
        .. '2 keys 00=0123 03=0456'))

    r:active(1, true)
    r:frames(1)
    assert(r:has('#003 t=   0.1s S01 SCENE START active=-1- mode=0'))
    r:note(0x0456, 17)
    local play = {a0 = 0x4103, a1 = 0x1ff, a2 = 0xfff0, ra = SLOT_SOUND_SITE}
    r:call(SE_PLAY, play)
    r:call(SE_PLAY, play)
    assert(r:countOf('SD_SEPlay id=0x4103') == 1, 'repeats are counted')
    assert(r:has('#004 t=   0.1s S01 SD_SEPlay id=0x4103 bank=1 idx=3 -> '
        .. 'key=0x0456 note=17 from=LOAD#001 '
        .. 'caller=Model_PlaySlotAnimationSounds(0x80051100) vol=255 '
        .. 'pan=-16'))

    -- Unfilled index, extra bits, and the SD_SEPlayFull wrapper's caller.
    r:call(FULL, {ra = BATTLE_SITE})
    r:call(SE_PAN, {a0 = 0x4e05, ra = FULL + 0x10})
    assert(r:has('SD_SESetPan id=0x4E05 bank=0 idx=5 extra_bits=0xE00 -> '
        .. 'key=none note=- from=unknown '
        .. 'caller=SD_SEPlayFull(0x8003FEF0)<DuelScene_UpdateBattle'
        .. '(0x8001F600)'))
    r:call(SE_STOP, {a0 = 0x4005, ra = 0x80170000})
    assert(r:has('SD_SEStop id=0x4005 bank=0 idx=5 -> key=none '
        .. 'note=- from=unknown caller=overlay(0x80170000)'))

    r:call(SE_PLAY, {a0 = 0x0012, ra = BATTLE_SITE})
    r:call(SE_PLAY, {a0 = 0x8012, ra = BATTLE_SITE})
    r:call(SLOT_SOUNDS, {a0 = 1})
    r:call(SLOT_SOUNDS, {a0 = 1})

    -- A bank change without a load, then a library load that changes nothing.
    r:key(0, 1, 0x0777)
    r:frames(1)
    assert(r:has('BANK bank0 changed without a LOAD: 1 keys 01=0777'))
    r:call(LOAD, {a0 = 1, a1 = 0x80100000, ra = LIBRARY_LOAD + 8})
    r:frames(1)
    assert(r:has('BANK bank1 unchanged by LOAD #'))

    r:active(1, false)
    r:frames(1)
    assert(r:has('S01 SCENE END'))
    -- The next scene prints the same call again.
    r:active(0, true)
    r:frames(1)
    r:call(SE_PLAY, play)
    assert(r:countOf('SD_SEPlay id=0x4103') == 2)

    r:reset()
    stopped(r, 'reset; reload the script and start again')
    assert(r:has('frames: 8 scenes: 2 control_hits: 1 loads: 2 '
        .. 'tagged_calls: 5 dropped_rows: 0'))
    assert(r:has('untagged_calls: SD_SEPlay=2 SD_SESetPan=0 SD_SESetVolume=0 '
        .. 'SD_SESetVolumePan=0 SD_SEStop=0'))
    assert(r:has('Model_PlaySlotAnimationSounds_calls: slot0=0 slot1=2 slot2=0'))
    assert(r:has('tagged SD_SEPlay id=0x4103 caller=Model_PlaySlotAnimationSounds '
        .. 'calls=3'))
    assert(r:has('tagged SD_SESetPan id=0x4E05 caller=SD_SEPlayFull<'
        .. 'DuelScene_UpdateBattle calls=1'))
    assert(r:has('final bank1 from=#'))
    assert(r:has('final bank0 from=none 1 keys 01=0777'))
end

-- No g_SDValue yet: everything is unreadable, nothing crashes.
do
    local r = capture({pointer = 0x00000002})
    r:call(CONTROL, {})
    r:call(SE_PLAY, {a0 = 0x4001, ra = SLOT_SOUND_SITE})
    assert(r:has('key=unreadable note=- from=unknown'))
    r:call(LOAD, {a0 = 7, a1 = 0, ra = 0})
    r:frames(1)
    r:reset()
    stopped(r, 'reset')
    assert(r:has('final bank0 from=none unreadable'))
end

-- Eight scenes, the load window, missing control, timeout, callback error,
-- signature mismatch.
do
    local r = capture()
    r:call(CONTROL, {})
    for _ = 1, 8 do
        r:active(2, true)
        r:frames(1)
        r:active(2, false)
        r:frames(1)
    end
    stopped(r, 'captured 8 scenes')
end
do
    local r = capture()
    r:call(CONTROL, {})
    r:call(LOAD, {a0 = 0})
    r:frames(240 * 60 - 1)
    assert(not r:has('script: sound_lookup_bank_watch'))
    r:frames(1)
    stopped(r, 'four-minute window after the first LOAD ended')
end
do
    local r = capture()
    r:frames(300 * 60)
    stopped(r, 'five minutes without a control hit')
end
do
    local r = capture()
    r:call(CONTROL, {})
    r:frames(720 * 60)
    stopped(r, 'timed out')
end
do
    local r = capture({brokenRegisters = true})
    r:call(SE_PLAY, {})
    r:frames(1)
    stopped(r, 'callback error')
    assert(r:has('callback_error: '))
end
do
    local r = capture({badControl = true})
    assert(r:has('1 signature mismatches'))
    r:reset()
    stopped(r, 'reset')
    assert(r:has('signature_mismatch: control Main_RunFrameServices '
        .. '0x8001306C expected 0x27BDFFE0 read 0x00000000'))
end

print = originalPrint
print('sound_lookup_bank_watch: all cases passed')
