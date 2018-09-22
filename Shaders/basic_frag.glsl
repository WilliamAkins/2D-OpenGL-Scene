#version 330

// this shader works with textured shapes

// input packet
in packet {

	vec4 colour;
	vec2 textureCoord;

} inputFragment;

uniform sampler2D texture;


// output packet
layout (location = 0) out vec4 fragmentColour;


void main(void) {
   fragmentColour = texture2D(texture, inputFragment.textureCoord);

}