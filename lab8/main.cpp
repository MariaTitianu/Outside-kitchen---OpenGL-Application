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

#define lightNum 4

#include <GLFW/glfw3.h>

#include "../../glm/glm.hpp"
#include "../../glm/gtc/matrix_transform.hpp"
#include "../../glm/gtc/matrix_inverse.hpp"
#include "../../glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "Keyframe.hpp"

#include "SkyBox.hpp"
#include <iostream>
#include <chrono>

using namespace std::chrono;

gps::Model3D oras;
gps::Model3D geamuri;
gps::Model3D lampite;

enum CameraMode {
	FREE,
	PRESENTATION
};
CameraMode cameraStatus = FREE;

float last_xpos, last_ypos;
const float PITCH_MULT = 0.0025, YAW_MULT = 0.0025;
bool firstRun = true;
GLfloat near_plane = 0.1f, far_plane = 255.0f;

float cameraSpeed = 1.0f;
float cameraSpeedMax = 1.0f;
float cameraAcceleration = 0.0002f;
//animation
milliseconds last_time;
milliseconds current_time;
milliseconds animation_initial_time, animationLengthFromPosition= 1000ms;

std::vector<gps::Keyframe> keyframe_vector{
	gps::Keyframe(glm::vec3(0.325578f, 1.409597f, 0.221673f), glm::vec3(1.325069f, 1.429595f, 0.246563f), glm::vec3(-0.019992f, 0.999800f, -0.000498f), 0ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(9.447121f, 1.489379f, 1.582937f), glm::vec3(10.441292f, 1.464381f, 1.478059f), glm::vec3(0.024860f, 0.999688f, -0.002623f), 3000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(11.430288f, 1.378221f, 1.084252f), glm::vec3(11.326725f, 1.140518f, 0.118450f), glm::vec3 ( - 0.025344f, 0.971338f, -0.236348f), 5000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(10.833172f, 1.160686f, 0.264682f), glm::vec3(10.769896f, 0.841377f, -0.680854f), glm::vec3(-0.021321f, 0.947651f, -0.318596f), 8000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3( 12.147247f, 1.160686f, 0.189668f), glm::vec3(12.083710f, 0.853248f, -0.759777f), glm::vec3(-0.020528f, 0.951568f, -0.306753f), 12000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(11.465453f, 1.278623f, 0.336850f), glm::vec3(12.429255f, 1.111905f, 0.128789f), glm::vec3(0.162964f, 0.986005f, -0.035180f), 14000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(10.717615f, 1.021175f, 2.176663f), glm::vec3(11.714358f, 1.098597f, 2.154125f), glm::vec3(-0.077402f, 0.996998f, 0.001750f), 16000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(14.996320f, 1.366862f, 2.031034f), glm::vec3(15.940397f, 1.341864f, 1.702259f), glm::vec3(0.023607f, 0.999688f, -0.008221f), 18000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(14.398093f, 1.412532f, 1.342983f), glm::vec3(13.407144f, 1.460014f, 1.468537f), glm::vec3(0.047105f, 0.998872f, -0.005968f), 21000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(2.747718f, 1.597317f, -1.137050f), glm::vec3( 1.771091f, 1.539848f, -1.344165f), glm::vec3(-0.056218f, 0.998347f, -0.011922f), 23000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(-1.410410f, 1.367754f, -1.990653f), glm::vec3(-2.399813f, 1.345255f, -2.134094f), glm::vec3(-0.022266f, 0.999747f, -0.003228f), 25000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(-7.185678f, 1.670901f, 0.191114f), glm::vec3(-8.118288f, 1.571068f, 0.537916f), glm::vec3(-0.093573f, 0.995004f, 0.034796f), 27000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(-5.620029f, 1.818617f, 1.830073f), glm::vec3(-4.661648f, 1.681549f, 2.080506f), glm::vec3 (0.132615f, 0.990562f, 0.034653f), 29000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(0.325578f, 1.409597f, 0.221673f), glm::vec3(1.325069f, 1.429595f, 0.246563f), glm::vec3(-0.019992f, 0.999800f, -0.000498f), 31000ms, gps::Keyframe::ANGULAR)
};
int keyframe_index;
bool replace_start_frame = false;

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

