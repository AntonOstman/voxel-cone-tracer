// "ShaderToy Tutorial - Ray Marching for Dummies!" 
// by Martijn Steinrucken aka BigWings/CountFrolic - 2018
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
//
// This shader is part of a tutorial on YouTube
// https://youtu.be/PGtv-dBi2wE

#version 150

#define MAX_STEPS 100
#define MAX_DIST 100.
#define SURF_DIST .01

out vec4 outColor;
uniform sampler3D texUnit;
uniform mat4 invOrtho;

float sdBox( vec3 p, vec3 b)
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float GetDist(vec3 p) {
	vec3 s = vec3(0, 1, 6);
       
    float planeDist = p.y;
    float boxDist = sdBox(p - s, vec3(0.2));
    
    float d = min(boxDist, planeDist);
    //float d = planeDist;
    return d;
}

float RayMarch(vec3 ro, vec3 rd) {
	float dO=0.;
    
    for(int i=0; i<MAX_STEPS; i++) {
    	vec3 p = ro + rd*dO;
        float dS = GetDist(p);
        dO += dS;
        if(dO>MAX_DIST || dS<SURF_DIST) break;
    }    
    
    return dO;
}

void main()
{
    // vec2 uv = (gl_FragCoord.xy  - .5*vec2(1920,1080))/1080.0;
    //
    // vec3 col = vec3(0);
    //
    // vec3 ro = vec3(0, 1, 0);
    // vec3 rd = normalize(vec3(uv.x, uv.y, 1));
    //
    // float d = RayMarch(ro, rd) / 20.0;
    // col = vec3(d,d,d);
    float z = 64;
    vec4 voxelColor = vec4(0);
    for (float i = 0; i < 64; i++){
        // Sample in 3d from 0 -> texture size
        float depth = i/64.0;
        vec4 voxelBoolean = texture(texUnit, vec3(gl_FragCoord.xy/vec2(1920,1080), depth));
        if (voxelBoolean.r > 0.1){
            voxelColor = vec4(vec3(depth), 1.0);
        }
    }  
    
    outColor = voxelColor;
}
