#version 330 core
in vec2 a_position;

uniform float zoom;
uniform vec2 center;
uniform mat3 viewmatrix;

vec2 applyZoom(vec2 point, vec2 center, float zoom) {
    return (point - center) * zoom + center;
}

void main()
{
    vec2 newPoint = /*viewmatrix*vec3(a_position,1.0);*/applyZoom(a_position, center, zoom);

    gl_Position = vec4(newPoint.xy, 0.0, 1.0);
}