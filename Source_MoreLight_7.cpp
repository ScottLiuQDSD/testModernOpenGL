// third-party libraries
#include <GL/glew.h>
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
	GLfloat			shininess;
	glm::vec3		specularColor;

	ModelAsset() :
		shaders(nullptr),
		texture(nullptr),
		vbo(0),
		vao(0),
		drawType(GL_TRIANGLES),
		drawStart(0),
		drawCount(0),
		shininess(0.0f),
		specularColor(1.0f, 1.0f, 1.0f)
	{ }
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
	{ }
};

/*
 Represents a point light
*/
struct Light {
	glm::vec3	position;
	glm::vec3	intensities; //a.k.a. the color of the light
	float		attenuation;
	float		ambientCoefficient;
};

// constants
const glm::vec2 SCREEN_SIZE7(800, 600);

// globals
GLFWwindow	*gWindow7 = nullptr;
double gScrollY7 = 0.0;
tdogl::Camera gCamera7;
ModelAsset gWoodenCrate7;
std::list<ModelInstance> gInstances7;
GLfloat gDegreesRotated7 = 0.0f;
Light gLight7;
std::string path7 = std::string("F:/Demo/TestVTKDemo/testModernOpenGL/");

// return a new tdogl::Program created from the given vertex and fragment shader filenames
static tdogl::Program* LoadShaders7(const char* vertFilename, const char* fragFilename)
{
	std::vector<tdogl::Shader> shaders;
	shaders.push_back(tdogl::Shader::shaderFromFile(path7 + vertFilename, GL_VERTEX_SHADER));
	shaders.push_back(tdogl::Shader::shaderFromFile(path7 + fragFilename, GL_FRAGMENT_SHADER));
	return new tdogl::Program(shaders);
}

// returns a new tdogl::Texture created from the given filename
static tdogl::Texture* LoadTexture7(const char* filename)
{
	tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(path7 + filename);
	bmp.flipVertically();
	return new tdogl::Texture(bmp);
}


// initialses the gWoodenCrate global
static void LoadWoodenCrateAsset7()
{
	gWoodenCrate7.shaders = LoadShaders7("vertexShaders.txt", "FragmentShaders.txt");
	gWoodenCrate7.drawType = GL_TRIANGLES;
	gWoodenCrate7.drawStart = 0;
	gWoodenCrate7.drawCount = 6 * 2 * 3;
	gWoodenCrate7.texture = LoadTexture7("wooden-crate.jpg");
	gWoodenCrate7.shininess = 80.0;
	gWoodenCrate7.specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glGenBuffers(1, &gWoodenCrate7.vbo);
	glGenVertexArrays(1, &gWoodenCrate7.vao);

	//bind the vao
	glBindVertexArray(gWoodenCrate7.vao);

	//bind the vbo
	glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate7.vbo);

	//make a cube out of triangles (two triangles per side)
	GLfloat vertexData[] = {
		//  X     Y     Z       U     V          Normal
		// bottom
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

		// top
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		// front
		-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		// back
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,

		// left
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		// right
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// connect the xyz to the "vert" attribute of the vertex shader.
	glEnableVertexAttribArray(gWoodenCrate7.shaders->attrib("vert"));
	glVertexAttribPointer(gWoodenCrate7.shaders->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);

	// connect the uv coords to the "vertTexCoord" attribute of the vertex shader
	glEnableVertexAttribArray(gWoodenCrate7.shaders->attrib("vertTexCoord"));
	glVertexAttribPointer(gWoodenCrate7.shaders->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE, 8 * sizeof(GLfloat), (const GLvoid*) (3 * sizeof(GLfloat)));

	// connect the normal to the "vertNormal" attribute of the vertex shader
	glEnableVertexAttribArray(gWoodenCrate7.shaders->attrib("vertNormal"));
	glVertexAttribPointer(gWoodenCrate7.shaders->attrib("vertNormal"), 3, GL_FLOAT, GL_TRUE, 8 * sizeof(GLfloat), (const GLvoid*) (5 * sizeof(GLfloat)));

	// unbind the vao
	glBindVertexArray(0);
}

// convenience function that returns a translation matrix
glm::mat4 translate7(GLfloat x, GLfloat y, GLfloat z)
{
	return glm::translate(glm::mat4(), glm::vec3(x, y, z));
}


