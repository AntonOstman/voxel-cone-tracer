#version 150

// Simplified Phong: No materials, only one, hard coded light source
// (in view coordinates) and no ambient

// Note: Simplified! In particular, the light source is given in view
// coordinates, which means that it will follow the camera.
// You usually give light sources in world coordinates.

out vec4 outColor;
in vec3 viewNormal; // Phong
in vec3 worldNormal; // Phong (specular)
in vec3 worldSurface; // Phong (specular)
in vec3 viewSurface; // Phong (specular)

uniform vec3 ka;
uniform vec3 ke;
uniform vec3 kd;
uniform vec3 ks;

uniform vec3 lightSource;
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;

void main(void)
{
    // TODO same for all, should be done on cpu 
	// vec3 light = viewSurface; // Light position in view
	// vec3 light = vec3(mat3(modelviewMatrix) * vec4(lightSource - viewSurface, 1.0));
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
	vec3 shade = kd*diffuse + ks*specular + ka / 5.0 + ke;
	outColor = vec4(shade, 1.0);
}
