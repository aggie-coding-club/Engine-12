#version 330 core

layout (location = 0) in vec3 vPositionModel; // in object space
layout (location = 1) in vec3 vNormalModel;   // in object space
layout (location = 2) in vec2 vTexCoords;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelInverseTranspose;
uniform mat4 lightSpaceMatrix;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 texCoords;
out vec4 fragPosLightSpace;

void main()
{
    // Transform position to world space
    fragPosition = vec3(model * vec4(vPositionModel, 1.0));

    // Transform normal to world space, preserving direction
    fragNormal = normalize(vec3(modelInverseTranspose * vec4(vNormalModel, 0.0)));

    texCoords = vTexCoords;

    fragPosLightSpace = lightSpaceMatrix * vec4(fragPosition, 1.0);

    // Compute final vertex position in clip space
    gl_Position = projection * view * model * vec4(vPositionModel, 1.0);
}
