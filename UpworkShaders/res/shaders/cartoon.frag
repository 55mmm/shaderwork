#version 400

in vec2 passTextureCoords;

out vec4 fragmentColor;

uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D celTexture;

void main()
{
	fragmentColor = mix(texture(normalMap, passTextureCoords), texture(celTexture, passTextureCoords), 0.5);
}