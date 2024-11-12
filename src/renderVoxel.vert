#version 420

in vec3 in_Position;
out vec4 position;
// in  vec3 in_Normal;
// out vec3 viewNormal;
// out vec3 worldNormal;
// out vec3 viewSurface; 
// out vec3 worldSurface; 
//

layout(r8) uniform image3D voxelMemory;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
	mat4 modelviewMatrix = viewMatrix * worldMatrix;
	// viewNormal = inverse(transpose(mat3(modelviewMatrix))) * in_Normal; // Phong, "fake" normal transformation
	// worldNormal = in_Normal; // Phong, "fake" normal transformation
	//
	// viewSurface = vec3(modelviewMatrix * vec4(in_Position, 1.0)); // Don't include projection here - we only want to go to view coordinates
	// worldSurface = vec3(worldMatrix * vec4(in_Position, 1.0)); // Don't include projection here - we only want to go to view coordinates
    // imageLoad(voxelMemory, vec3(0,0,0));

	gl_Position = projectionMatrix * modelviewMatrix * vec4(in_Position, 1.0); // This should include projection
    position = gl_Position;
}
