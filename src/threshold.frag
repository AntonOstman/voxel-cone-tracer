#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
out vec4 out_Color;

void main(void)
{
    vec4 color = texture(texUnit, outTexCoord);
    color.x = min(2, max(color.x - 1.0f, 0));
    color.y = min(2, max(color.y - 1.0f, 0));
    color.z = min(2, max(color.z - 1.0f, 0));
    out_Color = color;
}
