CTriSwitch = {}
CTriSwitch.__index = CTriSwitch

function CTriSwitch.new(name, device, channel, power, control, default_value)
   local s = {}
   setmetatable(s, CTriSwitch)
   s.name = name
   s.device = device
   s.channel = channel
   s.power = power
   s.control = control
   s.state = 0
   s.default_value = default_value
   s.power.on_state_changed = function(state) s:on_slave_state_changed() end
   s.control.on_state_changed = s.power.on_state_changed
   return s
end

function slave_state(state)
	return state > 0 and 1 or 0
end

function make_state(power_state, control_state)
	return slave_state(power_state) * 2 + slave_state(control_state)
end

function CTriSwitch:on_slave_state_changed()
	local state = make_state(self.power:get_state(), self.control:get_state())
	Log.log('switch ' .. self:pretty_name() .. ': slave changed: ' .. state)
	if self.awaiting_state then
		if self.awaiting_state == state then
			self.state = state
			self.awaiting_state = nil
			self:send_state()
		end
		return
	elseif self.state ~= state and state ~= 1 then
		self.state = state
		self:send_state()
	end
end

function CTriSwitch:handle_command(command)
	Log.log('switch ' .. self:pretty_name() .. ': got command: ' .. command)
	if self.on_command then
		self:on_command(command)
	end
	if command == 'on' then
		self:on()
	elseif command == 'off' then
		self:off()
	elseif command == 'toggle' then
		self:toggle()
	else
		p = string.gmatch(command, '[^ ]+')
		cmd = p()
		if cmd == 'set' then
			cmd = p()
		end
		cmd = tonumber(cmd)
		if cmd == nil then
			return
		end
		self:set(cmd)
	end
end

function CTriSwitch:handle_state(state)
end

function CTriSwitch:send_state()
	Mqtt.post(self.device .. "/switch/" .. self.channel .. "/state", self:get_state(), true)
	if self.on_state_changed then self.on_state_changed(self:get_state()) end
end

function CTriSwitch:toggle()
	self:set(self.state == 0 and self.default_value or 0)
end

function CTriSwitch:on()
	self:set(self.default_value)
end

function CTriSwitch:off()
	self:set(0)
end

-- 00 0
-- 01 x
-- 10 1
-- 11 2
function CTriSwitch:set(value)
	if value == 0 then
		self.awaiting_state = 0
		self.power:off()
		self.control:off()
	elseif value > 0 and value <= 2 then
		self.awaiting_state = value + 1
		self.control:set(value - 1)
		self.power:on()
	end
end

function CTriSwitch:get_state()
	return self.state == 0 and 0 or self.state - 1
end

function CTriSwitch:get_name()
	return self.name
end

function CTriSwitch:get_device()
	return self.device
end

function CTriSwitch:get_channel()
	return self.channel
end

function CTriSwitch:pretty_name()
	return self.name .. '(' .. self.device .. '/' .. self.channel .. ')'
end

function TriSwitch(t)
	assert(t.name, "Switch: Missing name")
	assert(t.device, "Switch: Missing device")
	assert(t.channel, "Switch: Missing channel")
	assert(t.power, "TriSwitch: Missing power")
	assert(switches[t.power], "TriSwitch: Unknown switch " .. t.power)
	assert(t.control, "TriSwitch: Missing control")
	assert(switches[t.control], "TriSwitch: Unknown switch " .. t.control)

	s = CTriSwitch.new(t.name, t.device, t.channel, switches[t.power], switches[t.control], t.default_value or 1)
	switches[t.name] = s
	switches[id(t.device, t.channel)] = s
end
