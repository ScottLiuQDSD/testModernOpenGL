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

struct ModelAsset {
	tdogl::Program* shaders;
	tdogl::Texture* texture;
	GLuint	vbo;
	GLuint	vao;
	GLenum	drawType;
	GLint	drawStart;
	GLint	drawCount;
	ModelAsset() :
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

struct ModelInstance {
	ModelAsset	*asset;
	glm::mat4	transform;

	ModelInstance() :
		asset(nullptr),
		transform()
	{

	}
};

const glm::vec2 SCREEN_SIZE4(800, 600);

GLFWwindow	*gWindow4 = nullptr;
double		gScrollY4 = 0.0;
tdogl::Camera	gCamera4;
ModelAsset		gWoodenCrate4;
std::list<ModelInstance> gInstances4;
GLfloat			gDegreesRotated4 = 0.0f;
std::string path = std::string("F:/Demo/TestVTKDemo/testModernOpenGL/");

static tdogl::Program* LoadShaders(const char* vertFileName, const char* fragFileName)
{
	std::vector<tdogl::Shader> shaders;
	shaders.push_back(tdogl::Shader::shaderFromFile(path + vertFileName, GL_VERTEX_SHADER));
	shaders.push_back(tdogl::Shader::shaderFromFile(path + fragFileName, GL_FRAGMENT_SHADER));
	return new tdogl::Program(shaders);
}

static tdogl::Texture* LoadTexture(const char* filename)
{
	tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(path + filename);
	bmp.flipVertically();
	return new tdogl::Texture(bmp);
}

static void LoadWoodenCrateAsset()
{
	gWoodenCrate4.shaders = LoadShaders("vertexShaders.txt", "FragmentShaders.txt");
	gWoodenCrate4.drawType = GL_TRIANGLES;
	gWoodenCrate4.drawStart = 0;
	gWoodenCrate4.drawCount = 6*2*3;
	gWoodenCrate4.texture = LoadTexture("wooden-crate.jpg");
	glGenBuffers(1, &gWoodenCrate4.vbo);
	glGenVertexArrays(1, &gWoodenCrate4.vao);

	glBindVertexArray(gWoodenCrate4.vao);

	glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate4.vbo);

	GLfloat vertexData[] = {
		//  X     Y     Z       U     V
		// bottom
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f,

		// top
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		// front
		-1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		// back
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 1.0f,

		// left
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 0.0f,

		// right
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(gWoodenCrate4.shaders->attrib("vert"));
	glVertexAttribPointer(gWoodenCrate4.shaders->attrib("vert"), 
						  3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);

	glEnableVertexAttribArray(gWoodenCrate4.shaders->attrib("vertTexCoord"));
	glVertexAttribPointer(gWoodenCrate4.shaders->attrib("vertTexCoord"), 
						  2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid*) (3 * sizeof(GLfloat)));

	glBindVertexArray(0);

}

// convenience function that returns a translation matrix
glm::mat4 translate(GLfloat x, GLfloat y, GLfloat z)
{
	return glm::translate(glm::mat4(), glm::vec3(x, y, z));
}


// convenience function that returns a scaling matrix
glm::mat4 scale(GLfloat x, GLfloat y, GLfloat z)
{
	return glm::scale(glm::mat4(), glm::vec3(x, y, z));
}

static void CreateInstances()
{
	ModelInstance dot;
	dot.asset = &gWoodenCrate4;
	dot.transform = glm::mat4();
	gInstances4.push_back(dot);
	
	ModelInstance i;
	i.asset = &gWoodenCrate4;
	i.transform = translate(0, -4, 0)* scale(1, 2, 1);
	gInstances4.push_back(i);

	ModelInstance hLeft;
	hLeft.asset = &gWoodenCrate4;
	hLeft.transform = translate(-8, 0, 0)* scale(1, 6, 1);
	gInstances4.push_back(hLeft);

	ModelInstance hRight;
	hRight.asset = &gWoodenCrate4;
	hRight.transform = translate(-4, 0, 0)* scale(1, 6, 1);
	gInstances4.push_back(hRight);

	ModelInstance hMid;
	hMid.asset = &gWoodenCrate4;
	hMid.transform = translate(-6, 0, 0)* scale(2, 1, 0.8f);
	gInstances4.push_back(hMid);
}

static void RenderInstance(const ModelInstance& inst)
{
	ModelAsset* asset = inst.asset;
	tdogl::Program* shaders = asset->shaders;

	//bind the shaders
	shaders->use();

	//set the shader uniforms
	shaders->setUniform("camera", gCamera4.matrix());
	shaders->setUniform("model", inst.transform);
	shaders->setUniform("tex", 0); //set to 0 because the texture will be bound to GL_TEXTURE0

	//bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, asset->texture->object());

	//bind VAO and draw
	glBindVertexArray(asset->vao);
	glDrawArrays(asset->drawType, asset->drawStart, asset->drawCount);

	//unbind everything
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	shaders->stopUsing();
}

