#version 430
layout (location = 0) in vec3 v_position;

out vec3 TexCoords;

uniform mat4 u_MVP;

void main()
{
    TexCoords = v_position;
    gl_Position = u_MVP * vec4(v_position, 1.0);
}  