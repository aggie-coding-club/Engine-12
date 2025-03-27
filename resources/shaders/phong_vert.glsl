#version 330 core

layout (location = 0) in vec3 vPositionModel; // in object space
layout (location = 1) in vec3 vNormalModel;   // in object space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelInverseTranspose;

out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
    // Transform position to world space
    fragPosition = vec3(model * vec4(vPositionModel, 1.0));

    // Transform normal to world space, preserving direction
    fragNormal = normalize(vec3(modelInverseTranspose * vec4(vNormalModel, 0.0)));

    // Compute final vertex position in clip space
    gl_Position = projection * view * model * vec4(vPositionModel, 1.0);
}