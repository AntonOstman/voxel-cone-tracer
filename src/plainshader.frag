#version 150

// in vec2 outTexCoord;
// uniform sampler2D texUnit;
out vec4 out_Color;
uniform sampler3D voxelTexture;
in vec3 fragCoords; 

void main(void)
{
    // out_Color = texture(voxelTexture , fragCoords); 
    out_Color = texture(voxelTexture , fragCoords)* gl_FragCoord.z; 
    // out_Color = vec4(color,1.0);
}
