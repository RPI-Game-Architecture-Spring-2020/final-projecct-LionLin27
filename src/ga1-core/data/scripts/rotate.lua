
function update (component, frame_params)
	rot_rate = 0.5;
	entity = component_get_entity(component);
	entity_rotate(entity, frame_params, rot_rate);
end
