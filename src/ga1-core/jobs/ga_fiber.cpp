/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_fiber.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

ga_fiber::ga_fiber(function_t func, void* func_data, size_t stack_size)
{
	const size_t k_stack_align = 64 * 1024;
	stack_size = stack_size > k_stack_align ? stack_size : k_stack_align;
	stack_size = (stack_size + k_stack_align - 1) & ~(k_stack_align - 1);

	_impl = CreateFiber(stack_size, (LPFIBER_START_ROUTINE)func, func_data);
}

ga_fiber::~ga_fiber()
{
	if (_impl)
	{
		DeleteFiber(_impl);
	}
}

ga_fiber& ga_fiber::operator=(ga_fiber&& other)
{
	if (&other != this)
	{
		_impl = other._impl;
		other._impl = 0;
	}
	return *this;
}

ga_fiber ga_fiber::convert_thread(void* data)
{
	ga_fiber fiber;
	fiber._impl = ConvertThreadToFiber(data);
	return fiber;
}

void ga_fiber::switch_to(const ga_fiber& fiber)
{
	SwitchToFiber(fiber._impl);
}

void* ga_fiber::get_data()
{
	return GetFiberData();
}
