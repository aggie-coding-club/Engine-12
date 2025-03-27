#version 120

attribute vec3 vPositionModel; // in object space
attribute vec3 vNormalModel; // in object space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 modelInverseTranspose;


varying vec3 fragPosition;
varying vec3 fragNormal;

void main()
{

    fragPosition = vec3(model * vec4(vPositionModel, 1.0));
    fragNormal = normalize(vec3(modelInverseTranspose * vec4(vNormalModel, 0.0)));


    // gl_Position already contains the world space for vertices
    gl_Position = projection * view * model * vec4(vPositionModel, 1.0);

}