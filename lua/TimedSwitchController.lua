CTimedSwitchController = {}
CTimedSwitchController.__index = CTimedSwitchController

function CTimedSwitchController:make_handler()
	return function()
		self:on_timer_expired()
	end
end

function CTimedSwitchController:__call(event)
	if event == EV_KEY_PRESS then
		self.switch:on()
		self.timer:start(self.timeout)
	end
end

function CTimedSwitchController:on_timer_expired()
	self.switch:off()
end

function CTimedSwitchController.new(switch, timeout)
   local s = {}
   setmetatable(s, CTimedSwitchController)
   s.switch = switch
   s.timeout = timeout
   s.timer = Timer.new(s:make_handler())
   return s
end

function TimedSwitchController(switch_name, keyboard, key, timeout)
	local c = CTimedSwitchController.new(switches[switch_name], timeout)
	register_event_handler(keyboard, key, c)
end
