#version 330

// this shader is used only with non textured shapes

// input packet
in packet {

	vec4 colour;
	vec2 textureCoord;

} inputFragment;

// uniform sampler2D texture;

// output packet
layout (location = 0) out vec4 fragmentColour;

void main(void) {
    fragmentColour = inputFragment.colour;
	
	// comment out below to restore our rainbow star
	// this overrides our raindbow star
	
	// fragmentColour = vec4(1,1,0,0); 
}