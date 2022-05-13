#version 400

in vec3 passPosition;
in vec3 passNormal;
in vec2 passTextureCoords;

in vec3 toCameraVector;

in vec4 fragmentPositionLightSpace;

out vec4 outColor;

uniform sampler2D tex0; // regular texture
uniform sampler2D tex1; // depth map

uniform vec3 lightPosition;
uniform vec3 lightColor;

uniform float reflectivity;
uniform float shineDamper;

float shadowCalculation(float nDotL)
{
    // perform perspective divide
    vec3 projCoords = fragmentPositionLightSpace.xyz / fragmentPositionLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(tex1, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.02 * (1.0 - nDotL), 0.0002);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(tex1, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(tex1, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    shadow = 1.0 - shadow;

    if(projCoords.z > 1.0)
        shadow = 1.0;

    return shadow;
}

void main()
{
    vec3 surfaceNormal = normalize(passNormal);
    vec3 toLightVector = normalize(lightPosition - passPosition);
    float nDotL = dot(toLightVector, surfaceNormal);

    // Do shadow calculation
    float shadow = 0.85 + 0.15 * shadowCalculation(nDotL);

    // Diffuse lighting
    
    float brightness = 0.45 + 0.55 * clamp(nDotL, 0.0, 1.0);
    vec3 diffuse = shadow * brightness * lightColor;

    // Specular lighting
    vec3 unitToCameraVector = normalize(toCameraVector);
    vec3 lightDirection = -toLightVector;
    vec3 reflectedLightDirection = reflect(lightDirection, surfaceNormal);

    float specularFactor = dot(reflectedLightDirection, unitToCameraVector);
    specularFactor = clamp(specularFactor, 0.0, 1.0);
    float dampedFactor = pow(specularFactor, shineDamper);
    vec3 finalSpecular = dampedFactor * reflectivity * lightColor;

    outColor = vec4(diffuse, 1.0) * texture(tex0, passTextureCoords) + vec4(shadow * finalSpecular, 0.0);
}
