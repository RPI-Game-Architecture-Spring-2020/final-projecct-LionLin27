#version 430

layout (quads, fractional_even_spacing,ccw) in;

uniform mat4 u_mvp;
layout (binding = 0) uniform sampler2D u_texture;
layout (binding = 2) uniform sampler2D u_height;
layout (binding = 3) uniform sampler2D u_normal;

in vec2 tcs_out[];
out vec2 tes_out;

uniform mat4 u_mvMat;

out vec3 o_normal;
out vec3 o_tangent;
out vec3 o_vertPos;

//	Classic Perlin 2D Noise 
//	by Stefan Gustavson
//
vec2 fade(vec2 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
float cnoise(vec2 P){
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod(Pi, 289.0); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;
  vec4 i = permute(permute(ix) + iy);
  vec4 gx = 2.0 * fract(i * 0.0243902439) - 1.0; // 1/41 = 0.024...
  vec4 gy = abs(gx) - 0.5;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;
  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);
  vec4 norm = 1.79284291400159 - 0.85373472095314 * 
    vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;
  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));
  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 0.05 *  n_xy;
}

void main (void)
{	
	vec2 tc = vec2(tcs_out[0].x + (gl_TessCoord.x)/64.0,
					tcs_out[0].y + (1.0-gl_TessCoord.y)/64.0);

	// map the tessellated grid onto the texture rectangle:
	vec4 tessellatedPoint = vec4(gl_in[0].gl_Position.x + gl_TessCoord.x/64.0, 0.0,
								gl_in[0].gl_Position.z + gl_TessCoord.y/64.0, 1.0);
	
	// add the height from the height map to the vertex:
	//tessellatedPoint.y += (texture(u_height, tc).r) / 60.0;
	tessellatedPoint.y += cnoise(vec2(tessellatedPoint.x*100, tessellatedPoint.z*100));
	
	gl_Position = tessellatedPoint * u_mvp;
	tes_out = tc;

	o_normal = vec3(0,1,0);
	o_tangent = vec3(1,0,0);
	o_vertPos = (tessellatedPoint * u_mvMat).xyz;
}