#version 430

in vec3 v_vertex;
in vec3 v_color;
in vec3 v_normal;

out vec3 f_color;

uniform float time;
uniform mat4 transform;
uniform vec3 directionalLight;

const float M_PI = 3.1415926535897932384626433832795;

void main(void)
{
	gl_Position = transform * vec4(v_vertex, 1.0f);
	
	// Color
	vec3 cosPos = cos(v_vertex);
	vec3 vColorScaled = v_color * 10.0f;
	vec3 color = 0.5f + 0.5f * cos(time + cosPos + vColorScaled);

	// Lighting. Uncomment for weird lighting :)
	//float dotProduct = max(0, dot(v_normal, directionalLight));
	//float theta = acos(dotProduct);
	//float factor = theta / (M_PI * 0.5f);
	//color *= 1 - factor;

	f_color = color;
}