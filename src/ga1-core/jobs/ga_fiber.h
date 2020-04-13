#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "framework/ga_compiler_defines.h"

#if defined(GA_MINGW)
#include <sys/types.h>
#endif

/*
** A fiber object.
** This the execution context for a thread including the registers and stack.
*/
class ga_fiber
{
public:
	typedef void(*function_t)(void* data);

	ga_fiber() : _impl(0) {}
	ga_fiber(ga_fiber&& other) : _impl(other._impl) { other._impl = 0; }
	ga_fiber(function_t func, void* func_data, size_t stack_size);
	~ga_fiber();

	ga_fiber& operator=(ga_fiber&& other);

	static ga_fiber convert_thread(void* data);
	static void switch_to(const ga_fiber& fiber);
	static void* get_data();

private:
	void* _impl;
};
