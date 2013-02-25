////////////////////////////////////////////////////
// anim.cpp
// Template code for drawing an articulated figure.
// CS 174A 
////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef WIN32
#include <GL/glut.h>
#endif

#ifdef WIN32
#include "GL/freeglut.h"
#endif

#include "Ball.h"
#include "FrameSaver.h"
#include "Timer.h"

FrameSaver FrSaver ;
Timer TM ;

BallData *Arcball = NULL ;
int Perspective = 1 ;
int Width = 480 ;
int Height = 480 ;
int Button = -1 ;
float Zoom = 1 ;
int PrevY = 0 ;

int Animate = 0 ;
int Recording = 0 ;

void resetArcball() ;
void save_image();
void instructions();
void set_colour(float r, float g, float b) ;

const int STRLEN = 100;
typedef char STR[STRLEN];

#define PI 3.1415926535897
#define X 0
#define Y 1
#define Z 2

// The eye point and look-at point.
double eye[3] = {0, 0.0, 50.0};
double ref[3] = {0.0, 0.0, 0.0};
double TIME = 0.0 ;

/////////////////////////////////////////////////////
//    PROC: drawCylinder()
//    DOES: this function 
//          render a solid cylinder  oriented along the Z axis. Both bases are of radius 1. 
//          The bases of the cylinder are placed at Z = 0, and at Z = 1.
//
//          
// Don't change.
//////////////////////////////////////////////////////
void drawCylinder(void)
{
    static GLUquadricObj *cyl = NULL ;
    if( cyl == NULL )
    {
        cyl = gluNewQuadric() ;
    }
    if( cyl == NULL )
    {
        fprintf(stderr,"Cannot allocate cylinder.\n") ;
        return ;
    }
    gluQuadricDrawStyle(cyl,GLU_FILL) ;
    gluQuadricNormals(cyl,GLU_SMOOTH) ;
    gluCylinder(cyl,1.0,1.0,1.0,10,10) ;
}

//////////////////////////////////////////////////////
//    PROC: drawCone()
//    DOES: this function 
//          render a solid cone oriented along the Z axis with base radius 1. 
//          The base of the cone is placed at Z = 0, and the top at Z = 1. 
//         
// Don't change.
//////////////////////////////////////////////////////
void drawCone(void)
{
    glutSolidCone(1,1,20,20) ;
}


void drawSquareTex()
{
    float i,j ;

    int NSUB = 50 ;
    float d = 1.0 / NSUB ;

    for( i = -0.5 ; i < 0.5 ; i += d )
        for( j = -0.5 ; j < 0.5 ; j+= d )
        {
            glBegin(GL_POLYGON) ;
            glNormal3f(0,0,1) ;
            float r = 1.0 ;

            //r = 0.5 + i+j ;
            float radius = 0.5 ;
            r = 0.5 + cos((i*i+j*j)*20*3.14) ;
            set_colour(r,0.5,0.1) ;
            glVertex3f(i,j,0) ;
            glVertex3f(i+d,j,0) ;
            glVertex3f(i+d,j+d,0) ;
            glVertex3f(i,j+d,0) ;
            glEnd() ;
        }
}


//////////////////////////////////////////////////////
//    PROC: drawCube()
//    DOES: this function draws a cube with dimensions 1,1,1
//          centered around the origin.
// 
// Don't change.
//////////////////////////////////////////////////////

void drawCube(void)
{
    glutSolidCube(1.0) ;
}


//////////////////////////////////////////////////////
//    PROC: drawSphere()
//    DOES: this function draws a sphere with radius 1
//          centered around the origin.
// 
// Don't change.
//////////////////////////////////////////////////////

void drawSphere(void)
{ 
    glutSolidSphere(1.0, 50, 50) ;
}


void resetArcball()
{
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}


//////////////////////////////////////////////////////
//    PROC: myKey()
//    DOES: this function gets caled for any keypresses
// 
//////////////////////////////////////////////////////

