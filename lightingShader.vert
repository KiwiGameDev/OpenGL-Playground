#version 430
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_color;
layout (location = 2) in vec3 v_normal;
layout (location = 3) in vec2 v_texCoord;

out vec3 worldPos;
out vec3 normal;

uniform mat4 u_model;
uniform mat4 u_localToClip;

void main()
{
    gl_Position = u_localToClip * vec4(v_position, 1.0);
    worldPos = vec3(u_model * vec4(v_position, 1.0));
    normal = v_normal;
} 