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

/*	===============================================
Desc:	
Precondition: 
Postcondition:
=============================================== */ 
void object::paintTexture(float x, float y, char r, char g, char b){
	int width = blendTexture->getWidth();
	int height = blendTexture->getHeight();

	float tempX = x + 0.5;
	tempX = 0.25 + (tempX*0.5);
	//float u = 1 - (isect[0] + 0.75);
	float u = 1 - tempX;
	float v = 1 - (y + 0.5);
	//isect = rotY_mat(DEG_TO_RAD(90)) * isect;
	//Vector isectV(isect[0], isect[1], isect[2]);
	//isectV.normalize();

	//float u = 0.5 + atan2(isectV[2], isectV[0]) / (2.0f * PI);
	//float v = 0.5 + asin(isectV[1]) / (1.0f*PI);
	//float v = 1-(0.5 + isect[1]);

	//std::cout << "u: " << u << " v: " << v << std::endl;

	int drawX = u * width;
	int drawY = v * height;
	for (int xx = drawX - 5; xx < drawX + 5; xx++){
		for (int yy = drawY - 5; yy < drawY + 5; yy++){
			if ((xx > 0) && (xx < width) && (yy > 0) && (yy < height)) {
				blendTexture->setPixel(xx, yy, r, g, b);
			}
		}
	}
		glGenTextures(1,&blendTextureID);
		// Now we begin to edit the texture
	//std::cout << "binding texture" << std::endl;
		glBindTexture(GL_TEXTURE_2D, blendTextureID);
		//  Now map the actual ppm image to the texture
	//std::cout << "store texture in memory " << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 
					  0,
					  GL_RGB,
					  blendTexture->getWidth(),
					  blendTexture->getHeight(),
					  0,
					  GL_RGB,
					  GL_UNSIGNED_BYTE,
					  blendTexture->getPixels());

}

void object::paintTexture(Point& isect, char r, char g, char b) {
	int width = blendTexture->getWidth();
	int height = blendTexture->getHeight();

	float u = RAD_TO_DEG(atan2(isect[2], isect[0]));
	float v = RAD_TO_DEG(atan2(isect[2], isect[1]));

	//std::cout << "u: " << u << " v: " << v << std::endl;

	u = u / 180.0f;
	u = 1.0f - u;
	v = v / 180.0f;

	u = 0.25 + (u * 0.5);

	int drawX = u * width;
	int drawY = v * height;
	for (int xx = drawX - 5; xx < drawX + 5; xx++){
		for (int yy = drawY - 5; yy < drawY + 5; yy++){
			if ((xx > 0) && (xx < width) && (yy > 0) && (yy < height)) {
				blendTexture->setPixel(xx, yy, r, g, b);
			}
		}
	}
	glGenTextures(1, &blendTextureID);
	// Now we begin to edit the texture
	//std::cout << "binding texture" << std::endl;
	glBindTexture(GL_TEXTURE_2D, blendTextureID);
	//  Now map the actual ppm image to the texture
	//std::cout << "store texture in memory " << std::endl;
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		blendTexture->getWidth(),
		blendTexture->getHeight(),
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		blendTexture->getPixels());
}

void object::drawFilledObject() {
	objectTexture->render();
}

void object::drawWiredObject() {
	objectTexture->renderWireFrame();
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
		Step 1: Allocate memory for texture
		Step 2: Generate textures and bind them to object
	*/
	switch (textureNumber) {
		case 0:
			if (baseTexture == NULL) {
				baseTexture = new ppm(_fileName);
				baseTextureID = loadTexture(baseTexture->getWidth(), baseTexture->getHeight(), baseTexture->getPixels());
			}
			else {
				delete baseTexture;
				baseTexture = new ppm(_fileName);
				glBindTexture(GL_TEXTURE_2D, 0);
				baseTextureID = loadTexture(baseTexture->getWidth(), baseTexture->getHeight(), baseTexture->getPixels());
			}
			break;
		case 1:
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
		case 2:
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
void object::render(){
	// Enable the ability to draw 2D textures
	glEnable(GL_TEXTURE_2D);

	// Render the base texture for us to see how it changes
	glBindTexture(GL_TEXTURE_2D, baseTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glPushMatrix();
	glTranslatef(0.75f, 0.75f, -1.0f);
	glScalef(0.15, 0.15, 0.15);
	glBegin(GL_QUADS);    
	    glNormal3f(0.0, 1.0f, 0.0f);
	    glTexCoord2f(0.0f, 1.0f);
	    glVertex3f(-1.0f, -1.0f, 0.0f);
	    glTexCoord2f(1.0f, 1.0f);
	    glVertex3f(1.0f, -1.0f, 0.0f);
	    glTexCoord2f(1.0f, 0.0f);
	    glVertex3f(1.0f, 1.0f, 0.0f);
	    glTexCoord2f(0.0f, 0.0f);
	    glVertex3f(-1.0f, 1.0f, 0.0f);
    glEnd();
    glPopMatrix();

    // Render the blend texture for us to see
    glBindTexture(GL_TEXTURE_2D, blendTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glPushMatrix();
	glTranslatef(-0.75f, 0.75f, -1.0f);
	glScalef(0.15, 0.15, 0.15);
	glBegin(GL_QUADS);
		glNormal3f(0.0, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
	glEnd();
	glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}
