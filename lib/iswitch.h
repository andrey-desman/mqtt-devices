#pragma once

#include <cstddef>

class iswitch
{
public:
	virtual ~iswitch() { }
	virtual size_t get_channel_count() = 0;
	virtual size_t get_channel_state(size_t channel) = 0;
	virtual void set_channel_state(size_t channel, size_t value) = 0;
};

