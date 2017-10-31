#include <math.h>	// For math routines (such as sqrt & trig).
#include<iostream>
#include <fstream>
#include <string>
#include<unistd.h>
using namespace std;
#include <stdio.h>
#include <windows.h>
#include <GL/glut.h>	// OpenGL Graphics Utility Library
// The next global variable controls the animation's state and speed.


float RotateAngle = 0.0f;	// Angle in degrees of rotation around y-axis
float Azimuth = 20.0;	// Rotated up or down by this amount
string line;
char cha[25];
float AngleStepSize = 3.0f;	// Step three degrees at a time
const float AngleStepMax = 10.0f;
const float AngleStepMin = 0.1f;
static float lmodel_twoside[] = {GL_TRUE};
//static float lmodel_oneside[] = {GL_FALSE};

int WireFrameOn = 1;
float garbage_R = 1.0, garbage_G = 1.0, garbage_B = 1.0;
GLubyte image[1024][1024][3];

float levely=0.0,rad=0.8,a1=90,a2=3.0;	// == 1 for wire frame mode
GLUquadricObj* myReusableQuadric;


void drawGluSlantCylinder(double height, double radiusBase, double radiusTop, int slices, int stacks)
{
    // Draw the cylinder.
    gluQuadricNormals(myReusableQuadric, GLU_SMOOTH);
    gluQuadricTexture(myReusableQuadric, 1);
    gluCylinder(myReusableQuadric, radiusBase, radiusTop, height, slices, stacks);
}

void drawGluCylinder(double height, double radius, int slices, int stacks) {
    drawGluSlantCylinder(height, radius, radius, slices, stacks);
}


void drawGluSlantCylinderWithCaps(double height, double radiusBase, double radiusTop, int slices, int stacks)
{
    // First draw the cylinder
    drawGluSlantCylinder(height, radiusBase, radiusTop, slices, stacks);
    // Draw the top disk cap
    glPushMatrix();
    gluQuadricNormals(myReusableQuadric, GLU_SMOOTH);
    gluQuadricTexture(myReusableQuadric, 1);
    glTranslated(0.0, 0.0, height);
    gluDisk(myReusableQuadric, 0.0, radiusTop, slices, stacks);
    glPopMatrix();
    // Draw the bottom disk cap
    glPushMatrix();
    gluQuadricNormals(myReusableQuadric, GLU_SMOOTH);
    gluQuadricTexture(myReusableQuadric, 1);
    glRotated(180.0, 1.0, 0.0, 0.0);
    gluDisk(myReusableQuadric, 0.0, radiusBase, slices, stacks);
    glPopMatrix();
}

void drawGluCylinderWithCaps(double height, double radius, int slices, int stacks) {
    drawGluSlantCylinderWithCaps(height, radius, radius, slices, stacks);
}

// glutKeyboardFunc is called below to set this function to handle
//	all "normal" key presses.

void myKeyboardFunc(unsigned char key, int x, int y)
{
    switch (key) {
        case 'w':
            WireFrameOn = 1 - WireFrameOn;
            if (WireFrameOn) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Just show wireframes
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// Show solid polygons
            }
            glutPostRedisplay();
        break;
        case 'R':
            AngleStepSize *= 1.5;
            if (AngleStepSize>AngleStepMax) {
                AngleStepSize = AngleStepMax;
            }
            break;
        case 'r':
            AngleStepSize /= 1.5;
            if (AngleStepSize<AngleStepMin) {
                AngleStepSize = AngleStepMin;
            }
            break;
        case 27:	// Escape key
            exit(1);
    }

}



// glutSpecialFunc is called below to set this function to handle
//	all "special" key presses. See glut.h for the names of
//	special keys.

void mySpecialKeyFunc(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP:
            if (Azimuth<45.0f)
            Azimuth += AngleStepSize;
            if (Azimuth>45.0f) {
                Azimuth =44.0f ;
            }
            break;
        case GLUT_KEY_DOWN:
            if (Azimuth>-45.0f)
                Azimuth -= AngleStepSize;
            if (Azimuth < -45.0f) {
                Azimuth = -44.0f;
            }
            break;
        case GLUT_KEY_LEFT:
            RotateAngle += AngleStepSize;
            if (RotateAngle > 180.0f) {
                RotateAngle -= 360.0f;
            }
            break;
        case GLUT_KEY_RIGHT:
            RotateAngle -= AngleStepSize;
            if (RotateAngle < -180.0f) {
                RotateAngle += 360.0f;
            }
            break;
        case GLUT_KEY_F1:
            a1=135.0;
            a2=3.8;
            break;
        case GLUT_KEY_F2:
            a1=90.0;
            a2=3.0;
            break;
    }
    glutPostRedisplay();
}



