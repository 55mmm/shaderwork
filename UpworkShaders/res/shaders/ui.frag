#version 400

in vec2 passTextureCoords;

out vec4 fragmentColor;

uniform sampler2D normalMap;
uniform sampler2D depthMap;

void main()
{
	fragmentColor = texture(normalMap, passTextureCoords);
}