#version 140

in vec2 vertTexel;
in vec4 tangentLightDir;
in vec4 worldNormal;

uniform sampler2D texBaseImage;
uniform sampler2D texNormalMap;

uniform vec4 lightColor;
uniform vec4 ambient;

out vec4 outColor;

void main()
{
	vec4 baseColor = texture(texBaseImage, vertTexel);
	vec4 normalColor = texture(texNormalMap, vertTexel);

	// This is the tangent-space normal
	vec4 normal = normalColor * 2.0 - vec4(1.0);
	normal.w = 0.0;

	float intensity = max(0.0, dot(normal, normalize(tangentLightDir)));
	outColor = (worldNormal * 0.5 + 0.5) * baseColor * (intensity * lightColor + (1.0 - intensity) * ambient);
}