void myKey(unsigned char key, int x, int y)
{
    float time ;
    switch (key) {
        case 'q':
        case 27:
            exit(0); 
        case 's':
            FrSaver.DumpPPM(Width,Height) ;
            break;
        case 'r':
            resetArcball() ;
            glutPostRedisplay() ;
            break ;
        case 'a': // togle animation
            Animate = 1 - Animate ;
            // reset the timer to point to the current time		
            time = TM.GetElapsedTime() ;
            TM.Reset() ;
            // printf("Elapsed time %f\n", time) ;
            break ;
        case '0':
            //reset your object
            break ;
        case 'm':
            if( Recording == 1 )
            {
                printf("Frame recording disabled.\n") ;
                Recording = 0 ;
            }
            else
            {
                printf("Frame recording enabled.\n") ;
                Recording = 1  ;
            }
            FrSaver.Toggle(Width);
            break ;
        case 'h':
        case '?':
            instructions();
            break;
    }
    glutPostRedisplay() ;

}


/*********************************************************
    PROC: myinit()
    DOES: performs most of the OpenGL intialization
     -- change these with care, if you must.
**********************************************************/

void myinit(void)
{
    GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
    //GLfloat position[] = { -3.0, 3.0, 3.0, 0.0 };
    GLfloat position[] = { 0.0, 0.0, 30.0, 1.0 };
    GLfloat diffuse2[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat specular2[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat position2[] = { 0.0, 100.0, 00.0, 1.0 };

    GLfloat lmodel_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat local_view[] = { 0.0 };

    /**** set lighting parameters ****/
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightf(GL_LIGHT0, GL_SHININESS, 100) ;
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE) ;

    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse2);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular2);
    glLightfv(GL_LIGHT1, GL_POSITION, position2);
    glLightf(GL_LIGHT1, GL_SHININESS, 500) ;

    /*    glFrontFace (GL_CW); */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glPixelStorei(GL_PACK_ALIGNMENT,1) ;
    glPixelStorei(GL_UNPACK_ALIGNMENT,1) ;
    glShadeModel(GL_SMOOTH) ;

    Arcball = new BallData ;
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}

/*********************************************************
    PROC: set_colour();
    DOES: sets all material properties to the given colour
    -- don't change
**********************************************************/

void set_colour(float r, float g, float b)
{
    float ambient = 0.2f;
    float diffuse = 0.7f;
    float specular = 0.7f;
    GLfloat mat[4];
    /**** set ambient lighting parameters ****/
    mat[0] = ambient*r;
    mat[1] = ambient*g;
    mat[2] = ambient*b;
    mat[3] = 1.0;
    glMaterialfv (GL_FRONT, GL_AMBIENT, mat);

    /**** set diffuse lighting parameters ******/
    mat[0] = diffuse*r;
    mat[1] = diffuse*g;
    mat[2] = diffuse*b;
    mat[3] = 1.0;
    glMaterialfv (GL_FRONT, GL_DIFFUSE, mat);

    /**** set specular lighting parameters *****/
    mat[0] = specular*r;
    mat[1] = specular*g;
    mat[2] = specular*b;
    mat[3] = 1.0;
    glMaterialfv (GL_FRONT, GL_SPECULAR, mat);
    glMaterialf (GL_FRONT, GL_SHININESS, 1.0);
}


/*********************************************************
	Drawing functions -- called in Display() to create the scene
**********************************************************/

void drawGround(void) //draws the ground for the animation
{
	glPushMatrix();
	set_colour(0, .37, 0); //sets the plane color to dark green
	glScalef(40, 40, .3);
	drawCube();
	glPopMatrix();
}

