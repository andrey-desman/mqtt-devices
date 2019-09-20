CTriCurtainController = {}
CTriCurtainController.__index = CTriCurtainController

function CTriCurtainController:__call(event)
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

function CTriCurtainController.new(switch, direction)
   local s = {}
   setmetatable(s, CTriCurtainController)
   s.switch = switch
   s.direction = direction
   s.last_event = EV_KEY_RELEASE
   return s
end

function TriCurtainController(t)
	assert(t.switch, "TriCurtainController: Missing switch")
	assert(switches[t.switch], "TriCurtainController: Unknown switch " .. t.switch)
	assert(t.keyboard, "TriCurtainController: Missing keyboard")
	assert(t.key, "TriCurtainController: Missing key")
	assert(t.direction, "TriCurtainController: Missing direction")

	local c = CTriCurtainController.new(switches[t.switch], t.direction)
	register_event_handler(t.keyboard, t.key, c)
end
