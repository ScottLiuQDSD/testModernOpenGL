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
 Represents a point light
*/
struct Light {
	glm::vec3 position;
	glm::vec3 intensities; //a.k.a. the color of the light
};

// constants
const glm::vec2 SCREEN_SIZE6(800, 600);

// globals
GLFWwindow	*gWindow6 = nullptr;
double gScrollY6 = 0.0;
tdogl::Camera gCamera6;
ModelAsset gWoodenCrate6;
std::list<ModelInstance> gInstances6;
GLfloat gDegreesRotated6 = 0.0f;
Light gLight6;
std::string path6 = std::string("F:/Demo/TestVTKDemo/testModernOpenGL/");

// return a new tdogl::Program created from the given vertex and fragment shader filenames
static tdogl::Program* LoadShaders6(const char* vertFilename, const char* fragFilename)
{
	std::vector<tdogl::Shader> shaders;
	shaders.push_back(tdogl::Shader::shaderFromFile(path6 + vertFilename, GL_VERTEX_SHADER));
	shaders.push_back(tdogl::Shader::shaderFromFile(path6 + fragFilename, GL_FRAGMENT_SHADER));
	return new tdogl::Program(shaders);
}

// returns a new tdogl::Texture created from the given filename
static tdogl::Texture* LoadTexture6(const char* filename)
{
	tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(path6 + filename);
	bmp.flipVertically();
	return new tdogl::Texture(bmp);
}

// initialses the gWoodenCrate global
static void LoadWoodenCrateAsset6()
{
	gWoodenCrate6.shaders = LoadShaders6("vertexShaders.txt", "FragmentShaders.txt");
	gWoodenCrate6.drawType = GL_TRIANGLES;
	gWoodenCrate6.drawStart = 0;
	gWoodenCrate6.drawCount = 6 * 2 * 3;
	gWoodenCrate6.texture = LoadTexture6("wooden-crate.jpg");
	glGenBuffers(1, &gWoodenCrate6.vbo);
	glGenVertexArrays(1, &gWoodenCrate6.vao);

	//bind the vao
	glBindVertexArray(gWoodenCrate6.vao);

	//bind the vbo
	glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate6.vbo);

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
	glEnableVertexAttribArray(gWoodenCrate6.shaders->attrib("vert"));
	glVertexAttribPointer(gWoodenCrate6.shaders->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);

	// connect the uv coords to the "vertTexCoord" attribute of the vertex shader
	glEnableVertexAttribArray(gWoodenCrate6.shaders->attrib("vertTexCoord"));
	glVertexAttribPointer(gWoodenCrate6.shaders->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE, 8 * sizeof(GLfloat), (const GLvoid*) (3 * sizeof(GLfloat)));

	// connect the normal to the "vertNormal" attribute of the vertex shader
	glEnableVertexAttribArray(gWoodenCrate6.shaders->attrib("vertNormal"));
	glVertexAttribPointer(gWoodenCrate6.shaders->attrib("vertNormal"), 3, GL_FLOAT, GL_TRUE, 8 * sizeof(GLfloat), (const GLvoid*) (5 * sizeof(GLfloat)));

	// unbind the vao
	glBindVertexArray(0);
}

// convenience function that returns a translation matrix
glm::mat4 translate6(GLfloat x, GLfloat y, GLfloat z)
{
	return glm::translate(glm::mat4(), glm::vec3(x, y, z));
}


// convenience function that returns a scaling matrix
glm::mat4 scale6(GLfloat x, GLfloat y, GLfloat z)
{
	return glm::scale(glm::mat4(), glm::vec3(x, y, z));
}

static void CreateInstances6()
{
	ModelInstance dot;
	dot.asset = &gWoodenCrate6;
	dot.transform = glm::mat4();
	gInstances6.push_back(dot);

	ModelInstance i;
	i.asset = &gWoodenCrate6;
	i.transform = translate6(0, -4, 0) * scale6(1, 2, 1);
	gInstances6.push_back(i);

	ModelInstance hLeft;
	hLeft.asset = &gWoodenCrate6;
	hLeft.transform = translate6(-8, 0, 0) * scale6(1, 6, 1);
	gInstances6.push_back(hLeft);

	ModelInstance hRight;
	hRight.asset = &gWoodenCrate6;
	hRight.transform = translate6(-4, 0, 0) * scale6(1, 6, 1);
	gInstances6.push_back(hRight);

	ModelInstance hMid;
	hMid.asset = &gWoodenCrate6;
	hMid.transform = translate6(-6, 0, 0) * scale6(2, 1, 0.8f);
	gInstances6.push_back(hMid);
}

// renders a single 'ModelInstance'
static void RenderInstance6(const ModelInstance& inst)
{
	ModelAsset *asset = inst.asset;
	tdogl::Program *shader = asset->shaders;

	//bind the shaders
	shader->use();

	//set the shader uniforms
	shader->setUniform("camera", gCamera6.matrix());
	shader->setUniform("model", inst.transform);
	shader->setUniform("tex", 0);	// set to 0 because the texture will be bound to GL_TEXTURE0
	shader->setUniform("light.position", gLight6.position);
	shader->setUniform("light.intensities", gLight6.intensities);

	//bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, asset->texture->object());

	//bind vao and draw
	glBindVertexArray(asset->vao);
	glDrawArrays(asset->drawType, asset->drawStart, asset->drawCount);

	//unbind everything
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	shader->stopUsing();
}

