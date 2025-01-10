#version 420

out vec4 outColor;
in vec3 viewNormal; // Phong
in vec3 worldNormal; // Phong (specular)
in vec3 worldSurface; // Phong (specular)
in vec3 viewSurface; // Phong (specular)
in vec4 worldPosition;

uniform vec3 ka;
uniform vec3 ke;
uniform vec3 kd;
uniform vec3 ks;

uniform vec3 lightSource;
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;

uniform float voxelResolution;
uniform float voxelMin;

layout(rgba8) uniform image3D voxelMemory;

void main(void)
{

    // Voxel coloring
	vec3 lightIncident = normalize(mat3(viewMatrix) * (lightSource - worldSurface));

	float diffuse, specular;
	// Diffuse
	diffuse = dot(normalize(viewNormal), lightIncident);
	diffuse = max(0.0, diffuse); // No negative light
	
	// Specular
	vec3 r = reflect(-lightIncident, normalize(viewNormal));
	vec3 v = normalize(-viewSurface); // View direction
	specular = dot(r, v);
	if (specular > 0.0)
		specular = 1.0 * pow(specular, 150.0);
	specular = max(specular, 0.0);
	specular = min(specular, 1.0);
	vec3 shade = kd*diffuse + ks*specular + ka + ke;

    // Voxelising
    // scale [-1, 1] to [0, 1]
    vec4 textureCoords = 0.5 * worldPosition + 0.5;
    
    // Convert to voxel coordinates
    ivec4 voxelcoord = ivec4((textureCoords) * voxelResolution);

    // Race condition if too many threads are ran, 
    // but should not be a problem

    imageStore(voxelMemory, ivec3(voxelcoord), vec4(shade, 1.0));
    vec4 isvoxel = imageLoad(voxelMemory, ivec3(voxelcoord));
	// outColor = normalize(gl_FragCoord - vec4(0,0, gl_FragCoord.z, 0));
	// outColor = vec4((gl_FragCoord.x - 0.5) / 1080.0, (gl_FragCoord.y - 0.5) / 1080.0, 0.0, gl_FragCoord.w);
    if (isvoxel.x > 0.5) {
        outColor = vec4(voxelcoord / voxelResolution);
    }
	// outColor = vec4(1.0);
}
