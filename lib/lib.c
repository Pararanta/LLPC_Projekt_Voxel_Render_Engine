#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "lib.h"

void display_super(RenderQueque * queque, int size_x, int size_y, int size_z, int resolution, GLuint render_program, GLuint display_program, GLuint fill_program, GLuint overlay_program){
    float * theta = malloc(sizeof(float)*4);
    int time;

    while(1){
        unsigned int time_start = glutGet(GLUT_ELAPSED_TIME);

        GLenum err = glewInit();
        GLuint display, light;

        GLuint render = getRenderVolume(queque, time, size_x, size_y, size_z, fill_program, overlay_program);

        glBindImageTexture(0, light, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(1, render, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
        glUseProgram(render_program);
        glUniform1i(glGetUniformLocation(render_program, "light_x"), size_x/2 + (int)(sin(time/3000.0f)*(size_x/2)));
        glUniform1i(glGetUniformLocation(render_program, "light_y"), size_y/2 + (int)(cos(time/3000.0f)*(size_y/2)));
        glUniform1i(glGetUniformLocation(render_program, "light_z"), size_z/4 + abs((int)(sin(time/5000.0f)*(size_x/4))));
        glDispatchCompute(size_x/16, size_y/16, size_z/8);

        glGenTextures(1, &display);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, display);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution, resolution, 0, GL_RGBA, GL_FLOAT, NULL);

        glBindImageTexture(0, display, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glBindImageTexture(1, light, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

        theta[2] = sin(time/8000.0f);
        theta[3] = cos(time/8000.0f);
        theta[0] = sin(-sin(time/3000.0f)/2-0.7f);
        theta[1] = cos(-sin(time/3000.0f)/2-0.07f);

        glUseProgram(display_program);
        glUniform1fv(glGetUniformLocation(display_program, "theta"),  4, theta);
        glUniform1i(glGetUniformLocation(display_program, "size_x"), size_x);
        glUniform1i(glGetUniformLocation(display_program, "size_y"), size_y);
        glUniform1i(glGetUniformLocation(display_program, "size_z"), size_z);
        glUniform1i(glGetUniformLocation(display_program, "resolution"), resolution);
        glDispatchCompute(resolution/16, resolution/16, 1);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D, display);
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0f, 1.0f);  
        glVertex3f(-1.0f,-1.0f, 1.0f);    
        glTexCoord2f(1.0f, 1.0f);  
        glVertex3f( 1.0f,-1.0f, 1.0f);  
        glTexCoord2f(1.0f, 0.0f);  
        glVertex3f( 1.0f, 1.0f, 1.0f);  
        glTexCoord2f(0.0f, 0.0f);  
        glVertex3f(-1.0f, 1.0f, 1.0f);

        glEnd();
        glutSwapBuffers();

        glDeleteTextures(1, &display);
        glDeleteTextures(1, &light);
        glDeleteTextures(1, &render);
        
        time += glutGet(GLUT_ELAPSED_TIME) - time_start;
        printf("%d\n", 1000/(glutGet(GLUT_ELAPSED_TIME) - time_start));
    }
}

int init(int argc, char **argv, void (*display)(void)){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);	//Line C
	glutInitWindowSize(512,512);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Simple Window");
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glutDisplayFunc(display);
    GLenum err = glewInit();
}

