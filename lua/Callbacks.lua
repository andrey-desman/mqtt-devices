event_handlers = {}
switch_state_handlers = {}

function id(name, idx)
	return name .. '/' .. idx
end

function register_event_handler(keyboard, key, handler)
	event_handlers[id(keyboard, key)] = handler
end

function register_switch_state_handler(switch, channel, handler)
	switch_state_handlers[id(switch, channel)] = handler
end

function on_switch_state_changed(switch, channel, state)
	local handler = switch_state_handlers[id(switch, channel)]
	if handler then handler(state) end
end

function on_key_event(keyboard, key, event)
	local handler = event_handlers[id(keyboard, key)]
	if handler then handler(event) end
end

function Mqtt.handle_message(topic, payload)
	Log.log("Got message on " .. topic .. ": " .. payload)

	p = string.gmatch(topic, '[^/]+')
	device = p()
	class = p()
	channel = math.tointeger(p())
	subject = p()

	if class == 'switch' then
		CSwitch.handle_message(device, channel, subject, payload)
	elseif class == 'event' then
		on_key_event(device, channel, math.tointeger(payload))
	end
end

Mqtt.subscribe("+/switch/#")
Mqtt.subscribe("+/event/#")
