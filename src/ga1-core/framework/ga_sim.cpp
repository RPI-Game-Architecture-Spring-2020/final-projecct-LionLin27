/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_sim.h"

#include "ga_compiler_defines.h"

#include "entity/ga_entity.h"
#include "jobs/ga_job.h"


#include "ga_frame_params.h"

#if defined(GA_MINGW)
#include <malloc.h>
#endif

ga_sim::ga_sim()
{
	_selected_ent_index = 0;
}

ga_sim::~ga_sim()
{
}

void ga_sim::add_entity(ga_entity* ent)
{
	_entities.push_back(ent);
}

void ga_sim::update(ga_frame_params* params)
{
	// Create jobs that update all entities in parallel (one job per entity).
	// There are 2 components:
	// 1. The job declarations; a function and a pointer to data for that function.
	// 2. The data for each job; an entity and the frame_params.

	auto decls = static_cast<ga_job_decl_t*>(alloca(sizeof(ga_job_decl_t) * _entities.size()));

	struct update_data_t
	{
		ga_entity* _entity;
		ga_frame_params* _params;
	};
	auto update_data = static_cast<update_data_t*>(alloca(sizeof(update_data_t) * _entities.size()));

	for (int i = 0; i < _entities.size(); ++i)
	{
		update_data[i]._entity = _entities[i];
		update_data[i]._params = params;

		decls[i]._data = update_data + i;
		decls[i]._entry = [](void* data)
		{
			auto update_data = static_cast<update_data_t*>(data);
			update_data->_entity->update(update_data->_params);
		};
	}

	// Dispatch the jobs:
	int32_t update_counter;
	ga_job::run(decls, int(_entities.size()), &update_counter);
	ga_job::wait(&update_counter);

	// select entity
	if (params->_btn_pressed_mask & k_button_z) {
		_selected_ent_index++;
		if (_selected_ent_index >= _entities.size()) {
			_selected_ent_index = 0;
		}
	}
	params->_selected_ent = selected_ent();
}

void ga_sim::late_update(ga_frame_params* params)
{
	auto decls = static_cast<ga_job_decl_t*>(alloca(sizeof(ga_job_decl_t) * _entities.size()));

	struct update_data_t
	{
		ga_entity* _entity;
		ga_frame_params* _params;
	};
	auto update_data = static_cast<update_data_t*>(alloca(sizeof(update_data_t) * _entities.size()));

	for (int i = 0; i < _entities.size(); ++i)
	{
		update_data[i]._entity = _entities[i];
		update_data[i]._params = params;

		decls[i]._data = update_data + i;
		decls[i]._entry = [](void* data)
		{
			auto update_data = static_cast<update_data_t*>(data);
			update_data->_entity->late_update(update_data->_params);
		};
	}

	int32_t update_counter;
	ga_job::run(decls, int(_entities.size()), &update_counter);
	ga_job::wait(&update_counter);
}

ga_entity* ga_sim::selected_ent()
{
	return _entities[_selected_ent_index];
}
