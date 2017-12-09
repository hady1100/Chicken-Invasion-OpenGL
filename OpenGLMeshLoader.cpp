#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>

int WIDTH = 1280;
int HEIGHT = 720;
float backgroundAnim = 0.0;
float spaceShipLocationX = 0.0;
float spaceShipLocationY = 0.0;
char title[] = "3D Model Loader Sample";
float chickenForward = 0.0;
bool first_person = false;
bool change = false;
// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio =  (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(15, 10, 45);
Vector At(15, 10, 0);
Vector Up(0, 1, 0);

int cameraZoom = 0;

//bullet obj and array
int cur_bullet = 0;

struct Bullet{
	float x, y, z;
	bool visible;
};

struct Chicken{
	float x, y, z;
	bool visible;
	Model_3DS model_chicken;
};

Bullet bullet[200];
Chicken chicken[30];

Bullet create_bullet(float x, float y, float z){
	Bullet b;
	b.x = x; b.y = y; b.z = z;
	b.visible = true;
	return b;
}

void hide_bullet(Bullet* b){
	(*b).visible = false;
}

void create_chicken(){
	int xTra = 3;
	int yTra = 3;
	for(int i = 0; i<30; i++){
		chicken[i].x= xTra;
		chicken[i].y= yTra;
		chicken[i].z= 3;
		chicken[i].visible = true;
		chicken[i].model_chicken.Load("Models/chicken/Chicken N161109.3DS"); 
		if(xTra< 27){
			xTra=xTra+3;
		} else{
			xTra=3;
			yTra=yTra+3;
		}
	}
}

void draw_chicken(){
	for(int i=0; i<30; i++){
		if(chicken[i].visible == true){
			glPushMatrix();
			glTranslated(chicken[i].x, chicken[i].y, chicken[i].z);
			glRotated(-80,0,1,0);
			glScaled(1.5,1.5,1.5);
			chicken[i].model_chicken.Draw();
			glPopMatrix();
		} else {
			chicken[i].model_chicken.visible =false;
			printf("%di \n", i);
		}
	}
}

void check_collision(){
	for(int i=0; i<30; i++){
		for(int j=0; j<200; j++){
			if(bullet[j].visible == false || chicken[i].visible == false)
				continue;
			if(chicken[i].z - 0.2 <= bullet[j].z && chicken[i].z + 0.2 >= bullet[j].z){
				bool condition = chicken[i].x+0.6>=bullet[j].x && chicken[i].x-0.6<=bullet[j].x 
					&& chicken[i].y+0.6>=bullet[j].y && chicken[i].y-0.6<=bullet[j].y;
				if(condition){
					chicken[i].visible=false;
					bullet[j].visible=false;
				}
			}
		}
	}
}

// Model Variables
Model_3DS model_ship;

// Textures
GLuint tex_space;
GLuint tex_space_left;
GLuint tex_space_right;
GLuint tex_space_top;
GLuint tex_space_bottom;

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //		
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

