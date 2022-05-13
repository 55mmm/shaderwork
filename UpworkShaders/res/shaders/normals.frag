#version 400

in vec3 passPosition;
in vec3 passNormal;

out vec4 outColor;

void main()
{
    outColor = vec4(normalize(passPosition), 1.0);
}
