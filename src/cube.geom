#version 150
layout (points) in;
layout(triangle_strip, max_vertices = 36) out; // Output: cube (36 vertices for 6 faces)
//
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float voxelResolution;
uniform float voxelSize;
out vec3 fragCoords; 


void main() {
    // Define the offsets for the cube vertices relative to the center
    float cubeSize = voxelSize / voxelResolution;
    fragCoords = gl_in[0].gl_Position.xyz / voxelSize;
    mat4 mvp = projectionMatrix * viewMatrix * worldMatrix;
    vec3 offsets[8] = vec3[8](
        vec3(-1, -1, -1), // Bottom-left-back
        vec3( 1, -1, -1), // Bottom-right-back
        vec3(-1,  1, -1), // Top-left-back
        vec3( 1,  1, -1), // Top-right-back
        vec3(-1, -1,  1), // Bottom-left-front
        vec3( 1, -1,  1), // Bottom-right-front
        vec3(-1,  1,  1), // Top-left-front
        vec3( 1,  1,  1)  // Top-right-front
    );

    // Scale the offsets by half the cube size
    for (int i = 0; i < 8; ++i) {
        offsets[i] *= cubeSize * 0.5;
    }

    // Define the 12 triangles of the cube (6 faces * 2 triangles per face)
    int faces[36] = int[36](
        0, 1, 2,  1, 3, 2,  // Back face
        4, 5, 6,  5, 7, 6,  // Front face
        0, 2, 4,  2, 6, 4,  // Left face
        1, 5, 3,  5, 7, 3,  // Right face
        2, 3, 6,  3, 7, 6,  // Top face
        0, 1, 4,  1, 5, 4   // Bottom face
    );

    // Input point position
    vec3 center = gl_in[0].gl_Position.xyz;

    // Emit all triangles
    for (int i = 0; i < 36; ++i) {
        vec3 vertexPosition = center + offsets[faces[i]];
        gl_Position = mvp * vec4(vertexPosition, 1.0);
        EmitVertex();
    }
    EndPrimitive();
}
