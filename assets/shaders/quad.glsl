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

uniform vec3 quadColor;
uniform sampler2D fontTexture;

out vec4 color;

void main() {
    color = vec4(quadColor.r, quadColor.g, quadColor.b, 1.0);
}