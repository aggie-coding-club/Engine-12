#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec2 pos;

void main()
{
	pos = aTexCoord;

	vec4 position = vec4(aPosition, 1.0);

	// If you want a screen-space quad, this assumes -1 to 1 range
	position.xy *= 2.0;

	gl_Position = position;
}
