#version 400

uniform mat4 u_mvp;
uniform mat4 u_trans;
uniform mat4[4] u_skin;
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_color;
layout(location = 4) in uvec4 in_joints;
layout(location = 5) in vec4 in_weights;


// TODO: Homework 6.
// First, create a mat4 array to hold the skinning matrices of the skeleton.
// Refer to ga_animated_material::bind for clues on how to define this array.

// Next, create in variables for vertex attributes of the bar model.
// Refer to bar.egg to determine what attributes it has, and refer to the
// ga_model_component constructor for the locations to bind the input
// variables to.

out vec3 o_normal;
out vec3 o_color;

void main(void)
{
	// TODO: Homework 6.
	// Implement skinning math.
	// Each vertex must be multiplied by the skinning matrices of the joints
	// that influence it, and each result weighted by the influence of each joint.
	// The sum of the transformed, weighted positions is the skinned position of
	// the vertex.  It can then be multiplied by the mvp matrix.
	
	o_color = in_color;

	vec4 vertPos =  vec4(in_vertex,1);
	vec4 weightedSum = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 weightedSumNormal = vec4(0.0, 0.0, 0.0, 0.0);
	for(int i=0;i<4;i++){
		vec4 skinnedVert =  vertPos * u_skin[in_joints[i]];
		weightedSum += in_weights[i] * skinnedVert;

		vec4 skinnedNormal = vec4(in_normal, 1) * u_skin[in_joints[i]];
		weightedSumNormal += in_weights[i] * skinnedNormal;
	}
	gl_Position = weightedSum * u_mvp;
	o_normal = vec3(weightedSumNormal/length(weightedSumNormal) * u_trans);
	//o_normal = vec3(weightedSumNormal/length(weightedSumNormal)) * vec3(u_trans);
	//o_normal = in_normal;
}