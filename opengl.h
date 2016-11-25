#ifndef MYOPENGL_H
#define MYOPENGL_H

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

// Funcoes da interface grafica e OpenGL
void init(int argc, char** argv);
void display();
void keyboard( unsigned char key, int x, int y );
void mouse(int button, int state, int x, int y);
void arrow_keys(int key, int x, int y);
void reshape(int w, int h);
void buildTex();

#endif