GLuint sunlightStrengthLoc;
GLfloat sunlightStrength;
GLfloat sunlightStrengthMoon = 0.02f;
GLfloat sunlightStrengthSun = 1.0f;

GLfloat sunlightAngle;
glm::mat4 sunlightRotation;
glm::vec3 sunlightDir = glm::vec3(-12.5f, 50.0f, -50.0f);
GLuint sunlightDirLoc;
glm::vec3 sunlightColor;
GLuint sunlightColorLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

GLfloat ambientStrengthDefault = 0.2f;
GLfloat ambientStrengthSunlight = 1.0f;
GLfloat ambientStrengthLights = 50.0f;
GLuint ambientStrengthLoc, ambientColorLoc;
glm::vec3 ambientColorDisabled = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 ambientColorWindow = glm::vec3(1.0f, 0.97f, 0.55f);

GLuint lightEnabledLoc;
GLuint lightPositionLoc;
GLuint lightDirectionLoc;
GLuint lightFOVLoc;
GLuint lightStrengthLoc;

bool e_pressed_last = false;
bool l_pressed_last = false;
bool k_pressed_last = false;
bool geamurilights_on = false;
bool luminitelights_on = false, q_pressed_last = false;

gps::Camera myCamera(glm::vec3(41.061756f, 26.729397f, 24.829081f), glm::vec3(40.436047f, 26.138233f, 24.320145f), glm::vec3(-0.458614f, 0.806552f, -0.373024f), true);

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
GLfloat skyboxBoostMoon = 0.005f;
GLfloat skyboxBoostSun = 1.0f;

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
			keyframe_index = 0;
			if (replace_start_frame) {
				keyframe_vector.at(0) = gps::Keyframe(myCamera.getPosition(), myCamera.getTarget(), myCamera.getUp(), 0ms);
				keyframe_vector.at(keyframe_vector.size() - 1) = gps::Keyframe(myCamera.getPosition(), myCamera.getTarget(), myCamera.getUp(), keyframe_vector.at(keyframe_vector.size() - 1).getTimestamp(), gps::Keyframe::ANGULAR);
				keyframe_vector.at(keyframe_vector.size() - 1).incrementTimestamp(animationLengthFromPosition);
			}
			else {
				keyframe_vector.push_back(gps::Keyframe(myCamera.getPosition(), myCamera.getTarget(), myCamera.getUp(), keyframe_vector.at(keyframe_vector.size() - 1).getTimestamp(), gps::Keyframe::ANGULAR));
				keyframe_vector.at(keyframe_vector.size() - 1).incrementTimestamp(animationLengthFromPosition);
				keyframe_vector.insert(keyframe_vector.begin(), gps::Keyframe(myCamera.getPosition(), myCamera.getTarget(), myCamera.getUp(), 0ms, gps::Keyframe::ANGULAR));
				for (int i = 1; i < keyframe_vector.size(); i++) {
					keyframe_vector.at(i).incrementTimestamp(animationLengthFromPosition);
				}
				replace_start_frame = true;
			}
			cameraStatus = PRESENTATION;
		}
	}
	if (pressedKeys[GLFW_KEY_1]) {
		if (cameraStatus != FREE) {
			cameraStatus = FREE;
		}
	}
	//geamurilights
	if (pressedKeys[GLFW_KEY_L] != l_pressed_last) {
		if (pressedKeys[GLFW_KEY_L]) {
			luminitelights_on = !luminitelights_on;
		}
		l_pressed_last = pressedKeys[GLFW_KEY_L];
	}
	if (pressedKeys[GLFW_KEY_K] != k_pressed_last) {
		if (pressedKeys[GLFW_KEY_K]) {
			geamurilights_on = !geamurilights_on;
		}
		k_pressed_last = pressedKeys[GLFW_KEY_K];
	}
	//sunlight rotation
	if (pressedKeys[GLFW_KEY_R]) {
		sunlightAngle += 0.1f;
	}
	//sunlight or moonlight
	if (pressedKeys[GLFW_KEY_E] != e_pressed_last) {
		if (pressedKeys[GLFW_KEY_E]) {
			if (sunlightStrength == sunlightStrengthMoon) {
				sunlightStrength = sunlightStrengthSun;
				skyboxBoost = skyboxBoostSun;
			}
			else {
				sunlightStrength = sunlightStrengthMoon;
				skyboxBoost = skyboxBoostMoon;
			}
		}
		e_pressed_last = pressedKeys[GLFW_KEY_E];
	}

	//display current camera position
	if (pressedKeys[GLFW_KEY_Q] != q_pressed_last) {
		if (pressedKeys[GLFW_KEY_Q]) {
			fprintf(stdout, "Position: %ff, %ff, %ff\nTarget %ff, %ff, %ff\nUp %ff, %ff, %ff\n",
				myCamera.getPosition().x, myCamera.getPosition().y, myCamera.getPosition().z,
				myCamera.getTarget().x, myCamera.getTarget().y, myCamera.getTarget().z,
				myCamera.getUp().x, myCamera.getUp().y, myCamera.getUp().z);
		}
		q_pressed_last = pressedKeys[GLFW_KEY_Q];
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
	sunlightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(sunlightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	sunlightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "sunlightDir");
	glUniform3fv(sunlightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * sunlightRotation)) * sunlightDir));

	//set light color
	sunlightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	sunlightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "sunlightColor");
	glUniform3fv(sunlightColorLoc, 1, glm::value_ptr(sunlightColor));

	//set default ambient strength
	ambientStrengthLoc = glGetUniformLocation(myCustomShader.shaderProgram, "ambientStrengthTexture");
	glUniform1f(ambientStrengthLoc, ambientStrengthDefault);

	ambientColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "ambientColor");
	glUniform3fv(ambientColorLoc, 1, glm::value_ptr(ambientColorDisabled));
	
	//set sunlight strength
	sunlightStrength = sunlightStrengthSun;
	sunlightStrengthLoc = glGetUniformLocation(myCustomShader.shaderProgram, "sunlightStrength");
	glUniform1f(sunlightStrengthLoc, sunlightStrength);
	//skybox
	skyboxShader.useShaderProgram();
	skyboxBoost = skyboxBoostSun;
	skyboxBoostLoc = glGetUniformLocation(skyboxShader.shaderProgram, "boost");
	glUniform1f(skyboxBoostLoc, skyboxBoost);
}

