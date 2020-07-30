#version 430

in vec2 frag_UV;

out vec4 frag_color;

uniform sampler2D ourTexture;
void main() {
	frag_color = texture(ourTexture,frag_UV);
    //frag_color = vec4(1.0, 1.0, 0.0, 1.0);
}
