#shader vertex
#version 330 core

layout (location = 0) in vec4 vertex;
uniform mat4 projection;
uniform mat4 model;

out vec2 coords;

void main() {
    coords = vertex.xy;
    gl_Position = projection * model  * vec4(vertex.xy, 0.0, 1.0 );
}

#shader fragment
#version 330 core

uniform vec3 textColor;
uniform float time;
uniform vec2 size;
uniform sampler2D fontTexture;
//uniform vec2 position;

in vec2 coords;
out vec4 color;

float circle(vec2 _st, float _radius){
    vec2 dist = _st-vec2(0.5);
    return 1.-smoothstep(_radius-(_radius*0.01),
                         _radius+(_radius*0.01),
                         dot(dist,dist)*4.0);
}

void main() {
    float pct = 0.0;
    //vec2 st = gl_FragCoord.xy / coords;
    vec2 c = ((coords - 1)  + vec2(0.5)) * vec2(3.0);
    c = fract(c);
    pct = distance(c, vec2(0.5));

    float cir = circle(c, 0.5);

    // color = vec4(sin(time) * pct, -pct * sin(time * 5), sin(time * 1.5) * pct, pct) * vec4(cir);
    //color = vec4(1.0, 1.0, 1.0, 1.0);
    //color = texture(fontTexture, size) * vec4(0.8, 0.2, 0.0, 0.5);

    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(fontTexture, coords).r);
    color = vec4(textColor, 1.0) * sampled;
}
