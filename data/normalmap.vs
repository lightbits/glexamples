#version 140

in vec3 position;
in vec3 normal;
in vec2 texel;
in vec3 tangent;
in vec3 bitangent;

uniform vec3 lightPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 vertTexel;
out vec4 tangentLightDir; // interpolate across vertices
out vec4 worldNormal;

void main()
{
	mat4 modelView = view * model;
	gl_Position = projection * modelView * vec4(position, 1.0);
	vertTexel = texel;

	vec4 vertViewNormal = normalize(modelView * vec4(normal, 0.0));
	vec4 vertViewTangent = normalize(modelView * vec4(tangent, 0.0));
	vec4 vertViewBitangent = normalize(modelView * vec4(bitangent, 0.0));
	vec4 viewDirToLight = view * vec4(normalize(lightPos - position), 0.0);

	mat4 TBN = transpose(mat4(
		vertViewTangent,
		vertViewBitangent,
		vertViewNormal,
		vec4(0, 0, 0, 1)));
	tangentLightDir = TBN * viewDirToLight;

	worldNormal = model * vec4(normal, 0.0);
}