void drawObjects(gps::Shader shader, bool depthPass) {
	shader.useShaderProgram();

	glUniform1f(sunlightStrengthLoc, sunlightStrength);

	oras.Draw(shader);

	if (geamurilights_on && !depthPass) {
		glUniform3fv(ambientColorLoc,1,  glm::value_ptr(ambientColorWindow));
		glUniform1f(ambientStrengthLoc, ambientStrengthLights);
		geamuri.Draw(shader);
		glUniform3fv(ambientColorLoc, 1, glm::value_ptr(ambientColorDisabled));
		glUniform1f(ambientStrengthLoc, ambientStrengthDefault);
	}
	else {
		geamuri.Draw(shader);
	}

	if (luminitelights_on && !depthPass) {
		glUniform3fv(ambientColorLoc, 1, glm::value_ptr(ambientColorWindow));
		glUniform1f(ambientStrengthLoc, ambientStrengthLights);
		lampite.Draw(shader);
		glUniform3fv(ambientColorLoc, 1, glm::value_ptr(ambientColorDisabled));
		glUniform1f(ambientStrengthLoc, ambientStrengthDefault);
	}
	else {
		lampite.Draw(shader);
	}
}
glm::mat4 computeLightSpaceTrMatrix() {
	glm::mat4 lightView = glm::lookAt(glm::vec3(sunlightRotation * glm::vec4(sunlightDir, 1.0f)), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = glm::ortho(-700.0f, 700.0f, -700.0f, 700.0f, near_plane, far_plane);
	return lightProjection * lightView;
}
void drawLights(gps::Shader shader) {
	shader.useShaderProgram();

	int lightEnabled[lightNum] = { luminitelights_on, luminitelights_on, luminitelights_on, luminitelights_on };
	lightEnabledLoc = glGetUniformLocation(shader.shaderProgram, "lightEnabled");
	glUniform1iv(lightEnabledLoc, lightNum, lightEnabled);

	model = glm::mat4(1.0f);
	glm::vec3 lightPosition[lightNum]{
		glm::vec3(model * glm::vec4(glm::vec3(3.941f,  1.2f,-2.1f), 1.0f)),
		glm::vec3(model * glm::vec4(glm::vec3(12.669f, 1.2f,-2.4224f), 1.0f)),
		glm::vec3(model * glm::vec4(glm::vec3(14.156f, 1.2f, 3.6445f), 1.0f)),
		glm::vec3(model * glm::vec4(glm::vec3(24.725f, 1.2f, 4.5494f), 1.0f))
	};
	lightPositionLoc = glGetUniformLocation(shader.shaderProgram, "lightPosition");
	glUniform3fv(lightPositionLoc, lightNum, glm::value_ptr(lightPosition[0]));

	glm::vec3 lightDirection[lightNum] = { glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),glm::vec3(0.0f, 1.0f, 0.0f),glm::vec3(0.0f, 1.0f, 0.0f) };
	lightDirectionLoc = glGetUniformLocation(shader.shaderProgram, "lightDirection");
	glUniform3fv(lightDirectionLoc, lightNum, glm::value_ptr(lightDirection[0]));

	glm::vec3 lightColor[lightNum]{
		ambientColorWindow
		,ambientColorWindow
		,ambientColorWindow
		,ambientColorWindow
	};
	lightColorLoc = glGetUniformLocation(shader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, lightNum, glm::value_ptr(lightColor[0]));

	float lightFOV[lightNum]{
		150.0f
		,150.0f
		,150.0f
		,150.0f
	};
	lightFOVLoc = glGetUniformLocation(shader.shaderProgram, "lightFOV");
	glUniform1fv(lightFOVLoc, lightNum, lightFOV);

	float lightStrength[lightNum]{
		1.0f
		,1.0f
		,1.0f
		,1.0f
	};
	lightStrengthLoc = glGetUniformLocation(shader.shaderProgram, "lightStrength");
	glUniform1fv(lightStrengthLoc, lightNum, lightStrength);
}

