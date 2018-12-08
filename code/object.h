/*  =================== File Information =================
	File Name: object.h
	Description:
	Author: Michael Shah
	Last Modified: 3/10/14

	Purpose: 
	Usage:	

	Further Reading resources: 
	===================================================== */
#ifndef OBJECT_H
#define OBJECT_H

#include "Algebra.h"
#include "ppm.h"
#include "ply.h"

/*
	This object renders a piece of geometry ('a sphere by default')
	that has one texture that can be drawn on.

*/
class object{
	public:
		/*	===============================================
		Desc:
		Precondition: 
		Postcondition:
		=============================================== */ 
		object();
		/*	===============================================
		Desc:
		Precondition: 
		Postcondition:
		=============================================== */ 
		~object();
		/*	===============================================
		Desc:
		Precondition: 
		Postcondition:
		=============================================== */
		GLuint loadTexture(int width, int height, char* pixels);
		/*	===============================================
		Desc:	This instantiates an image to be rendered.
				
				If memory has already been allocated for this,
				then our contract is to delete the previous image,
				and overwrite it with the new one.
		Precondition: 
		Postcondition:
		=============================================== */ 
		void setTexture(int textureNumber,std::string _fileName);
		/*	===============================================
		Desc:	Draw two planes for each of the ppms texture
		Precondition: 
		Postcondition:
		=============================================== */ 
		void render(int option);
		/*	===============================================
		Desc:	Draw the filled rendered ply
		Precondition: 
		Postcondition:
		=============================================== */ 
		void drawFilledObject();
		/*	===============================================
		Desc:	Draw the wired rendered object
		Precondition:
		Postcondition:
		=============================================== */
		void drawWiredObject();

		bool resetAnimation();
		void idleAnimation(float theta, float phi);
		void reloadObject();
		void moveObject(float theta, float phi);
		void rotateObject(float theta, float phi);
		void stretchObject(Point isectPoint, float x, float y);
		
	private:
		// The first texture image
		// This should be a white, black, pink, or other solid image that
		// we can draw on.
		ppm* baseTexture;
		// A second texture image that can be loaded
		// This is a second texture that we can blend onto our sphere
		// This is demonstrating multiple textures or 'multitexturing' as it
		// is called in the graphics world.
		ppm* blendTexture;

		//Ply object to map on to
		ply* objectTexture;

		// Create a texture id that uniquly identifies each texture image
		// I believe OpenGL will use these id's when we generate a texture
		// to uniquely identify which array of pixels to map to an image.
		GLuint baseTextureID;
		GLuint blendTextureID;
};

#endif