void drawWall(GLuint tex) {
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glScaled(20.0, 1, 20.0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0-backgroundAnim); glVertex3f(-0.5, 0, -0.5);
	glTexCoord2f(0, 1-backgroundAnim); glVertex3f(-0.5, 0, 0.5); 
	glTexCoord2f(1, 1-backgroundAnim); glVertex3f(0.5, 0, 0.5); 
	glTexCoord2f(1, 0-backgroundAnim); glVertex3f(0.5, 0, -0.5);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

void drawWallFace(GLuint tex) {
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glScaled(20.0, 1, 20.0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-0.5, 0, -0.5);
	glTexCoord2f(0, 1); glVertex3f(-0.5, 0, 0.5); 
	glTexCoord2f(1, 1); glVertex3f(0.5, 0, 0.5); 
	glTexCoord2f(1, 0); glVertex3f(0.5, 0, -0.5);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

void drawSpace(){
	//draw wall
	//face blue
	glPushMatrix();
	glTranslated(15.0, 10.0, 0.0);
	glScaled(1.5,1,1);
	glRotated(-90, 1.0, 0.0, 0.0);
	drawWallFace(tex_space);
	glPopMatrix();

	
	//left red
	glPushMatrix();
	glTranslated(0.0, 10.0, 10.0);
	glRotated(90, 0, 0, 1);
	glScaled(1, 1, 1.5);
	drawWall(tex_space_left);
	glPopMatrix();

	//right green
	glPushMatrix();
	glTranslated(30.0, 10.0, 10.0);
	glScaled(1, 1, 1.5);
	glRotated(90, 0, 0, 1);
	drawWall(tex_space_right);
	glPopMatrix();
	
	//bottom yellow
	glPushMatrix();
	glTranslated(15.0, 0, 10.0);
	glScaled(1.5, 1, 1.5);
	drawWall(tex_space_bottom);
	glPopMatrix();

	//top black
	glPushMatrix();
	glTranslated(15.0, 20.0, 10.0);
	glScaled(1.5, 1, 1.5);
	drawWall(tex_space_top);
	glPopMatrix();

}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = {0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	drawSpace();

	glPushMatrix();
	glTranslated(15+spaceShipLocationX,5+spaceShipLocationY,30);
	glScaled(0.05,0.05,0.05);
	model_ship.Draw();
	glPopMatrix();

	//handle  bullets
	for(int i =0; i < 200; i++){
		if(bullet[i].visible == true){
			bullet[i].z -=  0.2;
		glPushMatrix();
		glTranslated(bullet[i].x, bullet[i].y, bullet[i].z-0.7);
		//glTranslated(10, 10, 15);
		glutSolidSphere(0.15, 20, 20);
		glPopMatrix();
			if(bullet[i].z < -1)
				bullet[i].visible = false;
		}
	}

	//handle chicken
	glPushMatrix();
	glTranslated(0.0,0.0,chickenForward);
	draw_chicken();
	glPopMatrix();

	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void spaceShipNavigation(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		if(spaceShipLocationY < 10){
			spaceShipLocationY = spaceShipLocationY + 0.1;
		}
		break;
	case GLUT_KEY_DOWN:
		if(spaceShipLocationY > 0){
			spaceShipLocationY = spaceShipLocationY - 0.1;
		}
		break;
	case GLUT_KEY_LEFT:
		if(spaceShipLocationX > -10){
			spaceShipLocationX = spaceShipLocationX - 0.1;
		}
		break;
	case GLUT_KEY_RIGHT:
		if(spaceShipLocationX < 10){
			spaceShipLocationX = spaceShipLocationX + 0.1;
		}
		break;
	}

	glutPostRedisplay();
}

//=================================
// Keyboard function.
//=================================
int delay_shoot = 0;
void myKeyboard(unsigned char button, int x, int y)
{
	switch (button)
	{
	case 's':
		first_person = !first_person;
		break;
	case 'k':
		cur_bullet = cur_bullet % 200;
		bullet[cur_bullet] = create_bullet(15+spaceShipLocationX, 5+spaceShipLocationY, 30);
		cur_bullet++;
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_ship.Load("Models/ship/models/space craft.3DS");
	//model_chicken.Load("Models/chicken/Chicken N161109.3DS");
	create_chicken();
	loadBMP(&tex_space, "textures/ground.bmp", true);
	loadBMP(&tex_space_left, "textures/ground.bmp", true);
	loadBMP(&tex_space_right, "textures/ground.bmp", true);
	loadBMP(&tex_space_bottom, "textures/ground.bmp", true);
	loadBMP(&tex_space_top, "textures/ground.bmp", true);
}

void Timer(int value){
	backgroundAnim = backgroundAnim + 0.0020;
	chickenForward = chickenForward + 0.06;
	check_collision();
	if(first_person){
		Eye.x=15+spaceShipLocationX; 
		Eye.y=5+spaceShipLocationY;
		Eye.z=28.5;
		At.x=15+spaceShipLocationX;
		At.y=5+spaceShipLocationY;
		At.z=0;
	} else {
		Eye.x=15; 
		Eye.y=10;
		Eye.z=45;	
		At.x=15;
		At.y=10;
		At.z=0;
	}

	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	glutPostRedisplay();
	glutTimerFunc(50, Timer, 0);
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);
	glutTimerFunc(0, Timer, 0);
	
	glutSpecialFunc(spaceShipNavigation);
	glutKeyboardFunc(myKeyboard);
	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();
		glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}