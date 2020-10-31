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
	ga_light_component(class ga_entity* ent, class ga_positional_light* light);
	virtual void update(struct ga_frame_params* params) override;

private:
	enum class LightType { directional, positional };
	LightType _type;
	class ga_light* _light;
};

