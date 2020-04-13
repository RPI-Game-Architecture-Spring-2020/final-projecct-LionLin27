#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

/*
** A thread-safe and lock-free pool of integers.
*/
class ga_intpool
{
public:
	ga_intpool(int index_count);
	~ga_intpool();

	int alloc();
	void free(int index);

	int get_index_count() const;

private:
	void* _impl;
};
