#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelInverseTranspose;
uniform mat4 lightSpaceMatrix;
uniform vec3 lightPosition;

uniform sampler2D shadowMap;

struct Light
{
    vec3 position;
    vec3 color;
};

#define NUM_LIGHTS 10

uniform Light lights[NUM_LIGHTS];

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 texCoords;
in vec4 fragPosLightSpace;

out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightPosition - fragPosition)), 0.005);
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    if(projCoords.z > 1.0)
    {
        shadow = 0.0;
    }

    return shadow;
}


void main()
{
    vec3 color = ka;

    vec3 normal = normalize(fragNormal);
    float shadow = ShadowCalculation(fragPosLightSpace, normal);

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 lightDir = normalize(lights[i].position - fragPosition);
        vec3 reflectDir = reflect(-lightDir, normal);

        // Transform the fragment position to view space
        vec4 viewPosition = vec4(viewMatrix * vec4(fragPosition, 1.0));

        // Calculate the view vector
        vec3 viewDir = normalize(-viewPosition.xyz); // From camera to fragment

        vec3 diffuse  = kd * max(0.0, dot(normal, lightDir));
        vec3 spectral = ks * pow(max(0.0, dot(reflectDir, viewDir)), s);

        color += lights[i].color * (1.0 - shadow) * (diffuse + spectral);
    }

    FragColor = vec4(color, 1.0);
}

