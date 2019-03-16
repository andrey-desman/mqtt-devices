switches = {}

CSwitch = {}
CSwitch.__index = CSwitch

function CSwitch.new(name, device, channel)
   local s = {}
   setmetatable(s, CSwitch)
   s.name = name
   s.device = device
   s.channel = channel
   s.state = 0
   return s
end

function CSwitch.handle_message(device, channel, subject, payload)
	local s = switches[id(device, channel)]
	if s == nil then
		return
	end

	if subject == 'command' then
		s:handle_command(payload)
	elseif subject == 'state' then
		s:handle_state(payload)
	end
end

function CSwitch:handle_state(state)
	local state = math.tointeger(state)

	if state == nil then
		Log.log('switch ' .. self:pretty_name() .. ': invalid state received: ' .. state)
		return
	else
		Log.log('switch ' .. self:pretty_name() .. ': ' .. self.state .. ' -> ' .. state)
	end
	self.state = state

	if self.on_state_changed then
		self:on_state_changed(state)
	end
end

function CSwitch:handle_command(command)
	Log.log('switch ' .. self:pretty_name() .. ': got command: ' .. command)
	if self.on_command then
		self:on_command(command)
	end
end

function CSwitch:send_command(command)
	Mqtt.post(self.device .. "/switch/" .. self.channel .. "/command", command, false)
end

function CSwitch:toggle()
	self:send_command('toggle')
end

function CSwitch:on()
	self:send_command('on')
end

function CSwitch:off()
	self:send_command('off')
end

function CSwitch:set(value)
	self:send_command('set ' .. value)
end

function CSwitch:inc(delta)
	delta = delta or 10
	self:send_command('inc ' .. delta)
end

function CSwitch:dec(delta)
	delta = delta or 10
	self:send_command('dec ' .. delta)
end

function CSwitch:get_state()
	return self.state
end

function CSwitch:get_name()
	return self.name
end

function CSwitch:get_device()
	return self.device
end

function CSwitch:get_channel()
	return self.channel
end

function CSwitch:pretty_name()
	return self.name .. '(' .. self.device .. '/' .. self.channel .. ')'
end

function Switch(t)
	assert(t.name, "Switch: Missing name")
	assert(t.device, "SwitchController: Missing device")
	assert(t.channel, "SwitchController: Missing channel")

	s = CSwitch.new(t.name, t.device, t.channel)
	switches[t.name] = s
	switches[id(t.device, t.channel)] = s
end
