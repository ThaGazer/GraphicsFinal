/*  =================== File Information =================
	File Name: ppm.h
	Description:
	Author: Michael Shah
	Last Modified: 3/10/14

	Purpose: 
	Usage:	

	Further Reading resources: 
	http://antongerdelan.net/opengl/raycasting.html
	===================================================== */
#include <string.h>
#include <iostream>
#include <fstream>
#include <freeglut.h>
#include <GL/glui.h>
#include <math.h>
#include "object.h"

/** These are the live variables passed into GLUI ***/
int main_window;

//Glui menu variables
int framing = 0;
int level = 3;

//default values for paints on objects. 

float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float obj_pos[] = { 0.0, 0.0, 0.0 };

// Flag for OpenGL interaction
bool drawRay = false;
bool objectMovement = false;
bool objectRotation = false;
bool objectStretch = false;
bool objectAniReload = false;
bool objectIdle = true;
Point isectPointWorldCoord;

// Window information
double windowXSize = 500;
double windowYSize = 500;

float camera_left = -1.0f;
float camera_right = 1.0f;
float camera_top = 1.0f;
float camera_bottom = -1.0f;
float camera_near = -1.0f;
float camera_far = 2.0f;

float mouseX = 0;
float mouseY = 0;
float mouseZ = 1.0f;
float mouseClickX = 0;
float mouseClickY = 0;
int mouseScreenX = 0;
int mouseScreenY = 0;

int shiftX = 0;
int shiftY = 0;

string textureFile_c = "./data/smile.ppm";
string objectFile_c = "./data/sphere.ply";
string objectFile_a = "./data/mario.ply";

using namespace std;

/*
 * We are going to create a single sphere that we load
 */
object* myObject = new object();

/* Updates the mouse cooridnates
 * This function turns a x and y screen cooridnate into a world coordinate
 * in object space
 */
void updateMouse(int x, int y) {
	mouseScreenX = x;
	mouseScreenY = y;

	mouseX = (2.0f * x)/windowXSize - 1.0f;
    mouseY = 1.0f - (2.0f * y)/ windowYSize;
    mouseZ = camera_near;
    //std::cout << "Screen (" << x << "," << y << ") to Object (" << mouseX << "," << mouseY << ")" << std::endl;
}

/*
 * Detects an inertsection between a ray and a point
 */
double Intersect(Point eyePointP, Vector rayV, Matrix transformMatrix) {
	Matrix inverTM = invert(transformMatrix);
	Point objPoint = inverTM * eyePointP;
	Vector objRay = inverTM * rayV;

	double t, A, B, C;
	A = dot(objRay, objRay);
	B = 2 * dot((objPoint - Point()), objRay);
	C = dot((objPoint - Point()), (objPoint - Point())) - .25;

	t = (B*B) - (4*A*C);

	if (t < 0) {
		return -1;
	}
	
	double posT = (-B + sqrt(t)) / (2 * A);
	double negT = (-B - sqrt(t)) / (2 * A);
		
	if (posT > 0 && negT > 0) {
		return std::min(posT, negT);
	}
	else if (posT > 0 && negT < 0) {
		return posT;
	}
	else if (negT > 0 && posT < 0) {
		return negT;
	}
	else {
		return -1;
	}
}

Vector generateRay(Point point) {
	Vector ray(point[0], point[1], -camera_far);
	ray.normalize();
	return ray;
}

Point getEyePoint() {
	return Point(mouseX, mouseY, -camera_near);
}

Point getIsectPointWorldCoord(Point eye, Vector ray, double t) {
	return eye + t * ray;
}

/*
 * This function you will have to modify
 */
void drawRayFunc(int x, int y){
	if(drawRay==true){
		// Draw a bounding box around the sphere to help debug your intersection
		Point eyePointP = getEyePoint(); 
		Vector rayV = generateRay(eyePointP);

		float t = Intersect(eyePointP, rayV, Matrix());
		std::cout << "t: " << t << std::endl;

		isectPointWorldCoord = getIsectPointWorldCoord(eyePointP, rayV, t);
		
		if (t > 0) { //mouse intersects with object
			objectStretch = true;
		}
		else { //mouse clicks outside object bounds
			objectMovement = true;
		}
		objectIdle = false;
	}
}

/*
 * Controls interactions with the object 
 * depending on where the mouse click
 */
void objectInteraction() {
	if (drawRay) {
		if (objectMovement) {
			if (!objectRotation) {
				float theta = (mouseScreenX - mouseClickX) / 5000;
				float phi = (mouseClickY - mouseScreenY) / 5000;

				myObject->moveObject(theta, phi);
			}
			else {
				float theta = (mouseScreenX - mouseClickX) / 5000;
				float phi = (mouseClickY - mouseScreenY) / 5000;

 				myObject->rotateObject(theta, phi);
			}
		}
		else if (objectStretch == true) {
			float theta = fmod((mouseScreenX - mouseClickX), 360) / 5000;
			float phi = fmod((mouseClickY - mouseScreenY), 360) / 5000;

			myObject->stretchObject(isectPointWorldCoord, theta, phi);
		}
		else {
			exit(1);
		}
	}
}

/*
 * Renders the scene
 */	
