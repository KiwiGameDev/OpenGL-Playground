#version 430
in vec3 f_color;
in vec2 texCoord;

out vec4 fragColor;

uniform vec3 lightColor;

uniform sampler2D u_texture0;

void main(void)
{
    vec4 color = vec4(f_color, 1.0);
    vec4 texture = texture(u_texture0, texCoord);
    fragColor = color * texture;
}