GLuint loadTexture2d(char * path, int size_x, int size_y, int bitdepth){
    GLuint texture;
    unsigned char * data;
    data = stbi_load(path, &size_x, &size_y, &bitdepth, 0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size_x, size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    free(data);
    return texture;
}

GLuint loadTexture3d(char * path, int size_x, int size_y, int size_z, int bitdepth, GLuint loader_program){
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, size_x, size_y, size_z, 0, GL_RGBA, GL_FLOAT, 0);
    glBindImageTexture(0, texture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    for(int z = 0; z < size_z; z++){
        char * subpath;
        asprintf(&subpath, "%s/%04d.png", path, z);
        GLuint layer = loadTexture2d(subpath, size_x, size_y, bitdepth);
        free(subpath);
	    glBindImageTexture(1, layer, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glUseProgram(loader_program);
        glUniform1i(glGetUniformLocation(loader_program, "layer"), z);
        glDispatchCompute(size_x/16, size_y/16, 1);
        glDeleteTextures(1, &layer);
    }

    return texture;
}

GLuint * loadFrames(char * path, int size_x, int size_y, int size_z, int frame_count, int bitdepth, GLuint loader_program){
    GLuint * frames = calloc(frame_count, sizeof(GLuint));

    for(int f = 0; f < frame_count; f++){
        char * subpath;
        asprintf(&subpath, "%s/%d", path, f);
        GLuint frame = loadTexture3d(subpath, size_x, size_y, size_z, bitdepth, loader_program);
        free(subpath);
	    frames[f] = frame;
    }

    return frames;
}

Object * loadObject(char * path, int size_x, int size_y, int size_z, int frame_count, int offset_x, int offset_y, int offset_z, int bitdepth, GLuint loader_program){
    Object * object = malloc(sizeof(Object));

    object->size_x = size_x;
    object->size_y = size_y;
    object->size_z = size_z;

    object->offset_x = offset_x;
    object->offset_y = offset_y;
    object->offset_z = offset_z;

    object->frame_count = frame_count;
    object->frames = loadFrames(path, size_x, size_y, size_z, frame_count, bitdepth, loader_program);

    return object;
}

GLuint getRenderVolume(RenderQueque * queque, int time, int size_x, int size_y, int size_z, GLuint fill_program, GLuint overlay_program){
    GLuint render;
    glGenTextures(1, &render);
    glBindTexture(GL_TEXTURE_3D, render);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, 128, 128, 128, 0, GL_RGBA, GL_FLOAT, 0);
    glBindImageTexture(0, render, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
    glUseProgram(fill_program);
    glDispatchCompute(size_x/16, size_y/16, size_z/8);

    time /= 125;

    while(queque){

        glBindImageTexture(1, queque->this->frames[time%queque->this->frame_count], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
        glUseProgram(overlay_program);
        glUniform1i(glGetUniformLocation(overlay_program, "offset_x"), queque->this->offset_x);
        glUniform1i(glGetUniformLocation(overlay_program, "offset_y"), queque->this->offset_y);
        glUniform1i(glGetUniformLocation(overlay_program, "offset_z"), queque->this->offset_z);
        glDispatchCompute(queque->this->size_x/16, queque->this->size_y/16, queque->this->size_z/8);
        queque = queque->next;
    }

    return render;
}

RenderQueque * addToQueque(RenderQueque * queque, Object * object){
    RenderQueque * new = malloc(sizeof(RenderQueque));
    new->next = queque;
    new->this = object;
    return new;
}

GLuint generateLoaderProgram() {
    GLuint progHandle = glCreateProgram();
    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

    const char *csSrc[] = {
        "#version 430\n",
        "uniform int layer;\n\
         layout (binding=0, rgba32f) uniform image3D target;\n\
         layout (binding=1, rgba32f) uniform image2D source;\n\
         layout (local_size_x = 16, local_size_y = 16) in;\n\
         void main() {\n\
            ivec2 position = ivec2(gl_GlobalInvocationID.xy);\n\
            vec4 color = imageLoad(source, position);\n\
            /*color = vec4(vec3(1.0f,1.0f,1.0f) - vec3(color), color.w*0.05f);*/\n\
            imageStore(target, ivec3(position, layer), color);\n\
         }"
    };

    glShaderSource(cs, 2, csSrc, NULL);
    glCompileShader(cs);
    int rvalue;
    glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        printf("Error in compiling the compute shader\n");
        GLchar log[10240];
        GLsizei length;
        glGetShaderInfoLog(cs, 10239, &length, log);
        printf("Compiler log:\n%s\n", log);
        exit(40);
    }
    glAttachShader(progHandle, cs);

    glLinkProgram(progHandle);
    glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        printf("Error in linking compute shader program\n");
        GLchar log[10240];
        GLsizei length;
        glGetProgramInfoLog(progHandle, 10239, &length, log);
        printf("Linker log:\n%s\n", log);
        exit(41);
    }   
    glUseProgram(progHandle);
    
    glUniform1i(glGetUniformLocation(progHandle, "destTex"), 0);
    
    return progHandle;
}

GLuint generateOverlayProgram() {
    // Creating the compute shader, and the program object containing the shader
    GLuint progHandle = glCreateProgram();
    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

    // In order to write to a texture, we have to introduce it as image2D.
    // local_size_x/y/z layout variables define the work group size.
    // gl_GlobalInvocationID is a uvec3 variable giving the global ID of the thread,
    // gl_LocalInvocationID is the local index within the work group, and
    // gl_WorkGroupID is the work group's index
    const char *csSrc[] = {
        "#version 430\n",
        "uniform int offset_x;\n\
         uniform int offset_y;\n\
         uniform int offset_z;\n\
         layout (binding=0, rgba32f) uniform image3D target;\n\
         layout (binding=1, rgba32f) uniform image3D source;\n\
         layout (local_size_x = 16, local_size_y = 16, local_size_z = 8) in;\n\
         void main() {\n\
            ivec3 source_position = ivec3(gl_GlobalInvocationID.xyz);\n\
            ivec3 target_position = source_position + ivec3(offset_x, offset_y, offset_z);\n\
            vec4 base_color = imageLoad(source, source_position);\n\
            vec4 overlay_color = imageLoad(target, target_position);\n\
            float new_alpha = (1 - overlay_color.w)*base_color.w + overlay_color.w;\n\
            vec4 color = vec4(((1-overlay_color.w)*base_color.w*vec3(base_color) + overlay_color.w*vec3(overlay_color))/new_alpha, new_alpha);\n\
            imageStore(target, target_position, color);\n\
         }"
    };

    glShaderSource(cs, 2, csSrc, NULL);
    glCompileShader(cs);
    int rvalue;
    glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        printf("Error in compiling the compute shader\n");
        GLchar log[10240];
        GLsizei length;
        glGetShaderInfoLog(cs, 10239, &length, log);
        printf("Compiler log:\n%s\n", log);
        exit(40);
    }
    glAttachShader(progHandle, cs);

    glLinkProgram(progHandle);
    glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        printf("Error in linking compute shader program\n");
        GLchar log[10240];
        GLsizei length;
        glGetProgramInfoLog(progHandle, 10239, &length, log);
        printf("Linker log:\n%s\n", log);
        exit(41);
    }   
    glUseProgram(progHandle);
    
    glUniform1i(glGetUniformLocation(progHandle, "destTex"), 0);
    
    return progHandle;
}

