#version 460 core

out vec4 FragColor;

in vec2 vTexCoords;

uniform sampler2D screenTexture;

void main()
{
	vec3 col = texture(screenTexture, vTexCoords).rgb;
	FragColor = vec4(col, 1.0);
}