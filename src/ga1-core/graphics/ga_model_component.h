#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "entity/ga_component.h"

#include <cstdint>

/*
** Renderable model component.
*/
class ga_model_component : public ga_component
{
public:
	ga_model_component(class ga_entity* ent, struct ga_model* model, class ga_material* mat, bool lit = false);
	ga_model_component(class ga_entity* ent, struct ga_patch* model, class ga_material* mat, bool lit = false);
	ga_model_component(class ga_entity* ent, struct ga_terrain* model, class ga_material* mat, bool lit = true);
	ga_model_component(class ga_entity* ent, struct ga_model* model, const char* texturePath);
	ga_model_component(class ga_entity* ent, struct ga_model* model, class ga_directional_light* light);
	virtual ~ga_model_component();

	virtual void update(struct ga_frame_params* params) override;

	ga_patch* get_patch();
	ga_terrain* get_terrain();

private:
	void bind_model(struct ga_model* model);
	void bind_patch(struct ga_patch* patch);
	class ga_material* _material;
	class ga_directional_light* _light;
	uint32_t _vao;
	uint32_t _vbos[4];
	uint32_t _index_count;
	bool _drawBuffer = false;
	bool _drawPatch = false;
	bool _drawTerrain = false;
	bool _lit = false;
	ga_patch* _patch = nullptr;
	ga_terrain* _terrain = nullptr;
};