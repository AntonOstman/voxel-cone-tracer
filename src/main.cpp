// Lab 1-1, multi-pass rendering with FBOs and HDR.
// Revision for 2013, simpler light, start with rendering on quad in final stage.
// Switched to low-res Stanford Bunny for more details.
// No HDR is implemented to begin with. That is your task.

// 2018: No zpr, trackball code added in the main program.
// 2021: Updated to use LittleOBJLoader.
// 2022: Cleaned up. Made C++ variant.

#include <GL/gl.h>
#include <GL/glext.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include "MicroGlut.h"
#define MAIN
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"
#include "GL_utilities.h"
// uses framework Cocoa
// uses framework OpenGL

// Window size is defined here since it is also used for the FBOs
const int initWidth = 1080, initHeight = 1080;

/*#define NUM_LIGHTS 4*/

typedef struct {
    float anglex;
    float angley;
    float anglez;

    float tx;
    float ty;
    float tz;
} pose;

pose campose;
pose boxpose;
GLfloat voxelResolution = 64.0;

mat4 projectionMatrix;
mat4 viewMatrix;

GLfloat square[] = {
							-1,-1,0,
							-1,1, 0,
							1,1, 0,
							1,-1, 0};

GLfloat squareTexCoord[] = {
							 0, 0,
							 0, 1,
							 1, 1,
							 1, 0};

GLuint squareIndices[] = {0, 1, 2, 0, 2, 3};

Model* squareModel;

//----------------------Globals-------------------------------------------------
Model **model1;
FBOstruct *fbo1, *fbo2, *fbo3;
GLuint phongshader = 0, plaintextureshader = 0, lowpassshader = 0, lowpassshadery = 0, lowpassshaderx = 0, thresholdshader = 0, combineshader = 0, voxeliser = 0, voxelrender = 0;
GLuint voxelmemory = 0;
GLuint* voxelpointer = &voxelmemory;

GLuint voxelvertexBuffer;
std::vector<vec3> vertices;

void generateVoxelMemory(GLuint* tex, GLsizei voxelResolution){
    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_3D, *tex);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, voxelResolution, voxelResolution, voxelResolution, 0, GL_RED,  GL_BYTE, nullptr);
    printError("glteximage3d");

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    printError("gltexparams");

    glClearTexImage(*tex, 0, GL_RED, GL_BYTE, NULL);
    glBindTexture(GL_TEXTURE_3D, 0);
    printError("bindtexture");
}

//-------------------------------------------------------------------------------------
void runfilter(GLuint shader, FBOstruct *in1, FBOstruct *in2, FBOstruct *out)
{
    glUseProgram(shader);
    // Many of these things would be more efficiently done once and for all
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glUniform1i(glGetUniformLocation(shader, "texUnit"), 0);
    glUniform1i(glGetUniformLocation(shader, "texUnit2"), 1);

    useFBO(out, in1, in2);
    DrawModel(squareModel, shader, "in_Position", NULL, "in_TexCoord");
    glFlush();
}