void renderScene() {

	myCustomShader.useShaderProgram();
	glCheckError();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCheckError();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glCheckError();
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glCheckError();
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glCheckError();
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	//calculateAnimations();
	switch (cameraStatus) {
	case PRESENTATION:
		if (keyframe_index == keyframe_vector.size() - 2 && keyframe_vector.at(keyframe_index + 1).getTimestamp() <= current_time - animation_initial_time) {
			cameraStatus = FREE;
		}
		else {
			if (keyframe_vector.at(keyframe_index + 1).getTimestamp() <= current_time - animation_initial_time) {
				keyframe_index++;
			}
			gps::Keyframe interpolatedKeyframe = keyframe_vector.at(keyframe_index).getInterpolatedKeyframe(keyframe_vector.at(keyframe_index + 1), current_time - animation_initial_time);
			myCamera = gps::Camera(interpolatedKeyframe.getPostionVec(), interpolatedKeyframe.getTargetVec(), interpolatedKeyframe.getUpVec(), true);
		}
		break;
	case FREE: break;
	}
//lumina
	glCheckError();
	sunlightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(sunlightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniform3fv(sunlightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * sunlightRotation)) * sunlightDir));
	glCheckError();
	drawLights(myCustomShader);
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
	glCheckError();	
	initOpenGLState();
	glCheckError();
	initObjects();
	glCheckError();
	initShaders();
	glCheckError();
	initUniforms();

	while (!glfwWindowShouldClose(glWindow)) {
		last_time = current_time;
		current_time = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		if (debug_mode) {
			fprintf(stdout, "current time %lld\n", current_time - animation_initial_time);
		}
		processMovement();
		renderScene();
		glCheckError();
		glfwPollEvents();
		glCheckError();
		glfwSwapBuffers(glWindow);
		glCheckError();
	}

	cleanup();

	return 0;
}
