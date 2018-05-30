#include "glew.h"
#include "glm\gtc\type_ptr.hpp"
#include "GLFW\glfw3.h"
#include "tdogl\Texture.h"
#include "tdogl\Program.h"
#include "tdogl\Camera.h"
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

// constants
const glm::vec2 SCREEN_SIZE(800, 600);

// globals
GLFWwindow* gWindow3 = NULL;
tdogl::Texture* gTexture3 = NULL;
tdogl::Program* gProgram3 = NULL;
GLuint gVAO3 = 0.0f;
GLuint gVBO3 = 0.0f;
GLfloat gDegreesRotated3 = 0.0f;
tdogl::Camera gCamera;
double gScrollY = 0.0;


void OnScroll(GLFWwindow* window, double deltaX, double deltaY)
{
	gScrollY += deltaY;
}

void Update3(float secondsElapsed)
{
	const GLfloat degreePerSecond = 45.0f;

	gDegreesRotated3 += secondsElapsed * degreePerSecond;
	while (360.0f < gDegreesRotated3) {
		gDegreesRotated3 -= 360.0f;
	}

	const float moveSpeed = 2.0;
	if (glfwGetKey(gWindow3, 'S')) {
		gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.forward());
	} else if (glfwGetKey(gWindow3, 'W')) {
		gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.forward());
	}

	if (glfwGetKey(gWindow3, 'A')) {
		gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.right());
	} else if (glfwGetKey(gWindow3, 'D')) {
		gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.right());
	}

	if (glfwGetKey(gWindow3, 'Z')) {
		gCamera.offsetPosition(secondsElapsed * moveSpeed * -glm::vec3(0, 1, 0));
	} else if (glfwGetKey(gWindow3, 'X')) {
		gCamera.offsetPosition(secondsElapsed * moveSpeed * glm::vec3(0, 1, 0));
	}

	//rotate camera based on mouse movement
	const float mouseSensitivity = 0.1f;
	double mouseX, mouseY;
	glfwGetCursorPos(gWindow3, &mouseX, &mouseY);
	gCamera.offsetOrientation(mouseSensitivity * (float) mouseY, mouseSensitivity * (float) mouseX);
	glfwSetCursorPos(gWindow3, 0, 0);

	const float zoomSensitivity = -0.2f;
	float fieldOfView = gCamera.fieldOfView() + zoomSensitivity * (float) gScrollY;
	if (fieldOfView < 5.0f) fieldOfView = 5.0f;
	if (fieldOfView > 130.0f) fieldOfView = 130.0f;
	gCamera.setFieldOfView(fieldOfView);
	gScrollY = 0;

}
// loads the vertex shader and fragment shader, and links them to make the global gProgram3
static void LoadShaders()
{
	std::vector<tdogl::Shader> shaders;
	shaders.push_back(tdogl::Shader::shaderFromFile("F:/Demo/TestVTKDemo/testModernOpenGL/vertexShaders.txt", GL_VERTEX_SHADER));
	shaders.push_back(tdogl::Shader::shaderFromFile("F:/Demo/TestVTKDemo/testModernOpenGL/FragmentShaders.txt", GL_FRAGMENT_SHADER));
	gProgram3 = new tdogl::Program(shaders);

}


// loads a triangle into the VAO global
static void LoadTriangle()
{
	// make and bind the VAO
	glGenVertexArrays(1, &gVAO3);
	glBindVertexArray(gVAO3);

	// make and bind the VBO
	glGenBuffers(1, &gVBO3);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO3);

	// Put the six rectangle vertices (XYZ) and texture coordinates (UV) into the VBO
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

	// connect the xyz to the "vert" attribute of the vertex shader
	glEnableVertexAttribArray(gProgram3->attrib("vert"));
	glVertexAttribPointer(gProgram3->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);

	// connect the uv coords to the "vertTexCoord" attribute of the vertex shader
	glEnableVertexAttribArray(gProgram3->attrib("vertTexCoord"));
	glVertexAttribPointer(gProgram3->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE, 5 * sizeof(GLfloat), (const GLvoid*) (3 * sizeof(GLfloat)));

	// unbind the VAO
	glBindVertexArray(0);
}


// loads the file "hazard.png" into gTexture3
static void LoadTexture()
{
	tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile("F:/Demo/TestVTKDemo/testModernOpenGL/wooden-crate.jpg");
	bmp.flipVertically();
	gTexture3 = new tdogl::Texture(bmp);
}


// draws a single frame
static void Render()
{
	// clear everything
	glClearColor(0, 0, 0, 1); // black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind the program (the shaders)
	gProgram3->use();

	// set the "camera" uniform
	gProgram3->setUniform("camera", gCamera.matrix());

	// set the "model" uniform in the vertex shader, based on the gDegreesRotated3 global
	gProgram3->setUniform("model", glm::rotate(glm::mat4(), glm::radians(gDegreesRotated3), glm::vec3(0, 1, 0)));

	// bind the texture and set the "tex" uniform in the fragment shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexture3->object());
	gProgram3->setUniform("tex", 0); //set to 0 because the texture is bound to GL_TEXTURE0

	// bind the VAO (the triangle)
	glBindVertexArray(gVAO3);

	// draw the VAO
	glDrawArrays(GL_TRIANGLES, 0, 6 * 2 * 3);

	// unbind the VAO, the program and the texture
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	gProgram3->stopUsing();

	// swap the display buffers (displays what was just drawn)
	glfwSwapBuffers(gWindow3);
}

void OnError_3(int errorCode, const char* msg)
{
	throw std::runtime_error(msg);
}

// the program starts here
void AppMain_3()
{
	// initialise GLFW
	glfwSetErrorCallback(OnError_3);
	if (!glfwInit())
		throw std::runtime_error("glfwInit failed");


	// open a window with GLFW
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	gWindow3 = glfwCreateWindow((int) SCREEN_SIZE.x, (int) SCREEN_SIZE.y, "OpenGL Tutorial", NULL, NULL);
	if (!gWindow3)
		throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");

	// GLFW settings
	glfwMakeContextCurrent(gWindow3);
	glfwSetInputMode(gWindow3, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(gWindow3, OnScroll);
	glfwSetCursorPos(gWindow3, 0, 0);

	// initialise GLEW
	glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
	if (glewInit() != GLEW_OK)
		throw std::runtime_error("glewInit failed");

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

	// load vertex and fragment shaders into opengl
	LoadShaders();

	// load the texture
	LoadTexture();

	// create buffer and fill it with the points of the triangle
	LoadTriangle();

	gCamera.setPosition(glm::vec3(0, 0, 4));
	gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

	double lastTime = glfwGetTime();
	// run while the window is open
	while (!glfwWindowShouldClose(gWindow3)) {
		// process pending events
		glfwPollEvents();

		double thisTime = glfwGetTime();
		Update3(float(thisTime - lastTime));
		lastTime = thisTime;

		// draw one frame
		Render();

		// check for errors
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
			std::cerr << "OpenGL Error " << error << std::endl;

		//exit program if escape key is pressed
		if (glfwGetKey(gWindow3, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(gWindow3, GL_TRUE);
	}

	// clean up and exit
	glfwTerminate();
}