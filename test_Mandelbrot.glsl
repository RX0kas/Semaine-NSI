const vec2 pos = vec2(0.0);
const float zoom = 1.0;

vec2 applyZoom(vec2 point, vec2 center, float zoom) {
    return (point - center) * zoom + center;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 resolution = iResolution.xy;
    float x = float(resolution.x);
    float y = float(resolution.y);
    float x2 = pow(x,2.0);
    // Cardioïde
    float p = sqrt(pow(float(resolution.x)-(1.0/4.0),2.0)+pow(resolution.y,2.0));
    if (float(resolution.x) < 2.0*pow(p,2.0)+(1.0/4.0)) {
        fragColor = vec4(1.0);
        return;
    }
    // Bourgeon principal
    if (pow(float(resolution.x)+1.0,2.0)+pow(float(resolution.y),2.0) < 1.0/16.0) {
        fragColor = vec4(1.0);
        return;
    }
    
    
    
    vec2 c = (2.0 * applyZoom(fragCoord,pos,zoom) - resolution) / resolution.y;
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
    fragColor = vec4(vec3(colorValue), 1.0);
}