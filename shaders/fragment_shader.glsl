#version 330

uniform sampler2D buffer;
noperspective in vec2 TexCoord;

out vec3 outColor;

void main(void) {
    // Samples the CPU-generated buffer and outputs it to the screen
    outColor = texture(buffer, TexCoord).rgb;
}