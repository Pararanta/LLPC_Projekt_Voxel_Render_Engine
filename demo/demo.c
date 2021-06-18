#include <lib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <sys/time.h>
#include <math.h>

 GLuint load_program, fill_program, overlay_program, render_program, display_program;

void render(){
    RenderQueque * render = NULL;
    render = addToQueque(render, loadObject("./monument",32,32,48,16,32,72,16,4,load_program));
    render = addToQueque(render, loadObject("./ocean",128,128,32,1,0,0,0,4,load_program));
    render = addToQueque(render, loadObject("./object",128,128,8,1,0,0,8,4,load_program));
    display_super(render, 128, 128, 128, 512, render_program, display_program, fill_program, overlay_program);
}

int main(int argc, char **argv){
    init(argc, argv, render);
    load_program = generateLoaderProgram();
    fill_program = generateTextureFill();
    overlay_program = generateOverlayProgram();
    render_program = generateRenderProgram();
    display_program = generateDisplayProgram();
    glutMainLoop();
    return 0;
}