GLuint generateTextureFill() {
    // Creating the compute shader, and the program object containing the shader
    GLuint progHandle = glCreateProgram();
    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

    // In order to write to a texture, we have to introduce it as image2D.
    // local_size_x/y/z layout variables define the work group size.
    // gl_GlobalInvocationID is a uvec3 variable giving the global ID of the thread,
    // gl_LocalInvocationID is the local index within the work group, and
    // gl_WorkGroupID is the work group's index
    const char *csSrc[] = {
        "#version 430\n",
        "layout (binding=0, rgba32f) uniform image3D target;\n\
         layout (local_size_x = 16, local_size_y = 16, local_size_z = 8) in;\n\
         void main() {\n\
            ivec3 position = ivec3(gl_GlobalInvocationID.xyz);\n\
            imageStore(target, position, vec4(0.0f, 0.0f, 0.0f, 0.0f));\n\
         }"
    };

    glShaderSource(cs, 2, csSrc, NULL);
    glCompileShader(cs);
    int rvalue;
    glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        printf("Error in compiling the compute shader\n");
        GLchar log[10240];
        GLsizei length;
        glGetShaderInfoLog(cs, 10239, &length, log);
        printf("Compiler log:\n%s\n", log);
        exit(40);
    }
    glAttachShader(progHandle, cs);

    glLinkProgram(progHandle);
    glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        printf("Error in linking compute shader program\n");
        GLchar log[10240];
        GLsizei length;
        glGetProgramInfoLog(progHandle, 10239, &length, log);
        printf("Linker log:\n%s\n", log);
        exit(41);
    }   
    glUseProgram(progHandle);
    
    glUniform1i(glGetUniformLocation(progHandle, "destTex"), 0);
    
    return progHandle;
}

