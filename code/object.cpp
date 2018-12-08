/*  =================== File Information =================
	File Name: object.h
	Description:
	Author: Michael Shah
	Last Modified: 3/10/14

	Purpose: 
	Usage:	

	Further Reading resources: 
	===================================================== */
#include <GL/glui.h>
#include <iostream>
#include <cmath>
#include "object.h"
#include "Algebra.h"
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923

/*	===============================================
Desc:
Precondition: 
Postcondition:
=============================================== */ 
object::object(){
	baseTexture = NULL;
	blendTexture = NULL;
	objectTexture = NULL;
}

/*	===============================================
Desc:
Precondition: 
Postcondition:
=============================================== */ 
object::~object(){
	if(baseTexture != NULL) {
		delete baseTexture;
	}
	if(blendTexture != NULL) {
		delete blendTexture;
	}
	if (objectTexture != NULL) {
		delete objectTexture;
	}
}

void object::drawFilledObject() {
	glEnable(GL_POLYGON_OFFSET_FILL);
	glColor3f(1.0, 1.0, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, blendTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	objectTexture->render();

	glDisable(GL_TEXTURE_2D);
}

void object::drawWiredObject() {
	objectTexture->renderWireFrame();
}

void object::moveObject(float theta, float phi) {
	objectTexture->move(theta, phi);
}

void object::rotateObject(float theta, float phi) {
	objectTexture->rotate(theta, phi);
}

void object::stretchObject(Point iSect, float theta, float phi) {
	objectTexture->stretch(iSect, theta, phi);
}

void object::resetAnimation(){
	cout << "Resetting animation..." << endl;
	ply* tmp = new ply(objectTexture->getfile());

	while (!objectTexture->animatReset(tmp)) {
		cout << "shifting" << endl;
	}
}

void object::reloadObject() {
	objectTexture->reload();
}

/*	===============================================
Desc:	This instantiates an image to be rendered.
		
		textureNumber 0 corresponds to the base texture
		textureNuber 1 corresponds to the next texture

		If texture number is less than 0, then default to 0
		If texture number is greater than 1, then default to 1
Precondition: 
Postcondition:
=============================================== */ 
void object::setTexture(int textureNumber,std::string _fileName){
	/*
		Algorithm
		if 0: load texture mapping
		if 1: load ply
	*/
	switch (textureNumber) {
		case 0:
			if (blendTexture == NULL) {
				blendTexture = new ppm(_fileName);
				blendTextureID = loadTexture(blendTexture->getWidth(), blendTexture->getHeight(), blendTexture->getPixels());
			}
			else {
				delete blendTexture;
				blendTexture = new ppm(_fileName);
				glBindTexture(GL_TEXTURE_2D, 0);
				blendTextureID = loadTexture(blendTexture->getWidth(), blendTexture->getHeight(), blendTexture->getPixels());
			}
			break;
		case 1:
			if (objectTexture != NULL) {
				delete objectTexture;
			}
			objectTexture = new ply(_fileName);
			break;
		default:
			exit(1);
	}

	// Repeat the process for the 2nd texture
}

/*	===============================================
Desc:	Loads a new array of colors of a specified dimension
		into our object.
Precondition: 
Postcondition:
=============================================== */
GLuint object::loadTexture(int width, int height, char* pixels){
	//std::cout << "loading texture" << std::endl;
	GLuint textureId;
		glGenTextures(1,&textureId);
		// Now we begin to edit the texture
	//std::cout << "binding texture" << std::endl;
		glBindTexture(GL_TEXTURE_2D, textureId);
		//  Now map the actual ppm image to the texture
	//std::cout << "store texture in memory " << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 
					  0,
					  GL_RGB,
					  width,
					  height,
					  0,
					  GL_RGB,
					  GL_UNSIGNED_BYTE,
					  pixels);
	// Now at this point we have generated some textures that can be used for this object
	// We do not need our ppms at this point, because they have been shipped off to the
	// GPU!

	return textureId;
}

/*	===============================================
Desc:
Precondition: 
Postcondition:
=============================================== */ 
void object::render(int option){
	switch (option) {
		case 0:
			drawWiredObject();
			break;
		case 1:
			drawFilledObject();
			break;
		default:
			exit(1);
	}
}