void init(void)
{
	dumpInfo();  // shader info

	// GL inits
	glClearColor(0.1, 0.1, 0.3, 0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	// Load and compile shaders
	plaintextureshader = loadShaders("src/plaintextureshader.vert", "src/raymarcher.frag");  // puts texture on teapot
	lowpassshader = loadShaders("src/plaintextureshader.vert", "src/lowpass.frag");  // lowpass
	lowpassshaderx = loadShaders("src/plaintextureshader.vert", "src/lowpass-x.frag");  // lowpass
	lowpassshadery = loadShaders("src/plaintextureshader.vert", "src/lowpass-y.frag");  // lowpass
	thresholdshader = loadShaders("src/plaintextureshader.vert", "src/threshold.frag");  // threshold
	combineshader = loadShaders("src/plaintextureshader.vert", "src/combine.frag");  // threshold
	/*phongshader = loadShaders("src/phong.vert", "src/phong.frag");  // renders with light (used for initial renderin of teapot)*/

	phongshader = loadShaders("src/voxeliser.vert", "src/voxeliser.frag");  // renders with light (used for initial renderin of teapot)
	/*voxelrender = loadShaders("src/renderVoxel.vert", "src/renderVoxel.frag");  // renders with light (used for initial renderin of teapot)*/

	printError("init shader");

    generateVoxelMemory(voxelpointer, voxelResolution);

	fbo1 = initFBO(initWidth, initHeight, 0);
	fbo2 = initFBO(initWidth, initHeight, 0);
	fbo3 = initFBO(initWidth, initHeight, 0);

	// load the model
	model1 = LoadModelSet("assets/CornellBox-Original.obj");

	squareModel = LoadDataToModel(
			(vec3 *)square, NULL, (vec2 *)squareTexCoord, NULL,
			squareIndices, 4, 6);

	vec3 cam = vec3(0, 5, 15);
	vec3 point = vec3(0, 1, 0);
	vec3 up = vec3(0, 1, 0);

    // adhoc settings
    boxpose.anglex = 0;
    boxpose.angley = 0;
    boxpose.anglez = 0;
    boxpose.tx = 0;
    boxpose.ty = -9.5 - 0.8*4;
    boxpose.tz = -8.0;

    campose.anglex = 0;
    campose.angley = 0;
    campose.anglez = 0;
    campose.tx = 0;
    campose.ty = 0;
    campose.tz = 0;

	viewMatrix = lookAtv(cam, point, up);
}


/*
   @param sceneFBO - fbo with rendered scene
   @param intermediateFBO - intermediate fbo used in a ping pong step
   @param fboOut - Rendered scene with added bloom effect

   Screen space blooming effect output to fbo
*/

void addBloom(FBOstruct *sceneFBO, FBOstruct *intermediateFBO, FBOstruct *fboOut, 
              GLuint thresholdingshader, GLuint lowpassx, GLuint lowpassy, GLuint combiningshader)
{
   runfilter(thresholdingshader, sceneFBO, 0L, intermediateFBO);

   for (int i = 0; i < 30; i++){
       runfilter(lowpassx, intermediateFBO, 0L, fboOut);
       runfilter(lowpassy, fboOut, 0L, intermediateFBO);
   }

   runfilter(combiningshader, sceneFBO, intermediateFBO, fboOut);
}


void march(){


}

//-------------------------------callback functions------------------------------------------
void display(void)
{
	// This function is called whenever it is time to render
	// a new frame; due to the idle()-function below, this
	// function will get called several times per second

	// render to fbo1!
	useFBO(fbo1, 0L, 0L);

	// Clear framebuffer & zbuffer
	glClearColor(0.2,0.2,0.5,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate shader program
	glUseProgram(phongshader);

    mat4 modelToWorldMatrix = Rx(boxpose.angley) * Rz(boxpose.anglez);
	// Scale and place bunny since it is too small
	modelToWorldMatrix = modelToWorldMatrix * T(boxpose.tx, boxpose.ty, boxpose.tz);
    float scale = 15;
	modelToWorldMatrix = modelToWorldMatrix * S(scale, scale, scale);

	glUniformMatrix4fv(glGetUniformLocation(phongshader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(phongshader, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(phongshader, "worldMatrix"), 1, GL_TRUE, modelToWorldMatrix.m);
	glUniform1i(glGetUniformLocation(phongshader, "texUnit"), 0);

	// Enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

    vec3 lightSource = vec3(-0.24, 1.98, 0.16);
    glClearTexImage(voxelmemory, 0, GL_RED, GL_BYTE, NULL);
    for (int i = 0; model1[i] != NULL; i++)
    {
        glUniform3fv(glGetUniformLocation(phongshader, "ka"), 1, &model1[i]->material->Ka.x);
        glUniform3fv(glGetUniformLocation(phongshader, "kd"), 1, &model1[i]->material->Kd.x);
        glUniform3fv(glGetUniformLocation(phongshader, "ks"), 1, &model1[i]->material->Ks.x);
        glUniform3fv(glGetUniformLocation(phongshader, "ke"), 1, &model1[i]->material->Ke.x);
        glUniform3fv(glGetUniformLocation(phongshader, "lightSource"), 1, &lightSource.x);
        glUniform1fv(glGetUniformLocation(phongshader, "voxelResolution"), 1, &voxelResolution);
    printError("uniforms");

        /*generateVoxelMemory(voxelpointer, 256);*/
        glActiveTexture(GL_TEXTURE0);
        /*glBindImageTexture(GL_TEXTURE_3D, voxelmemory);*/
        /*glTexImage3D(*tex, 0, GL_R8, voxelResolution, voxelResolution, voxelResolution, 0, GL_RED, GL_TEXTURE_3D, nullptr);*/
        glBindImageTexture(0, voxelmemory, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R8);
    printError("bindvoxem memory");

        GLint location = glGetUniformLocation(phongshader, "voxelTexture");
        glUniform1i(location, 0); // Set the sampler to use texture unit 0
    printError("set voxel location");

        DrawModel(model1[i], phongshader, "in_Position", NULL, NULL);
    printError("Draw model error");
    }

    /*DrawModel(squareModel, phongshader, "in_Position", NULL, NULL);*/
	// Done rendering the FBO! Set up for rendering on screen, using the result as texture!

	/*glFlush(); // Can cause flickering on some systems. Can also be necessary to make drawing complete.*/

   /*runfilter(thresholdshader, fbo1, 0L, fbo2);*/
   /**/
   /*for (int i = 0; i< 30; i++){*/
   /*    runfilter(lowpassshaderx, fbo2, 0L, fbo3);*/
   /*    runfilter(lowpassshadery, fbo3, 0L, fbo2);*/
   /*}*/
   /**/
   /*runfilter(combineshader, fbo1, fbo2, fbo3);*/

    /*addBloom(fbo1, fbo2, fbo3, thresholdshader, lowpassshaderx, lowpassshadery, combineshader);*/
	useFBO(0L, fbo1, 0L);
	/*useFBO(0L, fbo3, 0L);*/

	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_3D, voxelmemory);
    glActiveTexture(GL_TEXTURE0);
	glUniformMatrix4fv(glGetUniformLocation(plaintextureshader, "invOrtho"), 1, GL_TRUE, InvertMat4(projectionMatrix).m);
	glUniform1i(glGetUniformLocation(plaintextureshader, "texUnit"), 0);

    glUseProgram(plaintextureshader);

	DrawModel(squareModel, plaintextureshader, "in_Position", NULL, NULL);

	glutSwapBuffers();
}




void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	GLfloat ratio = (GLfloat) w / (GLfloat) h;
	/*projectionMatrix = perspective(100, ratio, 1.0, 1000);*/
    float val = 100;
    projectionMatrix = ortho(-val, val, -val, val, -val, val);
}

// Trackball

int prevx = 0, prevy = 0;

void mouseUpDown(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		prevx = x;
		prevy = y;
	}
}

void mouseDragged(int x, int y)
{
	vec3 p;
	mat4 m;
	
	// This is a simple and IMHO really nice trackball system:
	
	// Use the movement direction to create an orthogonal rotation axis

	p.y = x - prevx;
	p.x = -(prevy - y);
	p.z = 0;


	// Create a rotation around this axis and premultiply it on the model-to-world matrix
	// Limited to fixed camera! Will be wrong if the camera is moved!

	/*m = ArbRotate(p, sqrt(p.x*p.x + p.y*p.y) / 50.0); // Rotation in view coordinates	*/
	/*modelToWorldMatrix = Mult(m, modelToWorldMatrix);*/
    boxpose.anglez += (x - prevx) / 10.0;
    boxpose.angley += (y - prevy) / 10.0;
	
	prevx = x;
	prevy = y;
	
	glutPostRedisplay();
}
unsigned char prev_key;

void translate(float x, float y, float z, mat4* matrix){
    matrix->m[3] = matrix->m[3] + x;
    matrix->m[7] = matrix->m[7] + y; 
    matrix->m[11] = matrix->m[11] + z; 
    /*printf("x: %f, y: %f, z: %f\n", matrix->m[3], matrix->m[7], matrix->m[11]);*/
}

void move(unsigned char key, mat4* matrix, const char* keyset){
    float speed = 0.8;

    if (key == keyset[0]){
        translate(speed, 0.0, 0.0, matrix);
    }
    if (key == keyset[1]){
       translate(0.0, speed ,0.0, matrix);
    }
    if (key == keyset[2]){
        translate(0.0, 0, speed, matrix);
    }
    if (key == keyset[3]){
        translate(-speed, 0, 0, matrix);
    }
    if (key == keyset[4]){
        translate(0.0, -speed, 0.0, matrix);
    }
    if (key == keyset[5]){
        translate(0.0, 0.0, -speed, matrix);
    }
}

void keyPressed(unsigned char key, int xx, int yy) {

    const char* keyset = "aYwdys";
    mat4 posi = mat4(0);
    move(key, &posi, keyset);
    boxpose.tx += posi.m[3];
    boxpose.ty += posi.m[7];
    boxpose.tz += posi.m[11];
}

//-----------------------------main-----------------------------------------------
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(initWidth, initHeight);

	glutInitContextVersion(4, 2);
	glutCreateWindow ("Render to texture with FBO");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutMouseFunc(mouseUpDown);
	glutMotionFunc(mouseDragged);
	glutRepeatingTimer(50);

	init();
	glutMainLoop();
	exit(0);
}

