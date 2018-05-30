// third-party libraries
#include <glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// standard C++ libraries
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <list>

// tdogl classes
#include "tdogl/Program.h"
#include "tdogl/Texture.h"
#include "tdogl/Camera.h"

/*
 Represents a textured geometry asset

 contains everything necessary to draw arbitrary geometry with a single texture.
  - shaders
  - a VBO
  - a VAO
  - the parameters to glDrawArrays (drawType, drawStart, drawCount)
 */
struct ModelAsset {
	tdogl::Program	*shaders;
	tdogl::Texture	*texture;
	GLuint			vbo;
	GLuint			vao;
	GLenum			drawType;
	GLint			drawStart;
	GLint			drawCount;
	ModelAsset():
		shaders(nullptr),
		texture(nullptr),
		vbo(0),
		vao(0),
		drawType(GL_TRIANGLES),
		drawStart(0),
		drawCount(0)
	{

	}
};

/*
 Represents an instance of an 'ModelAsset'
 contains a pointer to the asset, and a model transformation matrix to be used
 when drawing.
 */
struct ModelInstance {
	ModelAsset	*asset;
	glm::mat4	transform;
	ModelInstance():
		asset(nullptr),
		transform()
	{

	}
};

/*
 Repersents a point light
*/
struct Light {
	glm::vec3 position;
	glm::vec3 intensities; //a.k.a. the color of the light
};



