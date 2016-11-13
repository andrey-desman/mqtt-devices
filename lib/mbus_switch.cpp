#include "mbus_switch.h"

mbus_switch::mbus_switch(const std::string& server_addr, uint16_t port, mbus_uword slave_addr)
	: server_addr_(server_addr)
	, server_port_(port)
	, slave_addr_(slave_addr)
{
	mbus_init(&mbus_);
}

mbus_switch::~mbus_switch()
{
	mbus_close(&mbus_);
}

void mbus_switch::reconnect()
{
	if (mbus_connected(&mbus_))
	{
		mbus_reconnect(&mbus_);
	}
	else
	{
		mbus_connect(&mbus_, server_addr_.c_str(), server_port_, 0);
	}
}

uint16_t mbus_switch::read_register(uint16_t addr)
{
	mbus_uword reg;

	if (!mbus_connected(&mbus_))
	{
		reconnect();
	}

	if (mbus_cmd_read_holding_registers(&mbus_, slave_addr_, addr, 1, &reg) < 0)
	{
		reconnect();
		mbus_cmd_read_holding_registers(&mbus_, slave_addr_, addr, 1, &reg);
	}

	return reg;
}

void mbus_switch::write_register(uint16_t addr, uint16_t data)
{
	if (!mbus_connected(&mbus_))
	{
		reconnect();
	}

	if (mbus_cmd_preset_single_register(&mbus_, slave_addr_, addr, data) < 0)
	{
		reconnect();
		mbus_cmd_preset_single_register(&mbus_, slave_addr_, addr, data);
	}
}
