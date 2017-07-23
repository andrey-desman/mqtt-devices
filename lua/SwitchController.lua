CSwitchController = {}
CSwitchController.__index = CSwitchController

function CSwitchController:__call(event)
	if event == EV_KEY_PRESS then
		self.switch:toggle()
	end
end

function CSwitchController.new(switch)
   local s = {}
   setmetatable(s, CSwitchController)
   s.switch = switch
   return s
end

function SwitchController(t)
	assert(t.switch, "SwitchController: Missing switch")
	assert(switches[t.switch], "SwitchController: Unknown switch " .. t.switch)
	assert(t.keyboard, "SwitchController: Missing keyboard")
	assert(t.key, "SwitchController: Missing key")

	local c = CSwitchController.new(switches[t.switch])
	register_event_handler(t.keyboard, t.key, c)
end
