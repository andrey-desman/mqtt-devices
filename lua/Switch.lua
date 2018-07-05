switches = {}

CSwitch = {}
CSwitch.__index = CSwitch

function CSwitch:__call(state)
	self.state = state

	if self.on_state_changed then
		self:on_state_changed(state)
	end
end

function CSwitch.new(name, device, channel)
   local s = {}
   setmetatable(s, CSwitch)
   s.name = name
   s.device = device
   s.channel = channel
   s.state = 0
   return s
end

function CSwitch:toggle()
	bus.send_switch_command(self.device, self.channel, 'toggle')
end

function CSwitch:on()
	bus.send_switch_command(self.device, self.channel, 'on')
end

function CSwitch:off()
	bus.send_switch_command(self.device, self.channel, 'off')
end

function CSwitch:set(value)
	bus.send_switch_command(self.device, self.channel, 'set ' .. value)
end

function CSwitch:inc(delta)
	delta = delta or 10
	bus.send_switch_command(self.device, self.channel, 'inc ' .. delta)
end

function CSwitch:dec(delta)
	delta = delta or 10
	bus.send_switch_command(self.device, self.channel, 'dec ' .. delta)
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

	register_switch_state_handler(t.device, t.channel, s)
end

