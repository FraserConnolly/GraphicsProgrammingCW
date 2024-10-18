#version 400

in vec3 v_norm;
in vec4 v_pos; 
in vec3 vertexColour;
in vec2 textureCoord;

uniform vec3 lightDir;

out vec4 FragColour;

void main()
{
	float intensity;
	vec4 color;

	intensity = dot(lightDir, v_norm);

	if (intensity > 0.9)
		color = vec4(1.0,0.5,0.5,1.0);
	else if (intensity > 0.5)
		color = vec4(0.6,0.5,0.5,1.0);
	else if (intensity > 0.1)
		color = vec4(0.3,0.0,0.0,1.0);
	else
		color = vec4(0.2,0.0,0.0,1.0);
	
	FragColour = color;
}