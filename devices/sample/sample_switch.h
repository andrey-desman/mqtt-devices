#pragma once

#include "iswitch.h"
#include <cassert>

template<int N>
class sample_switch: public iswitch
{
public:

	sample_switch()
	{
		std::fill(&state_[0], &state_[N], 0);
	}

	virtual size_t get_channel_count()
	{
		return N;
	}

	virtual size_t get_channel_state(size_t channel)
	{
		assert(channel < N);
		return state_[channel];
	}

	virtual void set_channel_state(size_t channel, size_t value)
	{
		assert(channel < N);
		state_[channel] = value;
	}

private:
	size_t state_[N];
};

