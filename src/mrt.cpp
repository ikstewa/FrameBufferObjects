
#include <stdio.h>
#include <string>
#include <windows.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "glsl/GLSLProgram.hpp"

GLuint fbo;
GLuint depthBuffer;
GLuint imgTexture, outlineTexture;
GLuint depthTexture, normalTexture;

//GLuint fbo2;

const int texWidth = 512;
const int texHeight = 512;

int sWidth = 800;
int sHeight = 600;

GLfloat xrot = 45;
GLfloat yrot = 45;
GLfloat xspeed = 0.02f;
GLfloat yspeed = 0.01f;

// shader program
GLSLProgram * mrtprogram;
GLSLProgram * sobelFilter;

// FPS vars
int frame, my_time, timebase;
char buffer[50];
void drawText(int x, int y, void * font, const char* text);
void fps();

void init()
{

   GLint maxBuffers;
   glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxBuffers);
   printf("MAX_COLOR_ATTACHMENTS: %d\n", maxBuffers);


   glShadeModel(GL_SMOOTH);
   glClearColor(0.0f, 0.0f, 0.2f, 0.5f);
   glClearDepth(1.0f);
   glEnable(GL_DEPTH_TEST);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   glDepthFunc(GL_LEQUAL);
   glViewport(0, 0, sWidth, sHeight);

   // setup the FBO
   glGenFramebuffersEXT(1, &fbo);
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

   // create the render buffer for depth
   glGenRenderbuffersEXT(1, &depthBuffer);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, texWidth, texHeight);

   // IMAGE TEXTURE
   glGenTextures(1, &imgTexture);
   glBindTexture(GL_TEXTURE_2D, imgTexture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   // attach texture
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, imgTexture, 0);


   // DEPTH TEXTURE
   glGenTextures(1, &depthTexture);
   glBindTexture(GL_TEXTURE_2D, depthTexture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   // Attach the texture to the FBO for rendering
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, depthTexture, 0);


   // NORMAL TEXTURE
   glGenTextures(1, &normalTexture);
   glBindTexture(GL_TEXTURE_2D, normalTexture);
   glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA16, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   // attach the second texture
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, normalTexture, 0);

   // attach the depth buffer to the FBO
   glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);

   // define the render targets
   GLenum mrt[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT};
   glDrawBuffers(3, mrt);

   // check the FBO status
   GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
   if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
   {
      printf("ERROR: FBO status not complete\n");
      exit(1);
   }

   //// setup the FBO
   //glGenFramebuffersEXT(1, &fbo2);
   //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

   //glGenTextures(1, &outlineTexture);
   //glBindTexture(GL_TEXTURE_2D, outlineTexture);
   //glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA16, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, outlineTexture, 0);

   //glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);

   //status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
   //if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
   //{
   //   printf("ERROR: FBO status not complete\n");
   //   exit(1);
   //}

   // detach the FBO
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void initShader()
{
   mrtprogram = new GLSLProgram("mrt.vs","mrt.fs");
   sobelFilter = new GLSLProgram("SobelFilter.vert", "SobelFilter.frag");
}

void ShutDown()
{
   glDeleteFramebuffersEXT(1, &fbo);
   //glDeleteFramebuffersEXT(1, &fbo2);
   glDeleteRenderbuffersEXT(1, &depthBuffer);
   glDeleteTextures(1, &depthTexture);
   glDeleteTextures(1, &normalTexture);
   glDeleteTextures(1, &imgTexture);
   glDeleteTextures(1, &outlineTexture);

   delete mrtprogram;
   delete sobelFilter;
}

void reshape( int w, int h )
{
   sWidth = w;
   sHeight = h;

   glViewport( 0, 0, w, h );
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if ( h == 0 )
      gluPerspective(45, (float)w, 1.0, 5000.0);
   else
      gluPerspective(45, (float)w/(float)h, 1.0, 5000.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard( unsigned char key, int x, int y )
{
   switch( key )
   {
   case 27:
      ShutDown();
      exit(0);
      break;
   default:
      break;
   }
}

void idle()
{
   glutPostRedisplay();
}

void drawCube()
{
   // draw the textured cube to the screen
   glBegin(GL_QUADS);
      // Front Face
      glNormal3f( 0.0f, 0.0f, 1.0);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5,  0.5);
      glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5, -0.5,  0.5);
      glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5,  0.5,  0.5);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5,  0.5,  0.5);
      // Back Face
      glNormal3f( 0.0f, 0.0f,-1.0);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5, -0.5, -0.5);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5,  0.5, -0.5);
      glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5,  0.5, -0.5);
      glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5, -0.5, -0.5);
      // Top Face
      glNormal3f( 0.0f, 1.0, 0.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5,  0.5, -0.5);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5,  0.5,  0.5);
      glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5,  0.5,  0.5);
      glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5,  0.5, -0.5);
      // Bottom Face
      glNormal3f( 0.0f,-1.0, 0.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5, -0.5, -0.5);
      glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5, -0.5, -0.5);
      glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5, -0.5,  0.5);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5, -0.5,  0.5);
      // Right face
      glNormal3f( 1.0, 0.0f, 0.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5, -0.5, -0.5);
      glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5,  0.5, -0.5);
      glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5,  0.5,  0.5);
      glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5, -0.5,  0.5);
      // Left Face
      glNormal3f(-1.0, 0.0f, 0.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5, -0.5, -0.5);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5, -0.5,  0.5);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5,  0.5,  0.5);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5,  0.5, -0.5);
   glEnd();

}

