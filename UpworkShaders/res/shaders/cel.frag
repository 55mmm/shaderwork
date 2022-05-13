#version 400

in vec3 passPosition;
in vec3 passNormal;
in vec2 passTextureCoords;

in vec3 toCameraVector;

out vec4 outColor;

uniform sampler2D tex0; // regular texture

uniform vec3 lightPosition;
uniform vec3 lightColor;

uniform float reflectivity;
uniform float shineDamper;

float cel(float f)
{
    const float steps = 156;
    return floor(f * steps) / steps;
}

void main()
{
    vec3 surfaceNormal = normalize(passNormal);
    vec3 toLightVector = normalize(lightPosition - passPosition);
    float nDotL = dot(toLightVector, surfaceNormal);

    // Diffuse lighting
    
    // Uncomment the following line if you want brighter lighting
    float brightness = cel(0.25 + 0.75 * clamp(nDotL, 0.0, 1.0));
    // Unomment the following line out if you want darker lighting
    // float brightness = cel(clamp(nDotL, 0.0, 1.0));

    vec3 diffuse = brightness * lightColor;

    // Specular lighting
    vec3 unitToCameraVector = normalize(toCameraVector);
    vec3 lightDirection = -toLightVector;
    vec3 reflectedLightDirection = reflect(lightDirection, surfaceNormal);

    float specularFactor = dot(reflectedLightDirection, unitToCameraVector);
    specularFactor = clamp(specularFactor, 0.0, 1.0);
    float dampedFactor = pow(specularFactor, shineDamper);
    vec3 finalSpecular = cel(dampedFactor * reflectivity) * lightColor;

    outColor = vec4(diffuse, 1.0) * texture(tex0, passTextureCoords) + vec4(finalSpecular, 0.0);
}
