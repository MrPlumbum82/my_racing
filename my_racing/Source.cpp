//Sadan Mallhi
//Assignment 4
//	NOTE: there is a bug in the program that I dont understand, maybe you could help me.
//			When the car goes of the screen from only the NORTH side the game crashes, but
//			if the car goes of the screen from any other side it just keeps going (which is fine).
//			Why is it behaving differently when the car goes too far North?
#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <stdlib.h>
#include "glut.h"
#include "glm.cpp"

const int ground = 1;
const int road = 2;
static int view_mode = 1;
const int rows = 32;
const int cols = 32;
static GLfloat colors[25][25][3];
int base;
bool racing = false;
const float PI = 3.14159;
GLMmodel *terrain;
GLMmodel *car;

// Human Car
static float rot = 0;
static float speed = 0.05;
static float deltaY2 = 0.0;
static float deltaX2 = 0.0;
static float carY2 = 7.5;
static float carX2 = 26.4;

// AI Car
static float deltaY1 = speed * 1.05;
static float carY1 = 7.5;
static float carX1 = 27;
static char CarOneDirection = 'n';
char currentDir;
bool changeDir = false;


// Camera
const float followDist = 1.8;
static float viewEyeY = carY2 - followDist;
static float viewEyeX = carX2 - followDist;
static float refX = -sin(rot*3.14159 / 180);
static float refY = cos(rot*3.14159 / 180);
static float cameraLocationX = sin(rot*3.14159 / 180);
static float cameraLocationY = -cos(rot*3.14159 / 180);

void init(void)
{
	//Sky Color
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	//Loading .obj files
	terrain = glmReadOBJ("Mount.obj");
	glmUnitize(terrain);
	glmScale(terrain, 28);

	car = glmReadOBJ("Porsche.obj");
	glmUnitize(car);
	glmScale(car, 1);

	//Lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	float light0_amb[] = { .2, .2, .2, 1 };
	float light0_dif[] = { .6, .6, .6, 1 };
	float light0_spec[] = { .75, .75, .75, 1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_spec);

	//Colors for ground
	for (int i = 0; i<rows + 1; i++)
		for (int j = 0; j<cols + 1; j++)
		{
			colors[i][j][0] = 0.0;
			colors[i][j][1] = 0.6 + ((((float)rand() / ((float)RAND_MAX/2))-1)/10);
			colors[i][j][2] = 0.0;
		}

	//Creating list for ground
	glNewList(ground, GL_COMPILE);
	for (int i = 0; i<cols; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= rows; j++)
		{
			glColor3fv(colors[i][j]);
			glVertex3i(i, j, 0);
			glColor3fv(colors[i + 1][j]);
			glVertex3i(i + 1, j, 0);
		}
		glEnd();
	}
	glEndList();
	
	//Creating list for road
	glNewList(road, GL_COMPILE);
	//straight road
	glBegin(GL_QUADS);
		glColor3f(0.3, 0.3, 0.3);
		glVertex3f(6.0, 24.0, 0.0);
		glVertex3f(4.0, 24.0, 0.0);
		glVertex3f(4.0, 8.0, 0.0);
		glVertex3f(6.0, 8.0, 0.0);
	glEnd();

	//curved road
	glBegin(GL_QUAD_STRIP);
		glColor3f(0.3, 0.3, 0.3);
		float outerRadius = 4;
		float innerRadius = 2;
		for (float i = PI/2; i <=  PI; i += PI / 16){ //draws 8 vertices on circumfrance
			glVertex3f(cos(i)*outerRadius + 8, sin(i)*outerRadius + 24, 0);
			glVertex3f(cos(i)*innerRadius + 8, sin(i)*innerRadius + 24, 0);
		}
	glEnd();
	glEndList();

	//Billboard
	base = glGenLists(128);
	for (int i = 0; i<128; i++)
	{
		glNewList(base + i, GL_COMPILE);
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, i);
		glEndList();
	}
	glListBase(base);

}