// convenience function that returns a scaling matrix
glm::mat4 scale7(GLfloat x, GLfloat y, GLfloat z)
{
	return glm::scale(glm::mat4(), glm::vec3(x, y, z));
}

static void CreateInstances7()
{
	ModelInstance dot;
	dot.asset = &gWoodenCrate7;
	dot.transform = glm::mat4();
	gInstances7.push_back(dot);

	ModelInstance i;
	i.asset = &gWoodenCrate7;
	i.transform = translate7(0, -4, 0) * scale7(1, 2, 1);
	gInstances7.push_back(i);

	ModelInstance hLeft;
	hLeft.asset = &gWoodenCrate7;
	hLeft.transform = translate7(-8, 0, 0) * scale7(1, 6, 1);
	gInstances7.push_back(hLeft);

	ModelInstance hRight;
	hRight.asset = &gWoodenCrate7;
	hRight.transform = translate7(-4, 0, 0) * scale7(1, 6, 1);
	gInstances7.push_back(hRight);

	ModelInstance hMid;
	hMid.asset = &gWoodenCrate7;
	hMid.transform = translate7(-6, 0, 0) * scale7(2, 1, 0.8f);
	gInstances7.push_back(hMid);
}

// renders a single 'ModelInstance'
static void RenderInstance7(const ModelInstance& inst)
{
	ModelAsset *asset = inst.asset;
	tdogl::Program *shaders = asset->shaders;

	//bind the shaders
	shaders->use();

	//set the shader uniforms
	shaders->setUniform("camera", gCamera7.matrix());
	shaders->setUniform("model", inst.transform);
	shaders->setUniform("materialTex", 0); //set to 0 because the texture will be bound to GL_TEXTURE0
	shaders->setUniform("materialShininess", asset->shininess);
	shaders->setUniform("materialSpecularColor", asset->specularColor);
	shaders->setUniform("light.position", gLight7.position);
	shaders->setUniform("light.intensities", gLight7.intensities);
	shaders->setUniform("light.attenuation", gLight7.attenuation);
	shaders->setUniform("light.ambientCoefficient", gLight7.ambientCoefficient);
	shaders->setUniform("cameraPosition", gCamera7.position());


	//bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, asset->texture->object());

	//bind vao and draw
	glBindVertexArray(asset->vao);
	glDrawArrays(asset->drawType, asset->drawStart, asset->drawCount);

	//unbind everything
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	shaders->stopUsing();
}

//draw a single frame
static void Render7()
{
	// clear everything
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// render all the instances
	std::list<ModelInstance>::const_iterator iter;
	for (iter = gInstances7.begin(); iter != gInstances7.end(); ++iter) {
		RenderInstance7(*iter);
	}

	//swap the display buffers (displays what was just drawn)
	glfwSwapBuffers(gWindow7);
}

