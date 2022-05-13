#version 400

in vec2 passTextureCoords;

out vec4 fragmentColor;

uniform sampler2D tex0;

void main()
{
	fragmentColor = texture(tex0, passTextureCoords);
}