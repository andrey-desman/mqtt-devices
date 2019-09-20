CCurtainController = {}
CCurtainController.__index = CCurtainController

function CCurtainController:__call(event)
	if event == EV_KEY_RELEASE then
		if self.last_event == EV_KEY_PRESS then
			self.switch:set(self.direction)
		elseif self.last_event == EV_KEY_REPEAT then
			self.switch:stop()
		end
	elseif event == EV_KEY_REPEAT then
		if self.last_event ~= EV_KEY_REPEAT then
			self.switch:set(self.direction)
		end
	end
	self.last_event = event
end

function CCurtainController.new(switch, direction)
   local s = {}
   setmetatable(s, CCurtainController)
   s.switch = switch
   s.direction = direction
   s.last_event = EV_KEY_RELEASE
   return s
end

function CurtainController(t)
	assert(t.switch, "CurtainController: Missing switch")
	assert(switches[t.switch], "CurtainController: Unknown switch " .. t.switch)
	assert(t.keyboard, "CurtainController: Missing keyboard")
	assert(t.key, "CurtainController: Missing key")
	assert(t.direction, "CurtainController: Missing direction")

	local c = CCurtainController.new(switches[t.switch], t.direction)
	register_event_handler(t.keyboard, t.key, c)
end
