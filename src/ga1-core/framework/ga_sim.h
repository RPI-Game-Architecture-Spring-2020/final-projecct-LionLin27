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

	class ga_entity* selected_ent();
	void select_last_ent();

	void set_env_map(class ga_cube_texture* env) { _env_map = env; }
	class ga_cube_texture* get_env_map() { return _env_map; };

private:
	std::vector<class ga_entity*> _entities;
	int _selected_ent_index;

	// env map ( there could be better place to put this..?
	class ga_cube_texture* _env_map;
};
