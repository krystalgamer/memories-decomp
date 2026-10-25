-- sound_lookup_bank_watch.lua
--
-- WHAT THIS ANSWERS
--   SDValue.field_044C (g_SDValue + 0x44C) is two banks of 32 u16 sound keys.
--   A sound-effect id of the form 0x4000 | bank * 0x100 | index (0..31) is
--   not played directly: SD_SEPlay, SD_SESetPan, SD_SESetVolume,
--   SD_SESetVolumePan and SD_SEStop first replace it with
--   field_044C[bank][index]. Static reading suggests one bank per 3-D model
--   slot:
--     * SD_LoadSequenceBankPair(side, src) fills field_044C[side] while it
--       uploads a sample block to SPU 0xD810 + side * 0x19000;
--       Model_AdvanceSlotLoad calls it with the model slot index and
--       Library_LoadPackageStage with side 1;
--     * Model_PlaySlotAnimationSounds builds the id as 0x4000 + (slot << 8).
--   Nobody has seen which monster's sounds sit in which bank, whether menus
--   or the Library use the banks too, or whether any other caller builds a
--   0x4000 id. This trace lets the two banks (and the 0x100 bank bit) be
--   named.
--
--   Every row is printed the moment it happens with a running #NNN marker and
--   a time stamp, so it can be matched to what was on screen:
--     * LOAD rows: each SD_LoadSequenceBankPair call (bank, source, the
--       source block's entry count, caller), followed on the next VSync by a
--       BANK row naming every LOAD since the last VSync for that bank,
--     * BANK rows for any other change of a bank's contents,
--     * SCENE rows when a 3-D model scene starts or ends (active slots),
--     * tagged-call rows: the first call per scene of each function, caller
--       and id, with the bank, index, resolved key and the LOAD whose written
--       range covers that entry (a LOAD only writes entries below its count,
--       so a shorter reload leaves higher entries with their older origin;
--       'LOAD#NNN?' when the count was unreadable, 'unknown' when no LOAD
--       explains the entry). Repeats are counted in the final document.
--   Untagged sound-effect calls are only counted.
--
-- HOW TO RUN
--   1. Open PCSX-Redux with the game and enable the debugger. Select the
--      interpreter CPU (restart the emulator if you switch).
--   2. Debug -> Lua editor, paste this file. Load a save or start from the
--      main menu.
--   3. Start a Free Duel and play until two or three monster battles have
--      run (the 3-D attack scenes). If you can, then leave the duel and open
--      the Library and look at a card's monster.
--   4. It prints its document after 8 scenes, four minutes after the first
--      LOAD, after 12 minutes in total, or on reset.
--   5. Copy the whole document into
--      tools/trace/result/sound_lookup_bank_watch.txt and fill in the
--      context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   One line per SCENE (or per Library view): the two monsters, which one was
--   yours, and which monster's sounds you heard (attack, roar, death).

local ffi = require('ffi')

local SCRIPT_NAME = 'sound_lookup_bank_watch'
-- Breakpoints fire only while the debugger setting is on and the interpreter
-- CPU is the engine actually running. PCSX-Redux reads the debugger flag live,
-- so a script can switch it on itself. The CPU engine is chosen at emulator
-- start and Lua cannot query it: the Dynarec setting only says what the next
-- start will use. The setup therefore counts as confirmed only once one of
-- this script's execution breakpoints has fired. Runs before any breakpoint
-- is installed.
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
    local configured = 'unknown'
    local okDynarec, dynarecOn = pcall(function() return emulator.Dynarec end)
    if okDynarec and dynarecOn == false then
        configured = 'interpreter'
    elseif okDynarec and dynarecOn == true then
        configured = 'DYNAREC: breakpoints will not fire; select the '
            .. 'interpreter CPU and restart the emulator'
    end
    return 'debugger=' .. debugger .. ' configured_cpu=' .. configured
        .. ' active_cpu=unverified'
end)()
print(SCRIPT_NAME .. ': setup ' .. TRACE_SETUP)

