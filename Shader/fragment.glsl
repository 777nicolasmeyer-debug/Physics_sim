#version 330 core

in vec2 texCoord;
in vec3 Normal;
out vec4 frag;
uniform sampler2D tex;

void main() {
frag = texture(tex, texCoord);
}
