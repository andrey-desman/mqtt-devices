CTimedSwitchController = {}
CTimedSwitchController.__index = CTimedSwitchController

function CTimedSwitchController:make_handler()
	return function()
		self:on_timer_expired()
	end
end

function CTimedSwitchController:__call(event)
	if event == EV_KEY_PRESS then
		if self.switch:get_state() == 0 then
			self.switch:on()
			self.timer:start(self.timeout)
			bus.log("Turning off " .. self.switch:pretty_name() .. " after " .. self.timeout .. " seconds")
		else
			self.switch:off()
			self.timer:stop()
		end
	-- Hanlding long press, but only if it was pressed to turn the switch on initially
	elseif event == EV_KEY_REPEAT and self.switch:get_state() ~= 0 and self.long_press_timeout ~= nil then
		self.timer:start(self.long_press_timeout)
		bus.log("Turning off " .. self.switch:pretty_name() .. " after " .. self.long_press_timeout .. " seconds")
	end
end

function CTimedSwitchController:on_timer_expired()
	self.switch:off()
end

function CTimedSwitchController.new(switch, timeout, long_press_timeout)
   local s = {}
   setmetatable(s, CTimedSwitchController)
   s.switch = switch
   s.timeout = timeout
   s.long_press_timeout = long_press_timeout
   s.timer = Timer.new(s:make_handler())
   return s
end

function TimedSwitchController(t)
	assert(t.switch, "TimedSwitchController: Missing switch")
	assert(switches[t.switch], "TimedSwitchController: Unknown switch " .. t.switch)
	assert(t.timeout, "TimedSwitchController: Missing timeout")
	assert(t.keyboard, "TimedSwitchController: Missing keyboard")
	assert(t.key, "TimedSwitchController: Missing key")

	local c = CTimedSwitchController.new(switches[t.switch], t.timeout, t.long_press_timeout)
	register_event_handler(t.keyboard, t.key, c)
end
