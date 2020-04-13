#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <vector>

/*
** Represents the simulation stage of the frame.
** Owns the entities.
*/
class ga_sim
{
public:
	ga_sim();
	~ga_sim();

	void add_entity(class ga_entity* ent);

	void update(struct ga_frame_params* params);
	void late_update(struct ga_frame_params* params);

private:
	std::vector<class ga_entity*> _entities;
};