GLuint generateRenderProgram() {
    // Creating the compute shader, and the program object containing the shader
    GLuint progHandle = glCreateProgram();
    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

    const char *csSrc[] = {
        "#version 430\n",
          "uniform int light_x;\n\
            uniform int light_y;\n\
            uniform int light_z;\n\
          layout (binding=0, rgba32f) uniform image3D render;\
          layout (binding=1, rgba32f) uniform image3D source;\
         layout (local_size_x = 16, local_size_y = 16, local_size_z = 8) in;\n\
          void main() {\
                    ivec3 position = ivec3(gl_GlobalInvocationID.xyz);\
                    ivec3 light_position = ivec3(light_x, light_y, light_z);\
                    vec3 total_absorbtion = vec3(0.0f, 0.0f, 0.0f);\
                    ivec3 distance2 = light_position - position;\
                    int distance = distance2.x*distance2.x + distance2.y*distance2.y + distance2.z*distance2.z;\
                    \
                    uint answer;\
                    uint x;\
                    uint temp;\
                    \
                    if(distance > 2){\
                        answer = 128; \
                        for( x=0x8000; x>0; x=x>>1 )\
                        {\
                            answer |= x;\
                            temp = answer*answer;\
                            if (temp == distance) break;\
                            if (temp > distance) answer ^= x;\
                        }\
                    }\
                    distance = int(answer);\
                    \
                    vec3 step = distance2/float(distance);\
                    for(int k = 0; k < distance; k++){\
                        ivec3 new_position = light_position - ivec3(step*k);\
                        /*if(!(new_position.x < 256 && new_position.y < 256 && new_position.z < 256  && new_position.x >= 0 && new_position.y >= 0 && new_position.z >= 0 && new_position.z >= 0)) continue;*/\
                        if(position.x == new_position.x && position.y == new_position.y && position.z == new_position.x) continue;\
                        vec4 absorbtion_color = imageLoad(source, new_position);\
                        absorbtion_color = vec4(vec3(1.0f,1.0f,1.0f)-vec3(absorbtion_color), absorbtion_color.w);\
                        total_absorbtion += vec3(absorbtion_color)*absorbtion_color.w;\
                        total_absorbtion = max(total_absorbtion, vec3(0.0f, 0.0f, 0.0f));\
                        /*if(total_absorbtion.x + total_absorbtion.y + total_absorbtion.z == 0.0f) break;*/\
                    }\
            total_absorbtion = min(total_absorbtion, vec3(1.0f, 1.0f, 1.0f));\
            vec4 albedo_color = imageLoad(source, position);\
            imageStore(render, position, vec4(vec3(albedo_color) - total_absorbtion, albedo_color.w));\
         }"
    };

   glShaderSource(cs, 2, csSrc, NULL);
    glCompileShader(cs);
    int rvalue;
    glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        printf("Error in compiling the compute shader\n");
        GLchar log[10240];
        GLsizei length;
        glGetShaderInfoLog(cs, 10239, &length, log);
        printf("Compiler log:\n%s\n", log);
        exit(40);
    }
    glAttachShader(progHandle, cs);

    glLinkProgram(progHandle);
    glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        printf("Error in linking compute shader program\n");
        GLchar log[10240];
        GLsizei length;
        glGetProgramInfoLog(progHandle, 10239, &length, log);
        printf("Linker log:\n%s\n", log);
        exit(41);
    }   
    glUseProgram(progHandle);
    
    glUniform1i(glGetUniformLocation(progHandle, "destTex"), 0);
    
    return progHandle;
}