void DrawLabeledSquare(char *label)
{
	glBegin(GL_QUADS);
	glVertex3i(-2, -1, 0);
	glVertex3i(2, -1, 0);
	glVertex3i(2, 1, 0);
	glVertex3i(-2, 1, 0);
	glEnd();

	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glScalef(1.0 / 300, 1.0 / 300, 1.0 / 300);
	int size = strlen(label);
	glTranslatef(-size / 2.0*104.76, -119.05 / 2, 10.0);
	glLineWidth(5);
	glCallLists(size, GL_BYTE, label);
	glLineWidth(1);
	glPopMatrix();
}

void drawYellowCarDir(){
	if (changeDir == true) {
		CarOneDirection = currentDir;
		changeDir = false;
	}

	switch (CarOneDirection){
	case 'n':
		glRotatef(180, 0, 0, 1);
		break;
	case 'w':
		glRotatef(-90, 0, 0, 1);
		break;
	case 'e':
		glRotatef(90, 0, 0, 1);
		break;
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	switch (view_mode)
	{
	case 1: //Default
		gluLookAt(	16.0, 16.0, 35.0,
					16.0, 16.0, 0.0, 
					0.0, 1.0, 0.0);
		break;
	case 2:	//Spy Hunter
		gluLookAt(	viewEyeX + followDist, viewEyeY + followDist, 9.0,
					viewEyeX + followDist, viewEyeY + followDist, 0.0,
					0.0, 1.0, 0.0); 
		break;
	case 3:	//Chase Car
		gluLookAt(	carX2 + (cameraLocationX * 2.5), carY2 + (cameraLocationY * 2.5), 0.8,
					carX2 + (refX * 100), carY2 + (refY * 100), 0.0,
					0.0, 0.0, 1.0);
		break;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	//Terrain
	glColor3f(0, 1, 0);
	glPushMatrix();
		glTranslatef(4, 16, 1);
		glRotatef(90, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		glmDraw(terrain, GLM_SMOOTH);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(29, 16, 1);
		glRotatef(-90, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		glmDraw(terrain, GLM_SMOOTH);
	glPopMatrix();

	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

	glPushMatrix();
		glTranslated(16, 16, 0);
		glScalef(1.1, 1.1, 0);
		glTranslated(-16, -16, 0);
		glCallList(ground);
	glPopMatrix();
	
	glDisable(GL_DEPTH_TEST);

	//Road
	//Left Side
	glCallList(road);
	//Bottom Side
	glPushMatrix();
		glTranslatef(24, 6, 0);
		glRotatef(90, 0, 0, 1);
		glTranslatef(-6, -8, 0);
		glCallList(road);
	glPopMatrix();
	//Right Side
	glPushMatrix();
		glTranslatef(26, 24, 0);
		glRotatef(-180, 0, 0, 1);
		glTranslatef(-6, -8, 0);
		glCallList(road);
	glPopMatrix();
	//Top Side
	glPushMatrix();
		glTranslatef(8, 26, 0);
		glRotatef(-90, 0, 0, 1);
		glTranslatef(-6, -8, 0);
		glCallList(road);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);

	//Yellow Car		
	glColor3f(1, 1, 0);
	glPushMatrix();
		glTranslatef(carX1, carY1, .17);
		glScalef(0.5, 0.5, 0.5);
		drawYellowCarDir();
		glRotatef(90, 1, 0, 0);
		glmDraw(car, GLM_SMOOTH);
	glPopMatrix();

	//Red Car
	glColor3f(1, 0, 0);
	glPushMatrix();
		glTranslatef(carX2, carY2, .17);
		glScalef(0.5, 0.5, 0.5);
		glRotatef(rot, 0, 0, 1);
		glRotatef(180, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		glmDraw(car, GLM_SMOOTH);
	glPopMatrix();

	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

	// Billboards
	glPushMatrix();
	glTranslatef(16, 4, 1);
	glRotatef(180, 0, 0, 1);
	glRotatef(90, 1, 0, 0);
	glScalef(0.5, 0.5, 0.5);
	glColor3f(0.1, 0.2, 0.3);
	DrawLabeledSquare("Sadan!");
	glPopMatrix();

	glPushMatrix();
	glTranslatef(4, 16, 1);
	glRotatef(90, 0, 0, 1);
	glRotatef(90, 1, 0, 0);
	glScalef(0.5, 0.5, 0.5);
	glColor3f(0.3, 0.1, 0.1);
	DrawLabeledSquare("Made By");
	glPopMatrix();

	glPushMatrix();
	glTranslatef(16, 28, 1);
	glRotatef(0, 0, 0, 1);
	glRotatef(90, 1, 0, 0);
	glScalef(0.5, 0.5, 0.5);
	glColor3f(0.1, 0.1, 0.3);
	DrawLabeledSquare("Racing");
	glPopMatrix();

	glPushMatrix();
	glTranslatef(28, 16, 1);
	glRotatef(-90, 0, 0, 1);
	glRotatef(90, 1, 0, 0);
	glScalef(0.5, 0.5, 0.5);
	glColor3f(0.1, 0.3, 0.1);
	DrawLabeledSquare("Welcome!");
	glPopMatrix();

	glDisable(GL_DEPTH_TEST);

	glutSwapBuffers();
}

void update()
{
	if (racing && viewEyeY + deltaY1 < 32)
	{
		//CarOne Control (Yellow Car)
		if (carX1 >= 27 && carY1 < 27){
			carY1 = carY1 + deltaY1;
			currentDir = 'n';
		} if (carY1 >= 27 && carX1 > 5){
			carX1 = carX1 - deltaY1;
			currentDir = 'w';
		} if (carX1 <= 5 && carY1 > 5){
			carY1 = carY1 - deltaY1;
			currentDir = 's';
		} if (carY1 <= 5 && carY1 < 27){
			carX1 = carX1 + deltaY1;
			currentDir = 'e';
		} 
		if (CarOneDirection != currentDir){
			changeDir = true;
		}

		//CarTwo Control (Red Car)
		deltaX2 = speed * -sin(rot*3.14159 / 180);
		deltaY2 = speed * cos(rot*3.14159 / 180);
		carX2 += deltaX2;
		carY2 += deltaY2;

		//Updating Camera Location
		viewEyeX = carX2 - followDist;
		viewEyeY = carY2 - followDist;
		refX = -sin(rot*3.14159 / 180);
		refY = cos(rot*3.14159 / 180);
		cameraLocationX = sin(rot*3.14159 / 180);
		cameraLocationY = -cos(rot*3.14159 / 180);
		
		glutPostRedisplay();
	}
	else racing = false;
}

void reset(void)
{
	//resetting to default values
	rot = 0;
	deltaY2 = 0.0;
	deltaX2 = 0.0;
	carY2 = 7.5;
	carX2 = 26.4;

	deltaY1 = speed * 1.05;
	carY1 = 7.5;
	carX1 = 27;
	viewEyeY = carY2 - followDist;
	viewEyeX = carX2 - followDist;

	refX = -sin(rot*3.14159 / 180);
	refY = cos(rot*3.14159 / 180);
	cameraLocationX = sin(rot*3.14159 / 180);
	cameraLocationY = -cos(rot*3.14159 / 180);

	glutPostRedisplay();
}

void  reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.005, 150.0);
	glViewport(0, 0, w, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: exit(0);
	default: view_mode = key - '0';
	}
	glutPostRedisplay();
}

void timer(int delay)
{
	if (racing)
	{
		update();
		glutTimerFunc(delay, timer, delay);
	}
}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		racing = true;
		glutTimerFunc(40, timer, 40);
		break;
	case GLUT_KEY_DOWN:
		reset();
		racing = false;
		break;
	case GLUT_KEY_LEFT:
		rot += 4;
		break;
	case GLUT_KEY_RIGHT:
		rot -= 4;
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(700, 700);
	glutCreateWindow("Racing Game");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	init();

	glutMainLoop();
	return 0;
}

