// Copyright NVIDIA Corporation 2007 -- Ignacio Castano <icastano@nvidia.com>
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include <nvcore/StrLib.h>
#include <nvcore/StdStream.h>

#include <nvimage/Image.h>
#include <nvimage/DirectDrawSurface.h>


#define GLEW_STATIC
#include <GL/glew.h>

#if NV_OS_DARWIN
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif



#include "cmdline.h"

GLuint tex0, tex1;

float scale = 1.0f;
float target_scale = 1.0f;
float x = 0, y = 0;
float target_x = 0, target_y = 0;
int level = 0;
int max_level = 0;
int win_w, win_h;
int w, h;

bool keys[256];

void initOpengl()
{
    glewInit();

    if (!glewIsSupported(
        "GL_VERSION_2_0 "
        "GL_ARB_vertex_program "
        "GL_ARB_fragment_program "
        ))
    {
        printf("Unable to load required extension\n");
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_TEXTURE_2D);

    glutReportErrors();
}

GLuint createTexture(nv::DirectDrawSurface & dds)
{
    GLuint tex;
    glGenTextures(1, &tex);

    if (dds.isTexture2D()) {
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        const uint count = dds.mipmapCount();

        max_level = count - 1;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, max_level);

        for (uint i = 0; i < count; i++)
        {
            nv::Image img;
            dds.mipmap(&img, 0, i); // face, mipmap

            glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, img.width(), img.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img.pixels());
        }
    }
    else {
        // Add support for cubemaps.
    }
    return tex;
}

void drawQuad()
{
    //glColor3f(1, 0, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(-w, h);
    glTexCoord2f(1.0, 0.0); glVertex2f(w, h);
    glTexCoord2f(1.0, 1.0); glVertex2f(w, -h);
    glTexCoord2f(0.0, 1.0); glVertex2f(-w, -h);
    glEnd();
}


void glutKeyboardCallback(unsigned char key, int x, int y)
{
    keys[key] = true;
    glutPostRedisplay();
}

void glutKeyboardUpCallback(unsigned char key, int x, int y)
{
    keys[key] = false;
    glutPostRedisplay();
}

void glutSpecialCallback(int key, int x, int y)
{
    if (key >= 0 && key < 256) {
        keys[key] = true;
    }
    glutPostRedisplay();
}

void glutSpecialUpCallback(int key, int x, int y)
{
    if (key >= 0 && key < 256) {
        keys[key] = false;
    }
    glutPostRedisplay();
}

void glutReshapeCallback(int w, int h)
{
    win_w = w;
    win_h = h;
    glViewport(0, 0, w, h);
}

void processKeys()
{
    // Process keys.
    if (keys['q']) {
        exit(0);
    }
    if (keys['='] || keys['+']) {
        target_scale += target_scale / 16;
    }
    if (keys['-'] || keys['_']) {
        target_scale -= target_scale / 16;
    }
    if (keys['r']) {
        target_scale = 1.0;
        target_x = target_y = 0.0;
    }

    if (keys[GLUT_KEY_RIGHT]) {
        target_x -= 1 / scale;
        if (target_x < -w) target_x = -w;
    }
    if (keys[GLUT_KEY_LEFT]) {
        target_x += 1 / scale;
        if (target_x > w) target_x = w;
    }
    if (keys[GLUT_KEY_DOWN]) {
        target_y += 1 / scale;
        if (target_y > h) target_y = h;
    }
    if (keys[GLUT_KEY_UP]) {
        target_y -= 1 / scale;
        if (target_y < -h) target_y = -h;
    }
    if (keys[GLUT_KEY_PAGE_DOWN]) {
        if (level < max_level) level++;
    }
    if (keys[GLUT_KEY_PAGE_UP]) {
        if (level > 0) level--;
    }


    // Update parameters.
    /*if (scale < target_scale) {
        scale *= 1.001; //
        if (scale > target_scale) scale = target_scale;
    }
    else if (scale > target_scale) {
        scale *= 0.999; //
        if (scale < target_scale) scale = target_scale;
    }*/

    if (scale != target_scale) {
        glutPostRedisplay();
    }
}

void glutDisplayCallback(void)
{
    processKeys();

    // Draw texture.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, tex0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, level);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(target_scale/win_w, target_scale/win_h, 1);
    glTranslatef(target_x, target_y, 0);

    drawQuad();

    glutSwapBuffers();
}

void glutIdleCallback(void)
{
    //processKeys();

    glutPostRedisplay();
}


// View options:
// - Display RGB
// - Display RGBA
// - Display Alpha
// - Zoom in/out
// - Wrap around or clamp
// - View mipmaps.
// - View cube faces.
// - View cube map.


int main(int argc, char *argv[])
{
    MyAssertHandler assertHandler;
    MyMessageHandler messageHandler;

    if (argc != 2 && argc != 3)
    {
        printf("NVIDIA Texture Tools - Copyright NVIDIA Corporation 2007\n\n");
        printf("usage: nvddsview file0 [file1]\n\n");
        return 1;
    }

    // Load surface.
    nv::DirectDrawSurface dds;

    if (!dds.load(argv[1]) || !dds.isValid())
    {
        printf("The file '%s' is not a valid DDS file.\n", argv[1]);
        return 1;
    }

    win_w = w = dds.width();
    win_h = h = dds.height();

    // @@ Clamp window size if texture larger than desktop?


    glutInit(&argc, argv);

    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( win_w, win_h );
    glutCreateWindow( "DDS View" );
    glutReshapeFunc( glutReshapeCallback );
    glutKeyboardFunc( glutKeyboardCallback );
    glutKeyboardUpFunc( glutKeyboardUpCallback );
    glutSpecialFunc( glutSpecialCallback );
    glutSpecialUpFunc( glutSpecialUpCallback );
    glutDisplayFunc( glutDisplayCallback );
    glutIdleFunc( glutIdleCallback );

    initOpengl();

    tex0 = createTexture(dds);

    // @@ Add IMGUI, fade in and out when mouse over.


    glutMainLoop();

    return 0;
}

