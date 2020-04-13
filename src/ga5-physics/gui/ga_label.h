#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_widget.h"

/*
** GUI text display widget.
*/
class ga_label : public ga_widget
{
public:
	ga_label(const char* text, float x, float y, struct ga_frame_params* params);
	~ga_label();
};
