#pragma once

#include "entity/ga_component.h"

#include <cstdint>

class ga_pyramid_component : public ga_component
{
#define _numVBOs 2
public:
	ga_pyramid_component(class ga_entity* ent);
	virtual ~ga_pyramid_component();

	virtual void update(struct ga_frame_params* params) override;

private:
	//static int _numVBOs = 1;
	class ga_custom_shader_material* _material;
	uint32_t _vao;
	uint32_t _vbos[_numVBOs];
	uint32_t _index_count;

};

