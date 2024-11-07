all :  project

project: main.cpp ../common/GL_utilities.c ../common/LoadTGA.c ../common/Linux/MicroGlut.c ../common/VectorUtils4.h ../common/LittleOBJLoader.h
	g++ -Wall -o project -DGL_GLEXT_PROTOTYPES main.cpp ../common/GL_utilities.c ../common/LoadTGA.c ../common/Linux/MicroGlut.c -I../common -I../common/Linux -lXt -lX11 -lm -lGL

clean :
	rm main

