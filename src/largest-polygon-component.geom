#version 150

// Inspired by https://wickedengine.net/2017/08/voxel-based-global-illumination/

layout (triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float voxelResolution;
uniform float voxelSize;
uniform mat4 mvp;

out vec4 worldPosition; 

void main() {
    // Define the 8 corners of the cube relative to the center
    // Camera is looking down -z, 
    vec3 pos1 = gl_in[0].gl_Position.xyz;
    vec3 pos2 = gl_in[1].gl_Position.xyz;
    vec3 pos3 = gl_in[2].gl_Position.xyz;
    vec3 normal = abs(cross(pos1 - pos2, pos1 - pos3));

    if (normal.x > normal.y && normal.x > normal.z) {
        pos1.xyz = pos1.zyx;
        pos2.xyz = pos2.zyx;
        pos3.xyz = pos3.zyx;
    }
    if (normal.y > normal.x && normal.y > normal.z){
        pos1.xyz = pos1.xzy;
        pos2.xyz = pos2.xzy;
        pos3.xyz = pos3.xzy;
    }

    worldPosition = gl_in[0].gl_Position;
    gl_Position = mvp * vec4(pos1, gl_in[0].gl_Position.w);
    EmitVertex();

    worldPosition = gl_in[1].gl_Position;
    gl_Position = mvp * vec4(pos2, gl_in[1].gl_Position.w);
    EmitVertex();

    worldPosition = gl_in[2].gl_Position;
    gl_Position = mvp * vec4(pos3, gl_in[2].gl_Position.w);
    EmitVertex();
    EndPrimitive();
}
