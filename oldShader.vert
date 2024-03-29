#version 430
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_color;
layout (location = 2) in vec3 v_normal;
layout (location = 3) in vec2 v_texCoord;

out vec3 f_color;
out vec2 texCoord;

uniform float u_time;
uniform mat4 u_transform;
uniform vec3 u_directionalLight;

const float M_PI = 3.1415926535897932384626433832795;

void main(void)
{
	// Transform
	gl_Position = u_transform * vec4(v_position, 1.0f);
	
	// Color
	vec3 cosPos = cos(v_position);
	vec3 vColorScaled = v_color * 10.0f;
	vec3 color = 0.5f + 0.5f * cos(u_time + cosPos + vColorScaled);

	// Lighting. Uncomment for weird lighting :)
	//float dotProduct = max(0, dot(v_normal, u_directionalLight));
	//float theta = acos(dotProduct);
	//float factor = theta / (M_PI * 0.5f);
	//color *= 1 - factor;

	f_color = color;

	texCoord = v_texCoord;
}