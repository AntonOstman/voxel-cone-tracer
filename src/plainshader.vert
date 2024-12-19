#version 150

in  vec3 in_Position;
uniform mat4 mvp;

void main(void)
{

	gl_Position = vec4(in_Position, 1.0); // Geom shader has projection
}