-- Every breakpoint goes through here so the first real hit can confirm the
-- running engine. The wrapped invoker's return value is passed through.
local traceExecutionObserved = false
local function traceAddBreakpoint(address, kind, width, cause, invoker)
    if invoker == nil then
        return PCSX.addBreakpoint(address, kind, width, cause)
    end
    return PCSX.addBreakpoint(address, kind, width, cause, function(...)
        if not traceExecutionObserved then
            traceExecutionObserved = true
            pcall(print, SCRIPT_NAME .. ': execution breakpoint observed; '
                .. 'setup confirmed')
        end
        return invoker(...)
    end)
end
local function traceExecutionStatus()
    if traceExecutionObserved then
        return 'execution_breakpoint=observed'
    end
    return 'execution_breakpoint=NOT-observed: the setup is unverified and '
        .. 'silence is not evidence'
end

local RAM_BASE = 0x80000000
local RAM_END = 0x80200000
local FPS = 60
local MAIN_MODE = 0x8009b26c
local SD_VALUE_PTR = 0x8009b45c
local NOTE_INDEX_BY_ID = 0x43c
local BANKS = 0x44c
local BANK_STRIDE = 0x40
local BANK_ENTRIES = 32
local KEY_NONE = 0xffff
local BLOCK_CAPACITY = 204 -- key slots in one source block
local SLOTS = 0x800f2c40
local SLOT_SIZE = 0xe20
local SLOT_COUNT = 3
local IS_ACTIVE = 0xe1f

-- {address, first instruction word, name}
local CONTROL = {0x8001306c, 0x27bdffe0, 'control Main_RunFrameServices'}
local LOAD_BANK = {0x80048d08, 0x27bdffd0, 'SD_LoadSequenceBankPair'}
local SE_PLAY_FULL = {0x8003fee0, 0x27bdffe8, 'SD_SEPlayFull'}
local SLOT_SOUNDS = {0x8005106c, 0x27bdff70, 'Model_PlaySlotAnimationSounds'}
local SE_FUNCTIONS = {
    {0x80048658, 0x27bdffe0, 'SD_SEPlay'},
    {0x80048768, 0x27bdffc8, 'SD_SESetPan'},
    {0x80048920, 0x27bdffd8, 'SD_SESetVolume'},
    {0x80048a28, 0x27bdffc0, 'SD_SESetVolumePan'},
    {0x80047db0, 0x27bdffd8, 'SD_SEStop'},
}

-- Resident callers of the sound-effect and bank functions, so rows can name
-- them. Anything else prints as unknown with its address.
local FUNCTIONS = {
    {0x8001825c, 0x3ac, 'DuelScene_UpdateResume'},
    {0x80019d18, 0x13b4, 'DuelScene_UpdateCardPlacement'},
    {0x8001b170, 0x610, 'DuelScene_UpdateCardMove'},
    {0x8001bd88, 0x14b8, 'DuelScene_UpdateHandActions'},
    {0x8001d670, 0x1600, 'DuelScene_UpdateFieldActions'},
    {0x8001f55c, 0x1378, 'DuelScene_UpdateBattle'},
    {0x80023d08, 0x2b4, 'DuelCursor_ProcessGridDirection'},
    {0x80024200, 0x188, 'DuelScene_Update'},
    {0x8002a3cc, 0xdc, 'Library_UpdateCursorMotion'},
    {0x8002aca4, 0xdfc, 'Library_UpdateCardView'},
    {0x8002bd0c, 0x230, 'Library_LoadPackageStage'},
    {0x8002ec74, 0x13c, 'Script_OpSound'},
    {0x80038798, 0x68, 'DuelEffect_PlaySoundCommand'},
    {0x80039794, 0x1a0, 'TextBox_UpdateChannels'},
    {0x8003fee0, 0x28, 'SD_SEPlayFull'},
    {0x8003ff88, 0x2c, 'SD_CdSoundPlay'},
    {0x8005106c, 0x2e4, 'Model_PlaySlotAnimationSounds'},
    {0x800559d4, 0x748, 'Model_UpdateSlotControl'},
    {0x80056828, 0x554, 'Model_AdvanceSlotLoad'},
    {0x80058624, 0x214, 'SD_ApplyPositionalSound'},
    {0x8006cd78, 0x243c, 'Model_UpdateBurstEffect'},
    {0x8006f1b4, 0x13d8, 'Model_UpdateSparkEffect'},
}

