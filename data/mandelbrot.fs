#version 140

in vec2 uv;

uniform float zoom;
uniform vec2 offset;

out vec4 outColor;

void main()
{
	float left = -2.5;
	float right = 1.0;
	float bottom = -1.0;
	float top = 1.0;

	vec2 c = vec2(left + (right - left) * uv.x, bottom + (top - bottom) * uv.y) * (1.0f - zoom);
	c += offset;

	vec2 z = vec2(0.0, 0.0);
	int i = 0;
	int maxIteration = 128;
	while(dot(z, z) < 4.0 && i < maxIteration)
	{
		z = vec2(
				z.x * z.x - z.y * z.y + c.x,
				2.0 * z.x * z.y + c.y);
		++i;
	}

	float alpha = i / float(maxIteration);
	// float alpha = sqrt(i / float(maxIteration));
	outColor = vec4(alpha * 20.0 / 255.0, alpha * 190.0 / 255.0, alpha * 255.0 / 255.0, 1.0);
}