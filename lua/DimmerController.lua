CDimmerController = {}
CDimmerController.__index = CDimmerController

function CDimmerController:__call(event)
	if event == EV_KEY_RELEASE then
		if not self.gradual then
			self.switch:toggle()
		else
			self.gradual = false
		end
	elseif event == EV_KEY_REPEAT then
		local state = self.switch:get_state()

		if not self.gradual then
			if state == 100 then
				self.dim_up = false
			elseif state == 0 then
				self.dim_up = true
			else
				self.dim_up = not self.dim_up
			end
			self.gradual = true
		end
		if self.dim_up then
			if state < 100 then self.switch:inc(self.interval) end
		else
			if state > 0 then self.switch:dec(self.interval) end
		end
	end
end

function CDimmerController.new(switch)
   local s = {}
   setmetatable(s, CDimmerController)
   s.switch = switch
   s.gradual = false
   s.dim_up = false
   return s
end

function DimmerController(t)
	assert(t.switch, "DimmerController: Missing switch")
	assert(switches[t.switch], "DimmerController: Unknown switch " .. t.switch)
	assert(t.keyboard, "DimmerController: Missing keyboard")
	assert(t.key, "DimmerController: Missing key")

	local c = CDimmerController.new(switches[t.switch])
	c.interval = math.floor(t.interval)
	register_event_handler(t.keyboard, t.key, c)
end