/*
* drawScene() handles the animation and the redrawing of the
* graphics window contents.
*/

void drawScene(void)

{
    // Clear the rendering window
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!myReusableQuadric) {
        myReusableQuadric = gluNewQuadric();
        // Should (but don't) check if pointer is still null --- to catch memory allocation errors.
        gluQuadricNormals(myReusableQuadric, GL_TRUE);
    }

    // Rotate the image
    glMatrixMode(GL_MODELVIEW);	// Current matrix affects objects positions
    glLoadIdentity();	// Initialize to the identity
    glTranslatef(-0.5, 0.0, -35.0);	// Translate from origin (in front of viewer)
    glRotatef(RotateAngle, 1.0, 1.0, 1.0);	// Rotate around y-axis
    glRotatef(Azimuth, 1.0, 0.0, 0.0);	// Set Azimuth angle
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, 2);
    glTranslatef(0.0, -1.5, 0.0);
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glColor3f(1.0, 0.0, 0.0);	// Reddish color
    // Parameters: height, radius, slices, stacks
    drawGluSlantCylinder(3.0, 0.9,1.1, 15, 15);
    glPopMatrix();
    //water


    glEnable(GL_CULL_FACE);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, 1);
    glTranslatef(0.0, -1.550, 0.0);
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    //glColor3f(0.30, 0.20, 0.10);	// blue color
    glColor3f(garbage_R, garbage_G, garbage_B);
    // Parameters: height, base radius, top radius, slices, stacks

    drawGluSlantCylinderWithCaps(levely, 0.8, rad, 1000, 200);
    glPopMatrix();
    //cap
    glBindTexture(GL_TEXTURE_2D, 3);
    glEnable(GL_CULL_FACE);
    glPushMatrix();
    glTranslatef(0.0,a2-1.5, 0.0);
    glRotatef(-a1, 1.0, 0.0, 0.0);
  //  glColor3f(0.4, 0.4, 0.4);	// Greyish color
    // Parameters: height, base radius, top radius, slices, stacks
    drawGluSlantCylinderWithCaps(0.5, 1.2, 1.2, 20,7 );
    glPopMatrix();
    //base
    glEnable(GL_CULL_FACE);
    glPushMatrix();
    glTranslatef(0.0, -1.6, 0.0);
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glColor3f(0.6, .60, 0.60);	// red color
    // Parameters: height, base radius, top radius, slices, stacks
    drawGluSlantCylinderWithCaps(0.1, 0.90, 0.90, 20,20 );
    glPopMatrix();
    // Flush the pipeline, swap the buffers
    glEnable(GL_CULL_FACE);
    glPushMatrix();
    glTranslatef(-.4, a2-1.6, 0.0);
    glRotatef(-a1, 1.0, 0.0, 0.0);
    glColor3f(0.0, 0.7, 0.7);	// Greenish color
    // Parameters: height, base radius, top radius, slices, stacks
    drawGluSlantCylinderWithCaps(0.5, .2, .2, 200,200 );
    glPopMatrix();
    glEnable(GL_CULL_FACE);
    glPushMatrix();
    glTranslatef(0.4, a2-1.6, 0.0);
    glRotatef(-a1, 1.0, 0.0, 0.0);
    glColor3f(0.0, 0.7, 0.7);	// Greenish color
    // Parameters: height, base radius, top radius, slices, stacks
    drawGluSlantCylinderWithCaps(0.5, .2, .2, 200,200 );
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    ifstream myfile;
    myfile.open("C:\\Users\\user\\hello\\trash\\readfrom.txt", ios::in);
    if (myfile.is_open()){
        getline(myfile,line);
    }
    else
        line="error";
    glRasterPos2f(.41, .4);
    int len=0, i;
    for (i=0; line[i]!='\0'; i++)
        len++;
    cout<<line;
    int j=0;
    for (i=len; line[i]!=';'; i--){
        cha[j]=line[i];
        cout<<cha[j]<<endl;
        j++;
        cout<<endl<<line[i]<<endl;
    }
    cha[j]='\0';
    cout<<"Charac"<<cha[1]<<endl;
    switch(cha[1]){
        case 'a':
            levely=2.6;
            rad=0.89;
            cout<<"a";
            break;
        case 'b':
            levely=1.6;
            rad=0.84;
            break;
        case 'c':
            levely=1.4;
            rad=0.82;
            break;
        case 'd':
            levely=0.2;
            rad=0.80;
            break;
        case 'A':
            garbage_B = 0.0;
            garbage_G = 0.0;
            break;
        case 'B':
            garbage_B = 1.0;
            garbage_G = 1.0;
            break;
        case 'g':
            levely=2.1;
            rad=0.92;
            break;
        case 'h':
            levely=2.4;
            rad=0.94;
            break;
        case 'i':
            levely=2.7;
            rad=0.96;
            break;
        case 'j':
            levely=3.0;
            rad=0.98;
            break;
    }
    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
}


