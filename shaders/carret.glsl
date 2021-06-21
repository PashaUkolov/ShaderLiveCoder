#shader vertex
#version 330 core

layout (location = 0) in vec4 vertex;
uniform mat4 projection;
uniform mat4 model;

out vec2 texCoords;

void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0 );
    texCoords = vertex.zw;
}

#shader fragment
#version 330 core

uniform vec3 textColor;
uniform float time;
uniform vec2 size;
uniform sampler2D fontTexture;

out vec4 color;

void main() {
    float anim = sin(time * 10.0f) * 0.5 + 0.5;
    color = vec4(textColor.r, textColor.g, textColor.b, sin(anim));
    color = vec4(textColor.r, textColor.g, textColor.b, anim);
}
