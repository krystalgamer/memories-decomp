-- text_stream_owner_watch.lua
--
-- WHAT THIS ANSWERS
--   src/ygo_types.h declares TextStreamOwner as 22 stream pointers
--   (TEXT_STREAM_SLOT_COUNT) followed by the signed selector stream_index at
--   +0x58, a narrow view over DuelEffectChannel (gText_aChannels, 0x800EB0F8,
--   four 0x64-byte records). src/game/duel_effect_text.c reaches it only
--   through the TEXT_STREAM_OWNER / TEXT_STREAM_OWNER_VIEW casts, because
--   DuelEffectChannel declares root_stream at +0x00 and then unrelated
--   fields (field_04.., local_x/y, entry_write_cursor at +0x20,
--   render_object at +0x28, decimal_buffer at +0x44 ...) over the same
--   0x00-0x57 bytes. The doubt: is that region really a stack of text
--   streams selected by stream_index (pushed for nested strings such as a
--   card name or a formatted decimal, popped by the FF terminator), how deep
--   does it go in practice, and what kind of address does each live slot
--   hold (resident data, an overlay, the tail string banks, the channel's
--   own decimal_buffer, scratchpad)? The answer decides whether
--   DuelEffectChannel gets a real `u8 *streams[N]` member in an anonymous
--   union over its prefix (with N the observed depth bound) and the casts
--   TEXT_STREAM_OWNER / TEXT_STREAM_OWNER_VIEW disappear, or whether
--   TextStreamOwner must stay a separate view because the slots above the
--   live depth are never streams.
--
-- WHAT IS WATCHED (resident code only; each entry is checked against its
-- first two retail words on every hit and a mismatching hit is refused)
--   * TextBox_BuildStep          0x800393B0  steps one channel; reads
--                                            streams[stream_index]
--   * Text_ExpandSelectedCardInfo 0x80037DA4 pushes a card-name/type string
--   * Text_PushFormattedDecimal  0x80038148  pushes decimal_buffer (a number)
--   * Text_PushStagedString      0x800383DC  pushes gText_StagedStringId
--   * Text_PushStreamOffset      0x80038DB8  FC: pushes a same-bank offset
--   * Text_EndStream             0x80038E7C  FF: pops one level
--   * TextStream_ReadU32LE       0x80036D70  reader, TextStreamOwner *a0
--   * TextStream_ReadByte        0x8003B7E0  reader, TextStreamOwner *a0
--   * control: Main_RunFrameServices 0x8001306C
--   On each hit a0 is the owner. It is refused unless it is non-null,
--   word-aligned and the whole 0x64-byte record lies in 2 MB of RAM. Then
--   stream_index (+0x58) and the 22 words at +0x00..+0x54 are read; each
--   non-null word is classified (self+0xNN, resident, overlay slot, tail
--   string banks 0x801AC000+, ram-high, scratchpad, other) and counted per
--   slot index separately for live slots (index <= stream_index) and the
--   slots above it. The selected stream's first 8 bytes are dumped only
--   when it is a RAM pointer. Rows are printed live with a time stamp: the
--   first sight of each owner, every change of an owner's stream_index
--   (with the function that saw it and the live slots), and every push/pop
--   call with its operands (staged string id, decimal value address/format/
--   value, card-info op byte, FC offset). Output is bounded: 240 rows, 6
--   repeats per distinct transition, 40 rows per push/pop function, 16
--   owners, 4 example values per slot; everything beyond is counted.
--
-- HOW TO RUN (about three minutes)
--   1. PCSX-Redux with the game, debugger enabled, interpreter CPU (restart
--      the emulator if you switch).
--   2. Boot to the main menu. Debug -> Lua editor, paste this file. It
--      prints "armed".
--   3. Start a Free Duel (or a campaign duel). Play until card battle or
--      effect text appears, ideally a message that names a card and a
--      number (an attack, damage or ATK/LP message). If time is left, open
--      a card's text with the info view.
--   4. The document prints by itself three minutes after arming. Copy it
--      into tools/trace/result/text_stream_owner_watch.txt and fill in the
--      context.
--
-- WHAT TO WRITE IN THE CONTEXT
--   One or two lines: which messages appeared and roughly when (seconds
--   since arming), e.g. "t=40s 'Blue-Eyes attacks', t=55s '1200 damage'".

local ffi = require('ffi')

local SCRIPT_NAME = 'text_stream_owner_watch'
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

