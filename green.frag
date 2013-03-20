#version 150 core

out vec4 out_color;
uniform sampler2D texsampler;
in vec2 uv;

void main(void)
{
	out_color = vec4(uv, 1.0, 1.0); //
	//out_color=vec4(texture2D(texsampler, uv).rgb, 1.0);
}
