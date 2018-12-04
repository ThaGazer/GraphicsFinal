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
int wireframe = 0;
int filled = 1;
int paint = 0;

int red_Scroll = 0;
int green_Scroll = 0;
int blue_Scroll = 0;

// Definition for rollout for r g b color sliders
#define COLORR_ID 101
#define COLORG_ID 102
#define COLORB_ID 103

float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float obj_pos[] = { 0.0, 0.0, 0.0 };

// function declarations
void drawRayFunc(int x, int y);

// Flag for OpenGL interaction
bool drawRay = false;

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
int mouseScreenX = 0;
int mouseScreenY = 0;

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
 * This function is called everytime the mouse moves
 * In order to get our ray to draw nicely, we update the mouse coordinates
 * where the ray is first cast, and then draw the ray, and then draw the rest
 * of our scene.
 */
void myGlutMotion(int x, int y ) {
	updateMouse(x,y);
	drawRayFunc(mouseX,mouseY);
    glutPostRedisplay();
}

/*	This function is called everytime the mouse is clicked
 *
 * left click - draw our ray
 * right click - color the pixel appropriately.
 *	
 */
void myGlutMouse(int button, int button_state, int x, int y ) {
	updateMouse(x,y);

    // Cast a ray to the screen
    // Setup a flag so we can see the actual ray that is being cast.
    if(button_state == GLUT_UP && button==GLUT_LEFT_BUTTON){
        drawRay = false;
    }
    if((button_state == GLUT_DOWN && button==GLUT_LEFT_BUTTON) && drawRay==false){
        drawRay = true;
    }
}

/*
 * paint object takes two floats, x and y, which go from 0 to 1.
 */
void paintObject(float x, float y) {
	myObject->paintTexture(x, y, red_Scroll, green_Scroll, blue_Scroll);
}

/*
 *
 */
void paintObject(Point& isect) {
	myObject->paintTexture(isect, red_Scroll, green_Scroll, blue_Scroll);
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
		std::cout << x << y << std::endl;
		Point eyePointP = getEyePoint(); 
		Vector rayV = generateRay(eyePointP);

		float t = Intersect(eyePointP, rayV, Matrix());
		std::cout << "t: " << t << std::endl;

		Point isectPointWorldCoord = getIsectPointWorldCoord(eyePointP, rayV, t);

		if (t > 0) {
			glColor3f(1, 0, 0);
			glutWireCube(1.0f);
			if (paint == 1) {
				paintObject(isectPointWorldCoord);
			}
			else{
				glColor3f(red_Scroll / (float)255.0f, green_Scroll / (float)255.0f, blue_Scroll / (float)255.0f);
				glPushMatrix();
					glTranslatef(isectPointWorldCoord[0], isectPointWorldCoord[1], isectPointWorldCoord[2]);
					glutSolidSphere(0.05f, 10, 10);
				glPopMatrix();
			}
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

	drawRayFunc(mouseX,mouseY);

	if (filled) {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glColor3f(1.0, 1.0, 1.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		myObject->render();
		glPushMatrix();
			glRotatef(90,0,1,0);
			myObject->drawTexturedSphere();
		glPopMatrix();
	}
	
	if (wireframe) {
		glDisable(GL_POLYGON_OFFSET_FILL);
		glColor3f(1.0, 1.0, 1.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		myObject->render();
		glPushMatrix();
			glRotatef(90, 0, 1, 0);
			myObject->drawTexturedSphere();
		glPopMatrix();
	}

	glutSwapBuffers();
}

/*
 * If you add any call backs for GLUI, you can put them here
 */
void control_cb( int control )
{

}

/*
 * Reclaim memory we've allocated
 */
void onExit()
{
	delete myObject;
}

/*
 * Main
 */
int main(int argc, char* argv[])
{

	atexit(onExit);

	//Initialize GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(windowXSize, windowYSize);

	main_window = glutCreateWindow("CSI 4341 In Class Assignment 7");
	glutDisplayFunc(myGlutDisplay);
	glutReshapeFunc(myGlutReshape);
	
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

	//Setup textured Objects
	myObject->setTexture(0,"./data/pink.ppm");
	myObject->setTexture(1,"./data/smile.ppm");

	//Here's the GLUI code
	GLUI *glui = GLUI_Master.create_glui("GLUI");

	GLUI_Scrollbar* sb1 = new GLUI_Scrollbar(glui, "Red", GLUI_SCROLL_HORIZONTAL, &red_Scroll, COLORR_ID, control_cb);
	sb1->set_int_limits(0, 255);
	GLUI_Scrollbar* sb2 = new GLUI_Scrollbar(glui, "Green", GLUI_SCROLL_HORIZONTAL, &green_Scroll, COLORG_ID, control_cb);
	sb2->set_int_limits(0, 255);
	GLUI_Scrollbar* sb3 = new GLUI_Scrollbar(glui, "Blue", GLUI_SCROLL_HORIZONTAL, &blue_Scroll, COLORB_ID, control_cb);
	sb3->set_int_limits(0, 255);
	sb1->set_int_val(255);
	sb2->set_int_val(255);

	glui->add_column(true);

	GLUI_Panel *render_panel = glui->add_panel("Render");
	new GLUI_Checkbox(render_panel, "Wireframe", &wireframe);
	new GLUI_Checkbox(render_panel, "Filled", &filled);
	new GLUI_Checkbox(render_panel, "Paint", &paint);

	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(main_window);
	/* We register the idle callback with GLUI, *not* with GLUT */
	GLUI_Master.set_glutIdleFunc(myGlutIdle);
	GLUI_Master.set_glutMouseFunc( myGlutMouse );
	glutMotionFunc( myGlutMotion );

	glutMainLoop();

	return EXIT_SUCCESS;
}



