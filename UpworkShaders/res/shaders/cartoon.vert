#version 400

in vec2 position;

out vec2 passTextureCoords;

uniform mat4 view;


void main()
{
    gl_Position = view * vec4(position, 0.0, 1.0);

    passTextureCoords = 0.5 * (position + vec2(1.0, 1.0));
}