/* drawBee subroutines ********/
void drawBody()
{
	//first draw the back ball
	set_colour(1, 1, .2); //yellow
	glScalef(2, 1, 1);
	drawSphere();

	//now the front 
	set_colour(.2, .2, .2); //gray
	glTranslatef(1.5, 0, 0);
	drawCube();

	//head
	set_colour(0, 0, .5); //navy
	glScalef(.5, 1, 1); //back to round
	glScalef(.5, .5, .5); //smaller
	glTranslatef(3, 0, 0);
	drawSphere(); 

	//translate back to center block
	glTranslatef(-3, 0, 0);
	

	
}

void drawWings()
{
	glPushMatrix();
	set_colour(.2, .2, .2); //gray
	
	glTranslatef(0, 1, 4);
	glScalef(2, .3, 6);
	drawCube();
	//undo, draw other wing
	glScalef(.5, 3.333, .1667);
	glTranslatef(0, 0, -8);
	glScalef(2, .3, 6);
	drawCube();
	glPopMatrix();
}

void drawLegs(char side)
{
	glPushMatrix();
	set_colour(.2, .2, .2); //gray
	//top parts
	int angle;
	if (side == 'l') 
	{
		angle = 45;
		glTranslatef(1, -1.5, 1.5);
	}
	else
	{
		angle = -45;
		glTranslatef(1, -1.5, -1.5);
	}

	glRotatef(angle, 1, 0, 0);
	glScalef(.5, .5, 2);
	drawCube();

	for (int i = 1; i < 3; i++)
	{
		glTranslatef(-2, 0, 0);	
		drawCube();
	}
	//bottom parts
	glTranslatef(4, -1.25, 0); //move back, and down
	if (side == 'l') glTranslatef(0, 0, .85);
	else	/*r*/	 glTranslatef(0, 0, -.85);

	glScalef(2, 2, .5);
	glRotatef(angle, 1, 0, 0); //90 deg
	glScalef(.5, .5, 2);
	
	drawCube();
	for (int i = 1; i < 3; i++)
	{
		glTranslatef(-2, 0, 0);
		drawCube();
	}

	

	
	glPopMatrix();
}
/*****************************/
void drawBee(void) 
{
	glPushMatrix();
	drawBody();
	drawWings();
	drawLegs('l');
	drawLegs('r');
	glPopMatrix();
}


/* drawFlower subroutines ***********/
void drawFlowerHead(void)
{
	glPushMatrix();
	set_colour(1.0, 0, 0);
	glTranslatef(0, 0, 4);
	
	drawSphere();
	glPopMatrix();
}

void drawFlowerStem(void)
{
	glPushMatrix();

	set_colour(.54, .27, .07);
	glScalef(.15, .15, .44);
	//stem1
	glTranslatef(0, 0, .44);
	drawCube();

	for (int i = 2; i <= 8; i++) //draw the rest of the stems
	{
		glTranslatef(0, 0, .89);
		drawCube();
	}
	
	glPopMatrix(); 
}
/************************************/

void drawFlower()
{
	drawFlowerHead();
	drawFlowerStem();
}


/*********************************************************
**********************************************************
**********************************************************

    PROC: display()
    DOES: this gets called by the event handler to draw
          the scene, so this is where you need to build
          your ROBOT --  
      
        MAKE YOUR CHANGES AND ADDITIONS HERE

    Add other procedures if you like.

**********************************************************
**********************************************************
**********************************************************/
void display(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);   // set the background colour to white
    // OK, now clear the screen with the background colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    eye[Z] = 8 ;
    gluLookAt (eye[X], eye[Y], eye[Z],
        ref[X], ref[Y], ref[Z], 0.0,1.0,0.0);
	
	
    glMatrixMode(GL_MODELVIEW) ;
    glLoadIdentity();
    HMatrix arcball_rot;
    Ball_Value(Arcball,arcball_rot);
    glMultMatrixf((float *)arcball_rot);

    glScalef(Zoom, Zoom, Zoom) ;
	
	//drawGround();
	drawBee();
	//drawFlower();
	

	
	
    glutSwapBuffers();
    if(Recording == 1)
        FrSaver.DumpPPM(Width,Height) ;
}