local FPS = 60
local WINDOW_FRAMES = 180 * FPS
local RAM_BASE = 0x80000000
-- The game runs in 2 MB; only addresses whose record lies wholly inside it
-- are dereferenced.
local RAM_SIZE = 0x200000
local CHANNEL_SIZE = 0x64
local SLOT_COUNT = 22
local INDEX_OFFSET = 0x58
local CHANNELS = 0x800eb0f8
local CHANNEL_COUNT = 4
local STAGED_STRING_ID = 0x8009b32e
local SELECTED_CARD_ID = 0x8009b338

local MAX_ROWS = 240
local MAX_ROWS_PER_FN = 40
local MAX_TRANSITION_REPEATS = 6
local MAX_OWNERS = 16
local MAX_EXAMPLES = 4
local MAX_REFUSAL_ROWS = 8

-- Address, first two retail words, name, kind.
local CONTROL = {0x8001306c, 0x27bdffe0, 0xafbf0018, 'Main_RunFrameServices',
                 'control'}
local FUNCTIONS = {
    {0x800393b0, 0x27bdffe0, 0xafb00010, 'TextBox_BuildStep', 'step'},
    {0x80037da4, 0x27bdffe8, 0xafb00010, 'Text_ExpandSelectedCardInfo',
     'push'},
    {0x80038148, 0x27bdffd8, 0xafb1001c, 'Text_PushFormattedDecimal', 'push'},
    {0x800383dc, 0x97860426, 0x3402cfff, 'Text_PushStagedString', 'push'},
    {0x80038db8, 0x27bdffe8, 0xafb00010, 'Text_PushStreamOffset', 'push'},
    {0x80038e7c, 0x90820058, 0x00000000, 'Text_EndStream', 'pop'},
    {0x80036d70, 0x80820058, 0x00000000, 'TextStream_ReadU32LE', 'reader'},
    {0x8003b7e0, 0x80820058, 0x00000000, 'TextStream_ReadByte', 'reader'},
}

local mem = PCSX.getMemPtr()

local function ramOffset(addr, length)
    if type(addr) ~= 'number' then return nil end
    local seg = addr - addr % 0x20000000
    if seg ~= 0 and seg ~= RAM_BASE and seg ~= 0xa0000000 then return nil end
    local off = addr % 0x20000000
    if off + (length or 1) > RAM_SIZE then return nil end
    return off
end
local function u8(addr)
    local off = ramOffset(addr, 1)
    if mem == nil or off == nil then return nil end
    return tonumber(ffi.cast('uint8_t*', mem + off)[0])
end
local function u16(addr)
    local off = ramOffset(addr, 2)
    if mem == nil or off == nil or off % 2 ~= 0 then return nil end
    return tonumber(ffi.cast('uint16_t*', mem + off)[0])
end
local function u32(addr)
    local off = ramOffset(addr, 4)
    if mem == nil or off == nil or off % 4 ~= 0 then return nil end
    return tonumber(ffi.cast('uint32_t*', mem + off)[0])
end
local function s32(addr)
    local v = u32(addr)
    if v ~= nil and v >= 0x80000000 then v = v - 0x100000000 end
    return v
end
local function s8(addr)
    local v = u8(addr)
    if v ~= nil and v >= 0x80 then v = v - 0x100 end
    return v
end
-- Little-endian, unaligned: stream cursors point at byte code.
local function readLE(addr, width)
    local value = 0
    for k = width - 1, 0, -1 do
        local b = u8(addr + k)
        if b == nil then return nil end
        value = value * 0x100 + b
    end
    return value
end
local function hex8(v)
    if v == nil then return '????????' end
    return string.format('%08X', v)
end

local REGIONS = {
    {0x80010000, 0x800129d8, 'resident-rodata'},
    {0x800129d8, 0x800906e0, 'resident-text'},
    {0x800906e0, 0x8009b090, 'resident-data'},
    {0x8009b090, 0x800fe728, 'resident-bss'},
    {0x800fe728, 0x8013a000, 'resident-gap'},
    {0x8013a000, 0x80146000, 'ovl-8013A000'},
    {0x80146000, 0x80168000, 'ovl-80146000'},
    {0x80168000, 0x8017a000, 'ovl-80168000'},
    {0x8017a000, 0x80180000, 'ovl-8017A000'},
    {0x80180000, 0x801ac000, 'ovl-80180000'},
    {0x801ac000, 0x801e0000, 'tail-banks'},
    {0x801e0000, 0x80200000, 'ram-high'},
    {0x80000000, 0x80010000, 'kernel'},
    {0x1f800000, 0x1f800400, 'scratchpad'},
}

