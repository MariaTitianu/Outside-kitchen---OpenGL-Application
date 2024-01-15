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
#include "Keyframe.hpp"

#include "SkyBox.hpp"
#include <iostream>
#include <chrono>

using namespace std::chrono;

#define lightNum 4

const unsigned int SHADOW_WIDTH = 32768;
const unsigned int SHADOW_HEIGHT = 32768;

GLuint shadowMapFBO;
GLuint depthMapTexture;
bool showDepthMap;

gps::Model3D oras;
gps::Model3D geamuri;
gps::Model3D lampite;
gps::Model3D sticla,sticla1, sticla2;
gps::Model3D doza, doza1, doza2;
gps::Model3D screenQuad;
gps::Model3D chifla,top_chifla, tomato, patty, cheese, salad;

enum CameraMode {
	FREE,
	PRESENTATION
};
CameraMode cameraStatus = FREE;

enum AnimationStatus {
	STOPPED,
	INITIALISING,
	RUNNING
};
AnimationStatus animationStatus = STOPPED;
milliseconds animationInitialTime;
int animationKeyframeIndex;
std::vector<gps::Keyframe> animationKeyframeVector{
	gps::Keyframe(glm::vec3(11.1969f, 1.12427f, -1.32859f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(11.999f, 0.682508f, -1.19389f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 2000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(11.999f, 0.682508f, -1.19389f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 3000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(11.999f, 1.0f, -1.19389f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 3500ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(11.999f, 0.682508f, -1.19389f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 4000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(11.999f, 0.682508f, -1.19389f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 5000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(12.7363f, 1.50028f, -0.842085f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 6000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(14.0284f, 1.50028f, -0.276667f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 7000ms, gps::Keyframe::ANGULAR)
};
float halfHeight;
bool firstIngredient = true;
gps::Model3D chosenIngredient;

gps::Keyframe chiflaInitialKeyframe(glm::vec3(10.2196f, 0.56819f, -1.32859f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0ms, gps::Keyframe::ANGULAR);
float chiflaHalfHeight = 0.0266f;

gps::Keyframe targetKeyframe(glm::vec3(14.0284f, 0.825997f, -0.276667f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 8000ms, gps::Keyframe::ANGULAR);

std::vector<std::pair<gps::Keyframe, gps::Model3D>> objectPersistanceVector;

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
milliseconds animation_initial_time_aliment, animationLength_aliment = 1000ms;
milliseconds last_time_aliment;
milliseconds current_time_aliment;

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
	,gps::Keyframe(glm::vec3(0.358665f, 1.482273f, 3.800300f), glm::vec3(1.291042f, 1.491704f, 3.438936f), glm::vec3(-0.000000f, 1.000000f, 0.000000f), 31000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(0.325578f, 1.409597f, 0.221673f), glm::vec3(1.325069f, 1.429595f, 0.246563f), glm::vec3(-0.019992f, 0.999800f, -0.000498f), 33000ms, gps::Keyframe::ANGULAR)
};
int keyframe_index;
bool replace_start_frame = false;

bool debug_mode = false;

int glWindowWidth = 1600;
int glWindowHeight = 900;
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
glm::vec3 sunlightDir = glm::vec3(95.935310f, 237.573547f, 189.779480f);
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

bool m_pressed_last = false;
bool e_pressed_last = false, h_pressed_last = false;
bool l_pressed_last = false, q_pressed_last = false;
bool k_pressed_last = false, f_pressed_last = false;
bool geamurilights_on = false;
bool luminitelights_on = false;

//flat
int flat;
GLuint flatLoc;

//fog
int fog;
GLuint fogLoc;

//transparency
GLfloat alpha;
GLuint alphaLoc;

//reflexie
GLuint viewLocReflection;
GLuint projectionLocReflection;
GLuint normalMatrixLocReflection;
GLuint alphaLocReflection;
GLuint fogLocReflection;
GLuint modelLocReflection;

gps::Camera myCamera(glm::vec3(41.061756f, 26.729397f, 24.829081f), glm::vec3(40.436047f, 26.138233f, 24.320145f), glm::vec3(-0.458614f, 0.806552f, -0.373024f), true);

bool pressedKeys[1024];
float angleY = 0.0f;

gps::Shader myCustomShader;
gps::Shader reflectionShader;
gps::Shader skyboxShader;
gps::Shader depthShader;
gps::Shader screenQuadShader;

gps::SkyBox skybox;

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
		//printf("Camera speed: %f, time difference: %lld\n", cameraSpeed, (current_time.count() - last_time.count()));
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
	
	if (pressedKeys[GLFW_KEY_9]) {
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
	
	//fog
	if (pressedKeys[GLFW_KEY_F] != f_pressed_last) {
		if (pressedKeys[GLFW_KEY_F]) {
			if (fog == 0) {
				fog = 1;
			}
			else {
				fog = 0;
			}
		}
		f_pressed_last = pressedKeys[GLFW_KEY_F];
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
	if (pressedKeys[GLFW_KEY_F1]) { //solid
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (pressedKeys[GLFW_KEY_F2]) { //wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (pressedKeys[GLFW_KEY_F3]) { //point
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	if (pressedKeys[GLFW_KEY_F4]) { //flat
		flat = 2;
	}
	if (pressedKeys[GLFW_KEY_F5]) { //normals flat
		flat = 1;
	}
	if (pressedKeys[GLFW_KEY_F6]) { //smooth
		flat = 0;
	}

	if (pressedKeys[GLFW_KEY_M] != m_pressed_last) {
		if (pressedKeys[GLFW_KEY_M]) {
			showDepthMap = !showDepthMap;
		}
		m_pressed_last = pressedKeys[GLFW_KEY_M];
	}
	if (pressedKeys[GLFW_KEY_1] && animationStatus == STOPPED) {
		animationKeyframeIndex = 0;
		animationStatus = INITIALISING;
		animationKeyframeVector.insert(animationKeyframeVector.begin(), chiflaInitialKeyframe);
		animationKeyframeVector.push_back(targetKeyframe);
		chosenIngredient = chifla;
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

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);
	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
		0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void drawSkybox() {
	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 2000.0f);
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
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller valu e as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initObjects() {
	oras.LoadModel("objects/scena_completa.obj");
	geamuri.LoadModel("objects/geamuri.obj");
	lampite.LoadModel("objects/lampite.obj");
	sticla.LoadModel("objects/sticla.obj");
	sticla1.LoadModel("objects/sticla1.obj");
	sticla2.LoadModel("objects/sticla2.obj");
	doza.LoadModel("objects/doza.obj");
	doza1.LoadModel("objects/doza1.obj");
	doza2.LoadModel("objects/doza_indoita.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");
	chifla.LoadModel("objects/bottom_bun.obj");
	cheese.LoadModel("objects/cheese.obj");
	patty.LoadModel("objects/patty.obj");
	salad.LoadModel("objects/salad.obj");
	tomato.LoadModel("objects/tomato.obj");
	top_chifla.LoadModel("objects/top_bun.obj");
	skybox.Load(faces);
}


void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	depthShader.loadShader("shaders/depthShader.vert", "shaders/depthShaders.frag");
	depthShader.useShaderProgram();
	reflectionShader.loadShader("shaders/reflectionShader.vert", "shaders/reflectionShader.frag");
	reflectionShader.useShaderProgram();

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
	
	//flat
	flat = 0;
	flatLoc = glGetUniformLocation(myCustomShader.shaderProgram, "flatness");
	glUniform1i(flatLoc, flat);

	//fog
	fog = 0;
	fogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "enableFog");
	glUniform1i(fogLoc, fog);

	//alpha
	alpha = 1.0f;
	alphaLoc = glGetUniformLocation(myCustomShader.shaderProgram, "alpha");
	glUniform1f(alphaLoc, alpha);

	//reflection
	reflectionShader.useShaderProgram();
	modelLocReflection = glGetUniformLocation(reflectionShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLocReflection, 1, GL_FALSE, glm::value_ptr(model));
	viewLocReflection = glGetUniformLocation(reflectionShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLocReflection, 1, GL_FALSE, glm::value_ptr(view));
	projectionLocReflection = glGetUniformLocation(reflectionShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLocReflection, 1, GL_FALSE, glm::value_ptr(projection));
	normalMatrixLocReflection = glGetUniformLocation(reflectionShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLocReflection, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	alphaLocReflection = glGetUniformLocation(reflectionShader.shaderProgram, "alpha");
	glUniform1f(alphaLocReflection, alpha);
	fogLocReflection = glGetUniformLocation(reflectionShader.shaderProgram, "enableFog");
	glUniform1i(fogLocReflection, fog);
	//skybox
	skyboxShader.useShaderProgram();
	skyboxBoost = skyboxBoostSun;
	skyboxBoostLoc = glGetUniformLocation(skyboxShader.shaderProgram, "boost");
	glUniform1f(skyboxBoostLoc, skyboxBoost);
}
void drawReflectiveObjects(gps::Shader shader, bool depthPass) {
	shader.useShaderProgram();
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLocReflection, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLocReflection, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		alpha = 1.0f;
		glUniform1f(alphaLocReflection, alpha);
		glUniform1i(fogLocReflection, fog);
	}
	doza.Draw(shader);
	doza1.Draw(shader);
	doza2.Draw(shader);
}

void drawTransparentObjects(gps::Shader shader, bool depthPass) {
	shader.useShaderProgram();

	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		alpha = 0.7f;
		glUniform1f(alphaLoc, alpha);
	}
	sticla.Draw(shader);
	sticla1.Draw(shader);
	sticla2.Draw(shader);
}
void drawObjects(gps::Shader shader, bool depthPass) {
	shader.useShaderProgram();
	if (!depthPass) {
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniform1i(flatLoc, flat);
		glUniform1f(sunlightStrengthLoc, sunlightStrength);
		glUniform1i(fogLoc, fog);
		alpha = 1.0f;
		glUniform1f(alphaLoc, alpha);
	}

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

	glCheckError();

	chifla.Draw(shader);

	glCheckError();

	//if (!depthPass) {
	//	for (std::pair<gps::Keyframe, gps::Model3D> object : objectPersistanceVector) {
	//		glCheckError();
	//		model = glm::translate(glm::mat4(1.0f), object.first.getPostionVec());
	//		glCheckError();
	//		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//		glCheckError();
	//		chifla.Draw(shader);
	//		glCheckError();
	//		model = glm::mat4(1.0f);
	//		glCheckError();
	//		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//		glCheckError();
	//	}
	//}
}

glm::mat4 rotateAroundPoint(glm::mat4 initialMatrix, glm::vec3 point, float angle, glm::vec3 rotationAxis) {
	glm::mat4 matrix = glm::translate(initialMatrix, point);
	matrix = glm::rotate(matrix, angle, rotationAxis);
	matrix = glm::translate(matrix, -point);
	return matrix;
}

glm::mat4 rotateAroundPoint(glm::vec3 point, float angle, glm::vec3 rotationAxis) {
	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), point);
	matrix = glm::rotate(matrix, angle, rotationAxis);
	matrix = glm::translate(matrix, -point);
	return matrix;
}



glm::mat4 computeLightSpaceTrMatrix() {
	glCheckError();
	glm::mat4 lightView = glm::lookAt(glm::vec3(sunlightRotation * glm::vec4(sunlightDir, 1.0f)), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glCheckError();
	glm::mat4 lightProjection = glm::ortho(-2000.0f, 2000.0f, -2000.0f, 2000.0f, near_plane, 2000.0f);
	glCheckError();
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
void computeAnimation(gps::Shader shader) {
	shader.useShaderProgram();

	if (animationStatus == INITIALISING) {
		animationInitialTime = current_time;
		animationStatus = RUNNING;
	}

	if (animationKeyframeIndex == animationKeyframeVector.size() - 2 && animationKeyframeVector.at(animationKeyframeIndex + 1).getTimestamp() <= current_time - animationInitialTime) {
		animationStatus = STOPPED;
		objectPersistanceVector.push_back(std::pair<gps::Keyframe, gps::Model3D>(animationKeyframeVector.at(animationKeyframeVector.size() - 1), chosenIngredient));
		animationKeyframeVector.erase(animationKeyframeVector.begin());
		animationKeyframeVector.erase(animationKeyframeVector.end() - 1);
	}
	else {
		if (animationKeyframeVector.at(animationKeyframeIndex + 1).getTimestamp() <= current_time - animationInitialTime) {
			animationKeyframeIndex++;
		}
		gps::Keyframe interpolatedKeyframe = animationKeyframeVector.at(animationKeyframeIndex).getInterpolatedKeyframe(animationKeyframeVector.at(animationKeyframeIndex + 1), current_time - animationInitialTime);
		
		view = myCamera.getViewMatrix();
		model = glm::translate(glm::mat4(1.0f), interpolatedKeyframe.getPostionVec());
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniform1i(flatLoc, flat);
		glUniform1f(sunlightStrengthLoc, sunlightStrength);
		glUniform1i(fogLoc, fog);
		alpha = 1.0f;
		glUniform1f(alphaLoc, alpha);
		chosenIngredient.Draw(shader);
		model = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	}

	//	if (current_time_aliment - last_time_aliment > animationLength_aliment) {
	//		view = myCamera.getViewMatrix();
	//		model = glm::translate(model, counter_position4chifla + glm::vec3(0.0f, -0.257394f, 0.95001f));
	//		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	//		glUniform1i(flatLoc, flat);
	//		glUniform1f(sunlightStrengthLoc, sunlightStrength);
	//		glUniform1i(fogLoc, fog);
	//		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//		alpha = 1.0f;
	//		glUniform1f(alphaLoc, alpha);
	//		chifla.Draw(shader);
	//	}
	//	else if (current_time_aliment - last_time_aliment > animationLength_aliment) {
	//		model = glm::mat4(1.0f);
	//		last_time = current_time;
	//		current_time = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	//		model = rotateAroundPoint(model, counter_position4chifla + glm::vec3(2.0f, 0.0f, 0.0f), glm::radians(1.25), glm::vec3(0.0f, 1.0f, 0.0f));
	//		model = glm::translate(model, -counter_position4chifla + glm::vec3(-2.0f, 0.0f, 0.0f));
	//		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	//		glUniform1i(flatLoc, flat);
	//		glUniform1f(sunlightStrengthLoc, sunlightStrength);
	//		glUniform1i(fogLoc, fog);
	//		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//		chifla.Draw(shader);
	//	}
	//	model = glm::mat4(1.0f);
	//	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//}

}
void renderScene() {
	depthShader.useShaderProgram();
	glCheckError();
	glUniformMatrix4fv(glGetUniformLocation(depthShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
	glCheckError();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glCheckError();
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthShader, true);
	drawReflectiveObjects(depthShader, true);
	drawTransparentObjects(depthShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCheckError();
	// render depth map on screen - toggled with the M key
	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);
		glClear(GL_COLOR_BUFFER_BIT);
		screenQuadShader.useShaderProgram();
		glCheckError();
		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);
		glCheckError();
		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {
		// final scene rendering pass (with shadows)
		glViewport(0, 0, retina_width, retina_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCheckError();
		myCustomShader.useShaderProgram();

		sunlightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(sunlightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(sunlightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * sunlightRotation)) * sunlightDir));
		glCheckError();
		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);
		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));
		myCustomShader.useShaderProgram();
		glCheckError();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		model = glm::mat4(1.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glCheckError();
		if(animationStatus != STOPPED){
			computeAnimation(myCustomShader);
		}
		
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
		sunlightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(sunlightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(sunlightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * sunlightRotation)) * sunlightDir));
		drawLights(myCustomShader);
		drawReflectiveObjects(reflectionShader, false);
		drawSkybox();
		drawObjects(myCustomShader, false);
		drawTransparentObjects(myCustomShader, false);
	}
}




void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char * argv[]) {
	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}
	
	initOpenGLState();
	glCheckError();
	initObjects();
	glCheckError();
	initShaders();
	glCheckError();
	initUniforms();
	current_time = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	current_time_aliment = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	glCheckError();

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
		glfwSwapBuffers(glWindow);
		glCheckError();
	}

	cleanup();

	return 0;
}
