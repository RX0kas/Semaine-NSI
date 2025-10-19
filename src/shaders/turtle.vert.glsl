#version 330 core
in vec2 a_position;

uniform float zoom;
uniform vec2 center;
uniform mat3 viewmatrix;

void main()
{
    vec3 newPoint = viewmatrix*vec3(a_position,1.0);
    gl_Position = vec4(newPoint.xy, 0.0, 1.0);
}