local function classify(value, owner)
    if owner ~= nil and value >= owner and value < owner + CHANNEL_SIZE then
        return string.format('self+0x%02X', value - owner)
    end
    for _, r in ipairs(REGIONS) do
        if value >= r[1] and value < r[2] then return r[3] end
    end
    return 'other'
end
-- The class name without the self offset, for histograms.
local function classKey(class)
    if class:sub(1, 5) == 'self+' then
        if class == 'self+0x44' then return 'self(decimal_buffer)' end
        return 'self'
    end
    return class
end

local function ownerName(addr)
    local d = addr - CHANNELS
    if d >= 0 and d < CHANNEL_COUNT * CHANNEL_SIZE and d % CHANNEL_SIZE == 0 then
        return string.format('gText_aChannels[%d]', d / CHANNEL_SIZE)
    end
    return 'not-gText_aChannels'
end

local function moduleAt(pc)
    if pc >= 0x800129d8 and pc < 0x800906e0 then return 'resident' end
    for _, r in ipairs(REGIONS) do
        if r[3]:sub(1, 4) == 'ovl-' and pc >= r[1] and pc < r[2] then
            return r[3]
        end
    end
    return 'other'
end

-- Capture state.
local vsyncs, controlHits = 0, 0
local rows, dropped = {}, 0
local fnStats = {}
for _, f in ipairs(FUNCTIONS) do
    fnStats[f[4]] = {hits = 0, accepted = 0, bad_channel = 0, bad_signature = 0,
                     rows = 0}
end
local owners, ownerOrder, untrackedOwnerHits = {}, {}, 0
local slotLive, slotAbove = {}, {}
for k = 0, SLOT_COUNT - 1 do
    slotLive[k] = {count = 0, classes = {}, examples = {}}
    slotAbove[k] = {count = 0, classes = {}}
end
local transitionSeen, transitionsTotal, transitionsSuppressed = {}, 0, 0
local maxIndex, minIndex = nil, nil
local badIndex = 0
local refusalRows = 0
local callbackError = nil
local done = false
local resetSeen = false

breakpoints_text_stream_owner_watch = {}

local function stamp()
    return string.format('vs=%d t=%6.1fs', vsyncs, vsyncs / FPS)
end

