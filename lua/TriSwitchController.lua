CTriSwitchController = {}
CTriSwitchController.__index = CTriSwitchController

function CTriSwitchController:__call(event)
	if event == EV_KEY_PRESS then
		self.is_active = true
	elseif event == EV_KEY_RELEASE then
		if self.is_active then self.switch:set(self.switch:get_state() == 0 and self.lo or 0) end
		self.is_active = false
	elseif event == EV_KEY_REPEAT then
		if self.is_active then self.switch:set(self.hi) end
		self.is_active = false
	end
end

function CTriSwitchController.new(switch, invert)
   local s = {}
   setmetatable(s, CTriSwitchController)
   s.switch = switch
   if invert then
	   s.lo = 2
	   s.hi = 1
   else
	   s.lo = 1
	   s.hi = 2
   end
   s.is_active = false
   return s
end

function TriSwitchController(t)
	assert(t.switch, "TriSwitchController: Missing switch")
	assert(switches[t.switch], "TriSwitchController: Unknown switch " .. t.switch)
	assert(t.keyboard, "TriSwitchController: Missing keyboard")
	assert(t.key, "TriSwitchController: Missing key")

	local c = CTriSwitchController.new(switches[t.switch], t.invert)
	register_event_handler(t.keyboard, t.key, c)
end
