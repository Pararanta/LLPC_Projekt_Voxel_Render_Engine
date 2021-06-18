#ifndef LIB_DOT_H
#define LIB_DOT_H
#include <stdio.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <sys/time.h>
#include <math.h>

typedef struct {
    int size_x;
    int size_y;
    int size_z;
    int offset_x;
    int offset_y;
    int offset_z;
    int frame_count;
    GLuint * frames;
} Object;

typedef struct RenderQueque {
    Object * this;
    struct RenderQueque * next;
} RenderQueque;

typedef struct {
    int size_x;
    int size_y;
    int size_z;
    RenderQueque * queue;
} RenderVolume;

GLuint generateLoaderProgram();
GLuint generateTextureFill();
GLuint generateOverlayProgram();
GLuint generateRenderProgram();
GLuint generateDisplayProgram();

void display_super(RenderQueque * queque, int size_x, int size_y, int size_z, int resolution, GLuint render_program, GLuint display_program, GLuint fill_program, GLuint overlay_program);
int init(int argc, char **argv, void (*display)(void));
GLuint loadTexture2d(char * path, int size_x, int size_y, int bitdepth);
GLuint loadTexture3d(char * path, int size_x, int size_y, int size_z, int bitdepth, GLuint loader_program);
GLuint * loadFrames(char * path, int size_x, int size_y, int size_z, int frame_count, int bitdepth, GLuint loader_program);
Object * loadObject(char * path, int size_x, int size_y, int size_z, int frame_count, int offset_x, int offset_y, int offset_z, int bitdepth, GLuint loader_program);

GLuint getRenderVolume(RenderQueque * queque, int time, int size_x, int size_y, int size_z, GLuint fill_program, GLuint overlay_program);
RenderQueque * addToQueque(RenderQueque * queque, Object * object);

#endif