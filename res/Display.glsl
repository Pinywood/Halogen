#shader vertex
#version 330 core

layout (location = 0) in vec2 vertexPositions;
layout (location = 1) in vec2 v_TexCoords;

out vec2 f_TexCoords;

void main()
{
	gl_Position = vec4(vertexPositions, 0.0, 1.0);
	f_TexCoords = 1.0 - v_TexCoords;
}

#shader fragment
#version 330 core

uniform sampler2D Image;
uniform float exposure;
uniform float gamma;
in vec2 f_TexCoords;

out vec4 FragmentColor;

vec4 ToneMapper(in vec4 color)
{
	float alpha = 5.0;
	float beta = 2.0;
	color = pow(color, vec4(alpha));
	color = color / (color + vec4(vec3(beta), 0.0));
	return color;
}

void main()
{
	vec4 colorOut = texture(Image, f_TexCoords);
	colorOut *= vec4(vec3(exposure), 1.0);
	colorOut = ToneMapper(colorOut);
	colorOut = pow(colorOut, vec4(1.0/gamma));
	FragmentColor = colorOut;
}
