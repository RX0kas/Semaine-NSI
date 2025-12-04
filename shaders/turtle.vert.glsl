#version 330 core

layout(location = 0) in vec2 aPos;

uniform mat3 viewmatrix;
uniform float uAspect; // FBOw / FBOh

void main()
{
    vec3 p = viewmatrix * vec3(aPos, 1.0);

    // Correction du ratio : on étire X pour compenser l’écran
    p.x /= uAspect;

    gl_Position = vec4(p.xy, 0.0, 1.0);
}
