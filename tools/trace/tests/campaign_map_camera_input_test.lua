-- Run from the repository root:
--   luajit tools/trace/tests/campaign_map_camera_input_test.lua
-- Replays the campaign-map breakpoints and polling listener with mock PCSX
-- callbacks and real LuaJIT FFI RAM.
local ffi = require('ffi')
local hostPrint = print
local scriptPath = SCRIPT_UNDER_TEST
    or 'tools/trace/campaign_map_camera_input.lua'

local RAM_BASE = 0x80000000
local MOVE_CAMERA = 0x80168388
local MOVE_CAMERA_SIGNATURE = 0x27bdffe8
local UPDATE_LOCATION = 0x80168fcc
local UPDATE_LOCATION_SIGNATURE = 0x27bdffe0
local MAIN_MODE = 0x8009b26c
local PAD1_HELD = 0x8009b3a4
local CAMERA = 0x800f2848

local function capture(options)
    options = options or {}
    local memory = ffi.new('uint8_t[?]', 0x200000)
    local events = {}
    local breakpoints = {}
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
        value = value % 0x100000000
        set16(addr, value % 0x10000)
        set16(addr + 2, math.floor(value / 0x10000))
    end

    set8(MAIN_MODE, options.mode or 5)
    set32(
        MOVE_CAMERA,
        options.moveSignature or MOVE_CAMERA_SIGNATURE
    )
    set32(
        UPDATE_LOCATION,
        options.updateSignature or UPDATE_LOCATION_SIGNATURE
    )
    set16(CAMERA, 0x06a4)
    set16(CAMERA + 0x02, 0x0400)
    set16(CAMERA + 0x04, 0x0180)
    set32(CAMERA + 0x1c, 0)
    set32(CAMERA + 0x24, 0)

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
    breakpoint_campaign_map_camera_move = nil
    breakpoint_campaign_map_camera_update = nil
    listener_campaign_map_camera_input_reset = nil
    listener_campaign_map_camera_input = nil
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

    function result:frames(count)
        for _ = 1, count do
            events['GPU::Vsync']()
        end
    end

    function result:call(address)
        assert(breakpoints[address] ~= nil, 'breakpoint must be installed')
        assert(breakpoints[address].enabled, 'breakpoint must be enabled')
        breakpoints[address].callback()
    end

    function result:move()
        self:call(MOVE_CAMERA)
    end

    function result:update()
        self:call(UPDATE_LOCATION)
    end

    function result:input(mask)
        set16(PAD1_HELD, mask or 0)
    end

    function result:angle(value)
        set16(CAMERA + 0x02, value)
    end

    function result:moveSignature(value)
        set32(MOVE_CAMERA, value)
    end

    function result:reset()
        events['ExecutionFlow::Reset']()
    end

    function result:disabled(address)
        return breakpoints[address] ~= nil
            and not breakpoints[address].enabled
    end

    return result
end

local live = capture()
assert(live:has('breakpoints installed'))
live:update()
live:input(0x8000)
live:frames(1)
live:move()
live:angle(0x03fe)
live:frames(1)
live:input(0)
live:frames(59)
assert(live:has(
    'status: CampaignMap_MoveCameraDpad executed; follow-up captured'
))
assert(live:has('move_entry=01'))
assert(live:has('held=0x8000'))
assert(live:has('move_delta=01'))
assert(live:has('angle=-2'))
assert(live:has('move_hits=1 active_tick_hits=1'))
assert(live:disabled(MOVE_CAMERA))
assert(live:disabled(UPDATE_LOCATION))

local idleCall = capture()
idleCall:update()
idleCall:move()
idleCall:frames(60)
assert(idleCall:has('routine_observed=true'))
assert(idleCall:has('move_entry=01 frame=000000 mode=5 held=0x0000'))

local controlledNegative = capture()
controlledNegative:update()
controlledNegative:input(0x8000)
controlledNegative:frames(1)
controlledNegative:angle(0x03fe)
controlledNegative:input(0)
controlledNegative:frames(1799)
assert(controlledNegative:has(
    'status: active map tick observed but free-camera routine was not observed'
))
assert(controlledNegative:has('input_delta=01'))
assert(controlledNegative:has('angle=-2'))
assert(controlledNegative:has('routine_observed=false'))
assert(controlledNegative:has('active_tick_observed=true'))

local setupFailure = capture()
setupFailure:frames(299)
assert(not setupFailure:has('no free-camera hit yet'))
setupFailure:frames(1)
assert(setupFailure:has('no free-camera hit yet'))
setupFailure:frames(1500)
assert(setupFailure:has(
    'status: no active-tick hit; select interpreter CPU and rerun'
))

local wrongMode = capture({mode = 8})
assert(wrongMode:has('main mode 8 is not Campaign Map mode 5'))

local wrongOverlay = capture({moveSignature = 0x12345678})
assert(wrongOverlay:has(
    'status: campaign overworld overlay signatures do not match retail'
))

local replaced = capture()
replaced:update()
replaced:frames(1)
replaced:moveSignature(0)
replaced:frames(1)
assert(replaced:has(
    'status: overworld overlay signature changed during capture'
))
assert(replaced:disabled(MOVE_CAMERA))
assert(replaced:disabled(UPDATE_LOCATION))

local reset = capture()
reset:reset()
assert(reset:has(
    'status: reset observed; rerun from the visible campaign map'
))
assert(reset:disabled(MOVE_CAMERA))
assert(reset:disabled(UPDATE_LOCATION))

local bounded = capture()
for _ = 1, 120 do
    bounded:move()
end
assert(bounded:has('status: maximum free-camera hit count reached'))
assert(bounded:has('move_hits=120'))
assert(bounded:disabled(MOVE_CAMERA))
assert(bounded:disabled(UPDATE_LOCATION))

print = hostPrint
print('campaign_map_camera_input: all nine callback-replay cases passed')
