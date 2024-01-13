//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
	#define GLFW_INCLUDE_GLCOREARB
	#define GL_SILENCE_DEPRECATION
#else
	#define GLEW_STATIC
	#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "../../glm/glm.hpp"
#include "../../glm/gtc/matrix_transform.hpp"
#include "../../glm/gtc/matrix_inverse.hpp"
#include "../../glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"

#include "SkyBox.hpp"
#include <iostream>
#include <chrono>

using namespace std::chrono;

gps::Model3D oras;
gps::Model3D geamuri;
gps::Model3D lampite;

enum CameraMode {
	FREE,
	PRESENTATION,
	FOLLOW_CAR,
	INSIDE_CAR
};
CameraMode cameraStatus = FREE;

float last_xpos, last_ypos;
const float PITCH_MULT = 0.0025, YAW_MULT = 0.0025;
bool firstRun = true;
GLfloat near_plane = 0.1f, far_plane = 255.0f;

float cameraSpeed = 1.0f;
float cameraSpeedMax = 1.0f;
float cameraAcceleration = 0.0002f;

milliseconds last_time;
milliseconds current_time;
milliseconds animation_initial_time;
bool debug_mode = false;

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;

//lumina

GLfloat sunlightStrength;
GLfloat sunlightStrengthMoon = 0.02f;
GLfloat sunlightStrengthSun = 0.5f;
glm::mat4 sunlightRotation;
glm::vec3 sunlightDir = glm::vec3(-12.5f, 50.0f, -50.0f);
GLuint sunlightDirLoc;
glm::vec3 sunlightColor;
GLuint sunlightColorLoc;
GLuint ambientStrengthLoc;
glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

GLuint sunlightStrengthLoc;

GLfloat ambientStrengthDefault = 0.2f;
GLfloat ambientStrengthSunlight = 1.0f;
GLfloat ambientStrengthLights = 50.0f;
GLfloat sunlightAngle;

gps::Camera myCamera(
				glm::vec3(0.0f, 0.0f, 2.5f), 
				glm::vec3(0.0f, 0.0f, -10.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));

bool pressedKeys[1024];
float angleY = 0.0f;

gps::Shader myCustomShader;

gps::SkyBox skybox;
gps::Shader skyboxShader;

std::vector<const GLchar*> faces{
	"skybox/px.jpg"
	,"skybox/nx.jpg"
	,"skybox/py.jpg"
	,"skybox/ny.jpg"
	,"skybox/pz.jpg"
	,"skybox/nz.jpg"
};

GLuint skyboxBoostLoc;

GLfloat skyboxBoost;
GLfloat skyboxBoostMoon = 0.1f;
GLfloat skyboxBoostSun = 2.0f;

GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (!firstRun && cameraStatus == FREE) {
		myCamera.rotate(-PITCH_MULT * (ypos - last_ypos), -YAW_MULT * (xpos - last_xpos));
	}
	else {
		firstRun = false;
	}

	myCustomShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	last_xpos = xpos;
	last_ypos = ypos;
}

void processMovement()
{
	//camera movement
	if (cameraStatus == FREE && 
		(pressedKeys[GLFW_KEY_W] || pressedKeys[GLFW_KEY_S] || pressedKeys[GLFW_KEY_A] || pressedKeys[GLFW_KEY_D]) || pressedKeys[GLFW_KEY_LEFT_SHIFT] || pressedKeys[GLFW_KEY_LEFT_CONTROL]) {
		cameraSpeed = cameraSpeed > cameraSpeedMax ? cameraSpeedMax : cameraSpeed + cameraAcceleration * (current_time.count() - last_time.count());
		printf("Camera speed: %f, time difference: %lld\n", cameraSpeed, (current_time.count() - last_time.count()));
		if (pressedKeys[GLFW_KEY_W] && cameraStatus == FREE) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_S] && cameraStatus == FREE) {
			myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_A] && cameraStatus == FREE) {
			myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_D] && cameraStatus == FREE) {
			myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_LEFT_SHIFT] && cameraStatus == FREE) {
			myCamera.move(gps::MOVE_UP, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_LEFT_CONTROL] && cameraStatus == FREE) {
			myCamera.move(gps::MOVE_DOWN, cameraSpeed);
		}
	}
	else {
		cameraSpeed = 0.0f;
	}
	if (pressedKeys[GLFW_KEY_0]) {
		if (cameraStatus != PRESENTATION) {
			animation_initial_time = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		}
	}
	if (pressedKeys[GLFW_KEY_9]) {
		if (cameraStatus != FREE) {
			cameraStatus = FREE;
		}
	}
}
void drawSkybox() {
	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));
	glUniform1f(skyboxBoostLoc, skyboxBoost);
	skybox.Draw(skyboxShader, view, projection);
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	//for sRBG framebuffer
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	//for antialising
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	oras.LoadModel("objects/oras/scena_completa.obj");
	geamuri.LoadModel("objects/oras/geamuri.obj");
	lampite.LoadModel("objects/oras/lampite.obj");

	skybox.Load(faces);
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();

	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//set default ambient strength
	ambientStrengthLoc = glGetUniformLocation(myCustomShader.shaderProgram, "ambientStrengthTexture");
	glUniform1f(ambientStrengthLoc, ambientStrengthDefault);

	//set sunlight strength
	sunlightStrength = sunlightStrengthMoon;
	sunlightStrengthLoc = glGetUniformLocation(myCustomShader.shaderProgram, "sunlightStrength");
	glUniform1f(sunlightStrengthLoc, sunlightStrength);
	//skybox
	skyboxShader.useShaderProgram();
	skyboxBoost = skyboxBoostSun;
	skyboxBoostLoc = glGetUniformLocation(skyboxShader.shaderProgram, "boost");
	glUniform1f(skyboxBoostLoc, skyboxBoost);
}

void drawObjects(gps::Shader shader, bool depthPass) {
	oras.Draw(shader);
	geamuri.Draw(shader);
	lampite.Draw(shader);
}
glm::mat4 computeLightSpaceTrMatrix() {
	glm::mat4 lightView = glm::lookAt(glm::vec3(sunlightRotation * glm::vec4(sunlightDir, 1.0f)), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = glm::ortho(-700.0f, 700.0f, -700.0f, 700.0f, near_plane, far_plane);
	return lightProjection * lightView;
}

void renderScene() {

	myCustomShader.useShaderProgram();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	model = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	drawSkybox();
	drawObjects(myCustomShader, false);
}

void cleanup() {

	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char * argv[]) {
	
	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}
	current_time = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	
	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();

	while (!glfwWindowShouldClose(glWindow)) {
		last_time = current_time;
		current_time = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		if (debug_mode) {
			fprintf(stdout, "current time %lld\n", current_time - animation_initial_time);
		}
		processMovement();
		renderScene();
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
		glCheckError();
	}

	cleanup();

	return 0;
}