/**********************************************
    PROC: myReshape()
    DOES: handles the window being resized 
    
      -- don't change
**********************************************************/

void myReshape(int w, int h)
{
    Width = w;
    Height = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /*** this defines the field of view of the camera   ***/
    /*** Making the first 4 parameters larger will give ***/
    /*** a larger field of view, therefore making the   ***/
    /*** objects in the scene appear smaller            ***/
    // glFrustum(-1,1,-1,1,7,100);
    glOrtho(-6,6,-6,6,-500,500) ;
    //glOrtho(-6,6,-6,6,-500,500) ;
    //gluPerspective(20,(float) w/(float) h,1,100) ;
    //glFrustum(-1,1,-1,1,4,100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /*** this sets the virtual camera          ***/
    /*** gluLookAt( x,y,z,   x,y,z   x,y,z );  ***/
    /***            camera  look-at camera-up  ***/
    /***            pos'n    point   vector    ***/
    gluLookAt(eye[X],eye[Y],eye[Z],
        ref[X],ref[Y],ref[Z], 0,1,0);

    HMatrix arcball_rot;
    Ball_Value(Arcball,arcball_rot);
    glMultMatrixf((float *)arcball_rot);
}

void instructions() 
{
    printf("Press:\n");
    printf("  s to save the image\n");
    printf("  r to restore the original view.\n") ;
    printf("  0 to set it to the zero state.\n") ;
    printf("  a to toggle the animation.\n") ;
    printf("  m to toggle frame dumping.\n") ;
    printf("  q to quit.\n");
}

// start or end interaction
void myMouseCB(int button, int state, int x, int y)
{
    Button = button ;
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width-1.0;
        arcball_coords.y = -2.0*(float)y/(float)Height+1.0;
        Ball_Mouse(Arcball, arcball_coords) ;
        Ball_Update(Arcball);
        Ball_BeginDrag(Arcball);

    }
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_UP )
    {
        Ball_EndDrag(Arcball);
        Button = -1 ;
    }
    if( Button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
        PrevY = y ;
    }


    // Tell the system to redraw the window
    glutPostRedisplay() ;
}

// interaction (mouse motion)
void myMotionCB(int x, int y)
{
    if( Button == GLUT_LEFT_BUTTON )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width - 1.0 ;
        arcball_coords.y = -2.0*(float)y/(float)Height + 1.0 ;
        Ball_Mouse(Arcball,arcball_coords);
        Ball_Update(Arcball);
        glutPostRedisplay() ;
    }
    else if( Button == GLUT_RIGHT_BUTTON )
    {
        if( y - PrevY > 0 )
            Zoom  = Zoom * 1.03 ;
        else 
            Zoom  = Zoom * 0.97 ;
        PrevY = y ;
        glutPostRedisplay() ;
    }
}


void idleCB(void)
{
    if( Animate == 1 )
    {
        // TM.Reset() ; // commenting out this will make the time run from 0
        // leaving 'Time' counts the time interval between successive calls to idleCB
        if( Recording == 0 )
            TIME = TM.GetElapsedTime() ;
        else
            TIME += 0.033 ; // save at 30 frames per second.
        printf("TIME %f\n", TIME) ;
        glutPostRedisplay() ; 
    }
}
/*********************************************************
     PROC: main()
     DOES: calls initialization, then hands over control
           to the event handler, which calls 
           display() whenever the screen needs to be redrawn
**********************************************************/

int main(int argc, char** argv) 
{

    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Width,Height);
    glutCreateWindow(argv[0]);

    myinit();

    glutIdleFunc(idleCB) ;
    glutReshapeFunc (myReshape);
    glutKeyboardFunc( myKey );
    glutMouseFunc(myMouseCB) ;
    glutMotionFunc(myMotionCB) ;
    instructions();

    glutDisplayFunc(display);
    glutMainLoop();

    TM.Reset() ;
    return 0;         // never reached
}




