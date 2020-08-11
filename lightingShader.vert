#version 430
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texCoords;
layout (location = 3) in vec3 v_tangent;

out vec3 WorldPos;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 u_model;
uniform mat4 u_localToClip;

void main()
{
    gl_Position = u_localToClip * vec4(v_position, 1.0);

    WorldPos = vec3(u_model * vec4(v_position, 1.0));
    TexCoords = v_texCoords;

    vec3 T = normalize(vec3(u_model * vec4(v_tangent, 0.0)));
    vec3 N = normalize(vec3(u_model * vec4(v_normal,  0.0)));
    vec3 B = -cross(N, T);

    TBN = mat3(T, B, N);
} 