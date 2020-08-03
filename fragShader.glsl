#version 430
in vec3 f_color;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D u_texture0;

void main(void)
{
    fragColor = texture(u_texture0, texCoord) * vec4(f_color, 1.0);
}