#version 400

in vec3 position;
in vec3 normal;
in vec2 textureCoords;

out vec3 passPosition;
out vec3 passNormal;
out vec2 passTextureCoords;

out vec3 toCameraVector;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform int flipY;

void main()
{
    vec4 worldPosition = model * vec4(position, 1.0);
    if (flipY == 1)
        worldPosition.y = -worldPosition.y;
    gl_Position = projection * view * worldPosition;

    passPosition = worldPosition.xyz;
    passNormal = (model * vec4(normal, 0.0)).xyz;
    passTextureCoords = textureCoords;

    toCameraVector = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPosition.xyz;

}