void renderTextures()
{
   // bind the FBO for rendering
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

   // save the viewport and set to the size of the texture
   glPushAttrib(GL_VIEWPORT_BIT);
   glViewport(0, 0, texWidth, texHeight);

   // render to the FBO
   glClearColor( 0.0f, 0.0f, 0.0f, 0.5f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   glTranslatef(0.0f, 0.0f, -2.0f);
   glRotatef(xrot, 1.0f, 0.0f, 0.0f);
   glRotatef(yrot, 0.0f, 1.0f, 0.0f);

   // enable shader program
   mrtprogram->use();
   glBegin(GL_QUADS);
      // front face
      glNormal3f( 0.0f, 0.0f, 1.0);
      glColor4f( 0.0f, 1.0f, 0.0f, 1.0f );
      glVertex3f(-0.5f, -0.5f,  0.5f);
      glVertex3f( 0.5f, -0.5f,  0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);
      // Back face
      glNormal3f( 0.0f, 0.0f,-1.0);
      glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      // Top face
      glNormal3f( 0.0f, 1.0, 0.0f);
      glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
      glVertex3f(-0.5f,  0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f( 0.5f,  0.5f, -0.5f);
      // Bottom face
      glNormal3f( 0.0f,-1.0, 0.0f);
      glColor4f(0.0f,1.0f,1.0f,1.0f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f,  0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);
      // Right face
      glNormal3f( 1.0, 0.0f, 0.0f);
      glColor4f(1.0f,1.0f,0.0f,1.0f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f( 0.5f, -0.5f,  0.5f);
      // Left Face
      glNormal3f(-1.0, 0.0f, 0.0f);
      glColor4f(1.0f,1.0f,1.0f,1.0f);
      glVertex3f(-0.5, -0.5f, -0.5f);
      glVertex3f(-0.5, -0.5f,  0.5f);
      glVertex3f(-0.5,  0.5f,  0.5f);
      glVertex3f(-0.5,  0.5f, -0.5f);
   glEnd();
   // disable shader program
   mrtprogram->disable();

   // Restore old viewport and set rendering back to default fb
   glPopAttrib();
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}

void renderQuad()
{
   glBegin(GL_QUADS);
      glNormal3f( 0.0f, 0.0f, 1.0);
      glTexCoord2f(0.0f, 1.0f); glVertex2f( 0.0f, 0.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex2f( 100.0f, 0.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex2f( 100.0f, 100.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex2f( 0.0f, 100.0f);
   glEnd();
}

void display()
{
   renderTextures();

   /*
   glClearColor(0.0f, 0.0f, 0.2f, 0.5f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();

      glLoadIdentity();
      gluOrtho2D(0, 100, 0, 100);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
         glPushAttrib(GL_VIEWPORT_BIT);
         glLoadIdentity();

         //glTranslatef(0, 0, 1);

         //// Bottom left
         //glViewport(0, 0, sWidth/2, sHeight/2);
         //glBindTexture(GL_TEXTURE_2D, imgTexture);
         //renderQuad();
         //// Bottom Right
         //glViewport(sWidth/2, 0, sWidth, sHeight/2);
         //glBindTexture(GL_TEXTURE_2D, outlineTexture);
         //renderQuad();
         // Top left
         //glViewport(0, sHeight/2, sWidth/2, sHeight);
         glBindTexture(GL_TEXTURE_2D, depthTexture);
         glEnable(GL_TEXTURE_2D);
         renderQuad();
         //// Top Right
         //glViewport(sWidth/2, sHeight/2, sWidth, sHeight);
         //glBindTexture(GL_TEXTURE_2D, normalTexture);
         //renderQuad();


         glDisable(GL_TEXTURE_2D);
         glPopAttrib();

      glPopMatrix();
      glMatrixMode(GL_PROJECTION);

   glPopMatrix();
   //*/


   ///*
   glClearColor(0.0f, 0.0f, 0.2f, 0.5f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   // bind the first texture for rendering
   glBindTexture(GL_TEXTURE_2D, depthTexture);
   // regenerate the mipmap images as the texture has changed
   //glGenerateMipmapEXT(GL_TEXTURE_2D);
   glEnable(GL_TEXTURE_2D);

   glTranslatef(-1.0f, 1.0f, -4.5f);
   glRotatef(-xrot, 1.0f, 0.0f, 0.0f);
   glRotatef(-yrot, 0.0f, 1.0f, 0.0f);

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

   drawCube();

   // draw the second cube
   glLoadIdentity();
   glBindTexture(GL_TEXTURE_2D, normalTexture);
   glEnable(GL_TEXTURE_2D);

   glTranslatef(1.0f, 1.0f, -4.5f);
   glRotatef(-xrot, 1.0f, 0.0f, 0.0f);
   glRotatef(-yrot, 0.0f, 1.0f, 0.0f);

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

   drawCube();

   // draw the third cube
   glLoadIdentity();
   glBindTexture(GL_TEXTURE_2D, imgTexture);
   glEnable(GL_TEXTURE_2D);

   glTranslatef(1.0f, -1.0f, -4.5f);
   glRotatef(-xrot, 1.0f, 0.0f, 0.0f);
   glRotatef(-yrot, 0.0f, 1.0f, 0.0f);

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

   drawCube();

   // draw the fourth cube
   glLoadIdentity();
   glBindTexture(GL_TEXTURE_2D, imgTexture);
   glEnable(GL_TEXTURE_2D);

   glTranslatef(-1.0f, -1.0f, -4.5f);
   glRotatef(-xrot, 1.0f, 0.0f, 0.0f);
   glRotatef(-yrot, 0.0f, 1.0f, 0.0f);

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

   sobelFilter->use();
   sobelFilter->sendUniform("colorMap", 0);
   sobelFilter->sendUniform("width", texWidth);
   sobelFilter->sendUniform("height", texHeight);
   drawCube();
   sobelFilter->disable();

   glDisable(GL_TEXTURE_2D);
   //*/

   xrot += xspeed;
   yrot += yspeed;

   //fps();

   glutSwapBuffers();
}

int main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
   glutInitWindowSize(sWidth,sHeight);
   glutCreateWindow( "FrameBuffer Objects!" );

   // Setup GLEW
   GLenum err = glewInit();
   if (err != GLEW_OK || !glewIsSupported("GL_VERSION_2_0"))
   {
      printf("OpenGL 2.0 not supported. No shaders!\n");
      printf("%s\n", glewGetErrorString(err));
      printf("%s\n", (char*)glGetString(GL_VERSION));
      return 0;
   }
   printf("OpenGL 2.0 supported.\n");

   init();
   initShader();

   glutDisplayFunc( display );
   glutReshapeFunc( reshape );
   glutKeyboardFunc( keyboard );
   glutIdleFunc( idle );
   glutMainLoop();

   return 0;
}

void fps()
{
   glClear(GL_DEPTH_BUFFER_BIT);

   // calculate fps
   frame++;
   my_time = glutGet(GLUT_ELAPSED_TIME);
   if (my_time - timebase > 1000) {
      sprintf(buffer, "FPS: %4.2f", frame*1000.0/(my_time-timebase));
      timebase = my_time;
      frame = 0;
   }

   drawText(87, 90, GLUT_BITMAP_HELVETICA_12, buffer);
}


// Draws text to the screen. Can be invoked from anywhere in the display loop
// Specify color via glColor()
// NOTE: coords are from lower left screen. [0,100]
void drawText(int x, int y, void * font, const char* text)
{
	std::string textString(text);

    // TODO: verify correct matrix mode return
    // remember the current matrix mode
    GLint matrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);

    // remember the current lighitng status
    GLboolean flipLights;
    glGetBooleanv(GL_LIGHTING, &flipLights);

    // Change the projection to a new Ortho for screen coord drawing
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, 100.0, 0.0, 100.0);

        // start with a fresh ModelView matrix to allow text drawing spontaneously
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();

            glDisable(GL_LIGHTING);
            glColor3f(1.0f,0.0f,0.0f);

            glRasterPos2i(x,y);

            //for (c = text; *c != '\0'; c++) //Removed because we changed text to be const.
            for (unsigned int index=0; index < textString.size(); ++index)
            {
                glutBitmapCharacter(font, textString[index]);
            }

        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
    glPopMatrix();



    // return to the previous state
    if (flipLights)
        glEnable(GL_LIGHTING);
    // revert matrix_mode
    glMatrixMode(matrixMode);
    //glMatrixMode(GL_MODELVIEW);
}