// draws a single frame
static void Render()
{
	// clear everything
	glClearColor(0, 0, 0, 1); // black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render all the instances
	std::list<ModelInstance>::const_iterator it;
	for (it = gInstances4.begin(); it != gInstances4.end(); ++it) {
		RenderInstance(*it);
	}

	// swap the display buffers (displays what was just drawn)
	glfwSwapBuffers(gWindow4);
}

// update the scene based on the time elapsed since last update
static void Update(float secondsElapsed)
{
	//rotate the first instance in 'gInstances'
	const GLfloat degreesPerSecond = 180.0f;
	gDegreesRotated4 += secondsElapsed *degreesPerSecond;
	while (360.0f < gDegreesRotated4)
	{
		gDegreesRotated4 -= 360.0f;
	}
	gInstances4.front().transform = glm::rotate(glm::mat4(), glm::radians(gDegreesRotated4),
											glm::vec3(0, 1, 0));

	//move position of camera based on WASD keys, and XZ keys for up and down
	const float moveSpeed = 2.0; //units per second
	if (glfwGetKey(gWindow4, 'S')) {
		gCamera4.offsetPosition(secondsElapsed * moveSpeed * -gCamera4.forward());
	} else if (glfwGetKey(gWindow4, 'W')) {
		gCamera4.offsetPosition(secondsElapsed * moveSpeed * gCamera4.forward());
	}
	if (glfwGetKey(gWindow4, 'A')) {
		gCamera4.offsetPosition(secondsElapsed * moveSpeed * -gCamera4.right());
	} else if (glfwGetKey(gWindow4, 'D')) {
		gCamera4.offsetPosition(secondsElapsed * moveSpeed * gCamera4.right());
	}
	if (glfwGetKey(gWindow4, 'Z')) {
		gCamera4.offsetPosition(secondsElapsed * moveSpeed * -glm::vec3(0, 1, 0));
	} else if (glfwGetKey(gWindow4, 'X')) {
		gCamera4.offsetPosition(secondsElapsed * moveSpeed * glm::vec3(0, 1, 0));
	}

	// rotate camera based on mouse movement
	const float mouseSensitivity = 0.1f;
	double mouseX, mouseY;
	glfwGetCursorPos(gWindow4, &mouseX, &mouseY);
	gCamera4.offsetOrientation(mouseSensitivity*(float) mouseY, mouseSensitivity*(float) mouseX);
	glfwSetCursorPos(gWindow4, 0, 0);

	//increase or decrease field of view based on mouse wheel
	const float zoomSensitivity = -0.2f;
	float fieldOfView = gCamera4.fieldOfView() + zoomSensitivity * (float) gScrollY4;
	if(fieldOfView < 5.0f) {
		fieldOfView = 5.0f;
	}
	if (fieldOfView > 130.0f) {
		fieldOfView = 130.0f;
	}
	gCamera4.setFieldOfView(fieldOfView);
	gScrollY4 = 0;
}

// records how far the y axis has been scrolled
void OnScroll4(GLFWwindow* window, double deltaX, double deltaY)
{
	gScrollY4 += deltaY;
}

void OnError4(int errorCode, const char* msg)
{
	throw std::runtime_error(msg);
}

// the program starts here
void AppMain_4()
{
	// initialise GLFW
	glfwSetErrorCallback(OnError4);
	if (!glfwInit())
		throw std::runtime_error("glfwInit failed");

	// open a window with GLFW
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	gWindow4 = glfwCreateWindow((int) SCREEN_SIZE4.x, (int) SCREEN_SIZE4.y, "OpenGL Tutorial", NULL, NULL);
	if (!gWindow4)
		throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");

	// GLFW settings
	glfwSetInputMode(gWindow4, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(gWindow4, 0, 0);
	glfwSetScrollCallback(gWindow4, OnScroll4);
	glfwMakeContextCurrent(gWindow4);

	// initialise GLEW
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

	// initialise the gWoodenCrate asset
	LoadWoodenCrateAsset();

	// create all the instances in the 3D scene based on the gWoodenCrate asset
	CreateInstances();

	// setup gCamera
	gCamera4.setPosition(glm::vec3(-4, 0, 17));
	gCamera4.setViewportAspectRatio(SCREEN_SIZE4.x / SCREEN_SIZE4.y);

	// run while the window is open
	double lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(gWindow4)) {
		// process pending events
		glfwPollEvents();

		// update the scene based on the time elapsed since last update
		double thisTime = glfwGetTime();
		Update((float) (thisTime - lastTime));
		lastTime = thisTime;

		// draw one frame
		Render();

		// check for errors
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
			std::cerr << "OpenGL Error " << error << std::endl;

		//exit program if escape key is pressed
		if (glfwGetKey(gWindow4, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(gWindow4, GL_TRUE);
	}

	// clean up and exit
	glfwTerminate();
}