GLuint generateDisplayProgram() {
    GLuint progHandle = glCreateProgram();
    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

const char *csSrc[] = {
        "#version 430\n",
        "uniform float theta[];\
        uniform int size_x;\
        uniform int size_y;\
        uniform int size_z;\
        uniform int resolution;\
        layout (binding=0, rgba32f) uniform image2D render;\
         layout (binding=1, rgba32f) uniform image3D threed;\
         layout (local_size_x = 16, local_size_y = 16) in;\
         void main() {\
            int max_size = max(max(size_x,size_y),size_z);\
            ivec2 position = ivec2(gl_GlobalInvocationID.xy);\
            vec3 initial_position = vec3(position.x/(float(resolution)*2) - 0.25f, position.y/(float(resolution)*2) - 0.25f,  0);\
            initial_position = vec3(initial_position.x*theta[3] - initial_position.y * theta[1]*theta[2], initial_position.x*theta[2] + initial_position.y*theta[1]*theta[3], initial_position.y*theta[0]);\
            vec4 color = vec4(1.0f, 1.0f, 1.0f, 0.0f);\
\
            float theta02 = theta[0]*theta[2];\
            float theta03 = theta[0]*theta[3];\
\
            for(int i = resolution-1; i > 0 ; i--){\
                vec3 my_position = initial_position;\
                float z = i/float(resolution) - 0.5f;\
                my_position.x +=z*theta02;\n\
                my_position.z +=z*theta[1];\n\
                my_position.y -= z*theta03;\
    \
                my_position *= float(resolution)/float(max_size);\
                my_position += vec3(0.5f, 0.5f, 0.5f);\
                my_position *= float(max_size);\
                vec4 newColor;\
                if(my_position.x < size_x && my_position.y < size_y && my_position.z < size_z && my_position.x >= 0.0f && my_position.y >= 0.0f && my_position.z >= 0.0f && my_position.z >= 0) newColor = imageLoad(threed, ivec3(my_position));\
                else continue;\
                if(newColor.w == 0.0f) continue;\
                float new_alpha = (1 - color.w)*newColor.w + color.w;\n\
                color = vec4(((1-color.w)*newColor.w*vec3(newColor) + color.w*vec3(color))/new_alpha, new_alpha);\n\
                if(color.w == 1.0f) break;\
            }\
            imageStore(render, position, color);\
            if(color.w == 0.0f) imageStore(render, position, vec4(1.0f, 1.0f, 1.0f, 1.0f));\
         }"
    };

   glShaderSource(cs, 2, csSrc, NULL);
    glCompileShader(cs);
    int rvalue;
    glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        printf("Error in compiling the compute shader\n");
        GLchar log[10240];
        GLsizei length;
        glGetShaderInfoLog(cs, 10239, &length, log);
        printf("Compiler log:\n%s\n", log);
        exit(40);
    }
    glAttachShader(progHandle, cs);

    glLinkProgram(progHandle);
    glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        printf("Error in linking compute shader program\n");
        GLchar log[10240];
        GLsizei length;
        glGetProgramInfoLog(progHandle, 10239, &length, log);
        printf("Linker log:\n%s\n", log);
        exit(41);
    }   
    glUseProgram(progHandle);
    
    glUniform1i(glGetUniformLocation(progHandle, "destTex"), 0);
    
    return progHandle;
}