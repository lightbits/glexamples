#version 140

in vec2 vertTexel;
in vec4 worldNormal;
in vec4 worldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos; // in world-coordinates
uniform vec4 lightColor;
uniform vec4 ambient;

uniform sampler2D texBaseImage;

out vec4 outColor;

void main()
{
	vec4 dirToLight = normalize(vec4(lightPos, 1.0) - worldPos);
	float intensity = max(0.0, dot(worldNormal, dirToLight));
	outColor = intensity * lightColor + (1.0 - intensity) * ambient;

	// Multiply the color encoded normal
	outColor *= (worldNormal * 0.5 + vec4(0.5));
	outColor *= texture(texBaseImage, vertTexel);
}