// update the scene based on the time elapsed since last update
static void Update7(float secondsElapsed)
{
	//rotate the first instance in `gInstances`
	const GLfloat degreesPerSecond = 180.0f;
	gDegreesRotated7 += secondsElapsed * degreesPerSecond;
	while (gDegreesRotated7 > 360.0f)
		gDegreesRotated7 -= 360.0f;
	gInstances7.front().transform = glm::rotate(glm::mat4(), glm::radians(gDegreesRotated7), glm::vec3(0, 1, 0));

	//move position of camera based on WASD keys, and XZ keys for up and down
	const float moveSpeed = 4.0; //units per second
	if (glfwGetKey(gWindow7, 'S')) {
		gCamera7.offsetPosition(secondsElapsed * moveSpeed * -gCamera7.forward());
	} else if (glfwGetKey(gWindow7, 'W')) {
		gCamera7.offsetPosition(secondsElapsed * moveSpeed * gCamera7.forward());
	}
	if (glfwGetKey(gWindow7, 'A')) {
		gCamera7.offsetPosition(secondsElapsed * moveSpeed * -gCamera7.right());
	} else if (glfwGetKey(gWindow7, 'D')) {
		gCamera7.offsetPosition(secondsElapsed * moveSpeed * gCamera7.right());
	}
	if (glfwGetKey(gWindow7, 'Z')) {
		gCamera7.offsetPosition(secondsElapsed * moveSpeed * -glm::vec3(0, 1, 0));
	} else if (glfwGetKey(gWindow7, 'X')) {
		gCamera7.offsetPosition(secondsElapsed * moveSpeed * glm::vec3(0, 1, 0));
	}

	//move light
	if (glfwGetKey(gWindow7, '1'))
		gLight7.position = gCamera7.position();
	else if (glfwGetKey(gWindow7, '2'))
		gLight7.position = glm::vec3(-4, 0, 4);

	// change light color
	if (glfwGetKey(gWindow7, '3'))
		gLight7.intensities = glm::vec3(1, 0, 0); //red
	else if (glfwGetKey(gWindow7, '4'))
		gLight7.intensities = glm::vec3(0, 1, 0); //green
	else if (glfwGetKey(gWindow7, '5'))
		gLight7.intensities = glm::vec3(1, 1, 1); //white


	//rotate camera based on mouse movement
	const float mouseSensitivity = 0.1f;
	double mouseX, mouseY;
	glfwGetCursorPos(gWindow7, &mouseX, &mouseY);
	gCamera7.offsetOrientation(mouseSensitivity * (float) mouseY, mouseSensitivity * (float) mouseX);
	glfwSetCursorPos(gWindow7, 0, 0); //reset the mouse, so it doesn't go out of the window

	//increase or decrease field of view based on mouse wheel
	const float zoomSensitivity = -0.2f;
	float fieldOfView = gCamera7.fieldOfView() + zoomSensitivity * (float) gScrollY7;
	if (fieldOfView < 5.0f) fieldOfView = 5.0f;
	if (fieldOfView > 130.0f) fieldOfView = 130.0f;
	gCamera7.setFieldOfView(fieldOfView);
	gScrollY7 = 0;
}

// records how far the y axis has been scrolled
void OnScroll7(GLFWwindow* window, double deltaX, double deltaY)
{
	gScrollY7 += deltaY;
}

void OnError7(int errorCode, const char* msg)
{
	throw std::runtime_error(msg);
}

// the program starts here
void AppMain_7()
{
	// initialize GLFW
	glfwSetErrorCallback(OnError7);
	if (!glfwInit())
		throw std::runtime_error("glfwInit failed");

	// open a window with GLFW
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	gWindow7 = glfwCreateWindow((int) SCREEN_SIZE7.x, (int) SCREEN_SIZE7.y, "OpenGL Tutorial", NULL, NULL);
	if (!gWindow7)
		throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");

	// GLFW settings
	glfwSetInputMode(gWindow7, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(gWindow7, 0, 0);
	glfwSetScrollCallback(gWindow7, OnScroll7);
	glfwMakeContextCurrent(gWindow7);

	// initialize GLEW
	glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
	if (glewInit() != GLEW_OK)
		throw std::runtime_error("glewInit failed");

	// GLEW throws some errors, so discard all the errors so far
	while (glGetError() != GL_NO_ERROR) {}

	// print out some info about the graphics drivers
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

	// make sure OpenGL version 3.2 API is available
	if (!GLEW_VERSION_3_2)
		throw std::runtime_error("OpenGL 3.2 API is not available.");

	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// initialize the gWoodenCrate asset
	LoadWoodenCrateAsset7();

	// create all the instances in the 3D scene based on the gWoodenCrate asset
	CreateInstances7();

	// setup gCamera
	gCamera7.setPosition(glm::vec3(-4, 0, 17));
	gCamera7.setViewportAspectRatio(SCREEN_SIZE7.x / SCREEN_SIZE7.y);
	gCamera7.setNearAndFarPlanes(0.5f, 100.0f);

	// setup gLight
	gLight7.position = glm::vec3(-4, 0, 4);
	gLight7.intensities = glm::vec3(1, 1, 1); //white
	gLight7.attenuation = 0.2f;
	gLight7.ambientCoefficient = 0.005f;

	// run while the window is open
	float lastTime = (float) glfwGetTime();
	while (!glfwWindowShouldClose(gWindow7)) {
		// process pending events
		glfwPollEvents();

		// update the scene based on the time elapsed since last update
		float thisTime = (float) glfwGetTime();
		Update7(thisTime - lastTime);
		lastTime = thisTime;

		// draw one frame
		Render7();

		// check for errors
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
			std::cerr << "OpenGL Error " << error << std::endl;

		//exit program if escape key is pressed
		if (glfwGetKey(gWindow7, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(gWindow7, GL_TRUE);
	}

	// clean up and exit
	glfwTerminate();
}






