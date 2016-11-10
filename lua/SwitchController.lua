switches = {}

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

function SwitchController(switch_name, keyboard, key)
	local c = CSwitchController.new(switches[switch_name])
	register_event_handler(keyboard, key, c)
end
