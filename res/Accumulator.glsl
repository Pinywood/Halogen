#shader vertex
#version 330 core

layout (location = 0) in vec2 vertexPositions;
layout (location = 1) in vec2 v_TexCoords;

out vec2 f_TexCoords;

void main()
{
	gl_Position = vec4(vertexPositions, 0.0, 1.0);
	f_TexCoords = v_TexCoords;
}

#shader fragment
#version 330 core

uniform sampler2D CurrentSampleImage;
uniform sampler2D Accumulated;
uniform float CurrentSample;
in vec2 f_TexCoords;

out vec4 FragmentColor;

void main()
{
	FragmentColor = texture(CurrentSampleImage, f_TexCoords) + CurrentSample * texture(Accumulated, f_TexCoords);
	FragmentColor = FragmentColor / (CurrentSample + 1.0);
}