local MAX_SCENES = 8
local MAX_ROWS = 500
local MAX_CALL_ROWS = 200
local MAX_BANK_ROWS = 60
local WINDOW_FRAMES = 240 * FPS
local NO_CONTROL_FRAMES = 300 * FPS
local TIMEOUT_FRAMES = 720 * FPS

local mem = PCSX.getMemPtr()

local function inRam(addr, size)
    return addr ~= nil and addr >= RAM_BASE and addr + size <= RAM_END
end
local function u8(addr)
    if mem == nil or not inRam(addr, 1) then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + (addr - RAM_BASE))[0])
end
local function u16(addr)
    if mem == nil or not inRam(addr, 2) or addr % 2 ~= 0 then return nil end
    return tonumber(ffi.cast('uint16_t*', mem + (addr - RAM_BASE))[0])
end
local function u32(addr)
    if mem == nil or not inRam(addr, 4) or addr % 4 ~= 0 then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + (addr - RAM_BASE))[0])
end
local function show(v, fmt)
    if v == nil then return 'unreadable' end
    return string.format(fmt or '%d', v)
end

local frames = 0
local marker = 0
local rows, dropped = {}, 0
local callRows, bankRows = 0, 0
local sceneCount = 0
local scene = nil
local firstLoadFrame = nil
local controlHits, loads, tagged = 0, 0, 0
local untagged = {}
local slotSoundCalls = {}
local lastFullCaller = nil
local banks = {}        -- last seen contents, '' when unreadable
local bankKeys = {}     -- last seen key tables
-- Origin of each of the 64 entries (bank * 32 + index), set when a LOAD
-- starts: 'LOAD#NNN' for entries inside its written range, 'LOAD#NNN?' when
-- its range could not be read, nil when no LOAD explains the entry.
local entrySource = {}
local pendingLoads = {} -- LOADs not yet described by a BANK row
local calls, callOrder = {}, {}
local mismatches = {}
local callbackError = nil
local done = false

breakpoints_sound_lookup_bank_watch = {}

local function stamp()
    return string.format('t=%6.1fs', frames / FPS)
end

local function live(text)
    marker = marker + 1
    local line = string.format('#%03d %s %s %s', marker, stamp(),
                               scene and string.format('S%02d', scene.number)
                                   or '---', text)
    print(line)
    if #rows < MAX_ROWS then rows[#rows + 1] = line else dropped = dropped + 1 end
    return marker
end

local function functionName(pc)
    if pc == nil then return 'unknown' end
    for _, f in ipairs(FUNCTIONS) do
        if pc >= f[1] and pc < f[1] + f[2] then return f[3] end
    end
    if pc >= 0x800906e0 and pc < RAM_END then return 'overlay' end
    return 'unknown'
end

local function caller(ra)
    local name = functionName(ra)
    local text = string.format('%s(0x%08X)', name, ra)
    if name == 'SD_SEPlayFull' and lastFullCaller ~= nil then
        text = text .. '<' .. string.format('%s(0x%08X)',
                                            functionName(lastFullCaller),
                                            lastFullCaller)
    end
    return text
end

local function sdValue()
    local p = u32(SD_VALUE_PTR)
    if p == nil or not inRam(p, BANKS + 2 * BANK_STRIDE) or p % 4 ~= 0 then
        return nil
    end
    return p
end

local function readBank(bank)
    local base = sdValue()
    if base == nil then return nil end
    local keys = {}
    for i = 0, BANK_ENTRIES - 1 do
        keys[i] = u16(base + BANKS + bank * BANK_STRIDE + i * 2)
        if keys[i] == nil then return nil end
    end
    return keys
