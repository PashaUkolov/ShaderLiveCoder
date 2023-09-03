#shader vertex
#version 330 core

layout (location = 0) in vec4 vertex;
uniform mat4 projection;
uniform mat4 model;

out vec2 texCoords;

void main() {
    //gl_Position = projection * model* vec4(vertex.xy, 0.0, 1.0 );
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0 );
    texCoords = vertex.zw;
}

#shader fragment
#version 330 core

uniform vec3 textColor;
uniform float time;
uniform vec2 size;
uniform sampler2D fontTexture;
//uniform vec2 position;

in vec2 texCoords;
out vec4 color;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(fontTexture, texCoords).r);
    color = vec4(textColor, 1.0) * sampled;

    //color = vec4(1.0, 1.0, 1.0, 1.0);
}
