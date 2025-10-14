#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

const vec2 pos = vec2(0.0);
const float zoom = 1.0;

vec2 applyZoom(vec2 point, vec2 center, float zoom) {
    return (point - center) * zoom + center;
}

void main() {
    vec2 resolution = vec2(800.0, 600.0);
    
    vec2 c = (2.0 * applyZoom(TexCoord,pos,zoom) - resolution) / resolution.y;
    vec2 z = vec2(0.0);
    
    float iterations = 0.0;
    float maxIteration = 500.0;
    for (; iterations < maxIteration && dot(z,z) < 4.0; iterations++) {
        // z = z^2 + c
        z = vec2(
            z.x * z.x - z.y * z.y,
            2.0 * z.x * z.y) + c;
    }
    
    float colorValue = iterations / maxIteration;
    FragColor = vec4(vec3(colorValue), 1.0);
}