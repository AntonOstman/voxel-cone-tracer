#version 420

in vec3 in_Position;
uniform mat4 mvp;

void main(void)
{
	gl_Position = mvp * vec4(in_Position, 1.0); // This should include projection
}
