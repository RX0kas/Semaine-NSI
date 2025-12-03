#version 330 core

layout(location = 0) in vec2 aPos;
uniform mat3 viewmatrix;

void main()
{
    // Convertit aPos (monde) en homogène (x,y,1)
    vec3 world = vec3(aPos, 1.0);

    // Transformation monde → NDC [-1,1]
    vec3 clip = viewmatrix * world;

    gl_Position = vec4(clip.xy, 0.0, 1.0);
}