end

local function describe(keys)
    if keys == nil then return 'unreadable' end
    local parts = {}
    for i = 0, BANK_ENTRIES - 1 do
        if keys[i] ~= KEY_NONE then
            parts[#parts + 1] = string.format('%02d=%04X', i, keys[i])
        end
    end
    if #parts == 0 then return 'empty' end
    return #parts .. ' keys ' .. table.concat(parts, ' ')
end

local function signature(keys)
    return keys and describe(keys) or ''
end

local function activeMask()
    local mask = ''
    for i = 0, SLOT_COUNT - 1 do
        local v = u8(SLOTS + i * SLOT_SIZE + IS_ACTIVE)
        mask = mask .. ((v ~= nil and v ~= 0) and tostring(i) or '-')
    end
    return mask
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_sound_lookup_bank_watch) do
        pcall(function() bp:disable() end)
    end
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<one line per SCENE or Library view: the two monsters, which was')
    print(' yours, and whose sounds you heard>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('frames: %d scenes: %d control_hits: %d loads: %d '
                        .. 'tagged_calls: %d dropped_rows: %d', frames,
                        sceneCount, controlHits, loads, tagged, dropped))
    local parts = {}
    for _, f in ipairs(SE_FUNCTIONS) do
        parts[#parts + 1] = f[3] .. '=' .. (untagged[f[3]] or 0)
    end
    print('untagged_calls: ' .. table.concat(parts, ' '))
    parts = {}
    for i = 0, SLOT_COUNT - 1 do
        parts[#parts + 1] = 'slot' .. i .. '=' .. (slotSoundCalls[i] or 0)
    end
    print('Model_PlaySlotAnimationSounds_calls: ' .. table.concat(parts, ' '))
    for _, m in ipairs(mismatches) do print('signature_mismatch: ' .. m) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    for b = 0, 1 do
        local sources, seenSource = {}, {}
        for i = 0, BANK_ENTRIES - 1 do
            local src = entrySource[b * BANK_ENTRIES + i] or 'unknown'
            if not seenSource[src] then
                seenSource[src] = true
                sources[#sources + 1] = src
            end
        end
        print(string.format('final bank%d from=%s %s', b,
                            table.concat(sources, ','),
                            describe(readBank(b))))
    end
    print('')
    for _, key in ipairs(callOrder) do
        print(string.format('tagged %s calls=%d', key, calls[key]))
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function markers(list)
    local parts = {}
    for _, load in ipairs(list) do
        parts[#parts + 1] = string.format('#%03d', load.marker)
    end
    return table.concat(parts, ',')
end

-- A bank's row names every pending LOAD whose written range reaches it, so a
-- bank-0 count above 32 is named for bank 1 too. Entries that changed
-- outside every pending LOAD's written range lose their origin; the row says
-- how many.
local function pollBanks(origins)
    for b = 0, 1 do
        local keys = readBank(b)
        local sig = signature(keys)
        local mine, covered = {}, {}
        for _, load in ipairs(origins) do
            if load.bank == b or load.first < (b + 1) * BANK_ENTRIES
                and load.last >= b * BANK_ENTRIES then
                mine[#mine + 1] = load
            end
            for k = load.first, load.last do covered[k] = true end
        end
        local stray = 0
        local old = bankKeys[b]
        if keys ~= nil and old ~= nil then
            for i = 0, BANK_ENTRIES - 1 do
                local flat = b * BANK_ENTRIES + i
                if keys[i] ~= old[i] and not covered[flat] then
                    stray = stray + 1
                    entrySource[flat] = nil
                end
            end
        end
        local text = nil
        if banks[b] ~= nil and banks[b] ~= sig then
            if #mine > 0 then
                text = string.format('BANK bank%d filled by LOAD %s: %s', b,
                                     markers(mine), describe(keys))
                if stray > 0 then
                    text = text .. string.format(
                        ' (%d entries changed outside the loaded range)',
                        stray)
                end
            else
                if keys == nil or old == nil then
                    for i = 0, BANK_ENTRIES - 1 do
                        entrySource[b * BANK_ENTRIES + i] = nil
                    end
                end
                text = string.format('BANK bank%d changed without a LOAD: %s',
                                     b, describe(keys))
            end
        elseif #mine > 0 then
            text = string.format('BANK bank%d unchanged by LOAD %s: %s', b,
                                 markers(mine), describe(keys))
        end
        if text ~= nil and bankRows < MAX_BANK_ROWS then
            bankRows = bankRows + 1
            live(text)
        end
        banks[b] = sig
        bankKeys[b] = keys
    end
end

-- SD_LoadSequenceBankPair writes field_044C[side][j] for every j below the
-- source block's s32 count, as one flat run that would pass the end of
-- bank 0 into bank 1. The call is synchronous, so the origin is recorded now
-- and is already right for any sound call before the next VSync.
local function onLoad()
    local n = PCSX.getRegisters().GPR.n
    local side, src, ra = tonumber(n.a0), tonumber(n.a1), tonumber(n.ra)
    loads = loads + 1
    firstLoadFrame = firstLoadFrame or frames
    local count = u32(src)
    if count ~= nil and count >= 0x80000000 then count = count - 0x100000000 end
    local id = live(string.format('LOAD bank=%d src=0x%08X count=%s caller=%s '
                                  .. 'mode=%s', side, src, show(count),
                                  caller(ra), show(u8(MAIN_MODE))))
    if side ~= 0 and side ~= 1 then return end
    local first = side * BANK_ENTRIES
    local label = string.format('LOAD#%03d', id)
    local last
    if count == nil or count > BLOCK_CAPACITY then
        last = first + BANK_ENTRIES - 1
        label = label .. '?'
    else
        last = math.min(first + count, 2 * BANK_ENTRIES) - 1
    end
    for k = first, last do entrySource[k] = label end
    pendingLoads[#pendingLoads + 1] = {bank = side, marker = id,
                                       first = first, last = last}
end

local function onTagged(name)
    local n = PCSX.getRegisters().GPR.n
    local id = tonumber(n.a0) % 0x10000
    local ra = tonumber(n.ra)
    if id >= 0x8000 or id - id % 0x1000 ~= 0x4000 then
        untagged[name] = (untagged[name] or 0) + 1
        return
    end
    tagged = tagged + 1
    local bank = (id % 0x200 >= 0x100) and 1 or 0
    local index = id % 0x20
    local extra = id % 0x1000 - id % 0x200 + (id % 0x100 - index)
    local key, note = nil, nil
    local base = sdValue()
    if base ~= nil then
        key = u16(base + BANKS + bank * BANK_STRIDE + index * 2)
        local noteTable = u32(base + NOTE_INDEX_BY_ID)
        if key ~= nil and key ~= KEY_NONE and noteTable ~= nil then
            note = u16(noteTable + key * 2)
        end
    end
    local args = ''
    if name == 'SD_SEPlay' then
        local pan = tonumber(n.a2) % 0x10000
        if pan >= 0x8000 then pan = pan - 0x10000 end
        args = string.format(' vol=%d pan=%d', tonumber(n.a1) % 0x100, pan)
    end
    local who = caller(ra)
    local summary = string.format('%s id=0x%04X caller=%s', name, id,
                                  functionName(ra) == 'SD_SEPlayFull'
                                      and who:gsub('%(0x%x+%)', '') or
                                      functionName(ra))
    if calls[summary] == nil then
        calls[summary] = 0
        callOrder[#callOrder + 1] = summary
    end
    calls[summary] = calls[summary] + 1
    local sceneKey = name .. string.format('%04X', id) .. who
    local seen = scene and scene.seen or nil
    if seen ~= nil then
        if seen[sceneKey] then return end
        seen[sceneKey] = true
    end
    if callRows >= MAX_CALL_ROWS then return end
    callRows = callRows + 1
    live(string.format('%s id=0x%04X bank=%d idx=%d%s -> key=%s note=%s '
                       .. 'from=%s caller=%s%s', name, id, bank, index,
                       extra ~= 0 and string.format(' extra_bits=0x%03X', extra)
                           or '',
                       key == KEY_NONE and 'none' or show(key, '0x%04X'),
                       (key == nil or key == KEY_NONE) and '-'
                           or note == KEY_NONE and 'none' or show(note),
                       entrySource[bank * BANK_ENTRIES + index] or 'unknown',
                       who, args))
end

local function poll()
    if done then return end
    frames = frames + 1
    if callbackError ~= nil then
        finish('callback error')
        return
    end
    local mask = activeMask()
    if scene == nil and mask:find('%d') then
        if sceneCount < MAX_SCENES then
            sceneCount = sceneCount + 1
            scene = {number = sceneCount, seen = {}}
            live(string.format('SCENE START active=%s mode=%s', mask,
                               show(u8(MAIN_MODE))))
        end
    elseif scene ~= nil and not mask:find('%d') then
        live('SCENE END')
        scene = nil
    end
    local origins = pendingLoads
    pendingLoads = {}
    pollBanks(origins)
    if sceneCount >= MAX_SCENES and scene == nil then
        finish('captured ' .. MAX_SCENES .. ' scenes')
    elseif firstLoadFrame ~= nil
        and frames - firstLoadFrame >= WINDOW_FRAMES then
        finish('four-minute window after the first LOAD ended')
    elseif frames >= NO_CONTROL_FRAMES and controlHits == 0 then
        finish('five minutes without a control hit; the setup is not '
               .. 'working, check the setup line')
    elseif frames >= TIMEOUT_FRAMES then
        finish('timed out')
    end
end

local function guarded(handler)
    return function()
        if done then return end
        local ok, err = pcall(handler)
        if not ok and callbackError == nil then callbackError = tostring(err) end
    end
end

local function add(target, handler)
    breakpoints_sound_lookup_bank_watch[#breakpoints_sound_lookup_bank_watch + 1] =
        traceAddBreakpoint(target[1], 'Exec', 4, SCRIPT_NAME .. ': ' .. target[3],
                           guarded(handler))
end

local function check(target)
    local word = u32(target[1])
    if word ~= target[2] then
        mismatches[#mismatches + 1] = string.format(
            '%s 0x%08X expected 0x%08X read %s', target[3], target[1],
            target[2], show(word, '0x%08X'))
    end
end

for _, target in ipairs({CONTROL, LOAD_BANK, SE_PLAY_FULL, SLOT_SOUNDS}) do
    check(target)
end
for _, target in ipairs(SE_FUNCTIONS) do check(target) end

add(CONTROL, function() controlHits = controlHits + 1 end)
add(LOAD_BANK, onLoad)
add(SE_PLAY_FULL, function()
    lastFullCaller = tonumber(PCSX.getRegisters().GPR.n.ra)
end)
add(SLOT_SOUNDS, function()
    local slot = tonumber(PCSX.getRegisters().GPR.n.a0)
    slotSoundCalls[slot] = (slotSoundCalls[slot] or 0) + 1
end)
for _, target in ipairs(SE_FUNCTIONS) do
    local name = target[3]
    add(target, function() onTagged(name) end)
end

for b = 0, 1 do
    bankKeys[b] = readBank(b)
    banks[b] = signature(bankKeys[b])
end

listeners_sound_lookup_bank_watch = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        local ok, err = pcall(poll)
        if not ok then finish('script error: ' .. tostring(err)) end
    end),
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        finish('reset; reload the script and start again')
    end),
}

print(string.format('%s: %d breakpoints installed, %d signature mismatches; '
                    .. 'start a Free Duel and play a few attacks',
                    SCRIPT_NAME, #breakpoints_sound_lookup_bank_watch,
                    #mismatches))
