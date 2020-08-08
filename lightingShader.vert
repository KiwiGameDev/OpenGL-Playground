#version 430
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texCoords;

out vec3 worldPos;
out vec3 normal;
out vec2 texCoords;

uniform mat4 u_model;
uniform mat4 u_localToClip;

void main()
{
    gl_Position = u_localToClip * vec4(v_position, 1.0);
    worldPos = vec3(u_model * vec4(v_position, 1.0));
    normal = mat3(transpose(inverse(u_model))) * v_normal;
    texCoords = v_texCoords;
} 