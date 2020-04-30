#pragma once

/*
** RPI Game Architecture Engine
**
*/

#include "entity/ga_component.h"

#include <cstdint>
#include "math/ga_vec3f.h"

class ga_light_component : public ga_component
{
public:
	ga_light_component(class ga_entity* ent, class ga_directional_light* light);
	virtual ~ga_light_component();
	virtual void update(struct ga_frame_params* params) override;

private:
	class ga_directional_light* _light;
};