void myGlutDisplay(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	switch (framing) {
		case 0:
			if (objectAniReload) {
				if (!myObject->resetAnimation()) {
					objectAniReload = false;
				}
			}

			glDisable(GL_POLYGON_OFFSET_FILL);
			glColor3f(1.0, 1.0, 1.0);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			myObject->render(0);
			break;
		case 1:
			glEnable(GL_POLYGON_OFFSET_FILL);
			glColor3f(1.0, 1.0, 1.0);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glPushMatrix();
				myObject->render(1);
			glPopMatrix();
			break;
		default:
			exit(1);
	}

	glutSwapBuffers();
}

/*
 *
 */
void myGlutKeyBoard(unsigned char key, int x, int y) {
	switch (key) {
		case 'q':
			exit(0);
			break;
		case 'a':
			objectAniReload = true;
			break;
		case 'r':
			myObject->reloadObject();
			break;
		case 'f':
			objectRotation = !objectRotation;
			break;
		default:
			exit(1);
	}
	glutPostRedisplay();
}

/*
* This function is called everytime the mouse moves
* In order to get our ray to draw nicely, we update the mouse coordinates
* where the ray is first cast, and then draw the ray, and then draw the rest
* of our scene.
*/
void myGlutMotion(int x, int y) {
	updateMouse(x, y);
	objectInteraction();
	glutPostRedisplay();
}

/*	This function is called everytime the mouse is clicked
*
* left click - draw our ray
* right click - color the pixel appropriately.
*
*/
void myGlutMouse(int button, int button_state, int x, int y) {
	updateMouse(x, y);
	// Cast a ray to the screen
	// Setup a flag so we can see the actual ray that is being cast.
	if (button_state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
		drawRay = false;
		objectMovement = false;
		objectStretch = false;
	}
	if ((button_state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) && drawRay == false) {
		drawRay = true;
		mouseClickX = x;
		mouseClickY = y;
		drawRayFunc(x, y);
		cout << mouseClickX << " " << mouseClickY << endl;
	}
}

/*
* According to the GLUT specification, the current window is
* undefined during an idle callback.  So we need to explicitly change
* it if necessary
*/
void myGlutIdle(void) {
	if (glutGetWindow() != main_window)
		glutSetWindow(main_window);

	glutPostRedisplay();
}

/*
*When the window is resized, update aspect ratio to get correct viewing frustrum.
*/
void myGlutReshape(int x, int y) {
	glViewport(0, 0, x, y);

	windowXSize = x;
	windowXSize = y;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//using ortho projection for this assignment
	glOrtho(camera_left, camera_right, camera_bottom, camera_top, camera_near, camera_far);

	glutPostRedisplay();
}

/*
 * If you add any call backs for GLUI, you can put them here
 */
void control_cb( int control ) {
	switch (level) {
		case 0:
			//TODO add objects to face
			cout << "Loading A+ level project..." << endl;
			myObject->setTexture(0, "");
			myObject->setTexture(1, objectFile_a);
			myObject->render(0);
			break;
		case 1:
			//TODO enable A level features
			cout << "Loading A level project..." << endl;
			myObject->setTexture(0, "");
			myObject->setTexture(1, objectFile_a);
			myObject->render(0);
			break;
		case 2:
			cout << "Loading B level project..." << endl;
			myObject->setTexture(0, "");
			myObject->setTexture(1, objectFile_a);
			myObject->render(0);
			break;
		case 3:
			cout << "Loading C level project..." << endl;
			myObject->setTexture(0, textureFile_c);
			myObject->setTexture(1, objectFile_c);
			myObject->render(0);
			break;
		default:
			exit(1);
	}
}

/*
 * Reclaim memory we've allocated
 */
void onExit() {
	delete myObject;
}

/*
 * Main
 */
int main(int argc, char* argv[]) {

	atexit(onExit);

	//Initialize GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(windowXSize, windowYSize);

	main_window = glutCreateWindow("CSI 4341 Final Project");
	glutDisplayFunc(myGlutDisplay);
	glutKeyboardUpFunc(myGlutKeyBoard);
	glutReshapeFunc(myGlutReshape);
	
	//Setup textured Objects
	myObject->setTexture(0, textureFile_c);
	myObject->setTexture(1, objectFile_c);

	//Set up OpenGL lighting
	glShadeModel(GL_SMOOTH);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	static float one[] = { 1, 1, 1, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, one);
	
	//Enable z-buferring
	glEnable(GL_DEPTH_TEST);
	glPolygonOffset(1, 1);

	//Here's the GLUI code
	GLUI *glui = GLUI_Master.create_glui("GLUI");

	GLUI_Panel* panel = glui->add_panel("Object Renderer");
	GLUI_RadioGroup* framingGroup = glui->add_radiogroup_to_panel(panel, &framing, NULL, control_cb);
	glui->add_radiobutton_to_group(framingGroup, "Wireframe");
	glui->add_radiobutton_to_group(framingGroup, "Filled");

	glui->add_column_to_panel(panel, true);

	GLUI_RadioGroup* radioGroup = glui->add_radiogroup_to_panel(panel, &level, NULL, control_cb);
	glui->add_radiobutton_to_group(radioGroup, "Level A+");
	glui->add_radiobutton_to_group(radioGroup, "Level A");
	glui->add_radiobutton_to_group(radioGroup, "Level B");
	glui->add_radiobutton_to_group(radioGroup, "Level C");

	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(main_window);

	//We register the idle callback with GLUI, *not* with GLUT
	GLUI_Master.set_glutIdleFunc(myGlutIdle);
	GLUI_Master.set_glutMouseFunc( myGlutMouse );
	glutMotionFunc( myGlutMotion );

	glutMainLoop();

	return EXIT_SUCCESS;
}