local function live(text)
    if #rows < MAX_ROWS then
        rows[#rows + 1] = text
        print(text)
        if #rows == MAX_ROWS then
            print(SCRIPT_NAME .. ': row budget of ' .. MAX_ROWS .. ' reached; '
                  .. 'further rows are only counted')
        end
    else
        dropped = dropped + 1
    end
end

local function signatureOk(f)
    return u32(f[1]) == f[2] and u32(f[1] + 4) == f[3]
end

local function signatureCheck()
    local bad = {}
    local all = {CONTROL}
    for _, f in ipairs(FUNCTIONS) do all[#all + 1] = f end
    for _, f in ipairs(all) do
        if not signatureOk(f) then
            bad[#bad + 1] = string.format('signature %s 0x%08X expected '
                .. '%08X %08X read %s %s MISMATCH', f[4], f[1], f[2], f[3],
                hex8(u32(f[1])), hex8(u32(f[1] + 4)))
        end
    end
    return bad, #all
end

-- Why an owner pointer cannot be dereferenced, or nil when it can.
local function badOwner(a0)
    if a0 == 0 then return 'null' end
    if a0 % 4 ~= 0 then return 'unaligned' end
    if ramOffset(a0, CHANNEL_SIZE) == nil then return 'outside RAM' end
    return nil
end

local function liveSlots(owner, slots, index)
    local parts = {}
    for k = 0, math.min(index, SLOT_COUNT - 1) do
        local v = slots[k]
        parts[#parts + 1] = string.format('%d=%s(%s)', k, hex8(v),
            v == 0 and 'null' or classify(v, owner))
    end
    if #parts == 0 then return 'none' end
    return table.concat(parts, ' ')
end

local function preview(cursor)
    if cursor == nil or cursor == 0 or ramOffset(cursor, 8) == nil then
        return 'not-RAM'
    end
    local parts = {}
    for k = 0, 7 do parts[#parts + 1] = string.format('%02X', u8(cursor + k)) end
    return table.concat(parts, ' ')
end

local function selected(slots, index)
    if index < 0 or index >= SLOT_COUNT then return nil end
    return slots[index]
end

-- Operands the push/pop functions are about to consume.
local function detail(name, owner, slots, index)
    local cursor = selected(slots, index)
    local readable = cursor ~= nil and cursor ~= 0
        and ramOffset(cursor, 5) ~= nil
    if name == 'Text_PushStagedString' then
        local id = u16(STAGED_STRING_ID)
        return 'staged_id=' .. (id and string.format('0x%04X', id) or '??')
    elseif name == 'Text_PushFormattedDecimal' then
        if not readable then return 'operands=unreadable' end
        local va = readLE(cursor, 4)
        local fmt = u8(cursor + 4)
        local value = (va % 4 == 0) and s32(va) or nil
        return string.format('value_addr=%s(%s) format=0x%02X value=%s',
            hex8(va), classify(va, owner), fmt,
            value and tostring(value) or 'unreadable')
    elseif name == 'Text_ExpandSelectedCardInfo' then
        local id = u16(SELECTED_CARD_ID)
        return string.format('op=%s card=%s',
            readable and string.format('0x%02X', u8(cursor)) or '??',
            id and tostring(id) or '??')
    elseif name == 'Text_PushStreamOffset' then
        if not readable then return 'offset=unreadable' end
        return string.format('offset=0x%04X', readLE(cursor, 2))
    end
    return ''
end

local function ownerEntry(a0)
    local e = owners[a0]
    if e ~= nil then return e end
    if #ownerOrder >= MAX_OWNERS then return nil end
    e = {hits = 0, byFn = {}, minIndex = nil, maxIndex = nil, last = nil,
         transitions = 0, firstVs = vsyncs}
    owners[a0] = e
    ownerOrder[#ownerOrder + 1] = a0
    return e
end

local function onHit(f)
    local name = f[4]
    local st = fnStats[name]
    st.hits = st.hits + 1
    if not signatureOk(f) then
        st.bad_signature = st.bad_signature + 1
        if st.bad_signature == 1 then
            live(string.format('%s REFUSED %s: entry words %s %s are not the '
                .. 'retail %08X %08X', stamp(), name, hex8(u32(f[1])),
                hex8(u32(f[1] + 4)), f[2], f[3]))
        end
        return
    end
    local n = PCSX.getRegisters().GPR.n
    local a0 = (tonumber(n.a0) or 0) % 0x100000000
    local ra = (tonumber(n.ra) or 0) % 0x100000000
    local why = badOwner(a0)
    if why ~= nil then
        st.bad_channel = st.bad_channel + 1
        if refusalRows < MAX_REFUSAL_ROWS then
            refusalRows = refusalRows + 1
            live(string.format('%s REFUSED %s owner=0x%08X (%s) ra=0x%08X',
                               stamp(), name, a0, why, ra))
        end
        return
    end
    st.accepted = st.accepted + 1

    local index = s8(a0 + INDEX_OFFSET)
    local slots = {}
    for k = 0, SLOT_COUNT - 1 do slots[k] = u32(a0 + 4 * k) or 0 end
    if index < 0 or index >= SLOT_COUNT then badIndex = badIndex + 1 end
    if maxIndex == nil or index > maxIndex then maxIndex = index end
    if minIndex == nil or index < minIndex then minIndex = index end
    for k = 0, SLOT_COUNT - 1 do
        local v = slots[k]
        if v ~= 0 then
            local class = classKey(classify(v, a0))
            local bucket = k <= index and slotLive[k] or slotAbove[k]
            bucket.count = bucket.count + 1
            bucket.classes[class] = (bucket.classes[class] or 0) + 1
            if k <= index and #bucket.examples < MAX_EXAMPLES then
                local known = false
                for _, x in ipairs(bucket.examples) do
                    if x == v then known = true end
                end
                if not known then bucket.examples[#bucket.examples + 1] = v end
            end
        end
    end

    local e = ownerEntry(a0)
    if e == nil then
        untrackedOwnerHits = untrackedOwnerHits + 1
    else
        e.hits = e.hits + 1
        e.byFn[name] = (e.byFn[name] or 0) + 1
        if e.minIndex == nil or index < e.minIndex then e.minIndex = index end
        if e.maxIndex == nil or index > e.maxIndex then e.maxIndex = index end
        if e.last == nil then
            live(string.format('%s OWNER 0x%08X %s first seen in %s ra=0x%08X '
                .. 'in=%s idx=%d live=[%s] sel=%s', stamp(), a0, ownerName(a0),
                name, ra, moduleAt(ra), index, liveSlots(a0, slots, index),
                preview(selected(slots, index))))
        elseif e.last ~= index then
            e.transitions = e.transitions + 1
            transitionsTotal = transitionsTotal + 1
            local key = string.format('%08X %d>%d %s', a0, e.last, index, name)
            local seen = (transitionSeen[key] or 0) + 1
            transitionSeen[key] = seen
            if seen <= MAX_TRANSITION_REPEATS then
                live(string.format('%s IDX 0x%08X %s %d -> %d seen-in=%s '
                    .. 'ra=0x%08X live=[%s] sel=%s', stamp(), a0,
                    ownerName(a0), e.last, index, name, ra,
                    liveSlots(a0, slots, index),
                    preview(selected(slots, index))))
            else
                transitionsSuppressed = transitionsSuppressed + 1
            end
        end
        e.last = index
    end

    if f[5] == 'push' or f[5] == 'pop' then
        if st.rows < MAX_ROWS_PER_FN then
            st.rows = st.rows + 1
            live(string.format('%s CALL %s owner=0x%08X %s ra=0x%08X in=%s '
                .. 'idx=%d %s live=[%s] sel=%s', stamp(), name, a0,
                ownerName(a0), ra, moduleAt(ra), index,
                detail(name, a0, slots, index), liveSlots(a0, slots, index),
                preview(selected(slots, index))))
        else
            dropped = dropped + 1
        end
    end
end

local function histogram(classes)
    local keys = {}
    for k in pairs(classes) do keys[#keys + 1] = k end
    table.sort(keys)
    local parts = {}
    for _, k in ipairs(keys) do
        parts[#parts + 1] = k .. '=' .. classes[k]
    end
    if #parts == 0 then return '-' end
    return table.concat(parts, ' ')
end

local function verdict(signatureBad)
    local textHits = 0
    for _, f in ipairs(FUNCTIONS) do
        textHits = textHits + fnStats[f[4]].hits
    end
    if controlHits == 0 and textHits == 0 then
        return 'nothing fired, not even the Main_RunFrameServices control: '
            .. 'the setup failed (debugger off or dynarec still running); '
            .. 'silence is not evidence'
    end
    local accepted = 0
    for _, f in ipairs(FUNCTIONS) do
        accepted = accepted + fnStats[f[4]].accepted
    end
    if accepted == 0 then
        if signatureBad > 0 then
            return 'inconclusive: US signature mismatch; every text hit was '
                .. 'refused'
        end
        return 'inconclusive: the control fired but no text owner was '
            .. 'accepted; the route never showed text'
    end
    local used = {}
    for k = 0, SLOT_COUNT - 1 do
        if slotLive[k].count > 0 then used[#used + 1] = tostring(k) end
    end
    local pushes = 0
    for _, f in ipairs(FUNCTIONS) do
        if f[5] == 'push' then pushes = pushes + fnStats[f[4]].accepted end
    end
    local text = string.format('stream_index ranged %d..%d over %d accepted '
        .. 'hits; live slots used: %s; pushes=%d pops=%d transitions=%d; '
        .. 'out-of-range stream_index=%d', minIndex, maxIndex, accepted,
        #used > 0 and table.concat(used, ',') or 'none', pushes,
        fnStats.Text_EndStream.accepted, transitionsTotal, badIndex)
    if signatureBad > 0 then
        text = text .. '; WARNING: US signature mismatch at the end'
    end
    if pushes == 0 then
        text = text .. '; no nested string was pushed, so the route did not '
            .. 'reach a message with an inserted name or number'
    end
    return text
end

local function finish(reason)
    if done then return end
    done = true
    for _, bp in ipairs(breakpoints_text_stream_owner_watch) do
        pcall(function() bp:disable() end)
    end
    local bad, total = signatureCheck()
    print('')
    print('==== USER CONTEXT ====')
    print('')
    print('<which messages appeared and roughly when (seconds since arming);')
    print(' did you open a card text?>')
    print('')
    print('==== TRACE RESULT =====')
    print('')
    print('script: ' .. SCRIPT_NAME)
    print('setup: ' .. TRACE_SETUP .. ' ' .. traceExecutionStatus())
    print('status: ' .. reason)
    print(string.format('vsyncs: %d control_hits: %d reset: %s', vsyncs,
                        controlHits, resetSeen and 'yes' or 'no'))
    print('verdict: ' .. verdict(#bad))
    print(string.format('signatures: %d/%d US words ok at end', total - #bad,
                        total))
    for _, line in ipairs(bad) do print(line) end
    if callbackError ~= nil then print('callback_error: ' .. callbackError) end
    print(string.format('bounds: rows=%d/%d dropped_rows=%d owners=%d/%d '
        .. 'untracked_owner_hits=%d transitions_suppressed=%d', #rows,
        MAX_ROWS, dropped, #ownerOrder, MAX_OWNERS, untrackedOwnerHits,
        transitionsSuppressed))
    print('')
    for _, f in ipairs(FUNCTIONS) do
        local st = fnStats[f[4]]
        print(string.format('fn %s 0x%08X hits=%d accepted=%d '
            .. 'refused_owner=%d refused_signature=%d', f[4], f[1], st.hits,
            st.accepted, st.bad_channel, st.bad_signature))
    end
    print('')
    for _, a0 in ipairs(ownerOrder) do
        local e = owners[a0]
        local fns = {}
        for _, f in ipairs(FUNCTIONS) do
            if e.byFn[f[4]] then
                fns[#fns + 1] = f[4] .. '=' .. e.byFn[f[4]]
            end
        end
        print(string.format('owner 0x%08X %s hits=%d idx=%d..%d last=%d '
            .. 'transitions=%d first_vs=%d %s', a0, ownerName(a0), e.hits,
            e.minIndex, e.maxIndex, e.last, e.transitions, e.firstVs,
            table.concat(fns, ' ')))
    end
    print('')
    for k = 0, SLOT_COUNT - 1 do
        local l, a = slotLive[k], slotAbove[k]
        if l.count + a.count > 0 then
            local ex = {}
            for _, v in ipairs(l.examples) do ex[#ex + 1] = hex8(v) end
            print(string.format('slot %02d +0x%02X live=%d {%s} examples=%s | '
                .. 'above=%d {%s}', k, 4 * k, l.count, histogram(l.classes),
                #ex > 0 and table.concat(ex, ',') or '-', a.count,
                histogram(a.classes)))
        end
    end
    print('')
    for _, line in ipairs(rows) do print(line) end
    print('')
    print('--- end of trace, copy everything above into '
          .. 'tools/trace/result/' .. SCRIPT_NAME .. '.txt ---')
end

local function guarded(handler)
    return function(...)
        if done then return end
        local ok, err = pcall(handler, ...)
        if not ok and callbackError == nil then callbackError = tostring(err) end
    end
end

local function add(f, handler)
    local list = breakpoints_text_stream_owner_watch
    list[#list + 1] = traceAddBreakpoint(f[1], 'Exec', 4,
                                         SCRIPT_NAME .. ': ' .. f[4],
                                         guarded(handler))
end

add(CONTROL, function() controlHits = controlHits + 1 end)
for _, f in ipairs(FUNCTIONS) do
    add(f, function() onHit(f) end)
end

listeners_text_stream_owner_watch = {
    PCSX.Events.createEventListener('GPU::Vsync', function()
        local ok, err = pcall(function()
            if done then return end
            vsyncs = vsyncs + 1
            if callbackError ~= nil then
                finish('callback error')
            elseif vsyncs >= WINDOW_FRAMES then
                finish(string.format('%d-second window ended',
                                     WINDOW_FRAMES / FPS))
            end
        end)
        if not ok then finish('script error: ' .. tostring(err)) end
    end),
    -- The route needs no reset. PCSX-Redux drops Lua breakpoints on one, so
    -- the capture ends there rather than going silent.
    PCSX.Events.createEventListener('ExecutionFlow::Reset', function()
        pcall(function()
            if done then return end
            resetSeen = true
            live(stamp() .. ' reset observed; breakpoints are gone, ending')
            finish('console reset; capture ended early')
        end)
    end),
}

print(string.format('%s: %d breakpoints installed', SCRIPT_NAME,
                    #breakpoints_text_stream_owner_watch))
do
    local bad, total = signatureCheck()
    print(string.format('%s: signatures %d/%d US words ok', SCRIPT_NAME,
                        total - #bad, total))
    for _, line in ipairs(bad) do print(SCRIPT_NAME .. ': ' .. line) end
end
print(SCRIPT_NAME .. ': armed. Route: from the main menu start a Free Duel '
      .. '(or a campaign duel) and play until battle or effect text names a '
      .. 'card and a number (attack, damage, ATK/LP); if time is left open a '
      .. 'card\'s text. Note the time stamps of the messages. The document '
      .. 'prints by itself ' .. (WINDOW_FRAMES / FPS) .. ' seconds after '
      .. 'arming.')
