#version 420

out vec4 outColor;
in vec3 viewNormal; // Phong
in vec3 worldNormal; // Phong (specular)
in vec3 worldSurface; // Phong (specular)
in vec3 viewSurface; // Phong (specular)
in vec4 position;

uniform float voxelResolution;
uniform float voxelMin;

layout(r8) uniform image3D voxelMemory;

void main(void)
{

    // TODO same for all, should be done on cpu 
	// vec3 light = viewSurface; // Light position in view
	// vec3 light = vec3(mat3(modelviewMatrix) * vec4(lightSource - viewSurface, 1.0));

    // Voxelising

    // scale [-1, 1] to [0, 1]
    vec4 textureCoords = 0.5 * position + 0.5;
    
    // Convert to voxel coordinates
    ivec4 voxelcoord = ivec4((textureCoords) * voxelResolution);

    // Race condition if too many threads are ran, 
    // but should not be a problem

    imageStore(voxelMemory, ivec3(voxelcoord), vec4(1.0));
    vec4 isvoxel = imageLoad(voxelMemory, ivec3(voxelcoord));
	// outColor = normalize(gl_FragCoord - vec4(0,0, gl_FragCoord.z, 0));
	// outColor = vec4((gl_FragCoord.x - 0.5) / 1080.0, (gl_FragCoord.y - 0.5) / 1080.0, 0.0, gl_FragCoord.w);
    if (isvoxel.x > 0.5){
        outColor = vec4(voxelcoord / voxelResolution);
    }
	// outColor = vec4(1.0);
}
