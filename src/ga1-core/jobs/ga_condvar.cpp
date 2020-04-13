/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_condvar.h"

ga_condvar::ga_condvar()
{
}

ga_condvar::~ga_condvar()
{
}

void ga_condvar::wait()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_condvar.wait(lock);
}

void ga_condvar::wait_for(int ms)
{
	std::unique_lock<std::mutex> lock(_mutex);
	_condvar.wait_for(lock, std::chrono::milliseconds(ms));
}

void ga_condvar::wake_all()
{
	_condvar.notify_all();
}
