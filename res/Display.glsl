#shader vertex
#version 330 core

layout (location = 0) in vec2 vertexPositions;
layout (location = 1) in vec2 v_TexCoords;

uniform float DisplayAspectRatio;
uniform float RenderAspectRatio;
uniform vec2 Shift;
uniform float Zoom;

out vec2 f_TexCoords;

void main()
{
	vec2 position = vertexPositions * vec2(1.0, DisplayAspectRatio / RenderAspectRatio);
	position += Shift;
	position *= Zoom;
	gl_Position = vec4(position, 0.0, 1.0);
	f_TexCoords = v_TexCoords;
}

#shader fragment
#version 330 core

uniform sampler2D Image;
in vec2 f_TexCoords;

out vec4 FragmentColor;

void main()
{
	vec4 colorOut = texture(Image, f_TexCoords);
	FragmentColor = colorOut;
}
