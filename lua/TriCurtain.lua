CTriCurtain = {}
CTriCurtain.__index = CTriCurtain

function CTriCurtain:make_handler()
	return function()
		self.triswitch:off()
	end
end

function CTriCurtain.new(name, device, channel, timeout, invert, triswitch)
   local s = {}
   setmetatable(s, CTriCurtain)
   s.name = name
   s.device = device
   s.channel = channel
   s.timeout = timeout
   s.invert = invert
   s.triswitch = triswitch
   s.timer = Timer.new(s:make_handler())
   return s
end

function CTriCurtain:handle_command(command)
	if self.on_command then
		self:on_command(command)
	end
	if command == 'open' then
		self:open()
	elseif command == 'close' then
		self:close()
	elseif command == 'toggle' then
		self:toggle()
	elseif command == 'stop' then
		self:stop()
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

function CTriCurtain:handle_state(state)
end

function CTriCurtain:send_state()
	Mqtt.post(self.device .. "/switch/" .. self.channel .. "/state", self.state, true)
end

function CTriCurtain:toggle()
	self:set(1 - self.state)
end

function CTriCurtain:open()
	self:set(0)
end

function CTriCurtain:close()
	self:set(1)
end

function CTriCurtain:stop()
	self.triswitch:off()
	self.timer:stop()
end

function CTriCurtain:set(value)
	if value < 0 or value > 1 then return end
	self.state = value
	self:send_state()
	local switch_value = (self.invert and 1 - value or value) + 1
	self.triswitch:set(switch_value)
	self.timer:start(self.timeout)
end

function CTriCurtain:get_state()
	return self.state
end

function CTriCurtain:get_name()
	return self.name
end

function CTriCurtain:get_device()
	return self.device
end

function CTriCurtain:get_channel()
	return self.channel
end

function CTriCurtain:pretty_name()
	return self.name .. '(' .. self.device .. '/' .. self.channel .. ')'
end

function TriCurtain(t)
	assert(t.name, "Switch: Missing name")
	assert(t.device, "Switch: Missing device")
	assert(t.channel, "Switch: Missing channel")
	assert(t.triswitch, "TriCurtain: Missing triswitch")
	assert(switches[t.triswitch], "TriCurtain: Unknown switch " .. t.triswitch)
	assert(t.timeout, "TriCurtain: Missing timeout")

	s = CTriCurtain.new(t.name, t.device, t.channel, t.timeout, t.invert, switches[t.triswitch])
	switches[t.name] = s
	switches[id(t.device, t.channel)] = s
end