//draw a single frame
static void Render6()
{
	// clear everything
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// render all the instances
	std::list<ModelInstance>::const_iterator iter;
	for (iter = gInstances6.begin(); iter != gInstances6.end(); ++iter) {
		RenderInstance6(*iter);
	}

	//swap the display buffers (displays what was just drawn)
	glfwSwapBuffers(gWindow6);
}

// update the scene based on the time elapsed since last update
static void Update6(float secondsElapsed)
{
	//rotate the first instance in `gInstances`
	const GLfloat degreesPerSecond = 180.0f;
	gDegreesRotated6 += secondsElapsed * degreesPerSecond;
	while (gDegreesRotated6 > 360.0f)
		gDegreesRotated6 -= 360.0f;
	gInstances6.front().transform = glm::rotate(glm::mat4(), glm::radians(gDegreesRotated6), glm::vec3(0, 1, 0));

	//move position of camera based on WASD keys, and XZ keys for up and down
	const float moveSpeed = 4.0; //units per second
	if (glfwGetKey(gWindow6, 'S')) {
		gCamera6.offsetPosition(secondsElapsed * moveSpeed * -gCamera6.forward());
	} else if (glfwGetKey(gWindow6, 'W')) {
		gCamera6.offsetPosition(secondsElapsed * moveSpeed * gCamera6.forward());
	}
	if (glfwGetKey(gWindow6, 'A')) {
		gCamera6.offsetPosition(secondsElapsed * moveSpeed * -gCamera6.right());
	} else if (glfwGetKey(gWindow6, 'D')) {
		gCamera6.offsetPosition(secondsElapsed * moveSpeed * gCamera6.right());
	}
	if (glfwGetKey(gWindow6, 'Z')) {
		gCamera6.offsetPosition(secondsElapsed * moveSpeed * -glm::vec3(0, 1, 0));
	} else if (glfwGetKey(gWindow6, 'X')) {
		gCamera6.offsetPosition(secondsElapsed * moveSpeed * glm::vec3(0, 1, 0));
	}

	//move light
	if (glfwGetKey(gWindow6, '1'))
		gLight6.position = gCamera6.position();

	// change light color
	if (glfwGetKey(gWindow6, '2'))
		gLight6.intensities = glm::vec3(1, 0, 0); //red
	else if (glfwGetKey(gWindow6, '3'))
		gLight6.intensities = glm::vec3(0, 1, 0); //green
	else if (glfwGetKey(gWindow6, '4'))
		gLight6.intensities = glm::vec3(1, 1, 1); //white


	//rotate camera based on mouse movement
	const float mouseSensitivity = 0.1f;
	double mouseX, mouseY;
	glfwGetCursorPos(gWindow6, &mouseX, &mouseY);
	gCamera6.offsetOrientation(mouseSensitivity * (float) mouseY, mouseSensitivity * (float) mouseX);
	glfwSetCursorPos(gWindow6, 0, 0); //reset the mouse, so it doesn't go out of the window

	//increase or decrease field of view based on mouse wheel
	const float zoomSensitivity = -0.2f;
	float fieldOfView = gCamera6.fieldOfView() + zoomSensitivity * (float) gScrollY6;
	if (fieldOfView < 5.0f) fieldOfView = 5.0f;
	if (fieldOfView > 130.0f) fieldOfView = 130.0f;
	gCamera6.setFieldOfView(fieldOfView);
	gScrollY6 = 0;
}

// records how far the y axis has been scrolled
void OnScroll6(GLFWwindow* window, double deltaX, double deltaY)
{
	gScrollY6 += deltaY;
}

void OnError6(int errorCode, const char* msg)
{
	throw std::runtime_error(msg);
}

// the program starts here
void AppMain_6()
{
	// initialize GLFW
	glfwSetErrorCallback(OnError6);
	if (!glfwInit())
		throw std::runtime_error("glfwInit failed");

	// open a window with GLFW
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	gWindow6 = glfwCreateWindow((int) SCREEN_SIZE6.x, (int) SCREEN_SIZE6.y, "OpenGL Tutorial", NULL, NULL);
	if (!gWindow6)
		throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");

	// GLFW settings
	glfwSetInputMode(gWindow6, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(gWindow6, 0, 0);
	glfwSetScrollCallback(gWindow6, OnScroll6);
	glfwMakeContextCurrent(gWindow6);

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
	LoadWoodenCrateAsset6();

	// create all the instances in the 3D scene based on the gWoodenCrate asset
	CreateInstances6();

	// setup gCamera
	gCamera6.setPosition(glm::vec3(-4, 0, 17));
	gCamera6.setViewportAspectRatio(SCREEN_SIZE6.x / SCREEN_SIZE6.y);
	gCamera6.setNearAndFarPlanes(0.5f, 100.0f);

	// setup gLight
	gLight6.position = gCamera6.position();
	gLight6.intensities = glm::vec3(1, 1, 1); //white

	// run while the window is open
	float lastTime = (float) glfwGetTime();
	while (!glfwWindowShouldClose(gWindow6)) {
		// process pending events
		glfwPollEvents();

		// update the scene based on the time elapsed since last update
		float thisTime = (float) glfwGetTime();
		Update6(thisTime - lastTime);
		lastTime = thisTime;

		// draw one frame
		Render6();

		// check for errors
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
			std::cerr << "OpenGL Error " << error << std::endl;

		//exit program if escape key is pressed
		if (glfwGetKey(gWindow6, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(gWindow6, GL_TRUE);
	}

	// clean up and exit
	glfwTerminate();
}






