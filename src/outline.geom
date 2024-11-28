#version 150

layout (points) in;
layout(line_strip, max_vertices = 24) out; // Output: 12 edges (24 vertices)

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float voxelResolution;
uniform float voxelSize;
out vec3 fragCoords; 

void main() {
    // Define the 8 corners of the cube relative to the center
    fragCoords = gl_in[0].gl_Position.xyz / voxelSize;

    mat4 mvp = projectionMatrix * viewMatrix * worldMatrix;
    vec3 offsets[8] = vec3[8](
        vec3(-1, -1, -1), // 0: Bottom-left-back
        vec3( 1, -1, -1), // 1: Bottom-right-back
        vec3(-1,  1, -1), // 2: Top-left-back
        vec3( 1,  1, -1), // 3: Top-right-back
        vec3(-1, -1,  1), // 4: Bottom-left-front
        vec3( 1, -1,  1), // 5: Bottom-right-front
        vec3(-1,  1,  1), // 6: Top-left-front
        vec3( 1,  1,  1)  // 7: Top-right-front
    );

    // Scale the offsets by half the cube size
    for (int i = 0; i < 8; ++i) {
        offsets[i] *= voxelSize / voxelResolution * 0.5;
    }

    // Define the cube's 12 edges as pairs of indices
    int edgeIndices[24] = int[24](
        0, 1,  1, 3,  3, 2,  2, 0,  // Back face edges
        4, 5,  5, 7,  7, 6,  6, 4,  // Front face edges
        0, 4,  1, 5,  2, 6,  3, 7   // Connecting edges
    );

    // Input point position
    vec3 center = gl_in[0].gl_Position.xyz;

    // Emit line segments for all edges
    for (int i = 0; i < 24; i += 2) {
        vec3 start = center + offsets[edgeIndices[i]];
        vec3 end = center + offsets[edgeIndices[i + 1]];

        gl_Position = mvp * vec4(start, 1.0);
        EmitVertex();

        gl_Position = mvp * vec4(end, 1.0);
        EmitVertex();

        EndPrimitive(); // End the current line
    }
}
