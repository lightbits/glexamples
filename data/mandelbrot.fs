#version 140

in vec2 uv;

uniform float zoom;
uniform vec2 offset;

out vec4 outColor;

void main()
{
	float x = 0.0;
	float y = 0.0;

	float tu = uv.x * 0.5 + 0.5;
	float tv = uv.y * 0.5 + 0.5;

	float left = -2.5;
	float right = 1.0;
	float bottom = -1.0;
	float top = 1.0;

	vec2 c = vec2(left + (right - left) * tu, bottom + (top - bottom) * tv) * (1.0f - zoom);
	c += offset;

	int i = 0;
	while(x * x + y * y < 4.0 && i < 128)
	{
		float temp = x * x - y * y + c.x;
		y = 2.0 * x * y + c.y;
		x = temp;
		++i;
	}

	float alpha = i / 128.0;
	outColor = vec4(alpha * 20.0 / 255.0, alpha * 190.0 / 255.0, alpha * 255.0 / 255.0, 1.0);
}