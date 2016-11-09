function on_switch_state_changed(switch, channel, state)
	io.write( "switch " .. switch .. "/" .. channel .. " state changed to " .. state .. "\n")
end

function on_key_event(keyboard, key, event)
	io.write( "event " .. event .. " on " .. keyboard .. "/" .. key .. "\n")
	if keyboard == 'kbd1' and key == 29 and event == 1 then
		bus.send_switch_command('switch1', 2, 'toggle')
	end
end
