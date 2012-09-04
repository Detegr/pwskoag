#version 150 core

out vec4 out_color;
in vec2 out_pos;

int between(float x, float a, float b)
{
	if(x > a && x < b) return 0;
	else return -1;
}

void main(void)
{
	float z=0.9;
	if(between(out_pos.x, -0.8, 0.8)==0 &&
	   between(out_pos.y, -0.8, 0.8)==0) z=0.3;
	out_color = vec4(z, 0.0, 0.0, 1.0);
}