/*
void initLightAndMaterial(void)
{
    static float ambient[] = {0.7, 0.7, 0.7, 0.0};
    static float diffuse[] = {0.5, 0.0, .20, 1.0};
    static float position[] = {90.0, 90.0, 150.0, 0.0};
    static float front_mat_shininess[] ={60.0};
    static float front_mat_specular[] ={0.20, 0.20, 0.20, 1.0};
    static float front_mat_diffuse[] = {1.0, 0.0, 0.0, 0.0};
    static float back_mat_shininess[] = {60.0};
    static float back_mat_specular[] =  {0.0, 0.0, 1.0, 1.0};
    static float back_mat_diffuse[] = {0.0, 0.0, 1.0, 1.0};
    static float lmodel_ambient[] ={1.0, 1.0, 1.0, 1.0};
    //  setMatrix();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess);
    glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse);
    glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess);
    glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular);
    glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
}*/

void make_image(const char* filename)
{
    FILE *file;
    unsigned int dataPos,size,width, height;
	unsigned char header[54];
	file = fopen(filename, "rb");
	fread(header, 1, 54, file);				//Windows BMP begin with 54 byte header

    dataPos		= *(int*)&(header[0x0A]);	//dec10, Actual BMP data
	size		= *(int*)&(header[0x22]);	//dec34, BMP Size
	width		= *(int*)&(header[0x12]);	//dec18, Image Width
	height		= *(int*)&(header[0x16]);	//dec22, Image Height
	fread(image, 1, size, file);

    GLuint texture;
	glGenTextures(1, &texture);				//Generate (allocate) 1 texture name
	glBindTexture(GL_TEXTURE_2D, texture);//Bind the 2D texture
        /* Set up Texturing */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
}


// Initialize OpenGL's rendering modes

void initRendering()
{
    glEnable(GL_DEPTH_TEST);	// Depth testing must be turned on
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Just show wireframes at first

    make_image("C:\\Users\\user\\hello\\trash\\wasteagain.bmp");
    make_image("C:\\Users\\user\\hello\\trash\\metal.bmp");
    make_image("C:\\Users\\user\\hello\\trash\\metal1.bmp");
}

// Called when the window is resized
// w, h - width and height of the window in pixels.

void resizeWindow(int w, int h)
{
    double aspectRatio;
    // Define the portion of the window used for OpenGL rendering.
    glViewport(0, 0, w, h);	// View port uses whole window
    // Set up the projection view matrix: perspective projection
    // Determine the min and max values for x and y that should appear in the window.
    // The complication is that the aspect ratio of the window may not match the
    //	aspect ratio of the scene we want to view.
    w = (w == 0) ? 1 : w;
    h = (h == 0) ? 1 : h;
    aspectRatio = (double)w / (double)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(15.0, aspectRatio, 25.0, 45.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Main routine
// Set up OpenGL, define the callbacks and start the main loop

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(10, 60);
    glutInitWindowSize(360, 360);
    glutCreateWindow("waste bin");
    initRendering();
    glutKeyboardFunc(myKeyboardFunc);	// Handles "normal" ascii symbols
    glutSpecialFunc(mySpecialKeyFunc);	// Handles "special" keyboard keys
    glutReshapeFunc(resizeWindow);
    glutDisplayFunc(drawScene);
    glutMainLoop();
    return(0);
}

