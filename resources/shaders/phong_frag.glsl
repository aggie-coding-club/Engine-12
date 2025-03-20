#version 120

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 modelInverseTranspose;

struct lightStruct
{
    vec3 position;
    vec3 color;
};

#define NUM_LIGHTS 10

uniform lightStruct lights[NUM_LIGHTS];

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

varying vec3 fragPosition;
varying vec3 fragNormal;

void main()
{
    vec3 color = ka;

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 normal = normalize(fragNormal);
        vec3 lightDir = normalize(lights[i].position - fragPosition);
        vec3 reflectDir = reflect(-lightDir, normal);

        // Transform the fragment position to view space
        vec4 viewPosition = view * vec4(fragPosition, 1.0);

        // Calculate the view vector
        vec3 viewDir = normalize(-viewPosition.xyz); // From camera to fragment

        vec3 diffuse  = kd * max(0, dot(normal, lightDir));
        vec3 spectral = ks * pow(max(0, dot(reflectDir, viewDir)), s);
        // vec3 spectral = vec3(0.0f);

        color += lights[i].color * (diffuse + spectral);
    }

    gl_FragColor = vec4(color, 1.0f);
}