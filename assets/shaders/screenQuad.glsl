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

in vec2 texCoords;

uniform vec3 quadColor;
uniform sampler2D fontTexture;

uniform vec2 iResolution;

out vec4 color;

void main() {
    vec2 uv = texCoords;
	uv -= 0.5;
	uv /= vec2(iResolution.x / iResolution.y, 1);
    color = vec4(uv.x, uv.y, 0.0, 1.0);
}