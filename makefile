all :  project

COMMON_PATH = ./common
SRC = ./src

project: $(SRC)/main.cpp $(COMMON_PATH)/GL_utilities.c $(COMMON_PATH)/LoadTGA.c $(COMMON_PATH)/Linux/MicroGlut.c $(COMMON_PATH)/VectorUtils4.h $(COMMON_PATH)/LittleOBJLoader.h
	g++ -Wall -o project -DGL_GLEXT_PROTOTYPES $(SRC)/main.cpp $(COMMON_PATH)/GL_utilities.c $(COMMON_PATH)/LoadTGA.c $(COMMON_PATH)/Linux/MicroGlut.c -I$(COMMON_PATH) -I$(COMMON_PATH)/Linux -lXt -lX11 -lm -lGL

clean :
	rm project

